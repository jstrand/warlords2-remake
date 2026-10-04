// A quiet stand-in until the mixer is written: nothing sounds, and what
// waits on a sample waits on nothing.
#include "platform/sound.hpp"

namespace sound {
static Options opts;
static std::string chosen = "fm";
void init(const std::string&, const w2::uidata::Strings&) {}
void shutdown() {}
Options& options() { return opts; }
void saveOptions() {}
void toggle(const std::string& which) {
  if (which == "music") opts.music = !opts.music;
  else if (which == "effects") opts.effects = !opts.effects;
  else if (which == "speech") opts.speech = !opts.speech;
}
void music(int, const std::vector<w2::Side*>&) {}
void stopMusic() {}
std::string synth() { return chosen; }
bool synthAvailable(const std::string& s) { return s == "fm"; }
void setSynth(const std::string& s) { chosen = s; }
double effect(const std::string&) { return 0; }
bool speak(int, std::function<void()>) { return false; }
bool speechOn() { return false; }
bool busy() { return false; }
void update() {}
}  // namespace sound
