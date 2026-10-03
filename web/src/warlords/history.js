// The game's history: what the History menu plays back.
//
// docs/formats/history.md. Once a round, when the turn counter goes up
// (8065:17f6 -> 6d51:0d60), a record is taken of every side's gold, score
// and city count, every city's owner, and the round's deeds -- kept as they
// happen, two a side, a lower type counting for more (6d51:1244).

import * as armytype from "./armytype.js";
import * as report from "./report.js";
import * as game from "./game.js";

// the deed types, which are also their rank: lower counts for more
export const EMERGES = 0, KILLED = 1, QUEST_DONE = 2, QUEST_GIVEN = 3;
export const VANQUISHED = 4, WON = 5, FINDS = 6, VICTORIOUS = 7;
export const TREACHERY = 8, WAR = 9, PEACE = 10;
// first values with a meaning of their own
export const IN_BATTLE = -1, SEARCHING = -2;          // KILLED
export const BY_NAME = -1;                            // WON: the name won it
export const ALLIES = 100, SAGE = 101, GOLD = 102;    // FINDS

export const LAST_TURN = 201;
export const MAX_EVENTS = 10;

/** Note a deed of a side's (6d51:1244). `name` is the hero's or the side's. */
export function deed(g, side, type, v1, v2, name) {
  if (!side) return;
  g.deeds = g.deeds || {};
  const d = g.deeds[side.index] || [];
  g.deeds[side.index] = d;
  const e = { side: side.index, type, v1: v1 || 0, v2: v2 || 0, name: (name || "").slice(0, 15) };
  if (d.length < 2) { d.push(e); return; }
  const worse = d[1].type >= d[0].type ? 1 : 0;
  if (d[worse].type > type) d[worse] = e;
}

// What History > Triumphs counts: a side's own row counts what it lost; its
// row for another side, what it killed of them (67cc:1b43-1e92).
export const ARMIES = 0, CREATURES = 1, HEROES = 2, NAVIES = 3, STANDARDS = 4;

function bump(g, side, opp, k, n) {
  if (side == null || opp == null || side > 7 || opp > 7) return;
  g.triumphs = g.triumphs || {};
  const t = g.triumphs[side] || {};
  g.triumphs[side] = t;
  const row = t[opp] || [0, 0, 0, 0, 0];
  t[opp] = row;
  row[k] += n == null ? 1 : n;
}

/** An army of its owner's killed by `killer`'s. */
export function tally(g, army, killer) {
  const loser = army.owner;
  let k;
  if (army.type === armytype.HERO) k = HEROES;
  else if ((g.types.byId[army.type].bonus[48] || 0) !== 0) k = CREATURES;
  else k = ARMIES;
  let std = 0;
  for (const it of army.items || []) if (it.index < 8) std++;
  for (const row of [[loser, loser], [killer, loser]]) {
    bump(g, row[0], row[1], k);
    if (army.atSea) bump(g, row[0], row[1], NAVIES);
    if (std > 0) bump(g, row[0], row[1], STANDARDS, std);
  }
}

/** The count for side `me` against `opp`, kind k. */
export function triumph(g, me, opp, k) {
  const t = g.triumphs && g.triumphs[me] && g.triumphs[me][opp];
  return t ? (t[k] || 0) : 0;
}

/** The round's record (6d51:0d60), taken as the turn counter goes up. */
export function record(g) {
  if (g.turn > LAST_TURN) return;
  const r = { gold: [], score: [], cities: [], owners: [], events: [] };
  const win = report.figures(g, g.sides[0], report.WINNING);
  for (let i = 0; i < 8; i++) {
    const s = g.map.sides[i];
    r.gold[i] = (s && s.inUse) ? (s.gold || 0) : 0;
    r.score[i] = win.value[i] || 0;
    r.cities[i] = s ? game.sideCities(g, s).length : 0;
  }
  g.map.cities.forEach((c, i) => {
    r.owners[i] = c.razed ? 0xff : (c.ownerIndex == null ? 8 : c.ownerIndex);
  });
  const deeds = g.deeds || {};
  let n = 0;
  const take = {};
  for (let i = 0; i < 8; i++) if (deeds[i] && deeds[i][0]) { take[i] = 1; n++; }
  for (let i = 0; i < 8; i++) {
    if (deeds[i] && deeds[i][1] && n < MAX_EVENTS) { take[i] = 2; n++; }
  }
  for (let i = 0; i < 8; i++) {
    for (let k = 0; k < (take[i] || 0); k++) r.events.push(deeds[i][k]);
  }
  g.deeds = {};
  g.history = g.history || [];
  g.history.push(r);
}
