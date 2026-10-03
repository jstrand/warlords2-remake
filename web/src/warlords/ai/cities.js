// The computer players' city phases: evaluate, clean city, neutral, quick
// attack, update hide, rebuilding, production and vectoring.
// docs/re/ai.md; addresses are Ghidra's.

import * as rules from "../rules.js";
import * as game from "../game.js";
import * as core from "./core.js";
import * as groups from "./groups.js";
import * as moves from "./moves.js";
import { luaSort } from "../../util.js";

// garrison wanted on the first tile, by how many armies the city holds
// (DS:0824), 8 from thirteen up
const KEEP = [0, 0, 0, 0, 3, 3, 4, 5, 5, 6, 7, 7, 7];

// the city's four tiles in the order the garrison fills them (DS:0814/081c)
const TILE_DX = [1, 0, 0, 1];
const TILE_DY = [0, 0, 1, 1];

// the roles that make a city try a quick attack (DS:0932)
const QUICK_ROLES = new Set([5, 8, 6, 4, 14, 7]);

/** The side's own cities, last first, as the original walks them. */
export function own(g, side) {
  const out = [];
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (c.ownerIndex === side.index) out.push(c);
  }
  return out;
}

/** The neutral cities among a city's neighbours (57ea:01f6): standing, not
 *  the quest city, and -- after the first two places and the first found --
 *  less than 20 away. A list of { city, dist, slot } (slot from 1). */
export function neutralNeighbours(g, side, c) {
  const d = core.data(g, side);
  const [nb, nd] = core.neighbours(g, c);
  const out = [];
  for (let j = 0; j < nb.length; j++) {
    const n = nb[j];
    if (n.index !== d.questCity && core.standing(n) && n.ownerIndex == null
        && (out.length === 0 || j < 2 || nd[j] < 20)) {
      out.push({ city: n, dist: nd[j], slot: j + 1 });
    }
  }
  return out;
}

/** The distance from (x, y) to the nearest army of another side -- or of side
 *  `of` -- that the side can see (57ea:0a3e). 1000 when there is none. */
export function nearestArmy(g, side, x, y, of) {
  let best = 1000;
  for (let i = g.armies.length - 1; i >= 0; i--) {
    const a = g.armies[i];
    if (a.owner != null && !a.transit) {
      const ok = of == null ? a.owner !== side.index : a.owner === of;
      if (ok && core.explored(g, side.index, a.x, a.y)) {
        const dd = core.dist(x, y, a.x, a.y);
        if (dd < best) best = dd;
      }
    }
  }
  return best;
}

function slotStrength(g, slot) {
  let s = slot.strength;
  if (g.types.byId[slot.type].siege) s += 2;
  return s;
}

/** Can this city build something worth having (623c:1a4d)? [good, slot]. */
export function buildsWell(g, side, c) {
  const d = core.data(g, side);
  const slot = rules.bestSlot(c.slots, 3, g.types, side.enhanced);
  if (!slot) return [false, null];
  const s = slotStrength(g, slot);
  if ((d.own < 12 || s > 2) && (d.own < 8 || s > 1)) return [true, slot];
  return [false, slot];
}

/** The slot to buy over (623c:1ae9), from 0: the first empty one, else the
 *  weakest under 10. null when all four are full of strong types. */
function slotToReplace(g, c) {
  if (c.slots.length < rules.PRODUCTION_SLOTS) return c.slots.length;
  let best = null, least = 10;
  c.slots.forEach((slot, i) => {
    const s = slotStrength(g, slot);
    if (s < least) { best = i; least = s; }
  });
  return best;
}

/** Is a city part way through building something (623c:0fd5)? */
export function building(c) {
  if (c.producing == null) return false;
  const slot = c.slots[c.producing];
  return slot != null && c.countdown !== 0 && c.countdown !== slot.time;
}

/** set_city_production (623c:0e5a): refused after turn 5 when the side has
 *  less than the type's cost + 30 in gold. */
function setProduction(g, side, c, slot) {
  for (let i = 0; i < c.slots.length; i++) {
    const s = c.slots[i];
    if (s === slot) {
      if (side.gold < (s.cost || 0) + 30 && g.turn > 5) return false;
      game.setProduction(g, c, i);
      return true;
    }
  }
  return false;
}

function produceFor(g, side, c, purpose) {
  const slot = rules.bestSlot(c.slots, purpose, g.types, side.enhanced);
  if (slot) setProduction(g, side, c, slot);
}

/** Vector a city's production (623c:0f10): only while it is building. */
export function vector(g, c, dest) {
  if (!building(c)) c.vectorTo = undefined;
  else c.vectorTo = dest ? dest.index : undefined;
}

/** ai_buy_production_type (5db9:0bd1), if the side has 30 over its price. */
function buyType(g, side, c, typeId) {
  const t = g.types.byId[typeId];
  if (!t) return false;
  if (!(side.gold > t.price + 30)) return false;
  const n = slotToReplace(g, c);
  if (n == null) return false;
  game.buyProduction(g, side, c, n, typeId);
  game.sortProduction(c);
  const d = core.data(g, side);
  d.bought++;
  c.producing = undefined; c.countdown = 0; c.vectorTo = undefined;
  core.setRole(d, c, core.BUILDING);
  return true;
}

/** Buy the first flying type the side can afford into a city (623c:11d5). */
function buyFlier(g, side, c) {
  const n = slotToReplace(g, c);
  if (n == null) return;
  const d = core.data(g, side);
  for (let id = 0; id <= 28; id++) {
    const t = g.types.byId[id];
    if (t && t.flies && (t.bonus[48] || 0) === 0 && t.price + 30 <= side.gold) {
      game.buyProduction(g, side, c, n, id);
      game.sortProduction(c);
      core.setCflag(d, c, core.CF_FLIER);
      c.producing = undefined; c.countdown = 0; c.vectorTo = undefined;
      return;
    }
  }
}

function markFliers(g, d) {
  for (const c of g.map.cities) {
    if (core.standing(c)) {
      core.clearCflag(d, c, core.CF_FLIER);
      for (const slot of c.slots) {
        if (g.types.byId[slot.type].flies) core.setCflag(d, c, core.CF_FLIER);
      }
    }
  }
}

/** Clear "not seen yet" for cities the side has seen round (59bf:0b55). */
export function clearUnseen(g, side) {
  if (g.map.options.hiddenMap === 0) return;
  const d = core.data(g, side);
  for (const c of g.map.cities) {
    if (core.cflag(d, c, core.CF_UNSEEN)) {
      let seen = false;
      for (let x = c.x - 1; x <= c.x + 2; x++) {
        for (let y = c.y - 1; y <= c.y + 2; y++) {
          if (x >= 0 && y >= 0 && x < g.map.width && y < g.map.height && game.seen(g, side.index, x, y)) {
            seen = true;
          }
        }
      }
      if (seen) core.clearCflag(d, c, core.CF_UNSEEN);
    }
  }
}

/** evaluate (ai_phase_evaluate, 59bf:0000). */
export function evaluate(g, side, who) {
  const d = core.data(g, side);
  markFliers(g, d);
  clearUnseen(g, side);
  d.own = 0; d.enemy = 0; d.neutral = 0; d.unseen = 0;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (core.standing(c)) {
      if (c.ownerIndex === who) {
        d.own++;
        d.held[c.index] = (d.held[c.index] || 0) + 1;
        if (core.role(d, c) === 0) core.setRole(d, c, core.JUST_TAKEN);
      } else {
        if (core.cflag(d, c, core.CF_UNSEEN)) d.unseen++;
        core.setRole(d, c, 0);
        d.held[c.index] = 0;
        if (c.ownerIndex == null) d.neutral++; else d.enemy++;
      }
    } else {
      core.setRole(d, c, 0);
      d.held[c.index] = 0;
    }
  }
  if (d.own !== 0) {
    for (let i = g.map.cities.length - 1; i >= 0; i--) {
      const c = g.map.cities[i];
      if (core.role(d, c) === core.STOP && d.unseen !== 0 && d.explorers < 5
          && core.cflag(d, c, core.CF_FLIER)) {
        core.setRole(d, c, core.EXPLORER2);
      }
      if (core.role(d, c) === core.JUST_TAKEN) {
        core.setRole(d, c, neutralNeighbours(g, side, c).length > 0 ? core.NEAR_NEUTRAL : core.BUILDING);
      }
    }
    d.turns++;
  }
}

/** The garrison a city wants (ai_wanted_garrison, 5ca7:0a3d). */
export function wanted(g, side, c) {
  const [nb] = core.neighbours(g, c);
  let foreign = 0, war = 0;
  for (let j = nb.length - 1; j >= 0; j--) {
    const n = nb[j];
    if (n.ownerIndex != null && n.ownerIndex !== side.index && core.standing(n)) {
      foreign++;
      if (core.state(g, side.index, n.ownerIndex) === 2) war++;
    }
  }
  if (war !== 0) return 8;
  if (foreign >= 2) return 4;
  if (foreign === 1) return 3;
  return 2;
}

/** Put the city's armies in the order they stand in (5ca7:0b5c): the ones to
 *  keep on the first tile first. [list, keep]. */
function arrange(g, side, c, list) {
  const d = core.data(g, side);
  const n = list.length;
  let keepWanted = KEEP[n] != null ? KEEP[n] : 8;
  let mask = 0, minMax = 0, keep = 0;
  let attack = false;
  const pool = list.slice();
  if (core.role(d, c) === core.RALLY) {
    let fliers = 0, heroes = 0;
    for (let i = n - 1; i >= 0; i--) {
      const a = pool[i];
      if (core.isHero(a)) heroes++;
      else if (core.flies(g, a) && ((a.strength || 0) > 3 || fliers === 0)) fliers++;
    }
    if (fliers !== 0 && heroes !== 0) fliers += heroes;
    if ((fliers > 3 && n < 17) || (fliers > 2 && n < 9)) attack = true;
    for (let gi = core.MAX_GROUPS; gi >= 1; gi--) {
      const grp = d.groups[gi];
      if (grp.active !== 0 && grp.rally === c.index) {
        if (core.has(grp.flags, core.GF_MOVE12)) minMax = 12;
        else if (core.has(grp.flags, core.GF_MOVE16)) minMax = 16;
        else minMax = grp.size;
        if (core.has(grp.flags, core.GF_FLY)) attack = true;
      }
    }
  }
  const k = 3 - Math.floor((d.held[c.index] || 0) / 3);
  if (k > 0 && k < n && n - keepWanted < k) keepWanted = n - k;

  const out = [];
  for (;;) {
    let bestKeep = -1, bestKeepI = null, bestOther = -1, bestOtherI = null;
    for (let i = n - 1; i >= 0; i--) {
      const a = pool[i];
      if (a) {
        let score = a.strength || 0;
        if (out.length < keepWanted) {
          const ab = core.ability(g, a);
          if (!core.has(mask, 1) && core.isHero(a)) {
            score += 1000;
          } else if (minMax <= (a.maxMoves || 0)) {
            if (!core.has(mask, 0x10) && core.flies(g, a)) score += 900;
            else if (!attack) {
              if (!core.has(mask, 2) && ab === 2) score += 800;
              else if (!core.has(mask, 4) && ab === 3) score += 700;
              else if (!core.has(mask, 0x80) && core.magical(g, a)) score += 600;
              else if (!core.has(mask, 8) && ab === 1) score += 500;
              else if (!core.has(mask, 0x20) && core.woods(g, a)) score += 400;
              else if (!core.has(mask, 0x40) && core.hills(g, a)) score += 300;
            }
          }
        } else {
          mask = 0xfff;
        }
        if (core.has(mask, 1) && core.isHero(a)) score = 1;
        if (core.has(mask, 0x80) && core.magical(g, a)) score = 2;
        if ((a.maxMoves || 0) < minMax || (attack && !core.flies(g, a) && !core.isHero(a))) {
          if (bestOther < score) { bestOther = score; bestOtherI = i; }
        } else if (bestKeep < score) {
          bestKeep = score; bestKeepI = i;
        }
      }
    }
    if (bestKeepI === null && bestOtherI === null) break;
    if (mask !== 0xfff) {
      const a = pool[bestKeepI !== null ? bestKeepI : bestOtherI];
      const ab = core.ability(g, a);
      if (core.isHero(a)) mask = core.set(mask, 1);
      else {
        if (ab === 2) mask = core.set(mask, 2);
        if (ab === 3) mask = core.set(mask, 4);
        if (core.flies(g, a)) mask = core.set(mask, 0x10);
        if (core.magical(g, a)) mask = core.set(mask, 0x80);
        if (ab === 1) mask = core.set(mask, 8);
        if (core.woods(g, a)) mask = core.set(mask, 0x20);
        if (core.hills(g, a)) mask = core.set(mask, 0x40);
      }
    }
    if (bestKeepI === null) {
      out.push(pool[bestOtherI]); pool[bestOtherI] = null;
    } else {
      out.push(pool[bestKeepI]); pool[bestKeepI] = null;
      keep++;
    }
  }
  if (keepWanted < keep) keep = keepWanted;
  return [out, keep];
}

function disband(g, side, a) {
  game.disband(g, side, [a]);
}

/** Hand items round between a city's heroes (6087:17db, 1a3d). */
function shareItems(g, d, heroes) {
  const count = (h, types) => (h.items || []).filter((it) => types.has(it.type)).length;
  const give = (from, to, types) => {
    const items = from.items || [];
    for (let i = 0; i < items.length; i++) {
      const it = items[i];
      if (types.has(it.type)) {
        items.splice(i, 1);
        to.items = to.items || [];
        to.items.push(it);
        d.itemsPassed++;
        return true;
      }
    }
    return false;
  };
  for (const t of [new Set([6]), new Set([5]), new Set([7])]) {
    let moved = true;
    while (moved) {
      moved = false;
      for (const h of heroes) {
        if (count(h, t) > 1) {
          for (const o of heroes) {
            if (o !== h && count(o, t) === 0 && count(h, t) > 1) {
              if (give(h, o, t)) moved = true;
            }
          }
        }
      }
    }
  }
  // battle, command and standard items: each hero passes on only what it
  // started with, so nothing goes back and forth
  const fightT = new Set([1, 2, 8]);
  const start = [], ownN = [];
  heroes.forEach((h, i) => {
    start[i] = (h.items || []).filter((it) => fightT.has(it.type));
    ownN[i] = start[i].length;
  });
  let moved = true;
  while (moved) {
    moved = false;
    heroes.forEach((h, i) => {
      if (start[i].length > 0) {
        let least = ownN[i], to = null;
        for (let j = 0; j < heroes.length; j++) {
          if (j !== i && ownN[j] < least) { least = ownN[j]; to = j; }
        }
        if (to !== null) {
          const it = start[i].shift();
          const k = h.items.indexOf(it);
          if (k >= 0) h.items.splice(k, 1);
          const o = heroes[to];
          o.items = o.items || [];
          o.items.push(it);
          d.itemsPassed++;
          ownN[i]--; ownN[to]++;
          moved = true;
        }
      }
    });
  }
}

/** Look over a city's armies and stand them where they belong
 *  (ai_city_garrison_check, 5ca7:023f). A generator: a hidden map's explorers
 *  set off from here. */
export function* garrison(g, side, c, force) {
  const d = core.data(g, side);
  const cx = c.x, cy = c.y;
  let last = d.garrison[c.index] || 0;
  core.setCflag(d, c, core.CF_CLEANED);
  d.garrison[c.index] = 0;
  const list = [], heroes = [];
  let ordered = 0, fliers = 0, magic = 0;
  let lastHero = null;
  for (let i = g.armies.length - 1; i >= 0; i--) {
    const a = g.armies[i];
    if (a && !a.transit && a.x != null && a.x >= cx && a.x <= cx + 1 && a.y >= cy && a.y <= cy + 1) {
      if (a.owner !== side.index) {
        disband(g, g.map.sides[a.owner == null ? 8 : a.owner] || side, a);
      } else {
        d.garrison[c.index]++;
        if (a.aiOrder === core.ORDER_CITY && a.aiDest === c.index) core.clearOrder(a);
        if ((a.aiOrder || 0) !== 0) ordered++;
        if (core.isHero(a) && heroes.length < 8) { heroes.push(a); lastHero = a; }
        if (core.flies(g, a)) fliers++;
        if (core.magical(g, a)) magic++;
        if (list.length < 32) list.push(a); else disband(g, side, a);
      }
    }
  }
  if (lastHero) core.pickUp(g, { hero: lastHero, armies: [lastHero] });
  if (heroes.length > 1) shareItems(g, d, heroes);
  core.clearCflag(d, c, core.CF_HERO);
  core.clearCflag(d, c, core.CF_MAGIC);
  if (heroes.length !== 0) core.setCflag(d, c, core.CF_HERO);
  if (magic !== 0) core.setCflag(d, c, core.CF_MAGIC);
  if (list.length > 16) { last = 0; ordered = 0; }

  const role = core.role(d, c);
  if (!((ordered < 3 || role === core.RALLY)
        && (force || heroes.length !== 0 || d.garrison[c.index] !== last
            || g.rng.dice(1, 6, -1) === 0
            || game.armiesAt(g, cx + 1, cy + 1).length > 0))) {
    return true;
  }
  d.keep[c.index] = 0;
  if (role === core.WEAK || role === core.BUILDING || role === core.STOP) {
    const want = wanted(g, side, c);
    if (list.length < 2) core.setRole(d, c, core.WEAK);
    else if (list.length < want) core.setRole(d, c, core.BUILDING);
    else core.setRole(d, c, core.STOP);
  }
  const [sorted, keepN] = arrange(g, side, c, list);
  const n = sorted.length;
  if (!force && side.gold < 300 && (side.income || 0) < (side.upkeepTotal || 0) * 2) {
    if (core.role(d, c) !== core.RALLY && (d.held[c.index] || 0) > 10
        && heroes.length + magic + 4 < n && nearestArmy(g, side, cx, cy) > 10) {
      for (let i = heroes.length + magic + 4; i < n; i++) {
        if (sorted[i]) { disband(g, side, sorted[i]); sorted[i] = false; }
      }
    }
  }
  if (n > 24) {
    for (let i = 24; i < n; i++) {
      if (sorted[i]) { disband(g, side, sorted[i]); sorted[i] = false; }
    }
  }
  if ((g.map.options.quickStart === 0 || g.turn > 2) && n < 2) core.setRole(d, c, core.WEAK);
  if (g.map.options.hiddenMap !== 0 && core.role(d, c) === core.EXPLORER2 && d.explorers < 5) {
    let go = fliers;
    const short = 2 - (n - fliers);
    if (short > 0) go = fliers - short;
    if (g.map.options.quickStart !== 0 && g.turn < 3) go = 1;
    for (let i = 0; i < n; i++) {
      const a = sorted[i];
      if (go > 0 && a && core.alive(g, a) && core.flies(g, a)) {
        d.explorers++;
        a.aiExplore = true;
        yield* moves.explore(g, side, a, null);
        sorted[i] = false;
        go--;
      }
    }
  }
  let tile, perTile;
  const r = core.role(d, c);
  if (r === core.NEAR_NEUTRAL || r === core.TAKING_NEUTRAL) {
    tile = 1; perTile = 8;
  } else {
    tile = keepN === 0 ? 1 : 0;
    perTile = keepN === 0 ? 8 : keepN;
  }
  for (let i = 0; i < n; i++) {
    const a = sorted[i];
    if (a && core.alive(g, a) && tile <= 3) {
      if (tile === 0) d.keep[c.index] = (d.keep[c.index] || 0) + 1;
      a.x = cx + TILE_DX[tile]; a.y = cy + TILE_DY[tile];
      a.atSea = false;
      a.target = undefined;
      a.group = undefined;
      a.aiGroup = 0;
      a.done = undefined;
      perTile--;
      if (perTile === 0) { tile++; perTile = 8; }
    }
  }
  return true;
}

/** clean city (ai_phase_clean_city, 5ca7:01f1). */
export function* clean(g, side) {
  const d = core.data(g, side);
  for (const c of own(g, side)) {
    if (!core.cflag(d, c, core.CF_CLEANED)) yield* garrison(g, side, c, false);
  }
}

/** Send stacks from a city at the neutral cities round it (57ea:06c7). */
function* takeNeutrals(g, side, c, count, heading, stack) {
  const d = core.data(g, side);
  const neutral = g.map.options.neutralCities !== 0;
  let odds = 100;
  let from = c;
  let lastFrom = null, lastPick = null;
  for (;;) {
    stack = stack.filter((a) => core.alive(g, a) && a.owner === side.index);
    if (stack.length === 0) return;
    let sel = core.select(g, stack);
    const nb = neutralNeighbours(g, side, from);
    if (nb.length === 0) return;
    let best = null, bestScore = 1000;
    for (let j = nb.length - 1; j >= 0; j--) {
      const e = nb[j];
      const n = e.city;
      if (!core.cflag(d, n, core.CF_UNSEEN) && (heading[n.index] || 0) < 3) {
        if (neutral) odds = core.odds(g, sel, n.x, n.y);
        if (odds > 74) {
          const score = g.rng.dice(1, 10, 0) + e.dist + (heading[n.index] || 0) * 10
                      + (e.dist >= 41 ? 10 : 0) + (e.dist >= 51 ? 30 : 0) + (100 - odds);
          if (score < bestScore) { best = n; bestScore = score; }
        }
      }
    }
    if (!best) break;
    heading[best.index] = (heading[best.index] || 0) + count;
    lastFrom = c; lastPick = best;
    sel = core.order(g, stack, core.ORDER_CITY, best.index, 0x80);
    if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
    if (best.ownerIndex !== side.index) return;
    if (d.cautious !== 0) return;
    from = best;
    if (nearestArmy(g, side, best.x, best.y) < 10) return;
  }
  if (g.map.options.hiddenMap !== 0 && lastFrom === c && lastPick) {
    core.order(g, stack, core.ORDER_CITY, lastPick.index, 0x80);
  }
}

/** How many armies a city holds back (57ea:03aa): [extra, lost, near]. */
function holdBack(g, side, c) {
  const d = core.data(g, side);
  const near = nearestArmy(g, side, c.x, c.y);
  const extra = (near < 5 ? 2 : 0) + (near < 15 ? 1 : 0) + (d.cautious !== 0 ? 1 : 0);
  let lost = 0;
  for (let i = 0; i < 8; i++) lost += d.citiesLost[i];
  return [extra, Math.min(2, lost), near];
}

/** The idle armies on a city's corner tile, beyond the ones held back, the
 *  fastest first. */
function idle(g, side, c, reserve, test) {
  const found = [];
  for (const a of core.onTile(g, side.index, c.x, c.y)) {
    if ((a.aiGroup || 0) === 0 && !a.done && (a.aiOrder || 0) === 0 && (!test || test(a))) {
      if (reserve > 0) reserve--;
      else if (found.length < 8) found.push(a);
    }
  }
  luaSort(found, (p, q) => (p.maxMoves || 0) > (q.maxMoves || 0));
  return found;
}

/** Send a city's idle armies at its neutral neighbours (57ea:03aa). True with
 *  the map hidden (the city then sends explorers). */
function* attackNeutrals(g, side, c, heading) {
  const [extra, lost] = holdBack(g, side, c);
  const list = idle(g, side, c, extra + lost);
  if (list.length === 0) return false;
  const per = g.map.options.neutralCities !== 0 ? 8 : 1;
  let i = 0;
  for (;;) {
    const stack = [];
    while (i < list.length && stack.length < per) { stack.push(list[i]); i++; }
    if (stack.length === 0) break;
    if (extra + lost === 0 && g.map.options.hiddenMap !== 0 && nearestArmy(g, side, c.x, c.y) < 15) break;
    yield* takeNeutrals(g, side, c, stack.length, heading, stack);
  }
  return g.map.options.hiddenMap !== 0;
}

/** Send a city's idle armies to explore, one by one (57ea:0b19). */
function* sendExplorers(g, side, c, fliersOnly) {
  const d = core.data(g, side);
  let grouping = false;
  for (let i = 1; i <= core.MAX_GROUPS; i++) if (d.groups[i].active !== 0) grouping = true;
  const [extra, lost] = holdBack(g, side, c);
  const list = idle(g, side, c, extra + lost, (a) => {
    if (core.isHero(a) && grouping) return false;
    if (fliersOnly && !core.flies(g, a)) return false;
    return true;
  });
  for (const a of list) {
    if (extra + lost === 0 && g.map.options.hiddenMap !== 0 && nearestArmy(g, side, c.x, c.y) < 15) return;
    if (core.alive(g, a)) yield* moves.explore(g, side, a, c);
  }
}

/** The first neutral neighbour fewer than three stacks are heading for
 *  (57ea:0935), or null. */
export function openNeutral(g, side, c, heading) {
  const nb = neutralNeighbours(g, side, c);
  for (let j = nb.length - 1; j >= 0; j--) {
    const n = nb[j].city;
    if ((heading[n.index] || 0) <= 2) return n;
  }
  return null;
}

/** One pass of the neutral phase (57ea:00b5). */
function* neutralPass(g, side, pass, heading) {
  const d = core.data(g, side);
  let acted = 0;
  for (const c of own(g, side)) {
    if (c.ownerIndex === side.index && (pass === 0 || core.role(d, c) === core.JUST_TAKEN)) {
      if (core.role(d, c) === core.JUST_TAKEN) {
        yield* garrison(g, side, c, true);
        core.setRole(d, c, core.NEAR_NEUTRAL);
      }
      let act = neutralNeighbours(g, side, c).length > 0;
      if (!act) {
        const r = core.role(d, c);
        if (r === core.NEAR_NEUTRAL || r === core.TAKING_NEUTRAL) core.setRole(d, c, core.WEAK);
        if (g.map.options.quickStart === 0 && core.role(d, c) === core.BUILDING && g.turn < 6) act = true;
      }
      if (act) {
        acted++;
        if (!(yield* attackNeutrals(g, side, c, heading))) {
          core.setRole(d, c, openNeutral(g, side, c, heading) ? core.NEAR_NEUTRAL : core.WEAK);
        } else {
          yield* sendExplorers(g, side, c, g.turn > 10);
          core.setRole(d, c, core.TAKING_NEUTRAL);
        }
      }
    }
  }
  return acted;
}

/** neutral (ai_phase_neutral, 57ea:0000): up to ten passes. */
export function* neutral(g, side) {
  const heading = {};
  for (const a of core.armies(g, side.index)) {
    if (a.aiOrder === core.ORDER_CITY && a.aiDest != null && a.aiDest < g.map.cities.length) {
      heading[a.aiDest] = (heading[a.aiDest] || 0) + 1;
    }
  }
  for (let pass = 0; pass <= 9; pass++) {
    if ((yield* neutralPass(g, side, pass, heading)) === 0) break;
  }
}

/** A strong garrison strikes at a weak neighbour (5e97:053f). */
function* quickStrike(g, side, c) {
  const d = core.data(g, side);
  const role = core.role(d, c);
  let x = c.x;
  const y = c.y;
  if (role !== core.RALLY) x++;
  if (!(role !== core.RALLY || (d.garrison[c.index] || 0) > 11)) return;
  const need = (side.gold < 40 && (side.income || 0) < (side.upkeepTotal || 0)) ? 65 : 85;
  const list = core.collect(g, side.index, x, y, 8);
  const n = list.length;
  if (n === 0) return;
  if (n < 4 && (role === core.MEMBER || role === core.RALLY)) return;
  let sel = core.select(g, list);
  const [nb, nd] = core.neighbours(g, c);
  let danger = false, best = null, bestOdds = 0, bestTurns = 100;
  for (let j = nb.length - 1; j >= 0; j--) {
    const t = nb[j];
    if (!core.cflag(d, t, core.CF_UNSEEN) && t.ownerIndex !== side.index && core.standing(t)
        && t.index !== d.questCity
        && (t.ownerIndex == null || core.state(g, side.index, t.ownerIndex) === 2)) {
      const o = core.odds(g, sel, t.x, t.y);
      if (o === 0) {
        if ((n < 4 && nd[j] < 25) || (n < 6 && nd[j] < 15)) danger = true;
      } else if (o >= need) {
        const turns = Math.floor(nd[j] / Math.max(1, sel.minMoves - 2)) + 1;
        if (turns < bestTurns || (turns === bestTurns && bestOdds < o)) {
          best = t; bestOdds = o; bestTurns = turns;
        }
      }
    }
  }
  if (!danger && best) {
    sel = core.order(g, list, core.ORDER_CITY, best.index, 0);
    if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
  }
}

/** quick attack (ai_phase_quick_attack, 5e97:04ba): not for a cautious side. */
export function* quickAttack(g, side) {
  const d = core.data(g, side);
  if (d.cautious !== 0) return;
  for (const c of own(g, side)) {
    if (c.ownerIndex === side.index && QUICK_ROLES.has(core.role(d, c))
        && (d.garrison[c.index] || 0) > 3) {
      yield* quickStrike(g, side, c);
    }
  }
}

/** update hide (ai_phase_update_hide, 59bf:0c1c). */
export function updateHide(g, side) {
  const d = core.data(g, side);
  clearUnseen(g, side);
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (core.role(d, c) === core.TAKING_NEUTRAL) {
      for (const e of neutralNeighbours(g, side, c)) {
        if (!core.cflag(d, e.city, core.CF_UNSEEN)) {
          core.setRole(d, c, core.NEAR_NEUTRAL);
          break;
        }
      }
    }
  }
}

/** The city that should buy a new type (ai_city_for_type, 5db9:0c83). */
function cityForType(g, side) {
  const d = core.data(g, side);
  let best = null, bestScore = -1;
  for (const c of own(g, side)) {
    const r = core.role(d, c);
    let candidate = false, younger = 0;
    if (r === core.TAKING_NEUTRAL || r === core.NEAR_NEUTRAL) {
      if ((d.held[c.index] || 0) > 3) {
        younger = 4;
        candidate = !buildsWell(g, side, c)[0];
      }
    } else if (r === core.WEAK || r === core.BUILDING) {
      candidate = !buildsWell(g, side, c)[0];
    } else if (r === core.STOP) {
      if (!buildsWell(g, side, c)[0]) {
        core.setRole(d, c, core.NOWHERE);
        candidate = true;
      }
    } else if (r === core.NOWHERE) {
      if (!buildsWell(g, side, c)[0]) candidate = true;
      else core.setRole(d, c, core.STOP);
    }
    if (candidate) {
      const score = (d.held[c.index] || 0) - younger;
      if (bestScore < score) { best = c; bestScore = score; }
    }
  }
  return best;
}

/** rebuilding (ai_rebuilding, 5db9:0af2). */
export function rebuild(g, side) {
  const d = core.data(g, side);
  const limit = Math.max(5, Math.floor(d.rebuildLimit * g.map.cities.length / 80));
  if ((d.heroes > 4 || d.own <= limit * d.heroes) && side.gold > 499) {
    const t = side.gold < 2001 ? d.rebuildType : d.rebuildTypeRich;
    const c = cityForType(g, side);
    if (c && side.gold > 499) buyType(g, side, c, t);
  }
}

/** The member cities of an assault group (5f19:0b72). */
function pickMembers(g, side, grp) {
  const d = core.data(g, side);
  const minMax = core.has(grp.flags, core.GF_MOVE12) ? 12
               : core.has(grp.flags, core.GF_MOVE16) ? 16 : grp.size;
  // 1-based, as the group's member slots are
  const members = {}, strength = {};
  let count = 0;
  for (const c of own(g, side)) {
    if (groups.free(g, side, c, false)) {
      const f = d.flags[c.index] || 0;
      const fits = (!core.has(grp.flags, core.GF_MOVE12) || core.has(f, core.CF_MOVE12))
                && (!core.has(grp.flags, core.GF_MOVE16) || core.has(f, core.CF_MOVE16))
                && (!core.has(grp.flags, core.GF_FLY) || core.has(f, core.CF_FLYGROUP));
      const plain = core.has(grp.flags, core.GF_MOVE16) || core.has(grp.flags, core.GF_MOVE12)
                 || core.has(grp.flags, core.GF_FLY)
                 || !(core.has(f, core.CF_MOVE12) || core.has(f, core.CF_FLYGROUP)
                      || core.has(f, core.CF_MOVE16));
      if (fits && plain) {
        const [good, slot] = buildsWell(g, side, c);
        if (good && minMax <= slot.move) {
          let at = null;
          if (count < 4) {
            at = count + 1;
          } else {
            let least = 100;
            for (let k = 1; k <= 4; k++) {
              if (strength[k] < least) { least = strength[k]; at = k; }
            }
            if (at !== null && !(least <= slot.strength)) at = null;
          }
          if (at !== null) {
            if (members[at] === undefined) count++;
            members[at] = c; strength[at] = slot.strength;
          }
        }
      }
    }
  }
  grp.members = {};
  for (let k = 1; k <= count; k++) {
    const c = members[k];
    grp.members[k] = c.index;
    core.setRole(d, c, core.MEMBER);
  }
}

/** production (ai_production, 5db9:06d4). */
export function production(g, side) {
  const d = core.data(g, side);
  for (const c of own(g, side)) {
    if (core.role(d, c) === core.MEMBER) {
      core.setRole(d, c, core.STOP);
      c.vectorTo = undefined;
    }
  }
  for (let gi = core.MAX_GROUPS; gi >= 1; gi--) {
    if (d.groups[gi].active !== 0) pickMembers(g, side, d.groups[gi]);
  }
  if (g.map.options.hiddenMap !== 0) {
    // a city whose neutral neighbours are all unseen goes looking (57ea:02ff)
    for (const c of own(g, side)) {
      if (core.role(d, c) === core.NEAR_NEUTRAL) {
        const nb = neutralNeighbours(g, side, c);
        if (nb.length > 0) {
          let seen = false;
          for (const e of nb) if (!core.cflag(d, e.city, core.CF_UNSEEN)) seen = true;
          if (!seen) core.setRole(d, c, core.TAKING_NEUTRAL);
        }
      }
    }
  }
  for (const c of own(g, side)) {
    c.vectorTo = undefined;
    if (!building(c)) {
      if (side.gold < 40 && (side.income || 0) < (side.upkeepTotal || 0)) return;
      const r = core.role(d, c);
      let purpose = 3;
      if (r === core.TAKING_NEUTRAL || r === core.EXPLORER) {
        if (!core.cflag(d, c, core.CF_FLIER)) buyFlier(g, side, c);
        purpose = 4;
      } else if (r === core.NEAR_NEUTRAL) {
        purpose = g.map.options.neutralCities !== 0 ? 2 : 1;
      } else if (r === core.WEAK) {
        purpose = 2;
      } else if (r === core.EXPLORER2) {
        purpose = 4;
      } else if (r === core.STOP) {
        purpose = null;
        c.producing = undefined; c.countdown = 0;
      }
      if (purpose != null) produceFor(g, side, c, purpose);
    }
  }
}

/** vectoring (ai_vectoring, 5db9:085f): each group's members send what they
 *  build to its rally city. */
export function vectoring(g, side) {
  const d = core.data(g, side);
  for (let gi = d.maxGroups; gi >= 1; gi--) {
    const grp = d.groups[gi];
    const rally = grp.rally != null ? g.map.cities[grp.rally] : null;
    if (grp.active !== 0 && rally && rally.ownerIndex === side.index) {
      core.setRole(d, rally, core.RALLY);
      for (let k = 4; k >= 1; k--) {
        const m = grp.members[k] != null ? g.map.cities[grp.members[k]] : null;
        if (m && core.role(d, m) === core.MEMBER) vector(g, m, rally);
      }
    }
  }
}
