#include "warlords/ailfm.hpp"

#include <algorithm>
#include <cmath>

namespace w2 {

namespace {

// ADLIB.ADV file offsets (it loads at offset 0 of its segment)
constexpr size_t ADV_FREQ = 0x101;       // 192 words: 12 half-tones x 16 fine steps of F-number
constexpr size_t ADV_OCTAVE = 0x281;     // 96 bytes: the octave of each note from C-2
constexpr size_t ADV_HALFTONE = 0x2e1;   // 96 bytes: its half-tone
constexpr size_t ADV_RESET = 0x340;      // register values written to 1..0xF5 at start-up
constexpr size_t ADV_VELOCITY = 0x52b;   // 16 bytes: velocity / 8 -> sensitivity, 82..127
constexpr size_t ADV_PROGRAMS = 0x2203;  // the program each channel 1..8 starts on

// operator register offsets of voice v's modulator; the carrier is +3
const int OPREG[9] = {0, 1, 2, 8, 9, 10, 16, 17, 18};

int byteAt(const std::string& s, size_t i) { return i < s.size() ? (uint8_t)s[i] : 0; }
int u16(const std::string& s, size_t i) { return byteAt(s, i) + byteAt(s, i + 1) * 256; }
int s16(const std::string& s, size_t i) {
  int v = u16(s, i);
  return v >= 0x8000 ? v - 0x10000 : v;
}
int s8(int v) { return v >= 0x80 ? v - 0x100 : v; }

// hi8(a * b * 2), then one more unless it came to 0: the driver's product
int scale(int a, int b) {
  int v = (a * b * 2 / 256) % 256;
  return v == 0 ? 0 : v + 1;
}

inline int floorDiv(int a, int b) { return (int)std::floor((double)a / b); }

}  // namespace

AilFm::AilFm(const std::string& adv, const std::string& ad) {
  for (int i = 0; i < 192; i++) freq_[i] = s16(adv, ADV_FREQ + i * 2);
  for (int i = 0; i < 96; i++) {
    octave_[i] = byteAt(adv, ADV_OCTAVE + i);
    halftone_[i] = byteAt(adv, ADV_HALFTONE + i);
  }
  for (int r = 1; r <= 0xf5; r++) reset_[r] = byteAt(adv, ADV_RESET + r);
  for (int i = 0; i < 16; i++) velocity_[i] = byteAt(adv, ADV_VELOCITY + i);
  for (int ch = 1; ch <= 8; ch++) programs_[ch] = byteAt(adv, ADV_PROGRAMS + ch);
  // The Global Timbre Library: a directory of 6-byte entries { patch, bank,
  // offset:u32 } ended by bank 0xFF, and at each offset a timbre -- a u16
  // length then the bytes.
  for (size_t p = 0; p + 6 <= ad.size(); p += 6) {
    int patch = byteAt(ad, p), bank = byteAt(ad, p + 1);
    if (bank == 0xff) break;
    size_t off = (size_t)u16(ad, p + 2) + (size_t)u16(ad, p + 4) * 65536;
    int len = u16(ad, off);
    Timbre t;
    for (int i = 2; i < len; i++) t.push_back((uint8_t)byteAt(ad, off + i));
    lib_[bank * 128 + patch] = t;
  }
  for (int r = 1; r <= 0xf5; r++) chip_.write(r, reset_[r]);
  // channel state, as the driver's start-up leaves it on channels 1..9
  for (int ch = 1; ch <= 9; ch++) {
    Chan& c = chans_[ch];
    c.vol = 127;
    c.expr = 127;
    if (ch <= 8) program(ch, programs_[ch]);
  }
  for (int& o : owner_) o = -1;
}

void AilFm::program(int ch, int p) {
  auto it = lib_.find(chans_[ch].bank * 128 + p);
  chans_[ch].timbre = it == lib_.end() ? nullptr : &it->second;
}

// Write whatever the slot's flags say has changed (1807).
void AilFm::update(Slot& s) {
  int v = s.voice;
  if (v < 0) return;
  int f = s.flags;
  Chan& c = chans_[s.ch];
  int m = OPREG[v], k = OPREG[v] + 3;
  if (f >= 0x80) {
    int vib = c.mod >= 64 ? 0x40 : 0;
    chip_.write(0x20 + m, s.modMult + s.modAvek + (s.modAvek % 0x80 >= 0x40 ? 0 : vib));
    chip_.write(0x20 + k, s.carMult + s.carAvek + (s.carAvek % 0x80 >= 0x40 ? 0 : vib));
    f -= 0x80;
  }
  if (f >= 0x40) {
    int vol = scale(scale(c.vol, c.expr), s.vel);
    int ml = s.modLevel;
    if (s.scaleMod) ml = ml * vol / 127;
    int cl = s.carLevel * vol / 127;
    chip_.write(0x40 + m, 63 - ml % 64 + s.modKsl);
    chip_.write(0x40 + k, 63 - cl % 64 + s.carKsl);
    f -= 0x40;
  }
  if (f >= 0x20) {
    chip_.write(0x60 + m, s.modAD);
    chip_.write(0x60 + k, s.carAD);
    chip_.write(0x80 + m, s.modSR);
    chip_.write(0x80 + k, s.carSR);
    f -= 0x20;
  }
  if (f >= 0x10) {
    chip_.write(0xe0 + k, s.carWS);
    chip_.write(0xe0 + m, s.modWS);
    f -= 0x10;
  }
  if (f >= 0x08) {
    chip_.write(0xc0 + v, s.fbc % 16);
    f -= 0x08;
  }
  if (f % 2 == 1) {
    if (s.keyon == 0) {
      chip_.write(0xb0 + v, s.b0 - s.b0 % 64 + s.b0 % 32);   // key off, pitch kept
    } else {
      chip_.write(0xa0 + v, frequency(s, c) % 256);
      s.b0 = (fw_ / 256) % 4 + block_ * 4 + s.keyon;
      chip_.write(0xb0 + v, s.b0);
    }
    f -= 1;
  }
  s.flags = f;
}

// F-number and block for a slot's note (1b61): the note less two octaves,
// folded into the 96 the tables cover, in sixteenths of a half-tone.
int AilFm::frequency(const Slot& s, const Chan& c) {
  int n = s.note + s.transpose - 24;
  while (n < 0) n += 12;
  while (n > 95) n -= 12;
  int bend = floorDiv(c.bend, 32) * 12;   // +-12 half-tones over the range
  int x = floorDiv(n * 256 + bend + 8, 16);
  while (x < 0) x += 192;
  while (x > 0x5ff) x -= 192;
  int semi = x / 16;
  int fw = freq_[halftone_[semi] * 16 + x % 16];
  int block = octave_[semi] - 1;
  // the upper half-tones are stored an octave down, flagged negative
  if (fw < 0) block++;
  if (block < 0) {
    block++;
    fw = floorDiv(fw, 2);
  }
  if (fw < 0) fw += 0x10000;
  fw_ = fw;
  block_ = block;
  return fw;
}

// Give a slot a free voice, round-robin from the last one (1740); with none
// free, let the slots fight over them (1c69).
void AilFm::allocate(Slot& s) {
  int v = nextVoice_;
  for (int i = 0; i < 9; i++) {
    v = (v + 1) % 9;
    nextVoice_ = v;
    if (owner_[v] < 0) {
      s.voice = v;
      owner_[v] = s.ch;
      chans_[s.ch].voices++;
      s.flags = 0xf9;
      update(s);
      return;
    }
  }
  steal();
}

// A note's claim to a voice is 0x7FFF less the voices its channel already
// holds (a channel locked by controller 112 claims 0xFFFF). While the best
// claim of a note without a voice is at least the weakest claim of one
// with a voice, the weaker note is cut off and its voice handed over.
void AilFm::steal() {
  int prio[16] = {};
  int n = 0;
  for (int i = 0; i < 16; i++) {
    Slot& s = slots_[i];
    if (s.active) {
      n++;
      Chan& c = chans_[s.ch];
      int p = (c.lock >= 64 ? 0xffff : 0x7fff) - c.voices;
      prio[i] = p < 0 ? 0 : p;
    }
  }
  while (n > 0) {
    int best = -1, bestP = 0, worst = -1, worstP = 0xffff;
    for (int i = 0; i < 16; i++) {
      Slot& s = slots_[i];
      if (!s.active) continue;
      int p = prio[i];
      if (s.voice < 0) {
        if (p >= bestP) { best = i; bestP = p; }
      } else if (p <= worstP) {
        worst = i;
        worstP = p;
      }
    }
    if (bestP < worstP || bestP == 0 || best < 0 || worst < 0) return;
    Slot& w = slots_[worst];
    Slot& b = slots_[best];
    int v = w.voice;
    release(w);
    w.active = false;
    b.voice = v;
    owner_[v] = b.ch;
    chans_[b.ch].voices++;
    b.flags = 0xf9;
    update(b);
    n--;
  }
}

// Key off and give the voice back (179c).
void AilFm::release(Slot& s) {
  if (s.voice < 0) return;
  s.keyon = 0;
  if (s.flags % 2 == 0) s.flags++;
  update(s);
  chans_[s.ch].voices--;
  owner_[s.voice] = -1;
  s.voice = -1;
}

void AilFm::noteOn(int ch, int key, int vel) {
  if (vel == 0) return noteOff(ch, key);
  const Timbre* t;
  if (ch == 9) {
    auto it = percussion_.find(key);
    if (it == percussion_.end()) {
      auto l = lib_.find(127 * 128 + key);
      t = l == lib_.end() ? nullptr : &l->second;
      percussion_[key] = t;
    } else {
      t = it->second;
    }
  } else {
    t = chans_[ch].timbre;
  }
  if (!t || t->size() != 12) return;
  Slot* sp = nullptr;
  for (Slot& s : slots_)
    if (!s.active) { sp = &s; break; }
  if (!sp) return;
  Slot& s = *sp;
  const Timbre& T = *t;
  s.ch = ch;
  s.key = key;
  // a drum plays at the pitch its timbre's transpose byte names
  if (ch == 9) { s.note = T[0]; s.transpose = 0; }
  else { s.note = key; s.transpose = s8(T[0]); }
  s.vel = velocity_[vel / 8];
  s.active = true;
  s.sustained = false;
  s.voice = -1;
  // Copy a 14-byte timbre into the slot (1d6c). Bytes, after the length:
  // transpose, then the modulator's 20/40/60/80/E0 registers, C0, and the
  // carrier's 20/40/60/80/E0.
  s.keyon = 0x20;
  s.fbc = T[6];
  s.conn = T[6] % 2;
  s.modKsl = T[2] - T[2] % 64;
  s.modLevel = 63 - T[2] % 64;
  s.carKsl = T[8] - T[8] % 64;
  s.carLevel = 63 - T[8] % 64;
  s.modAvek = T[1] - T[1] % 16;
  s.modMult = T[1] % 16;
  s.carAvek = T[7] - T[7] % 16;
  s.carMult = T[7] % 16;
  s.modAD = T[3];
  s.modSR = T[4];
  s.modWS = T[5];
  s.carAD = T[9];
  s.carSR = T[10];
  s.carWS = T[11];
  // which operators follow the volume: the carrier always, the modulator
  // only when it is heard directly (additive)
  s.scaleMod = s.conn == 1;
  s.flags = 0xf9;
  allocate(s);
}

void AilFm::noteOff(int ch, int key) {
  Chan& c = chans_[ch];
  for (Slot& s : slots_) {
    if (s.active && s.key == key && s.ch == ch) {
      if (c.sustain >= 64) {
        s.sustained = true;
      } else {
        release(s);
        s.active = false;
      }
    }
  }
}

void AilFm::controller(int ch, int n, int v) {
  Chan& c = chans_[ch];
  int bitv = 0;
  int* field = nullptr;
  if (n == 114) c.bank = v;
  else if (n == 112) c.lock = v;
  else if (n == 64) {
    c.sustain = v;
    if (v < 64) {
      for (Slot& s : slots_)
        if (s.active && s.ch == ch && s.sustained) noteOff(ch, s.key);
    }
  } else if (n == 1) { field = &c.mod; bitv = 0x80; }
  else if (n == 7) { field = &c.vol; bitv = 0x40; }
  else if (n == 11) { field = &c.expr; bitv = 0x40; }
  else if (n == 10) { field = &c.pan; bitv = 0x40; }
  if (field) {
    *field = v;
    for (Slot& s : slots_) {
      if (s.active && s.ch == ch) {
        if ((s.flags / bitv) % 2 == 0) s.flags += bitv;
        update(s);
      }
    }
  }
}

// One MIDI message (2009). Notes only sound on channels 2-10 (1-9 here).
void AilFm::message(int st, int a, int b) {
  int hi = st & 0xf0, ch = st & 15;
  if (hi == 0x90) {
    if (ch >= 1 && ch <= 9) noteOn(ch, a, b);
  } else if (hi == 0x80) {
    noteOff(ch, a);
  } else if (hi == 0xb0) {
    controller(ch, a, b);
  } else if (hi == 0xc0) {
    program(ch, a);
  } else if (hi == 0xe0) {
    chans_[ch].bend = b * 128 + a - 0x2000;
    for (Slot& s : slots_) {
      if (s.active && s.ch == ch) {
        if (s.flags % 2 == 0) s.flags++;
        update(s);
      }
    }
  }
}

void AilFm::play(const xmi::Sequence* seq, bool loop) {
  stop();
  seq_ = seq;
  pos_ = 0;
  tick_ = 0;
  loop_ = loop;
  playing_ = seq != nullptr;
}

void AilFm::stop() {
  for (Slot& s : slots_) {
    if (s.active) {
      release(s);
      s.active = false;
    }
  }
  playing_ = false;
}

// one interval of AIL's 120 Hz timer
void AilFm::interval() {
  const xmi::Sequence* q = seq_;
  if (!playing_ || !q) return;
  int t = tick_;
  while (pos_ < q->events.size() && q->events[pos_].t <= t) {
    const auto& e = q->events[pos_];
    message(e.st, e.a, e.b);
    pos_++;
  }
  tick_ = t + 1;
  if (pos_ >= q->events.size() && tick_ > q->length) {
    if (loop_) play(q, true);
    else stop();
  }
}

bool AilFm::idle() const { return !playing_ && chip_.silent(); }

void AilFm::render(int16_t* out, int n) {
  const double per = Opl::RATE / xmi::TICK_RATE;
  int i = 0;
  while (i < n) {
    if (untilTick_ <= 0) {
      interval();
      untilTick_ += per;
    }
    int k = std::min(n - i, (int)std::ceil(untilTick_));
    chip_.generate(out + i, k);
    i += k;
    untilTick_ -= k;
  }
}

}  // namespace w2
