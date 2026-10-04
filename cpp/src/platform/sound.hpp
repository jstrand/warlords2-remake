// The game's sound: the samples (.8SN) queued one after another as DIGPAK
// plays them, the advisor's voice, and the music -- the AdLib songs
// synthesized live through the remake's own OPL2, or the MT-32 and Sound
// Canvas recordings streamed from pre-rendered-sound/. docs/re/sound.md.
#pragma once

#include <functional>
#include <string>
#include <vector>

#include "warlords/types.hpp"
#include "warlords/uidata.hpp"

namespace sound {

/** Open the audio device and read what the game plays: FILE.DAT's names,
 *  OPTIONS.SND's switches. Quiet if there is no device. */
void init(const std::string& dataDir, const w2::uidata::Strings& files);
void shutdown();

struct Options {
  bool music = true, effects = true, speech = true;
};
Options& options();
/** Write OPTIONS.SND after a switch has changed. */
void saveOptions();
void toggle(const std::string& which);   // "music", "effects", "speech"

/** Play a cue's song (warlords/cues). */
void music(int cue, const std::vector<w2::Side*>& sides = {});
void stopMusic();
/** The synthesizer the songs play on: "fm", "mt32" or "sc55". */
std::string synth();
bool synthAvailable(const std::string& s);
void setSynth(const std::string& s);

/** Queue a named sample ("turn", "war", "ding", ...); how long it plays, in
 *  seconds, 0 if it does not. */
double effect(const std::string& name);
/** The advisor says a FILE.DAT group; `after` runs when he has finished.
 *  False when there is nothing to say (no clip, speech off, no device). */
bool speak(int group, std::function<void()> after);
bool speechOn();
/** Is a sample still sounding, or waiting to? */
bool busy();
/** Called every frame: runs what finished playing. */
void update();

}  // namespace sound
