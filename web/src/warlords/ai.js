// The computer players.
//
// A port of WARLORD2.EXE's AI (docs/re/ai.md). Every side has a block of AI
// data (ai/core.js) filled at game start from its level and its character
// card (docs/formats/crd.md); each computer turn runs the original's nineteen
// phases in the original's order (ai_turn, 5db9:0000).
//
//   ai/core.js       data, neighbours, the selection, flood, battles, walk
//   ai/cities.js     evaluate, garrisons, neutrals, production, vectoring
//   ai/groups.js     the assault groups and picking an enemy
//   ai/moves.js      standing orders, rescue, explorers, hero parties
//   ai/heroes.js     the hero phase
//   ai/diplomacy.js  proposals
//
// A turn is a generator: it yields whatever the front end's hooks yield
// (a walk to show, a battle, a pause), and the front end resumes it when the
// showing is done. runSync plays one through without stopping.

import * as aicard from "./aicard.js";
import * as core from "./ai/core.js";
import * as cities from "./ai/cities.js";
import * as groups from "./ai/groups.js";
import * as moves from "./ai/moves.js";
import * as heroes from "./ai/heroes.js";
import * as diplo from "./ai/diplomacy.js";
import * as heroMod from "./hero.js";
import * as quest from "./quest.js";

export { core };

/** What the front end listens with. onWalk and onFight are generator
 *  functions -- they may yield to have the turn wait -- and onSpoils a plain
 *  one. */
export const hooks = { onWalk: null, onFight: null, onSpoils: null };

/** Drain a generator, ignoring what it yields; returns what it returns. */
export function runSync(gen) {
  let r = gen.next();
  while (!r.done) r = gen.next();
  return r.value;
}

/** The level's built-in settings (59bf:0d7b). */
function levelDefaults(g, d, level) {
  const r = g.rng;
  if (level === 0) {
    d.rebuildLimit = 30; d.cautious = 1;
    d.dieHuman = r.dice(1, 4, 0); d.dieWarlord = r.dice(1, 4, 0); d.dieLord = r.dice(1, 4, 0);
    d.maxGroups = 1; d.solidarity = 0;
    d.rebuildType = 3; d.rebuildTypeRich = 3;
    d.raze = 0; d.sack = 0; d.pillage = 0; d.perCity = 0; d.bonusHuman = 0;
    d.bonusWarlord = 0; d.bonusLord = 0; d.bonusKnight = 0; d.poor = 0; d.early = 0;
    d.humanShare = 80;
  } else if (level === 1) {
    d.rebuildLimit = 20; d.cautious = 0;
    d.dieHuman = r.dice(1, 4, 0); d.dieWarlord = r.dice(1, 4, 0); d.dieKnight = r.dice(1, 8, 0);
    d.bold = true;
    d.solidarity = 0; d.maxGroups = 2;
    d.rebuildType = 6; d.rebuildTypeRich = 18;
    d.raze = 5; d.sack = 10; d.pillage = 20; d.perCity = 1; d.bonusHuman = 5;
    d.bonusKnight = 0; d.bonusLord = 0; d.bonusWarlord = 0; d.poor = 0; d.early = 0;
    d.humanShare = 35;
  } else {
    d.rebuildLimit = 10; d.cautious = 0;
    d.dieHuman = r.dice(1, 10, 0); d.dieLord = r.dice(1, 8, 0); d.dieKnight = r.dice(1, 6, 0);
    d.maxGroups = 4; d.solidarity = 1;
    d.bold = true;
    d.rebuildType = 7; d.rebuildTypeRich = 0;
    d.raze = 5; d.sack = 10; d.pillage = 20; d.perCity = 5; d.bonusHuman = 50;
    d.bonusKnight = 0; d.bonusLord = 0; d.bonusWarlord = 0; d.poor = 50; d.early = 1;
    d.humanShare = 35;
  }
}

/** Fill a computer side's AI data from its card (59bf:0d7b), dice and all. */
function fromCard(g, side, d, card) {
  const r = g.rng;
  d.dieHuman = r.dice(1, card.dieHuman, 0);
  d.dieLord = r.dice(1, card.dieLord, 0);
  d.dieKnight = r.dice(1, card.dieKnight, 0);
  d.dieWarlord = r.dice(1, card.dieWarlord, 0);
  d.maxGroups = Math.max(0, Math.min(core.MAX_GROUPS, card.groups));
  d.solidarity = card.solidarity;
  if (card.bold !== 0) d.bold = true;
  d.cautious = card.cautious;
  d.rebuildType = card.rebuildType; d.rebuildTypeRich = card.rebuildTypeRich;
  d.rebuildLimit = card.rebuildLimit;
  d.raze = card.raze; d.sack = card.sack; d.pillage = card.pillage; d.perCity = card.perCity;
  d.bonusHuman = card.bonusHuman; d.bonusWarlord = card.bonusWarlord;
  d.bonusLord = card.bonusLord; d.bonusKnight = card.bonusKnight;
  d.poor = card.poor; d.early = card.early;
  d.humanShare = card.humanShare * 10 + r.dice(1, 10, 0);
  // the card's fight order replaces the side's
  g.map.fightOrder[side.index] = card.fightOrder;
}

/** A side's AI data at game start (ai_init_side, 59bf:084d). */
export function initSide(g, side) {
  const d = core.newData(g);
  side.ai = d;
  const quick = g.map.options.quickStart !== 0;
  for (const c of g.map.cities) {
    d.roles[c.index] = quick ? core.WEAK : 0;
    d.held[c.index] = 0;
    d.flags[c.index] = g.map.options.hiddenMap !== 0 ? core.CF_UNSEEN : 0;
  }
  let level = 2;
  if (side.computer && (side.level === 0 || side.level === 1)) level = side.level;
  levelDefaults(g, d, level);
  if (side.inUse && side.computer) {
    const card = g.dataDir != null ? aicard.load(g.dataDir, level, side.card || 0) : null;
    if (card) fromCard(g, side, d, card);
  }
  return d;
}

/** Split the cities no side starts with among the computer players, for the
 *  AI to think of as its own ground (623c:010b). */
function shareOutCities(g) {
  for (const c of g.map.cities) c.claim = c.ownerIndex != null ? c.ownerIndex : core.NEUTRAL;
  const shares = [];
  let humans = 0, computers = 0;
  const from = {};
  for (let i = 0; i < 8; i++) {
    const s = g.map.sides[i];
    shares[i] = false;
    if (s && s.inUse) {
      if (s.computer) { shares[i] = true; computers++; } else humans++;
      from[i] = [s.capX, s.capY];
      const c = core.cityAt(g, s.capX, s.capY);
      if (c) c.claim = i;
    }
  }
  if (computers === 0 && humans !== 0) {
    let best = null, bestRoll = null;
    for (let i = 7; i >= 0; i--) {
      const s = g.map.sides[i];
      if (s && s.inUse) {
        shares[i] = true;
        const roll = g.rng.dice(1, 100, 0);
        if (best === null || bestRoll < roll) { best = i; bestRoll = roll; }
      }
    }
    if (best !== null) shares[best] = false;
  }
  let turn = null;
  for (let i = 7; i >= 0; i--) if (shares[i]) { turn = i; break; }
  if (turn === null) return;
  for (;;) {
    const [x, y] = from[turn];
    let pick = null, bestD = null;
    for (let i = g.map.cities.length - 1; i >= 0; i--) {
      const c = g.map.cities[i];
      if (core.standing(c) && c.claim === core.NEUTRAL) {
        const d = core.dist(x, y, c.x, c.y);
        if (bestD === null || d < bestD) { pick = c; bestD = d; }
      }
    }
    if (!pick) break;
    pick.claim = turn;
    const s = g.map.sides[turn];
    if (g.rng.dice(1, 10, -1) < 5) from[turn] = [s.capX, s.capY];
    else from[turn] = [pick.x, pick.y];
    do { turn = (turn + 1) % 8; } while (!shares[turn]);
  }
}

/** Set the computer players up for a new game (79fa:0000). */
export function startGame(g) {
  for (const s of g.map.sides) initSide(g, s);
  shareOutCities(g);
  for (const s of g.map.sides) {
    s.diploScore = g.rng.dice(1, 8, 0);
    if (g.greatest && !s.computer) s.diploScore += 400;
  }
}

/** ai_turn_setup (5db9:0386). */
function turnSetup(g, side) {
  const d = core.data(g, side);
  side.aiSolidarity = d.solidarity;
  const quick = g.map.options.quickStart !== 0 && g.map.options.hiddenMap !== 0;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (c.ownerIndex === side.index) {
      const r = core.role(d, c);
      if (r === core.EXPLORER || r === core.EXPLORER2) {
        core.setRole(d, c, g.turn < 5 ? core.WEAK : core.STOP);
      }
      if (quick) {
        if (g.turn === 1) core.setRole(d, c, core.EXPLORER);
        else if (g.turn < 3) core.setRole(d, c, core.EXPLORER2);
      }
      core.clearCflag(d, c, core.CF_CLEANED);
    }
  }
}

/** Play one computer turn: ai_turn (5db9:0000), phase by phase. `pause`, if
 *  given, is a generator function called between phases with the phase's
 *  name, so a front end can show progress. */
export function* playTurn(g, side, pause) {
  const p = pause || function* () {};
  const hidden = g.map.options.hiddenMap !== 0;

  // the computer always hires an offered hero it can afford
  if (side.heroOffer && side.gold >= (side.heroOffer.price || 0)) {
    heroMod.recruit(g, side, side.heroOffer);
    side.heroOffer = null;
  }
  for (const a of g.armies) if (a.owner === side.index) a.aiMoved = undefined;

  turnSetup(g, side);
  diplo.phase(g, side);                        yield* p("diplomacy");
  yield* heroes.phase(g, side);                yield* p("move hero");
  if (hidden) yield* moves.search(g, side);
  yield* p("move search");
  yield* moves.heroParties(g, side);           yield* p("move explore");
  yield* groups.assault(g, side);              yield* p("assault");
  yield* moves.moveAll(g, side);               yield* p("move #1");
  yield* moves.rescue(g, side);                yield* p("rescue");
  cities.evaluate(g, side, side.index);        yield* p("evaluate");
  yield* cities.clean(g, side);                yield* p("clean city");
  yield* cities.neutral(g, side);              yield* p("neutral");
  yield* moves.moveAll(g, side);               yield* p("move #2");
  yield* cities.quickAttack(g, side);
  if (hidden) cities.updateHide(g, side);
  groups.assaultXX(g, side);                   yield* p("assault XX");
  yield* moves.specials(g, side);              yield* p("specials");
  cities.rebuild(g, side);                     yield* p("rebuilding");
  yield* moves.lastRescue(g, side);            yield* p("last rescue");
  cities.production(g, side);                  yield* p("production");
  cities.vectoring(g, side);                   yield* p("vectoring");
}

/** Every walk a computer stack makes is reported here once it is made, so a
 *  front end can show it. */
export function* walked(g, stack, r) {
  if (hooks.onWalk && r && (r.steps || 0) > 0) yield* hooks.onWalk(g, stack, r);
}

/** Where a hired hero appears for a computer (ai_hero_city, 5db9:0919). */
export function heroCity(g, side, dflt) {
  const d = core.data(g, side);
  let best = dflt, bestScore = 0;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (c.ownerIndex === side.index) {
      const r = core.role(d, c);
      let score = 0;
      if (r === core.RALLY) score = g.rng.dice(1, 100, 100);
      else if (r === core.TAKING_NEUTRAL) score = g.rng.dice(1, 100, 50);
      else if (r === core.NEAR_NEUTRAL) score = g.rng.dice(1, 100, 0);
      if (bestScore < score) { best = c; bestScore = score; }
    }
  }
  return best;
}

/** A computer's quest hero has taken a city (5e97:038d). True when it razed
 *  or sacked, so the capture is not also taken for an occupation. */
export function questCapture(g, side, c, stack) {
  const q = side.quest;
  if (g.map.options.quests === 0 || !q || q.target !== c) return false;
  if (q.type !== quest.OCCUPY && q.type !== quest.RAZE) return false;
  if (!(stack || []).includes(q.hero)) return false;
  const d = core.data(g, side);
  d.questsDone++;
  d.questCity = undefined;
  if (q.type === quest.RAZE) {
    groups.raze(g, side, c, true, core.select(g, stack));
    return true;
  }
  return false;
}

/** A battle has been fought on a side's tile (ai_record_battle, 5db9:09d7). */
export function recordBattle(g, defender, attacker, x, y, heroesLost, armiesLost, allLost, cityTile) {
  if (defender == null || attacker == null || defender === core.NEUTRAL) return;
  const d = core.data(g, defender);
  d.heroesKilled[attacker] += heroesLost;
  d.armiesKilled[attacker] += armiesLost;
  d.battles[attacker]++;
  if (allLost) d.lost[attacker]++;
  if (cityTile) {
    d.cityBattles[attacker]++;
    if (allLost) d.citiesLost[attacker]++;
  }
}
