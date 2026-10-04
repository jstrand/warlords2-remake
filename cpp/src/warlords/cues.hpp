// What the game plays, and when: the choices behind the music and the
// advisor's voice, as WARLORD2.EXE makes them. docs/re/sound.md.
//
// Headless. The file names come from DATA/FILE.DAT (uidata::strings). A game
// should not play out differently because the music is on, so the caller
// passes a roll of its own: roll(n) -> 1..n.
#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "warlords/types.hpp"
#include "warlords/uidata.hpp"

namespace w2::cues {

using Roll = std::function<int(int)>;

// The argument of 6dda:0000, the music selector.
constexpr int TITLE = 0, PLAY = 1, COMPUTER = 2, TRIUMPH = 3, HERO = 4, TEMPLE = 5;
constexpr int SAGE = 6, PROMOTION = 7, MEDAL = 8, SURRENDER = 9, DEFIANCE = 10, BEGIN = 11;

/** The song for a cue: {FILE.DAT name, loops}; "" for none. */
std::pair<std::string, bool> song(const uidata::Strings& files, int cue, const std::vector<Side*>& sides, const Roll& roll);

// The advisor's clips are FILE.DAT groups 40-62, each with its subtitle 30
// groups on -- or, for the four set phrases, at index 1 of its own.
constexpr int GREET = 59, MOMENT = 60, BEGIN_WAR = 61, QUIT = 62;

/** What the advisor says as `side`'s turn opens (6dda:026f(5)): a FILE.DAT
 *  group, or NONE. Updates side.advisor as the original does. */
int advisor(const Game& g, Side& side, const Roll& roll);
/** The clip's sample and subtitle names for an advisor group. */
std::pair<std::string, std::string> clip(const uidata::Strings& files, int group, const Roll& roll);

}  // namespace w2::cues
