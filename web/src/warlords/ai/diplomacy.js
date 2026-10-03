// The computer players' diplomacy (ai_phase_diplomacy, 558d:0000): each turn
// the side sets every proposal afresh. docs/re/ai.md > Diplomacy.

import * as core from "./core.js";
import * as groups from "./groups.js";

/** May the side go for this city (558d:0851)? */
export function canAttack(g, side, c) {
  const own = core.owner(c);
  if (own === core.NEUTRAL) return true;
  const d = core.data(g, side);
  let wars = 0, last = null;
  for (let s = 7; s >= 0; s--) {
    if (core.inPlay(g, s) && core.state(g, side.index, s) === 2) { wars++; last = s; }
  }
  const st = core.state(g, side.index, own);
  if (st === 2) return true;
  if (st === 0 && wars !== 0 && (wars !== 1 || last !== own)) return false;
  if (st > 2) return false;
  return !!d.bold;
}

/** Solidarity with a side (558d:0a6e). */
export function spared(g, side, other) {
  const d = core.data(g, side);
  if (g.map.options.diplomacy === 0 || d.solidarity === 0 || !side.computer) return false;
  for (let h = 7; h >= 0; h--) {
    const hs = g.map.sides[h];
    if (core.inPlay(g, h) && !hs.computer && core.state(g, other, h) === 2 && (hs.aiSolidarity || 0) !== 0) {
      return true;
    }
  }
  return false;
}

/** Claim one more city as the side's own ground (558d:0917). */
function claim(g, side) {
  const cap = side.capital;
  if (!cap) return;
  let best = null, bestD = null;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    const cl = c.claim != null ? c.claim : core.NEUTRAL;
    if (core.standing(c) && cl !== side.index && (cl === core.NEUTRAL || !core.inPlay(g, cl))) {
      const dd = core.dist(c.x, c.y, cap.x, cap.y);
      if (dd < 40 && (bestD === null || dd < bestD)) { best = c; bestD = dd; }
    }
  }
  if (best) best.claim = side.index;
}

/** diplomacy (ai_phase_diplomacy, 558d:0000). */
export function phase(g, side) {
  const d = core.data(g, side);
  const me = side.index;
  const human = !side.computer;
  claim(g, side);
  if (g.map.options.diplomacy === 0) return;
  const enemy = groups.pickEnemy(g, side);

  const swapped = [], total = [], seen = [], theyWar = [], theyPeace = [];
  for (let b = 7; b >= 0; b--) {
    core.propose(g, me, b, 0);
    swapped[b] = 0; total[b] = 0; seen[b] = 0; theyWar[b] = 0; theyPeace[b] = 0;
    const p = core.proposal(g, b, me);
    if (p === 2) theyWar[b] = 1; else if (p === 0) theyPeace[b] = 1;
  }
  let tiles = 0;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (core.standing(c)) tiles++;
    const o = c.ownerIndex;
    if (o != null) {
      total[o]++;
      if (!core.cflag(d, c, core.CF_UNSEEN)) seen[o]++;
      const cl = c.claim != null ? c.claim : core.NEUTRAL;
      if (o === me) {
        if (cl !== core.NEUTRAL) swapped[cl]++;
      } else if (cl === me) {
        swapped[o]++;
      }
    }
  }
  // the grudge tests; each has a war branch that can never run (docs/re/ai.md)
  for (let b = 7; b >= 0; b--) {
    if (b !== me && core.inPlay(g, b)) {
      const grudge = (theyPeace[b] - theyWar[b]) * 2;
      if (swapped[b] >= grudge + 4) core.propose(g, me, b, 1);
    }
  }
  for (let b = 7; b >= 0; b--) {
    if (b !== me && core.inPlay(g, b)) {
      const grudge = (theyPeace[b] - theyWar[b]) * 2;
      const threat = d.lost[b] + d.citiesLost[b] * 4 + d.heroesKilled[b] * 2;
      if (threat >= grudge + 5) core.propose(g, me, b, Math.max(1, core.proposal(g, me, b)));
    }
  }
  if (enemy != null) core.propose(g, me, enemy, 2);
  for (let gi = core.MAX_GROUPS; gi >= 1; gi--) {
    const grp = d.groups[gi];
    if (grp.active !== 0 && grp.target != null) core.propose(g, me, grp.target, 2);
  }
  // whoever holds too much of the world
  let leader = null, most = 0;
  for (let s = 7; s >= 0; s--) {
    if (core.inPlay(g, s)) {
      const share = Math.floor(total[s] * 100 / Math.max(1, tiles));
      const limit = core.isComputer(g, s) ? 50 : d.humanShare;
      if (most < total[s] && limit < share) { leader = s; most = total[s]; }
    }
  }
  if (leader !== null && leader !== me) {
    for (let s = 7; s >= 0; s--) {
      if (core.inPlay(g, s)) core.propose(g, me, s, (s === leader || s === enemy) ? 2 : 0);
    }
  }
  if (!human && d.solidarity !== 0) {
    for (let s = 7; s >= 0; s--) {
      if (core.inPlay(g, s) && core.isComputer(g, s) && s !== me
          && core.proposal(g, me, s) !== 0 && spared(g, side, s)) {
        core.propose(g, me, s, 0);
      }
    }
    let atWarWithHuman = false;
    for (let s = 7; s >= 0; s--) {
      if (core.inPlay(g, s) && !core.isComputer(g, s) && core.state(g, me, s) === 2) atWarWithHuman = true;
    }
    if (atWarWithHuman) {
      for (let s = 7; s >= 0; s--) {
        if (core.inPlay(g, s) && core.isComputer(g, s) && (g.map.sides[s].aiSolidarity || 0) !== 0) {
          core.propose(g, me, s, 0);
        }
      }
    }
  }
  if (!human && g.greatest) {
    const fights = groups.fightingHumans(g);
    for (let s = 7; s >= 0; s--) {
      if (core.inPlay(g, s) && core.isComputer(g, s) && (fights[me] || fights[s])) core.propose(g, me, s, 0);
    }
  }
  for (let b = 7; b >= 0; b--) if (seen[b] === 0) core.propose(g, me, b, 0);
  for (let gi = core.MAX_GROUPS; gi >= 1; gi--) {
    const grp = d.groups[gi];
    if (grp.active !== 0 && grp.target != null && core.proposal(g, me, grp.target) === 0) {
      groups.cancel(g, side, gi);
    }
  }
}
