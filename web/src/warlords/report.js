// The figures behind the Report menu's five reports: Army, City, Gold,
// Production and Winning (6ef3:02fb). The drawing is ui/reports.js.

import * as game from "./game.js";

export const ARMY = 0, CITY = 1, GOLD = 2, PRODUCTION = 3, WINNING = 4;

/** The Winning report's score for a side: (gold + 5 income + upkeep + each
 *  city's income times defence) / 30, within 1..500. */
export function score(g, side) {
  let cities = 0;
  for (const c of game.sideCities(g, side)) cities += (c.defence || 0) * (c.income || 0);
  const s = Math.floor((side.gold + game.income(g, side) * 5 + game.upkeep(g, side) + cities) / 30);
  return Math.max(1, Math.min(500, s));
}

/** One report's figures: { value[i], out[i], max, result }. */
export function figures(g, side, n) {
  const r = { value: [], out: [], max: 0 };
  for (let i = 0; i < 8; i++) {
    const s = g.map.sides[i];
    r.out[i] = !(s && s.alive);
    r.value[i] = 0;
  }
  if (n === PRODUCTION) {
    r.result = (side.produced || []).length;
    return r;
  }
  if (n === ARMY) {
    for (const a of g.armies) {
      if (a.owner != null && a.owner >= 0 && a.owner < 8) r.value[a.owner]++;
    }
  } else if (n === CITY) {
    for (let i = 0; i < 8; i++) {
      const s = g.map.sides[i];
      r.value[i] = s ? game.sideCities(g, s).length : 0;
    }
  } else if (n === GOLD) {
    for (let i = 0; i < 8; i++) {
      const s = g.map.sides[i];
      r.value[i] = s ? (s.gold || 0) : 0;
    }
  } else if (n === WINNING) {
    const scores = [];
    for (let i = 0; i < 8; i++) {
      const s = g.map.sides[i];
      scores[i] = (s && s.inUse) ? score(g, s) : 1;
    }
    let rank = 0;
    for (let i = 0; i < 8; i++) if (scores[side.index] < scores[i]) rank++;
    for (let i = 0; i < 8; i++) r.value[i] = Math.floor(scores[i] / 5);
    r.max = 100;
    r.result = rank;
    return r;
  }
  for (let i = 0; i < 8; i++) r.max = Math.max(r.max, r.value[i]);
  // the top of the scale is the largest figure made even
  if (r.max > 0 && Math.floor(r.max / 2) === Math.floor((r.max - 1) / 2)) r.max++;
  r.result = r.value[side.index];
  return r;
}
