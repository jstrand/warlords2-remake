// Headless tests for the rules core: the Lua remake's love2d/test/run.lua,
// ported. No browser, no graphics.
//
//     node web/test/run.js            -- from the repository root
//
// Every assertion cites the rule it checks; docs/rules.md is the spec. Where
// the Lua counts from 1 these count from 0, so a city's first production
// slot is slot 0 here.

import * as fs from "node:fs";
import { loadData } from "./node.js";
import * as armytype from "../src/warlords/armytype.js";
import * as game from "../src/warlords/game.js";
import * as movement from "../src/warlords/move.js";
import * as combat from "../src/warlords/combat.js";
import * as rules from "../src/warlords/rules.js";
import { Rng } from "../src/warlords/rng.js";
import * as scn from "../src/warlords/scn.js";
import * as report from "../src/warlords/report.js";
import * as hero from "../src/warlords/hero.js";
import * as quest from "../src/warlords/quest.js";
import * as history from "../src/warlords/history.js";
import * as site from "../src/warlords/site.js";
import * as saveMod from "../src/warlords/save.js";
import * as diplomacy from "../src/warlords/diplomacy.js";
import * as slots from "../src/warlords/slots.js";
import * as ai from "../src/warlords/ai.js";
import * as aicard from "../src/warlords/aicard.js";
import * as cues from "../src/warlords/cues.js";
import * as uidata from "../src/warlords/uidata.js";
import * as aicore from "../src/warlords/ai/core.js";
import * as aigroups from "../src/warlords/ai/groups.js";
import * as aidiplomacy from "../src/warlords/ai/diplomacy.js";
import * as vfs from "../src/vfs.js";
import { fmt } from "../src/util.js";

loadData();
const DATA = "";
const SCENARIOS = ["ERYTHEA", "ILLURIA", "DRAGON", "HADESHA", "ISLADIA", "SORCERY", "TUTORIA"];

let passed = 0, failed = 0;
const nil = (v) => (v === undefined ? null : v);

export function ok(cond, what) {
  if (cond) passed++;
  else { failed++; console.log("  FAIL  " + what); }
}

/** Equal, with null and undefined both standing for Lua's nil. */
export function eq(got, want, what) {
  if (nil(got) === nil(want)) passed++;
  else { failed++; console.log(`  FAIL  ${what}: got ${got}, want ${want}`); }
}

const newGame = (scenario, opts) => game.newGame(DATA, scenario, opts);
const removeArmy = (g, a) => { const i = g.armies.indexOf(a); if (i >= 0) g.armies.splice(i, 1); };

// ------------------------------------------------------------------ dice

function testDice() {
  console.log("dice");
  const r = new Rng(1234);
  let lo = Infinity, hi = -Infinity;
  for (let i = 0; i < 20000; i++) {
    const v = r.dice(1, 100, 0);
    lo = Math.min(lo, v); hi = Math.max(hi, v);
  }
  eq(lo, 1, "1d100 lower bound");
  eq(hi, 100, "1d100 upper bound");
  lo = Infinity; hi = -Infinity;
  for (let i = 0; i < 20000; i++) {
    const v = r.dice(3, 500, 500);
    lo = Math.min(lo, v); hi = Math.max(hi, v);
  }
  ok(lo >= 503, "3d500+500 lower bound");
  ok(hi <= 2000, "3d500+500 upper bound");
  const seen = {};
  for (let i = 0; i < 2000; i++) seen[r.dice(1, 5, -1)] = true;
  for (let i = 0; i <= 4; i++) ok(seen[i], "dice(1,5,-1) reaches " + i);
  ok(!seen[5], "dice(1,5,-1) stays below 5");
  const a = new Rng(7), b = new Rng(7);
  let same = true;
  for (let i = 0; i < 100; i++) if (a.dice(2, 6, 0) !== b.dice(2, 6, 0)) same = false;
  ok(same, "a seed reproduces the sequence");
}

function testArmyTypes() {
  console.log("ARMYTYPE.DAT");
  const t = armytype.load(DATA + "/TERRAIN0/ARMYTYPE.DAT");
  eq(t.list.length, 29, "29 army types");
  ok(t.byId[armytype.HERO] != null, "type 28 exists");
  eq(t.byId[armytype.HERO].name, "Hero", "type 28 is the Hero");
  eq(t.byId[armytype.SCOUTS].name, "Scouts", "type 11 is Scouts");
  const hcav = t.list.find((a) => a.name === "Heavy Cav.");
  ok(hcav != null, "Heavy Cav. is in the table");
  eq(hcav.strength, 4, "Heavy Cav. strength");
  eq(hcav.time, 3, "Heavy Cav. production time");
  eq(hcav.cost, 8, "Heavy Cav. cost");
  eq(hcav.move, 16, "Heavy Cav. base move");
  ok(t.list.some((a) => a.flies), "some types fly");
}

function testProduction() {
  console.log("production slots and garrisons");
  const t = armytype.load(DATA + "/TERRAIN0/ARMYTYPE.DAT");
  const r = new Rng(42);
  const sl = rules.citySlots([4, 11, 2], t, r);
  eq(sl.length, 3, "one slot per production type");
  let sorted = true;
  for (let i = 1; i < sl.length; i++) if (sl[i].price < sl[i - 1].price) sorted = false;
  ok(sorted, "slots are sorted by price");
  let okBounds = true;
  for (let n = 1; n <= 2000; n++) {
    for (const s of rules.citySlots([1, 2, 3, 4], t, new Rng(n))) {
      const base = t.byId[s.type];
      if (s.strength < 1 || s.strength > 9) okBounds = false;
      if (s.strength < base.strength - 1 || s.strength > base.strength + 1) okBounds = false;
      if (s.move < 6) okBounds = false;
      if (s.move > base.move + 4) okBounds = false;
      if (s.time < 1 || s.time > base.time + 1) okBounds = false;
    }
  }
  ok(okBounds, "nudged stats stay within the documented range");
  const anyFlier = sl.some((s) => t.byId[s.type].flies);
  const best4 = rules.bestSlot(sl, 4, t, false);
  if (anyFlier) ok(best4 != null, "purpose 4 finds a flier");
  else ok(best4 == null, "purpose 4 refuses a city with no fliers");
  const fast = rules.bestSlot(sl, 6, t, false);
  const strong = rules.bestSlot(sl, 3, t, false);
  ok(fast != null && strong != null, "purposes 3 and 6 always choose something");
  eq(rules.cityDefence(2), 1, "fewer than 3 types: defence 1");
  eq(rules.cityDefence(3), 2, "3 types: defence 2");
  eq(rules.cityDefence(4), 2, "4 types: defence 2");
  const slot = { type: 4, name: "x", strength: 8, time: 2, cost: 8, move: 12 };
  eq(rules.armyFromSlot(slot, true).strength, 9, "Enhanced caps strength at 9");
  eq(rules.armyFromSlot(slot, false).strength, 8, "no Enhanced, no bonus");
  eq(rules.armyFromSlot(slot, false).upkeep, 4, "upkeep is half the slot cost");
}

function testGame(scenario) {
  console.log("game: " + scenario);
  const g = newGame(scenario, { seed: 3 });
  ok(g.sides.length >= 2, "at least two sides in play");
  eq(g.armies.length, g.map.cities.length, "exactly one starting army per city");
  for (const s of g.sides) {
    eq(game.sideCities(g, s).length, 1, s.name + " owns only its capital");
    eq(game.sideCities(g, s)[0].index, s.capital.index, s.name + "'s city is its capital");
  }
  for (const c of g.map.cities) {
    for (const slot of c.slots) ok(slot.type !== armytype.NAVY, c.name + " does not produce Navy");
    eq(c.defence, rules.cityDefence(c.slots.length), c.name + " defence");
    ok(c.slots.length <= 4, c.name + " has at most 4 slots");
  }
  for (const a of g.armies) {
    ok(g.types.byId[a.type] != null, "army has a real type");
    eq(a.moves, 0, "a starting garrison has no moves");
  }
  const perTile = {};
  for (const a of g.armies) {
    const k = a.y * g.map.width + a.x;
    perTile[k] = (perTile[k] || 0) + 1;
    ok(perTile[k] <= rules.MAX_STACK, "stack limit holds");
  }
  const h = newGame(scenario, { seed: 3 });
  let same = h.armies.length === g.armies.length;
  g.armies.forEach((a, i) => {
    const b = h.armies[i];
    if (!b || b.type !== a.type || b.strength !== a.strength) same = false;
  });
  ok(same, "the same seed rebuilds the same game");
}

function testTurnLoop(scenario) {
  console.log("turn loop: " + scenario);
  const g = newGame(scenario, { seed: 5 });
  const side = game.begin(g);
  ok(side != null, "the first side is to play");
  for (const a of game.sideArmies(g, side)) eq(a.moves, a.maxMoves, "a garrison starts the turn with its full move");
  const gold0 = side.gold;
  eq(side.income, side.capital.income, "income is the capital's income");
  ok(side.gold === Math.max(0, gold0), "gold was applied");
  {
    const one = game.sideArmies(g, side)[0];
    one.done = true; one.offered = true; one.fortified = true;
    for (let i = 0; i < g.sides.length; i++) game.endTurn(g);
    eq(one.done, null, "done for this turn wears off with the turn");
    eq(one.offered, null, "and so does having been offered this pass");
    eq(one.fortified, true, "but digging in outlasts it");
    one.fortified = undefined;
  }
  const a = game.sideArmies(g, side)[0];
  a.moves = 5;
  const before = a.maxMoves;
  for (let i = 0; i < g.sides.length; i++) game.endTurn(g);
  eq(a.moves, before + rules.MOVE_CARRY, "at most 2 unused moves carry over");
  a.moves = 1;
  for (let i = 0; i < g.sides.length; i++) game.endTurn(g);
  eq(a.moves, before + 1, "carry-over below the cap is exact");

  const city = side.capital;
  game.setProduction(g, city, 0);
  const want = city.slots[0].time;
  const n0 = game.sideArmies(g, side).length;
  for (let i = 0; i < want * g.sides.length; i++) game.endTurn(g);
  ok(game.sideArmies(g, side).length === n0 + 1, fmt("%s produced after %d turns", city.name, want));
  let built = null;
  for (const army of game.sideArmies(g, side)) if (army.homeCity === city.index && army !== a) built = army;
  ok(built != null, "the new army belongs to the city that built it");
  if (built) {
    eq(built.upkeep, Math.floor(city.slots[0].cost / 2), "upkeep is half the slot cost");
    eq(built.x, city.x, "the new army stands in its city");
  }
  // a side with no cities stays in play until the round ends (8065:18ab),
  // then goes, with its armies, its gold and its diplomacy
  const victim = g.sides[g.sides.length - 1];
  for (const c of game.sideCities(g, victim)) c.ownerIndex = undefined;
  victim.gold = 500;
  g.diplomacy.state[victim.index * 8] = diplomacy.WAR;
  ok(game.sideArmies(g, victim).length > 0, "the beaten side still has armies");
  const roundOf = g.turn;
  while (g.turn === roundOf) {
    ok(victim.alive, "a side with no cities is in play until the round ends");
    game.endTurn(g);
  }
  ok(!victim.alive, "a side with no cities is eliminated as the round ends");
  eq(game.sideArmies(g, victim).length, 0, "the eliminated side's armies are gone");
  eq(victim.gold, 0, "the eliminated side's gold is gone");
  eq(diplomacy.state(g, victim.index, 0), diplomacy.INTERMEDIATE, "the eliminated side's diplomacy is reset to uneasy");
  const fallen = g.ending && g.ending.fallen;
  ok(fallen && fallen[0] && fallen[0].side === victim, "the elimination is announced");
  const g2 = newGame(scenario, { seed: 6 });
  game.begin(g2);
  eq(g2.turn, 1, "the game starts on turn 1");
  for (let i = 0; i < g2.sides.length; i++) game.endTurn(g2);
  eq(g2.turn, 2, "a full round advances the turn");
}

function testVectoring(scenario) {
  console.log("vectoring: " + scenario);
  const g = newGame(scenario, { seed: 9 });
  const side = game.begin(g);
  const from = side.capital;
  const dest = g.map.cities.find((c) => c.ownerIndex == null && c.slots.length > 0);
  ok(dest != null, "found a neutral city to capture");
  if (!dest) return;
  dest.ownerIndex = side.index;
  for (const army of game.armiesAt(g, dest.x, dest.y)) army.owner = side.index;
  game.setProduction(g, from, 0);
  game.vector(g, from, dest);
  const time = from.slots[0].time;
  const atDest = game.armiesAt(g, dest.x, dest.y).length;
  for (let i = 0; i < (time + 2) * g.sides.length; i++) game.endTurn(g);
  ok(game.armiesAt(g, dest.x, dest.y).length > atDest, "a vectored army arrives two turns after it is built");
  game.setProduction(g, from, null);
  ok(from.vectorTo == null, "vectoring only sticks while the city is building");
  const senders = [];
  for (const c of g.map.cities) {
    if (c !== dest && senders.length < 5) {
      c.ownerIndex = side.index;
      senders.push(c);
    }
  }
  for (let i = 0; i < 4; i++) ok(game.vector(g, senders[i], dest), "vector " + (i + 1) + " is taken");
  ok(!game.vector(g, senders[4], dest), "a fifth is refused");
  eq(senders[4].vectorTo, null, "and the fifth city keeps no vector");
  eq(game.vectoredTo(g, dest).length, 4, "four cities send to the destination");
  ok(game.vector(g, senders[0], dest), "a city already sending there may be set again");
  ok(game.vector(g, senders[0], senders[0]), "vectoring a city to itself lifts the vector");
  eq(senders[0].vectorTo, null, "and leaves it with none");
  eq(game.nearestCity(g, dest.x, dest.y, side), dest, "a click on a city picks it");
  eq(game.nearestCity(g, dest.x + 1, dest.y - 1, side), dest, "and a click next to it too");
}

function testBuyProduction(scenario) {
  console.log("buying production: " + scenario);
  const g = newGame(scenario, { seed: 17 });
  const side = game.begin(g);
  const city = side.capital;
  side.gold = 5000;
  const list = game.buyableTypes(g);
  ok(list.length > 0 && list.length <= 20, "the Build Production screen has room for every type");
  for (const a of list) ok(a.price >= 0, a.name + " is for sale");
  eq(game.cannotBuy(g, side, city, g.types.byId[city.slots[0].type]), "has it", "a type already built is greyed out");
  let n0 = city.slots.length;
  eq(game.buySlot(city), n0 < 4 ? n0 : 0, "the starting slot");
  let pick = null;
  for (const a of list) if (!game.cannotBuy(g, side, city, a) && (!pick || a.price > pick.price)) pick = a;
  ok(pick != null, "something to buy");
  if (n0 === 4) { city.slots.pop(); n0 = 3; }
  game.setProduction(g, city, 0);
  const building = city.slots[0];
  const gold = side.gold;
  game.buyProduction(g, side, city, n0, pick.id);
  eq(side.gold, gold - pick.price, "the price comes off the treasury");
  const slot = city.slots[n0];
  eq(slot.type, pick.id, "the bought type fills the slot");
  eq(slot.strength, pick.strength, "a bought type takes ARMYTYPE's strength unmodified");
  eq(slot.time, pick.time, "and its time");
  eq(slot.move, pick.move, "and its move");
  eq(city.defence, rules.cityDefence(city.slots.length), "defence follows the number of types");
  eq(city.producing, 0, "buying into a new slot leaves production alone");
  game.sortProduction(city);
  for (let i = 1; i < city.slots.length; i++) ok(city.slots[i - 1].price <= city.slots[i].price, "sorted by price, cheapest first");
  eq(city.slots[city.producing], building, "the city builds the same type after sorting");
  game.vector(g, city, g.map.cities[0] !== city ? g.map.cities[0] : g.map.cities[1]);
  const other = list.find((a) => !game.cannotBuy(g, side, city, a));
  game.buyProduction(g, side, city, city.producing, other.id);
  eq(city.producing, null, "production stops when its type is bought over");
  eq(city.vectorTo, null, "and the vector goes with it");
  game.renameCity(g, city, "Newtown");
  const h = saveMod.decode(saveMod.encode(g), DATA);
  eq(h.map.cities[city.index].name, "Newtown", "a renamed city keeps its name");
  eq(h.map.cities[city.index].slots[city.slots.length - 1].type, city.slots[city.slots.length - 1].type,
     "bought production survives a save");
}

function testReports(scenario) {
  console.log("reports: " + scenario);
  const g = newGame(scenario, { seed: 23 });
  const side = game.begin(g);
  const f = report.figures(g, side, report.CITY);
  eq(f.result, game.sideCities(g, side).length, "the City report counts the side's cities");
  eq(f.max % 2, 0, "the top of the scale is even");
  let unused = 0;
  for (let i = 0; i < 8; i++) if (f.out[i]) unused++;
  eq(unused, 8 - g.sides.length, "only the sides in the game get a bar");
  const a = report.figures(g, side, report.ARMY);
  const mine = g.armies.filter((army) => army.owner === side.index).length;
  eq(a.result, mine, "the Army report counts the side's armies");
  eq(report.figures(g, side, report.GOLD).result, side.gold, "the Gold report is the treasury");
  const w = report.figures(g, side, report.WINNING);
  eq(w.max, 100, "the Winning report's scale is fixed");
  ok(w.result >= 0 && w.result <= 7, "and says where the side comes");
  side.gold = 100000;
  eq(report.figures(g, side, report.WINNING).result, 0, "a rich enough side comes first");
  eq(report.score(g, side), 500, "and its score stops at 500");
  const city = side.capital;
  game.setProduction(g, city, 0);
  city.countdown = 1;
  let turns = 0;
  do { game.endTurn(g); turns++; } while (!(g.side === side || turns > 20));
  ok(side.produced && side.produced.length >= 1, "the side's production is logged");
  if (side.produced && side.produced[0]) {
    eq(side.produced[0].city, city.index, "with the city that built it");
    eq(side.produced[0].kind, "built", "kept at home");
  }
  eq(report.figures(g, side, report.PRODUCTION).result, side.produced.length, "the Production report counts what was built");
}

function testDisband(scenario) {
  console.log("disband: " + scenario);
  const g = newGame(scenario, { seed: 29 });
  const side = game.begin(g);
  const [h] = hero.recruit(g, side, side.heroOffer);
  const stack = game.armiesAt(g, h.x, h.y);
  h.strength = 5;
  const fight = combat.stackStrengths(g, [h], h.x, h.y);
  eq(fight.get(h), 5 + combat.HERO_TABLE[5] + 1, "View > Stack: a hero's strength, its table value and its standard");
  const n = g.armies.length;
  side.quest = { type: quest.OCCUPY, hero: h };
  const std = h.items[0];
  game.disband(g, side, stack);
  eq(g.armies.length, n - stack.length, "the stack is gone");
  eq(game.armiesAt(g, h.x, h.y).length, 0, "from its tile");
  eq(std.status, 1, "the hero's standard lies on the ground");
  eq(side.quest, null, "and the quest is lost with the hero");
  const sg = game.signAt(g, 18, 12);
  ok(sg, "a sign at (18, 12)");
  eq(sg.lines[0], "Traveller! Pray excuse the ", "its first line");
  eq(sg.lines[1], "sulphur fumes. Keep On!", "and its second");
  sg.lines[0] = "Hello there";
  let back = saveMod.decode(saveMod.encode(g), DATA);
  eq(game.signAt(back, 18, 12).lines[0], "Hello there", "an edited sign survives a save");
  g.map.fightOrder[side.index][3] = 26;
  back = saveMod.decode(saveMod.encode(g), DATA);
  eq(back.map.fightOrder[side.index][3], 26, "so does an edited fight order");
  const diplo = side.diploScore;
  game.resign(g, side);
  eq(game.sideCities(g, side).length, 0, "a side that resigns holds no city");
  eq(game.sideArmies(g, side).length, 0, "and has no army");
  ok(side.capital.razed, "its capital is ruins");
  eq(side.diploScore, diplo, "burning its own on resigning costs nothing");
}

function testHistory(scenario) {
  console.log("history: " + scenario);
  const g = newGame(scenario, { seed: 41 });
  const side = game.begin(g);
  history.deed(g, side, history.FINDS, history.SAGE, 0, "Hero");
  history.deed(g, side, history.WON, side.index, 3, side.name);
  history.deed(g, side, history.EMERGES, 1, 0, "Hero");
  eq(g.deeds[side.index].length, 2, "two deeds a side");
  const types = [g.deeds[side.index][0].type, g.deeds[side.index][1].type].sort((a, b) => a - b);
  eq(types[0], history.EMERGES, "a lower type pushes out the higher");
  eq(types[1], history.WON, "and the lower of the two stays");
  history.deed(g, side, history.PEACE, 0, 1, "");
  eq(g.deeds[side.index].length, 2, "a higher type does not get in");
  let guard = 0;
  while (g.turn < 3 && guard < 50) { game.endTurn(g); guard++; }
  ok(g.history && g.history.length >= 1, "a record for the round");
  const r = g.history[0];
  eq(r.owners.length, g.map.cities.length, "every city's owner");
  ok(r.events.length >= 2, "with the deeds");
  eq(g.deeds[side.index], null, "and the slots cleared");
  const back = saveMod.decode(saveMod.encode(g), DATA);
  eq(back.history.length, g.history.length, "history survives a save");
  const foe = g.sides[1];
  history.tally(g, { owner: foe.index, type: 28, items: [{ index: foe.index }] }, side.index);
  eq(history.triumph(g, side.index, foe.index, history.HEROES), 1, "a hero killed");
  eq(history.triumph(g, side.index, foe.index, history.STANDARDS), 1, "and the standard it carried");
  eq(history.triumph(g, foe.index, foe.index, history.HEROES), 1, "the loser counts its own loss");
}

function testSetup() {
  console.log("new-game setup");
  const g = newGame("ERYTHEA", { seed: 5, options: { quests: 0 },
    sides: { 0: { computer: false }, 1: { computer: true, level: 2 }, 2: { off: true } } });
  eq(g.map.options.quests, 0, "the options chosen");
  ok(!g.map.sides[0].computer, "side 0 human");
  ok(g.map.sides[1].computer && g.map.sides[1].level === 2, "side 1 a Warlord");
  ok(!g.map.sides[2].inUse, "side 2 left out");
  const cap = g.map.sides[2].capital;
  eq(cap ? cap.ownerIndex : null, null, "and its capital neutral");
  for (const s of g.sides) ok(s.index !== 2, "not among the sides in play");
  const quick = newGame("ERYTHEA", { seed: 5, options: { quickStart: 1 }, sides: { 2: { off: true } } });
  const counts = {};
  let neutral = 0;
  for (const c of quick.map.cities) {
    if (c.ownerIndex == null) neutral++;
    else counts[c.ownerIndex] = (counts[c.ownerIndex] || 0) + 1;
  }
  eq(neutral, 0, "with Quick Start no city is left neutral");
  eq(counts[2], null, "a side left out gets none");
  let least = Infinity, most = 0;
  for (const s of quick.sides) {
    least = Math.min(least, counts[s.index] || 0); most = Math.max(most, counts[s.index] || 0);
    ok(s.capital.ownerIndex === s.index, "each side keeps its capital");
  }
  ok(most - least <= 1, "and the cities go round evenly");
  for (const a of quick.armies) {
    const c = game.cityAt(quick, a.x, a.y);
    if (c) eq(a.owner, c.ownerIndex, "a dealt city's garrison is its owner's");
  }
  const slow = newGame("ERYTHEA", { seed: 5, sides: { 2: { off: true } } });
  neutral = slow.map.cities.filter((c) => c.ownerIndex == null).length;
  ok(neutral > 0, "without it the other cities stay neutral");
}

function testSage(scenario) {
  console.log("sage: " + scenario);
  const g = newGame(scenario, { seed: 31 });
  const side = game.begin(g);
  const hx = side.capital.x, hy = side.capital.y;
  for (const s of g.map.sites) { s.rich = false; s.revealed = 0xff; s.searched = false; }
  eq(site.sageList(g, side, hx, hy).length, 0, "no rich site unshown: nothing to tell");
  const near = g.map.sites.filter((s) => Math.sqrt((s.x - hx) ** 2 + (s.y - hy) ** 2) < site.SAGE_RANGE);
  near.sort((a, b) => ((a.x - hx) ** 2 + (a.y - hy) ** 2) - ((b.x - hx) ** 2 + (b.y - hy) ** 2));
  ok(near.length >= 3, "three sites within reach");
  const g1 = near[2], g2 = near[0], isite = near[1];
  for (const s of [g1, g2, isite]) { s.rich = true; s.revealed = 0; }
  g1.content = site.GOLD; g2.content = site.GOLD;
  isite.content = site.ITEM; isite.item = g.map.items[9].index;
  const list = site.sageList(g, side, hx, hy);
  eq(list.length, 2, "gold once however many, and the item");
  const gold = list.find((e) => e.kind === "gold");
  ok(gold, "a gold entry");
  eq(site.sageShow(g, side, gold, hx, hy), g2, "the nearest gold site is shown");
  ok(((g2.revealed >> side.index) & 1) === 1, "and marked shown to the side");
  eq(site.sageShow(g, side, gold, hx, hy), g1, "then the next one");
  const items = site.sageList(g, side, hx, hy);
  eq(items.length, 1, "only the item is left");
  eq(items[0].item, g.map.items[9], "by its record");
  const before = side.gold;
  const n = site.sageGem(g, side);
  ok(n >= 503 && n <= 2000, "a gem is 3d500 + 500");
  eq(side.gold, before + n, "and is paid");
  g.map.options.hiddenMap = 1;
  g.explored = null;
  const [x0, y0, w, h] = site.sageMap(g, side, 3, 3);
  eq(x0, 0, "the patch is kept on the map");
  ok(w >= 16 && w <= 25 && h >= 16 && h <= 25, "16-25 tiles a side");
  ok(game.seen(g, side, x0 + w - 1, y0 + h - 1), "and uncovered");
}

function testHeroItems(scenario) {
  console.log("hero items: " + scenario);
  const g = newGame(scenario, { seed: 29 });
  const side = game.begin(g);
  const [h] = hero.recruit(g, side, side.heroOffer);
  eq(h.items[0], g.map.items[side.index], "the first hero carries the side's own standard record");
  eq(h.items[0].status, 3, "which is carried");
  eq(hero.itemsHere(g, h).length, 1, "nothing else is listed yet");
  const it = g.map.items[8];
  it.status = 1; it.x = h.x; it.y = h.y;
  const list = hero.itemsHere(g, h);
  eq(list.length, 2, "an item on the hero's tile is listed");
  eq(list[1], it, "after what the hero carries");
  hero.takeItem(g, h, it);
  eq(it.status, 3, "taking an item carries it");
  eq(h.items.length, 2, "the hero has it");
  hero.dropItem(g, h, it);
  eq(it.status, 1, "dropping puts it on the ground");
  eq(it.x, h.x, "where the hero stands");
  eq(h.items.length, 1, "and the hero no longer has it");
  ok(!game.plantFlag(g, side, h), "no flag in a city");
  let fx = null, fy = null;
  for (let dy = -3; dy <= 4; dy++) {
    for (let dx = -3; dx <= 4; dx++) {
      const x = h.x + dx, y = h.y + dy;
      const t = scn.terrainAt(g.map, x, y);
      if (fx === null && (t === movement.PLAIN || t === movement.ROAD) && !game.cityAt(g, x, y)) { fx = x; fy = y; }
    }
  }
  ok(fx != null, "found open ground near the capital");
  h.x = fx; h.y = fy;
  ok(game.plantFlag(g, side, h), "the flag is planted");
  const [sx] = game.standardAt(g, side);
  eq(sx, fx, "where the hero stands");
  ok(!game.plantFlag(g, side, h), "and only once");
  const city = side.capital;
  game.setProduction(g, city, 0);
  city.countdown = 1;
  ok(game.vectorToStandard(g, city, side), "a city may vector to the standard");
  const before = game.armiesAt(g, fx, fy).length;
  for (let i = 0; i < 3 * g.sides.length + 1; i++) game.endTurn(g);
  ok(game.armiesAt(g, fx, fy).length > before, "a vectored army arrives at the standard");
  hero.takeItem(g, h, g.map.items[side.index]);
  eq(game.standardAt(g, side)[0], null, "a standard picked up is no longer planted");
}

function testMovement(scenario) {
  console.log("movement: " + scenario);
  const g = newGame(scenario, { seed: 11 });
  const side = game.begin(g);
  const army = game.sideArmies(g, side)[0];
  const stack = [army];
  eq(movement.COST[movement.MOUNTAINS], 0, "mountains are impassable");
  eq(movement.COST[movement.ROAD], 1, "a road costs 1");
  eq(movement.COST[movement.HILLS], 6, "hills cost 6");
  const grid = movement.grid(g, side.index);
  let roaded = 0, roadCostOk = true;
  for (let y = 0; y < g.map.height; y++) {
    for (let x = 0; x < g.map.width; x++) {
      if (scn.roadAt(g.map, x, y) % 0x20 !== 0) {
        roaded++;
        if (grid[y * g.map.width + x] % 8 !== 1) roadCostOk = false;
      }
    }
  }
  ok(roaded > 0, "the scenario has roads");
  ok(roadCostOk, "every road tile costs 1");
  const W = movement.WATER_F;
  const land = 2, water = 2 | W;
  eq(movement.stepCost(land, water, movement.LAND, false, false, 10), null, "a land stack cannot enter open water");
  eq(movement.stepCost(land, 0, movement.LAND, false, false, 10), null, "nothing enters a cost-0 tile on land");
  eq(movement.stepCost(land, 6 | movement.HILLS_F, movement.LAND, false, false, 10), 6, "hills cost 6 without the bonus");
  eq(movement.stepCost(land, 6 | movement.HILLS_F, movement.LAND, false, true, 10), 2, "the hills bonus brings them to 2");
  eq(movement.stepCost(land, 4 | movement.FOREST_F, movement.LAND, true, false, 10), 2, "the woods bonus brings forest to 2");
  const cross = 1 | W | movement.CROSS_F;
  eq(movement.stepCost(land, cross, movement.LAND, false, false, 10), 1, "a crossing tile is free of the water charge");
  eq(movement.stepCost(cross, water, movement.LAND, false, false, 10), 2 + 10, "stepping from a crossing into water costs the penalty");
  eq(movement.stepCost(cross, water, movement.LAND, false, false, 20), 2 + 20, "a move aimed at water pays 20");
  eq(movement.stepCost(land, 0, movement.FLYING, false, false, 10), 2, "a flier crosses mountains at 2");
  eq(movement.stepCost(land, water, movement.FLYING, false, false, 10), 2, "a flier crosses water at 2");
  eq(movement.stepCost(land, 1, movement.FLYING, false, false, 10), 1, "a flier pays 1 on a road");
  eq(movement.stepCost(land, 6 | movement.HILLS_F, movement.FLYING, false, false, 10), 2, "a flier pays 2 over hills");
  eq(movement.stepCost(land, 0 + movement.CITY_F, movement.FLYING, false, false, 10), null, "a flier cannot cross a city that is not its own (1555:08bf)");
  eq(movement.distance(5, 5, 6, 6), 1, "a diagonal neighbour is one away");
  eq(movement.distance(5, 5, 7, 6), 2, "a knight's move is two");
  eq(movement.direction(5, 5, 6, 4), 1, "up and right is north-east");
  eq(movement.direction(5, 5, 9, 4), 1, "however far right");
  eq(movement.direction(5, 5, 5, 9), 4, "straight down is south");
  eq(movement.stepCost(water, land, movement.BOAT, false, false, 10), null, "a boat cannot go ashore");
  eq(movement.stepCost(water, water, movement.BOAT, false, false, 10), 2, "a boat moves on water");
  army.moves = 7;
  eq(movement.stackMoves([army, { moves: 3 }, { moves: 9 }]), 3, "a stack moves at the pace of its slowest army");

  army.moves = army.maxMoves;
  const from = { x: army.x, y: army.y };
  let target = null;
  for (let dx = -6; dx <= 6; dx++) {
    for (let dy = -6; dy <= 6; dy++) {
      const x = from.x + dx, y = from.y + dy;
      if (!target && x >= 0 && y >= 0 && x < g.map.width && y < g.map.height && !game.cityAt(g, x, y)) {
        const p = movement.findPath(g, stack, from.x, from.y, x, y);
        if (p && p.length >= 2 && p[p.length - 1].cost < movement.PAST_SHORE
            && !movement.has(movement.grid(g, side.index)[y * g.map.width + x], movement.WATER_F)) {
          target = { x, y, path: p };
        }
      }
    }
  }
  ok(target != null, "found somewhere to walk to");
  if (target) {
    const before = army.moves;
    const r = movement.moveTo(g, stack, target.x, target.y);
    ok(r.steps > 0, "the stack moved");
    eq(army.moves, Math.max(0, before - r.spent), "the cost was charged to the army");
    ok(army.x !== from.x || army.y !== from.y, "the army is somewhere else");
  }
  army.moves = 0;
  let r = movement.moveTo(g, stack, from.x, from.y + 1);
  eq(r.steps, 0, "an army with no moves stays put");
  const enemy = g.map.cities.find((c) => c.ownerIndex !== side.index);
  ok(enemy != null, "there is a city to attack");
  if (enemy) {
    army.moves = 99;
    army.x = enemy.x; army.y = enemy.y - 1;
    const path = movement.findPath(g, stack, army.x, army.y, enemy.x, enemy.y);
    if (path && path.length > 0) {
      const res = movement.walk(g, stack, path);
      eq(res.stopped, "attack", "stepping into an enemy city starts an attack");
      ok(res.attack && res.attack.city === enemy, "the attack names the city");
      eq(army.x, enemy.x, "the attacker has not entered the city");
      ok(army.y === enemy.y - 1, "the attacker stayed where it was");
    }
    const grid2 = movement.grid(g, side.index);
    eq(grid2[enemy.y * g.map.width + enemy.x] % 8, 0, "an enemy city is impassable in the cost grid");
  }
  {
    const a = { x: army.x, y: army.y, owner: side.index, type: 11, name: "Scouts", strength: 9, maxMoves: 20, moves: 20, upkeep: 1 };
    g.armies.push(a);
    let far, route;
    for (let d = 12; d <= 30; d += 2) {
      far = { x: Math.min(g.map.width - 2, a.x + d), y: a.y };
      route = movement.preview(g, [a], far.x, far.y);
      if (route && route.reach < route.path.length) break;
    }
    if (route) {
      ok(route.path.length > 0, "a stack under orders has a route to draw");
      ok(route.reach <= route.path.length, "it can walk no more of it than there is");
      eq(route.path[route.path.length - 1].x, far.x, "the route ends where it was sent");
      eq(route.path[route.path.length - 1].y, far.y, "on that row");
      let px = a.x, py = a.y, straight = true;
      for (const step of route.path) {
        if (Math.max(Math.abs(step.x - px), Math.abs(step.y - py)) !== 1) straight = false;
        px = step.x; py = step.y;
      }
      ok(straight, "and it is a chain of single steps");
      r = movement.moveTo(g, [a], far.x, far.y);
      ok(r.walked != null, "a walk reports the tiles it covered");
      eq(r.walked.length, r.steps, "one per step taken");
      if (r.steps > 0) {
        eq(r.walked[r.walked.length - 1].x, a.x, "ending where the stack now stands");
        eq(r.walked[r.walked.length - 1].y, a.y, "on that row");
      }
      const left = movement.preview(g, [a], far.x, far.y);
      if (left) ok(left.reach < left.path.length, "what is left of it is out of reach this turn");
    }
    removeArmy(g, a);
  }
}

function testStackLimit(scenario) {
  console.log("stack limit: " + scenario);
  const g = newGame(scenario, { seed: 13 });
  const side = game.begin(g);
  const army = game.sideArmies(g, side)[0];
  let tx = army.x, ty = army.y - 1;
  if (ty < 0) { tx = army.x; ty = army.y + 1; }
  for (let i = 0; i < rules.MAX_STACK; i++) {
    g.armies.push({ x: tx, y: ty, owner: side.index, type: army.type, name: army.name,
                    strength: 1, maxMoves: 10, moves: 10, upkeep: 0, homeCity: army.homeCity });
  }
  eq(game.armiesAt(g, tx, ty).length, rules.MAX_STACK, "the tile is full");
  army.moves = 99;
  const r = movement.moveTo(g, [army], tx, ty);
  eq(r.steps, 0, "a stack cannot stop on a full tile");
  ok(army.x !== tx || army.y !== ty, "the army stayed off the full tile");
}

function testCombat(scenario) {
  console.log("combat: " + scenario);
  const g = newGame(scenario, { seed: 17 });
  const side = game.begin(g);
  eq(combat.HERO_TABLE[0], 0, "a strength-0 hero commands nothing");
  eq(combat.HERO_TABLE[4], 1, "strength 4 gives +1");
  eq(combat.HERO_TABLE[9], 3, "strength 9 gives +3");
  eq(combat.CITY, 0, "city is class 0");
  eq(combat.OPEN, 1, "open is class 1");
  eq(combat.WOODS, 2, "woods is class 2");
  eq(combat.HILLS, 3, "hills is class 3");
  const seen = {};
  for (let y = 0; y < g.map.height; y += 7) {
    for (let x = 0; x < g.map.width; x += 7) {
      const cls = combat.terrainClass(g, x, y);
      const t = scn.terrainAt(g.map, x, y);
      seen[cls] = true;
      if (t === movement.FOREST) eq(cls, combat.WOODS, "forest fights as woods");
      if (t === movement.HILLS || t === movement.MOUNTAINS) eq(cls, combat.HILLS, "hills and mountains fight as hills");
      if (t === movement.CITY || t === movement.SITE) eq(cls, combat.CITY, "cities and sites fight as city");
      if (t === movement.PLAIN || t === movement.MARSH || t === movement.WATER) eq(cls, combat.OPEN, "plain, marsh and water fight as open");
    }
  }
  ok(seen[combat.OPEN], "the map has open ground");
  eq(g.map.combatCap, 5, "the combat cap is 5");
  const row = g.map.fightOrder[0], used = new Set();
  for (let t = 0; t <= 28; t++) {
    ok(row[t] != null, "type " + t + " has a fight order");
    used.add(row[t]);
  }
  eq(used.size, 29, "fight order is a permutation of 29 ranks");
  const neutral = g.map.cities.find((c) => c.ownerIndex == null && c.defence === 2);
  if (neutral) {
    const cls = combat.terrainClass(g, neutral.x, neutral.y);
    eq(combat.fortify(g, [], neutral.x, neutral.y, cls), 1, "a neutral city's defence 2 fortifies for 1");
    game.setCityOwner(g, neutral, g.sides[g.sides.length - 1].index);
    eq(combat.fortify(g, [], neutral.x, neutral.y, cls), 2, "an owned city's defence 2 fortifies for 2");
    game.setCityOwner(g, neutral, undefined);
  }
  let siege = null;
  for (const a of g.types.list) if (a.bonus[52] === combat.SIEGE) siege = a;
  if (siege && neutral) {
    const cls = combat.terrainClass(g, neutral.x, neutral.y);
    eq(combat.fortify(g, [{ type: siege.id }], neutral.x, neutral.y, cls), 0, "a Siege attacker cancels the city bonus");
  }
  const boatArmy = { type: 11, strength: 9, atSea: true };
  eq(combat.strength(g, boatArmy, 5, combat.OPEN, movement.WATER), 4, "an army at sea on water fights at 4");
  eq(combat.strength(g, boatArmy, 5, combat.OPEN, movement.SHORE), 4, "an army at sea on shore fights at 4");
  boatArmy.atSea = false;
  ok(combat.strength(g, boatArmy, 5, combat.OPEN, movement.PLAIN) > 4, "ashore it fights normally");
  eq(combat.strength(g, { type: 11, strength: 9 }, 99, combat.OPEN, movement.PLAIN), 15, "strength is capped at 15");
  const a1 = game.sideArmies(g, side)[0];
  const enemy = g.map.cities.find((c) => c.ownerIndex !== side.index);
  const [att, def, defOwner, city] = combat.lines(g, [a1], enemy.x, enemy.y);
  eq(att.length, 1, "the attacking line is the stack");
  ok(def.length >= 1, "the city has defenders");
  eq(city, enemy, "the battle names the city");
  for (const d of def) ok(d.owner !== side.index, "no defender belongs to the attacker");
  let sorted = true;
  for (let i = 1; i < def.length; i++) {
    const ownerRow = g.map.fightOrder[defOwner ?? 8];
    if (ownerRow[def[i].type] < ownerRow[def[i - 1].type]) sorted = false;
  }
  ok(sorted, "the defending line is sorted by fight order");
  const r = combat.resolve(g, att, def, enemy.x, enemy.y);
  ok(r.log.length >= 1, "the battle logged at least one death");
  eq(r.log.length, r.deadAttackers.length + r.deadDefenders.length, "one log entry per dead army");
  ok(r.won === (r.defenders.length === 0), "the attacker wins only if every defender is dead");
  const odds = (atkStrength, nAtk, defStrength, nDef) => {
    const A = [], D = [];
    for (let i = 0; i < nAtk; i++) A.push({ type: 11, strength: atkStrength, owner: side.index });
    for (let i = 0; i < nDef; i++) D.push({ type: 11, strength: defStrength, owner: undefined });
    let wins = 0;
    for (let i = 0; i < 200; i++) if (combat.resolve(g, A, D, 0, 0).won) wins++;
    return wins;
  };
  ok(odds(9, 8, 1, 1) > 190, "eight strong armies beat one weak one");
  ok(odds(1, 1, 9, 8) < 10, "one weak army loses to eight strong ones");
  const many = odds(3, 8, 9, 1);
  ok(many > 150, "many weak armies beat one strong one: " + many);
  eq(combat.ADVICE_BATTLES, 19, "the advisor fights 19 battles");
  const [verdict, wins] = combat.advise(g, att, def, enemy.x, enemy.y);
  ok(verdict != null, "the advisor has a verdict");
  ok(wins >= 0 && wins <= 19, "the advisor's win count is in range");
  eq(verdict, combat.ADVICE[Math.floor(wins / 2)], "the verdict is wins/2 into the table");
}

function testCapture(scenario) {
  console.log("capture: " + scenario);
  const g = newGame(scenario, { seed: 23 });
  const side = game.begin(g);
  const target = g.map.cities.find((c) => c.ownerIndex == null);
  ok(target != null, "found a neutral city");
  const stack = [];
  for (let i = 0; i < 8; i++) {
    const a = { x: target.x, y: target.y - 1, owner: side.index, type: 11, name: "Scouts", strength: 9, maxMoves: 20, moves: 20, upkeep: 1 };
    g.armies.push(a);
    stack.push(a);
  }
  {
    const far = { owner: side.index, type: 11, name: "Scouts", strength: 9, maxMoves: 20, moves: 20, upkeep: 1 };
    g.armies.push(far);
    let walk;
    for (const off of [[-3, -3], [3, -3], [-3, 3], [3, 3], [0, -3], [0, 3], [-3, 0], [3, 0]]) {
      far.x = target.x + off[0]; far.y = target.y + off[1]; far.moves = 20;
      walk = movement.moveTo(g, [far], target.x, target.y);
      if (walk.stopped === "attack") break;
    }
    eq(walk.stopped, "attack", "walking at a city ends in an assault");
    eq(walk.attack.x, target.x, "on the city's own tile");
    eq(walk.attack.y, target.y, "and row");
    eq(walk.attack.city, target, "which is the city");
    ok(Math.max(Math.abs(far.x - target.x), Math.abs(far.y - target.y)) === 1, "and the stack is left standing beside it, not in it");
    ok(far.x !== target.x || far.y !== target.y, "it did not walk in");
    eq(target.ownerIndex, null, "and the city is still the defender's");
    removeArmy(g, far);
  }
  const before = game.sideCities(g, side).length;
  let r;
  {
    const i = target.y * scn.MAP_W + target.x;
    const tile = g.map.tiles[i], armies = g.armies.length, from = stack[0].x;
    r = game.decideAttack(g, stack, target.x, target.y);
    ok(r.won, "the fight is decided");
    eq(target.ownerIndex, null, "but the city is not yet taken");
    eq(g.map.tiles[i], tile, "nor its castle repainted");
    eq(g.armies.length, armies, "nor the dead removed");
    eq(stack[0].x, from, "nor the survivors moved in");
    eq(r.captured, null, "and nothing is said to be captured");
    game.applyAttack(g, r);
    ok(g.map.tiles[i] !== tile, "applied, the castle is in the victor's colours");
    const owner = target.ownerIndex, n = g.armies.length;
    game.applyAttack(g, r);
    eq(g.armies.length, n, "and applying again changes nothing");
    eq(target.ownerIndex, owner, "");
  }
  ok(r.won, "the overwhelming stack took the city");
  ok(r.lines != null, "the attack reports the lines that fought");
  eq(r.lines.attackers.length, stack.length, "every attacker is in the line");
  eq(r.lines.city, target, "and the line knows it was a city");
  let down = 0;
  for (const s of r.log || []) { ok(s === 0 || s === 1, "the log says which side lost an army"); down++; }
  eq(down, r.deadAttackers.length + r.deadDefenders.length, "the log has one entry per army that fell");
  eq(target.ownerIndex, side.index, "the city changed hands");
  eq(game.sideCities(g, side).length, before + 1, "the side owns one more city");
  eq(target.producing, null, "a captured city is not building anything");
  for (const a of r.attackers) eq(a.x, target.x, "the survivors moved in");
  for (const dead of r.deadDefenders) ok(!g.armies.includes(dead), "a dead defender is removed");
  const victim = g.sides[g.sides.length - 1];
  victim.gold = 400;
  eq(game.loot(g, victim), 200, "one city: half its gold");
  const extra = g.map.cities.find((c) => c.ownerIndex == null && c !== target);
  if (extra) {
    game.setCityOwner(g, extra, victim.index);
    eq(game.loot(g, victim), Math.floor(Math.floor(400 / 2) / 2), "two cities: half the per-city share");
  }
}

function testTutorialHero() {
  console.log("tutorial: a hero cannot die attacking neutrals");
  const g = newGame("TUTORIA", { seed: 31 });
  const side = game.begin(g);
  eq(g.map.options.tutorial, 1, "TUTORIA sets the tutorial flag");
  ok(!side.computer, "the tutorial player is human");
  const h = { type: armytype.HERO, strength: 1, owner: side.index };
  const defenders = [];
  for (let i = 0; i < 8; i++) defenders.push({ type: 11, strength: 9, owner: undefined });
  let deaths = 0;
  for (let i = 0; i < 50; i++) deaths += combat.resolve(g, [h], defenders, 0, 0).deadAttackers.length;
  eq(deaths, 0, "the tutorial hero never dies against neutrals");
  g.map.options.tutorial = 0;
  deaths = 0;
  for (let i = 0; i < 50; i++) deaths += combat.resolve(g, [h], defenders, 0, 0).deadAttackers.length;
  ok(deaths > 0, "without the tutorial flag the hero can die");
}

function testCityChoices(scenario) {
  console.log("pillage, sack and raze: " + scenario);
  const g = newGame(scenario, { seed: 29 });
  const side = game.begin(g);
  const captured = (want) => {
    const city = g.map.cities.find((c) => c.ownerIndex == null && c.slots.length >= want);
    if (city) game.setCityOwner(g, city, side.index);
    return city;
  };
  const city = captured(2);
  ok(city != null, "found a city with something to pillage");
  if (city) {
    const n = city.slots.length, dear = city.slots[city.slots.length - 1];
    const want = Math.floor(Math.abs(g.types.byId[dear.type].price) / 2);
    const gold0 = side.gold, score0 = side.diploScore || 0;
    const [got] = game.pillage(g, side, city);
    eq(got, want, "pillage pays half the type's purchase price");
    eq(side.gold, gold0 + want, "the gold was paid");
    eq(city.slots.length, n - 1, "pillage removed one type");
    eq(city.defence, rules.cityDefence(city.slots.length), "defence was recomputed");
    const d = (side.diploScore || 0) - score0;
    ok(d >= 1 && d <= 5, "pillage costs 1d5 diplomatic score: " + d);
    for (const s2 of city.slots) ok(s2.type !== dear.type || s2 !== dear, "the pillaged type is gone");
  }
  const city2 = captured(3);
  if (city2) {
    const cheapest = city2.slots[0];
    let want = 0;
    for (let i = 1; i < city2.slots.length; i++) want += Math.floor(Math.abs(g.types.byId[city2.slots[i].type].price) / 2);
    const gold0 = side.gold, score0 = side.diploScore || 0;
    const n2 = city2.slots.length;
    const [got, lost] = game.sack(g, side, city2);
    eq(got, want, "sack pays for every type it strips");
    eq(lost.length, n2 - 1, "sack lists every type it strips");
    eq(lost.reduce((s, l) => s + l.gold, 0), want, "and what each was worth adds up to the gold");
    eq(side.gold, gold0 + want, "the gold was paid");
    eq(city2.slots.length, 1, "only the cheapest type is left");
    eq(city2.slots[0], cheapest, "and it is the cheapest one");
    eq(city2.defence, 1, "one type means defence 1");
    const d = (side.diploScore || 0) - score0;
    ok(d >= 6 && d <= 15, "sack costs 1d10+5 diplomatic score: " + d);
  }
  const city3 = captured(1);
  if (city3) {
    const before = game.sideCities(g, side).length;
    const other = g.map.cities.find((c) => c.ownerIndex === side.index && c !== city3);
    if (other) game.vector(g, other, city3);
    const score0 = side.diploScore || 0;
    game.raze(g, side, city3);
    eq(city3.ownerIndex, null, "a razed city is neutral");
    eq(city3.slots.length, 0, "a razed city produces nothing");
    eq(city3.income, 0, "a razed city earns nothing");
    eq(game.sideCities(g, side).length, before - 1, "the side no longer owns it");
    if (other) eq(other.vectorTo, null, "vectoring to it was cancelled");
    const d = (side.diploScore || 0) - score0;
    ok(d >= 11 && d <= 25, "raze costs 1d15+10 diplomatic score: " + d);
    const rb = movement.grid(g, side.index)[city3.y * g.map.width + city3.x];
    ok(!movement.has(rb, movement.CITY_F) && rb % 8 !== 0, "a razed city's ruins can be walked over: " + rb);
  }
  eq(game.pillage(g, side, { slots: [], index: -1 })[0], 0, "pillaging an empty city pays nothing");
  eq(game.sack(g, side, { slots: [{ type: 11 }], index: -1 })[0], 0, "sacking a one-type city pays nothing");
}

function testHeroes(scenario) {
  console.log("heroes: " + scenario);
  const g = newGame(scenario, { seed: 37 });
  const side = game.begin(g);
  const offer = side.heroOffer;
  ok(offer != null, "a hero offers itself on turn 1");
  eq(offer.price, 0, "the first hero is free");
  eq(offer.city, side.capital, "the first hero appears at the capital");
  const names = hero.names(g, side);
  eq(names.length, 100, "a side has a hundred candidate heroes");
  ok(offer.name != null && offer.name !== "", "the offer carries a name");
  ok(names.some((n) => n.name === offer.name), "and the name comes from that side's file");
  ok(!hero.names(g, g.map.sides[0]).some((n) => n.female), "side 0 has no heroines to offer");
  const [h, allies] = hero.recruit(g, side, offer);
  eq(h.type, armytype.HERO, "the recruit is a hero");
  eq(h.name, offer.name, "the hero keeps the name it was offered under");
  eq(h.strength, 5, "a new hero has strength 5");
  eq(h.maxMoves, 14, "a new hero has 14 movement");
  eq(h.moves, 14, "a new hero joins with its moves already full");
  eq(allies.length, 0, "the first hero brings no allies");
  eq(h.items.length, 1, "the first hero carries one item");
  eq(h.items[0].type, rules.ITEM_STANDARD, "and it is the side's standard");
  h.experience = 15;
  const promoted = hero.checkPromotions(g, side);
  eq(promoted.length, 1, "15 experience promotes a Hero");
  eq(h.level, 2, "the hero is now level 2");
  eq(h.title, "Cavalier", "level 2 is a Cavalier");
  eq(h.strength, 6, "promotion adds a strength");
  eq(h.maxMoves, 16, "promotion adds 2 movement");
  eq(hero.checkPromotions(g, side).length, 0, "no second promotion on the same experience");
  h.experience = 60;
  hero.checkPromotions(g, side);
  eq(h.level, 3, "60 experience promotes one step only");
  hero.checkPromotions(g, side);
  eq(h.title, "Paladin", "the next check reaches Paladin");
  eq(hero.checkPromotions(g, side).length, 0, "a Paladin is not promoted again");
  hero.addExperience(g, h, 99);
  eq(h.experience, rules.MAX_HERO_XP, "experience is capped at 60");
  const [type, n] = hero.allies(g);
  ok(type != null, "the allies have a type");
  ok(n >= 1 && n <= 3, "1 to 3 allies arrive");
  g.turn = 4;
  const [, escort] = hero.recruit(g, side, { price: 0, city: side.capital });
  ok(escort.length >= 1, "a later hero arrives with an escort");
  for (const a of escort) {
    eq(a.moves, a.maxMoves, "an ally joins with its moves already full");
    eq(a.upkeep, 0, "an ally costs no upkeep");
  }
  g.turn = 1;
  eq(hero.MAX_PER_SIDE, 5, "a side may hold 5 heroes");
  eq(hero.MAX_IN_GAME, 40, "the game holds 40 heroes");
  let blocked = true;
  for (let i = 0; i < 5; i++) g.armies.push({ type: armytype.HERO, owner: side.index, x: 0, y: 0 });
  g.turn = 2;
  for (let i = 0; i < 50; i++) if (hero.offer(g, side)) blocked = false;
  ok(blocked, "no hero is offered once the side is at its limit");
  const carrier = { type: armytype.HERO, owner: side.index, items: [{ name: "Firesword", type: rules.ITEM_BATTLE, value: 1 }] };
  let dry = null;
  for (let y = 0; y < g.map.height && !dry; y++) {
    for (let x = 0; x < g.map.width; x++) if (!dry && scn.terrainAt(g.map, x, y) === movement.PLAIN) dry = { x, y };
  }
  const dropped = hero.dropItems(g, carrier, dry.x, dry.y);
  eq(dropped.length, 1, "the item was dropped");
  eq(dropped[0].status, 1, "a dropped item lies on the ground");
  eq(dropped[0].x, dry.x, "it lies where the hero fell");
  eq(carrier.items.length, 0, "the dead hero carries nothing");
  let wet = null;
  for (let y = 0; y < g.map.height && !wet; y++) {
    for (let x = 0; x < g.map.width; x++) if (!wet && scn.terrainAt(g.map, x, y) === movement.WATER) wet = { x, y };
  }
  if (wet) {
    const drowner = { type: armytype.HERO, owner: side.index, items: [{ name: "Icesword", type: rules.ITEM_BATTLE, value: 1 }] };
    const lost = drowner.items[0];
    eq(hero.dropItems(g, drowner, wet.x, wet.y).length, 0, "nothing is dropped in water");
    eq(lost.status, 0, "an item lost at sea is gone for good");
  }
}

function testHeroExperienceBug() {
  console.log("hero experience: the original's bug");
  const g = newGame("ERYTHEA", { seed: 41 });
  const run = () => {
    const defHero = { type: armytype.HERO, strength: 5, owner: 1, experience: 0 };
    hero.battleExperience(g, [{ type: 11, strength: 3, owner: 0 }], [defHero], { deadByArmy: new Set() }, false);
    return defHero.experience;
  };
  rules.bugs.heroExperienceReadsAttackerTypes = true;
  eq(run(), 0, "with the bug on, a defending hero facing a non-hero gains nothing");
  rules.bugs.heroExperienceReadsAttackerTypes = false;
  eq(run(), 1, "with the bug off, the defender is credited");
  rules.bugs.heroExperienceReadsAttackerTypes = true;
  const h = { type: armytype.HERO, strength: 5, owner: 0, experience: 0 };
  hero.battleExperience(g, [h], [], { deadByArmy: new Set() }, false);
  eq(h.experience, 1, "an attacking hero gains 1 in the open");
  hero.battleExperience(g, [h], [], { deadByArmy: new Set() }, true);
  eq(h.experience, 3, "attacking a city is worth 2");
  const dead = { type: armytype.HERO, strength: 5, owner: 0, experience: 0 };
  hero.battleExperience(g, [dead], [], { deadByArmy: new Set([dead]) }, true);
  eq(dead.experience, 0, "a hero that died gains nothing");
}

function testAIGame(scenario, turns) {
  console.log(fmt("AI game: %s, %d turns", scenario, turns));
  const g = newGame(scenario, { seed: 101 });
  let side = game.begin(g);
  const ownedAtStart = {};
  for (const s of g.sides) ownedAtStart[s.index] = game.sideCities(g, s).length;
  while (side && g.turn <= turns) {
    ai.runSync(ai.playTurn(g, side));
    side = game.endTurn(g);
  }
  ok(g.turn > turns || side == null, "the game ran to the turn limit");
  const perTile = {};
  for (const a of g.armies) {
    ok(g.types.byId[a.type] != null, "every army has a real type");
    if (!a.transit) {
      ok(a.x != null && a.y != null, "a placed army has a position");
      ok(a.x >= 0 && a.x < g.map.width && a.y >= 0 && a.y < g.map.height, "every army is on the map");
      const k = a.y * g.map.width + a.x;
      perTile[k] = (perTile[k] || 0) + 1;
    }
    ok((a.moves || 0) >= 0, "movement never goes negative");
    ok((a.moves || 0) <= rules.MAX_MOVE, "movement never exceeds the cap");
  }
  const worst = Math.max(0, ...Object.values(perTile));
  ok(worst <= rules.MAX_STACK, "no tile ever holds more than 8 armies: " + worst);
  for (const s of g.sides) {
    ok(s.gold >= 0, s.name + " never goes into debt");
    if (!s.alive) ok(game.sideCities(g, s).length === 0, "a dead side owns nothing");
  }
  for (const c of g.map.cities) {
    if (c.ownerIndex != null) ok(g.map.sides[c.ownerIndex] != null, "a city's owner is a real side");
    ok(c.slots.length <= 4, "a city never gains production slots");
  }
  let moved = false, captures = 0;
  for (const s of g.sides) {
    const now = game.sideCities(g, s).length;
    if (now !== ownedAtStart[s.index]) moved = true;
    captures += Math.max(0, now - ownedAtStart[s.index]);
  }
  ok(moved, "cities changed hands over " + turns + " turns");
  ok(captures > 0, fmt("the computer players took %d cities", captures));
  ok(g.armies.length > g.map.cities.length, fmt("armies were built: %d from %d cities", g.armies.length, g.map.cities.length));
}

function testSlots() {
  console.log("army slots");
  // a stand-in game: the fight-order table and the army list are all the
  // model wants
  const stack = (n) => {
    const g = { map: { fightOrder: { 0: {} } }, armies: [] };
    const armies = [];
    for (let i = 1; i <= n; i++) {
      armies[i - 1] = { type: i, moves: 10 + i, group: 0 };
      g.map.fightOrder[0][i] = n - i;          // the first army ranks highest
      g.armies[i - 1] = armies[i - 1];
    }
    return [g, armies];
  };
  {
    const [g, a] = stack(3);
    const s = slots.build(g, a, 0);
    eq(s.n, 3, "three armies fill three slots");
    eq(s.army[0], a[0], "the highest in the fight order takes the first slot");
    ok(s.inGroup[0] && !s.inGroup[1] && !s.inGroup[2], "clicking a tile selects one army, not the stack");
    eq(s.group[0], 0, "ungrouped armies are each their own group");
    eq(s.group[2], 2, "numbered off down the bar");
    eq(s.mark[0], slots.TICK, "the one that moves is ticked");
    eq(s.mark[1], slots.CROSS, "the others are crossed");
    eq(slots.moves(s), 11, "and Group Move is that army's own");
    eq(slots.grouped(s), false, "so the Grp button is not green yet");
    eq(slots.selected(s).length, 1, "one army moves");
  }
  {
    const [g, a] = stack(3);
    const s = slots.build(g, a, 0);
    slots.toggle(s, g, 1);
    ok(s.inGroup[1] && s.group[1] === s.group[0], "a click adds an army to the group");
    eq(s.mark[1], null, "only the head of a group is marked");
    eq(slots.moves(s), 11, "the group moves at its slowest army's pace");
    slots.toggle(s, g, 1);
    ok(!s.inGroup[1], "clicking it again drops it out");
    ok(s.group[1] !== s.group[0], "into a group of its own");
    slots.toggle(s, g, 0);
    ok(s.inGroup[0], "the last army of a group cannot be dropped");
  }
  {
    const [g, a] = stack(3);
    const s = slots.build(g, a, 0);
    slots.all(s, g);
    eq(slots.selected(s).length, 3, "Grp takes the whole stack");
    eq(slots.grouped(s), true, "and turns the button green");
    eq(s.mark[1], null, "one mark for the one group");
    slots.single(s, g);
    eq(slots.selected(s).length, 1, "and Grp again breaks it up");
    eq(slots.grouped(s), false, "leaving the button red");
    slots.pickGroup(s, g, 2);
    ok(s.inGroup[2] && !s.inGroup[0], "a mark picks out that group alone");
    eq(s.mark[2], slots.TICK, "which is then the ticked one");
  }
  {
    const [g, a] = stack(3);
    const s = slots.build(g, a, 0);
    slots.all(s, g);
    s.inGroup[2] = false;
    slots.pickGroup(s, g, 0);
    ok(s.inGroup[2], "picking a group takes all of it");
  }
  {
    const [g, a] = stack(3);
    const s = slots.build(g, a, 0);
    slots.all(s, g);
    slots.commit(s, g);
    const id = a[0].group;
    ok(id !== slots.UNGROUPED && id === a[1].group && id === a[2].group, "a group of more than one is written back into its armies");
    const again = slots.build(g, a, 0);
    eq(slots.selected(again).length, 3, "so picking it up again takes the whole group");
    eq(slots.grouped(again), true, "with the Grp button still green");
    eq(again.group[0], again.group[2], "and all three in one group in the bar");
    slots.commit(again, g);
    eq(a[0].group, id, "picking it up does not renumber it");
    slots.toggle(again, g, 2);
    slots.commit(again, g);
    const third = slots.build(g, a, 0);
    eq(slots.selected(third).length, 2, "dropping one out leaves the rest grouped");
    eq(a[2].group, slots.UNGROUPED, "and the one dropped out is on its own");
    eq(a[0].group, id, "while the group keeps its id");
    slots.single(third, g);
    slots.commit(third, g);
    eq(a[0].group, slots.UNGROUPED, "and breaking it up clears the grouping");
    eq(slots.selected(slots.build(g, a, 0)).length, 1, "so a click picks up one army again");
  }
  {
    const [g, a] = stack(3);
    const s = slots.build(g, a, 0);
    slots.all(s, g);
    const left = slots.keep(s, g, [a[0], a[2]]);
    eq(left.n, 2, "the dead leave the bar");
    ok(left.inGroup[0] && left.inGroup[1], "the survivors keep moving together");
    eq(slots.keep(s, g, []), null, "and a wiped-out stack is no selection at all");
  }
}

function testScreenLayout() {
  console.log("screen layout");
  const ui = uidata.load(DATA);
  eq(ui.bitmaps[0], "startup.pck", "bitmap 0 is the startup screen");
  eq(ui.bitmaps[4], "button.pck", "bitmap 4 is the button sheet");
  eq(ui.bitmaps[78], "movebar7.pck", "there are 79 bitmaps, 0 to 78");
  eq(Object.keys(ui.joins).length, 36, "JOIN.DAT names 36 dialogs");
  eq(ui.joins[0].button, 0, "dialog 0 uses button group 0");
  eq(ui.joins[0].area, 2, "dialog 0 uses area screen 2");
  eq(ui.joins[9].button, 11, "dialog 9 takes button group 11");
  eq(ui.joins[12].button, 9, "and dialog 12 takes group 9");
  const d = uidata.dialog(ui, uidata.MAIN_SCREEN);
  eq(d.controls.length, 43, "the main screen has 43 controls");
  eq(d.regions.length, 6, "and 6 regions");
  const want = { 1: [400, 30, 224, 312], 2: [16, 30, 360, 360], 3: [0, 0, 640, 18], 9: [16, 408, 360, 56] };
  for (const r of d.regions) {
    const w = want[r.id];
    if (w) {
      eq(r.x, w[0], fmt("region %d x", r.id));
      eq(r.y, w[1], fmt("region %d y", r.id));
      eq(r.w, w[2], fmt("region %d width", r.id));
      eq(r.h, w[3], fmt("region %d height", r.id));
    }
  }
  eq(ui.shortcutNames[507], "Move All", "UDB.DAT names menu item 507");
  eq(ui.shortcutNames[535], "End Turn", "and item 535");
  const wantSc = ["Search", "Move All", "Heroes", "End Turn"];
  for (let i = 0; i < 4; i++) {
    eq(ui.shortcutNames[ui.shortcuts[i]], wantSc[i], fmt("button %d (control %d) is %s", i, 179 + i, wantSc[i]));
  }
  {
    const { w, h } = vfs.image(DATA + "/PICS/MENUBUTT.PCK");
    eq(w, 320, "MENUBUTT.PCK is 320 wide");
    eq(h, 197, "and 197 tall: 7 rows 28 apart, plus the last row's border");
    const cells = new Set();
    let n = 0;
    for (const id in ui.shortcutItems) {
      const item = ui.shortcutItems[id];
      eq(item.w, 32, fmt("item %s icon width", id));
      eq(item.h, 29, fmt("item %s icon height", id));
      for (let st = 0; st <= 2; st++) {
        const src = item.src[st];
        eq(src.x % 32, 0, fmt("item %s state %d sits on a column", id, st));
        eq(src.y % 28, 0, fmt("item %s state %d sits on a row", id, st));
        ok(src.x + 32 <= w && src.y + 29 <= h, fmt("item %s state %d is inside the sheet", id, st));
        const key = src.x + "," + src.y;
        ok(!cells.has(key), fmt("cell %s is used once", key));
        cells.add(key); n++;
      }
    }
    eq(n, 63, "21 items, three cells each");
    const endTurn = ui.shortcutItems[535];
    eq(endTurn.src[uidata.NORMAL].x, 0, "End Turn rests at the sheet's origin");
    eq(endTurn.src[uidata.NORMAL].y, 0, "at the top of it");
    eq(endTurn.src[uidata.ACTIVE].x, 32, "and lights up one cell right");
  }
  {
    const { w, h, px } = vfs.image(DATA + "/PICS/ABITS.PCK");
    eq(w, 480, "ABITS.PCK is 480 wide");
    eq(h, 40, "and 40 tall");
    const BG = 3, seen = new Set();
    for (let cell = 0; cell <= 8; cell++) {
      const count = {};
      for (let y = 0; y <= 29; y++) {
        for (let x = cell * 32; x <= cell * 32 + 31; x++) {
          const v = px[y * w + x];
          if (v !== BG) count[v] = (count[v] || 0) + 1;
        }
      }
      let best = null, bestN = 0;
      for (const v in count) if (count[v] > bestN) { best = v; bestN = count[v]; }
      ok(bestN > 40, fmt("ring %d has ink", cell));
      if (cell > 0) {
        ok(!seen.has(best), fmt("ring %d has its own colour", cell));
        seen.add(best);
      }
    }
    let ringRow = 0, stripRow = 0;
    for (let x = 0; x <= 287; x++) {
      if (px[15 * w + x] !== BG) ringRow++;
      if (px[38 * w + x] !== BG) stripRow++;
    }
    ok(stripRow > ringRow * 2, "row 38 is far denser than row 15, so it is not part of the rings");
    for (const [sx, sy, name] of [[344, 0, "cities"], [344, 20, "treasury"], [384, 0, "income"], [384, 20, "upkeep"]]) {
      ok(sx >= 288, fmt("the %s icon is past the nine rings", name));
      ok(sx + 40 <= w && sy + 20 <= h, fmt("the %s icon is inside the sheet", name));
      let ink = 0;
      for (let y = sy; y <= sy + 19; y++) for (let x = sx; x <= sx + 39; x++) if (px[y * w + x] !== BG) ink++;
      ok(ink > 40, fmt("the %s icon has ink", name));
      let edge = 0;
      for (let y = sy; y <= sy + 19; y++) if (px[y * w + sx - 1] !== BG) edge++;
      eq(edge, 0, fmt("the %s icon starts on a clear column", name));
    }
  }
  const sizes = {};
  let checked = 0;
  for (const gid in ui.buttons) {
    for (const c of ui.buttons[gid].controls) {
      if (c.bitmap !== 0 && c.w > 0) {
        const name = ui.bitmaps[c.bitmap];
        if (sizes[name] === undefined) {
          sizes[name] = false;
          for (const dir of ["/PICS/", "/TERRAIN0/", "/"]) {
            const p = DATA + dir + String(name).toUpperCase();
            if (vfs.hasImage(p)) { const im = vfs.image(p); sizes[name] = { w: im.w, h: im.h }; break; }
          }
        }
        const s = sizes[name];
        ok(s !== false, "bitmap resolves to a file: " + name);
        if (s) {
          for (let st = 0; st <= 2; st++) {
            ok(c.src[st].x + c.w <= s.w && c.src[st].y + c.h <= s.h, fmt("control %d state %d lies inside %s", c.id, st, name));
            checked++;
          }
        }
      }
    }
  }
  ok(checked >= 400, fmt("checked %d source rects", checked));
}

function testCityCastles() {
  console.log("city castles");
  const g = newGame("TUTORIA", { seed: 11 });
  const map = g.map;
  const WANT = { "-1": 96, 0: 98, 1: 100, 2: 102, 3: 104, 4: 106, 5: 108, 6: 128, 7: 130 };
  const city = map.cities[0];
  for (const k in WANT) {
    const owner = Number(k), base = WANT[k];
    city.ownerIndex = owner >= 0 ? owner : undefined;
    city.razed = undefined;
    ok(scn.cityTileBase(city) === base, fmt("owner %d takes castle block %d", owner, base));
    scn.setCityTiles(map, city);
    ok(scn.tileAt(map, city.x, city.y) === base, "top-left cell is the block");
    ok(scn.tileAt(map, city.x + 1, city.y) === base + 1, "top-right is +1");
    ok(scn.tileAt(map, city.x, city.y + 1) === base + 16, "bottom-left is +16");
    ok(scn.tileAt(map, city.x + 1, city.y + 1) === base + 17, "bottom-right is +17");
    for (let dx = 0; dx <= 1; dx++) {
      for (let dy = 0; dy <= 1; dy++) ok(scn.terrainAt(map, city.x + dx, city.y + dy) === 10, fmt("castle cell %d,%d is city terrain", dx, dy));
    }
  }
  for (let owner = 0; owner <= 7; owner++) {
    city.ownerIndex = undefined; city.razed = true; city.razedBy = owner;
    ok(scn.cityTileBase(city) === 0xa0 + 2 * owner, fmt("ruins of side %d take block %d", owner, 0xa0 + 2 * owner));
    scn.setCityTiles(map, city);
    for (let dx = 0; dx <= 1; dx++) {
      for (let dy = 0; dy <= 1; dy++) ok(scn.terrainAt(map, city.x + dx, city.y + dy) === 11, fmt("ruin cell %d,%d is ruins terrain", dx, dy));
    }
  }
  const g2 = newGame("TUTORIA", { seed: 12 });
  const c2 = g2.map.cities[0];
  for (const side of g2.sides) {
    game.setCityOwner(g2, c2, side.index);
    scn.setCityTiles(g2.map, c2);
  }
  const mine = game.sideCities(g2, g2.sides[0])[0];
  ok(mine != null, "the first side holds a city to raze");
  if (mine) {
    const was = mine.ownerIndex;
    game.raze(g2, g2.sides[0], mine, []);
    ok(mine.razedBy === was, "raze remembers who held the city");
    ok(scn.tileAt(g2.map, mine.x, mine.y) === 0xa0 + 2 * was, "raze restamps the map with that side's ruins");
  }
}

function testSites(scenario) {
  console.log("sites: " + scenario);
  const g = newGame(scenario, { seed: 53 });
  let rich = 0;
  for (const s of g.map.sites) {
    ok(s.content != null, "every site has contents");
    ok(s.content !== site.EMPTY, "no site is left empty");
    if (s.rich) {
      rich++;
      ok(s.type !== site.TEMPLE, "a temple is never rich");
    }
    if (s.content === site.ALLIES) {
      ok(s.allyType != null, "an ally ruin knows what joins");
      ok(g.types.byId[s.allyType] != null, "and it is a real army type");
    }
    if (s.content !== site.TEMPLE) ok(s.guardian != null && s.guardian >= 1 && s.guardian <= 9, "a ruin has a guardian");
  }
  eq(rich, Math.floor(g.map.sites.length * 3 / 10), "three in ten sites are rich");
  const placed = new Set();
  for (const s of g.map.sites) {
    if (s.content === site.ITEM) {
      ok(!placed.has(s.item), "an item is hidden in only one ruin");
      placed.add(s.item);
    }
  }
  for (const it of g.map.items) if (placed.has(it.index)) eq(it.status, 2, "a hidden item is marked hidden");
  const byIndex = {};
  for (const it of g.map.items) byIndex[it.index] = it;
  for (const s of g.map.sites) {
    if (s.content === site.ITEM) eq(site.itemReserved(byIndex[s.item]), s.rich, "a reserved item needs a rich ruin");
  }
  if (g.map.itemPool) {
    const names = new Set(g.map.itemPool.map((p) => p.name));
    for (let i = 8; i <= 21; i++) if (byIndex[i]) ok(names.has(byIndex[i].name), "item " + i + " came from the pool");
    for (let i = 0; i <= 7; i++) if (byIndex[i]) eq(byIndex[i].type, rules.ITEM_STANDARD, "records 0-7 are standards");
  }
  const ruin = g.map.sites.find((s) => s.content === site.GOLD);
  if (ruin) {
    const side = g.sides[0];
    const grunt = { x: ruin.x, y: ruin.y, owner: side.index, type: 11, strength: 3, maxMoves: 10, moves: 10 };
    g.armies.push(grunt);
    let r = site.search(g, [grunt], ruin.x, ruin.y);
    eq(r.kind, "no hero", "a stack without a hero cannot search a ruin");
    ok(!ruin.searched, "and the ruin is not used up");
    const h = { x: ruin.x, y: ruin.y, owner: side.index, type: armytype.HERO, strength: 9, maxMoves: 14, moves: 14, experience: 0, items: [] };
    g.armies.push(h);
    const gold0 = side.gold;
    r = site.search(g, [h, grunt], ruin.x, ruin.y);
    ok(r.kind === "gold" || r.kind === "killed", "a hero gets a result: " + r.kind);
    if (r.kind === "gold") {
      ok(r.gold >= 503 && r.gold <= 4000, "the gold is in range: " + r.gold);
      eq(side.gold, gold0 + r.gold, "the gold was paid");
      eq(h.experience, 3, "searching is worth 3 experience");
    }
    ok(ruin.searched, "a searched ruin is used up");
    eq(site.search(g, [h], ruin.x, ruin.y), null, "and cannot be searched again");
  }
  const temple = g.map.sites.find((s) => s.content === site.TEMPLE && s.templeIndex === 0);
  if (temple) {
    const side = g.sides[0];
    const a = { x: temple.x, y: temple.y, owner: side.index, type: 11, strength: 3 };
    const r = site.search(g, [a], temple.x, temple.y);
    eq(r.kind, "temple", "a temple blesses");
    eq(r.blessed, 1, "one army was blessed");
    eq(a.strength, 4, "the blessing adds a strength");
    eq(site.search(g, [a], temple.x, temple.y).blessed, 0, "the same temple does not bless twice");
    a.strength = 9;
    a.blessings = {};
    site.search(g, [a], temple.x, temple.y);
    eq(a.strength, 9, "a blessing never passes 9");
  }
  const strong = { strength: 9, items: [] }, weak = { strength: 1, items: [] };
  let survived = 0, died = 0;
  for (let i = 0; i < 400; i++) {
    if (site.survivesGuardian(g, strong, [strong], 3)) survived++;
    if (!site.survivesGuardian(g, weak, [weak], 8)) died++;
  }
  eq(survived, 400, "a strong hero always beats a weak guardian");
  ok(died > 0, "a weak hero sometimes dies to a strong one");
}

function testDiplomacy() {
  console.log("diplomacy");
  const d = diplomacy;
  const off = newGame("ERYTHEA", { seed: 61 });
  eq(off.map.options.diplomacy, 0, "the shipped scenarios have diplomacy off");
  eq(d.state(off, 0, 1), d.WAR, "with the option off every pair starts at war");
  ok(d.mayAttack(off, 0, 1), "and may attack freely");
  const g = newGame("ERYTHEA", { seed: 61, options: { diplomacy: 1 } });
  eq(d.state(g, 0, 1), d.PEACE, "with the option on every pair starts at peace");
  ok(!d.mayAttack(g, 0, 1), "a side at peace may not attack");
  ok(d.mayAttack(g, 0, null), "neutrals may always be attacked");
  eq(d.state(g, 0, 0), d.PEACE, "a side is at peace with itself");
  d.propose(g, 0, 1, d.WAR);
  const msgs = d.apply(g, g.sides[0]);
  eq(d.state(g, 0, 1), d.WAR, "declaring war takes effect at once");
  eq(d.state(g, 1, 0), d.WAR, "and binds the other side too");
  ok(msgs[0] && msgs[0].includes("War declared"), "and is announced");
  eq(d.proposal(g, 1, 0), d.WAR, "the other side's proposal is raised to match");
  const g2 = newGame("ERYTHEA", { seed: 62, options: { diplomacy: 1 } });
  d.propose(g2, 0, 1, d.WAR);
  d.apply(g2, g2.sides[0]);
  delete g2.diplomacy.proposal[1 * 8 + 0];
  d.propose(g2, 0, 1, d.PEACE);
  d.apply(g2, g2.sides[0]);
  eq(d.state(g2, 0, 1), d.WAR, "one-sided peace does not land");
  d.propose(g2, 0, 1, d.PEACE);
  d.propose(g2, 1, 0, d.PEACE);
  const m2 = d.apply(g2, g2.sides[0]);
  eq(d.state(g2, 0, 1), d.PEACE, "matched proposals make peace");
  ok(m2[0] && m2[0].includes("Peace negotiated"), "and it is announced");
  {
    const g3 = newGame("ERYTHEA", { seed: 63, options: { diplomacy: 1 } });
    d.propose(g3, 0, 1, d.WAR);
    d.apply(g3, g3.sides[0]);
    d.propose(g3, 0, 1, d.PEACE);
    let before = g3.sides[0].diploScore || 0;
    d.scoreUpdate(g3, g3.sides[0]);
    const gain = (g3.sides[0].diploScore || 0) - before;
    ok(gain >= 11 && gain <= 20, "peace offered from war is worth 1d10+10: " + gain);
    d.propose(g3, 1, 0, d.PEACE);
    before = g3.sides[0].diploScore;
    d.scoreUpdate(g3, g3.sides[0]);
    eq(g3.sides[0].diploScore, before, "not once the other side offers it too");
  }
  const g3 = newGame("ERYTHEA", { seed: 63 });
  g3.sides.forEach((s, i) => { s.diploScore = (i + 1) * 10; });
  const r = d.ratings(g3);
  eq(r[g3.sides[0].index], "Statesman", "the lowest score is the Statesman");
  eq(r[g3.sides[g3.sides.length - 1].index], "Running Dog", "the highest is the Running Dog");
  const ranks = d.RATING_RANKS[2];
  eq(ranks.length, 2, "two sides take two titles");
  eq(d.TITLES[ranks[0] - 1], "Statesman", "best of two");
  eq(d.TITLES[ranks[1] - 1], "Running Dog", "worst of two");
  const g4 = newGame("ERYTHEA", { seed: 64, options: { diplomacy: 1 } });
  const side = game.begin(g4);
  let victim = null;
  for (const s of g4.sides) if (s.index !== side.index) victim = s;
  const city = victim.capital;
  const army = { x: city.x, y: city.y - 1, owner: side.index, type: 11, name: "Scouts", strength: 3, maxMoves: 20, moves: 20, upkeep: 1 };
  g4.armies.push(army);
  const path = movement.findPath(g4, [army], army.x, army.y, city.x, city.y);
  if (path && path.length > 0) {
    const r2 = movement.walk(g4, [army], path);
    eq(r2.stopped, "at peace", "a stack at peace cannot walk into their city");
    eq(army.x, city.x, "and has not moved into it");
  }

  // but a stack of a side at peace in the open is passed over: the walk may
  // not stop on it, and goes on beyond it (1a8b:07f9)
  const other = victim;
  const W = g4.map.width;
  const open = (x, y) => {
    const t = scn.terrainAt(g4.map, x, y);
    return t !== movement.WATER && t !== movement.SHORE && t !== movement.CITY &&
      t !== movement.BRIDGE && !g4.map.cityTile[y * W + x] && game.armiesAt(g4, x, y).length === 0;
  };
  let sx = null, sy = null;
  for (let y = 10; y <= g4.map.height - 10; y++) {
    for (let x = 10; x <= W - 10; x++) {
      if (sx === null && open(x, y) && open(x + 1, y) && open(x + 2, y)) { sx = x; sy = y; }
    }
  }
  const walker = { x: sx, y: sy, owner: side.index, type: 11, name: "Scouts", strength: 3, maxMoves: 20, moves: 20, upkeep: 1 };
  const blocker = { x: sx + 1, y: sy, owner: other.index, type: 11, name: "Scouts", strength: 3, maxMoves: 20, moves: 20, upkeep: 1 };
  g4.armies.push(walker, blocker);
  const steps = [{ x: sx + 1, y: sy, cost: 2 }, { x: sx + 2, y: sy, cost: 2 }];
  const r3 = movement.walk(g4, [walker], steps);
  eq(walker.x, sx + 2, "a stack at peace in the open is passed over");
  eq(walker.moves, 16, "paying for the step past it");
  eq(r3.stopped, "arrived", "and the walk arrives");
  walker.x = sx; walker.moves = 20;
  g4.diplomacy.state[side.index * 8 + other.index] = d.WAR;
  const r4 = movement.walk(g4, [walker], steps);
  eq(r4.stopped, "attack", "a stack at war is attacked");
  eq(walker.x, sx, "from where the walker stands");

  // ruins are open ground: a fight there is for no city, and takes none
  const ruin = g4.map.cities.find((c) => c.ownerIndex != null && c.ownerIndex !== side.index);
  game.raze(g4, g4.map.sides[ruin.ownerIndex], ruin, [], true);
  for (let i = g4.armies.length - 1; i >= 0; i--) {
    const a = g4.armies[i];
    if (a.x != null && a.x >= ruin.x && a.x <= ruin.x + 1 && a.y >= ruin.y && a.y <= ruin.y + 1) g4.armies.splice(i, 1);
  }
  g4.diplomacy.state[side.index * 8 + other.index] = d.WAR;
  g4.armies.push({ x: ruin.x, y: ruin.y, owner: other.index, type: 11, name: "Scouts", strength: 1, maxMoves: 20, moves: 20, upkeep: 1 });
  g4.armies.push({ x: ruin.x + 1, y: ruin.y + 1, owner: other.index, type: 11, name: "Scouts", strength: 1, maxMoves: 20, moves: 20, upkeep: 1 });
  const strike = [];
  for (let i = 0; i < 8; i++) {
    const a = { x: ruin.x - 1, y: ruin.y, owner: side.index, type: 11, name: "Scouts", strength: 9, maxMoves: 20, moves: 20, upkeep: 1 };
    g4.armies.push(a);
    strike.push(a);
  }
  const fight = game.resolveAttack(g4, strike, ruin.x, ruin.y);
  eq(fight.lines.city ?? null, null, "a fight on ruins is not for a city");
  eq(fight.lines.defenders.length, 1, "and only the tile fought for defends");
  eq(fight.captured ?? null, null, "nothing is captured");
  eq(ruin.ownerIndex ?? null, null, "the ruins belong to nobody");
}

function testQuests() {
  console.log("quests");
  const q = quest;
  const off = newGame("ERYTHEA", { seed: 71 });
  const sideOff = game.begin(off);
  const heroOff = { x: 10, y: 10, owner: sideOff.index, type: armytype.HERO, strength: 5, experience: 0, items: [] };
  eq(q.assign(off, sideOff, heroOff), null, "no quests when the option is off");
  const g = newGame("ERYTHEA", { seed: 71, options: { quests: 1 } });
  const side = game.begin(g);
  const h = { x: side.capital.x, y: side.capital.y, owner: side.index, type: armytype.HERO, strength: 5, experience: 0, items: [] };
  g.armies.push(h);
  const quest1 = q.assign(g, side, h);
  ok(quest1 != null, "a quest is assigned");
  ok(quest1.type >= 0 && quest1.type <= 6, "with a known type");
  ok(quest1.target != null, "and a target");
  eq(q.assign(g, side, h), null, "only one quest at a time");
  ok(q.describe(quest1).length > 0, "a quest describes itself");
  const counts = {};
  for (const t of q.TYPE_TABLE) counts[t] = (counts[t] || 0) + 1;
  eq(counts[0], 1, "type 0 has one slot");
  eq(counts[4], 2, "type 4 has two");
  eq(counts[6], 2, "type 6 has two");
  eq(q.TYPE_TABLE.length, 10, "the table is rolled with 1d10");
  const C = (i) => g.map.cities[i - 1];      // the Lua's cities[i]
  side.quest = { type: q.OCCUPY, hero: h, target: C(2), done: 0 };
  side.gold = 5000;
  let r = q.event(g, side, "occupy", { city: C(2), stack: [h] });
  ok(r && !r.failed, "occupying the target completes the quest");
  eq(side.quest, null, "and the quest is cleared");
  eq(h.experience, q.EXPERIENCE, "the hero gains 10 experience");
  ok(r.reward != null, "a reward was given");
  const wasComputer = side.computer;
  side.computer = false; side.questNews = null;
  side.quest = { type: q.OCCUPY, hero: h, target: C(2), done: 0 };
  r = q.event(g, side, "occupy", { city: C(2), stack: [h] });
  eq(side.questNews, r, "the human keeps the news to be shown");
  side.computer = true; side.questNews = null;
  side.quest = { type: q.OCCUPY, hero: h, target: C(2), done: 0 };
  q.event(g, side, "occupy", { city: C(2), stack: [h] });
  eq(side.questNews, null, "the computer's quest ends unannounced");
  side.computer = wasComputer; side.questNews = null;
  h.experience = 0;
  side.quest = { type: q.OCCUPY, hero: h, target: C(3), done: 0 };
  r = q.event(g, side, "occupy", { city: C(3), stack: [] });
  ok(r && r.failed, "occupying without the hero fails the quest: " + (r && r.failed));
  eq(r.why, 0x22, "said as 'Alas! The city was not taken by thy hero!'");
  eq(h.experience, 0, "and pays no experience");
  const c5 = C(5);
  const owner5 = c5.ownerIndex, razed5 = c5.razed;
  side.quest = { type: q.RAZE, hero: h, target: c5, done: 0 };
  c5.razed = true;
  r = q.event(g, side, "turn");
  ok(r && r.failed && r.why === 0x21, "a raze quest's city razed by another fails it");
  c5.razed = false; c5.ownerIndex = side.index;
  side.quest = { type: q.RAZE, hero: h, target: c5, done: 0 };
  r = q.event(g, side, "turn");
  ok(r && r.failed && r.why === 0x2a, "one taken and kept fails it too");
  c5.razed = razed5; c5.ownerIndex = owner5;
  side.quest = { type: q.OCCUPY, hero: h, target: C(4), done: 0 };
  r = q.event(g, side, "raze", { city: C(4), stack: [h] });
  ok(r && r.failed, "razing a city you were to keep fails the quest");
  const victim = g.sides[g.sides.length - 1];
  side.quest = { type: q.SLAUGHTER, hero: h, target: victim, required: 3, done: 0 };
  const dead = [{ owner: victim.index }, { owner: victim.index }];
  eq(q.event(g, side, "battle", { stack: [h], killed: dead }), null, "two of three is not enough");
  eq(side.quest.done, 2, "the count rises");
  r = q.event(g, side, "battle", { stack: [h], killed: dead });
  ok(r && !r.failed, "the third kill completes it");
  side.quest = { type: q.SLAUGHTER, hero: h, target: victim, required: 2, done: 0 };
  q.event(g, side, "battle", { stack: [], killed: dead });
  eq(side.quest.done, 0, "kills away from the hero do not count");
  side.quest = { type: q.PILLAGE_GOLD, hero: h, target: true, required: 100, done: 0 };
  q.event(g, side, "pillage", { stack: [h], gold: 60 });
  eq(side.quest.done, 60, "pillaged gold counts");
  r = q.event(g, side, "pillage", { stack: [h], gold: 60 });
  ok(r && !r.failed, "reaching the total completes it");
  const item = { index: 99, name: "Testsword", type: rules.ITEM_BATTLE, value: 1, status: 3 };
  h.items = [item];
  side.quest = { type: q.RETRIEVE_ITEM, hero: h, target: item, done: 0 };
  r = q.event(g, side, "item", { hero: h });
  ok(r && !r.failed, "carrying the item completes the quest");
  ok(!h.items.includes(item), "the quest item is taken away (the reward may add another)");
  eq(item.status, 0, "it leaves play");
  const prey = { type: armytype.HERO, owner: victim.index };
  side.quest = { type: q.SLAY_HERO, hero: h, target: prey, done: 0 };
  eq(q.event(g, side, "battle", { stack: [], killed: [prey] }), null, "someone else killing the quarry does not count");
  r = q.event(g, side, "battle", { stack: [h], killed: [prey] });
  ok(r && !r.failed, "the hero killing the quarry completes it");
  side.quest = { type: q.PILLAGE_GOLD, hero: { type: armytype.HERO }, required: 1, done: 0 };
  r = q.event(g, side, "turn");
  ok(r && r.failed, "a quest with a dead hero is abandoned: " + (r && r.failed));
  side.gold = 50;
  let reward = q.reward(g, side, { hero: h });
  eq(reward.kind, "gold", "a side with no gold is given gold");
  ok(reward.gold >= 1002 && reward.gold <= 3000, "2d1000+1000: " + reward.gold);
  g.turn = 20;
  side.gold = 5000;
  reward = q.reward(g, side, { hero: h });
  eq(reward.kind, "allies", "a small side past turn 15 is given allies");
  ok(reward.armies.length >= 1 && reward.armies.length <= 8, "1d3+5 allies arrive");
}

function testEndGame() {
  console.log("end of the game");
  const g = newGame("ERYTHEA", { seed: 81 });
  for (const s of g.sides) s.computer = true;
  const winner = g.sides[0];
  for (const c of g.map.cities) c.ownerIndex = c.ownerIndex != null ? winner.index : undefined;
  for (let i = 1; i < g.sides.length; i++) g.sides[i].alive = false;
  const r = game.checkEnd(g);
  ok(r.triumph, "the last side standing has triumphed");
  ok(!r.over, "and the game goes on, to be looked over");
  eq(r.winner, winner, "the last side standing wins");
  ok(!winner.computer, "and is switched to human control");
  ok(g.won, "the game-won flag is set");
  ok(!r.noHumans, "a game that never had a human does not say the last one fell");
  const g2 = newGame("ERYTHEA", { seed: 82 });
  for (const c of g2.map.cities) c.ownerIndex = undefined;
  ok(!game.checkEnd(g2).over, "sides still in the game are players, cities or not");
  const r2 = game.endRound(g2);
  ok(r2.over, "with no cities owned the game is over at the round's end");
  eq(r2.winner ?? null, null, "and nobody won");
  ok(r2.message.includes("No more players"), "with the right message");
  eq(r2.fallen.length, g2.sides.length, "every side fell");

  // the round's end puts a side with no city out (8065:18ab)
  const g6 = newGame("ERYTHEA", { seed: 86, options: { diplomacy: 1 } });
  g6.sides.forEach((s, i) => { s.computer = i > 0; });
  game.begin(g6);
  const doomed = g6.sides[1], other = g6.sides[2];
  for (const c of game.sideCities(g6, doomed)) c.ownerIndex = other.index;
  g6.diplomacy.state[doomed.index * 8 + other.index] = diplomacy.WAR;
  g6.diplomacy.state[other.index * 8 + doomed.index] = diplomacy.WAR;
  ok(game.sideArmies(g6, doomed).length > 0, "the doomed side still has armies");
  game.endTurn(g6);
  eq(g6.side, other, "a computer side with no city has no turn");
  ok(doomed.alive, "but it is not out before the round's end");
  while (g6.turn === 1) game.endTurn(g6);
  ok(!doomed.alive, "the round's end puts it out");
  eq(game.sideArmies(g6, doomed).length, 0, "with all its armies");
  eq(doomed.gold, 0, "and no gold");
  eq(diplomacy.state(g6, other.index, doomed.index), diplomacy.INTERMEDIATE, "others stand uneasy with it");
  eq(diplomacy.proposal(g6, doomed.index, other.index), diplomacy.INTERMEDIATE, "both ways, proposals too");
  const f = g6.ending && g6.ending.fallen && g6.ending.fallen[0];
  ok(f && f.side === doomed, "its fall is told");
  ok(f && f.line >= 0 && f.line < game.FALLEN_LINES, "in a line of group 11");
  ok(f && f.boxed, "in a box while a human plays");

  // a human left with no city still plays its turn until the round's end
  const g7 = newGame("ERYTHEA", { seed: 87 });
  g7.sides.forEach((s, i) => { s.computer = i < g7.sides.length - 1; });
  game.begin(g7);
  const me7 = g7.sides[g7.sides.length - 1];
  for (const c of game.sideCities(g7, me7)) c.ownerIndex = g7.sides[0].index;
  for (let i = 1; i < g7.sides.length; i++) game.endTurn(g7);
  eq(g7.side, me7, "the human's turn comes round without a city");
  ok(me7.alive, "and it is still in the game");
  game.endTurn(g7);
  ok(!me7.alive, "the round's end puts it out");
  ok(me7.computer, "a fallen human is the computer's from then on");
  ok(g7.ending.noHumans, "the last human's fall is said");
  ok(g7.noHumansSaid, "once");
  for (let i = 0; i < g7.sides.length; i++) game.endTurn(g7);
  ok(!(g7.ending && g7.ending.noHumans), "and only once");
  const g3 = newGame("ERYTHEA", { seed: 83 });
  for (const s of g3.sides) s.computer = false;
  const me = g3.sides[0];
  for (let i = 1; i < g3.sides.length; i++) g3.sides[i].alive = false;
  const standing = g3.map.cities.length;
  let given = 0;
  for (const c of g3.map.cities) c.ownerIndex = undefined;
  for (const c of g3.map.cities) if (given < Math.floor(standing / 2)) { c.ownerIndex = me.index; given++; }
  ok(!game.checkEnd(g3).over, "exactly half is not enough");
  for (const c of g3.map.cities) if (c.ownerIndex == null) { c.ownerIndex = me.index; break; }
  const r3 = game.checkEnd(g3);
  ok(r3.won, "more than half wins");
  ok(!r3.over, "and the game goes on, to be looked over");
  eq(r3.winner, me, "and names the winner");
  ok(!game.checkEnd(g3).won, "which is said once");
  const g4 = newGame("ERYTHEA", { seed: 84 });
  const human = g4.sides[0];
  human.computer = false;
  for (let i = 1; i < g4.sides.length; i++) g4.sides[i].computer = true;
  for (const c of g4.map.cities) c.ownerIndex = undefined;
  const rival = g4.sides[1];
  const n = g4.map.cities.length;
  g4.map.cities.forEach((c, i) => {
    if (i + 1 <= Math.floor(n * 3 / 4)) c.ownerIndex = human.index;
    else if (i + 1 === n) c.ownerIndex = rival.index;
  });
  const r4 = game.checkEnd(g4);
  ok(r4.surrender, "a dominant human is offered surrender");
  ok(!r4.over, "but the game is not over");
  ok(g4.surrenderOffered, "and the flag is set");
  ok(!game.checkEnd(g4).surrender, "the offer is made once");
  const g5 = newGame("ERYTHEA", { seed: 85 });
  const side5 = g5.sides[0];
  for (let i = 1; i < g5.sides.length; i++) g5.sides[i].alive = false;
  for (const s of g5.sides) s.computer = false;
  for (const c of g5.map.cities) c.ownerIndex = undefined;
  g5.map.cities[0].ownerIndex = side5.index;
  for (let i = 1; i < g5.map.cities.length; i++) g5.map.cities[i].razed = true;
  ok(game.checkEnd(g5).won, "one city among ruins is still more than half");
  game.acceptSurrender(g4, human);
  ok(g4.won, "surrender accepted wins the game");
  ok(!rival.alive, "and the computer sides are out");
}

// ------------------------------------------------------------ saving a game

// The computer's neighbour table is shared by every game of a map, but each
// game must read it as its own cities, not the first game's.
function testNeighbourCache() {
  console.log("neighbour table across games");
  const g1 = newGame("ERYTHEA", { seed: 5 });
  const [first, firstDist] = aicore.neighbours(g1, g1.map.cities[0]);
  ok(first.length > 0, "a city has neighbours");
  const g2 = newGame("ERYTHEA", { seed: 6 });
  const mine = new Set(g2.map.cities);
  const [again, dist] = aicore.neighbours(g2, g2.map.cities[0]);
  eq(again.length, first.length, "the second game gets as many neighbours");
  ok(again.every((c, i) => mine.has(c) && c.index === first[i].index),
     "and they are the second game's own cities, the same ones by index");
  eq(dist[0], firstDist[0], "at the same distances");
}

function testSave() {
  console.log("save and load");
  const q = quest;
  const g = newGame("ERYTHEA", { seed: 91, options: { quests: 1, diplomacy: 1 } });
  let side = game.begin(g);
  while (side && g.turn <= 8) { ai.runSync(ai.playTurn(g, side)); side = game.endTurn(g); }
  let h = null;
  for (const a of g.armies) if (a.type === armytype.HERO) h = a;
  ok(h != null, "the game has a hero to save");
  const owner = g.map.sides[h.owner];
  owner.quest = { type: q.OCCUPY, hero: h, target: g.map.cities[4], targetKind: "city", done: 0 };
  h.items = [g.map.items[0]];
  g.map.items[0].status = 3;
  g.armies[0].group = 4; g.armies[1].group = 4;
  g.armies[0].fortified = true;
  const text = saveMod.encode(g);
  ok(text.length > 1000, "the save has content");
  const h2 = saveMod.decode(text, DATA);
  eq(h2.turn, g.turn, "the turn survives");
  eq(h2.current, g.current, "whose turn it is survives");
  eq(h2.armies.length, g.armies.length, "every army survives");
  eq(h2.rng.state, g.rng.state, "the dice carry on where they left off");
  g.armies.forEach((a, i) => {
    const b = h2.armies[i];
    eq(b.x, a.x, "army " + i + " keeps its place");
    eq(b.type, a.type, "army " + i + " keeps its type");
    eq(b.strength, a.strength, "army " + i + " keeps its strength");
    eq(b.moves, a.moves, "army " + i + " keeps its movement");
    eq(b.owner, a.owner, "army " + i + " keeps its owner");
    eq(b.group || 0, a.group || 0, "army " + i + " keeps the group it moves with");
    eq(b.fortified || false, a.fortified || false, "army " + i + " stays dug in");
  });
  g.map.cities.forEach((c, i) => {
    eq(h2.map.cities[i].ownerIndex, c.ownerIndex, "city " + i + " keeps its owner");
    eq(h2.map.cities[i].producing, c.producing, "city " + i + " keeps its production");
    eq(h2.map.cities[i].slots.length, c.slots.length, "city " + i + " keeps its slots");
  });
  g.sides.forEach((s, i) => {
    eq(h2.sides[i].gold, s.gold, s.name + " keeps its gold");
    eq(h2.sides[i].alive, s.alive, s.name + " keeps its standing");
    eq(h2.sides[i].diploScore || 0, s.diploScore || 0, s.name + " keeps its score");
  });
  g.map.sites.forEach((s, i) => {
    eq(h2.map.sites[i].content, s.content, "site " + i + " keeps its contents");
    eq(h2.map.sites[i].searched, s.searched, "site " + i + " remembers being searched");
    eq(h2.map.sites[i].rich, s.rich, "site " + i + " keeps its rich flag");
  });
  const owner2 = h2.map.sides[owner.index];
  ok(owner2.quest != null, "the quest survives");
  eq(owner2.quest.type, q.OCCUPY, "with its type");
  eq(owner2.quest.target.index, g.map.cities[4].index, "and its target city");
  ok(owner2.quest.hero != null, "and its hero");
  eq(owner2.quest.hero.type, armytype.HERO, "which is a hero");
  ok(h2.armies.some((a) => a.items && a.items.length > 0), "carried items survive");
  eq(diplomacy.state(h2, 0, 1), diplomacy.state(g, 0, 1), "the diplomatic state survives");
  let side2 = h2.sides[h2.current];
  const before = h2.turn;
  for (let i = 0; i < h2.sides.length * 2; i++) {
    if (!side2) break;
    ai.runSync(ai.playTurn(h2, side2));
    side2 = game.endTurn(h2);
  }
  ok(h2.turn > before, "the reloaded game plays on");
  // the Lua's file round trip is the browser's localStorage here: a save is
  // a string, and a string read back must give the same game
  const h3 = saveMod.decode(String(text), DATA);
  eq(h3.armies.length, g.armies.length, "a save kept as text reads back");
}

// ------------------------------------------------------------ the hidden map

function testHiddenMap() {
  console.log("hidden map");
  const off = newGame("ERYTHEA", { seed: 101 });
  ok(game.seen(off, 0, 5, 5), "with the option off every tile is seen");
  eq(game.reveal(off, 0, 5, 5, false), 0, "and nothing is revealed");
  const g = newGame("ERYTHEA", { seed: 101, options: { hiddenMap: 1 } });
  const side = game.begin(g);
  ok(game.seen(g, side.index, side.capital.x, side.capital.y), "its capital is seen");
  ok(!game.seen(g, side.index, 0, 0), "the far corner is not");
  const g2 = newGame("ERYTHEA", { seed: 102, options: { hiddenMap: 1 } });
  let open = null;
  for (let y = 20; y <= 60; y++) for (let x = 20; x <= 60; x++) {
    if (!open && !game.cityAt(g2, x, y)) open = { x, y };
  }
  eq(game.reveal(g2, 20, open.x, open.y, false), 9, "in the open a stack sees 3x3");
  eq(game.reveal(g2, 20, open.x, open.y, false), 0, "seeing it again reveals nothing new");
  eq(game.reveal(g2, 20, open.x + 20, open.y, true), 25, "a flying stack sees 5x5");
  const city = g2.map.cities[0];
  eq(game.reveal(g2, 21, city.x, city.y, false), 25, "standing on a city sees 5x5");
  const army = game.sideArmies(g, side)[0];
  army.moves = 99;
  let unseen = null;
  for (const c of g.map.cities) {
    if (!unseen && !game.seen(g, side.index, c.x, c.y) && c.ownerIndex !== side.index) unseen = c;
  }
  ok(unseen != null, "there is a city the side has not seen");
  eq(movement.findPath(g, [army], army.x, army.y, unseen.x, unseen.y), null, "a side cannot path into the dark");
  const count = () => { let n = 0; const m = g.explored[side.index]; if (m) for (const v of m) if (v) n++; return n; };
  const before = count();
  let target = null;
  for (let dx = -3; dx <= 3; dx++) for (let dy = -3; dy <= 3; dy++) {
    const x = army.x + dx, y = army.y + dy;
    if (!target && x >= 0 && y >= 0 && game.seen(g, side.index, x, y) && (x !== army.x || y !== army.y)) {
      const p = movement.findPath(g, [army], army.x, army.y, x, y);
      if (p && p.length > 0 && p[p.length - 1].cost < movement.PAST_SHORE) target = { x, y };
    }
  }
  if (target) {
    movement.moveTo(g, [army], target.x, target.y);
    const after = count();
    ok(after > before, fmt("walking uncovered %d more tiles", after - before));
  }
  let other = null;
  for (const s of g.sides) if (s.index !== side.index) other = s;
  ok(!game.seen(g, other.index, side.capital.x, side.capital.y), "another side has not seen our capital");
}

// --------------------------------------------------------------------- bugs

function testBugFlags() {
  console.log("bug compatibility");
  ok(rules.bugs.heroExperienceReadsAttackerTypes, "the original's bugs are reproduced by default");
  const used = new Set();
  for (const module of ["hero", "combat", "game", "move", "ai", "site", "quest"]) {
    const p = new URL("../src/warlords/" + module + ".js", import.meta.url);
    if (fs.existsSync(p)) {
      for (const m of fs.readFileSync(p, "utf8").matchAll(/rules\.bugs\.(\w+)/g)) used.add(m[1]);
    }
  }
  for (const name in rules.bugs) ok(used.has(name), "the " + name + " flag is read by the engine");
}

// ------------------------------------------------------------ the computer AI

function testComputerPlayers() {
  console.log("computer players");
  for (let level = 0; level <= 2; level++) eq(aicard.count(DATA, level), 9, "nine cards for level " + level);
  const w = aicard.load(DATA, 2, 0);
  eq(w.groups, 4, "the Standard Warlord runs four assault groups");
  eq(w.raze, 5, "and razes 5 in 1000");
  eq(w.humanShare, 3, "and turns on a human with 3x10+1d10 percent");
  eq(w.solidarity, 1, "and stands with the other computers");
  const k = aicard.load(DATA, 0, 0);
  eq(k.cautious, 1, "the Standard Knight is cautious");
  eq(k.groups, 1, "and runs one group");
  eq(aicard.describe(DATA, 2, 1)[0], "Attila the Hun", "a card's name is the first line of its .DSC");
  eq(new Rng(5).dice(1, 0, 3), 3, "dice(1, 0, 3) is 3");
  eq(aicore.dist(0, 0, 3, 4), 5, "distance is Euclidean");
  eq(aicore.dist(0, 0, 1, 1), 1, "and truncated");

  const sides = [];
  for (let i = 0; i <= 7; i++) sides[i] = { computer: true, level: i % 3 };
  sides[0] = { computer: false, level: 0 };
  const g = newGame("ERYTHEA", { seed: 71, sides });
  const knight = g.map.sides[3], lord = g.map.sides[1], warlord = g.map.sides[2];
  eq(knight.level, 0, "side 3 plays as a Knight");
  eq(knight.ai.maxGroups, 1, "a Knight runs one group");
  eq(knight.ai.cautious, 1, "and is cautious");
  ok(!knight.ai.bold, "and never attacks a side it is not at war with");
  eq(lord.ai.maxGroups, 3, "a Lord three");
  ok(lord.ai.bold, "a Lord is bold -- the level sets it and a card cannot clear it");
  eq(warlord.ai.maxGroups, 4, "a Warlord four");
  ok(warlord.ai.humanShare >= 31 && warlord.ai.humanShare <= 40,
    "a Warlord turns on a human holding 31-40% of the world: " + warlord.ai.humanShare);
  ok(knight.ai.humanShare >= 81 && knight.ai.humanShare <= 90, "a Knight at 81-90%");
  eq(g.map.fightOrder[3][8], w.fightOrder[8], "the card sets the side's fight order");
  let unclaimed = 0;
  for (const c of g.map.cities) if (c.claim == null || c.claim === aicore.NEUTRAL) unclaimed++;
  eq(unclaimed, 0, "the computers claim every city between them (623c:010b)");
  for (const s of g.sides) eq(s.capital.claim, s.index, s.name + " claims its capital");
  for (const s of g.sides) ok(s.diploScore >= 1 && s.diploScore <= 8, "a diplomatic score starts at 1d8");

  const gg = newGame("ERYTHEA", { seed: 72, sides, greatest: true, options: { diplomacy: 1 } });
  ok(gg.map.sides[0].diploScore > 400, "I am the Greatest: a human's score starts above 400");
  ok(gg.map.sides[1].diploScore <= 8, "a computer's does not");
  gg.diplomacy.state[1 * 8 + 0] = diplomacy.WAR; gg.diplomacy.state[0 * 8 + 1] = diplomacy.WAR;
  const fights = aigroups.fightingHumans(gg);
  ok(fights[1], "a computer at war with the human is marked");
  aidiplomacy.phase(gg, gg.map.sides[1]);
  eq(diplomacy.proposal(gg, 1, 2), diplomacy.PEACE, "and no computer proposes war on it");

  const h = newGame("ERYTHEA", { seed: 73, sides, options: { hiddenMap: 1 } });
  const human = h.map.sides[0], comp = h.map.sides[1];
  const hp = movement.prepare(h, movement.grid(h, human.index), human.index, movement.LAND, 0, 0);
  const cp = movement.prepare(h, movement.grid(h, human.index), comp.index, movement.LAND, 0, 0);
  let fogged = 0;
  for (let k2 = 0; k2 < h.map.width * h.map.height; k2++) {
    if (hp[k2] === movement.SHUT && cp[k2] !== movement.SHUT) fogged++;
  }
  ok(fogged > 1000, "unseen ground blocks the human's paths only: " + fogged);

  let army = null;
  for (const a of g.armies) if (a.owner === warlord.index) army = a;
  const sel = aicore.select(g, [army]);
  eq(aicore.odds(g, sel, 0, 0), 100, "nothing to fight is a sure thing");
  const [nb, nd] = aicore.neighbours(g, g.map.cities[0]);
  eq(nb.length, 6, "a city has six neighbours");
  for (const v of nd) ok(v < 100, "each within reach");

  eq(aigroups.pickEnemy(g, warlord), null, "no enemy is picked in the first turns");
  warlord.ai.battles[4] = 3;
  warlord.ai.citiesLost[4] = 2;
  ok(aigroups.pickEnemy(g, warlord) != null, "a side that has fought us can be picked");

  let c = g.map.cities[0];
  for (const cc of g.map.cities) if (cc !== knight.capital && cc !== warlord.capital) { c = cc; break; }
  c.ownerIndex = warlord.index;
  const before = warlord.diploScore;
  ok(aigroups.earlyVengeance(g, warlord.index, c, 0), "a Warlord takes vengeance on a human's city");
  ok(warlord.diploScore > before, "and it is an atrocity");
  ok(!aigroups.earlyVengeance(g, knight.index, c, 0), "a Knight does not");

  let side = game.begin(g);
  while (side && g.turn <= 12) {
    if (side.computer) ai.runSync(ai.playTurn(g, side));
    side = game.endTurn(g);
  }
  const grp = warlord.ai.groups[1];
  grp.active = 2; grp.target = 1; grp.rally = warlord.capital.index;
  grp.staged[2] = g.armies[g.armies.length - 1];
  const g2 = saveMod.decode(saveMod.encode(g), DATA);
  const w2 = g2.map.sides[warlord.index].ai;
  eq(w2.maxGroups, warlord.ai.maxGroups, "the AI data survives a save");
  eq(w2.groups[1].rally, warlord.capital.index, "and its groups");
  eq(w2.groups[1].staged[2], g2.armies[g2.armies.length - 1], "with their staged armies");
  eq(g2.map.cities[4].claim, g.map.cities[4].claim, "and the cities' claims");
}

// --------------------------------------------------------------- encampments

function testEncampment() {
  console.log("encampments");
  const g = newGame("ERYTHEA", { seed: 5 });
  const side = game.begin(g);
  const cap = side.capital;
  const a = g.armies.find((b) => b.owner === side.index && !b.transit);
  const freePlain = () => {
    for (let r = 2; r <= 8; r++) for (let dx = -r; dx <= r; dx++) for (let dy = -r; dy <= r; dy++) {
      const x = cap.x + dx, y = cap.y + dy;
      if (x >= 0 && y >= 0 && x < g.map.width - 1 && y < g.map.height
          && scn.terrainAt(g.map, x, y) === movement.PLAIN
          && scn.terrainAt(g.map, x + 1, y) === movement.PLAIN
          && (scn.roadAt(g.map, x, y) || 0) % 32 === 0
          && game.armiesAt(g, x, y).length === 0 && game.armiesAt(g, x + 1, y).length === 0) return [x, y];
    }
    return [null, null];
  };
  const [x, y] = freePlain();
  ok(a != null && x != null, "an army and open ground to encamp on");
  if (!(a && x != null)) return;
  a.x = x; a.y = y;
  ok(!game.towerAt(g, x, y), "a stack on open ground is no encampment");
  game.startTurn(g, side);
  ok(!game.towerAt(g, x, y), "nor after a turn in the army cycle");
  a.fortified = true;
  game.startTurn(g, side);
  ok(game.towerAt(g, x, y), "a defended stack encamps as its turn opens");
  eq(combat.fortify(g, [], x, y, combat.terrainClass(g, x, y)), 1, "an encampment fights as a fortification of 1");
  const r = movement.moveTo(g, [a], x + 1, y);
  eq(r.steps, 1, "the encamped stack walks off");
  ok(!game.towerAt(g, x, y), "and the empty tile is no encampment any more");
  a.x = cap.x; a.y = cap.y;
  game.startTurn(g, side);
  ok(!game.towerAt(g, cap.x, cap.y), "a defended stack in a city does not encamp");
}

// ------------------------------------------------------------- going to sea

function testSea() {
  console.log("going to sea");
  const move = movement;
  let g = newGame("ERYTHEA", { seed: 81 });
  let lx = null, ly, wx, wy;
  for (let y = 1; y <= g.map.height - 2; y++) for (let x = 1; x <= g.map.width - 2; x++) {
    const t2 = scn.terrainAt(g.map, x + 1, y);
    if (lx == null && scn.terrainAt(g.map, x, y) === move.PLAIN && (t2 === move.WATER || t2 === move.SHORE)
        && game.armiesAt(g, x, y).length === 0 && game.armiesAt(g, x + 1, y).length === 0) {
      lx = x; ly = y; wx = x + 1; wy = y;
    }
  }
  ok(lx != null, "Erythea has a plain beside the sea");
  let dragon = null;
  for (const t of g.types.list) if (t.flies && t.name.includes("Dragon")) dragon = t;
  const army = (typeId) => {
    const a = { type: typeId, owner: 0, x: lx, y: ly, strength: 5, maxMoves: 20, moves: 20, upkeep: 0 };
    g.armies.push(a);
    return a;
  };
  const hero = army(armytype.HERO);
  const d = army(dragon.id);
  const stack = [hero, d];
  eq(move.modeOf(g, stack), move.FLYING, "a hero with a dragon flies");
  move.walk(g, stack, [{ x: wx, y: wy, cost: 2 }]);
  eq(hero.x, wx, "and flies out over the water");
  ok(!d.atSea && !hero.atSea, "without either going to sea");
  eq(move.modeOf(g, stack), move.FLYING, "so it still flies");
  move.walk(g, stack, [{ x: lx, y: ly, cost: 2 }]);
  eq(hero.x, lx, "and can come back to land");
  const foot = army(1);
  move.settleSea(g, [foot], wx, wy, move.LAND, false);
  ok(foot.atSea, "a land army ending on water is at sea");
  move.settleSea(g, [foot], lx, ly, move.LAND, true);
  ok(!foot.atSea, "and ashore again on land");
  move.settleSea(g, [hero, d, foot], wx, wy, move.LAND, false);
  ok(!d.atSea, "a flier never goes to sea");
  ok(hero.atSea && foot.atSea, "the hero and the footman with it do");
  hero.atSea = false; foot.atSea = false;
  move.settleSea(g, [foot], wx, wy, move.BOAT, false);
  ok(!foot.atSea, "a boat move leaves the flag as it was");
  hero.x = wx; hero.y = wy; d.x = wx; d.y = wy;
  hero.atSea = true; d.atSea = true;
  removeArmy(g, foot);
  const g2 = saveMod.decode(saveMod.encode(g), DATA);
  for (const a of g2.armies) if (a.x === wx && a.y === wy) ok(!a.atSea, "a loaded flier and its hero are not at sea");

  g = newGame("ERYTHEA", { seed: 81 });
  const mirea = g.map.cities.find((c) => c.name === "Mirea");
  ok(mirea && mirea.ownerIndex === 0 && move.isPort(g, mirea), "Mirea is side 0's port");
  const grid = move.grid(g, 0);
  const byte = (x, y) => grid[y * g.map.width + x];
  eq(scn.terrainAt(g.map, 86, 7), move.SHORE, "the shore runs past Mirea");
  ok(move.has(byte(86, 7), move.WATER_F), "and a shore tile is water to a walker");
  eq(move.stepCost(byte(83, 9), byte(82, 9), move.LAND, false, false, 10), null,
    "so a land stack cannot step onto it from a plain");
  const sailor = { type: 1, owner: 0, x: mirea.x, y: mirea.y, strength: 5, maxMoves: 20, moves: 20, upkeep: 0 };
  g.armies.push(sailor);
  let r = move.moveTo(g, [sailor], 86, 4);
  eq(sailor.y, 7, "leaving port it stops on the first sea tile");
  eq(scn.terrainAt(g.map, sailor.x, sailor.y), move.SHORE, "the shore");
  ok(sailor.atSea, "at sea");
  eq(r.spent, move.COST[move.SHORE], "paying the tile's cost and no water charge");
  eq(sailor.moves, 0, "and going to sea uses up the rest of its move");
  eq(r.stopped, "out of moves", "which ends the walk");
  sailor.moves = 20;
  r = move.moveTo(g, [sailor], 86, 4);
  eq(sailor.y, 4, "next turn it sails out into open water");
  ok(sailor.atSea, "still at sea");
  eq(sailor.moves, 20 - (7 - 4) * move.COST[move.WATER], "paying 1 a tile");
  eq(move.findPath(g, [sailor], 86, 4, 86, 5)[0].cost, move.COST[move.WATER], "sailing on costs no water charge either");
  r = move.moveTo(g, [sailor], mirea.x, mirea.y);
  eq(sailor.x * 1000 + sailor.y, mirea.x * 1000 + mirea.y, "it sails back into port");
  ok(!sailor.atSea, "and comes ashore there");
  eq(sailor.moves, 0, "which uses up its move too");
  const kuuria = g.map.cities.find((c) => c.name === "Kuuria");
  let sx = null, sy;
  for (let x = kuuria.x - 1; x <= kuuria.x + 2; x++) for (let y = kuuria.y - 1; y <= kuuria.y + 2; y++) {
    const t = scn.terrainAt(g.map, x, y);
    if (sx == null && (t === move.WATER || t === move.SHORE) && !g.map.crossing[y * g.map.width + x]
        && game.armiesAt(g, x, y).length === 0) { sx = x; sy = y; }
  }
  ok(sx != null, "Kuuria has open water beside it");
  sailor.x = sx; sailor.y = sy; sailor.atSea = true; sailor.moves = 20;
  ok(move.findPath(g, [sailor], sx, sy, kuuria.x, kuuria.y) != null, "a stack at sea can path into the port it attacks");
}

// --------------------------------------------------------------------- sound

// The Lua checks its OPL synthesis here too; the browser plays recordings,
// so only what plays when is left to check.
function testSound() {
  console.log("sound");
  const files = uidata.strings(DATA + "/DATA/FILE.DAT");
  const first = () => 1;
  let [name, loop] = cues.song(files, cues.TITLE, [], first);
  eq(name, "STARTUP.XMI", "the start screens play STARTUP.XMI");
  ok(loop, "and it starts again when it ends");
  [name, loop] = cues.song(files, cues.COMPUTER, [{ computer: false, alive: true }], first);
  eq(name, "INT2.XMI", "a computer's turn beside a human plays FILE.DAT group 11");
  ok(!loop, "once");
  eq(cues.song(files, cues.BEGIN, [], first)[0], "INT22.XMI", "the war begins to INT22");
  const g = { turn: 1, map: { cities: [] }, armies: [] };
  const side = { index: 0, gold: 500 };
  for (let i = 0; i < 6; i++) g.map.cities[i] = { ownerIndex: 0 };
  eq(cues.advisor(g, side, first), null, "the advisor never speaks on turn 1");
  eq(side.advisor.mark, 5, "but he has marked six cities down as five");
  g.turn = 2;
  eq(cues.advisor(g, side, first), null, "nothing new, not a seventh turn: silence");
  for (let i = 0; i < 5; i++) g.map.cities[i].ownerIndex = 1;
  eq(cues.advisor(g, side, first), 40, "down to one city: 'Thy sorry efforts...' (VLOSE05)");
  eq(side.advisor.dir, 2, "and he remembers saying so");
  for (let i = 0; i < 5; i++) g.map.cities[i].ownerIndex = 0;
  eq(cues.advisor(g, side, first), 47, "back to six: 'Thou art doing well... so far!'");
  g.turn = 7;
  side.gold = 50;
  eq(cues.advisor(g, side, first), 54, "on a seventh turn, short of gold: VGOLD00");
}

// --------------------------------------------------------------------- main

if (!vfs.exists(DATA + "/TERRAIN0/ARMYTYPE.DAT")) {
  console.log("cannot find the game data in web/assets/data: run tools/web_assets.py");
  process.exit(1);
}

testDice();
testArmyTypes();
testProduction();
testBugFlags();
for (const s of SCENARIOS) if (vfs.exists(DATA + "/" + s + "/" + s + ".SCN")) testGame(s);
testTurnLoop("ERYTHEA");
testVectoring("ERYTHEA");
testBuyProduction("ERYTHEA");
testReports("ERYTHEA");
testHeroItems("ERYTHEA");
testSage("ERYTHEA");
testSetup();
testHistory("ERYTHEA");
testDisband("ERYTHEA");
testMovement("ERYTHEA");
testMovement("ISLADIA");
testStackLimit("ERYTHEA");
testSea();
testEncampment();
testCombat("ERYTHEA");
testCapture("ERYTHEA");
testCityChoices("ERYTHEA");
testHeroes("ERYTHEA");
testHeroExperienceBug();
testDiplomacy();
testQuests();
testEndGame();
testHiddenMap();
testSave();
testNeighbourCache();
testSites("ERYTHEA");
testSites("DRAGON");
testSlots();
testScreenLayout();
testCityCastles();
testComputerPlayers();
testAIGame("TUTORIA", 30);
testAIGame("ERYTHEA", 25);
if (vfs.exists(DATA + "/TUTORIA/TUTORIA.SCN")) testTutorialHero();
testSound();

console.log(fmt("\n%d passed, %d failed", passed, failed));
process.exit(failed === 0 ? 0 : 1);
