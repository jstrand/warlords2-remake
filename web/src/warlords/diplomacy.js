// Diplomacy: the state between every pair of sides, and the proposals that
// change it.
//
// docs/rules.md > Diplomacy. A byte per ordered pair: the current state and
// this side's proposal. Proposals are applied at the start of the proposing
// side's turn (diplomacy_apply, 484e:0db3).

import * as game from "./game.js";
import * as history from "./history.js";
import { luaSort } from "../util.js";

export const PEACE = 0, INTERMEDIATE = 1, WAR = 2;
export const STATE_NAMES = ["peace", "uneasy", "war"];

// STRING.DAT group 106, best first.
export const TITLES = [
  "Statesman", "Diplomat", "Pragmatist", "Politician",
  "Deceiver", "Scoundrel", "Turncoat", "Running Dog",
];

// which titles are used, by how many sides are in play (1-based ranks)
export const RATING_RANKS = {
  1: [1],
  2: [1, 8],
  3: [1, 4, 8],
  4: [1, 2, 6, 8],
  5: [1, 2, 4, 6, 8],
  6: [1, 2, 4, 6, 7, 8],
  7: [1, 2, 4, 5, 6, 7, 8],
  8: [1, 2, 3, 4, 5, 6, 7, 8],
};

const key = (a, b) => a * 8 + b;

/** War everywhere if Diplomacy is off, peace if it is on (484e:11bd). */
export function init(g) {
  const start = g.map.options.diplomacy !== 0 ? PEACE : WAR;
  g.diplomacy = { state: {}, proposal: {} };
  for (let a = 0; a < 8; a++) {
    for (let b = 0; b < 8; b++) {
      const v = a === b ? PEACE : start;
      g.diplomacy.state[key(a, b)] = v;
      g.diplomacy.proposal[key(a, b)] = v;
    }
  }
}

/** The state between two sides. */
export function state(g, a, b) {
  if (!g.diplomacy || a == null || b == null) return WAR;
  const v = g.diplomacy.state[key(a, b)];
  return v == null ? WAR : v;
}

export function atWar(g, a, b) {
  return state(g, a, b) !== PEACE;
}

/** May `a` attack `b`? A side at peace may not. */
export function mayAttack(g, a, b) {
  if (a == null || b == null) return true;        // neutrals are fair game
  return state(g, a, b) !== PEACE;
}

/** Propose a state to another side. */
export function propose(g, a, b, st) {
  if (!g.diplomacy || a === b) return;
  g.diplomacy.proposal[key(a, b)] = st;
}

/** The state a side proposes to another. */
export function proposal(g, a, b) {
  if (!g.diplomacy) return null;
  let p = g.diplomacy.proposal[key(a, b)];
  if (p == null) p = state(g, a, b);
  return p;
}

/** Apply one side's proposals. Returns a list of messages. */
export function apply(g, side) {
  if (!g.diplomacy) return [];
  const messages = [];
  const a = side.index;
  for (let b = 0; b < 8; b++) {
    const other = g.map.sides[b];
    if (b !== a && other) {
      const now = state(g, a, b);
      const want = proposal(g, a, b);
      if (want !== now) {
        if (want > now) {                            // escalation: at once
          g.diplomacy.state[key(a, b)] = want;
          g.diplomacy.state[key(b, a)] = want;
          if (proposal(g, b, a) < want) g.diplomacy.proposal[key(b, a)] = want;
          if (want === WAR) {
            history.deed(g, side, history.WAR, a, b, "");          // 484e:0f15
            messages.push(`War declared with ${other.name}!`);
          }
        } else if (proposal(g, b, a) <= want) {      // de-escalation: mutual
          g.diplomacy.state[key(a, b)] = want;
          g.diplomacy.state[key(b, a)] = want;
          if (want === PEACE) {
            history.deed(g, side, history.PEACE, a, b, "");         // 484e:1027
            messages.push(`Peace negotiated with ${other.name}!`);
          }
        }
      }
    }
  }
  return messages;
}

/** Must `a` be asked before it attacks `b`'s tile -- "Thou art attacking
 *  without first having declared war!" (attack_tile, 67cc:00b2)? Yes at
 *  peace, and when uneasy if the tile is a city. The original tests the
 *  city's index with > 0, so city 0 is attacked uneasy without asking. */
export function mustDeclare(g, a, b, city) {
  if (g.map.options.diplomacy === 0 || a == null || b == null) return false;
  const st = state(g, a, b);
  return st === PEACE || (st === INTERMEDIATE && city != null && city.index > 0);
}

/** "Shall we attack anyway?" -- yes (67cc:20a8): war at once, both ways,
 *  proposals too, and it costs the attacker 1d100+100 diplomatic score from
 *  peace, 1d15+10 from uneasy. History notes the treachery. */
export function attackAnyway(g, a, b) {
  if (a == null || b == null) return;
  const side = g.map.sides[a];
  const st = state(g, a, b);
  if (st === PEACE) game.addDiploScore(g, side, g.rng.dice(1, 100, 100));
  else if (st === INTERMEDIATE) game.addDiploScore(g, side, g.rng.dice(1, 15, 10));
  for (const k of [key(a, b), key(b, a)]) {
    g.diplomacy.state[k] = WAR;
    g.diplomacy.proposal[k] = WAR;
  }
  history.deed(g, side, history.TREACHERY, a, b, "");
}

/** At the end of a side's turn, its peace overtures count (484e:1063). */
export function scoreUpdate(g, side) {
  if (!g.diplomacy) return;
  const a = side.index;
  for (let b = 0; b < 8; b++) {
    if (b !== a && g.map.sides[b]) {
      const p = proposal(g, a, b), st = state(g, a, b);
      if (p < st && p < proposal(g, b, a)) addScore(g, side, st, p);
    }
  }
}

/** What a peaceful move adds to the diplomatic score (484e:1063). */
export function addScore(g, side, from, to) {
  let gain;
  if (from === WAR && to === PEACE) gain = g.rng.dice(1, 10, 10);
  else if (to < from) gain = g.rng.dice(1, 2, 1);
  if (gain != null) game.addDiploScore(g, side, gain);
  return gain;
}

/** Every side's diplomatic title, by side index (diplomatic_rating, 484e:0aed). */
export function ratings(g) {
  const order = g.sides.map((s, i) => ({ side: s, score: s.diploScore || 0, seq: i }));
  luaSort(order, (p, q) => (p.score !== q.score ? p.score < q.score : p.seq < q.seq));
  const ranks = RATING_RANKS[order.length] || RATING_RANKS[8];
  const out = {};
  order.forEach((entry, i) => {
    out[entry.side.index] = TITLES[(ranks[i] || TITLES.length) - 1];
  });
  return out;
}
