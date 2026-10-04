// A Yamaha YM3812 (OPL2), the FM chip on the AdLib and the Sound Blaster.
//
// The game's music is played by Miles' AIL AdLib driver (ADLIB.ADV, see
// ailfm) writing registers to this chip, so this is the last link to the
// sound the original makes. It follows Nuked-OPL3's model of the chip -- the
// log-sine and exponent ROMs, the envelope generator's rate counter, the
// tremolo and vibrato counters -- cut down to OPL2: nine two-operator
// channels, four waveforms, no rhythm mode (the driver never turns it on).
// A port of the Lua remake's opl.lua.
//
//     Opl chip;
//     chip.write(0x20, 0x01); ...
//     chip.generate(out, n);     // n signed samples, about +-32767
//
// It runs at the chip's own rate, RATE: a 3.579545 MHz crystal / 72.
#pragma once

#include <cstdint>

namespace w2 {

class Opl {
 public:
  static constexpr double RATE = 3579545.0 / 72;   // 49715.9 Hz

  Opl();
  void write(int reg, int v);
  /** Fill out[0..n-1] with the next n samples. */
  void generate(int16_t* out, int n);
  /** True once no operator can make a sound. */
  bool silent() const;

 private:
  struct Slot {
    int am = 0, vib = 0, egt = 0, ksr = 0, mult = 0, ksl = 0, tl = 0;
    int ar = 0, dr = 0, sl = 0, rr = 0, wf = 0;
    bool key = false, reset = false;
    int gen = 3, rout = 0x1ff, out = 0, prout = 0;
    uint32_t phase = 0;
    int phaseOut = 0, egOut = 0x1ff;
  };
  struct Channel {
    int fnum = 0, block = 0, fb = 0, con = 0, ksv = 0, ksl = 0;
    Slot* mod = nullptr;
    Slot* car = nullptr;
  };
  void envelope(Slot& s, const Channel& c);
  void phaseGen(Slot& s, const Channel& c);

  Slot slots_[18];
  Channel ch_[9];
  bool wse_ = false, dam_ = false, dvb_ = false;
  uint32_t timer_ = 0, egTimer_ = 0;
  int egState_ = 0, egAdd_ = 0, egTimerLo_ = 0;
  int tremPos_ = 0, trem_ = 0, vibPos_ = 0;
};

}  // namespace w2
