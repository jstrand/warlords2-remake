// The eight army slots under the map, and which of their armies move.
//
// Clicking a tile does not select the whole stack: it selects one army, and
// the bottom bar is where the player builds up the group that will move. The
// original keeps three parallel arrays over the tile's armies -- the group
// each belongs to, whether that group is the one moving, and the mark drawn
// under it. 89e0:0d30 builds them, 89e0:17e7 sorts and marks them, 89e0:000a
// writes the grouping back into the armies. docs/re/ui.md > The army slots.
//
// Slots are numbered from 0 here.

import { luaSort } from "../util.js";

export const MAX = 8;

// The mark under a slot (89e0:17e7): the head of the group that moves takes
// the tick, the head of every other group the cross.
export const CROSS = 0, TICK = 1;

// Group 0 means "not grouped": each such army is a group of its own.
export const UNGROUPED = 0;

function rank(g, s, i) {
  const row = g.map.fightOrder[s.side == null ? 8 : s.side];
  return row ? (row[s.army[i].type] || 0) : 0;
}

function swap(s, i, j) {
  [s.army[i], s.army[j]] = [s.army[j], s.army[i]];
  [s.group[i], s.group[j]] = [s.group[j], s.group[i]];
  [s.inGroup[i], s.inGroup[j]] = [s.inGroup[j], s.inGroup[i]];
}

/** Insertion-sort the slots so each group is contiguous: by group, then by
 *  fight order within it. */
function order(s, g) {
  for (let i = 1; i < s.n; i++) {
    let j = i;
    while (j > 0 && (s.group[j] < s.group[j - 1]
                     || (s.group[j] === s.group[j - 1] && rank(g, s, j - 1) < rank(g, s, j)))) {
      swap(s, j, j - 1);
      j--;
    }
  }
}

/** Work out the marks, and which group is the one that moves. */
function remark(s) {
  for (let i = 0; i < s.n; i++) {
    s.mark[i] = (i > 0 && s.group[i] === s.group[i - 1]) ? null : CROSS;
  }
  s.active = null;
  let head = -1;
  for (let i = 0; i < s.n; i++) if (s.inGroup[i]) { head = i; break; }
  if (head < 0) return;
  for (let i = 0; i < s.n; i++) {
    if (s.inGroup[i] !== (s.group[i] === s.group[head])) return;
  }
  s.mark[head] = TICK;
  s.active = s.group[head];
}

function refresh(s, g, resort) {
  if (resort) order(s, g);
  remark(s);
  return s;
}

/** Which armies a click on the tile picks up (8c07:06eb), as a Set. */
export function clicked(g, armies, side, anchor) {
  const row = g.map.fightOrder[side == null ? 8 : side];
  if (!anchor) {
    for (const a of armies) {
      if (!anchor || (row && (row[a.type] || 0) > (row[anchor.type] || 0))) anchor = a;
    }
  }
  if (!anchor) return new Set();
  const key = anchor.group || UNGROUPED;
  const out = new Set([anchor]);
  if (key !== UNGROUPED) {
    for (const a of armies) if ((a.group || UNGROUPED) === key) out.add(a);
  }
  return out;
}

/** The slots for the armies standing on one tile, at most eight of them.
 *  `selected` is the Set of armies that should be moving. */
export function build(g, armies, side, selected) {
  const s = { n: Math.min(armies.length, MAX), side, army: [], group: [], inGroup: [], mark: [] };
  for (let i = 0; i < s.n; i++) s.army[i] = armies[i];
  selected = selected || clicked(g, s.army, side);

  // 1b62:0a03's order: the remembered groups first, and within a group the
  // army highest in the fight order.
  const at = new Map();
  s.army.forEach((a, i) => at.set(a, i));
  const rowT = g.map.fightOrder[side == null ? 8 : side];
  luaSort(s.army, (p, q) => {
    const gp = p.group || 0, gq = q.group || 0;
    if (gp !== gq) return gp > gq;
    const rp = rowT ? (rowT[p.type] || 0) : 0;
    const rq = rowT ? (rowT[q.type] || 0) : 0;
    if (rp !== rq) return rp > rq;
    return at.get(p) < at.get(q);
  });

  // 89e0:0d30 numbers the groups off down the list
  let n = -1, previous;
  for (let i = 0; i < s.n; i++) {
    const key = s.army[i].group || UNGROUPED;
    if (!(key === previous && key !== UNGROUPED && n >= 0)) n++;
    s.group[i] = n;
    s.inGroup[i] = selected.has(s.army[i]);
    previous = key;
  }
  return refresh(s, g);
}

/** Click a slot (controls 224-231, 89e0:0963): add that army to the moving
 *  group, or drop it out into a group of its own. */
export function toggle(s, g, i) {
  if (i < 0 || i >= s.n) return s;
  if (!s.inGroup[i]) {
    for (let j = 0; j < s.n; j++) {
      if (s.inGroup[j]) { s.group[i] = s.group[j]; break; }
    }
    s.inGroup[i] = true;
  } else {
    const members = {};
    for (let j = 0; j < s.n; j++) members[s.group[j]] = (members[s.group[j]] || 0) + 1;
    if (members[s.group[i]] !== 1) {
      let free = 0;
      while (members[free]) free++;
      s.inGroup[i] = false;
      s.group[i] = free;
    }
  }
  return refresh(s, g, true);
}

/** Click the mark under a slot (controls 232-239, 89e0:0910). */
export function pickGroup(s, g, i) {
  if (i < 0 || i >= s.n) return s;
  const want = s.group[i];
  for (let j = 0; j < s.n; j++) s.inGroup[j] = s.group[j] === want;
  return refresh(s, g);
}

/** The Grp button while it is red, and the space bar (control 240). */
export function all(s, g) {
  for (let i = 0; i < s.n; i++) { s.group[i] = 0; s.inGroup[i] = true; }
  return refresh(s, g, true);
}

/** The Grp button while it is green (control 241). */
export function single(s, g) {
  for (let i = 0; i < s.n; i++) { s.group[i] = i; s.inGroup[i] = false; }
  if (s.n > 0) s.inGroup[0] = true;
  return refresh(s, g, true);
}

/** The armies that move. */
export function selected(s) {
  const out = [];
  for (let i = 0; i < s.n; i++) if (s.inGroup[i]) out.push(s.army[i]);
  return out;
}

/** Group Move: the pace of the slowest army in the group (1c8c:0912). */
export function moves(s) {
  let least = null;
  for (let i = 0; i < s.n; i++) {
    if (s.inGroup[i]) {
      const m = s.army[i].moves || 0;
      if (least === null || m < least) least = m;
    }
  }
  return least || 0;
}

/** Whether the Grp button shows green (89e0:0567). */
export function grouped(s) {
  if (s.n === 0) return false;
  if (s.n === 1) return true;
  for (let i = 0; i < s.n; i++) if (!s.inGroup[i]) return false;
  return true;
}

/** Rebuild the slots over a fresh list of armies, keeping whichever of them
 *  were moving. null when none are left. */
export function keep(s, g, armies) {
  const was = new Set();
  for (let i = 0; i < s.n; i++) if (s.inGroup[i]) was.add(s.army[i]);
  const sel = new Set();
  for (const a of armies) if (was.has(a)) sel.add(a);
  if (armies.length === 0) return null;
  return build(g, armies, s.side, sel.size > 0 ? sel : null);
}

/** Write the grouping back into the armies (89e0:000a's tail). */
export function commit(s, g) {
  const members = {};
  for (let i = 0; i < s.n; i++) members[s.group[i]] = (members[s.group[i]] || 0) + 1;

  const spareSet = new Set(), spare = [];
  for (let i = 0; i < s.n; i++) {
    const id = s.army[i].group || UNGROUPED;
    if (id > 1 && !spareSet.has(id)) { spareSet.add(id); spare.push(id); }
  }
  const claim = () => {
    if (spare.length > 0) return spare.shift();
    const used = new Set();
    for (const a of g.armies) used.add(a.group || UNGROUPED);
    let fresh = 2;
    while (used.has(fresh)) fresh++;
    return fresh;
  };

  const id = {};
  for (let i = 0; i < s.n; i++) {
    const gp = s.group[i];
    if (id[gp] === undefined) {
      if (members[gp] === 1) id[gp] = UNGROUPED;
      else if (gp === s.active) id[gp] = claim();
      else id[gp] = 1;
    }
    s.army[i].group = id[gp];
  }
  return s;
}
