// Heroes: offers, recruitment, experience, promotion and death.
//
// docs/rules.md > Heroes. This is where the first of the original's bugs
// shows up: see battleExperience and rules.bugs.

import * as armytype from "./armytype.js";
import * as rules from "./rules.js";
import * as scn from "./scn.js";
import * as move from "./move.js";
import * as game from "./game.js";
import * as ai from "./ai.js";
import * as history from "./history.js";
import * as vfs from "../vfs.js";
import { luaSort } from "../util.js";

export const MAX_IN_GAME = 40;          // heroes in the whole game
export const MAX_PER_SIDE = 5;          // 6 once the side owns this many cities
export const MAX_PER_BIG_SIDE = 6, BIG_SIDE_CITIES = 40;
export const START_STRENGTH = 5, START_MOVES = 14;
export const MAX_STRENGTH = 9;
export const PROMOTION_MOVES = 2;

// experience needed for each promotion, and what the levels are called; by
// level, 1-based as the game numbers them
export const LEVELS = { 1: "Hero", 2: "Cavalier", 3: "Champion", 4: "Paladin" };
export const PROMOTION_AT = { 1: 15, 2: 30, 3: 60 };

// Each side has its own hundred candidate heroes in TERRAIN0/HERONAM<side>.DAT,
// "#0 Sir Nick" a line. load_hero_name (6563:0c67) rolls dice(1, 100, 0); the
// count is hard-coded, so HERONAM4.DAT's 101st hero is never drawn.
export const NAME_ROLL = 100;

function nameFile(g, side) {
  return (g.dataDir || "") + "/TERRAIN0/HERONAM" + side.index + ".DAT";
}

/** The candidate list for a side, as [{ name, female }...]. Cached. */
export function names(g, side) {
  g.heroNames = g.heroNames || {};
  const got = g.heroNames[side.index];
  if (got) return got;
  const list = [];
  const text = vfs.readText(nameFile(g, side)) || "";
  for (const line of text.split(/[\r\n]+/)) {
    const m = line.match(/^#(\d)\s+(.+)$/);
    if (m) list.push({ name: m[2], female: m[1] === "1" });
  }
  g.heroNames[side.index] = list;
  return list;
}

/** Roll the name and sex of a hero offering itself to `side`: [name, female]. */
export function rollName(g, side) {
  const list = names(g, side);
  const n = g.rng.dice(1, NAME_ROLL, 0);
  const pick = list[n - 1] || list[list.length - 1];
  if (!pick) return ["Hero", false];
  return [pick.name, pick.female];
}

function countHeroes(g, side) {
  let all = 0, mine = 0;
  for (const a of g.armies) {
    if (a.type === armytype.HERO) {
      all++;
      if (side && a.owner === side.index) mine++;
    }
  }
  return [all, mine];
}

/** Does a hero offer itself to `side` this turn, and at what price? null when
 *  none does (hero_offer_check, 7563:0000). */
export function offer(g, side) {
  const named = (o) => {
    [o.name, o.female] = rollName(g, side);
    return o;
  };
  if (g.turn === 1) return named({ price: 0, city: side.capital, first: true });

  const [all, mine] = countHeroes(g, side);
  if (all >= MAX_IN_GAME) return null;
  const cities = game.sideCities(g, side);
  const limit = cities.length >= BIG_SIDE_CITIES ? MAX_PER_BIG_SIDE : MAX_PER_SIDE;
  if (mine >= limit) return null;

  const price = mine === 0 ? g.rng.dice(1, 400, 300) : g.rng.dice(1, 600, 1000);
  if (price > side.gold) return null;
  if (g.rng.dice(1, 30, 0) >= 7) return null;          // a 20% chance

  let city = g.rng.pick(cities);
  if (!city) return null;
  // a computer's hero appears where its AI wants one (ai_hero_city, 5db9:0919)
  if (side.computer) city = ai.heroCity(g, side, city);
  return named({ price, city });
}

/** The allies a hired hero brings: 1-3 of one random magical type. */
export function allies(g) {
  const magical = g.types.list.filter((a) => (a.bonus[48] || 0) !== 0);
  let type = g.rng.pick(magical);
  if (!type) {
    for (const a of g.types.list) if (a.name.includes("Dragon")) type = a;
  }
  const roll = g.rng.dice(1, 100, 0);
  const n = roll < 70 ? 1 : roll < 95 ? 2 : 3;
  return [type, n];
}

/** One ally army, as create_ally_army (6536:12e6) builds it: moves full, no
 *  upkeep. */
export function newAlly(g, type, x, y, owner, homeCity) {
  return {
    x, y, owner, type: type.id, name: type.name,
    strength: type.strength, maxMoves: type.move, moves: type.move,
    upkeep: 0, homeCity,
  };
}

/** Hire the offered hero: [hero, allies] (hero_recruit, 7563:031b). */
export function recruit(g, side, off) {
  const city = off.city;
  side.gold = Math.max(0, side.gold - (off.price || 0));
  let [hx, hy] = game.freeTileIn(g, city, true);
  if (hx == null) { hx = city.x; hy = city.y; }

  const h = {
    x: hx, y: hy, owner: side.index, type: armytype.HERO,
    name: off.name || "Hero", female: off.female || undefined,
    strength: START_STRENGTH,
    maxMoves: START_MOVES, moves: START_MOVES, upkeep: 0,
    homeCity: city.index, level: 1, experience: 0, items: [],
  };
  if (off.first) {
    // Turn 1: the hero carries its side's standard, item number = side
    const std = g.map.items[side.index];
    if (std) {
      std.status = 3; std.x = undefined; std.y = undefined;
      std.standardOf = side.index;
      h.items.push(std);
    }
  }
  g.armies.push(h);
  history.deed(g, side, history.EMERGES, city.index, 0, h.name);     // 7563:04e7

  const out = [];
  if (!off.first) {
    const [type, n] = allies(g);
    for (let i = 0; i < n; i++) {
      const [ax, ay] = game.freeTileIn(g, city, true);
      const a = newAlly(g, type, ax != null ? ax : hx, ay != null ? ay : hy, side.index, city.index);
      g.armies.push(a);
      out.push(a);
    }
  }
  return [h, out];
}

/** Give a hero experience, capped at 60. */
export function addExperience(g, h, n) {
  h.experience = Math.min(rules.MAX_HERO_XP, (h.experience || 0) + n);
}

/** Promote a side's heroes, one step each (hero_check_promotions, 7563:0579). */
export function checkPromotions(g, side) {
  const promoted = [];
  for (const a of g.armies) {
    if (a.type === armytype.HERO && a.owner === side.index) {
      const level = a.level || 1;
      const need = PROMOTION_AT[level];
      if (need != null && (a.experience || 0) >= need) {
        a.level = level + 1;
        a.strength = Math.min(MAX_STRENGTH, a.strength + 1);
        a.maxMoves += PROMOTION_MOVES;
        a.title = LEVELS[a.level];
        promoted.push(a);
      }
    }
  }
  return promoted;
}

/** Award experience after a battle. **Original bug**: a surviving defending
 *  hero is credited only when the attacker at the same place in the line is
 *  a hero (67cc:0c8e). */
export function battleExperience(g, attackers, defenders, result, wasCity) {
  for (const a of attackers) {
    if (a.type === armytype.HERO && !result.deadByArmy.has(a)) addExperience(g, a, wasCity ? 2 : 1);
  }
  defenders.forEach((d, i) => {
    if (!result.deadByArmy.has(d)) {
      let counts;
      if (rules.bugs.heroExperienceReadsAttackerTypes) {
        const sameSlot = attackers[i];                 // the wrong line, on purpose
        counts = sameSlot != null && sameSlot.type === armytype.HERO;
      } else {
        counts = true;
      }
      if (counts && d.type === armytype.HERO) addExperience(g, d, 1);
    }
  });
}

/** What the hero info dialog lists for a hero (796c:03d9): the items it
 *  carries, then those lying on its tile, each in item order. */
export function itemsHere(g, h) {
  const carried = (h.items || []).slice();
  const ground = g.map.items.filter((it) => it.status === 1 && it.x === h.x && it.y === h.y);
  const byIndex = (a, b) => (a.index || 0) < (b.index || 0);
  luaSort(carried, byIndex);
  luaSort(ground, byIndex);
  return carried.concat(ground);
}

/** Put an item down on the hero's tile (7563:0943) -- or lose it at sea. */
export function dropItem(g, h, it) {
  const items = h.items || [];
  const i = items.indexOf(it);
  if (i >= 0) items.splice(i, 1);
  if (scn.terrainAt(g.map, h.x, h.y) === move.WATER) {
    it.status = 0; it.x = undefined; it.y = undefined;
  } else {
    it.status = 1; it.x = h.x; it.y = h.y; it.planted = undefined;
  }
}

/** Pick an item up off the hero's tile (7563:08c7). */
export function takeItem(g, h, it) {
  it.status = 3; it.x = undefined; it.y = undefined; it.planted = undefined;
  h.items = h.items || [];
  h.items.push(it);
}

/** A dead hero drops everything it carried where it fell -- lost on water
 *  (hero_drop_items, 67cc:16cd). */
export function dropItems(g, h, x, y) {
  if (!h.items || h.items.length === 0) return [];
  const drowned = scn.terrainAt(g.map, x, y) === move.WATER;
  const dropped = [];
  for (const it of h.items) {
    if (drowned) {
      it.status = 0; it.x = undefined; it.y = undefined;
    } else {
      it.status = 1; it.x = x; it.y = y;
      dropped.push(it);
    }
    // cities vectoring to a lost standard stop doing so
    if (it.standardOf != null) {
      for (const c of g.map.cities) {
        if (c.vectorTo === -2 && c.ownerIndex === it.standardOf) c.vectorTo = undefined;
      }
    }
  }
  h.items = [];
  return dropped;
}
