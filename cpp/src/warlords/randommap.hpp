// The random map generator, "A Random World": random_map_setup (7bab:10e8)
// and random_map_generate (4bed:011c), phase for phase. docs/re/random_map.md.
//
// A port of the web port's web/src/warlords/randommap.js, line for line: the
// engines' dice are the same, so a seed makes the same world in all three.
// The result is a scenario like any other -- RANDOM.SCN (Erythea's,
// rewritten), .MAP, .RD, .SGN, .CTY and .SPC -- installed in memory
// (w2::installFile), where scn::load reads it as it reads the shipped ones.
// The shipped RANDOM folder holds the last world the original made, and is
// left alone.
#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>

#include "warlords/rng.hpp"

namespace w2::randommap {

constexpr const char* DIR = "RANDOM";
extern const char* const FILES[6];         // what it makes, by extension
using Files = std::map<std::string, std::string>;

// what the start menu shows beside each slider (4125:28e0, formats 4125:2918)
extern const int SLIDER_SHOWS[4][7];
extern const char* const SLIDER_FORMATS[4];
constexpr int RANDOM_SLIDER = 7;           // a slider of 7 is rolled: 1d7-1

struct Options {
  std::string dataDir;
  Rng* rng = nullptr;
  std::array<int, 4> sliders{{3, 3, 2, 3}};   // Water, Hills, Cities, Forest, 0-7
  bool allies = false;
  int terrainSet = 0;
};

/** The sliders as random_map_setup (7bab:10e8) takes them: 7 rolled as 1d7-1. */
std::array<int, 4> settle(std::array<int, 4> sliders, Rng& rng);

class Generator;

/** A world being made, a phase at a time, so the progress bar can move as
 *  the original's does. */
class Run {
 public:
  explicit Run(const Options& opts);
  ~Run();
  /** Do the next phase; false once the world is made. */
  bool step();
  /** How far along it is, 0-100, as the original's bar shows it. */
  int progress() const { return pct_; }
  /** The world's files, keyed by extension, once step() has said false. */
  const Files& files() const { return files_; }

 private:
  std::unique_ptr<Generator> gen_;
  int pct_ = 0;
  Files files_;
};

/** Make a random world through to its end. */
Files generateNow(const Options& opts);
/** Put a random world's files where scn::load will find them. */
void install(const std::string& dataDir, const Files& files);
/** The installed world's files, for a save to carry. */
Files installed(const std::string& dataDir);
/** The terrain set's name, as the start menu shows it (DATA\TERRAIN.DAT). */
std::string terrainSetName(const std::string& dataDir, int set);
/** How many terrain sets there are. */
int terrainSets(const std::string& dataDir);

}  // namespace w2::randommap
