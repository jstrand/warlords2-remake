#include "warlords/opl.hpp"

#include <cmath>

namespace w2 {

namespace {

// The chip's ROMs, as the die shows them: a quarter sine in log2 form, 256
// steps to the octave, and the 2^x table that undoes it.
struct Roms {
  int logsin[256], exp[256];
  Roms() {
    for (int i = 0; i < 256; i++) {
      logsin[i] = (int)std::floor(-std::log(std::sin((i + 0.5) * M_PI / 512)) / std::log(2.0) * 256 + 0.5);
      exp[i] = (int)std::floor(std::pow(2.0, (255 - i) / 256.0) * 1024 + 0.5);
    }
  }
};
const Roms R;

// frequency multiplier, doubled (the chip's 0.5 is 1)
const int MT[16] = {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30};
const int KSLROM[16] = {0, 32, 40, 45, 48, 51, 53, 55, 56, 58, 59, 60, 61, 62, 63, 64};
const int KSLSHIFT[4] = {8, 1, 2, 0};
const int INCSTEP[4][4] = {{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 1, 0}, {1, 1, 1, 0}};

// operator register offsets -> operator number (-1 for none)
const int SLOT_OF_REG[32] = {0,  1,  2,  3,  4,  5,  -1, -1, 6,  7,  8,  9,  10, 11, -1, -1,
                             12, 13, 14, 15, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
const int CH_MOD[9] = {0, 1, 2, 6, 7, 8, 12, 13, 14};

enum { ATTACK, DECAY, SUSTAIN, RELEASE };

// the exponent: level 0 is loudest, every 256 halves it
inline int calcExp(int level) {
  if (level > 0x1fff) level = 0x1fff;
  return (R.exp[level & 0xff] * 2) >> (level >> 8);
}

inline int logsinOf(int phase) {
  return (phase & 0x100) ? R.logsin[(phase & 0xff) ^ 0xff] : R.logsin[phase & 0xff];
}

// the four OPL2 waveforms: sine, half sine, absolute sine, quarter pulses
inline int wave(int wf, int phase, int env) {
  phase &= 0x3ff;
  int lv;
  if (wf == 0) {
    int v = calcExp(logsinOf(phase) + env * 8);
    return (phase & 0x200) ? -v - 1 : v;
  } else if (wf == 1) {
    lv = (phase & 0x200) ? 0x1000 : logsinOf(phase);
  } else if (wf == 2) {
    lv = logsinOf(phase);
  } else {
    lv = (phase & 0x100) ? 0x1000 : R.logsin[phase & 0xff];
  }
  return calcExp(lv + env * 8);
}

}  // namespace

Opl::Opl() {
  for (int c = 0; c < 9; c++) {
    ch_[c].mod = &slots_[CH_MOD[c]];
    ch_[c].car = &slots_[CH_MOD[c] + 3];
  }
}

static void updateKsl(int& ksl, int& ksv, int fnum, int block) {
  int k = KSLROM[fnum >> 6] * 4 - (8 - block) * 32;
  ksl = k < 0 ? 0 : k;
  ksv = block * 2 + ((fnum >> 9) & 1);
}

void Opl::write(int reg, int v) {
  int hi = reg & 0xe0;
  if (reg == 0x01) {
    wse_ = (v & 0x20) != 0;
  } else if (reg == 0xbd) {
    dam_ = (v & 0x80) != 0;
    dvb_ = (v & 0x40) != 0;
  } else if ((hi >= 0x20 && hi <= 0x80) || hi == 0xe0) {
    int n = SLOT_OF_REG[reg & 0x1f];
    if (n < 0) return;
    Slot& s = slots_[n];
    if (hi == 0x20) {
      s.am = v >> 7;
      s.vib = (v >> 6) & 1;
      s.egt = (v >> 5) & 1;
      s.ksr = (v >> 4) & 1;
      s.mult = v & 15;
    } else if (hi == 0x40) {
      s.ksl = v >> 6;
      s.tl = v & 0x3f;
    } else if (hi == 0x60) {
      s.ar = v >> 4;
      s.dr = v & 15;
    } else if (hi == 0x80) {
      s.sl = v >> 4;
      s.rr = v & 15;
      if (s.sl == 15) s.sl = 31;
    } else {
      s.wf = wse_ ? (v & 3) : 0;
    }
  } else if (reg >= 0xa0 && reg <= 0xa8) {
    Channel& c = ch_[reg - 0xa0];
    c.fnum = (c.fnum & 0x300) | v;
    updateKsl(c.ksl, c.ksv, c.fnum, c.block);
  } else if (reg >= 0xb0 && reg <= 0xb8) {
    Channel& c = ch_[reg - 0xb0];
    c.fnum = (c.fnum & 0xff) | ((v & 3) << 8);
    c.block = (v >> 2) & 7;
    updateKsl(c.ksl, c.ksv, c.fnum, c.block);
    bool on = (v & 0x20) != 0;
    c.mod->key = on;
    c.car->key = on;
  } else if (reg >= 0xc0 && reg <= 0xc8) {
    Channel& c = ch_[reg - 0xc0];
    c.fb = (v >> 1) & 7;
    c.con = v & 1;
  }
}

// One step of an operator's envelope generator (Nuked's OPL3_EnvelopeCalc).
void Opl::envelope(Slot& s, const Channel& c) {
  int eo = s.rout + s.tl * 4 + (c.ksl >> KSLSHIFT[s.ksl]);
  if (s.am) eo += trem_;
  s.egOut = eo > 0x1ff ? 0x1ff : eo;

  bool reset = false;
  int rate;
  if (s.key && s.gen == RELEASE) {
    reset = true;
    rate = s.ar;
  } else if (s.gen == ATTACK) rate = s.ar;
  else if (s.gen == DECAY) rate = s.dr;
  else if (s.gen == SUSTAIN) rate = s.egt == 0 ? s.rr : 0;
  else rate = s.rr;
  s.reset = reset;

  int shift = 0, rateHi = 0;
  if (rate != 0) {
    int ks = c.ksv >> (s.ksr == 1 ? 0 : 2);
    int r = ks + rate * 4;
    rateHi = r >> 2;
    int rateLo = r & 3;
    if (rateHi > 15) rateHi = 15;
    if (rateHi < 12) {
      if (egState_ == 1) {
        int es = rateHi + egAdd_;
        if (es == 12) shift = 1;
        else if (es == 13) shift = (rateLo >> 1) & 1;
        else if (es == 14) shift = rateLo & 1;
      }
    } else {
      shift = (rateHi & 3) + INCSTEP[rateLo][egTimerLo_];
      if (shift & 4) shift = 3;
      if (shift == 0) shift = egState_;
    }
  }

  int rout = s.rout, inc = 0;
  if (reset && rateHi == 15) rout = 0;
  bool off = (s.rout & 0x1f8) == 0x1f8;
  if (s.gen != ATTACK && !reset && off) rout = 0x1ff;
  if (s.gen == ATTACK) {
    if (s.rout == 0) s.gen = DECAY;
    else if (s.key && shift > 0 && rateHi != 15) inc = (int)std::floor((-s.rout - 1) / std::pow(2.0, 4 - shift));
  } else if (s.gen == DECAY) {
    if ((s.rout >> 4) == s.sl) s.gen = SUSTAIN;
    else if (!off && !reset && shift > 0) inc = 1 << (shift - 1);
  } else {
    if (!off && !reset && shift > 0) inc = 1 << (shift - 1);
  }
  s.rout = (rout + inc) & 0x1ff;
  if (reset) s.gen = ATTACK;
  if (!s.key) s.gen = RELEASE;
}

// the phase generator, with the chip's vibrato
void Opl::phaseGen(Slot& s, const Channel& c) {
  int f = c.fnum;
  if (s.vib) {
    int range = (f >> 7) & 7;
    int vp = vibPos_;
    if ((vp & 3) == 0) range = 0;
    else if (vp & 1) range >>= 1;
    if (!dvb_) range >>= 1;
    if (vp & 4) range = -range;
    f += range;
  }
  uint32_t base = ((uint32_t)(f << c.block) & 0xffffffff) >> 1;
  s.phaseOut = (int)(s.phase >> 9);
  if (s.reset) s.phase = 0;
  s.phase = (s.phase + ((base * MT[s.mult]) >> 1)) & 0x7ffff;
}

bool Opl::silent() const {
  for (const Slot& s : slots_)
    if (s.key || s.gen != RELEASE || s.rout < 0x1f8) return false;
  return true;
}

void Opl::generate(int16_t* out, int n) {
  for (int i = 0; i < n; i++) {
    int acc = 0;
    for (Channel& c : ch_) {
      Slot& m = *c.mod;
      Slot& k = *c.car;
      // skip a channel both of whose envelopes are silent
      if (!(m.key || k.key || m.rout < 0x1f8 || k.rout < 0x1f8 || m.gen != RELEASE || k.gen != RELEASE)) continue;
      // modulator, with feedback from its last two outputs
      int fbmod = 0;
      if (c.fb != 0) fbmod = (int)std::floor((m.prout + m.out) / std::pow(2.0, 9 - c.fb));
      m.prout = m.out;
      envelope(m, c);
      phaseGen(m, c);
      m.out = wave(m.wf, m.phaseOut + fbmod, m.egOut);
      // carrier, frequency-modulated by it (FM) or beside it (AM)
      k.prout = k.out;
      envelope(k, c);
      phaseGen(k, c);
      if (c.con == 0) {
        k.out = wave(k.wf, k.phaseOut + m.out, k.egOut);
        acc += k.out;
      } else {
        k.out = wave(k.wf, k.phaseOut, k.egOut);
        acc += m.out + k.out;
      }
    }
    if (acc > 32767) acc = 32767;
    else if (acc < -32768) acc = -32768;
    out[i] = (int16_t)acc;

    // the chip's counters: tremolo every 64 samples, vibrato every 1024
    uint32_t t = timer_;
    if ((t & 0x3f) == 0x3f) tremPos_ = (tremPos_ + 1) % 210;
    int tp = tremPos_;
    if (tp >= 105) tp = 210 - tp;
    trem_ = tp >> (dam_ ? 2 : 4);
    if ((t & 0x3ff) == 0x3ff) vibPos_ = (vibPos_ + 1) & 7;
    timer_ = (t + 1) & 0xffff;
    // the envelope clock: runs at half the sample rate, and each step's
    // trailing zeros pick which slow rates advance
    if (egState_ == 1) {
      uint32_t et = egTimer_;
      int sh = 0;
      while (sh < 13 && ((et >> sh) & 1) == 0) sh++;
      egAdd_ = sh > 12 ? 0 : sh + 1;
      egTimerLo_ = (int)(et & 3);
      egTimer_ = (et + 1) & 0xfffffff;
    }
    egState_ = 1 - egState_;
  }
}

}  // namespace w2
