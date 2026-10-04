// The computer players' assault groups: a rally city where a strike force
// gathers, up to four member cities building for it, and six enemy cities to
// take. Picking whom to attack is here too.
// docs/re/ai.md > Assault groups; addresses are Ghidra's.
#pragma once

#include <array>
#include <vector>

#include "warlords/ai/core.hpp"
#include "warlords/types.hpp"

namespace w2::ai::groups {

/** Is a city free to serve a new group (5f19:0a7a)? */
bool free(Game& g, Side& side, const City& c, bool members);
/** Is a city an active group's rally city (5f19:07f8)? */
bool isRally(Game& g, Side& side, const City& c);
/** Cancel a group (563e:066d). */
void cancel(Game& g, Side& side, int gi);
/** The gold sacking a city would bring (city_sack_value). */
int sackValue(const Game& g, const City& c);
void pillage(Game& g, Side& side, City& c);
void sack(Game& g, Side& side, City& c);
/** Raze a city (563e:1864); returns the stack's next city, or null. */
City* raze(Game& g, Side& side, City& c, bool force, core::Sel* sel = nullptr);
/** Early vengeance (5e97:0000). */
bool earlyVengeance(Game& g, int me, City& c, int was);
/** Look over what the side's cities can build for the groups (59bf:01b3). */
void prepare(Game& g, Side& side);
/** Send the group's stack at a city and deal with what it takes (563e:0f1b). */
int march(Game& g, Side& side, int gi, std::vector<Army*> list, City* c);
/** Follow up from the cities the group took (563e:16fd). */
void followUp(Game& g, Side& side, int gi);
/** assault (ai_phase_assault, 563e:0000). */
void assault(Game& g, Side& side);
/** Which side to attack (ai_pick_enemy, 5f19:0e04), or NONE. */
int pickEnemy(Game& g, Side& side);
/** For I am the Greatest: which computer sides are at war with a human. */
std::array<bool, 8> fightingHumans(const Game& g);
/** assault XX (ai_phase_assault_xx, 5f19:0000): start a new group. */
void assaultXX(Game& g, Side& side);

}  // namespace w2::ai::groups
