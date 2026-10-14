// Combat: building the two lines, the modifiers, and the fight itself.
//
// docs/rules.md > Combat. combat_setup (6a89:008b) builds the lines,
// combat_terrain_class (6a89:0000) classifies the tile and combat_resolve
// (67cc:08a6) fights.

import * as armytype from "./armytype.js";
import * as rules from "./rules.js";
import * as scn from "./scn.js";
import * as move from "./move.js";
import * as game from "./game.js";
import { luaSort } from "../util.js";

// terrain classes, in the order the ARMYTYPE bonus fields are stored
export const CITY = 0, OPEN = 1, WOODS = 2, HILLS = 3;
export const CLASS_NAMES = ["city", "open", "woods", "hills"];

// ARMYTYPE offsets
const BONUS_SELF = 32, BONUS_STACK = 40;     // + 2*class
const SUBTRACT = 50, ABILITY = 52;
export const SIEGE = 1, NEGATE_HERO = 2, NEGATE_OTHER = 3;

// hero strength -> command bonus, indexed 0..9
export const HERO_TABLE = [0, 0, 0, 0, 1, 1, 1, 2, 2, 3];

export const HIT_POINTS = 2;
export const MAX_STRENGTH = 15;
export const DIE = 20, DIE_INTENSE = 24;
export const STALEMATE = 10000;              // throws in one duel before the attacker loses it

/** The battle tile's terrain class (combat_terrain_class, 6a89:0000). */
export function terrainClass(g, x, y) {
  if (g.towerAt && g.towerAt[y * g.map.width + x]) return CITY;
  const t = scn.terrainAt(g.map, x, y);
  if (t === move.CITY || t === move.SITE) return CITY;
  if (t === move.FOREST) return WOODS;
  if (t === move.HILLS || t === move.MOUNTAINS) return HILLS;
  return OPEN;
}

function fightOrder(g, ownerIndex) {
  const row = ownerIndex == null ? 8 : ownerIndex;
  return (a) => g.map.fightOrder[row][a.type] || 0;
}

/** On water, shore and mountains a side holding a hero sends one flier to
 *  the back, keeping the hero's carrier alive longest: the first flier in
 *  the order the line was gathered that is not at sea gets 80 on its rank
 *  (6a89:008b, before the sort). Returns the army, or null. */
function heroCarrier(g, line, x, y) {
  const t = scn.terrainAt(g.map, x, y);
  if (t !== move.WATER && t !== move.SHORE && t !== move.MOUNTAINS) return null;
  if (!line.some((a) => a.type === armytype.HERO)) return null;
  for (const a of line) if (!a.atSea && g.types.byId[a.type].flies) return a;
  return null;
}

/** Sort a line by its owner's fight order, lowest first, ties keeping the
 *  order it was gathered in (an insertion sort, 6a89:05f1). */
function sortLine(g, line, ownerIndex, carrier) {
  const rank = fightOrder(g, ownerIndex);
  const key = new Map();
  line.forEach((a, i) => key.set(a, [rank(a) + (a === carrier ? 80 : 0), i]));
  luaSort(line, (p, q) => {
    const kp = key.get(p), kq = key.get(q);
    if (kp[0] !== kq[0]) return kp[0] < kq[0];
    return kp[1] < kq[1];
  });
  return line;
}

/** Build both battle lines for an attack on (x, y):
 *  [attackers, defenders, defOwner, city]. The defenders are every other
 *  side's army on the tile -- on a city, anywhere in its 2x2 -- gathered from
 *  the top of the army table down (combat_setup, 6a89:008b). */
export function lines(g, stack, x, y) {
  const attacker = stack[0] ? stack[0].owner : undefined;
  // only a standing city is fought for as one (its terrain is 10, which
  // combat_setup and after_battle test); ruins are open ground
  let city = g.map.cityTile[y * g.map.width + x];
  if (city && city.razed) city = undefined;
  const x0 = city ? city.x : x, y0 = city ? city.y : y;
  const x1 = city ? city.x + 1 : x, y1 = city ? city.y + 1 : y;
  const defenders = [];
  let defOwner;
  for (let i = g.armies.length - 1; i >= 0; i--) {
    const a = g.armies[i];
    if (!a.transit && a.owner != attacker && a.x >= x0 && a.x <= x1 && a.y >= y0 && a.y <= y1) {
      defenders.push(a);
      if (defOwner == null) defOwner = a.owner;
    }
  }
  const attackers = stack.slice();
  sortLine(g, attackers, attacker, heroCarrier(g, attackers, x, y));
  sortLine(g, defenders, defOwner, heroCarrier(g, defenders, x, y));
  return [attackers, defenders, defOwner == null ? undefined : defOwner, city];
}

function itemTotal(army, itemType) {
  let total = 0;
  for (const it of army.items || []) {
    if (it.type === itemType) total += it.value || 0;
  }
  return total;
}

/** Battle items (+n in battle) carried by one army. */
export function battleItems(army) {
  return itemTotal(army, rules.ITEM_BATTLE);
}

/** Command items carried by one army, a standard counting 1 (6a89:0d71). */
export function commandItems(army) {
  let n = itemTotal(army, rules.ITEM_COMMAND);
  for (const it of army.items || []) if (it.type === rules.ITEM_STANDARD) n++;
  return n;
}

/** The side's hero bonus: the table lookup on its strongest hero, plus every
 *  command item it carries. */
export function heroBonus(g, line) {
  let best = 0, command = 0;
  for (const a of line) {
    if (a.type === armytype.HERO) {
      const str = (a.strength || 0) + battleItems(a);
      best = Math.max(best, Math.min(9, str));
      command += commandItems(a);
    }
  }
  return HERO_TABLE[best] + command;
}

function lineHas(g, line, ability) {
  return line.some((a) => g.types.byId[a.type].bonus[ABILITY] === ability);
}

function stackBonus(g, line, cls) {
  let best = 0;
  for (const a of line) best = Math.max(best, g.types.byId[a.type].bonus[BONUS_STACK + 2 * cls] || 0);
  return best;
}

function subtract(g, line) {
  let least = 0;
  for (const a of line) least = Math.min(least, g.types.byId[a.type].bonus[SUBTRACT] || 0);
  return least;
}

/** The city's fortification bonus for the defender, or 0. */
export function fortify(g, attackers, x, y, cls) {
  if (cls !== CITY) return 0;
  if (lineHas(g, attackers, SIEGE)) return 0;
  let value;
  if (g.towerAt && g.towerAt[y * g.map.width + x]) {
    value = 1;
  } else if (scn.terrainAt(g.map, x, y) === move.SITE) {
    value = 2;
  } else {
    const c = g.map.cityTile[y * g.map.width + x];
    value = c ? (c.defence || 0) : 0;
  }
  const city = g.map.cityTile[y * g.map.width + x];
  if (city && city.ownerIndex == null) value = Math.floor(value / 2);   // neutral
  return value;
}

/** Both sides' modifiers: [attackMod, defendMod]. */
export function modifiers(g, attackers, defenders, x, y, cls) {
  const cap = g.map.combatCap != null ? g.map.combatCap : 5;
  const atkHero = lineHas(g, defenders, NEGATE_HERO) ? 0 : heroBonus(g, attackers);
  const atkStack = lineHas(g, defenders, NEGATE_OTHER) ? 0 : stackBonus(g, attackers, cls);
  const defHero = lineHas(g, attackers, NEGATE_HERO) ? 0 : heroBonus(g, defenders);
  const defStack = lineHas(g, attackers, NEGATE_OTHER) ? 0 : stackBonus(g, defenders, cls);
  const attackMod = Math.min(cap, atkHero + atkStack) + subtract(g, defenders);
  const defendMod = Math.min(cap, defHero + defStack + fortify(g, attackers, x, y, cls))
                  + subtract(g, attackers);
  return [attackMod, defendMod];
}

/** One army's fighting strength. A boat at sea on water or shore is 4. */
export function strength(g, army, mod, cls, terrain) {
  if (army.atSea && (terrain === move.WATER || terrain === move.SHORE)) return 4;
  const s = (army.strength || 0) + mod + battleItems(army)
          + (g.types.byId[army.type].bonus[BONUS_SELF + 2 * cls] || 0);
  return Math.min(MAX_STRENGTH, s);
}

/** What View > Stack shows beside each army of the group that moves: its
 *  strength in a fight on (x, y) (89e0:1b9c). Returns a Map army -> strength. */
export function stackStrengths(g, armies, x, y) {
  const cls = terrainClass(g, x, y);
  let total = 0, best = 0;
  for (const a of armies) {
    if (!a.atSea) {
      if (a.type === armytype.HERO) {
        const s = Math.min(9, (a.strength || 0) + battleItems(a));
        total += HERO_TABLE[s] + commandItems(a);
      } else {
        const v = g.types.byId[a.type].bonus[BONUS_STACK + 2 * cls] || 0;
        if (v > best) { total = total + v - best; best = v; }
      }
    }
  }
  total = Math.min(total, g.map.combatCap != null ? g.map.combatCap : 5);
  const out = new Map();
  for (const a of armies) {
    let s;
    if (a.atSea) s = 4;
    else if (a.type === armytype.HERO) s = (a.strength || 0) + battleItems(a);
    else s = (a.strength || 0) + (g.types.byId[a.type].bonus[BONUS_SELF + 2 * cls] || 0);
    out.set(a, Math.min(9, s) + total);
  }
  return out;
}

/** Fight a battle. The armies are not removed; the caller applies the
 *  outcome (game.applyAttack). log: 0 = a defender died, 1 = an attacker. */
export function resolve(g, attackers, defenders, x, y) {
  const cls = terrainClass(g, x, y);
  const terrain = scn.terrainAt(g.map, x, y);
  const [attackMod, defendMod] = modifiers(g, attackers, defenders, x, y, cls);
  const die = g.map.options.intenseCombat !== 0 ? DIE_INTENSE : DIE;

  const prepare = (line, mod) => line.map((a) => ({
    army: a,
    strength: Math.max(1, strength(g, a, mod, cls, terrain)),
    hits: HIT_POINTS - 1,     // dies when this drops below 0
  }));

  const atk = prepare(attackers, attackMod), def = prepare(defenders, defendMod);
  // an army at sea on water or shore fights as a boat, strength 4
  // (DS:029c / DS:02a4); Triumphs counts it a navy
  const boats = new Set();
  for (const a of attackers.concat(defenders)) {
    if (a.atSea && (terrain === move.WATER || terrain === move.SHORE)) boats.add(a);
  }
  const log = [];
  let ai = 0, di = 0, throws = 0;
  while (atk[ai] && def[di]) {
    const a = atk[ai], d = def[di];
    const ra = g.rng.dice(1, die, 0), rd = g.rng.dice(1, die, 0);
    let aHits = ra <= a.strength && rd > d.strength;
    let dHits = rd <= d.strength && ra > a.strength;
    // past 10,000 throws in one duel the attacker takes the hit; the count
    // starts again with each army that falls (67cc:08a6)
    if (throws > STALEMATE) { aHits = false; dHits = true; }
    throws++;

    // Tutorial: a human player's hero cannot die attacking a neutral defender.
    if (dHits && g.map.options.tutorial !== 0 && a.army.type === armytype.HERO
        && !(g.map.sides[a.army.owner || 0] || {}).computer && d.army.owner == null) {
      dHits = false;
    }
    if (aHits) {
      d.hits--;
      if (d.hits < 0) { d.dead = true; log.push(0); di++; throws = 0; }
    } else if (dHits) {
      a.hits--;
      if (a.hits < 0) { a.dead = true; log.push(1); ai++; throws = 0; }
    }
  }

  const survivors = (line) => line.filter((e) => !e.dead).map((e) => e.army);
  const deadAttackers = atk.filter((e) => e.dead).map((e) => e.army);
  const deadDefenders = def.filter((e) => e.dead).map((e) => e.army);
  const deadByArmy = new Set([...deadAttackers, ...deadDefenders]);

  return {
    won: survivors(def).length === 0,
    deadByArmy,
    log,
    attackMod, defendMod, class: cls, boats,
    defStrength0: defenders[0] ? strength(g, defenders[0], defendMod, cls, terrain) : 0,
    attackers: survivors(atk), defenders: survivors(def),
    deadAttackers, deadDefenders,
  };
}

/** The Military Advisor's verdict: 19 simulated battles, wins / 2 into
 *  STRING.DAT group 126 (military_advisor, 67cc:1f19). */
export const ADVICE = [
  "complete and utter suicide!",
  "sheerest folly! Thou shouldst not attack!",
  "a foolish decision!",
  "a brave choice! I leave it to thee!",
  "difficult but not impossible to win!",
  "very evenly matched!",
  "a hard-fought victory! But we shall win!",
  "a comfortable victory!",
  "an easy victory! We cannot lose!",
  "as simple as butchering sleeping cattle!",
];
export const ADVICE_BATTLES = 19;

export function advise(g, attackers, defenders, x, y) {
  let wins = 0;
  for (let i = 0; i < ADVICE_BATTLES; i++) {
    if (resolve(g, attackers, defenders, x, y).won) wins++;
  }
  return [ADVICE[Math.floor(wins / 2)], wins];
}
