// The game's music, effects and advisor voice, mixed in one SDL audio
// callback. A port of the Lua remake's sound.lua and musicthread.lua.
//
// Three switches, as Game > Settings has them and DATA/OPTIONS.SND keeps
// them -- three characters, '1' or '0', for Music, Effects and Speech.
//
// The samples are DIGPAK's: one plays at a time, and one asked for while
// another is sounding waits for it to end (255e:065e calls 255e:0736 first),
// so they queue here. Unsigned 8-bit mono at 11000 Hz.
//
// The AdLib music is synthesized live: the S-prefixed songs through the AIL
// driver into the emulated OPL2 (warlords/ailfm), rendered in the callback
// at the chip's own rate. The MT-32 and Sound Canvas songs are the
// recordings in pre-rendered-sound/<M|R><song>.ogg, streamed through
// libvorbisfile; tick 0 of the song is the recording's start, and a looping
// song starts over at its own length (from its .XMI) while the old one's
// tail rings out beneath.
//
// Everything is optional: with no audio device, or the files missing, the
// calls do nothing, and what waits on a sample does not wait.
#include "platform/sound.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>
#include <deque>
#include <map>
#include <memory>
#include <random>

#include "front/prefs.hpp"
#include "util/util.hpp"
#include "warlords/ailfm.hpp"
#include "warlords/cues.hpp"
#include "warlords/xmi.hpp"

#ifdef W2_VORBIS
#include <vorbis/vorbisfile.h>
#endif

namespace sound {

namespace {

constexpr int SAMPLE_RATE = 11000;
// The chip's nine voices leave a lot of headroom: a song peaks around a
// fifth of full scale, some 10 dB under the samples. Twice as loud brings
// the two close (musicthread.lua).
constexpr float FM_GAIN = 2.0f / 32768;

const std::map<std::string, char> PREFIX = {{"fm", 'S'}, {"mt32", 'M'}, {"sc55", 'R'}};

// FILE.DAT's sample names, by what the game calls them for
const std::map<std::string, std::pair<int, int>> EFFECT = {
    {"army", {64, 0}}, {"army2", {64, 5}}, {"ding", {64, 1}},  {"chord", {64, 2}}, {"dramatic", {37, 0}},
    {"orch", {37, 1}}, {"war", {38, 0}},   {"splash", {39, 0}}, {"turn", {63, 0}}};

using Pcm = std::shared_ptr<std::vector<float>>;

// One sample in the queue: the main thread's view of it.
struct Queued {
  Pcm pcm;
  std::function<void()> done;
  double endsAt = 0;   // by the wall clock, should the device stall
};

#ifdef W2_VORBIS
// A recording being streamed.
struct Stream {
  OggVorbis_File vf{};
  bool open = false, eof = false;
  int channels = 1;
  double step = 1;            // source frames per output frame
  std::vector<float> buf;     // decoded frames, interleaved stereo
  size_t frames = 0;          // in buf
  double pos = 0;             // into buf
  double played = 0;          // seconds handed out
  ~Stream() {
    if (open) ov_clear(&vf);
  }
  // Decode more until at least `need` frames lie ahead of pos.
  void fill(size_t need) {
    size_t at = (size_t)pos;
    if (at > 0) {
      buf.erase(buf.begin(), buf.begin() + std::min(buf.size(), at * 2));
      frames -= std::min(frames, at);
      pos -= at;
    }
    while (!eof && frames < need + 2) {
      float** pcm;
      int section;
      long n = ov_read_float(&vf, &pcm, 4096, &section);
      if (n <= 0) {
        eof = true;
        break;
      }
      for (long i = 0; i < n; i++) {
        float l = pcm[0][i], r = channels > 1 ? pcm[1][i] : l;
        buf.push_back(l);
        buf.push_back(r);
      }
      frames += n;
    }
  }
  bool done() const { return eof && pos + 1 >= frames; }
};
#endif

// Everything the callback touches; the main thread changes it only with
// the device locked.
struct Mixer {
  int rate = 44100;
  // the sample sounding
  Pcm sample;
  double samplePos = 0;
  bool sampleEnded = true;
  // the FM music
  std::unique_ptr<w2::AilFm> fm;
  std::shared_ptr<w2::xmi::Sequence> fmSeq;
  std::vector<int16_t> fmBuf;
  double fmPos = 0;
  bool fmWasPlaying = false, fmEnded = false;
#ifdef W2_VORBIS
  std::unique_ptr<Stream> rec;
  std::vector<std::unique_ptr<Stream>> tails;
#endif
};

struct State {
  std::string dataDir, recDir;
  w2::uidata::Strings files;
  Options on{false, false, false};
  SDL_AudioDeviceID dev = 0;
  Mixer mix;
  std::map<std::string, Pcm> samples;   // file name -> PCM, or null when missing
  std::deque<Queued> queue;
  std::optional<Queued> playing;
  int cue = w2::NONE;
  std::optional<std::pair<std::string, bool>> song;
  bool songPlaying = false;
  std::string synth = "fm";
  std::string adv, ad;
  // the recording's loop point, and whether it loops
  double recAt = 0;
  bool recLoop = false;
};
State S;

std::mt19937 dice{std::random_device{}()};
int roll(int n) { return (int)(dice() % (unsigned)std::max(1, n)) + 1; }

struct Lock {
  Lock() { if (S.dev) SDL_LockAudioDevice(S.dev); }
  ~Lock() { if (S.dev) SDL_UnlockAudioDevice(S.dev); }
};

// ------------------------------------------------------------- the callback

void mixSample(float* out, int n) {
  Mixer& m = S.mix;
  if (!m.sample || m.sampleEnded) return;
  const auto& p = *m.sample;
  double step = (double)SAMPLE_RATE / m.rate;
  for (int i = 0; i < n; i++) {
    size_t k = (size_t)m.samplePos;
    if (k >= p.size()) {
      m.sampleEnded = true;
      return;
    }
    double f = m.samplePos - k;
    float v = (float)(p[k] * (1 - f) + (k + 1 < p.size() ? p[k + 1] : 0) * f);
    out[i * 2] += v;
    out[i * 2 + 1] += v;
    m.samplePos += step;
  }
}

void mixFm(float* out, int n) {
  Mixer& m = S.mix;
  if (!m.fm || m.fm->idle()) {
    if (m.fmWasPlaying) m.fmEnded = true;
    m.fmWasPlaying = false;
    return;
  }
  double step = w2::Opl::RATE / m.rate;
  size_t need = (size_t)std::ceil(m.fmPos + n * step) + 2;
  // keep what is still ahead, render the rest
  size_t keep = (size_t)m.fmPos;
  if (keep > 0) {
    m.fmBuf.erase(m.fmBuf.begin(), m.fmBuf.begin() + std::min(keep, m.fmBuf.size()));
    m.fmPos -= keep;
    need -= keep;
  }
  if (m.fmBuf.size() < need) {
    size_t from = m.fmBuf.size();
    m.fmBuf.resize(need);
    m.fm->render(m.fmBuf.data() + from, (int)(need - from));
  }
  for (int i = 0; i < n; i++) {
    size_t k = (size_t)m.fmPos;
    double f = m.fmPos - k;
    float v = (float)((m.fmBuf[k] * (1 - f) + m.fmBuf[k + 1] * f) * FM_GAIN);
    v = std::clamp(v, -1.0f, 1.0f);
    out[i * 2] += v;
    out[i * 2 + 1] += v;
    m.fmPos += step;
  }
  bool now = m.fm->playing();
  if (m.fmWasPlaying && !now) m.fmEnded = true;
  m.fmWasPlaying = now;
}

#ifdef W2_VORBIS
void mixStream(Stream& s, float* out, int n, int rate) {
  if (s.done()) return;
  s.fill((size_t)std::ceil(n * s.step) + 2);
  for (int i = 0; i < n; i++) {
    size_t k = (size_t)s.pos;
    if (k + 1 >= s.frames) {
      if (s.eof) break;
      s.fill((size_t)std::ceil((n - i) * s.step) + 2);
      k = (size_t)s.pos;
      if (k + 1 >= s.frames) break;
    }
    double f = s.pos - k;
    out[i * 2] += (float)(s.buf[k * 2] * (1 - f) + s.buf[k * 2 + 2] * f);
    out[i * 2 + 1] += (float)(s.buf[k * 2 + 1] * (1 - f) + s.buf[k * 2 + 3] * f);
    s.pos += s.step;
  }
  s.played += (double)n / rate;
}
#endif

void callback(void*, Uint8* stream, int len) {
  float* out = (float*)stream;
  int n = len / (int)(sizeof(float) * 2);
  std::fill(out, out + n * 2, 0.0f);
  mixFm(out, n);
#ifdef W2_VORBIS
  Mixer& m = S.mix;
  if (m.rec) mixStream(*m.rec, out, n, m.rate);
  for (auto& t : m.tails) mixStream(*t, out, n, m.rate);
#endif
  mixSample(out, n);
  for (int i = 0; i < n * 2; i++) out[i] = std::clamp(out[i], -1.0f, 1.0f);
}

// ------------------------------------------------------------- helpers

// "SOUND\\army.8SN" -> <data>/SOUND/army.8SN, found ignoring case
std::string dataPath(const std::string& name) {
  std::string rel = name;
  std::replace(rel.begin(), rel.end(), '\\', '/');
  return S.dataDir + "/" + rel;
}

Pcm sampleData(const std::string& name) {
  auto it = S.samples.find(name);
  if (it != S.samples.end()) return it->second;
  Pcm p;
  if (auto bytes = w2::readFile(dataPath(name)); bytes && !bytes->empty()) {
    p = std::make_shared<std::vector<float>>(bytes->size());
    for (size_t i = 0; i < bytes->size(); i++) (*p)[i] = ((uint8_t)(*bytes)[i] - 128) / 128.0f;
  }
  S.samples[name] = p;
  return p;
}

void saveOptionsFile() {
  std::string s;
  s += S.on.music ? '1' : '0';
  s += S.on.effects ? '1' : '0';
  s += S.on.speech ? '1' : '0';
  w2::writeFile(S.dataDir + "/DATA/OPTIONS.SND", s);
}

// "INT12.XMI" -> <recordings>/RINT12.ogg
std::string recordingPath(const std::string& synth, const std::string& name) {
  std::string n = w2::upper(name);
  if (w2::endsWith(n, ".XMI")) n = n.substr(0, n.size() - 4);
  return S.recDir + "/" + PREFIX.at(synth) + n + ".ogg";
}

#ifdef W2_VORBIS
std::unique_ptr<Stream> openStream(const std::string& path) {
  auto s = std::make_unique<Stream>();
  std::string real = w2::resolvePath(path);
  if (real.empty() || ov_fopen(real.c_str(), &s->vf) != 0) return nullptr;
  s->open = true;
  vorbis_info* vi = ov_info(&s->vf, -1);
  s->channels = vi->channels;
  s->step = (double)vi->rate / S.mix.rate;
  return s;
}

// the main thread's half of a recording reaching its loop point
void stepRecording() {
  Lock lock;
  Mixer& m = S.mix;
  auto& r = m.rec;
  if (r && (r->played >= S.recAt || r->done())) {
    m.tails.push_back(std::move(r));
    if (S.recLoop && S.song) r = openStream(recordingPath(S.synth, S.song->first));
    else S.songPlaying = false;
  }
  m.tails.erase(std::remove_if(m.tails.begin(), m.tails.end(), [](auto& t) { return t->done(); }), m.tails.end());
  if (m.tails.size() > 4) m.tails.erase(m.tails.begin(), m.tails.end() - 4);
}
#endif

void stopRecordings() {
#ifdef W2_VORBIS
  std::unique_ptr<Stream> rec;
  std::vector<std::unique_ptr<Stream>> tails;
  {
    Lock lock;
    rec = std::move(S.mix.rec);
    tails = std::move(S.mix.tails);
    S.mix.tails.clear();
  }
#endif
}

void stopFm() {
  Lock lock;
  if (S.mix.fm) S.mix.fm->stop();
  S.mix.fmWasPlaying = false;
  S.mix.fmEnded = false;
}

// Start a song, `loop`ing or not, on the synthesizer chosen. False when it
// cannot play: the recording, or the FM driver, missing.
bool startSong(const std::string& name, bool loop) {
  if (!S.dev) return false;
#ifdef W2_VORBIS
  if (S.synth != "fm") {
    // the recording, and the song's own length from the file it was made
    // of: the driver starts a loop again one tick after it
    auto bytes = w2::readFile(S.dataDir + "/SOUND/" + PREFIX.at(S.synth) + w2::upper(name));
    auto seq = bytes ? w2::xmi::parse(*bytes) : std::nullopt;
    auto stream = seq ? openStream(recordingPath(S.synth, name)) : nullptr;
    if (stream) {
      stopFm();
      stopRecordings();
      Lock lock;
      S.mix.rec = std::move(stream);
      S.recAt = (seq->length + 1.0) / w2::xmi::TICK_RATE;
      S.recLoop = loop;
      S.song = std::make_pair(name, loop);
      S.songPlaying = true;
      return true;
    }
  }
#endif
  // the FM version's files carry an S: SINT12.XMI (255e:032f)
  if (S.adv.empty() || S.ad.empty()) return false;
  auto bytes = w2::readFile(S.dataDir + "/SOUND/S" + w2::upper(name));
  auto seq = bytes ? w2::xmi::parse(*bytes) : std::nullopt;
  if (!seq) return false;
  stopRecordings();
  auto shared = std::make_shared<w2::xmi::Sequence>(std::move(*seq));
  std::shared_ptr<w2::xmi::Sequence> old;
  {
    Lock lock;
    if (!S.mix.fm) S.mix.fm = std::make_unique<w2::AilFm>(S.adv, S.ad);
    old = S.mix.fmSeq;
    S.mix.fmSeq = shared;
    S.mix.fm->play(shared.get(), loop);
    S.mix.fmWasPlaying = true;
    S.mix.fmEnded = false;
  }
  S.song = std::make_pair(name, loop);
  S.songPlaying = true;
  return true;
}

// queue a sample behind whatever is sounding; `done` runs once it has ended
double enqueue(const std::string& name, std::function<void()> done) {
  Pcm p = S.dev ? sampleData(name) : nullptr;
  if (!p) {
    if (done) done();
    return 0;
  }
  S.queue.push_back(Queued{p, std::move(done)});
  update();
  return (double)p->size() / SAMPLE_RATE;
}

}  // namespace

// ------------------------------------------------------------- the interface

void init(const std::string& dataDir, const w2::uidata::Strings& files) {
  S.dataDir = dataDir;
  S.files = files;
  S.recDir = w2::fileExists("pre-rendered-sound") ? "pre-rendered-sound" : dataDir + "/../pre-rendered-sound";
  std::string opts = w2::readFile(dataDir + "/DATA/OPTIONS.SND").value_or("111");
  while (opts.size() < 3) opts += '1';
  S.on.music = opts[0] == '1';
  S.on.effects = opts[1] == '1';
  S.on.speech = opts[2] != '0';
  std::string synth = prefs::get("music");
  if (PREFIX.count(synth) && synthAvailable(synth)) S.synth = synth;
  S.adv = w2::readFile(dataDir + "/ADLIB.ADV").value_or("");
  S.ad = w2::readFile(dataDir + "/MIDPAK.AD").value_or("");
  SDL_AudioSpec want{}, have{};
  want.freq = 44100;
  want.format = AUDIO_F32SYS;
  want.channels = 2;
  want.samples = 1024;
  want.callback = callback;
  S.dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
  if (!S.dev) return;
  S.mix.rate = have.freq;
  SDL_PauseAudioDevice(S.dev, 0);
}

void shutdown() {
  if (!S.dev) return;
  SDL_CloseAudioDevice(S.dev);
  S.dev = 0;
#ifdef W2_VORBIS
  S.mix.rec.reset();
  S.mix.tails.clear();
#endif
  S.mix.fm.reset();
}

Options& options() { return S.on; }
void saveOptions() { saveOptionsFile(); }

// Turn one switch over (64d2:0576). Music turned on starts the play music;
// turned off it stops at once.
void toggle(const std::string& which) {
  if (which == "music") S.on.music = !S.on.music;
  else if (which == "effects") S.on.effects = !S.on.effects;
  else if (which == "speech") S.on.speech = !S.on.speech;
  if (which == "music") {
    if (S.on.music) {
      S.cue = w2::NONE;
      music(w2::cues::PLAY);
    } else {
      stopMusic();
    }
  }
  saveOptionsFile();
}

// Play a cue -- unless it is the one already playing, which goes on
// undisturbed (6dda:0000).
void music(int cue, const std::vector<w2::Side*>& sides) {
  if (!S.on.music) return;
  if (cue == S.cue && S.songPlaying) return;
  auto [name, loop] = w2::cues::song(S.files, cue, sides, roll);
  if (!name.empty() && startSong(name, loop)) S.cue = cue;
}

void stopMusic() {
  S.cue = w2::NONE;
  S.song.reset();
  S.songPlaying = false;
  stopFm();
  stopRecordings();
}

std::string synth() { return S.synth; }

// FM wants ADLIB.ADV; the others their recordings, and libvorbisfile.
bool synthAvailable(const std::string& s) {
  if (s == "fm") return true;
#ifdef W2_VORBIS
  return PREFIX.count(s) && w2::fileExists(recordingPath(s, "STARTUP.XMI"));
#else
  return false;
#endif
}

void setSynth(const std::string& s) {
  if (!PREFIX.count(s) || s == S.synth) return;
  S.synth = s;
  prefs::set("music", s);
  if (S.on.music && S.song && S.songPlaying) {
    auto song = *S.song;
    startSong(song.first, song.second);
  }
}

double effect(const std::string& what) {
  if (!S.on.effects || S.files.empty()) return 0;
  auto e = EFFECT.find(what);
  if (e == EFFECT.end()) return 0;
  auto [group, index] = e->second;
  if (group >= (int)S.files.size() || index >= (int)S.files[group].size()) return 0;
  const std::string& name = S.files[group][index];
  if (name.empty()) return 0;
  return enqueue(name, nullptr);
}

bool speak(int group, std::function<void()> after) {
  if (!S.on.speech || S.files.empty()) return false;
  auto [name, subtitle] = w2::cues::clip(S.files, group, roll);
  if (name.empty() || !S.dev || !sampleData(name)) return false;
  enqueue(name, std::move(after));
  return true;
}

bool speechOn() { return S.on.speech; }

bool busy() { return S.playing.has_value() || !S.queue.empty(); }

// Call every frame: start the next sample when the last has ended, hear
// whether a song ran out, and start a looping recording over.
void update() {
  if (S.playing) {
    bool ended;
    {
      Lock lock;
      ended = S.mix.sampleEnded;
    }
    // or, should the device stall, once its length has passed
    if (ended || w2::now() >= S.playing->endsAt) {
      auto done = std::move(S.playing->done);
      S.playing.reset();
      if (done) done();
    }
  }
  if (!S.playing && !S.queue.empty()) {
    Queued q = std::move(S.queue.front());
    S.queue.pop_front();
    q.endsAt = w2::now() + (double)q.pcm->size() / SAMPLE_RATE + 0.25;
    {
      Lock lock;
      S.mix.sample = q.pcm;
      S.mix.samplePos = 0;
      S.mix.sampleEnded = false;
    }
    S.playing = std::move(q);
  }
  {
    Lock lock;
    if (S.mix.fmEnded) {
      S.mix.fmEnded = false;
      if (S.song && !S.song->second) S.songPlaying = false;
    }
  }
#ifdef W2_VORBIS
  stepRecording();
#endif
}

}  // namespace sound
