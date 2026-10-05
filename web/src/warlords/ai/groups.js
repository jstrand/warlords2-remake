// The computer players' assault groups: a rally city where a strike force
// gathers, up to four member cities building for it, and six enemy cities to
// take. Picking whom to attack is here too.
// docs/re/ai.md > Assault groups; addresses are Ghidra's.
//
// Groups are numbered 1-4 and their lists keyed from 1, as the original's
// fixed slots are (core.emptyGroup).

import * as move from "../move.js";
import * as game from "../game.js";
import * as ai from "../ai.js";
import * as core from "./core.js";
import * as cities from "./cities.js";
import * as diplo from "./diplomacy.js";
import { luaSort } from "../../util.js";

// roles a rally city may be picked from (DS:0940, DS:094c)
const RALLY_ROLES = new Set([5, 8, 6, 4, 14]);

/** Integer division truncating toward zero, as the original's does. */
function div(a, b) {
  return Math.trunc(a / b);
}

function own(g, side) { return cities.own(g, side); }

function city(g, i) { return i != null ? g.map.cities[i] : undefined; }

/** Is a city free to serve a new group (5f19:0a7a)? */
export function free(g, side, c, members) {
  const d = core.data(g, side);
  for (let gi = d.maxGroups; gi >= 1; gi--) {
    const grp = d.groups[gi];
    if (grp.active !== 0 && grp.rally === c.index) return false;
  }
  const r = core.role(d, c);
  if (r === core.STOP || r === core.BUILDING) return true;
  return !!(members && r === core.MEMBER);
}

/** Is a city an active group's rally city (5f19:07f8)? */
export function isRally(g, side, c) {
  const d = core.data(g, side);
  for (let gi = d.maxGroups; gi >= 1; gi--) {
    const grp = d.groups[gi];
    if (grp.active !== 0 && grp.rally === c.index) return true;
  }
  return false;
}

/** Cancel a group (563e:066d): its rally and member cities go back to role 8. */
export function cancel(g, side, gi) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  const r = city(g, grp.rally);
  if (r) core.setRole(d, r, core.STOP);
  for (let k = 4; k >= 1; k--) {
    const m = city(g, grp.members[k]);
    if (m) core.setRole(d, m, core.STOP);
  }
  d.groups[gi] = core.emptyGroup();
}

/** The gold sacking a city would bring (city_sack_value). */
export function sackValue(g, c) {
  let v = 0;
  for (let i = 1; i < c.slots.length; i++) v += Math.floor(Math.abs(g.types.byId[c.slots[i].type].price) / 2);
  return v;
}

/** Pillage a city (563e:1a2a). */
export function pillage(g, side, c) {
  game.pillage(g, side, c);
  if (ai.hooks.onSpoils) ai.hooks.onSpoils(g, side, c, "pillaged");
}

/** Sack a city (563e:199c) -- pillage it when there is nothing to sack. */
export function sack(g, side, c) {
  if (sackValue(g, c) === 0) return pillage(g, side, c);
  game.sack(g, side, c);
  if (ai.hooks.onSpoils) ai.hooks.onSpoils(g, side, c, "sacked");
}

/** Raze a city (563e:1864); returns the stack's next city, or null. */
export function raze(g, side, c, force, sel) {
  if (!force) {
    const [nb, nd] = core.neighbours(g, c);
    let n = 0;
    for (let j = nb.length - 1; j >= 0; j--) {
      if (nb[j].ownerIndex === side.index && nd[j] < 45) n++;
    }
    if (n > 2) return null;
  }
  if (sackValue(g, c) >= 400) {
    sack(g, side, c);
    return null;
  }
  game.raze(g, side, c, sel ? sel.armies : []);
  if (ai.hooks.onSpoils) ai.hooks.onSpoils(g, side, c, "razed");
  if (!sel || sel.armies.length === 0) return null;
  const flood = core.floodFrom(g, side.index, sel.leader.x, sel.leader.y, 15, sel);
  return core.bestCity(g, sel, flood, true);
}

/** Early vengeance (5e97:0000). */
export function earlyVengeance(g, me, c, was) {
  const side = g.map.sides[me];
  const d = core.data(g, side);
  if (side.computer && was != null && was !== core.NEUTRAL && !core.isComputer(g, was)
      && d.early !== 0 && g.turn < 10 && d.questCity !== c.index) {
    if (sackValue(g, c) < 200) raze(g, side, c, false);
    else sack(g, side, c);
    return true;
  }
  return false;
}

/** What the group will do to the cities it takes (5f19:06a0). */
function rollSpoils(g, side, grp) {
  const d = core.data(g, side);
  let bias = d.own * d.perCity;
  if (!side.computer) bias += d.bonusHuman;
  else if (side.level === 2) bias += d.bonusWarlord;
  else if (side.level === 1) bias += d.bonusLord;
  else if (side.level === 0) bias += d.bonusKnight;
  grp.flags = 0;
  if (d.raze !== 0 && g.rng.dice(1, 1000, 0) < d.raze + bias) { grp.flags = core.GF_RAZE; return; }
  if (side.gold < 100) bias += d.poor;
  if (d.sack !== 0 && g.rng.dice(1, 1000, 0) < d.sack + bias) { grp.flags = core.GF_SACK; return; }
  if (d.pillage !== 0 && g.rng.dice(1, 1000, 0) < d.pillage + bias) grp.flags = core.GF_PILLAGE;
}

/** Look over what the side's cities can build for the groups (59bf:01b3). */
export function prepare(g, side) {
  const d = core.data(g, side);
  let active = 0;
  for (let gi = d.maxGroups; gi >= 1; gi--) if (d.groups[gi].active !== 0) active++;
  d.minStrength = 0; d.flyCities = 0; d.strongCities = 0; d.fastCities = 0;
  const top = [];
  for (const c of own(g, side)) {
    core.clearCflag(d, c, core.CF_MOVE12);
    core.clearCflag(d, c, core.CF_FLYGROUP);
    core.clearCflag(d, c, core.CF_MOVE16);
    if (core.role(d, c) !== core.RALLY) {
      const [good, slot] = cities.buildsWell(g, side, c);
      if (good) top.push(slot.strength + (g.types.byId[slot.type].siege ? 2 : 0));
    }
  }
  luaSort(top, (p, q) => p > q);
  let least = 100;
  for (let i = 0; i < Math.min(8, top.length); i++) if (top[i] < least) least = top[i];
  d.minStrength = Math.min(4, least);

  const strong = (c, slot) => g.types.byId[slot.type].siege || d.minStrength <= slot.strength;

  if (active === 4) {
    for (const c of own(g, side)) {
      if (free(g, side, c, true) && core.cflag(d, c, core.CF_FLIER)) {
        let ok = false;
        for (const slot of c.slots) if (g.types.byId[slot.type].flies && slot.strength > 4) ok = true;
        if (ok) {
          d.flyCities++;
          core.setCflag(d, c, core.CF_FLYGROUP);
        }
      }
    }
  }
  const flyEnough = d.flyCities >= 4;
  let left = flyEnough ? d.flyCities - 4 : d.flyCities;
  if (active > 2) {
    for (const c of own(g, side)) {
      if (free(g, side, c, true)) {
        let skip = false;
        if (core.cflag(d, c, core.CF_FLYGROUP)) {
          if (left === 0) skip = true;
          else { core.clearCflag(d, c, core.CF_FLYGROUP); left--; }
        }
        if (!skip) {
          let ok = false;
          for (const slot of c.slots) if (strong(c, slot) && slot.move > 15) ok = true;
          if (ok) {
            d.strongCities++;
            core.setCflag(d, c, core.CF_MOVE16);
          }
        }
      }
    }
  }
  const manyStrong = d.strongCities > 7;
  let fastBuilders = 0;
  const strongEnough = d.strongCities >= 4;
  left = strongEnough ? d.strongCities - 4 : d.strongCities;
  if (active > 1) {
    for (const c of own(g, side)) {
      if (free(g, side, c, true) && !core.cflag(d, c, core.CF_FLYGROUP)) {
        let skip = false;
        if (core.cflag(d, c, core.CF_MOVE16)) {
          if (left === 0) skip = true;
          else { core.clearCflag(d, c, core.CF_MOVE16); left--; }
        }
        if (!skip) {
          let ok = false;
          for (const slot of c.slots) {
            if (strong(c, slot) && slot.move > 11) {
              fastBuilders++;
              if (!manyStrong || slot.move > 15) ok = true;
            }
          }
          if (ok) {
            d.fastCities++;
            core.setCflag(d, c, core.CF_MOVE12);
          }
        }
      }
    }
  }
  const fastEnough = d.fastCities >= 4;
  left = fastEnough ? d.fastCities - 4 : d.fastCities;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (c.ownerIndex === side.index && !core.cflag(d, c, core.CF_FLYGROUP)
        && !core.cflag(d, c, core.CF_MOVE16) && core.cflag(d, c, core.CF_MOVE12) && left !== 0) {
      core.clearCflag(d, c, core.CF_MOVE12);
      left--;
    }
  }
  for (let gi = d.maxGroups; gi >= 1; gi--) {
    const grp = d.groups[gi];
    if (grp.active !== 0 && gi <= 4) {
      grp.size = 8;
      if (gi === 1) {
        grp.size = fastBuilders < 8 ? 8 : 12;
      } else if (gi === 2) {
        grp.flags = core.clear(grp.flags, core.GF_MOVE12);
        if (fastEnough) { grp.flags = core.set(grp.flags, core.GF_MOVE12); grp.size = 12; }
      } else if (gi === 3) {
        grp.flags = core.clear(grp.flags, core.GF_MOVE16);
        if (strongEnough) { grp.flags = core.set(grp.flags, core.GF_MOVE16); grp.size = 16; }
      } else if (gi === 4) {
        grp.flags = core.clear(grp.flags, core.GF_FLY);
        if (flyEnough) { grp.flags = core.set(grp.flags, core.GF_FLY); grp.size = 12; }
      }
    }
  }
}

/** Drop staged stacks that are gone or have left the group (563e:1251). */
function cleanStaged(g, side, gi) {
  const grp = core.data(g, side).groups[gi];
  for (let s = 1; s <= 4; s++) {
    const a = grp.staged[s];
    if (a && !(core.alive(g, a) && a.owner === side.index && a.aiGroup === gi)) delete grp.staged[s];
  }
}

/** Re-check the plan (563e:041c). Returns the number of targets. */
function checkPlan(g, side, gi) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  let n = 0;
  for (let k = 1; k <= 6; k++) {
    const t = city(g, grp.cities[k]);
    if (t) {
      if (d.questCity === t.index || core.owner(t) !== grp.target) delete grp.cities[k];
      else n++;
    }
  }
  const rally = city(g, grp.rally);
  if (!rally) return n;
  const [nb, nd] = core.neighbours(g, rally);
  for (let j = nb.length - 1; j >= 0; j--) {
    const m = nb[j];
    if (core.owner(m) === grp.target && !core.cflag(d, m, core.CF_UNSEEN)) {
      let have = false;
      for (let k = 1; k <= 6; k++) if (grp.cities[k] === m.index) have = true;
      if (!have) {
        n++;
        let slot = null, worst = null;
        for (let k = 6; k >= 1; k--) {
          if (grp.cities[k] == null) { slot = k; worst = -1; break; }
          if ((worst || 0) < (grp.dist[k] || 0)) { slot = k; worst = grp.dist[k]; }
        }
        if (slot !== null && (worst === -1 || nd[j] < worst)) {
          grp.cities[slot] = m.index; grp.dist[slot] = nd[j];
        }
      }
    }
  }
  return n;
}

/** With one target left, add its neighbours of the target side, and move the
 *  rally to one of the side's own cities next to it (563e:1579). */
function adjustRally(g, side, gi) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  let only = null, count = 0;
  for (let k = 1; k <= 6; k++) if (grp.cities[k] != null) { only = grp.cities[k]; count++; }
  if (count !== 1) return;
  const [nb] = core.neighbours(g, city(g, only));
  let next = null, rallyNear = false;
  for (let j = 0; j < nb.length; j++) {
    const m = nb[j];
    if (!core.cflag(d, m, core.CF_UNSEEN)) {
      if (core.owner(m) === grp.target) {
        for (let k = 1; k <= 6; k++) {
          if (grp.cities[k] == null) { grp.cities[k] = m.index; break; }
        }
      } else if (m.ownerIndex === side.index) {
        if (m.index === grp.rally) rallyNear = true;
        if (!isRally(g, side, m) && !next) next = m;
      }
    }
  }
  if (!rallyNear && next) grp.rally = next.index;
}

/** Move the rally to the side's nearest neighbouring city that is no other
 *  group's rally (563e:02ed); cancel the group when there is none. */
function relocate(g, side, gi) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  const r = city(g, grp.rally);
  const [nb] = core.neighbours(g, r);
  let best = null, bestD = 1000;
  for (let j = nb.length - 1; j >= 0; j--) {
    const m = nb[j];
    if (m.ownerIndex === side.index && !isRally(g, side, m)) {
      const dd = core.dist(m.x, m.y, r.x, r.y);
      if (dd < bestD) { best = m; bestD = dd; }
    }
  }
  if (!best) {
    cancel(g, side, gi);
    return false;
  }
  grp.rally = best.index;
  return true;
}

/** A city the group has taken (563e:134c). */
function* takeCity(g, side, gi, c) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  if (grp.rally === c.index) return false;
  yield* cities.garrison(g, side, c, true);
  for (let k = 1; k <= 6; k++) {
    if (grp.cities[k] === c.index) { delete grp.cities[k]; break; }
  }
  const near = (x) => {
    const [nb, nd] = core.neighbours(g, x);
    let n = 0;
    for (let j = 0; j < nb.length; j++) {
      if (core.owner(nb[j]) === grp.target) {
        if (nd[j] < 10) n++;
        if (nd[j] < 20) n++;
        if (nd[j] < 30) n++;
        if (nd[j] < 50) n++;
      }
    }
    return n;
  };
  const here = near(c);
  const rally = city(g, grp.rally);
  const there = rally ? near(rally) : 0;
  const pick = there < here ? c : rally;
  if (pick) {
    let have = false;
    for (let k = 1; k <= 6; k++) if (grp.taken[k] === pick.index) have = true;
    if (!have) {
      for (let k = 1; k <= 6; k++) {
        if (grp.taken[k] == null) { grp.taken[k] = pick.index; break; }
      }
    }
  }
  if (here === 0 && there === 0) {
    cancel(g, side, gi);
    return false;
  }
  grp.rally = pick.index;
  core.setRole(d, c, core.RALLY);
  yield* followUp(g, side, gi);
  return true;
}

/** Send the group's stack at a city and deal with what it takes (563e:0f1b).
 *  Returns 1 when something was taken, 3 otherwise. */
export function* march(g, side, gi, list, c) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  const lead = list[0];
  if (lead && core.alive(g, lead) && !core.standing(c)) {
    let best = null, bestD = 1000;
    for (let k = 1; k <= 6; k++) {
      const t = city(g, grp.cities[k]);
      if (t) {
        const dd = core.dist(lead.x, lead.y, t.x, t.y);
        if (dd < bestD && core.standing(t)) { best = t; bestD = dd; }
      }
    }
    if (best) c = best;
  }
  for (;;) {
    let n = 0;
    for (let k = 1; k <= 6; k++) if (grp.cities[k] != null) n++;
    list = list.filter((a) => core.alive(g, a) && a.owner === side.index);
    const was = core.owner(c);
    const sel = core.order(g, list, core.ORDER_CITY, c.index, 0);
    if (!sel) { cleanStaged(g, side, gi); return 3; }
    const r = yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
    if (r === 1) {
      game.disband(g, side, sel.armies);
      cleanStaged(g, side, gi);
      return 3;
    }
    cleanStaged(g, side, gi);
    if (c.ownerIndex !== side.index) return 3;
    if (was === side.index) return 3;
    const capital = side.capital === c;
    if (!capital && n >= 2 && core.has(grp.flags, core.GF_RAZE)) {
      const nextCity = raze(g, side, c, false, sel);
      if (!nextCity) return 1;
      c = nextCity;
      list = sel.armies;
    } else {
      if (!capital && core.has(grp.flags, core.GF_SACK)) sack(g, side, c);
      else if (!capital && core.has(grp.flags, core.GF_PILLAGE)) pillage(g, side, c);
      yield* takeCity(g, side, gi, c);
      return 1;
    }
  }
}

/** Look for a stack of the target side worth hitting near a staged stack
 *  (563e:0c32). Returns 2 if it went. */
function* hitArmies(g, side, gi, list, target, range, flood, sel) {
  const lead = list[0];
  if (!lead || lead.atSea) return 0;
  let bx = null, by = null, bestOdds = 0;
  for (let x = lead.x - range; x <= lead.x + range - 1; x++) {
    for (let y = lead.y - range; y <= lead.y + range - 1; y++) {
      if (x >= 0 && y >= 0 && x < g.map.width && y < g.map.height) {
        const here = game.armiesAt(g, x, y);
        const t = core.terrain(g, x, y);
        if (here[0] && here[0].owner === target && t !== move.CITY && t !== move.SHORE && t !== move.WATER
            && core.floodAt(flood, x, y) <= sel.minMoves - 1 && game.seen(g, side.index, x, y)) {
          const count = here.length;
          let strength = 0, heroes = 0;
          for (const a of here) {
            strength += a.strength || 0;
            if (core.isHero(a)) heroes++;
          }
          const o = core.odds(g, sel, x, y);
          if (o > 75 && ((count > 2 && strength > 10) || heroes !== 0) && bestOdds < o) {
            bx = x; by = y; bestOdds = o;
          }
        }
      }
    }
  }
  if (bx === null) return 0;
  const dest = lead.aiOrder === core.ORDER_CITY ? lead.aiDest : undefined;
  const s = core.order(g, list, core.ORDER_ROAM, 0, 0x20);
  if (s) {
    s.leader.target = { x: bx, y: by };
    yield* core.moveTo(g, s, bx, by);
    for (const a of list) {
      if (core.alive(g, a) && a.owner === side.index && dest != null) {
        a.aiOrder = core.ORDER_CITY; a.aiDest = dest;
      }
    }
  }
  return 2;
}

/** A staged stack's step (563e:0b49). */
function* stagedStep(g, side, gi, list, target, flood, sel) {
  let nearest = null, nd = 1000;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (c.ownerIndex === target) {
      const [dd] = core.cityDistance(g, flood, c);
      if (dd < 50 && dd < sel.minMoves) {
        const o = core.odds(g, sel, c.x, c.y);
        if (o > 75 && dd < nd) { nearest = c; nd = dd; }
      }
    }
  }
  let r = yield* hitArmies(g, side, gi, list, target, 15, flood, sel);
  if (r !== 0 && nearest) r = yield* march(g, side, gi, list, nearest);
  return r;
}

/** Move the group's staged stacks on (563e:0996). */
function* gather(g, side, gi) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  for (let s = 4; s >= 1; s--) {
    let again = true;
    while (again) {
      again = false;
      cleanStaged(g, side, gi);
      const a = grp.staged[s];
      if (!a) break;
      const list = core.collectOrdered(g, side.index, a.x, a.y, a.aiGroup, a.aiOrder, 0);
      if (list.length === 0) break;
      const sel = core.select(g, list);
      const flood = core.floodFrom(g, side.index, a.x, a.y, 15, sel);
      const was = list.map((b) => [b.x, b.y, b.moves]);
      const armies = g.armies.length;
      const r = yield* stagedStep(g, side, gi, list, grp.target, flood, sel);
      if (r === 0) {
        const last = list[list.length - 1];
        if (last && core.alive(g, last) && last.aiOrder === core.ORDER_CITY && city(g, last.aiDest)) {
          yield* march(g, side, gi, list, city(g, last.aiDest));
        }
      } else if (r === 2) {
        // The original goes round again for as long as a stack is found to
        // strike. One that cannot take a step would be found every time --
        // until the odds' dice fall short, which for a strong stack is
        // never -- so a pass that changed nothing ends it here.
        again = g.armies.length !== armies ||
          list.some((b, i) => b.x !== was[i][0] || b.y !== was[i][1] || b.moves !== was[i][2]);
      }
    }
  }
}

/** Strike from the rally city (563e:06e9). */
function* strike(g, side, gi) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  const r = city(g, grp.rally);
  if (!r) return false;
  const list = core.collect(g, side.index, r.x + 1, r.y, 0);
  const n = list.length;
  if (n === 0) return false;
  const sel = core.select(g, list);
  let best = null, bestScore = -1;
  for (let k = 1; k <= 6; k++) {
    const t = city(g, grp.cities[k]);
    if (t) {
      const turns = div(grp.dist[k] != null ? grp.dist[k] : -1, Math.max(1, sel.minMoves - 2)) + 1;
      const o = core.odds(g, sel, t.x, t.y);
      let score = (turns < 11 ? 10 - turns : 0) + o + (grp.bonus[k] || 0) + g.rng.dice(1, 4, 0);
      if (turns === 1) score += 100;
      if ((o > 75 || n > 7) && bestScore < score) { best = t; bestScore = score; }
    }
  }
  if (!best) return false;
  // the stack's lead -- a hero, else the strongest -- is staged (563e:08b2)
  let slot = null;
  for (let s = 4; s >= 1; s--) if (!grp.staged[s]) { slot = s; break; }
  if (slot !== null) {
    let pick = null, strongest = -1;
    for (let i = list.length - 1; i >= 0; i--) {
      const a = list[i];
      if (core.isHero(a)) { pick = a; break; }
      if (strongest < (a.strength || 0)) { pick = a; strongest = a.strength || 0; }
    }
    grp.staged[slot] = pick;
  }
  for (const a of list) a.aiGroup = gi;
  yield* march(g, side, gi, list, best);
  return true;
}

/** Follow up from the cities the group took (563e:16fd). */
export function* followUp(g, side, gi) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  for (let k = 1; k <= 6; k++) {
    const t = city(g, grp.taken[k]);
    if (t && t.index !== grp.rally && t.ownerIndex === side.index) {
      const [nb, nd] = core.neighbours(g, t);
      let near = 0;
      for (let j = nb.length - 1; j >= 0; j--) {
        const o = nb[j].ownerIndex;
        if (o != null && o !== side.index && nd[j] < 25) near++;
      }
      if (near === 0) {
        yield* cities.garrison(g, side, t, true);
        const list = core.collectOrdered(g, side.index, t.x + 1, t.y, 0, grp.size, 0);
        if (list.length > 3) {
          const sel = core.order(g, list, core.ORDER_CITY, grp.rally, 0);
          if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
        }
      }
    }
  }
}

/** Cancel the longest-running group (563e:0607). */
function cancelOldest(g, side) {
  const d = core.data(g, side);
  let oldest = null, age = -1;
  for (let gi = d.maxGroups; gi >= 1; gi--) {
    const grp = d.groups[gi];
    if (grp.active !== 0 && age < grp.active) { oldest = gi; age = grp.active; }
  }
  if (oldest !== null) cancel(g, side, oldest);
}

/** A group's turn (563e:00ca). */
function* runGroup(g, side, gi) {
  const d = core.data(g, side);
  const grp = d.groups[gi];
  const cap = side.capital;
  if (cap && cap.ownerIndex != null && cap.ownerIndex !== side.index) {
    let going = false;
    for (let k = d.maxGroups; k >= 1; k--) {
      const o = d.groups[k];
      if (o.active !== 0 && o.target === cap.ownerIndex) going = true;
    }
    if (!going) {
      cancelOldest(g, side);
      return false;
    }
  }
  if (checkPlan(g, side, gi) === 0) {
    cancel(g, side, gi);
    return false;
  }
  adjustRally(g, side, gi);
  let shared = false;
  for (let k = d.maxGroups; k >= 1; k--) {
    const o = d.groups[k];
    if (o.active !== 0 && k !== gi && o.rally === grp.rally) shared = true;
  }
  const r = city(g, grp.rally);
  if ((!shared && r && r.ownerIndex === side.index) || relocate(g, side, gi)) {
    yield* gather(g, side, gi);
    if (d.groups[gi].active !== 0) {
      let rc = city(g, d.groups[gi].rally);
      if (rc) yield* cities.garrison(g, side, rc, true);
      if (yield* strike(g, side, gi)) {
        rc = city(g, d.groups[gi].rally);
        if (rc) yield* cities.garrison(g, side, rc, true);
      }
      if (d.groups[gi].active !== 0) {
        yield* followUp(g, side, gi);
        return true;
      }
    }
  }
  return false;
}

/** assault (ai_phase_assault, 563e:0000). */
export function* assault(g, side) {
  const d = core.data(g, side);
  if (d.turns === 0) return;
  prepare(g, side);
  for (const c of own(g, side)) {
    if (core.role(d, c) === core.RALLY) core.setRole(d, c, core.STOP);
  }
  for (let gi = 1; gi <= d.maxGroups; gi++) {
    const grp = d.groups[gi];
    if (grp.active !== 0) {
      const r = city(g, grp.rally);
      if (r) core.setRole(d, r, core.RALLY);
      if (yield* runGroup(g, side, gi)) d.groups[gi].active++;
    }
  }
}

/** The enemy city with most of its own side's cities round it (5f19:04d2). */
function hub(g, enemy) {
  let best = null, most = 0;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (c.ownerIndex === enemy) {
      let n = 1;
      const [nb] = core.neighbours(g, c);
      for (let j = nb.length - 1; j >= 0; j--) if (nb[j].ownerIndex === enemy) n++;
      if (n > most || (n === most && g.rng.dice(1, 2, -1) !== 0)) { best = c; most = n; }
    }
  }
  return best;
}

/** Distances over the neighbour graph from a city (5f19:058a). */
function spread(g, from) {
  const reached = { [from.index]: true }, dist = { [from.index]: 0 }, done = {};
  let changed = true;
  while (changed) {
    changed = false;
    for (let i = g.map.cities.length - 1; i >= 0; i--) {
      const c = g.map.cities[i];
      if (reached[c.index] && !done[c.index]) {
        changed = true;
        done[c.index] = true;
        const [nb, nd] = core.neighbours(g, c);
        for (let j = nb.length - 1; j >= 0; j--) {
          const m = nb[j];
          const nd2 = dist[c.index] + nd[j];
          if (!reached[m.index]) {
            reached[m.index] = true; dist[m.index] = nd2;
          } else if (nd2 < dist[m.index]) {
            dist[m.index] = nd2;
          }
        }
      }
    }
  }
  return [reached, dist];
}

/** Plan a group against a side (5f19:01bb); the rally city, or null. */
function plan(g, side, grp, enemy) {
  const d = core.data(g, side);
  grp.cities = {}; grp.dist = {};
  const h = hub(g, enemy);
  if (!h) return null;
  let [reached, dist] = spread(g, h);
  let rally = null, best = 1e9;
  for (const c of own(g, side)) {
    if (RALLY_ROLES.has(core.role(d, c)) && reached[c.index] && dist[c.index] < best) {
      rally = c; best = dist[c.index];
    }
  }
  if (!rally) {
    if (!core.cflag(d, h, core.CF_UNSEEN)) {
      let bestD = 1e9;
      for (const c of own(g, side)) {
        if (RALLY_ROLES.has(core.role(d, c))) {
          const dd = core.dist(c.x, c.y, h.x, h.y);
          if (dd < bestD) { rally = c; bestD = dd; }
        }
      }
      if (rally) { grp.cities[1] = h.index; grp.dist[1] = 100; }
    }
    return rally;
  }
  [reached, dist] = spread(g, rally);
  let n = 0;
  for (let k = 1; k <= 6; k++) {
    let pick = null, pd = 1e9;
    for (let i = g.map.cities.length - 1; i >= 0; i--) {
      const c = g.map.cities[i];
      if (c.ownerIndex === enemy && !core.cflag(d, c, core.CF_UNSEEN) && reached[c.index] && dist[c.index] < pd) {
        pick = c; pd = dist[c.index];
      }
    }
    if (!pick) break;
    grp.cities[k] = pick.index; grp.dist[k] = pd;
    reached[pick.index] = false;
    n++;
  }
  if (n === 0 && !core.cflag(d, h, core.CF_UNSEEN)) {
    grp.cities[1] = h.index; grp.dist[1] = 100;
    n = 1;
  }
  if (n === 0) return null;
  return rally;
}

/** Which side to attack (ai_pick_enemy, 5f19:0e04), or null. */
export function pickEnemy(g, side) {
  const d = core.data(g, side);
  const me = side.index;
  const cap = side.capital;
  const capOwner = cap ? core.owner(cap) : core.NEUTRAL;
  const held = [], score = [], count = [], capHeld = [], fromThem = [], targeted = [];
  const rnd = [];
  for (let s = 0; s < 8; s++) {
    held[s] = 0; score[s] = 0; count[s] = 0; capHeld[s] = 0; fromThem[s] = 0; targeted[s] = 0;
    rnd[s] = g.rng.dice(1, 10, 0);
  }
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    const o = c.ownerIndex;
    if (o != null) {
      count[o]++;
      if (!core.cflag(d, c, core.CF_UNSEEN)) held[o]++;
      if (o === me && c.claim != null && c.claim !== core.NEUTRAL) fromThem[c.claim]++;
    }
  }
  let nGroups = 0;
  for (let gi = d.maxGroups; gi >= 1; gi--) {
    const grp = d.groups[gi];
    if (grp.active !== 0 && grp.target != null) {
      targeted[grp.target]++;
      nGroups++;
    }
  }
  if (capOwner !== core.NEUTRAL && capOwner !== me) score[capOwner] += 20;
  let others = 0;
  for (let s = 7; s >= 0; s--) {
    if (count[s] !== 0) {
      if (s !== me) others++;
      const theirs = g.map.sides[s].capital;
      if (theirs && theirs.ownerIndex === me) capHeld[s] = 1;
    }
  }
  let constant;
  if (!side.computer) constant = d.dieHuman;
  else if (side.level === 2) constant = d.dieWarlord;
  else if (side.level === 1) constant = d.dieLord;
  else constant = d.dieKnight;
  for (let s = 7; s >= 0; s--) {
    const o = g.map.sides[s];
    score[s] += rnd[s] + capHeld[s] * 15 + fromThem[s] * 4
              + d.heroesKilled[s] * 4 + d.armiesKilled[s] + d.battles[s]
              + d.lost[s] * 2 + d.cityBattles[s] * 2 + d.citiesLost[s] * 2 + (constant || 0);
    score[s] += Math.floor(Math.abs((side.diploScore || 0) - (o.diploScore || 0)) / 8);
    score[s] += Math.floor(Math.abs(count[me] - count[s]) / 4);
  }
  let proposingWar = false;
  for (let s = 7; s >= 0; s--) {
    if (core.inPlay(g, s) && s !== me && core.proposal(g, me, s) === 2) proposingWar = true;
  }
  if (g.greatest && side.computer) {
    const fights = fightingHumans(g);
    for (let s = 7; s >= 0; s--) {
      if (core.inPlay(g, s) && core.isComputer(g, s) && (fights[me] || fights[s])) score[s] = 0;
    }
  }
  const early = g.map.options.quickStart !== 0 ? 4 : 8;
  for (let s = 7; s >= 0; s--) {
    const fought = d.battles[s] + d.cityBattles[s];
    if (diplo.spared(g, side, s)) score[s] = 0;
    if (proposingWar && core.proposal(g, me, s) === 0) score[s] = 0;
    if (nGroups === 0 && held[s] === 0) score[s] = 0;
    if (g.turn < early && fought === 0) score[s] = 0;
    if (others > 1 && Math.floor(count[s] / 4) < targeted[s]) score[s] = 0;
  }
  score[me] = 0;
  let pick = null, top = 0;
  for (let s = 7; s >= 0; s--) {
    if (count[s] !== 0 && top < score[s]) { pick = s; top = score[s]; }
  }
  if (capOwner !== core.NEUTRAL && capOwner !== me) {
    // 5f19:1393 compares the capital's holder with the groups-per-side
    // counts, not with the groups' targets: kept as it is
    let found = false;
    for (let gi = d.maxGroups - 1; gi >= 0; gi--) if (capOwner === targeted[gi]) found = true;
    if (!found) pick = capOwner;
  }
  if (pick !== null && held[pick] === 0) pick = null;
  return pick;
}

/** For I am the Greatest: which computer sides are at war with a human. */
export function fightingHumans(g) {
  const out = {};
  for (let s = 0; s < 8; s++) {
    if (core.inPlay(g, s) && core.isComputer(g, s)) {
      for (let h = 0; h < 8; h++) {
        if (core.inPlay(g, h) && !core.isComputer(g, h) && core.state(g, s, h) === 2) out[s] = true;
      }
    }
  }
  return out;
}

/** assault XX (ai_phase_assault_xx, 5f19:0000): start a new group. */
export function assaultXX(g, side) {
  const d = core.data(g, side);
  if (d.turns === 0) return;
  let quiet = 0;
  for (const c of own(g, side)) {
    const r = core.role(d, c);
    if (r === core.BUILDING || r === core.STOP) quiet++;
  }
  let active = 0;
  for (let gi = 1; gi <= core.MAX_GROUPS; gi++) if (d.groups[gi].active !== 0) active++;
  if (!(active < d.maxGroups && quiet !== 0 && (active === 0 || quiet > 2))) return;
  let slot = null;
  for (let gi = d.maxGroups; gi >= 1; gi--) if (d.groups[gi].active === 0) slot = gi;
  if (slot === null) return;
  d.groups[slot] = core.emptyGroup();
  const enemy = pickEnemy(g, side);
  if (enemy == null) return;
  const grp = d.groups[slot];
  const rally = plan(g, side, grp, enemy);
  if (!rally) {
    d.groups[slot] = core.emptyGroup();
    return;
  }
  grp.active = 1; grp.target = enemy; grp.rally = rally.index;
  core.setRole(d, rally, core.RALLY);
  rollSpoils(g, side, grp);
  core.propose(g, side.index, enemy, 2);
  // the rally city is no group's member any more (5f19:083e)
  for (let gi = d.maxGroups; gi >= 1; gi--) {
    const o = d.groups[gi];
    if (o.active !== 0) {
      for (let k = 1; k <= 4; k++) {
        if (o.members[k] === rally.index) { delete o.members[k]; rally.vectorTo = undefined; }
      }
    }
  }
  // each target's bonus: its own side's cities round it (5f19:09ca)
  for (let k = 1; k <= 6; k++) {
    const t = city(g, grp.cities[k]);
    if (t) {
      let n = 0;
      for (const m of core.neighbours(g, t)[0]) if (m.ownerIndex === enemy) n++;
      grp.bonus[k] = n;
    }
  }
  prepare(g, side);
}
