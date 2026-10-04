// The game's music player: Miles' AIL 2.0 AdLib driver (ADLIB.ADV), which
// MIDPAK.COM loads for WARLORD2.EXE, rebuilt from its disassembly.
//
// The driver reads XMIDI (xmi) at 120 Hz, gives each note one of nine OPL2
// voices and writes the chip's registers from a timbre in the Global Timbre
// Library, MIDPAK.AD. Its tables -- frequencies, the reset values, the
// velocity curve -- are read from the player's own ADLIB.ADV. Offsets and
// routines are in docs/formats/sound.md. A port of the Lua remake's
// ailfm.lua.
//
//     AilFm drv(advBytes, adBytes);
//     drv.play(&seq, true);        // true: start again at the end
//     drv.render(out, n);          // n samples at Opl::RATE
#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "warlords/opl.hpp"
#include "warlords/xmi.hpp"

namespace w2 {

class AilFm {
 public:
  static constexpr double RATE = Opl::RATE;

  AilFm(const std::string& adv, const std::string& ad);
  /** Start a parsed sequence from its beginning; it must outlive the play. */
  void play(const xmi::Sequence* seq, bool loop);
  /** Stop the sequence and let every sounding note go. */
  void stop();
  /** Nothing playing and every note faded away. */
  bool idle() const;
  bool playing() const { return playing_; }
  /** The next n samples, stepping the sequence as time passes. */
  void render(int16_t* out, int n);

 private:
  using Timbre = std::vector<uint8_t>;   // the bytes after the u16 length
  struct Chan {
    int vol = 0, expr = 0, mod = 0, sustain = 0, lock = 0, bank = 0, bend = 0, pan = 0, voices = 0;
    const Timbre* timbre = nullptr;
  };
  struct Slot {
    bool active = false, sustained = false;
    int voice = -1, flags = 0, ch = 0, key = 0, note = 0, transpose = 0, vel = 0;
    int keyon = 0, fbc = 0, conn = 0, b0 = 0;
    int modKsl = 0, modLevel = 0, carKsl = 0, carLevel = 0;
    int modAvek = 0, modMult = 0, carAvek = 0, carMult = 0;
    int modAD = 0, modSR = 0, modWS = 0, carAD = 0, carSR = 0, carWS = 0;
    bool scaleMod = false;
  };

  void program(int ch, int p);
  void update(Slot& s);
  int frequency(const Slot& s, const Chan& c);
  void allocate(Slot& s);
  void steal();
  void release(Slot& s);
  void noteOn(int ch, int key, int vel);
  void noteOff(int ch, int key);
  void controller(int ch, int n, int v);
  void message(int st, int a, int b);
  void interval();

  // ADLIB.ADV's tables
  int freq_[192], octave_[96], halftone_[96], reset_[0xf6], velocity_[16], programs_[9];
  std::map<int, Timbre> lib_;   // bank * 128 + patch
  Opl chip_;
  Chan chans_[16];
  std::map<int, const Timbre*> percussion_;   // channel 10's cache
  Slot slots_[16];
  int owner_[9];                // voice -> channel, or -1 when free
  int nextVoice_ = 0;
  int fw_ = 0, block_ = 0;
  const xmi::Sequence* seq_ = nullptr;
  size_t pos_ = 0;
  int tick_ = 0;
  double untilTick_ = 0;
  bool playing_ = false, loop_ = false;
};

}  // namespace w2
