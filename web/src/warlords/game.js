// Game state and the turn loop.
//
// Headless on purpose: this module never touches the page, so the rules can
// be run and checked under node (web/test/). Rendering and input sit on top.

import * as armytype from "./armytype.js";
import * as rules from "./rules.js";
import { Rng } from "./rng.js";
import * as scn from "./scn.js";
import * as move from "./move.js";
import * as diplomacy from "./diplomacy.js";
import * as heroMod from "./hero.js";
import * as site from "./site.js";
import * as quest from "./quest.js";
import * as history from "./history.js";
import * as combat from "./combat.js";
import * as ai from "./ai.js";

const TRANSIT_TURNS = 2;       // a vectored army arrives two turns later
const STANDARD_DEST = -2;      // vector destination meaning "the side's standard"
export const STANDARD = STANDARD_DEST;

const key = (g, x, y) => y * g.map.width + x;

/** Every army standing on a tile (in transit armies are nowhere). */
export function armiesAt(g, x, y) {
  const out = [];
  for (const a of g.armies) if (!a.transit && a.x === x && a.y === y) out.push(a);
  return out;
}

/** The city standing on a tile, anywhere in its 2x2 footprint. */
export function cityAt(g, x, y) {
  return g.map.cityTile[key(g, x, y)];
}

/** Give a city to a side (null for neutral) and drop the cached cost grids. */
export function setCityOwner(g, city, sideIndex) {
  city.ownerIndex = sideIndex;
  move.invalidate(g);
}

export function sideCities(g, side) {
  const out = [];
  for (const c of g.map.cities) if (c.ownerIndex === side.index) out.push(c);
  return out;
}

export function sideArmies(g, side) {
  return g.armies.filter((a) => a.owner === side.index);
}

/** Items carried by a side's heroes, summed by effect type. */
export function itemBonus(g, side, itemType) {
  let total = 0;
  for (const a of g.armies) {
    if (a.owner === side.index && a.items) {
      for (const it of a.items) if (it.type === itemType) total += Math.max(1, it.value);
    }
  }
  return total;
}

/** A tile in the city with room for another army: the four tiles of its
 *  footprint in order, then -- if `spill` -- up to 20 random tries within one
 *  step. Returns [x, y], or [null, null] (FUN_6f8c_0bff). */
export function freeTileIn(g, city, spill) {
  for (let dx = 0; dx <= 1; dx++) {
    for (let dy = 0; dy <= 1; dy++) {
      const x = city.x + dx, y = city.y + dy;
      if (x < g.map.width && y < g.map.height && armiesAt(g, x, y).length < rules.MAX_STACK) {
        return [x, y];
      }
    }
  }
  if (!spill) return [null, null];
  for (let i = 0; i < 20; i++) {
    const x = city.x + g.rng.dice(1, 3, -2);
    const y = city.y + g.rng.dice(1, 3, -2);
    if (x >= 0 && y >= 0 && x < g.map.width && y < g.map.height
        && armiesAt(g, x, y).length < rules.MAX_STACK) {
      return [x, y];
    }
  }
  return [null, null];
}

/** docs/rules.md > Start of a side's turn, step 4. */
export function income(g, side) {
  let total = 0;
  const cities = sideCities(g, side);
  for (const c of cities) total += c.income;
  const perCity = itemBonus(g, side, rules.ITEM_GOLD_PER_CITY);
  return total + cities.length * perCity;
}

export function upkeep(g, side) {
  let total = 0;
  for (const a of g.armies) {
    if (a.owner === side.index && !a.transit) {
      let u = a.upkeep || 0;
      if (a.atSea) u = Math.max(rules.SEA_MIN_UPKEEP, u);
      total += u;
    }
  }
  return total;
}

function placeArmy(g, army) {
  g.armies.push(army);
  return army;
}

/** One starting army per city (docs/rules.md > Starting garrisons). */
function setupGarrisons(g) {
  for (const c of g.map.cities) {
    const owned = c.ownerIndex != null;
    const level = rules.garrisonLevel(owned, g.map.options.neutralCities, g.rng);
    let slot = null;
    if (level != null && c.slots.length > 0) {
      const side = owned ? g.map.sides[c.ownerIndex] : null;
      slot = rules.bestSlot(c.slots, rules.GARRISON_PURPOSE[level], g.types, side && side.enhanced);
    }
    if (slot) {
      const a = rules.armyFromSlot(slot, owned && g.map.sides[c.ownerIndex].enhanced);
      a.x = c.x; a.y = c.y; a.owner = c.ownerIndex; a.moves = 0; a.homeCity = c.index;
      placeArmy(g, a);
    } else {
      // Neutral Cities off: a placeholder Scouts army of strength 1.
      const a = g.types.byId[armytype.SCOUTS];
      placeArmy(g, {
        x: c.x, y: c.y, owner: undefined, type: armytype.SCOUTS, name: a.name,
        strength: 1, maxMoves: a.move, moves: 0, upkeep: 0, homeCity: c.index,
      });
    }
  }
}

/** Quick Start: deal every neutral city out to the sides (setup_capitals,
 *  79fa:07ca). */
function dealCities(g) {
  const from = {};
  let turn = null;
  for (let i = 7; i >= 0; i--) {
    const s = g.map.sides[i];
    if (s && s.inUse) {
      from[i] = [s.capX, s.capY];
      turn = i;
    }
  }
  if (turn === null) return;
  for (;;) {
    const [x, y] = from[turn];
    let pick = null, best = 1000;
    for (const c of g.map.cities) {
      if (c.ownerIndex == null) {
        const dx = x - c.x, dy = y - c.y;
        const d = Math.floor(Math.sqrt(dx * dx + dy * dy));
        if (d < best) { pick = c; best = d; }
      }
    }
    if (!pick) return;
    const side = g.map.sides[turn];
    pick.owner = side; pick.ownerIndex = turn;
    if (g.rng.dice(1, 10, -1) < 5) from[turn] = [side.capX, side.capY];
    else from[turn] = [pick.x, pick.y];
    do { turn = (turn + 1) % 8; } while (!from[turn]);
  }
}

/** Build a fresh game from the original data files. */
export function newGame(dataDir, scenario, opts = {}) {
  const g = {
    rng: new Rng(opts.seed || 0),
    dataDir,
    types: armytype.load(dataDir + "/TERRAIN0/ARMYTYPE.DAT"),
    map: scn.load(dataDir + "/" + scenario, scenario),
    armies: [],
    turn: 1,
    log: [],
    greatest: opts.greatest || undefined,
  };
  if (opts.options) for (const k in opts.options) g.map.options[k] = opts.options[k];
  if (opts.sides) {
    for (const s of g.map.sides) {
      const o = opts.sides[s.index];
      if (o && s.inUse) {
        if (o.name) s.name = o.name;          // retyped on the setup screen
        if (o.off) {
          if (s.capital) s.capital.owner = undefined;
          s.inUse = false;
        } else {
          s.computer = !!o.computer;
          s.level = o.level != null ? o.level : s.level;
          s.card = o.card || 0;
        }
      }
    }
  }

  g.sides = [];
  for (const s of g.map.sides) {
    s.alive = s.inUse;
    s.ownerIndex = s.index;
    if (s.inUse) g.sides.push(s);
  }
  // 79fa:0000: every side is observed unless the map is hidden and a human plays
  let humans = 0;
  for (const s of g.sides) if (!s.computer) humans++;
  const observed = !(g.map.options.hiddenMap !== 0 && humans >= 1);
  for (const s of g.sides) s.observe = observed;

  for (const c of g.map.cities) c.ownerIndex = c.owner ? c.owner.index : undefined;
  if (g.map.options.quickStart !== 0) dealCities(g);

  for (const c of g.map.cities) {
    c.slots = rules.citySlots(c.produces, g.types, g.rng);
    c.defence = rules.cityDefence(c.slots.length);
    c.producing = undefined;       // slot index being built (0-based)
    c.countdown = 0;
    c.vectorTo = undefined;
  }
  scn.refreshCityTiles(g.map);

  // items 0-7 are the eight sides' standards (docs/rules.md > Heroes)
  g.map.items.forEach((it, i) => {
    if (i < 8 && it.type === rules.ITEM_STANDARD) it.standardOf = i;
  });

  diplomacy.init(g);
  setupGarrisons(g);
  site.setup(g);
  for (const s of g.sides) revealStart(g, s);
  ai.startGame(g);

  g.current = 0;            // index into g.sides
  g.side = g.sides[0];
  return g;
}

/** STRING.DAT group 11 holds five ways of saying a side is gone. */
export const FALLEN_LINES = 5;

/** Put a side with no city left out of the game (8065:18ab): its heroes drop
 *  what they carry, its armies are gone, it is vanquished, and its gold is 0.
 *  Every side then stands uneasy with it, both ways, state and proposal.
 *  Returns how it is announced: a line of group 11, picked at random, in a
 *  box when `boxed`, otherwise in the status bar. */
function eliminate(g, side, boxed) {
  const mine = [];
  for (let i = g.armies.length - 1; i >= 0; i--) {
    if (g.armies[i].owner === side.index) mine.push(g.armies[i]);
  }
  disband(g, side, mine);
  history.deed(g, side, history.VANQUISHED, side.index, 0, "");     // 8065:19f1
  const line = g.rng.dice(1, FALLEN_LINES, -1);
  g.log.push(`${side.name} has been eliminated.`);
  side.alive = false;
  side.gold = 0;
  if (g.diplomacy) {
    for (let o = 0; o < 8; o++) {
      if (o === side.index) continue;
      for (const k of [side.index * 8 + o, o * 8 + side.index]) {
        g.diplomacy.state[k] = diplomacy.INTERMEDIATE;
        g.diplomacy.proposal[k] = diplomacy.INTERMEDIATE;
      }
    }
  }
  return { side, line, boxed };
}

/** Every side in the game left without a city is put out of it, in side
 *  order (8065:18ab). Its fall is told in a box while a human plays or when
 *  the side was a human's; otherwise in the status bar. */
export function eliminateFallen(g) {
  const human = g.sides.some((s) => s.alive && !s.computer);
  const fallen = [];
  for (const s of g.sides) {
    if (s.alive && sideCities(g, s).length === 0) fallen.push(eliminate(g, s, human || !s.computer));
  }
  return fallen;
}

function applyIncome(g, side) {
  side.income = income(g, side);
  side.upkeepTotal = upkeep(g, side);
  side.gold = Math.max(0, side.gold + side.income - side.upkeepTotal);
}

/** Place a produced army, or send it back; null if it was disbanded. */
function deliver(g, army, city) {
  const [x, y] = freeTileIn(g, city, true);
  if (x == null || city.ownerIndex !== army.owner) {
    if (army.returning) return null;      // already heading home: disbanded
    const home = g.map.cities[army.homeCity];
    army.returning = true;
    army.transit = { turns: TRANSIT_TURNS, dest: home.index };
    return army;
  }
  army.x = x; army.y = y; army.transit = undefined; army.returning = undefined;
  army.moves = 0;
  return army;
}

/** Step 5: run every producing city the side owns, logged in side.produced. */
function runProduction(g, side) {
  const arrived = [], built = [];
  // The armies on the road move on first, then the cities build
  // (6f8c:0000), so one sent this turn is not counted down until the next:
  // it arrives two turns later.
  for (const a of g.armies) {
    if (a.transit && a.owner === side.index) {
      a.transit.turns--;
      if (a.transit.turns <= 0) {
        if (a.transit.dest === STANDARD_DEST) {
          const [sx, sy] = standardAt(g, side);
          a.transit = undefined;
          if (sx != null && armiesAt(g, sx, sy).length < rules.MAX_STACK) {
            a.x = sx; a.y = sy; a.moves = 0; a.returning = undefined;
            arrived.push({ kind: "arrived", type: a.type, standard: true });
          } else {
            a.returning = true;
            a.transit = { turns: TRANSIT_TURNS, dest: a.homeCity };
          }
        } else {
          const dest = g.map.cities[a.transit.dest];
          a.transit = undefined;
          if (!deliver(g, a, dest)) a.disbanded = true;
          else if (!a.transit) arrived.push({ kind: "arrived", type: a.type, city: dest.index });
        }
      }
    }
  }

  for (const c of sideCities(g, side)) {
    if (c.producing != null) {
      c.countdown--;
      if (c.countdown <= 0) {
        const vectored = c.vectorTo != null && c.vectorTo !== c.index;
        const [x, y] = freeTileIn(g, c, true);
        if (side.gold <= 0) {
          c.countdown = 0;               // no gold: it waits, built but unpaid
        } else if (!(x != null || vectored)) {
          c.countdown = 0;               // nowhere to stand: it waits too
        } else {
          const slot = c.slots[c.producing];
          const a = rules.armyFromSlot(slot, side.enhanced);
          a.owner = side.index; a.homeCity = c.index; a.moves = 0;
          a.x = x == null ? undefined : x; a.y = y == null ? undefined : y;
          if (vectored) {
            a.transit = { turns: TRANSIT_TURNS, dest: c.vectorTo };
            a.x = undefined; a.y = undefined;
          }
          placeArmy(g, a);
          built.push({ kind: vectored ? "sent" : "built", type: a.type, city: c.index });
          c.countdown = slot.time;       // the city starts the next one
        }
      }
    }
  }

  for (let i = g.armies.length - 1; i >= 0; i--) {
    if (g.armies[i].disbanded) g.armies.splice(i, 1);
  }
  side.produced = arrived;
  for (const e of built) arrived.push(e);
}

/** Step 6: movement reset. */
function resetMovement(g, side) {
  const doubleMove = itemBonus(g, side, rules.ITEM_DOUBLE_MOVE) > 0;
  for (const a of g.armies) {
    // 8c07:0113 wipes both of the cycle's per-turn marks for the side's armies
    if (a.owner === side.index) { a.done = undefined; a.offered = undefined; }
    if (a.owner === side.index && !a.transit) {
      const carry = Math.min(a.moves || 0, rules.MOVE_CARRY);
      let base = a.atSea ? rules.SEA_MOVES : a.maxMoves;
      if (doubleMove && heroWithDoubleMoveAt(g, side, a.x, a.y)) base += a.maxMoves;
      a.moves = Math.min(rules.MAX_MOVE, base + carry);
    }
  }
}

// A stack given the Defend order encamps as its side's next turn opens
// (8c07:0000): its tile gets the tower flag when it still has its full
// movement and stands on plain, forest, hills, a bridge, marsh, the tower
// terrain or a road.
const ENCAMP_ON = new Set([move.PLAIN, move.FOREST, move.HILLS, move.BRIDGE, move.MARSH, move.TOWER]);

function encamp(g, side) {
  for (const a of g.armies) {
    if (a.owner === side.index && a.fortified && !a.transit && a.x != null
        && (a.moves || 0) >= (a.maxMoves || 0)) {
      const road = (scn.roadAt(g.map, a.x, a.y) || 0) % 32 !== 0;
      if (road || ENCAMP_ON.has(scn.terrainAt(g.map, a.x, a.y))) {
        g.towerAt = g.towerAt || {};
        g.towerAt[a.y * g.map.width + a.x] = true;
      }
    }
  }
}

/** Is (x, y) an encampment? */
export function towerAt(g, x, y) {
  return g.towerAt != null && g.towerAt[y * g.map.width + x] === true;
}

/** Take the tower flag off every tile nobody stands on any more. */
export function tidyTowers(g) {
  if (!g.towerAt) return;
  const keys = Object.keys(g.towerAt);
  if (keys.length === 0) return;
  const held = new Set();
  for (const a of g.armies) if (a.x != null && !a.transit) held.add(a.y * g.map.width + a.x);
  for (const k of keys) if (!held.has(Number(k))) delete g.towerAt[k];
}

/** Is one of the side's heroes carrying a double-movement item on this tile? */
export function heroWithDoubleMoveAt(g, side, x, y) {
  for (const a of g.armies) {
    if (a.owner === side.index && !a.transit && a.x === x && a.y === y
        && a.type === armytype.HERO && a.items) {
      for (const it of a.items) if (it.type === rules.ITEM_DOUBLE_MOVE) return true;
    }
  }
  return false;
}

/** Run the start of `side`'s turn; false if the side has no turn to play: a
 *  computer side left with no city (start_of_turn, 8cc6:0000) does nothing
 *  more until the round's end puts it out. A human's turn (8cc6:0259) goes
 *  on regardless, with whatever armies it still has. */
export function startTurn(g, side) {
  tidyExplored(g, side);
  side.diploNews = diplomacy.apply(g, side);
  for (const m of side.diploNews) g.log.push(m);
  if (side.computer && sideCities(g, side).length === 0) return false;
  side.heroOffer = heroMod.offer(g, side);
  heroMod.checkPromotions(g, side);
  const questResult = quest.event(g, side, "turn");
  if (questResult && questResult.failed) g.log.push(`Quest abandoned: ${questResult.failed}.`);
  applyIncome(g, side);
  runProduction(g, side);
  resetMovement(g, side);
  tidyTowers(g);
  encamp(g, side);
  return true;
}

/** Hand the turn to the next living side, starting a new game turn when the
 *  list wraps. Returns the side now to play, or null if the game is over. */
export function endTurn(g) {
  const leaving = g.side;
  if (leaving && leaving.alive && (leaving.computer || !g.won)) diplomacy.scoreUpdate(g, leaving);
  // what the last round's end found is for the turn that followed it only
  g.ending = null;
  for (let n = 0; n < 2 * g.sides.length + 1; n++) {
    g.current++;
    if (g.current >= g.sides.length) {
      g.current = 0;
      g.turn++;
      // the round's end (8065:17f6): the fallen are put out of the game and
      // the end looked for, and then the turn is written to the history
      endRound(g);
      if (g.ending.over) {
        g.side = null;
        return null;
      }
      history.record(g);     // 8065:17f6 -> 6d51:0d60
    }
    const side = g.sides[g.current];
    if (side.alive) {
      g.side = side;
      if (startTurn(g, side)) return side;
    }
  }
  g.side = null;
  return null;
}

/** Start the very first turn of a new game. */
export function begin(g) {
  g.current = 0;
  g.side = g.sides[0];
  if (!startTurn(g, g.side)) return endTurn(g);
  return g.side;
}

function removeArmies(g, dead) {
  const gone = new Set(dead);
  for (let i = g.armies.length - 1; i >= 0; i--) {
    if (gone.has(g.armies[i])) g.armies.splice(i, 1);
  }
}

/** The signpost on a tile, if there is one. */
export function signAt(g, x, y) {
  for (const s of g.map.signs || []) if (s.x === x && s.y === y) return s;
  return undefined;
}

/** Order > Disband (1b62:076a): the armies go, a hero's items drop where it
 *  stood, and a side whose quest hero goes loses the quest. */
export function disband(g, side, armies) {
  for (const a of armies) {
    if (a.type === armytype.HERO) {
      heroMod.dropItems(g, a, a.x, a.y);
      if (side.quest && side.quest.hero === a) side.quest = undefined;
    }
  }
  removeArmies(g, armies);
}

/** Gold taken with a city won from another side. */
export function loot(g, loser) {
  if (!loser) return 0;
  const n = sideCities(g, loser).length;
  const share = n > 1 ? Math.floor(loser.gold / n) : loser.gold;
  return Math.floor(share / 2);
}

/** Fight for a tile, deciding it and nothing more (combat_resolve,
 *  67cc:08a6); game.applyAttack makes it take effect. */
export function decideAttack(g, stack, x, y) {
  const [attackers, defenders, defOwner, city] = combat.lines(g, stack, x, y);
  const result = combat.resolve(g, attackers, defenders, x, y);
  result.lines = { attackers, defenders, city };
  const loser = defOwner != null ? g.map.sides[defOwner] : null;
  if (result.won && city && loser) result.loot = loot(g, loser);
  result.fought = { stack, x, y, defOwner };
  return result;
}

/** What an attack costs, paid before it is fought (attack_tile, 67cc:005a,
 *  then 67cc:0522): every army in the stack loses the tile's movement cost,
 *  2 at least, down to 0. A land stack attacking onto water or shore puts
 *  to sea first -- each army that cannot fly loses all its moves. The
 *  original does that to a boat too; here it is left to land stacks, as a
 *  navy at sea would move by land rules. */
export function chargeAttack(g, stack, x, y) {
  const city = cityAt(g, x, y);
  const t = city && !city.razed ? move.CITY : scn.terrainAt(g.map, x, y);
  const cost = Math.max(2, move.COST[t] || 0);
  const [mode, , , atSea] = move.stackMode(g, stack);
  if (mode === move.LAND && !atSea && (t === move.WATER || t === move.SHORE)) {
    for (const a of stack) {
      const flight = a.type === armytype.HERO && (a.items || []).some((it) => it.type === rules.ITEM_FLIGHT);
      if (!g.types.byId[a.type].flies && !flight) { a.atSea = true; a.moves = 0; }
    }
  }
  for (const a of stack) a.moves = Math.max(0, (a.moves || 0) - cost);
}

/** Fight for a tile and apply the outcome at once. */
export function resolveAttack(g, stack, x, y) {
  return applyAttack(g, decideAttack(g, stack, x, y));
}

/** Let a decided fight take effect (after_battle, 67cc:0a6b). Once only;
 *  returns the result with `captured` set to the city taken. */
export function applyAttack(g, result) {
  const f = result.fought;
  if (!f) return result;
  result.fought = undefined;
  const { stack, x, y, defOwner } = f;
  const attackers = result.lines.attackers, defenders = result.lines.defenders;
  const city = result.lines.city;

  const fallen = (h) => {
    history.deed(g, g.map.sides[h.owner == null ? 8 : h.owner], history.KILLED,
                 city ? city.index : history.IN_BATTLE, 0, h.name);
  };
  const mine = stack[0] ? stack[0].owner : undefined;
  {
    let heroesLost = 0;
    for (const dd of result.deadDefenders) if (dd.type === armytype.HERO) heroesLost++;
    ai.recordBattle(g, defOwner, mine, x, y, heroesLost, result.deadDefenders.length,
                    result.won && defenders.length > 0, city != null);
  }
  for (const a of result.deadAttackers) {
    history.tally(g, a, defOwner);
    if (a.type === armytype.HERO) { heroMod.dropItems(g, a, a.x, a.y); fallen(a); }
  }
  for (const d of result.deadDefenders) {
    history.tally(g, d, mine);
    if (d.type === armytype.HERO) { heroMod.dropItems(g, d, x, y); fallen(d); }
  }
  heroMod.battleExperience(g, attackers, defenders, result, city != null);
  const mySide = g.map.sides[stack[0] ? (stack[0].owner || 0) : 0];
  result.quest = quest.event(g, mySide, "battle", { stack: attackers, killed: result.deadDefenders });

  removeArmies(g, result.deadAttackers);
  removeArmies(g, result.deadDefenders);
  tidyTowers(g);

  if (result.won && city) {
    const winner = g.map.sides[stack[0].owner];
    const loser = defOwner != null ? g.map.sides[defOwner] : null;
    if (loser) {
      const l = result.loot || 0;
      winner.gold += l;
      loser.gold = Math.max(0, loser.gold - 2 * l);
    }
    city.producing = undefined; city.countdown = 0; city.vectorTo = undefined;
    // every city that was sending its armies here stops: no vector, and
    // nothing in production either (67cc:0a6b)
    for (const c of g.map.cities) {
      if (c.vectorTo === city.index) { c.vectorTo = undefined; c.producing = undefined; c.countdown = 0; }
    }
    city.ownerIndex = winner.index;
    scn.setCityTiles(g.map, city);
    move.invalidate(g);
    result.captured = city;
    history.deed(g, winner, history.WON, winner.index, city.index, winner.name);   // 67cc:0af5
    // a computer's quest hero taking its quest city (5e97:038d) razes it
    const handled = winner.computer && ai.questCapture(g, winner, city, result.attackers);
    if (!handled && winner.computer) {
      result.quest = quest.event(g, winner, "occupy", { city, stack: result.attackers }) || result.quest;
    }
  }

  // The survivors walk into the tile they just cleared -- as many as fit.
  if (result.won) {
    if (g.towerAt) delete g.towerAt[y * g.map.width + x];
    let room = rules.MAX_STACK - armiesAt(g, x, y).length;
    for (const a of result.attackers) {
      if (room <= 0) break;
      a.x = x; a.y = y; room--;
    }
  }
  return result;
}

// A production type's "value" is half its purchase price (ARMYTYPE +30).
function slotValue(g, slot) {
  return Math.floor(Math.abs(g.types.byId[slot.type].price) / 2);
}

function recompute(g, city) {
  city.defence = rules.cityDefence(city.slots.length);
  if (city.producing != null && city.producing >= city.slots.length) {
    city.producing = undefined; city.countdown = 0;
  }
}

/** Raise a side's diplomatic score. */
export function addDiploScore(g, side, n) {
  side.diploScore = (side.diploScore || 0) + n;
}

/** Keep the city as it is (city_occupy, 63fa:03fe): ask the quest. */
export function occupy(g, side, city, stack) {
  return quest.event(g, side, "occupy", { city, stack: stack || [] });
}

/** Strip the most expensive production type for gold (63fa:0886):
 *  [gold, lost]. */
export function pillage(g, side, city, stack) {
  if (city.slots.length < 1) return [0, []];
  const slot = city.slots.pop();          // slots are sorted cheapest first
  const gold = slotValue(g, slot);
  const lost = [{ type: slot.type, gold }];
  side.gold += gold;
  addDiploScore(g, side, g.rng.dice(1, 5, 0));
  recompute(g, city);
  quest.event(g, side, "pillage", { city, gold, stack: stack || [] });
  return [gold, lost];
}

/** Strip every production type but the cheapest (63fa:0941): [gold, lost]. */
export function sack(g, side, city, stack) {
  if (city.slots.length < 2) return [0, []];
  let gold = 0;
  const lost = [];
  for (let i = 1; i < city.slots.length; i++) {
    const v = slotValue(g, city.slots[i]);
    lost.push({ type: city.slots[i].type, gold: v });
    gold += v;
  }
  city.slots.length = 1;
  side.gold += gold;
  addDiploScore(g, side, g.rng.dice(1, 10, 5));
  recompute(g, city);
  quest.event(g, side, "pillage", { city, gold, stack: stack || [] });
  return [gold, lost];
}

/** The city becomes ruins (649c:016b). */
function makeRuins(g, city) {
  city.razed = true;
  city.razedBy = city.ownerIndex == null ? 0 : city.ownerIndex;
  city.ownerIndex = undefined;
  city.claim = rules.NEUTRAL;
  city.slots = [];
  city.producing = undefined; city.countdown = 0; city.vectorTo = undefined;
  city.defence = 0;
  city.income = 0;
  for (const c of g.map.cities) if (c.vectorTo === city.index) c.vectorTo = undefined;
  scn.setCityTiles(g.map, city);
  move.invalidate(g);
}

/** Burn the city to the ground. Razing a city as it falls (63fa:0000) costs
 *  1d15+10 and counts towards a raze quest; razing one of its own
 *  (649c:0061, `ownCity`) costs 1d25+25. */
export function raze(g, side, city, stack, ownCity) {
  makeRuins(g, city);
  if (ownCity) {
    addDiploScore(g, side, g.rng.dice(1, 25, 25));
    return;
  }
  addDiploScore(g, side, g.rng.dice(1, 15, 10));
  quest.event(g, side, "raze", { city, stack: stack || [] });
}

/** Order > Resign (7721:1608). */
export function resign(g, side) {
  for (const c of g.map.cities) {
    if (!c.razed && c.ownerIndex === side.index) makeRuins(g, c);
  }
  disband(g, side, sideArmies(g, side));
  if (g.map.options.hiddenMap !== 0) {
    g.explored = g.explored || {};
    const mask = new Uint8Array(g.map.width * g.map.height).fill(1);
    g.explored[side.index] = mask;
  }
}

// Tiles a side has seen: a mask per side.

function maskFor(g, side, make) {
  g.explored = g.explored || {};
  let m = g.explored[side];
  if (!m && make) {
    m = new Uint8Array(g.map.width * g.map.height);
    g.explored[side] = m;
  }
  return m;
}

/** Has this side (the side or its index) seen the tile? Always true with
 *  Hidden Map off. */
export function seen(g, side, x, y) {
  if (g.map.options.hiddenMap === 0) return true;
  if (typeof side === "object" && side !== null) side = side.index;
  const mask = g.explored && g.explored[side];
  if (!mask) return false;
  return mask[y * g.map.width + x] === 1;
}

/** Uncover the tiles around (x, y): 2 out for a flier or on a city, else 1
 *  (FUN_8611_1298). Returns how many were new. */
export function reveal(g, side, x, y, flying) {
  if (g.map.options.hiddenMap === 0) return 0;
  const mask = maskFor(g, side, true);
  const onCity = g.map.cityTile[y * g.map.width + x] !== undefined;
  const r = (flying || onCity) ? 2 : 1;
  let found = 0;
  for (let ty = y - r; ty <= y + r; ty++) {
    for (let tx = x - r; tx <= x + r; tx++) {
      if (tx >= 0 && ty >= 0 && tx < g.map.width && ty < g.map.height) {
        const k = ty * g.map.width + tx;
        if (!mask[k]) { mask[k] = 1; found++; }
      }
    }
  }
  return found;
}

// The hidden map's edges: an unseen tile is drawn with a cell of HIDDEN.PCK,
// picked by which of its eight neighbours are unseen too (8611:0f6f).
export const FOG_DX = [0, 1, 1, 1, 0, -1, -1, -1];
export const FOG_DY = [-1, -1, 0, 1, 1, 1, 0, -1];
export const FOG_CELL = [
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
  4, 4, 4, 4, 4, 4, 4, 13, 4, 4, 4, 4, 8, 8, 8, 9,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
  255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
  255, 11, 255, 11, 255, 11, 255, 1, 255, 11, 255, 11, 255, 11, 255, 1,
  255, 11, 255, 11, 255, 11, 255, 1, 255, 11, 255, 11, 3, 6, 3, 2,
  255, 11, 255, 11, 255, 11, 255, 1, 255, 11, 255, 11, 255, 11, 255, 1,
  4, 5, 4, 5, 4, 5, 4, 0, 4, 5, 4, 5, 8, 7, 8, 14,
];
export const FOG_NONE = 255, FOG_BLACK = 14;

/** The HIDDEN.PCK cell over (x, y) for this side, or null for a tile it has
 *  seen (or one whose edge has no cell). */
export function fogCell(g, side, x, y) {
  if (seen(g, side, x, y)) return null;
  let bits = 0;
  for (let i = 0; i < 8; i++) {
    const nx = x + FOG_DX[i], ny = y + FOG_DY[i];
    if (nx < 0 || ny < 0 || nx >= g.map.width || ny >= g.map.height || !seen(g, side, nx, ny)) {
      bits += 1 << i;
    }
  }
  const cell = FOG_CELL[bits];
  return cell === FOG_NONE ? null : cell;
}

/** The start of a turn tidies the hidden map (8611:1558): twice over, column
 *  by column, an unseen tile whose edge the sheet cannot draw is uncovered. */
export function tidyExplored(g, side) {
  if (g.map.options.hiddenMap === 0) return;
  if (typeof side === "object" && side !== null) side = side.index;
  const mask = maskFor(g, side, true);
  const W = g.map.width, H = g.map.height;
  for (let pass = 0; pass < 2; pass++) {
    for (let x = 0; x < W; x++) {
      for (let y = 0; y < H; y++) {
        const k = y * W + x;
        if (!mask[k]) {
          let bits = 0;
          for (let i = 0; i < 8; i++) {
            const nx = x + FOG_DX[i], ny = y + FOG_DY[i];
            if (nx < 0 || ny < 0 || nx >= W || ny >= H || !mask[ny * W + nx]) bits += 1 << i;
          }
          if (FOG_CELL[bits] === FOG_NONE) mask[k] = 1;
        }
      }
    }
  }
}

/** Uncover everything a side can already see: its cities and its armies. */
export function revealStart(g, side) {
  if (g.map.options.hiddenMap === 0) return;
  for (const c of sideCities(g, side)) reveal(g, side.index, c.x, c.y, false);
  for (const a of sideArmies(g, side)) if (!a.transit) reveal(g, side.index, a.x, a.y, false);
  tidyExplored(g, side);
}

/** The surrender offer taken (8065:1e4e). */
export function acceptSurrender(g, side) {
  for (const s of g.sides) if (s.computer) s.alive = false;
  g.won = true;
  history.deed(g, side, history.VICTORIOUS, side.index, 0, "");
}

/** The round's end (8065:17f6): the sides left without a city are put out
 *  of the game, then the end is looked for. Leaves the finding in g.ending,
 *  with `fallen` (eliminateFallen) for the interface to tell first. */
export function endRound(g) {
  const fallen = eliminateFallen(g);
  const ending = checkEnd(g);
  ending.fallen = fallen;
  if (ending.message) g.log.push(ending.message);
  g.ending = ending;
  return ending;
}

/** Is the game over, or nearly? Run at the round's end, once the fallen are
 *  out (end_game_check, 8065:1aed). Sets g.won and g.surrenderOffered. */
export function checkEnd(g) {
  // a human side out of the game is the computer's from now on, so that its
  // fall is told only once
  const humans = [], computers = [];
  let fallenHuman = false;
  for (const s of g.sides) {
    if (s.alive) {
      if (s.computer) computers.push(s); else humans.push(s);
    } else if (!s.computer) {
      s.computer = true;
      fallenHuman = true;
    }
  }
  let standing = 0;
  for (const c of g.map.cities) if (!c.razed) standing++;

  if (humans.length + computers.length === 0) {
    g.over = true;
    return { over: true, message: "Alas! No more players are left!" };
  }
  const ending = { over: false };
  // the last human has fallen and the computers fight on (8065:1c6f); a game
  // that never had a human said so as it began (group 14) instead
  if (!g.won && humans.length === 0 && fallenHuman && !g.noHumansSaid) {
    g.noHumansSaid = true;
    ending.noHumans = true;
  }
  // a lone human with more than half the cities has won -- once; the game
  // goes on, for the kingdom to be looked over
  if (!g.won && humans.length === 1 && computers.length === 0) {
    const mine = sideCities(g, humans[0]).length;
    if (mine * 2 > standing) {
      g.won = true;
      history.deed(g, humans[0], history.VICTORIOUS, humans[0].index, 0, "");
      ending.won = true;
      ending.winner = humans[0];
      ending.message = `${humans[0].name} rules the world!`;
    }
  }
  // the last computer side has triumphed: it is handed to the player, its
  // turn opens as a human's, and the world can be looked over
  if (!g.won && humans.length === 0 && computers.length === 1) {
    const winner = computers[0];
    g.won = true;
    winner.computer = false;
    history.deed(g, winner, history.VICTORIOUS, winner.index, 0, "");
    ending.triumph = true;
    ending.winner = winner;
    ending.message = `${winner.name} has triumphed!`;
  }
  // offered once, while computers still play
  if (!g.won && humans.length === 1 && computers.length > 0 && !g.surrenderOffered) {
    const mine = sideCities(g, humans[0]).length;
    let biggest = 0;
    for (const s of computers) biggest = Math.max(biggest, sideCities(g, s).length);
    if (mine * 2 > standing && mine > biggest + Math.floor(standing / 8)) {
      g.surrenderOffered = true;
      ending.surrender = true;
      ending.message = "Your enemies offer their surrender!";
    }
  }
  return ending;
}

/** Search whatever the stack is standing on, if anything. */
export function searchHere(g, stack, human) {
  if (stack.length === 0) return null;
  return site.search(g, stack, stack[0].x, stack[0].y, human);
}

/** A line of prose for a search result. */
export function describeSearch(r) {
  if (!r) return null;
  const name = r.site.name;
  switch (r.kind) {
    case "temple":
      return r.blessed > 0 ? `${name} blesses ${r.blessed} of your armies.`
                           : `${name} has blessed them already.`;
    case "no hero": return `${name} can only be searched by a hero.`;
    case "killed": return `Your hero is slain in ${name} by a ${r.monster ? r.monster.name : "guardian"}!`;
    case "gold": return `${name} yields ${r.gold} gold!`;
    case "item": return `Your hero finds the ${r.item ? r.item.name : "treasure"} in ${name}!`;
    case "allies": return `${r.armies.length} ${r.type.name} join you at ${name}!`;
    case "sage": return `A sage dwells in ${name}.`;
  }
  return `${name} holds nothing.`;
}

/** Choose what a city builds: a slot index from 0, or null to stop. */
export function setProduction(g, city, slotIndex) {
  city.producing = slotIndex == null ? undefined : slotIndex;
  city.countdown = slotIndex != null ? city.slots[slotIndex].time : 0;
  if (slotIndex == null) city.vectorTo = undefined;   // vectoring only sticks while building
}

// No more than four cities may send their armies to one destination.
export const MAX_VECTORED_TO = 4;

/** The cities sending what they build to `dest` (7087:0410) -- a city, or
 *  STANDARD for the cities of `side` sending theirs to its standard. */
export function vectoredTo(g, dest, side) {
  const out = [];
  for (const c of g.map.cities) {
    if (dest === STANDARD_DEST) {
      if (c.vectorTo === STANDARD_DEST && side && c.ownerIndex === side.index) out.push(c);
    } else if (c.vectorTo === dest.index && c !== dest) {
      out.push(c);
    }
  }
  return out;
}

/** Where the side's standard is planted: [x, y], or [null, null] (7563:0b47). */
export function standardAt(g, side) {
  const it = g.map.items[side.index];
  if (it && it.status === 1 && it.planted && it.x != null) return [it.x, it.y];
  return [null, null];
}

/** Hero > Plant Flag (7563:09f7). True if it was planted. */
export function plantFlag(g, side, h) {
  const t = scn.terrainAt(g.map, h.x, h.y);
  if (t === move.WATER || t === move.SHORE || t === move.CITY || t === move.SITE) return false;
  for (const it of g.map.items) if (it.planted && it.x === h.x && it.y === h.y) return false;
  const std = g.map.items[side.index];
  const items = h.items || [];
  const i = items.indexOf(std);
  if (i >= 0) {
    items.splice(i, 1);
    std.status = 1; std.x = h.x; std.y = h.y; std.planted = true;
    return true;
  }
  return false;
}

/** Send what a city builds to its side's planted standard. */
export function vectorToStandard(g, city, side) {
  if (standardAt(g, side)[0] == null) return false;
  if (city.vectorTo !== STANDARD_DEST && vectoredTo(g, STANDARD_DEST, side).length >= MAX_VECTORED_TO) {
    return false;
  }
  city.vectorTo = STANDARD_DEST;
  return true;
}

/** Send what a city builds to another city; null (or itself) stops it. */
export function vector(g, city, destCity) {
  if (destCity === city) destCity = null;
  if (destCity && city.vectorTo !== destCity.index
      && vectoredTo(g, destCity).length >= MAX_VECTORED_TO) {
    return false;
  }
  city.vectorTo = destCity ? destCity.index : undefined;
  return true;
}

/** The city nearest a tile by map_distance, the first of equals (828e:04fa). */
export function nearestCity(g, x, y, side, seer) {
  let best = null, bestD = null;
  for (const c of g.map.cities) {
    let ok = side == null || c.ownerIndex === side.index;
    if (ok && seer) {
      ok = seen(g, seer, c.x, c.y) || seen(g, seer, c.x + 1, c.y)
        || seen(g, seer, c.x, c.y + 1) || seen(g, seer, c.x + 1, c.y + 1);
    }
    if (ok) {
      const d = move.distance(x, y, c.x, c.y);
      if (bestD === null || d < bestD) { best = c; bestD = d; }
    }
  }
  return best;
}

/** The army types a side may buy for a city, in ARMYTYPE.DAT's own order. */
export function buyableTypes(g) {
  return g.types.list.filter((a) => a.price >= 0);
}

/** Why a type cannot be bought for this city right now, or null. */
export function cannotBuy(g, side, city, a) {
  for (const slot of city.slots) if (slot.type === a.id) return "has it";
  if (side.gold < a.price) return "too dear";
  return null;
}

/** The slot the Build Production screen starts on (7087:0978), from 0. */
export function buySlot(city) {
  if (city.slots.length < rules.PRODUCTION_SLOTS) return city.slots.length;
  return 0;
}

/** Buy an army type into a city's production slot `n` (from 0; one past the
 *  last makes a new one). buy_production_type, 7087:1299. */
export function buyProduction(g, side, city, n, typeId) {
  const a = g.types.byId[typeId];
  const building = city.producing != null ? city.slots[city.producing] : null;
  city.slots[n] = {
    type: a.id, name: a.name, strength: a.strength,
    time: a.time, cost: a.cost, move: a.move, price: Math.abs(a.price),
  };
  city.defence = rules.cityDefence(city.slots.length);
  side.gold -= a.price;
  if (building && city.slots[city.producing] !== building) {
    city.producing = undefined; city.countdown = 0; city.vectorTo = undefined;
  }
}

/** Put a city's production back in price order (7087:1544): an insertion
 *  sort; what the city builds follows its type. */
export function sortProduction(city) {
  const building = city.producing != null ? city.slots[city.producing] : null;
  const s = city.slots;
  for (let i = 1; i < s.length; i++) {
    let j = i;
    while (j > 0 && s[j].price < s[j - 1].price) {
      [s[j], s[j - 1]] = [s[j - 1], s[j]];
      j--;
    }
  }
  if (building) s.forEach((slot, i) => { if (slot === building) city.producing = i; });
}

/** Give a city a new name (Rename, 7204:204d). */
export function renameCity(g, city, name) {
  city.name = name;
}
