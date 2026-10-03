// The computer players' shared machinery: their per-side data, the city
// neighbour table, the stack they have "selected", the flood fill they
// measure distances with, the battles they simulate, and the walk that moves
// a stack and fights what it meets.
//
// A port of WARLORD2.EXE; the addresses are Ghidra's (docs/re/ai.md).
//
// Anything that walks or fights is a generator function: the front end shows
// each walk and battle as it happens by yielding from ai.onWalk and
// ai.onFight, which is what the Lua remake did with a coroutine. Callers use
// `yield*`; ai.runSync drains one where nothing is shown.

import * as armytype from "../armytype.js";
import * as rules from "../rules.js";
import * as scn from "../scn.js";
import * as move from "../move.js";
import * as game from "../game.js";
import * as diplomacy from "../diplomacy.js";
import * as combat from "../combat.js";
import * as siteMod from "../site.js";
import * as ai from "../ai.js";
import * as groups from "./groups.js";
import * as moves from "./moves.js";
import { luaSort } from "../../util.js";

export const NEUTRAL = rules.NEUTRAL;

// city roles, AI data +0x56 + city (docs/re/ai.md > City roles)
export const JUST_TAKEN = 1, TAKING_NEUTRAL = 2, NEAR_NEUTRAL = 3;
export const WEAK = 4, BUILDING = 5, MEMBER = 6, RALLY = 7;
export const STOP = 8, EXPLORER2 = 11, EXPLORER = 13, NOWHERE = 14;

// an army's standing order, the low nibble of record +14
export const ORDER_NONE = 0, ORDER_CITY = 1, ORDER_ITEM = 2;
export const ORDER_SITE = 3, ORDER_ROAM = 4;

// per-city AI flags, +0x11e + city
export const CF_UNSEEN = 0x01, CF_FLIER = 0x02, CF_CLEANED = 0x04;
export const CF_MOVE12 = 0x08, CF_MAGIC = 0x10, CF_HERO = 0x20;
export const CF_FLYGROUP = 0x40, CF_MOVE16 = 0x80;

// an assault group's flags, entry +0x5a
export const GF_RAZE = 0x01, GF_SACK = 0x02, GF_PILLAGE = 0x04;
export const GF_MOVE16 = 0x08, GF_MOVE12 = 0x10, GF_FLY = 0x20;

export const MAX_GROUPS = 4;
export const UNREACHED = 30001;

export function has(v, bit) { return ((v || 0) & bit) !== 0; }
export function set(v, bit) { return (v || 0) | bit; }
export function clear(v, bit) { return (v || 0) & ~bit; }

/** map_distance (2012:1199): the straight-line distance, truncated. */
export function dist(x1, y1, x2, y2) {
  const dx = x1 - x2, dy = y1 - y2;
  return Math.floor(Math.sqrt(dx * dx + dy * dy));
}

export function owner(c) { return c.ownerIndex == null ? NEUTRAL : c.ownerIndex; }

/** Is the city still a city (not ruins)? */
export function standing(c) { return !c.razed; }

export function side(g, index) { return g.map.sides[index]; }

/** Is a side in the game? */
export function inPlay(g, index) {
  const s = g.map.sides[index];
  return s != null && s.inUse && s.alive !== false;
}

export function isComputer(g, index) {
  const s = g.map.sides[index];
  return s != null && !!s.computer;
}

/** The diplomatic state between two sides. */
export function state(g, a, b) {
  if (a === NEUTRAL || b === NEUTRAL || a == null || b == null) return 2;
  if (a === b) return 0;
  return diplomacy.state(g, a, b);
}

/** A side's proposal to another. */
export function proposal(g, a, b) {
  if (a === b) return 0;
  return diplomacy.proposal(g, a, b);
}

export function propose(g, a, b, v) {
  if (a === b) return;
  diplomacy.propose(g, a, b, v);
}

/** The city whose footprint covers (x, y). */
export function cityAt(g, x, y) {
  return g.map.cityTile[y * g.map.width + x];
}

/** The site on (x, y) (828e:04b5). */
export function siteAt(g, x, y) {
  return g.map.siteAt ? g.map.siteAt[y * g.map.width + x] : undefined;
}

export function terrain(g, x, y) { return scn.terrainAt(g.map, x, y); }

/** A temple, or a ruin nobody has searched. */
export function siteOpen(s) {
  return s.content === siteMod.TEMPLE || !s.searched;
}

/** Has the side seen (x, y), or any tile next to it (623c:16ae)? */
export function explored(g, sd, x, y) {
  if (g.map.options.hiddenMap === 0) return true;
  for (let tx = x - 1; tx <= x + 1; tx++) {
    for (let ty = y - 1; ty <= y + 1; ty++) {
      if (tx >= 0 && ty >= 0 && tx < g.map.width && ty < g.map.height && game.seen(g, sd, tx, ty)) {
        return true;
      }
    }
  }
  return false;
}

export function flies(g, a) { return g.types.byId[a.type].flies; }
export function magical(g, a) { return (g.types.byId[a.type].bonus[48] || 0) !== 0; }
export function ability(g, a) { return g.types.byId[a.type].bonus[52] || 0; }
export function woods(g, a) { return g.types.byId[a.type].woodsMove; }
export function hills(g, a) { return g.types.byId[a.type].hillsMove; }
export function isHero(a) { return a.type === armytype.HERO; }

/** The side's armies, last record first. */
export function armies(g, sideIndex) {
  const out = [];
  for (let i = g.armies.length - 1; i >= 0; i--) {
    const a = g.armies[i];
    if (a.owner === sideIndex) out.push(a);
  }
  return out;
}

/** Every placed army of a side on (x, y), last record first. */
export function onTile(g, sideIndex, x, y) {
  const out = [];
  for (let i = g.armies.length - 1; i >= 0; i--) {
    const a = g.armies[i];
    if (a.owner === sideIndex && !a.transit && a.x === x && a.y === y) out.push(a);
  }
  return out;
}

/** Is an army still in the game? */
export function alive(g, a) {
  return g.armies.includes(a);
}

/** A fresh block of AI data (ai_init_side, 59bf:084d). */
export function newData(g) {
  const d = {
    turns: 0,
    own: 0, enemy: 0, neutral: 0, unseen: 0,
    rebuildLimit: 10,
    bold: false,
    heroes: 0,
    rebuildType: 0, rebuildTypeRich: 0,
    bought: 0,
    dieHuman: 0, dieLord: 0, dieKnight: 0, dieWarlord: 0,
    sims: 10,
    searchers: 0, explorers: 0,
    raze: 0, sack: 0, pillage: 0, perCity: 0,
    bonusHuman: 0, bonusWarlord: 0, bonusLord: 0, bonusKnight: 0,
    poor: 0,
    solidarity: 0,
    minStrength: 0, flyCities: 0, strongCities: 0, fastCities: 0,
    early: 0,
    humanShare: 80,
    questCity: undefined,
    cautious: 0,
    questsDone: 0, itemsPassed: 0,
    maxGroups: 1,
    roles: {}, held: {}, flags: {}, garrison: {}, keep: {},
    // 1-based, as the original numbers them: an army's aiGroup is the
    // number, and 0 means none
    groups: [undefined],
    heroesKilled: [], armiesKilled: [], battles: [], lost: [],
    cityBattles: [], citiesLost: [],
  };
  for (let i = 0; i < 8; i++) {
    d.heroesKilled[i] = 0; d.armiesKilled[i] = 0; d.battles[i] = 0;
    d.lost[i] = 0; d.cityBattles[i] = 0; d.citiesLost[i] = 0;
  }
  for (let i = 1; i <= MAX_GROUPS; i++) d.groups[i] = emptyGroup();
  return d;
}

/** An empty assault group (5f19:08ba). Its lists are keyed 1-6 (members and
 *  staged 1-4) with holes, as the original's fixed slots are. */
export function emptyGroup() {
  return {
    active: 0, target: undefined, rally: undefined,
    members: {}, cities: {}, dist: {}, bonus: {},
    staged: {}, taken: {}, size: 0, flags: 0,
  };
}

/** The AI data of a side, made on first use. */
export function data(g, sd) {
  if (typeof sd === "number") sd = g.map.sides[sd];
  sd.ai = sd.ai || newData(g);
  return sd.ai;
}

export function role(d, c) { return d.roles[c.index] || 0; }
export function setRole(d, c, r) { d.roles[c.index] = r; }
export function cflag(d, c, bit) { return has(d.flags[c.index], bit); }
export function setCflag(d, c, bit) { d.flags[c.index] = set(d.flags[c.index], bit); }
export function clearCflag(d, c, bit) { d.flags[c.index] = clear(d.flags[c.index], bit); }

// The shared neighbour table (623c:0398): each city's six nearest cities by
// land path, with the path length to each. It depends on the map alone, so
// it is kept per scenario.
const neighbourCache = new Map();

/** [cities, dists] of a city's neighbours. */
export function neighbours(g, c) {
  if (!g.aiNeighbours) {
    const parts = [g.map.name || ""];
    for (const o of g.map.cities) parts.push(o.x + "," + o.y);
    const k = parts.join(";");
    g.aiNeighbours = neighbourCache.get(k) || buildNeighbours(g);
    neighbourCache.set(k, g.aiNeighbours);
  }
  const e = g.aiNeighbours[c.index];
  return [e ? e.cities : [], e ? e.dist : []];
}

/** For every city: flood by land from it (radius 45, then 60) and keep six
 *  neighbours, spread over the four directions (623c:0749). */
export function buildNeighbours(g) {
  const out = {};
  const cities = g.map.cities;
  for (const c of cities) {
    const pick = (range) => {
      const flood = floodFrom(g, -1, c.x, c.y, range, { mode: move.LAND });
      const cand = [];
      for (const o of cities) {
        if (o !== c) {
          const [d] = cityDistance(g, flood, o);
          if (d < 100) cand.push({ city: o, d });
        }
      }
      luaSort(cand, (p, q) => p.d < q.d);
      while (cand.length > 50) cand.pop();
      let left = 0, right = 0, up = 0, down = 0;
      const picked = [], dists = [];
      while (picked.length < 6) {
        let best = -1, bestScore = null;
        cand.forEach((e, i) => {
          if (!e.taken) {
            let score = e.d;
            if ((e.city.x < c.x && left > 2) || (c.x < e.city.x && right > 2)
                || (e.city.y < c.y && up > 2) || (c.y < e.city.y && down > 2)) {
              score += 50;
            }
            if (bestScore === null || score < bestScore) { best = i; bestScore = score; }
          }
        });
        if (best < 0) break;
        const e = cand[best];
        e.taken = true;
        picked.push(e.city); dists.push(e.d);
        if (e.city.x < c.x) left++;
        if (c.x < e.city.x) right++;
        if (e.city.y < c.y) up++;
        if (c.y < e.city.y) down++;
      }
      return [picked, dists, cand.length];
    };
    let [picked, dists, found] = pick(45);
    if (found === 0) [picked, dists] = pick(60);
    out[c.index] = { cities: picked, dist: dists };
  }
  return out;
}

/** The movement points a selection shares, and how it moves. */
export function refresh(g, sel) {
  let least = 100;
  for (const a of sel.armies) least = Math.min(least, a.moves || 0);
  sel.minMoves = sel.armies.length > 0 ? least : 0;
  sel.mode = sel.armies.length > 0 ? move.modeOf(g, sel.armies) : move.LAND;
  sel.hero = undefined;
  for (const a of sel.armies) if (isHero(a) && !sel.hero) sel.hero = a;
  return sel;
}

/** Select a list of armies as one stack (623c:14d3). The leader is the first. */
export function select(g, list) {
  const sel = { armies: [] };
  for (const a of list) {
    if (a && alive(g, a) && !a.transit) sel.armies.push(a);
  }
  sel.leader = sel.armies[0];
  return refresh(g, sel);
}

function fightRank(g, a) {
  const row = g.map.fightOrder[a.owner == null ? 8 : a.owner] || g.map.fightOrder[8];
  return row ? (row[a.type] || 0) : 0;
}

/** The lowest group number from 2 up that none of the side's armies uses
 *  (1b62:0cde). */
export function newGroup(g, sideIndex) {
  const used = new Set();
  for (const a of g.armies) {
    if (a.owner === sideIndex && !a.transit && a.group) used.add(a.group);
  }
  for (let n = 2; n <= 254; n++) if (!used.has(n)) return n;
  return 0;
}

/** Select a list and give it an order (623c:0ae7). */
export function order(g, list, ord, dest, flags) {
  const sel = select(g, list);
  if (sel.armies.length === 0) return null;
  let best = null;
  for (const a of sel.armies) if (!best || fightRank(g, a) > fightRank(g, best)) best = a;
  sel.leader = best;
  const grp = sel.armies.length > 1 ? newGroup(g, best.owner) : 0;
  for (const a of sel.armies) {
    if (ord != null) {
      a.aiOrder = ord; a.aiDest = dest;
      a.group = grp !== 0 ? grp : undefined;
      setTarget(g, a, ord, dest);
    }
    a.aiNeutral = undefined; a.aiExplore = undefined;
    if (flags) {
      if (has(flags, 0x80)) a.aiNeutral = true;
      if (has(flags, 0x20)) a.aiExplore = true;
      if (has(flags, 0x100)) a.aiParty = true;
    }
  }
  return refresh(g, sel);
}

/** Where an order points an army (623c:0c7e). */
export function setTarget(g, a, ord, dest) {
  if (ord === ORDER_ROAM) return;
  let x, y;
  if (ord === ORDER_CITY && dest != null) {
    const c = g.map.cities[dest];
    if (c) {
      x = c.x; y = c.y;
      if (c.ownerIndex === a.owner) {
        x++; y++;
      } else {
        if (x < a.x) x++;
        if (y < a.y) y++;
      }
    }
  } else if (ord === ORDER_ITEM && dest != null) {
    const it = g.map.items[dest];
    if (it && it.x != null) { x = it.x; y = it.y; }
  } else if (ord === ORDER_SITE && dest != null) {
    const s = g.map.sites[dest];
    if (s) { x = s.x; y = s.y; }
  }
  a.target = { x: x != null ? x : a.x, y: y != null ? y : a.y };
}

/** Forget an army's order. */
export function clearOrder(a) {
  a.aiOrder = ORDER_NONE;
  a.aiDest = undefined;
}

/** Select the stack an army moves with (8c07:06eb); they are marked moved. */
export function selectStackOf(g, a) {
  const list = [];
  for (const b of onTile(g, a.owner, a.x, a.y)) {
    if (b === a || (a.group && a.group !== 0 && b.group === a.group)) {
      list.push(b);
      b.aiMoved = true;
    }
  }
  const sel = select(g, list);
  let best = null;
  for (const b of sel.armies) if (!best || fightRank(g, b) > fightRank(g, best)) best = b;
  sel.leader = best || a;
  return sel;
}

/** The side's armies on a tile with at least `minMoves` moves left, up to
 *  eight (623c:12c9). */
export function collect(g, sideIndex, x, y, minMoves) {
  const out = [];
  for (const a of onTile(g, sideIndex, x, y)) {
    if (out.length >= rules.MAX_STACK) break;
    if (!minMoves || (a.moves || 0) >= minMoves) out.push(a);
  }
  return out;
}

/** The side's armies on a tile in group nibble `grp` with standing order
 *  `ord`, and a maximum move of at least `minMax` (623c:13b2). */
export function collectOrdered(g, sideIndex, x, y, grp, ord, minMax) {
  const out = [];
  for (const a of onTile(g, sideIndex, x, y)) {
    if (out.length >= rules.MAX_STACK) break;
    if ((a.aiGroup || 0) === (grp || 0) && (a.aiOrder || 0) === (ord || 0)
        && (!minMax || (a.maxMoves || 0) >= minMax)) {
      out.push(a);
    }
  }
  return out;
}

// move.DIRS in the order Lua's pairs() walks the table the Lua keeps them in:
// 1..7, then 0. Dijkstra's answer does not depend on it, but it is the same.
const FLOOD_DIRS = [1, 2, 3, 4, 5, 6, 7, 0].map((i) => move.DIRS[i]);

/** Flood the map from (x, y) the way the selection moves (1555:1ab9): every
 *  tile within `range` gets its path cost from the start. */
export function floodFrom(g, sideIndex, x, y, range, sel) {
  const W = g.map.width, H = g.map.height;
  const grid = move.grid(g, sideIndex);
  const mode = sel && sel.mode != null ? sel.mode : move.LAND;
  let wd = false, hl = false;
  if (sel && sel.armies) {
    const m = move.stackMode(g, sel.armies);
    wd = m[1]; hl = m[2];
  }
  const x0 = Math.max(0, x - range), x1 = Math.min(W - 1, x + range);
  const y0 = Math.max(0, y - range), y1 = Math.min(H - 1, y + range);
  const distT = new Map(), done = new Set();
  const start = y * W + x;
  distT.set(start, 0);
  // a binary heap of [key, distance], 1-based as the Lua's
  const heap = [null, [start, 0]];
  let n = 1;
  const push = (k, d) => {
    n++;
    heap[n] = [k, d];
    let i = n;
    while (i > 1) {
      const p = Math.floor(i / 2);
      if (heap[p][1] <= heap[i][1]) break;
      [heap[p], heap[i]] = [heap[i], heap[p]];
      i = p;
    }
  };
  const pop = () => {
    const top = heap[1];
    heap[1] = heap[n]; heap[n] = undefined; n--;
    let i = 1;
    for (;;) {
      const l = i * 2, r = i * 2 + 1;
      let best = i;
      if (l <= n && heap[l][1] < heap[best][1]) best = l;
      if (r <= n && heap[r][1] < heap[best][1]) best = r;
      if (best === i) break;
      [heap[best], heap[i]] = [heap[i], heap[best]];
      i = best;
    }
    return top;
  };
  while (n > 0) {
    const top = pop();
    const k = top[0];
    if (!done.has(k)) {
      done.add(k);
      const d = distT.get(k);
      const kx = k % W, ky = Math.floor(k / W);
      for (const dir of FLOOD_DIRS) {
        const nx = kx + dir[0], ny = ky + dir[1];
        if (nx >= x0 && ny >= y0 && nx <= x1 && ny <= y1) {
          const nk = ny * W + nx;
          if (!done.has(nk)) {
            const c = move.stepCost(grid[k], grid[nk], mode, wd, hl, move.WATER_PENALTY);
            if (c != null && (!distT.has(nk) || d + c < distT.get(nk))) {
              distT.set(nk, d + c);
              push(nk, d + c);
            }
          }
        }
      }
    }
  }
  return { dist: distT, W, H };
}

export function floodAt(flood, x, y) {
  const d = flood.dist.get(y * flood.W + x);
  return d != null ? d + 1 : UNREACHED;
}

// the ring round a city's 2x2 footprint, as 59bf:0a85 walks it
const RING = [0, 2, 2, 4, 4, 4, 6, 6, 6, 0, 0, 0];
const DX = [0, 1, 1, 1, 0, -1, -1, -1];
const DY = [-1, -1, 0, 1, 1, 1, 0, -1];

/** The flood's distance to a city: [best, bx, by], the least over the twelve
 *  tiles round it, or 100 (1000 with `far`) when none is closer (59bf:0a85). */
export function cityDistance(g, flood, c, far) {
  let best = far ? 1000 : 100;
  let bx, by;
  let x = c.x, y = c.y;
  for (const dir of RING) {
    const nx = x + DX[dir], ny = y + DY[dir];
    if (nx < 0 || ny < 0 || nx >= 112 || ny >= 156) return [best, bx, by];
    x = nx; y = ny;
    const d = floodAt(flood, x, y);
    if (d < best) { best = d; bx = x; by = y; }
  }
  return [best, bx, by];
}

/** The chance, in percent, that the selection takes (x, y) (623c:15c5). */
export function odds(g, sel, x, y) {
  if (!sel || sel.armies.length === 0) return 0;
  const own = sel.armies[0].owner;
  let n = own != null ? data(g, own).sims : 10;
  if (n < 1) n = 1;
  const [attackers, defenders] = combat.lines(g, sel.armies, x, y);
  if (defenders.length === 0) return 100;
  let wins = 0;
  for (let i = 0; i < n; i++) if (combat.resolve(g, attackers, defenders, x, y).won) wins++;
  return Math.floor(wins * 100 / n);
}

/** A hero picks up whatever lies where it stands (6087:0427). */
export function pickUp(g, sel) {
  if (!sel.hero || !alive(g, sel.hero)) return;
  const h = sel.hero;
  const c = cityAt(g, h.x, h.y);
  for (const it of g.map.items) {
    if (it.status === 1 && it.x != null && !it.planted) {
      let here;
      if (c) here = it.x >= c.x && it.x <= c.x + 1 && it.y >= c.y && it.y <= c.y + 1;
      else here = it.x === h.x && it.y === h.y;
      if (here) {
        it.status = 3; it.x = undefined; it.y = undefined;
        h.items = h.items || [];
        h.items.push(it);
      }
    }
  }
}

/** Walk the selection towards (tx, ty) (1a8b:0c4f and 1a8b:07f9). Returns
 *  [code, x, y]: 1 no path, 2 stopped short, 3 an enemy army is next, 4 the
 *  path is walked, 5 an enemy city is next; with the tile ahead. */
function* step(g, sel, tx, ty) {
  const lead = sel.leader;
  lead.target = { x: tx, y: ty };
  const path = move.findPath(g, sel.armies, lead.x, lead.y, tx, ty);
  if (!path) return [1];
  if (path.length === 0) return [4];
  const r = move.walk(g, sel.armies, path);
  if (r.steps > 0) yield* ai.walked(g, sel.armies, r);
  refresh(g, sel);
  pickUp(g, sel);
  if (r.stopped === "attack") {
    return [r.attack.city ? 5 : 3, r.attack.x, r.attack.y];
  } else if (r.stopped === "at peace") {
    const ahead = path[r.steps];
    if (ahead && cityAt(g, ahead.x, ahead.y)) return [5, ahead.x, ahead.y];
    return [2];
  } else if (r.stopped === "arrived") {
    return [4];
  }
  return [2];
}

/** Fight for (x, y) with the selection, for real. Returns the result. */
export function* fight(g, sel, x, y) {
  // decided, shown, and only then taken effect (after_battle, 67cc:0a6b)
  const result = game.decideAttack(g, sel.armies, x, y);
  if (ai.hooks.onFight) yield* ai.hooks.onFight(g, sel.armies, x, y, result);
  game.applyAttack(g, result);
  const keep = sel.armies.filter((a) => alive(g, a));
  sel.armies = keep;
  if (!alive(g, sel.leader)) sel.leader = keep[0];
  refresh(g, sel);
  return result;
}

/** Mark the selection done for the turn (8c07:08fb). */
export function done(sel) {
  for (const a of sel.armies) a.done = true;
}

/** move_stack_to (1a8b:0001) for a computer: walk to (tx, ty), fighting what
 *  the walk runs into, until the walk stops. `keep` leaves the stack selected
 *  and not done. Returns the last walk code. */
export function* moveTo(g, sel, tx, ty, keep) {
  if (!sel || sel.armies.length === 0 || tx < 0 || ty < 0
      || tx >= g.map.width || ty >= g.map.height) {
    return 0;
  }
  let code;
  let again = true;
  while (again) {
    again = false;
    if (sel.armies.length === 0) break;
    let ax, ay;
    [code, ax, ay] = yield* step(g, sel, tx, ty);
    const lead = sel.leader;
    if (lead && lead.target && lead.x === lead.target.x && lead.y === lead.target.y) {
      code = 4;
      lead.target = undefined;
    }
    if (code === 2) {
      done(sel);
    } else if (code === 5) {
      const c = cityAt(g, tx, ty);
      if (c && standing(c)) {
        const dest = (lead && lead.aiDest != null && g.map.cities[lead.aiDest]) || c;
        let nx, ny;
        [again, nx, ny] = yield* attackCity(g, sel, dest, ax, ay);
        if (again) { tx = nx; ty = ny; }
      }
    } else if (code === 4) {
      if (lead && lead.aiOrder === ORDER_SITE && sel.hero) {
        const s = siteAt(g, sel.hero.x, sel.hero.y);
        if (s) yield* moves.searchSite(g, sel, sel.hero, s);
      }
      for (const a of sel.armies) {
        clearOrder(a);
        a.aiGroup = 0;
      }
    } else if (code === 3) {
      yield* fight(g, sel, ax, ay);
      if (sel.armies.length > 0) again = true;
    }
  }
  if (!keep) done(sel);
  return code;
}

/** Attack a city the walk has run into (1a8b:0254): [again, x, y]. */
export function* attackCity(g, sel, dest, ax, ay) {
  const d = data(g, sel.leader.owner);
  const me = sel.leader.owner;
  const own = owner(dest);
  const city = cityAt(g, ax, ay);
  if (own === NEUTRAL) {
    yield* fight(g, sel, ax, ay);
    if (city && city.ownerIndex === me) setRole(d, city, JUST_TAKEN);
  } else if (own !== me) {
    let pick = dest;
    if (d.questCity !== dest.index || !sel.hero) pick = retarget(g, sel, dest, ax, ay);
    if (pick !== dest) {
      for (const a of sel.armies) { a.aiOrder = ORDER_CITY; a.aiDest = pick.index; }
      return [true, pick.x, pick.y];
    }
    const was = city ? owner(city) : undefined;
    yield* fight(g, sel, ax, ay);
    if (city && city.ownerIndex === me) groups.earlyVengeance(g, me, city, was);
  }
  for (const a of sel.armies) clearOrder(a);
  return [false];
}

/** Is the city worth attacking with this stack, or is there better nearby
 *  (623c:1771)? */
export function retarget(g, sel, c, x, y) {
  const me = sel.leader.owner;
  const od = odds(g, sel, x, y);
  const own = owner(c);
  let fair = own === NEUTRAL || state(g, own, me) === 2;
  if (own !== NEUTRAL && !isComputer(g, own) && g.rng.dice(1, 4, -1) === 0) fair = true;
  if (!fair || od < 51) {
    const flood = floodFrom(g, me, sel.leader.x, sel.leader.y, 15, sel);
    const best = bestCity(g, sel, flood, fair);
    return best || c;
  }
  return c;
}

/** The best city for the selection to go for, by the flood (623c:1885). */
export function bestCity(g, sel, flood, skipOwn) {
  const me = sel.leader.owner;
  const d = data(g, me);
  let best = null, bestScore = -1;
  const cities = g.map.cities;
  for (let i = cities.length - 1; i >= 0; i--) {
    const c = cities[i];
    const own = owner(c);
    if (!(skipOwn && own === me) && standing(c) && d.questCity !== c.index
        && !cflag(d, c, CF_UNSEEN)
        && (own === NEUTRAL || own === me || state(g, own, me) === 2)) {
      let [dst, px] = cityDistance(g, flood, c);
      if (px != null) {
        let od;
        if (own === me) { od = 15; dst += 20; } else od = odds(g, sel, c.x, c.y);
        let score;
        if (od >= 51 && dst < sel.minMoves) score = 400 - dst + od * 10;
        else if (od > 10) score = 100 - dst + od * 5;
        else score = -2;
        if (score > bestScore) { best = c; bestScore = score; }
      }
    }
  }
  return best;
}
