// The computer players' movement phases: move (standing orders), rescue, last
// rescue, move search and move explore, specials -- and the explorer walk and
// hero parties they use. docs/re/ai.md > Movement phases.
#pragma once

#include <vector>

#include "warlords/ai/core.hpp"
#include "warlords/types.hpp"

namespace w2::ai::moves {

/** A hero searches the site it has reached (5ad0:15c3). 2 when it searched. */
int searchSite(Game& g, core::Sel* sel, Army* h, Site& s);
/** Send a hero and one flying companion off (5ad0:11a6). */
int party(Game& g, Side& side, std::vector<Army*> list);
/** Send an army exploring (57ea:0e4b): steps while it has moves. */
void explore(Game& g, Side& side, Army* a, City* home);
/** move search (ai_phase_move_search, 5ad0:0284), with the map hidden. */
void search(Game& g, Side& side);
/** move explore (ai_phase_move_explore, 5ad0:0458). */
void heroParties(Game& g, Side& side);
/** move #1 and #2 (ai_phase_move, 5ad0:0000). */
void moveAll(Game& g, Side& side);
/** An idle army out in the field (5ad0:0c5b). */
void sendIdle(Game& g, Side& side, Army* a);
/** rescue (ai_phase_rescue, 5ad0:0888). */
void rescue(Game& g, Side& side);
/** last rescue (ai_phase_last_rescue, 5ad0:0b4d). */
void lastRescue(Game& g, Side& side);
/** specials (ai_phase_specials, 5ad0:10e3). */
void specials(Game& g, Side& side);

}  // namespace w2::ai::moves
