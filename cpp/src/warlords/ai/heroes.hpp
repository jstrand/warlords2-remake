// The computer players' hero phase (ai_phase_move_hero, 6087:0000): each hero
// weighs a quest temple, a site, an item lying about and an enemy city --
// 100 - distance + 1d20 each -- and goes for the best, twice at most.
#pragma once

#include "warlords/types.hpp"

namespace w2::ai::heroes {

void phase(Game& g, Side& side);

}  // namespace w2::ai::heroes
