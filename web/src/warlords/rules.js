// The rules of Warlords II, as read out of WARLORD2.EXE.
//
// Every number here has a citation in docs/rules.md. The functions are pure:
// they take state and dice, and return values. Turn order and mutation live in
// game.js.

import { luaSort } from "../util.js";

/** Deliberate faults in the original, reproduced by default. */
export const bugs = {
  // Post-battle hero experience checks the *attacker's* type array when
  // deciding whether a surviving defender was a hero. Used by
  // hero.battleExperience.
  heroExperienceReadsAttackerTypes: true,
};

export const NEUTRAL = 15;          // the owner byte used for neutral cities
export const MAX_STACK = 8;         // armies on one tile
export const PRODUCTION_SLOTS = 4;  // army types a city can build
export const MAX_MOVE = 99;         // movement points are capped here at turn start
export const MOVE_CARRY = 2;        // unused moves carried into the next turn, at most
export const SEA_MOVES = 20;        // an army at sea gets this instead of its maximum
export const SEA_MIN_UPKEEP = 4;
export const MAX_HERO_XP = 60;

// Production purposes: weights on (time, strength, move).
export const PURPOSE = {
  1: { time: 10, str: 4, move: 1 },   // quick and cheap
  2: { time: 10, str: 10, move: 1 },  // balanced
  3: { time: 5, str: 10, move: 1 },   // strongest
  4: { time: 5, str: 10, move: 1 },   // flying types only
  5: { time: 5, str: 10, move: 1 },
  6: { time: 10, str: 1, move: 10 },  // fastest
};

// Garrison level (0-3) -> production purpose. DS:0d60.
export const GARRISON_PURPOSE = { 0: 1, 1: 6, 2: 2, 3: 3 };

// Item effects (docs/rules.md > Item types).
export const ITEM_BATTLE = 1, ITEM_COMMAND = 2;
export const ITEM_FLIGHT = 5, ITEM_DOUBLE_MOVE = 6, ITEM_GOLD_PER_CITY = 7;
export const ITEM_STANDARD = 8;

/** A city's defence: 1 below three production types, otherwise 2. */
export function cityDefence(nTypes) {
  return nTypes >= 3 ? 2 : 1;
}

/** Build a city's production slots the way setup_city_production does:
 *  ARMYTYPE stats, a random nudge per stat, then sorted by purchase price. */
export function citySlots(produceIds, types, rng) {
  const slots = [];
  for (const id of produceIds) {
    const a = types.byId[id];
    let { strength, time, cost, move } = a;
    if (rng.chance(10)) {
      if (rng.chance(60)) strength = Math.min(9, strength + 1);
      else strength = Math.max(1, strength - 1);
    }
    if (rng.chance(20)) {
      const r = rng.dice(1, 100, 0);
      move += r < 10 ? 4 : r < 60 ? 2 : r < 95 ? -2 : -4;
      move = Math.max(2, move);
    }
    move = Math.max(6, move);
    if (rng.chance(10)) {
      const quarter = Math.floor(cost / 4);
      cost += rng.chance(60) ? -quarter : quarter;
    }
    if (rng.chance(10)) {
      time = rng.chance(60) ? Math.max(1, time - 1) : time + 1;
    }
    slots.push({ type: id, name: a.name, strength, time, cost, move, price: Math.abs(a.price) });
  }
  luaSort(slots, (p, q) => p.price < q.price);
  return slots;
}

/** The slot a city would build for a purpose, or null. best_production_for:
 *  scanned last to first with a strict >, so ties go to the later slot. */
export function bestSlot(slots, purpose, types, sideBonus) {
  const w = PURPOSE[purpose];
  let best = null, bestScore = 0;
  for (let i = slots.length - 1; i >= 0; i--) {
    const slot = slots[i];
    const a = types.byId[slot.type];
    if (purpose !== 4 || a.flies) {
      let str = Math.min(9, slot.strength + (sideBonus ? 2 : 0));
      if (a.siege) str += 2;
      const time = slot.time + ((str < 3 && purpose !== 6) ? 1 : 0);
      const score = (10 - Math.min(10, time)) * w.time + str * w.str
                  + Math.floor(slot.move * w.move / 2);
      if (score > bestScore) { best = slot; bestScore = score; }
    }
  }
  return best;
}

/** The garrison level a city starts with; null means "no garrison". */
export function garrisonLevel(owned, neutralCities, rng) {
  if (owned) return 3;
  if (neutralCities <= 0) return null;
  return Math.min(3, rng.dice(1, 4, 0) + neutralCities - 2);
}

/** A new army's stats, from the city slot that built it. */
export function armyFromSlot(slot, enhanced) {
  let strength = slot.strength;
  if (enhanced) strength = Math.min(9, strength + 2);
  return {
    type: slot.type, name: slot.name,
    strength,
    maxMoves: slot.move,
    upkeep: Math.floor(slot.cost / 2),
  };
}
