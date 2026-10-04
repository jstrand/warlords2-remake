// The rules of Warlords II, as read out of WARLORD2.EXE.
//
// Every number here has a citation in docs/rules.md. The functions are pure:
// they take state and dice, and return values. Turn order and mutation live in
// game.cpp.
#pragma once

#include <map>
#include <optional>
#include <vector>

#include "warlords/types.hpp"

namespace w2::rules {

/** Deliberate faults in the original, reproduced by default. */
struct Bugs {
  // Post-battle hero experience checks the *attacker's* type array when
  // deciding whether a surviving defender was a hero. Used by
  // hero::battleExperience.
  bool heroExperienceReadsAttackerTypes = true;
};
extern Bugs bugs;

constexpr int NEUTRAL = 15;          // the owner byte used for neutral cities
constexpr int MAX_STACK = 8;         // armies on one tile
constexpr int PRODUCTION_SLOTS = 4;  // army types a city can build
constexpr int MAX_MOVE = 99;         // movement points are capped here at turn start
constexpr int MOVE_CARRY = 2;        // unused moves carried into the next turn, at most
constexpr int SEA_MOVES = 20;        // an army at sea gets this instead of its maximum
constexpr int SEA_MIN_UPKEEP = 4;
constexpr int MAX_HERO_XP = 60;

// Production purposes: weights on (time, strength, move).
struct Purpose { int time, str, move; };
const Purpose& purpose(int p);

// Garrison level (0-3) -> production purpose. DS:0d60.
constexpr int GARRISON_PURPOSE[4] = {1, 6, 2, 3};

// Item effects (docs/rules.md > Item types).
constexpr int ITEM_BATTLE = 1, ITEM_COMMAND = 2;
constexpr int ITEM_FLIGHT = 5, ITEM_DOUBLE_MOVE = 6, ITEM_GOLD_PER_CITY = 7;
constexpr int ITEM_STANDARD = 8;

/** A city's defence: 1 below three production types, otherwise 2. */
inline int cityDefence(int nTypes) { return nTypes >= 3 ? 2 : 1; }

/** Build a city's production slots the way setup_city_production does:
 *  ARMYTYPE stats, a random nudge per stat, then sorted by purchase price. */
std::vector<Slot> citySlots(const std::vector<int>& produceIds, const Types& types, Rng& rng);

/** The slot a city would build for a purpose, or none (its index).
 *  best_production_for: scanned last to first with a strict >, so ties go to
 *  the later slot. */
int bestSlot(const std::vector<Slot>& slots, int purpose, const Types& types, bool sideBonus);

/** The garrison level a city starts with; none means "no garrison". */
std::optional<int> garrisonLevel(bool owned, int neutralCities, Rng& rng);

/** A new army's stats, from the city slot that built it. */
Army armyFromSlot(const Slot& slot, bool enhanced);

}  // namespace w2::rules
