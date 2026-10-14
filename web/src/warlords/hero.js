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
  // none at all once the surrender has been offered (7563:0000)
  if (g.surrenderOffered) return null;
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

/** A random magical army type (random_magical_type, 7563:1a9b): the types
 *  are counted in id order, 0-27; Dragons (25) when there is none. */
export function magicalType(g) {
  const ids = [];
  for (let id = 0; id <= 27; id++) {
    const t = g.types.byId[id];
    if (t && (t.bonus[48] || 0) !== 0) ids.push(id);
  }
  const n = g.rng.dice(1, ids.length, -1);
  return g.types.byId[ids.length > 0 && ids[n] != null ? ids[n] : 25];
}

/** The allies a hired hero brings (hero_brings_allies, 7563:01fc): 1-3 of one
 *  random magical type. */
export function allies(g) {
  const type = magicalType(g);
  const roll = g.rng.dice(1, 100, 0);
  const n = roll < 70 ? 1 : roll < 95 ? 2 : 3;
  return [type, n];
}

/** One ally army, as create_ally_army (6536:12e6) builds it: moves full, no
 *  upkeep, home city 0. */
export function newAlly(g, type, x, y, owner) {
  return {
    x, y, owner, type: type.id, name: type.name,
    strength: type.strength, maxMoves: type.move, moves: type.move,
    upkeep: 0, homeCity: 0,
  };
}

/** Put an ally of the side's near (x, y) (create_ally_army, 6536:12e6): on
 *  the tile if it has room and no other side's army, else a random step
 *  from there and try again, ten tries in all. Returns the army, or null
 *  when none of the tries had room -- the ally is lost. */
export function placeAlly(g, type, x, y, side) {
  let px = x, py = y;
  for (let tries = 10; tries > 0; tries--) {
    if (px >= 0 && py >= 0 && px < g.map.width && py < g.map.height) {
      const here = game.armiesAt(g, px, py);
      if (here.length < 8 && (here.length === 0 || here[0].owner === side.index)) {
        return game.addArmy(g, newAlly(g, type, px, py, side.index));
      }
    }
    px += g.rng.dice(1, 3, -2);
    py += g.rng.dice(1, 3, -2);
  }
  return null;
}

/** Hire the offered hero: [hero, allies] (hero_recruit, 7563:031b). The hero
 *  stands on the city's first tile with room, or on a free tile round it --
 *  or, with none, on the last one tried. Its allies stand with it. */
export function recruit(g, side, off) {
  const city = off.city;
  side.gold = Math.max(0, side.gold - (off.price || 0));
  let [hx, hy, lx, ly] = game.freeTileIn(g, city, true);
  if (hx == null) { hx = lx != null ? lx : city.x; hy = ly != null ? ly : city.y; }

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
  game.addArmy(g, h);
  history.deed(g, side, history.EMERGES, city.index, 0, h.name);     // 7563:04e7

  const out = [];
  if (!off.first) {
    const [type, n] = allies(g);
    out.type = type; out.count = n;
    for (let i = 0; i < n; i++) {
      const a = placeAlly(g, type, hx, hy, side);
      if (a) out.push(a);
    }
  }
  return [h, out];
}

/** Give a hero experience, capped at 60. */
export function addExperience(g, h, n) {
  h.experience = Math.min(rules.MAX_HERO_XP, (h.experience || 0) + n);
}

/** Promote a side's heroes, one step each (hero_check_promotions, 7563:0579;
 *  hero_promote, 7563:0672): +1 strength (9 at most) and +2 to the maximum
 *  move. Returns [{ hero, gain }], gain the strength it really gained. */
export function checkPromotions(g, side) {
  const promoted = [];
  for (let i = g.armies.length - 1; i >= 0; i--) {
    const a = g.armies[i];
    if (a.type === armytype.HERO && a.owner === side.index) {
      const level = a.level || 1;
      const need = PROMOTION_AT[level];
      if (need != null && (a.experience || 0) >= need) {
        const before = a.strength || 0;
        a.level = level + 1;
        a.strength = Math.min(MAX_STRENGTH, before + 1);
        a.maxMoves += PROMOTION_MOVES;
        a.title = LEVELS[a.level];
        promoted.push({ hero: a, gain: a.strength - before });
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
 *  (hero_drop_items, 67cc:16cd). When the item is the current player's
 *  standard (item number = side), that side's cities stop vectoring to it.
 *  `cur` is the current player, g.side's by default. */
export function dropItems(g, h, x, y, cur) {
  if (!h.items || h.items.length === 0) return [];
  if (cur == null) cur = g.side ? g.side.index : undefined;
  const drowned = scn.terrainAt(g.map, x, y) === move.WATER;
  const dropped = [];
  for (const it of h.items) {
    it.planted = undefined;
    if (drowned) {
      it.status = 0; it.x = undefined; it.y = undefined;
    } else {
      it.status = 1; it.x = x; it.y = y;
      dropped.push(it);
    }
    if (it.index === cur) {
      for (const c of g.map.cities) {
        if (c.vectorTo === -2 && c.ownerIndex === cur) c.vectorTo = undefined;
      }
    }
  }
  h.items = [];
  return dropped;
}
