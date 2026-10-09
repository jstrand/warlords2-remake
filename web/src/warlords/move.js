// Movement: the cost grid, stack modes, pathfinding and walking a path.
//
// This follows WARLORD2.EXE: the original builds a one-byte-per-tile cost grid
// for the moving side (path_build_cost_grid, 1555:0d9e), runs a wavefront
// over it (path_wavefront, 1555:0373) and walks the resulting path
// (walk_path, 1a8b:07f9). See docs/rules.md > Movement and > Moving a stack.

import * as armytype from "./armytype.js";
import * as rules from "./rules.js";
import * as scn from "./scn.js";
import * as game from "./game.js";
import * as diplomacy from "./diplomacy.js";

// terrain type ids, as the scenario's own tile -> type table numbers them
export const ROAD = 0, BRIDGE = 1, WATER = 2, SHORE = 3;
export const FOREST = 4, HILLS = 5, MOUNTAINS = 6, PLAIN = 7;
export const MARSH = 8, TOWER = 9, CITY = 10, SITE = 11;

export const TERRAIN_NAMES = ["road", "bridge", "water", "shore", "forest", "hills",
  "mountains", "plain", "marsh", "tower", "city", "site"];

// DS:1274 in WARLORD2.EXE. 0 = impassable.
export const COST = [1, 1, 1, 2, 4, 6, 0, 2, 5, 2, 1, 2];

// cost-grid flags
export const WATER_F = 0x08, CROSS_F = 0x10;
export const HILLS_F = 0x20, FOREST_F = 0x40, CITY_F = 0x80;

// stack modes, as the original numbers them
export const BOAT = 0, LAND = 1, FLYING = 2;

export function has(byte, flag) { return (byte & flag) !== 0; }
function withF(byte, flag) { return byte | flag; }

export const WATER_PENALTY = 10;       // a land stack's route leaving open water
export const WATER_PENALTY_TO = 20;    // ... when the whole move is aimed at water
export const PAST_SHORE = 0x80;        // a step after going to sea or ashore
export const MAX_PATH = 200;           // a path is at most 200 compass steps

// compass directions, 0 = north, clockwise (the order the game stores paths in)
export const DIRS = [[0, -1], [1, -1], [1, 0], [1, 1], [0, 1], [-1, 1], [-1, 0], [-1, -1]];

/** How a stack moves, and what bonuses it carries: [mode, woods, hills, atSea]. */
export function stackMode(g, stack) {
  let atSea = false, anyBoat = false;
  let allFly = true, anyFly = false, allNonHeroFly = true, heroFlight = false;
  let woods = false, hills = false;
  for (const a of stack) {
    const t = g.types.byId[a.type];
    if (a.atSea) atSea = true;
    if (t.boat) anyBoat = true;
    if (t.flies && !a.atSea) anyFly = true;
    if (!t.flies || a.atSea) {
      allFly = false;
      if (a.type !== armytype.HERO) allNonHeroFly = false;
    }
    if (a.type === armytype.HERO && a.items) {
      for (const it of a.items) if (it.type === rules.ITEM_FLIGHT) heroFlight = true;
    }
    if (t.woodsMove) woods = true;
    if (t.hillsMove) hills = true;
  }
  if (atSea) return [LAND, false, false, true];
  if (anyBoat) return [BOAT, woods, hills, false];
  if (heroFlight || allFly || (allNonHeroFly && anyFly)) return [FLYING, woods, hills, false];
  return [LAND, woods, hills, false];
}

/** Just the mode, for the many callers that want only it. */
export function modeOf(g, stack) { return stackMode(g, stack)[0]; }

// 1555:109d walks round a city from its top left tile in these directions
const PORT_TOUR = [0, 2, 2, 4, 4, 4, 6, 6, 6, 0, 0, 0];

/** Is a city a port -- does any tile bordering its footprint hold a bridge,
 *  water or shore? Gives up the moment its walk would leave the map. */
export function isPort(g, city) {
  let x = city.x, y = city.y;
  for (const d of PORT_TOUR) {
    x += DIRS[d][0]; y += DIRS[d][1];
    if (x < 0 || y < 0 || x >= g.map.width || y >= g.map.height) return false;
    const t = scn.terrainAt(g.map, x, y);
    if (t === BRIDGE || t === WATER || t === SHORE) return true;
  }
  return false;
}

/** Build the cost grid for one side (path_build_cost_grid, 1555:0d9e), cached
 *  on the game state; move.invalidate drops it. */
export function grid(g, sideIndex) {
  g.grids = g.grids || {};
  const keyS = sideIndex == null ? -1 : sideIndex;
  const cached = g.grids[keyS];
  if (cached) return cached;

  const W = g.map.width, H = g.map.height;
  const out = new Array(W * H);
  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      const i = y * W + x;
      const terrain = scn.terrainAt(g.map, x, y);
      let byte = scn.roadAt(g.map, x, y) % 0x20 !== 0 ? 1 : COST[terrain];
      if (terrain === BRIDGE) byte = withF(withF(byte, CROSS_F), WATER_F);
      else if (terrain === WATER || terrain === SHORE) byte = withF(byte, WATER_F);
      else if (terrain === FOREST) byte = withF(byte, FOREST_F);
      else if (terrain === HILLS) byte = withF(byte, HILLS_F);
      if (g.map.crossing[i]) byte = withF(byte, CROSS_F);
      out[i] = byte;
    }
  }

  for (const c of g.map.cities) {
    const mine = c.ownerIndex == sideIndex && !c.razed;
    const port = (mine || c.razed) && isPort(g, c);
    for (let dx = 0; dx <= 1; dx++) {
      for (let dy = 0; dy <= 1; dy++) {
        const x = c.x + dx, y = c.y + dy;
        if (x < W && y < H) {
          const i = y * W + x;
          let byte = out[i];
          if (c.razed) {
            if (port) byte = withF(withF(byte, CROSS_F), WATER_F);
          } else {
            byte = withF(byte, CITY_F);
            if (mine) {
              if (port) byte = withF(withF(byte, CROSS_F), WATER_F);
            } else {
              byte = byte - (byte % 8);          // cost 0: impassable
            }
          }
          out[i] = byte;
        }
      }
    }
  }
  g.grids[keyS] = out;
  return out;
}

/** Drop the cached grids. Call after a city changes hands. */
export function invalidate(g) {
  g.grids = null;
}

/** The wavefront's weight for spreading from tile `from` to tile `to`, or null
 *  if it cannot spread there. This picks the route only. */
export function stepCost(fromByte, toByte, mode, woods, hills, penalty) {
  let cost = toByte % 8;
  const toWater = has(toByte, WATER_F), fromWater = has(fromByte, WATER_F);
  const toCross = has(toByte, CROSS_F);
  if (mode === FLYING) {
    if (cost === 0 && has(toByte, CITY_F)) return null;
    if (toWater && !toCross) return 2;
    if (cost === 0 || cost > 2) return 2;
    return cost;
  }
  if (mode === BOAT) {
    if (!toWater && !has(toByte, CITY_F)) return null;
    return cost === 0 ? null : cost;
  }
  if (cost === 0) return null;
  if (toWater !== fromWater && !toCross && !has(fromByte, CROSS_F)) return null;
  if (cost > 2) {
    if (woods && has(toByte, FOREST_F)) cost = 2;
    if (hills && has(toByte, HILLS_F)) cost = 2;
  }
  if ((!fromWater || has(fromByte, CROSS_F)) && toWater && !toCross) cost += penalty;
  return cost;
}

/** The moves a walk spends stepping onto a tile (1555:19e4). */
export function walkCost(byte, mode, woods, hills) {
  let cost = byte % 8;
  if (mode === FLYING) {
    if (has(byte, WATER_F) && !has(byte, CROSS_F)) return 2;
    if (cost === 0 || cost > 2) return 2;
    return cost;
  }
  if (mode === LAND && cost > 2) {
    if (woods && has(byte, FOREST_F)) cost = 2;
    if (hills && has(byte, HILLS_F)) cost = 2;
  }
  return cost;
}

/** Does a land stack stepping onto this tile go to sea (or come ashore)? */
export function crossesShore(byte, atSea) {
  if (has(byte, CROSS_F)) return false;
  return has(byte, WATER_F) !== !!atSea;
}

/** The movement points a stack shares: the lowest of its armies'. */
export function stackMoves(stack) {
  let least = Infinity;
  for (const a of stack) least = Math.min(least, a.moves || 0);
  return least === Infinity ? 0 : least;
}

/** The route a stack would take to (x, y) and how much of it it can walk now:
 *  { path, reach }, reach counting the steps within this turn's movement. */
export function preview(g, stack, x, y) {
  if (stack.length === 0) return null;
  const path = findPath(g, stack, stack[0].x, stack[0].y, x, y);
  if (!path || path.length === 0) return null;
  let left = stackMoves(stack), reach = 0;
  for (let i = 0; i < path.length; i++) {
    const step = path[i];
    if (step.cost > left) break;
    left -= step.cost;
    reach = i + 1;
  }
  return { path, reach };
}

// The search, as 1555:000a runs it: UNREACHED until the flood gets there,
// SHUT where the stack may never go, and then the distance from the
// destination -- negative while the tile still has to spread it further.
const UNREACHED = 30000;
export const SHUT = 30001;

// Where the wavefront spreads from a tile, by where the tile lies: inside,
// or along one of the map's edges or corners (4125:00e2, 00f6, 0162)
const NB_FIRST = [0, 9, 13, 19, 23, 29, 35, 39, 45, 49];
const NB_DX = [-1, 0, 1, 1, 1, 0, -1, -1, -10, 1, 1, 0, -10, 1, 1, 0, -1, -1,
  -10, 0, -1, -1, -10, 0, 1, 1, 1, 0, -10, -1, 0, 0, -1, -1, -10, 0, 1, 1, -10,
  -1, 0, 1, 1, -1, -10, -1, 0, -1, -10, 0, 1, 0, -1, -10];
const NB_DY = [-1, -1, -1, 0, 1, 1, 1, 0, -10, 0, 1, 1, -10, 0, 1, 1, 1, 0,
  -10, 1, 1, 0, -10, -1, -1, 0, 1, 1, -10, -1, -1, 1, 1, 0, -10, -1, -1, 0, -10,
  -1, -1, -1, 0, 0, -10, -1, -1, 0, -10, -1, 0, 1, 0, -10];

// the order path_trace tries the neighbours in (4125:0212)
const TRACE_ORDER = [0, 7, 1, 6, 2, 5, 3, 4];

/** The compass direction from one tile towards another (1a8b:0ae4). */
export function direction(x1, y1, x2, y2) {
  if (x1 === x2 && y1 === y2) return null;
  if (x1 === x2) return y2 < y1 ? 0 : 4;
  if (y1 === y2) return x2 < x1 ? 6 : 2;
  if (x1 <= x2) return y2 < y1 ? 1 : 3;
  return y2 < y1 ? 7 : 5;
}

/** map_distance (1a8b:0acc): the straight-line distance, rounded down. */
export function distance(x1, y1, x2, y2) {
  const ax = x1 - x2, ay = y1 - y2;
  return Math.floor(Math.sqrt(ax * ax + ay * ay));
}

const absval = (v) => (v < 0 ? -v : v);

/** path_prepare_grid (1555:08bf): nothing reached, and what the stack can
 *  never enter shut. */
export function prepare(g, grid, sideIndex, mode, dx, dy) {
  const W = g.map.width, H = g.map.height;
  const side = sideIndex != null ? g.map.sides[sideIndex] : null;
  const fog = g.map.options.hiddenMap !== 0 && side && !side.computer;
  const dist = new Array(W * H);
  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      const k = y * W + x;
      const b = grid[k];
      const hidden = fog && !game.seen(g, sideIndex, x, y);
      let shut;
      if (mode === LAND) shut = b % 8 === 0 || (hidden && !(x === dx && y === dy));
      else if (mode === BOAT) shut = !has(b, WATER_F) || (hidden && x !== dx && y !== dy);
      else shut = (b % 8 === 0 && has(b, CITY_F)) || (hidden && x !== dx && y !== dy);
      dist[k] = shut ? SHUT : UNREACHED;
    }
  }
  return dist;
}

// path_wavefront (1555:0373): sweeps squares ever wider round the
// destination; 6 tiles of margin on the first pass, 50 on the second.
function wavefront(q, pass) {
  const W = q.W, H = q.H;
  const sx = q.sx, sy = q.sy, dx = q.dx, dy = q.dy;
  const dist = q.dist, grid = q.grid, mode = q.mode;
  const margin = pass === 0 ? 6 : 50;
  if (pass === 0) q.ring = 0;
  const lox = Math.min(sx, dx), loy = Math.min(sy, dy);
  const hix = Math.max(sx, dx), hiy = Math.max(sy, dy);
  const bx0 = lox < margin ? 0 : lox - margin;
  const by0 = loy < margin ? 0 : loy - margin;
  const bx1 = hix + margin < W ? hix + margin : W - 1;
  const by1 = hiy + margin < H ? hiy + margin : H - 1;
  let found = 1, going = true;
  while (going) {
    let any = false;
    const r = q.ring;
    const x0 = Math.max(dx - r, 0, bx0), x1 = Math.min(dx + r, W - 1, bx1);
    const y0 = Math.max(dy - r, 0, by0), y1 = Math.min(dy + r, H - 1, by1);
    for (let x = x0; x <= x1; x++) {
      for (let y = y0; y <= y1; y++) {
        const k = y * W + x;
        const v = dist[k];
        if (v < 1) {
          any = true;
          const cur = grid[k];
          const nd = -v;
          dist[k] = nd;
          const curWater = has(cur, WATER_F), curCross = has(cur, CROSS_F);
          // the road builder spreads straight only (4125:00e2, class 9)
          let cls = q.roads ? 9 : 0;
          if (y === 0) cls = x === 0 ? 1 : (x === W - 1 ? 3 : 2);
          else if (y === H - 1) cls = x === 0 ? 6 : (x === W - 1 ? 8 : 7);
          else if (x === 0) cls = 4;
          else if (x === W - 1) cls = 5;
          let i = NB_FIRST[cls];
          while (NB_DX[i] !== -10) {
            const nx = x + NB_DX[i], ny = y + NB_DY[i];
            i++;
            const nk = ny * W + nx;
            const nv = dist[nk];
            if (nv !== SHUT) {
              const nb = grid[nk];
              let c = nb % 8;
              let ok = true;
              const nWater = has(nb, WATER_F), nCross = has(nb, CROSS_F);
              if (nx === sx && ny === sy) {
                if (mode === LAND && !curCross && !nCross && nWater !== curWater) ok = false;
                c = 1;
              } else if (mode === FLYING) {
                if ((!nWater || nCross) && c !== 0 && c < 3) {
                  // as dear as the tile, where that is 1 or 2
                } else {
                  c = 2;
                }
              } else if (mode === BOAT) {
                if (!nWater && !has(nb, CITY_F)) {
                  dist[nk] = SHUT;
                  ok = false;
                }
              } else {
                if (!curCross && !nCross && nWater !== curWater) {
                  ok = false;
                } else {
                  if (c > 2 && ((q.hills && has(nb, HILLS_F)) || (q.woods && has(nb, FOREST_F)))) c = 2;
                  if ((!curWater || curCross) && nWater && !nCross) c += q.penalty;
                }
              }
              if (ok && nd + c < absval(nv)) dist[nk] = v - c;
            }
          }
          if (x === sx && y === sy) going = false;
        }
      }
    }
    q.ring = r + 1;
    if (!any) { going = false; found = 0; }
  }
  return found;
}

// path_trace (1555:117b): from the start, each step to whichever neighbour
// the flood put nearest the destination; at most 198 steps.
function trace(q) {
  const W = q.W, H = q.H;
  const dist = q.dist, grid = q.grid;
  let x = q.sx, y = q.sy;
  let tx = q.dx, ty = q.dy;
  let d = absval(dist[y * W + x]);
  const steps = [];
  while (!(x === tx && y === ty)) {
    const cur = grid[y * W + x];
    const land = q.mode === LAND;
    const curWater = land && has(cur, WATER_F);
    const curCross = land && has(cur, CROSS_F);
    const toward = direction(x, y, tx, ty);
    let bx = null, by = null, bdir = null;
    for (const turn of TRACE_ORDER) {
      const dir = (toward + turn) % 8;
      const nx = x + DIRS[dir][0], ny = y + DIRS[dir][1];
      // the road builder steps diagonally only over water that is no bridge
      if (q.roads && dir % 2 === 1
          && !(has(grid[ny * W + nx], WATER_F) && !q.isBridge(nx, ny))) continue;
      if (nx >= 0 && ny >= 0 && nx < W && ny < H) {
        const v = dist[ny * W + nx];
        if (v !== UNREACHED && v !== SHUT) {
          const nb = grid[ny * W + nx];
          const blocked = land && !curCross && !has(nb, CROSS_F) && has(nb, WATER_F) !== curWater;
          // ... and takes a first neighbour no nearer than here
          const tie = q.roads && bx === null && absval(v) === d;
          if (!blocked && (absval(v) < d || tie)) {
            bx = nx; by = ny; bdir = dir; d = absval(v);
          }
        }
      }
    }
    if (bx !== null && steps.length < 198) {
      x = bx; y = by;
      steps.push({ x, y, dir: bdir });
    } else {
      tx = x; ty = y;
    }
  }
  return steps;
}

// path_single_step (1555:020e): a destination one tile away is simply stepped
// to, unless a land or boat stack would cross between land and water, or the
// ground there is impassable.
function singleStep(g, mode, sx, sy, dx, dy) {
  const wet = (t) => t === WATER || t === SHORE;
  const from = scn.terrainAt(g.map, sx, sy), to = scn.terrainAt(g.map, dx, dy);
  if (mode !== FLYING) {
    if (wet(from) !== wet(to)) return false;
    if (COST[to] === 0) return false;
  }
  return true;
}

/** The path from (sx,sy) to (dx,dy) for a stack, as a list of { x, y, cost }
 *  steps, or null if there is no route: 1555:000a, step for step. */
export function findPath(g, stack, sx, sy, dx, dy) {
  if (sx === dx && sy === dy) return [];
  const W = g.map.width, H = g.map.height;
  if (dx < 0 || dy < 0 || dx >= W || dy >= H) return null;

  const side = stack[0] ? stack[0].owner : undefined;
  const [mode, woods, hills, atSea] = stackMode(g, stack);
  const grd = grid(g, side);

  let path = null;
  const restore = new Map();
  if (distance(dx, dy, sx, sy) === 1 && singleStep(g, mode, sx, sy, dx, dy)) {
    path = [{ x: dx, y: dy }];
  } else {
    // a stack may always path *to* a city, just never through one it does
    // not own (path_mark_cities, 1555:0bde)
    const goal = dy * W + dx;
    const goalByte = grd[goal];
    const city = g.map.cityTile[goal];
    if (city && goalByte % 8 === 0 && has(goalByte, CITY_F)) {
      const port = isPort(g, city);
      for (let ox = 0; ox <= 1; ox++) {
        for (let oy = 0; oy <= 1; oy++) {
          const x = city.x + ox, y = city.y + oy;
          if (x < W && y < H) {
            const k = y * W + x;
            restore.set(k, grd[k]);
            let byte = grd[k] - grd[k] % 8 + COST[CITY];
            if (port) byte = withF(withF(byte, CROSS_F), WATER_F);
            grd[k] = byte;
          }
        }
      }
    }
    let penalty = 0;
    if (mode === LAND) {
      const t = scn.terrainAt(g.map, dx, dy);
      penalty = (t === WATER || t === SHORE) ? WATER_PENALTY_TO : WATER_PENALTY;
    }
    const q = { W, H, sx, sy, dx, dy, grid: grd, mode, woods, hills, penalty,
                dist: prepare(g, grd, side, mode, dx, dy) };
    let found = 0;
    if (q.dist[goal] !== SHUT || has(grd[goal], CITY_F)) {
      q.dist[goal] = -1;
      for (let pass = 0; pass <= 1; pass++) {
        if (found === 0) found = wavefront(q, pass);
      }
    }
    if (found === 1) path = trace(q);
  }

  // what the walk spends on each step (path_step_costs, 1555:18be)
  let ashore = false;
  for (const step of path || []) {
    const byte = grd[step.y * W + step.x];
    if (ashore) {
      step.cost = PAST_SHORE;
    } else {
      step.cost = walkCost(byte, mode, woods, hills);
      ashore = mode === LAND && crossesShore(byte, atSea);
    }
    delete step.dir;
  }
  for (const [k, byte] of restore) grd[k] = byte;
  if (path && path.length === 0) path = null;
  return path;
}

/** The route the random map generator lays a road along (5311:0c1c): the
 *  pathfinder run as pseudo-player 14 (path_build_cost_grid with player 14,
 *  4125:00e0 set). Its terrain costs are its own (`costs`, DS:01e0) and leave
 *  no ground impassable but a city; it moves by land rules, crossing water
 *  only at bridges and crossings; one pass of the wavefront, spreading only
 *  straight; and a trace that steps diagonally only over water.
 *  `m` gives terrain(x, y), road(x, y) and crossing(x, y) over a W x H map.
 *  Returns the steps as [{ x, y }], or null. */
export function roadRoute(m, costs, W, H, sx, sy, dx, dy) {
  if (dx < 0 || dy < 0 || dx >= W || dy >= H) return null;
  const grd = new Array(W * H);
  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      const t = m.terrain(x, y);
      let byte = m.road(x, y) ? 1 : costs[t];
      if (t === BRIDGE) byte |= CROSS_F | WATER_F;
      else if (t === WATER || t === SHORE) byte |= WATER_F;
      else if (t === FOREST) byte |= FOREST_F;
      else if (t === HILLS) byte |= HILLS_F;
      else if (t === CITY) byte = CITY_F;          // nobody's city is player 14's
      if (m.crossing(x, y)) byte |= CROSS_F;
      grd[y * W + x] = byte;
    }
  }
  const to = m.terrain(dx, dy);
  const dist = new Array(W * H);
  for (let k = 0; k < W * H; k++) dist[k] = grd[k] % 8 === 0 ? SHUT : UNREACHED;
  const q = { W, H, sx, sy, dx, dy, grid: grd, mode: LAND, woods: false, hills: false,
              penalty: to === WATER || to === SHORE ? WATER_PENALTY_TO : WATER_PENALTY,
              dist, roads: true, isBridge: (x, y) => m.terrain(x, y) === BRIDGE };
  const goal = dy * W + dx;
  if (dist[goal] === SHUT && !has(grd[goal], CITY_F)) return null;
  dist[goal] = -1;
  if (wavefront(q, 0) !== 1) return null;
  return trace(q).map((s) => ({ x: s.x, y: s.y }));
}

/** Walk `stack` along `path`, stopping where the rules say to stop.
 *  Returns { steps, spent, stopped, attack, walked }. */
function walkPath(g, stack, path) {
  const side = stack[0] ? stack[0].owner : undefined;
  let left = stackMoves(stack);
  const result = { steps: 0, spent: 0, stopped: "arrived" };
  let lastOk = 0, cumulative = 0;
  const costTo = {};

  for (let i = 1; i <= path.length; i++) {
    const step = path[i - 1];
    // a step is taken while what the walk has cost so far fits in the moves
    // left (1555:18be): there is no floor, so one move still buys a road
    if (step.cost > left) {
      result.stopped = "out of moves";
      break;
    }
    const city = g.map.cityTile[step.y * g.map.width + step.x];
    const here = game.armiesAt(g, step.x, step.y);
    if (city && !city.razed && city.ownerIndex != side) {
      if (!diplomacy.mayAttack(g, side, city.ownerIndex)) {
        result.stopped = "at peace";
        break;
      }
      result.stopped = "attack";
      result.attack = { x: step.x, y: step.y, city };
      break;
    }
    const theirs = here[0] && here[0].owner != side;
    if (theirs && diplomacy.mayAttack(g, side, here[0].owner)) {
      result.stopped = "attack";
      result.attack = { x: step.x, y: step.y };
      break;
    }
    left -= step.cost;
    cumulative += step.cost;
    // the stack may only stop on a tile of its own where it fits; other
    // steps -- a side at peace's stack among them -- are passed over
    // (1a8b:07f9)
    if (!theirs && here.length + stack.length <= rules.MAX_STACK) {
      lastOk = i;
      costTo[i] = cumulative;
    }
  }

  if (lastOk === 0) {
    if (result.stopped === "arrived") result.stopped = "blocked";
    return result;
  }

  const cost = costTo[lastOk];
  const dest = path[lastOk - 1];
  const [mode, , , wasAtSea] = stackMode(g, stack);
  for (const a of stack) {
    a.x = dest.x; a.y = dest.y;
    a.moves = Math.max(0, (a.moves || 0) - cost);
  }
  const change = settleSea(g, stack, dest.x, dest.y, mode, wasAtSea);
  if (change) {
    for (const a of stack) {
      if (!!a.atSea === (change === "to sea")) a.moves = 0;
    }
    result.stopped = "out of moves";
    result.attack = undefined;
  }

  // walking uncovers the map as it goes
  if (g.map.options.hiddenMap !== 0 && side != null) {
    const flying = modeOf(g, stack) === FLYING;
    let found = 0;
    for (const step of path) {
      if (step === dest) break;
      found += game.reveal(g, side, step.x, step.y, flying);
    }
    found += game.reveal(g, side, dest.x, dest.y, flying);
    if (found > 0) invalidate(g);
  }

  result.steps = lastOk;
  result.spent = cost;
  result.walked = [];
  for (let i = 0; i < lastOk; i++) result.walked.push({ x: path[i].x, y: path[i].y });
  if (lastOk < path.length && result.stopped === "arrived") result.stopped = "blocked";
  return result;
}

/** Going to sea and coming ashore, at the end of a walk (1a8b:04c8).
 *  Returns "to sea" or "ashore" when that happened, null otherwise. */
export function settleSea(g, stack, x, y, mode, wasAtSea) {
  if (mode !== LAND) return null;
  const t = scn.terrainAt(g.map, x, y);
  if (!wasAtSea) {
    if (t !== WATER && t !== SHORE) return null;
    let flier = false, walker = false;
    for (const a of stack) {
      if (g.types.byId[a.type].flies) flier = true;
      else if (a.type !== armytype.HERO) walker = true;
    }
    if (walker) flier = false;
    let change = null;
    for (const a of stack) {
      if (!g.types.byId[a.type].flies && (a.type !== armytype.HERO || !flier)) {
        a.atSea = true;
        change = "to sea";
      }
    }
    return change;
  } else if (t !== WATER && t !== SHORE && t !== BRIDGE) {
    for (const a of stack) a.atSea = false;
    return "ashore";
  }
  return null;
}

/** Walk `stack` along `path`; a tile left empty loses its encampment. */
export function walk(g, stack, path) {
  game.tidyTowers(g);
  const result = walkPath(g, stack, path);
  game.tidyTowers(g);
  return result;
}

/** Move a stack towards a tile: pathfind, then walk (move_stack_to, 1a8b:0001). */
export function moveTo(g, stack, x, y) {
  if (stack.length === 0) return { steps: 0, spent: 0, stopped: "blocked" };
  const path = findPath(g, stack, stack[0].x, stack[0].y, x, y);
  if (!path) return { steps: 0, spent: 0, stopped: "no route" };
  if (path.length === 0) return { steps: 0, spent: 0, stopped: "arrived" };
  return walk(g, stack, path);
}
