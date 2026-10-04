// The computer players' city phases: evaluate, clean city, neutral, quick
// attack, update hide, rebuilding, production and vectoring.
// docs/re/ai.md; addresses are Ghidra's.
#pragma once

#include <map>
#include <utility>
#include <vector>

#include "warlords/types.hpp"

namespace w2::ai::cities {

/** The side's own cities, last first, as the original walks them. */
std::vector<City*> own(const Game& g, const Side& side);
struct NeutralNeighbour {
  City* city;
  int dist;
  int slot;   // from 1
};
/** The neutral cities among a city's neighbours (57ea:01f6). */
std::vector<NeutralNeighbour> neutralNeighbours(Game& g, Side& side, const City& c);
/** The distance from (x, y) to the nearest army of another side -- or of side
 *  `of` -- that the side can see (57ea:0a3e). 1000 when there is none. */
int nearestArmy(const Game& g, const Side& side, int x, int y, int of = NONE);
/** Can this city build something worth having (623c:1a4d)? {good, slot index}. */
std::pair<bool, int> buildsWell(Game& g, Side& side, const City& c);
/** Is a city part way through building something (623c:0fd5)? */
bool building(const City& c);
/** Vector a city's production (623c:0f10): only while it is building. */
void vector(Game& g, City& c, const City* dest);
/** Clear "not seen yet" for cities the side has seen round (59bf:0b55). */
void clearUnseen(Game& g, Side& side);
/** evaluate (ai_phase_evaluate, 59bf:0000). */
void evaluate(Game& g, Side& side, int who);
/** The garrison a city wants (ai_wanted_garrison, 5ca7:0a3d). */
int wanted(Game& g, const Side& side, const City& c);
/** Look over a city's armies and stand them where they belong
 *  (ai_city_garrison_check, 5ca7:023f). */
bool garrison(Game& g, Side& side, City& c, bool force);
/** clean city (ai_phase_clean_city, 5ca7:01f1). */
void clean(Game& g, Side& side);
/** The first neutral neighbour fewer than three stacks are heading for
 *  (57ea:0935), or null. */
City* openNeutral(Game& g, Side& side, const City& c, std::map<int, int>& heading);
/** neutral (ai_phase_neutral, 57ea:0000): up to ten passes. */
void neutral(Game& g, Side& side);
/** quick attack (ai_phase_quick_attack, 5e97:04ba): not for a cautious side. */
void quickAttack(Game& g, Side& side);
/** update hide (ai_phase_update_hide, 59bf:0c1c). */
void updateHide(Game& g, Side& side);
/** rebuilding (ai_rebuilding, 5db9:0af2). */
void rebuild(Game& g, Side& side);
/** production (ai_production, 5db9:06d4). */
void production(Game& g, Side& side);
/** vectoring (ai_vectoring, 5db9:085f). */
void vectoring(Game& g, Side& side);

}  // namespace w2::ai::cities
