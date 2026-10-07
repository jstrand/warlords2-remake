// The random map generator, "A Random World": random_map_setup (7bab:10e8)
// and random_map_generate (4bed:011c), phase for phase. docs/re/random_map.md.
//
// RANDOM\RANDOM.DAT is both the generator's parameters and its memory: the
// original reads the whole 0xa560 bytes into one buffer and works in it, the
// 112x156 grid of terrain types at +0x6120 included. This does the same, so
// the offsets below are RANDOM.DAT's own, and the few reads the original makes
// off the edge of the grid land where they did. Writes off the grid are
// dropped; the original's would corrupt its own tables.
//
// The result is a scenario like any other -- RANDOM.SCN (Erythea's, rewritten),
// .MAP, .RD, .SGN, .CTY and .SPC -- which scn.load reads as it reads the
// shipped ones. The dice are the engine's (rng.js), so a map is not the one
// the original would make from the same seed, but every rule is its rule.

import * as vfs from "../vfs.js";
import * as armytype from "./armytype.js";
import "./game.js";      // before move.js, which game.js and the AI import in turn
import * as move from "./move.js";
import { u16, cstr } from "./bytes.js";
import { bytesOf, fmt } from "../util.js";

export const DIR = "RANDOM";
export const FILES = ["SCN", "MAP", "RD", "SGN", "CTY", "SPC"];   // what it makes
const W = 112, H = 156;

// terrain type ids, as the grid and the scenario's tile table number them
const ROAD = 0, BRIDGE = 1, WATER = 2, SHORE = 3, FOREST = 4, HILLS = 5;
const MOUNTAINS = 6, PLAIN = 7, MARSH = 8, TOWER = 9, CITY = 10, SITE = 11;

// RANDOM.DAT: the parameters (the sliders add to these)
const P_CITIES = 0x2a, P_EDGE_POINTS = 0x2c;
const P_MOUNTAINS = 0x34, P_HILLS = 0x36, P_WIDE_RIVERS = 0x38, P_RIVERS = 0x3a;
const P_FOREST = 0x3c, P_PASSES = 0x3e, P_EROSION = 0x40;
const SLIDER_HILLS = 0x44, SLIDER_WATER = 0x52, SLIDER_FOREST = 0x60, SLIDER_CITIES = 0x6e;
const CORNERS = 0x7c, EDGES = 0x8c, DIAGONALS = 0xac;
const NB = 0xbc;          // the 8 neighbours, S first and anticlockwise
const NB_ROAD = 0xdc;     // the 8 again, N first and clockwise: road shapes
// RANDOM.DAT: the generator's working space and its tables
const EDGE_COUNT = 0xfc, EDGE_PTS = 0x104, EDGE_START = 0x244, EDGE_END = 0x254;
const SHAPE = 0x264;      // neighbour mask -> tile variant, -1 for none
const ROAD_SHAPE = 0x364; // neighbour mask -> road id - 1, -1 for none
const TILES = 0x464;      // per terrain type, 16 variants x 2 checkerboard tiles
const SIDE_NAMES = 0x764, SITE_FORMATS = 0xed2;
const SIGN_CITY = 0x1888, SIGN_LEAGUES = 0x189c, SIGNS = 0x18b0;
const SIDE_CLASS = 0x1d62, REGION_CLASS = 0x1d72, PRODUCTION = 0x1d82;
const FOREST_DONE = 0x1f52, FOREST_WANTED = 0x1f54;
const NODES = 0x5dd6, NODE_COUNT = 0x611e, GRID = 0x6120;
const DAT_SIZE = 0xa560;

// .SCN offsets (docs/formats/scenario.md)
const S_SIDE_REC = 387, S_RANDOM = 0x120, S_TERRAIN_SET = 0x161;
const S_TERRAIN = 0x710, S_SITE_COUNT = 0x80f, S_SITES = 0x811, SITE_STRIDE = 31;
const S_CITY_COUNT = 5499, S_CITIES = 5501, CITY_STRIDE = 65;
const N_SITES = 40;

// directions the signposts give (4125:2b7c)
const COMPASS = ["north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest"];
// the twelve tiles round a city's 2x2 footprint, where its roads start (DS:0382)
const ROUND_CITY = [[-1, -1], [0, -1], [1, -1], [2, -1], [2, 0], [2, 1], [2, 2], [1, 2],
  [0, 2], [-1, 2], [-1, 1], [-1, 0]];
// the road builder's terrain costs, pseudo-player 14 (DS:01e0)
const ROAD_COST = [1, 1, 3, 3, 4, 5, 7, 2, 5, 2, 1, 7];
// what the start menu shows beside each slider (4125:28e0, formats 4125:2918)
export const SLIDER_SHOWS = [[5, 7, 9, 11, 13, 15, 17], [5, 7, 9, 11, 13, 15, 17],
  [70, 75, 80, 85, 90, 95, 100], [9, 11, 13, 15, 17, 19, 21]];
export const SLIDER_FORMATS = ["(%d%%)", "(%d%%)", "(%d)", "(%d%%)"];
export const RANDOM_SLIDER = 7;      // a slider of 7 is rolled: 1d7-1

const abs = (v) => (v < 0 ? -v : v);
const idiv = (a, b) => Math.trunc(a / b);       // C's division
const SAFETY = 200000;                          // a loop the original might never leave

/** Chebyshev distance (4c49:0fed). */
function reach(x1, y1, x2, y2) { return Math.max(abs(x1 - x2), abs(y1 - y2)); }

/** map_distance (1a8b:0acc): the straight line, rounded down. */
function distance(x1, y1, x2, y2) {
  const dx = x1 - x2, dy = y1 - y2;
  return Math.floor(Math.sqrt(dx * dx + dy * dy));
}

/** 4c49:0dd0: the step from one tile towards another, one of 8 compass
 *  directions by the slope, cut at tan 22.5 and tan 67.5 (4125:0292). */
function step(x1, y1, x2, y2) {
  if (x2 === x1) return [0, y2 === y1 ? 0 : y2 < y1 ? -1 : 1];
  if (y2 === y1) return [x2 < x1 ? -1 : 1, 0];
  const s = (y2 - y1) / (x2 - x1);
  if (x2 < x1) {
    if (s >= 2.414) return [0, -1];
    if (s >= 0.414) return [-1, -1];
    if (s >= -0.414) return [-1, 0];
    if (s >= -2.414) return [-1, 1];
    return [0, 1];
  }
  if (s >= 2.414) return [0, 1];
  if (s >= 0.414) return [1, 1];
  if (s >= -0.414) return [1, 0];
  if (s >= -2.414) return [1, -1];
  return [0, -1];
}

/** The direction from one tile to another for a signpost (828e:0b51). */
function compass(x1, y1, x2, y2) {
  if (x1 === x2) return y1 < y2 ? 4 : 0;
  if (y1 === y2) return x1 < x2 ? 2 : 6;
  if (x2 < x1 && y2 < y1) return 7;
  if (x2 < x1 && y1 < y2) return 5;
  if (x1 < x2 && y2 < y1) return 1;
  return 3;
}

export class Generator {
  constructor(dat, template, types, rng, opts) {
    this.m = new Uint8Array(DAT_SIZE);
    this.m.set(dat.subarray(0, DAT_SIZE));
    this.dv = new DataView(this.m.buffer);
    this.scn = new Uint8Array(template);
    this.sv = new DataView(this.scn.buffer);
    this.types = types;
    this.rng = rng;
    this.allies = !!opts.allies;
    this.tiles = new Uint16Array(W * H);
    this.rd = new Uint8Array(W * H);
    this.cty = "";
    this.spc = "";
    this.siteCell = -1;            // 4125:02d4, where the sites' round begins
    this.flags = {};               // 4125:02d6-02de, what touches a city
  }

  // --- memory -------------------------------------------------------------

  r16(o) { return this.dv.getInt16(o, true); }
  w16(o, v) { this.dv.setInt16(o, v, true); }
  str(o, n) { return cstr(this.m, o, n); }
  s16(o) { return this.sv.getInt16(o, true); }
  ws16(o, v) { this.sv.setInt16(o, v, true); }
  wstr(o, s) {                     // strcpy into the scenario image
    for (let i = 0; i < s.length; i++) this.scn[o + i] = s.charCodeAt(i) & 0xff;
    this.scn[o + s.length] = 0;
  }

  /** The terrain grid. Off the grid a read gets whatever RANDOM.DAT holds
   *  there, as the original's did. */
  at(x, y) {
    const o = GRID + y * W + x;
    return o >= 0 && o < DAT_SIZE ? this.m[o] : 0;
  }
  put(x, y, t) {
    const i = y * W + x;
    if (i >= 0 && i < W * H) this.m[GRID + i] = t;
  }
  nx(i) { return this.r16(NB + 4 * i); }
  ny(i) { return this.r16(NB + 4 * i + 2); }
  /** The index of a step among the 8 neighbours, or -1. */
  nbIndex(dx, dy) {
    let k = -1;
    for (let i = 0; i < 8; i++) if (this.nx(i) === dx && this.ny(i) === dy) k = i;
    return k;
  }

  // the tile map: the low byte is the tile, bit 15 a crossing
  tile(x, y) { return x >= 0 && y >= 0 && x < W && y < H ? this.tiles[y * W + x] & 0xff : 0; }
  setTile(x, y, t) {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    const i = y * W + x;
    this.tiles[i] = (this.tiles[i] & 0xff00) | (t & 0xff);
  }
  road(x, y) { return x >= 0 && y >= 0 && x < W && y < H ? this.rd[y * W + x] & 0x1f : 0; }
  setRoad(x, y, r) {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    const i = y * W + x;
    this.rd[i] = (this.rd[i] & 0xe0) | (r & 0x1f);
  }
  terrainOf(t) { return this.scn[S_TERRAIN + t]; }
  /** Variant `v` of terrain type `t` from RANDOM.DAT's tile table, the
   *  checkerboard picking one of its pair. */
  tileFor(t, v, x, y) { return this.r16(TILES + 64 * t + 4 * v + 2 * parity(x, y)) & 0xff; }

  // the cities, as the scenario holds them
  get cityCount() { return this.s16(S_CITY_COUNT); }
  set cityCount(n) { this.ws16(S_CITY_COUNT, n); }
  cx(i) { return this.s16(S_CITIES + CITY_STRIDE * i); }
  cy(i) { return this.s16(S_CITIES + CITY_STRIDE * i + 2); }
  crec(i) { return S_CITIES + CITY_STRIDE * i; }

  /** dice(n, sides, bonus) (6ecb:02bf): no sides gives the bonus, and the
   *  clamp makes fewer than none give n + bonus. */
  dice(n, sides, bonus = 0) {
    if (sides === 0) return bonus;
    if (sides < 0) return n + bonus;
    return this.rng.dice(n, sides, bonus);
  }

  /** terrain_near (4c49:11f5): is type `t` on any of the 8 tiles round. */
  near(x, y, t) {
    for (let i = 0; i < 8; i++) {
      const ax = x + this.nx(i), ay = y + this.ny(i);
      if (ax >= 0 && ay >= 0 && ax < W && ay < H && this.at(ax, ay) === t) return true;
    }
    return false;
  }

  /** 4c49:117d: plain on any of the 8 tiles round. */
  nearPlain(x, y) { return this.near(x, y, PLAIN); }

  /** 4bed:04ae: push a point by (1d3-2) x 1dn each way; with `toEdge`, 30%
   *  of the time it then goes to the nearest edge. Clamped to the map. */
  jitter(p, n, toEdge) {
    p.x += this.dice(1, 3, -2) * this.dice(1, n, 0);
    p.y += this.dice(1, 3, -2) * this.dice(1, n, 0);
    if (this.dice(1, 100, 0) < 30 && toEdge) {
      const l = p.x, t = p.y, r = W - 1 - p.x, b = H - 1 - p.y;
      if (l <= t && l <= r && l <= b) p.x = 0;
      if (t <= l && t <= r && t <= b) p.y = 0;
      if (r <= t && r <= l && r <= b) p.x = W - 1;
      if (b <= t && b <= l && b <= r) p.y = H - 1;
    }
    clamp(p);
  }

  // --- 4c49: the coastline --------------------------------------------------

  pt(o) { return { x: this.r16(o), y: this.r16(o + 2) }; }
  setPt(o, p) { this.w16(o, p.x); this.w16(o + 2, p.y); }

  /** 4c49:0087: where edge `i`'s stretch of coast begins and ends -- the
   *  map's corners drawn in 10 along the diagonal, then pushed about. */
  coastEnds(i) {
    const j = (i + 1) % 4;
    if (this.r16(P_EDGE_POINTS + 2 * i) < 3) {
      this.setPt(EDGE_START + 4 * i, this.pt(EDGES + 8 * i));
      this.setPt(EDGE_END + 4 * i, this.pt(EDGES + 8 * i + 4));
      return;
    }
    const a = this.pt(EDGES + 8 * i);
    a.x -= 10 * this.r16(DIAGONALS + 4 * i); a.y -= 10 * this.r16(DIAGONALS + 4 * i + 2);
    this.jitter(a, 16, true);
    this.put(a.x, a.y, PLAIN);
    this.setPt(EDGE_START + 4 * i, a);
    const b = this.pt(EDGES + 8 * i + 4);
    b.x -= 10 * this.r16(DIAGONALS + 4 * j); b.y -= 10 * this.r16(DIAGONALS + 4 * j + 2);
    this.jitter(b, 16, true);
    this.put(b.x, b.y, PLAIN);
    this.setPt(EDGE_END + 4 * i, b);
  }

  /** 4c49:0206: swap an edge's end with the next one's start where that
   *  keeps the outline turning round the middle. */
  coastOrder() {
    for (let i = 0; i < 4; i++) {
      const j = (i + 1) % 4;
      const e = this.pt(EDGE_END + 4 * i), s = this.pt(EDGE_START + 4 * j);
      if ((56 - e.x) / (78 - e.y) < (56 - s.x) / (78 - s.y)) {
        this.setPt(EDGE_END + 4 * i, s);
        this.setPt(EDGE_START + 4 * j, e);
      }
    }
  }

  onEdge(p) { return p.x === 0 || p.y === 0 || p.x === W - 1 || p.y === H - 1; }

  /** 4c49:02f9: the points along edge `i`, each a step on from the last
   *  towards the end and pushed about by up to half a step. */
  coastPoints(i) {
    const base = EDGE_PTS + 0x50 * i, want = this.r16(P_EDGE_POINTS + 2 * i);
    const s = this.pt(EDGE_START + 4 * i), e = this.pt(EDGE_END + 4 * i);
    this.setPt(base, s);
    let n = 1;
    if (want < 3 || (this.onEdge(s) && this.onEdge(e))) {
      this.setPt(base + 4, e);
      this.w16(EDGE_COUNT + 2 * i, 2);
      return;
    }
    const len = idiv(reach(e.x, e.y, s.x, s.y), want - 1);
    let k = 0;
    for (;;) {
      const p = this.pt(base + 4 * k);
      const [dx, dy] = step(p.x, p.y, e.x, e.y);
      const q = { x: p.x + dx * len, y: p.y + dy * len };
      this.jitter(q, idiv(len, 2), false);
      k++;
      this.put(q.x, q.y, PLAIN);
      this.setPt(base + 4 * k, q);
      n++;
      if (reach(e.x, e.y, q.x, q.y) < len || n >= want - 1) break;
    }
    this.setPt(base + 4 * (k + 1), e);
    this.w16(EDGE_COUNT + 2 * i, n + 1);
  }

  /** 4c49:104a: turn a step by -1 to +3 eighths. */
  turn(d) {
    const k = this.nbIndex(d[0], d[1]);
    const i = this.dice(1, 5, (k < 0 ? 8 : k) - 2) % 8;
    return [this.nx(i), this.ny(i)];
  }

  /** 4c49:0a0e: a wandering line of plain from a to b. */
  coastLine(a, b) {
    let cur = { x: a.x, y: a.y };
    let first = step(a.x, a.y, b.x, b.y);
    for (let n = 0; n < SAFETY; n++) {
      let d = step(cur.x, cur.y, b.x, b.y);
      const r = reach(cur.x, cur.y, b.x, b.y) < 3 ? 1 : this.dice(1, 3, 0);
      if (r === 2) d = first;
      else if (r === 3) { d = this.turn(d); first = d; }
      cur.x += d[0]; cur.y += d[1];
      clamp(cur);
      this.put(cur.x, cur.y, PLAIN);
      if (cur.x === b.x && cur.y === b.y) return;
    }
  }

  /** 4c49:1016: which edge a point lies on, -1 for none. */
  edgeOf(p) {
    if (p.x === 0) return 3;
    if (p.y === 0) return 0;
    if (p.x === W - 1) return 1;
    if (p.y === H - 1) return 2;
    return -1;
  }

  /** A straight run of plain from a to b, one step each. */
  straight(a, b) {
    const [dx, dy] = step(a.x, a.y, b.x, b.y);
    const cur = { x: a.x, y: a.y };
    for (let n = 0; n < SAFETY && !(cur.x === b.x && cur.y === b.y); n++) {
      cur.x += dx; cur.y += dy;
      clamp(cur);
      this.put(cur.x, cur.y, PLAIN);
    }
  }

  /** 4c49:0b0e: two points on the map's edge are joined along it, round a
   *  corner if need be; across the map they wander as any other. */
  coastAlongEdge(a, b) {
    const ea = this.edgeOf(a), eb = this.edgeOf(b);
    if (ea === eb) return this.straight(a, b);
    const apart = abs(ea - eb);
    if (apart === 2) return this.coastLine(a, b);
    const corner = this.pt(CORNERS + 4 * (apart === 3 ? 0 : Math.max(ea, eb)));
    this.straight(a, corner);
    this.straight(b, corner);
  }

  /** 4c49:05c7: join every point of the outline to the next. */
  coastJoin() {
    for (let i = 0; i < 4; i++) {
      const base = EDGE_PTS + 0x50 * i, n = this.r16(EDGE_COUNT + 2 * i);
      let k = 0;
      for (; k < n - 1; k++) this.coastSegment(this.pt(base + 4 * k), this.pt(base + 4 * (k + 1)));
      const j = (i + 1) % 4;
      this.coastSegment(this.pt(base + 4 * k), this.pt(EDGE_PTS + 0x50 * j));
    }
  }

  coastSegment(a, b) {
    if (this.onEdge(a) && this.onEdge(b)) this.coastAlongEdge(a, b);
    else this.coastLine(a, b);
  }

  /** 4c49:10bd: may the sea flood into this tile? */
  floods(x, y) {
    const t = this.at(x, y);
    if (t === SHORE || t === BRIDGE) return false;
    const inside = x !== 0 && y !== 0 && x !== W - 1 && y !== H - 1;
    if (!inside && t === 0) return true;
    return this.at(x + 1, y) === WATER || this.at(x - 1, y) === WATER
      || this.at(x, y + 1) === WATER || this.at(x, y - 1) === WATER;
  }

  /** 4c49:0867: the sea floods in from the edges up to the outline; what it
   *  does not reach is land. */
  coastFill() {
    for (let i = 0; i < W * H; i++) this.m[GRID + i] = this.m[GRID + i] === PLAIN ? SHORE : 0;
    const flood = (x, y) => { if (this.floods(x, y)) this.put(x, y, WATER); };
    for (let pass = 0; pass < 2; pass++) {
      for (let x = 0; x < W; x++) for (let y = 0; y < H; y++) flood(x, y);
      for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) flood(x, y);
      for (let x = W - 1; x >= 0; x--) for (let y = H - 1; y >= 0; y--) flood(x, y);
      for (let y = H - 1; y >= 0; y--) for (let x = W - 1; x >= 0; x--) flood(x, y);
    }
    for (let i = 0; i < W * H; i++) this.m[GRID + i] = this.m[GRID + i] === WATER ? WATER : PLAIN;
  }

  /** 4c49:0ce7: water beside land is shore. */
  shores() {
    for (let x = 0; x < W; x++) {
      for (let y = 0; y < H; y++) {
        if (this.at(x, y) !== WATER) continue;
        if (this.nearPlain(x, y)) this.put(x, y, SHORE);
        if (this.near(x, y, HILLS)) this.put(x, y, SHORE);
        if (this.near(x, y, MOUNTAINS)) this.put(x, y, SHORE);
      }
    }
  }

  /** 4c49:0000: the land. */
  coast() {
    for (let i = 0; i < 4; i++) this.coastEnds(i);
    this.coastOrder();
    for (let i = 0; i < 4; i++) this.coastPoints(i);
    this.coastJoin();
    this.coastFill();
    this.shores();
    if (this.dice(1, 100, 0) < 50) {
      // 4c49:0daa: 0-2 channels straight across, west to east
      const n = this.dice(1, 3, -1);
      for (let i = 0; i < n; i++) this.river(true, false, true);
      this.waterTidy();
    }
    this.shores();
  }

  // --- 4d71: mountains and hills -----------------------------------------

  node(i) { return NODES + 14 * i; }

  /** 4d71:0495 / 03af: seeds on random plain tiles, each a node. */
  seeds(n, t, kind) {
    for (let k = 0; k < n; k++) {
      let x = 0, y = 0;
      for (let tries = 0; tries < SAFETY; tries++) {
        x = this.dice(1, W, -1); y = this.dice(1, H, -1);
        if (this.at(x, y) === PLAIN) break;
      }
      this.put(x, y, t);
      const c = this.r16(NODE_COUNT), o = this.node(c);
      this.w16(o, x); this.w16(o + 2, y); this.w16(o + 4, kind); this.w16(o + 6, 0);
      this.w16(NODE_COUNT, c + 1);
    }
  }

  /** 4d71:057b: each node is linked to its nearest others -- a hill to 0-1,
   *  a mountain to 0-3. */
  link() {
    const count = this.r16(NODE_COUNT);
    if (count < 2) return;
    for (let i = 0; i < count; i++) {
      const o = this.node(i);
      let n = this.r16(o + 4) === 1 ? this.dice(1, 2, -1) : this.dice(1, 4, -1);
      if (n >= count - 1) n = count - 1;
      this.w16(o + 6, n);
      for (let l = 0; l < n; l++) {
        let best = 10000, bestJ = -1;
        for (let j = 0; j < count; j++) {
          if (j === i) continue;
          let k = 0;
          while (k < l && this.r16(o + 8 + 2 * k) !== j) k++;
          if (k !== l) continue;
          const p = this.node(j);
          const d = reach(this.r16(p), this.r16(p + 2), this.r16(o), this.r16(o + 2));
          if (d < best) { best = d; bestJ = j; }
        }
        this.w16(o + 8 + 2 * l, bestJ);
      }
    }
  }

  /** 4d71:09a7: a ridge from a to b, wandering as the coast does; `width` 2
   *  adds a tile to either side, 3 one two out. Only plain and hills give
   *  way to it. */
  ridge(a, b, width, t) {
    let cur = { x: a.x, y: a.y };
    let first = step(a.x, a.y, b.x, b.y);
    let k = 0;
    const lay = (x, y) => {
      const v = this.at(x, y);
      if (v === PLAIN || v === HILLS) this.put(x, y, t);
    };
    for (let n = 0; n < SAFETY; n++) {
      let d = step(cur.x, cur.y, b.x, b.y);
      const r = reach(cur.x, cur.y, b.x, b.y) < 3 ? 1 : this.dice(1, 3, 0);
      if (r === 2) d = first;
      else if (r === 3) { d = this.turn(d); first = d; }
      cur.x += d[0]; cur.y += d[1];
      const ki = this.nbIndex(d[0], d[1]);
      if (ki >= 0) k = ki;
      const l = (k + 2) % 8, rr = (k + 6) % 8;
      const p1 = { x: cur.x + this.nx(l), y: cur.y + this.ny(l) };
      const p2 = { x: cur.x + this.nx(rr), y: cur.y + this.ny(rr) };
      const p3 = { x: cur.x + 2 * this.nx(l), y: cur.y + 2 * this.ny(l) };
      const p4 = { x: cur.x + 2 * this.nx(rr), y: cur.y + 2 * this.ny(rr) };
      clamp(cur);
      lay(cur.x, cur.y);
      const done = cur.x === b.x && cur.y === b.y;
      if (width === 2) {
        clamp(p1);
        lay(p1.x, p1.y);
        lay(p2.x, p2.y);             // the original clamps only the one
      }
      if (width === 3) {
        clamp(p3);
        lay(p3.x, p3.y);
        lay(p4.x, p4.y);
      }
      if (done) return;
    }
  }

  /** 4d71:0884 / 08f3: a node with no links gets a short ridge of its own,
   *  up and to the left. */
  blob(p, t) {
    const q = { x: p.x + this.dice(1, 5, -10), y: p.y + this.dice(1, 5, -10) };
    clamp(q);
    this.ridge(p, q, this.dice(1, 2, 1), t);
  }

  /** 4d71:06d2: the ridges along the links. */
  ridges() {
    const count = this.r16(NODE_COUNT);
    if (count < 2) return;
    for (let i = 0; i < count; i++) {
      const o = this.node(i), p = { x: this.r16(o), y: this.r16(o + 2) };
      const hill = this.r16(o + 4) === 1, links = this.r16(o + 6);
      if (links === 0) { this.blob(p, hill ? HILLS : MOUNTAINS); continue; }
      for (let l = 0; l < links; l++) {
        const q = this.node(this.r16(o + 8 + 2 * l));
        const to = { x: this.r16(q), y: this.r16(q + 2) };
        if (hill && this.r16(q + 4) === 1) this.ridge(p, to, 3, HILLS);
        else this.ridge(p, to, this.dice(1, 2, 1), MOUNTAINS);
      }
    }
  }

  /** 4d71:002e: plain hemmed in by mountains (or hills) on all four sides
   *  joins them, mountains on the shore come down, and a diagonal run of
   *  three mountains is thickened on one side. */
  mountainTidy() {
    for (let x = 0; x < W; x++) {
      for (let y = 0; y < H; y++) {
        if (x !== 0 && y !== 0 && x !== W - 1 && y !== H - 1 && this.at(x, y) === PLAIN) {
          for (const t of [MOUNTAINS, HILLS]) {
            if (this.at(x, y + 1) === t && this.at(x + 1, y) === t
                && this.at(x - 1, y) === t && this.at(x, y - 1) === t) this.put(x, y, t);
          }
        }
        if (this.at(x, y) === MOUNTAINS && this.near(x, y, SHORE)) this.put(x, y, PLAIN);
      }
    }
    for (let x = 1; x < W - 1; x++) {
      for (let y = 1; y < H - 1; y++) {
        if (this.at(x, y) !== MOUNTAINS) continue;
        const m = (dx, dy) => this.at(x + dx, y + dy) === MOUNTAINS;
        if (m(1, 1) && m(-1, -1) && !m(1, -1) && !m(-1, 1)) {
          if (this.dice(1, 10, 0) > 5) this.put(x - 1, y + 1, MOUNTAINS);
          else this.put(x + 1, y - 1, MOUNTAINS);
        } else if (m(-1, 1) && m(1, -1) && !m(-1, -1) && !m(1, 1)) {
          if (this.dice(1, 10, 0) > 5) this.put(x + 1, y + 1, MOUNTAINS);
          else this.put(x - 1, y - 1, MOUNTAINS);
        }
      }
    }
  }

  /** 4d71:033a: mountains are ringed with hills. */
  foothills() {
    for (let x = 0; x < W; x++) {
      for (let y = 0; y < H; y++) {
        const t = this.at(x, y);
        if (t !== MOUNTAINS && t !== HILLS && this.near(x, y, MOUNTAINS)) this.put(x, y, HILLS);
      }
    }
  }

  /** 4d71:0000. */
  highlands() {
    this.w16(NODE_COUNT, 0);
    this.seeds(this.r16(P_MOUNTAINS), MOUNTAINS, 2);
    this.seeds(this.r16(P_HILLS), HILLS, 1);
    this.link();
    this.ridges();
    this.mountainTidy();
    this.foothills();
  }

  // --- 4f5f: erosion and passes --------------------------------------------

  /** 4f5f:0044: a walk between two random tiles; erosion wears its
   *  mountains to hills, a pass cuts mountains and hills to plain, five tiles
   *  across. */
  wear(pass) {
    const a = { x: this.dice(1, W, -1), y: this.dice(1, H, -1) };
    const b = { x: this.dice(1, W, -1), y: this.dice(1, H, -1) };
    const cur = { x: a.x, y: a.y };
    while (!(cur.x === b.x && cur.y === b.y)) {
      const [dx, dy] = step(cur.x, cur.y, b.x, b.y);
      const k = this.nbIndex(dx, dy);
      const l = (k + 1) % 8, r = (k + 7) % 8;
      const t = this.at(cur.x, cur.y);
      const hit = pass ? t === MOUNTAINS || t === HILLS : t === MOUNTAINS;
      if (hit) {
        const to = pass ? PLAIN : HILLS;
        for (const [ox, oy] of [[0, 0], [this.nx(l), this.ny(l)], [this.nx(r), this.ny(r)],
          [2 * this.nx(l), 2 * this.ny(l)], [2 * this.nx(r), 2 * this.ny(r)]]) {
          const p = { x: cur.x + ox, y: cur.y + oy };
          clamp(p);
          this.put(p.x, p.y, to);
        }
      }
      cur.x += dx; cur.y += dy;
    }
  }

  /** Hills with three mountains on their four sides become mountain
   *  (4f5f:04d6); plain with three hills, hills (05ae). */
  surrounded(t, by) {
    for (let x = 1; x < W - 1; x++) {
      for (let y = 1; y < H - 1; y++) {
        if (this.at(x, y) !== t) continue;
        const n = (this.at(x, y + 1) === by) + (this.at(x, y - 1) === by)
          + (this.at(x + 1, y) === by) + (this.at(x - 1, y) === by);
        if (n > 2) this.put(x, y, by);
      }
    }
  }

  /** 4f5f:0000. */
  erosion() {
    for (let i = 0; i < this.r16(P_EROSION); i++) this.wear(false);
    for (let i = 0; i < this.r16(P_PASSES); i++) this.wear(true);
    this.surrounded(HILLS, MOUNTAINS);
    this.surrounded(PLAIN, HILLS);
    this.foothills();
  }

  // --- 4eb7: rivers ----------------------------------------------------------

  /** 4eb7:005d: a river. Without `channel` it runs from a random hill or
   *  mountain to a random water or shore tile, 200 tries each; a channel
   *  runs from the west edge to the east. Each step lays water three tiles
   *  across (`wide`: five, a channel seven), turns aside a quarter of the
   *  time each way, and with `stops` the river ends on meeting water the
   *  second time. */
  river(wide, stops, channel) {
    let a, b;
    if (channel) {
      a = { x: 0, y: this.dice(1, H, -1) };
      b = { x: W - 1, y: this.dice(1, H, -1) };
    } else {
      a = { x: 0, y: 0 }; b = { x: 0, y: 0 };
      for (let i = 0; i < 200; i++) {
        a = { x: this.dice(1, W, -1), y: this.dice(1, H, -1) };
        const t = this.at(a.x, a.y);
        if (t === HILLS || t === MOUNTAINS) break;
      }
      for (let i = 0; i < 200; i++) {
        b = { x: this.dice(1, W, -1), y: this.dice(1, H, -1) };
        const t = this.at(b.x, b.y);
        if (t === WATER || t === SHORE) break;
      }
    }
    const cur = { x: a.x, y: a.y };
    let met = 0;
    let done = cur.x === b.x && cur.y === b.y;
    for (let n = 0; !done && n < SAFETY; n++) {
      const [dx, dy] = step(cur.x, cur.y, b.x, b.y);
      const k = this.nbIndex(dx, dy);
      const l = (k + 1) % 8, r = (k + 7) % 8, aside1 = (k + 2) % 8, aside2 = (k + 6) % 8;
      if (stops) {
        // nb(-1) reads the word before the table, as the original's did
        const ahead = this.at(cur.x + this.nx(k), cur.y + this.ny(k));
        if (ahead === WATER && met === 1) done = true;
        if (ahead === WATER && met === 0) met++;
      }
      const lay = (ox, oy) => {
        const p = { x: cur.x + ox, y: cur.y + oy };
        clamp(p);
        this.put(p.x, p.y, WATER);
      };
      lay(0, 0);
      lay(this.nx(l), this.ny(l));
      lay(this.nx(r), this.ny(r));
      if (wide) {
        lay(2 * this.nx(l), 2 * this.ny(l));
        lay(2 * this.nx(r), 2 * this.ny(r));
        if (channel) {
          lay(3 * this.nx(l), 3 * this.ny(l));
          lay(3 * this.nx(r), 3 * this.ny(r));
        }
      }
      cur.x += dx; cur.y += dy;
      if (cur.x === b.x && cur.y === b.y) done = true;
      const roll = this.dice(1, 4, 0);
      if (roll === 2) { cur.x += this.nx(aside1); cur.y += this.ny(aside1); }
      else if (roll === 3) { cur.x += this.nx(aside2); cur.y += this.ny(aside2); }
    }
  }

  /** 4eb7:060c: land with water on three of its four sides goes under, shore
   *  out of sight of land becomes open water, and a diagonal of water is
   *  given a shore tile beside it. */
  waterTidy() {
    const wet = (t) => t === SHORE || t === WATER;
    const land = (t) => t === PLAIN || t === HILLS || t === MOUNTAINS;
    for (let x = 0; x < W; x++) {
      for (let y = 0; y < H; y++) {
        if (!land(this.at(x, y))) continue;
        const n = wet(this.at(x, y + 1)) + wet(this.at(x, y - 1))
          + wet(this.at(x + 1, y)) + wet(this.at(x - 1, y));
        if (n > 2) this.put(x, y, WATER);
      }
    }
    for (let x = 0; x < W; x++) {
      for (let y = 0; y < H; y++) {
        if (this.at(x, y) === SHORE && !this.near(x, y, PLAIN) && !this.near(x, y, HILLS)
            && !this.near(x, y, MOUNTAINS)) this.put(x, y, WATER);
      }
    }
    const dry = (t) => t === HILLS || t === FOREST || t === PLAIN;
    for (let x = 1; x < W - 1; x++) {
      for (let y = 1; y < H - 1; y++) {
        if (!wet(this.at(x, y))) continue;
        const at = (dx, dy) => this.at(x + dx, y + dy);
        if (wet(at(1, 1)) && wet(at(-1, -1)) && dry(at(1, -1)) && dry(at(-1, 1))) {
          if (this.dice(1, 10, 0) > 4) this.put(x - 1, y + 1, SHORE);
          else this.put(x + 1, y - 1, SHORE);
        } else if (wet(at(-1, 1)) && wet(at(1, -1)) && dry(at(-1, -1)) && dry(at(1, 1))) {
          if (this.dice(1, 10, 0) > 4) this.put(x + 1, y + 1, SHORE);
          else this.put(x - 1, y - 1, SHORE);
        }
      }
    }
  }

  /** 4eb7:0000. */
  rivers() {
    for (let i = 0; i < this.r16(P_RIVERS); i++) this.river(false, true, false);
    for (let i = 0; i < this.r16(P_WIDE_RIVERS); i++) this.river(true, true, false);
    this.waterTidy();
    this.shores();
    this.mountainTidy();
    this.foothills();
    this.waterTidy();
    this.shores();
  }

  // --- 4e47: forests --------------------------------------------------------

  /** 4e47:00b5: a wood round a random plain tile: eight arms, and off each
   *  step of an arm two side shoots that shorten as it goes. */
  wood() {
    let x = 0, y = 0;
    for (let tries = 0; tries < SAFETY; tries++) {
      x = this.dice(1, W, -1); y = this.dice(1, H, -1);
      if (this.at(x, y) === PLAIN) break;
    }
    const arms = [];
    let span, base;
    if (this.dice(1, 100, 0) < 65) {
      for (let i = 0; i < 8; i++) arms[i] = this.dice(1, 8, 2);
      span = 4; base = 2;
    } else {
      for (let i = 0; i < 8; i++) arms[i] = this.dice(1, 10, 5);
      span = 6; base = 4;
    }
    const grow = (p) => {
      clamp(p);
      if (this.at(p.x, p.y) === PLAIN) {
        this.put(p.x, p.y, FOREST);
        this.w16(FOREST_DONE, this.r16(FOREST_DONE) + 1);
      }
    };
    this.put(x, y, FOREST);
    this.w16(FOREST_DONE, this.r16(FOREST_DONE) + 1);
    for (let i = 0; i < 8; i++) {
      const cur = { x, y };
      let k = 0;
      for (let s = 0; s < arms[i]; s++) {
        cur.x += this.nx(i); cur.y += this.ny(i);
        grow(cur);
        const l = (i + 2) % 8, r = (i + 6) % 8;
        k++;
        const nl = this.dice(1, span - idiv(k, 2), base - idiv(k, 4));
        const nr = this.dice(1, span - idiv(k, 2), base - idiv(k, 3));
        const p = { x: cur.x, y: cur.y };
        for (let j = 0; j < nl; j++) { p.x += this.nx(l); p.y += this.ny(l); grow(p); }
        const q = { x: cur.x, y: cur.y };
        for (let j = 0; j < nr; j++) { q.x += this.nx(r); q.y += this.ny(r); grow(q); }
      }
    }
  }

  /** 4e47:03d9: plain with forest on three sides is forest, and a diagonal
   *  of forest is given a tile beside it. */
  forestTidy() {
    for (let x = 0; x < W; x++) {
      for (let y = 0; y < H; y++) {
        if (this.at(x, y) !== PLAIN) continue;
        const n = (this.at(x, y + 1) === FOREST) + (this.at(x, y - 1) === FOREST)
          + (this.at(x + 1, y) === FOREST) + (this.at(x - 1, y) === FOREST);
        if (n > 2) {
          this.put(x, y, FOREST);
          this.w16(FOREST_DONE, this.r16(FOREST_DONE) + 1);
        }
      }
    }
    const open = (t) => t === HILLS || t === SHORE || t === PLAIN;
    for (let x = 1; x < W - 1; x++) {
      for (let y = 1; y < H - 1; y++) {
        if (this.at(x, y) !== FOREST) continue;
        const at = (dx, dy) => this.at(x + dx, y + dy);
        if (at(1, 1) === FOREST && at(-1, -1) === FOREST && open(at(1, -1)) && open(at(-1, 1))) {
          if (this.dice(1, 10, 0) < 5) this.put(x + 1, y - 1, FOREST);
          else this.put(x - 1, y + 1, FOREST);
        } else if (at(-1, 1) === FOREST && at(1, -1) === FOREST && open(at(-1, -1)) && open(at(1, 1))) {
          if (this.dice(1, 10, 0) < 5) this.put(x - 1, y - 1, FOREST);
          else this.put(x + 1, y + 1, FOREST);
        }
      }
    }
  }

  /** 4e47:0000: woods until they cover land / 100 x the Forest parameter. */
  forests() {
    let land = 0;
    for (let i = 0; i < W * H; i++) {
      const t = this.m[GRID + i];
      if (t === PLAIN || t === HILLS || t === MOUNTAINS) land++;
    }
    this.w16(FOREST_WANTED, idiv(land, 100) * this.r16(P_FOREST));
    this.w16(FOREST_DONE, 0);
    for (let n = 0; n < 10000 && this.r16(FOREST_DONE) < this.r16(FOREST_WANTED); n++) {
      this.wood();
      this.forestTidy();
    }
  }

  // --- 4fc9: marshes ----------------------------------------------------------

  /** 4fc9:0029: 1d5+3 tiles of marsh scattered round (x, y). */
  marshAround(x, y) {
    const n = this.dice(1, 5, 3);
    for (let i = 0; i < n; i++) {
      const p = { x: x + this.dice(1, 3, 0) * this.nx(this.dice(1, 8, -1)), y: 0 };
      p.y = y + this.dice(1, 3, 0) * this.ny(this.dice(1, 8, -1));
      clamp(p);
      if (this.at(p.x, p.y) === PLAIN) this.put(p.x, p.y, MARSH);
    }
  }

  /** 4fc9:010a: a marsh, near the shore if one of four tries finds it. */
  marsh() {
    let x = 0, y = 0, tries = 0;
    for (let n = 0; n < SAFETY; n++) {
      x = this.dice(1, 102, 5); y = this.dice(1, 146, 5);
      if (this.at(x, y) !== PLAIN) continue;
      if (tries >= 4) break;
      tries++;
      if (this.near(x, y, SHORE) || this.near(x + 1, y, SHORE)
          || this.near(x, y + 1, SHORE) || this.near(x + 1, y + 1, SHORE)) break;
    }
    this.put(x, y, MARSH);
    this.marshAround(x, y);
    this.marshAround(x + 1, y + 1);
    this.marshAround(x - 1, y - 1);
    this.marshAround(x - 1, y + 1);
    this.marshAround(x + 1, y - 1);
  }

  /** 4f5f:06a0: 1d3 marshes. */
  marshes() {
    const n = this.dice(1, 3, 0);
    for (let i = 0; i < n; i++) this.marsh();
  }

  // --- 513d: cities and sites ---------------------------------------------

  /** 513d:0331: a city's 2x2 footprint, clear of water, hills and other
   *  cities by a tile; on the coast if one of four tries finds it. */
  placeCity() {
    let x = 0, y = 0, tries = 0;
    for (let n = 0; n < SAFETY; n++) {
      x = this.dice(1, 102, 5); y = this.dice(1, 146, 5);
      let ok = true;
      for (const [dx, dy] of FOOTPRINT) {
        const t = this.at(x + dx, y + dy);
        if (t === SHORE || t === WATER || t === CITY || t === MOUNTAINS || t === HILLS) ok = false;
        if (this.near(x + dx, y + dy, CITY) || this.near(x + dx + 1, y + dy + 1, CITY)
            || this.near(x + dx - 1, y + dy - 1, CITY)) ok = false;
      }
      if (!ok) continue;
      if (tries >= 4) break;
      tries++;
      if (this.near(x, y, SHORE) || this.near(x + 1, y, SHORE)
          || this.near(x, y + 1, SHORE) || this.near(x + 1, y + 1, SHORE)) break;
    }
    const c = this.cityCount;
    this.ws16(this.crec(c), x);
    this.ws16(this.crec(c) + 2, y);
    this.cityCount = c + 1;
    for (const [dx, dy] of FOOTPRINT) this.put(x + dx, y + dy, CITY);
  }

  /** 513d:0000. */
  cities() {
    this.cityCount = 0;
    for (let i = this.r16(0xa84); i < this.r16(P_CITIES); i++) this.placeCity();
  }

  /** 513d:0a77: somewhere for a site. The map is taken a sixteenth at a time,
   *  round in turn: 50 tries there for plain clear of cities and of other
   *  sites by three, then 50 anywhere, then 50 for anything not a city. */
  siteSpot() {
    if (this.siteCell === -1) this.siteCell = this.dice(1, 16, -1);
    const cx = this.siteCell % 4, cy = idiv(this.siteCell, 4);
    let x = 0, y = 0, found = false;
    for (let i = 0; i < 50 && !found; i++) {
      x = this.dice(1, 20, idiv(cx * W, 4) + 3);
      y = this.dice(1, 31, idiv(cy * H, 4) + 3);
      found = this.at(x, y) === PLAIN && !this.near(x, y, CITY) && !this.near(x, y, SITE)
        && !this.near(x - 3, y, SITE) && !this.near(x + 3, y, SITE)
        && !this.near(x, y - 3, SITE) && !this.near(x, y + 3, SITE);
    }
    for (let i = 0; i < 50 && !found; i++) {
      x = this.dice(1, 102, 5); y = this.dice(1, 146, 5);
      found = this.at(x, y) === PLAIN && !this.near(x, y, CITY) && !this.near(x, y, SITE);
    }
    if (!found) {
      x = this.dice(1, 102, 5); y = this.dice(1, 146, 5);
      for (let i = 0; i < 50 && !found; i++) {
        x = this.dice(1, 102, 5); y = this.dice(1, 146, 5);
        found = this.at(x, y) !== CITY && this.at(x, y) !== SITE;
      }
    }
    this.siteCell = (this.siteCell + 1) % 16;
    return [x, y];
  }

  /** 513d:003a: forty sites, keeping the template's temples and ruins, with
   *  names and descriptions from RANDOM.DAT's word lists. */
  *sites() {
    this.ws16(S_SITE_COUNT, N_SITES);
    for (let i = 0; i < N_SITES; i++) {
      this.ws16(S_SITES + SITE_STRIDE * i + 2, -1);
      this.ws16(S_SITES + SITE_STRIDE * i, -1);
    }
    let tenth = 0;
    for (let i = 0; i < N_SITES; i++) {
      const now = idiv(i * 10, N_SITES);
      if (now !== tenth) { tenth = now; yield 70 + now; }
      const [x, y] = this.siteSpot();
      const o = S_SITES + SITE_STRIDE * i;
      this.ws16(o, x);
      this.ws16(o + 2, y);
      this.put(x, y, SITE);
      let name, text;
      if (this.scn[o + 24] === 1) {
        name = fmt("%s Temple", this.str(0x1000 + 10 * this.dice(1, 10, -1), 10));
        text = "#%03d|The %s can bless your|armies or give you|quests|\n";
      } else {
        const f = this.dice(1, this.r16(SITE_FORMATS), -1);
        const end = this.str(0xf9c + 10 * this.dice(1, 10, -1), 10);
        const word = this.str(0xf38 + 10 * this.dice(1, 10, -1), 10) + end;
        name = fmt(this.str(SITE_FORMATS + 2 + 20 * f, 20), word);
        text = "#%03d|%s is|inhabited by monsters and|full of treasure!|\n";
      }
      this.wstr(o + 4, name);
      this.spc += fmt(text, i, name);
    }
  }

  // --- 4fef: terrain to tiles -----------------------------------------------

  /** 4fef:005c: every terrain type its first tile, a city its castle and the
   *  first eight sites their own. */
  toTiles() {
    for (let y = 0; y < H; y++) {
      for (let x = 0; x < W; x++) {
        const t = this.at(x, y);
        if (t === CITY && this.at(x, y - 1) !== CITY && this.at(x - 1, y) !== CITY && x > 0 && y > 0) {
          const c = this.r16(TILES + 64 * CITY) & 0xff;
          this.setTile(x, y, c); this.setTile(x + 1, y, c + 1);
          this.setTile(x, y + 1, c + 16); this.setTile(x + 1, y + 1, c + 17);
        } else if (t === SITE) {
          this.setTile(x, y, this.tileFor(SITE, 0, x, y));
        } else if (t !== CITY) {
          this.setTile(x, y, this.tileFor(t, 0, x, y));
        }
      }
    }
    for (let i = 0; i < 8; i++) {
      const o = S_SITES + SITE_STRIDE * i;
      this.setTile(this.s16(o), this.s16(o + 2), this.r16(TILES + 64 * SITE + 4) & 0xff);
    }
  }

  /** The 8 neighbours as a mask (bit i for neighbour i), counting the map's
   *  edge in: 4fef:0c9d (water, shore, bridge), 0d4f (forest), 0dcb
   *  (mountains), 0e47 (hills or mountains). */
  mask(x, y, test) {
    let m = 0;
    for (let i = 0; i < 8; i++) {
      const ax = x + this.nx(i), ay = y + this.ny(i);
      const off = ax < 0 || ay < 0 || ax >= W || ay >= H;
      if (off || test(this.at(ax, ay))) m |= 1 << i;
    }
    return m;
  }

  shape(m) { return i8(this.m[SHAPE + m]); }

  /** 4fef:0464: the tiles that join each kind of ground to its neighbours. A
   *  shape the tile set has no tile for gives way: water to marsh, forest
   *  and hills often to plain, mountains to hills. */
  shapes() {
    const each = (t, f) => {
      for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) if (this.at(x, y) === t) f(x, y);
    };
    for (let y = 0; y < H; y++) {
      for (let x = 0; x < W; x++) {
        const t = this.at(x, y);
        if (t !== WATER && t !== SHORE) continue;
        const v = this.shape(this.mask(x, y, (n) => n === WATER || n === SHORE || n === BRIDGE));
        if (v < 0) {
          this.put(x, y, MARSH);
          this.setTile(x, y, this.tileFor(MARSH, 0, x, y));
          this.marshAround(x, y);
          this.tiles[y * W + x] &= 0x7fff;
        } else {
          this.put(x, y, WATER);
          this.setTile(x, y, this.tileFor(WATER, v, x, y));
        }
      }
    }
    each(FOREST, (x, y) => {
      const v = this.shape(this.mask(x, y, (n) => n === FOREST));
      if (v >= 0) {
        this.setTile(x, y, this.tileFor(FOREST, v, x, y));
        this.put(x, y, FOREST);
      } else if (this.dice(1, 10, 0) < 6) {
        this.setTile(x, y, this.tileFor(PLAIN, 0, x, y));
        this.put(x, y, PLAIN);
      } else {
        this.setTile(x, y, this.tileFor(FOREST, 13, x, y));
      }
    });
    each(MOUNTAINS, (x, y) => {
      const v = this.shape(this.mask(x, y, (n) => n === MOUNTAINS));
      if (v >= 0) {
        this.setTile(x, y, this.tileFor(MOUNTAINS, v, x, y));
        this.put(x, y, MOUNTAINS);
      } else {
        this.setTile(x, y, this.tileFor(HILLS, 0, x, y));
        this.put(x, y, HILLS);
      }
    });
    each(HILLS, (x, y) => {
      const v = this.shape(this.mask(x, y, (n) => n === MOUNTAINS || n === HILLS));
      if (v >= 0) {
        this.setTile(x, y, this.tileFor(HILLS, v, x, y));
        this.put(x, y, HILLS);
      } else if (this.dice(1, 10, 0) < 6) {
        this.setTile(x, y, this.tileFor(PLAIN, 0, x, y));
        this.put(x, y, PLAIN);
      } else {
        this.put(x, y, HILLS);
        this.setTile(x, y, this.tileFor(HILLS, 13, x, y));
      }
    });
    each(MARSH, (x, y) => {
      // mostly the plain marsh; three in ten one of four others
      const o = this.dice(1, 10, 0) < 7 ? 2 * parity(x, y) : 4 * this.dice(1, 4, 0);
      this.setTile(x, y, this.r16(TILES + 64 * MARSH + o) & 0xff);
      this.put(x, y, MARSH);
    });
  }

  // --- 5311: bridges, crossings and roads -----------------------------------

  /** 5311:0a7f: ground a bridge may land on. */
  firm(x, y) {
    const t = this.at(x, y);
    return t === PLAIN || t === FOREST || t === HILLS;
  }

  /** The nearest city, by map distance, to each listed spot (shared by
   *  5311:03c3 and 08f6; a spot's figure is worked out once). */
  nearestCities(list, n) {
    if (list.cityDist[0] !== -1) return;
    for (let i = n - 1; i >= 0; i--) {
      if (list.x[i] === -1) continue;
      let best = 10000;
      for (let c = this.cityCount - 1; c >= 0; c--) {
        best = Math.min(best, distance(list.x[i], list.y[i], this.cx(c), this.cy(c)));
      }
      list.cityDist[i] = best;
    }
  }

  /** 5311:08f6 (bridges) and 03c3 (crossings): the best spot still free --
   *  a die roll, plus more the nearer a city is. Bridges keep 10 apart. */
  pickSpot(list, n, apart) {
    this.nearestCities(list, n);
    for (let i = n - 1; i >= 0; i--) {
      if (list.chosen[i] !== 0 || list.x[i] === -1) continue;
      let best = 10000;
      for (let j = n - 1; j >= 0; j--) {
        if (list.chosen[j] !== 0) best = Math.min(best, distance(list.x[i], list.y[i], list.x[j], list.y[j]));
      }
      list.chosenDist[i] = best;
    }
    let bestScore = -1, pick = -1;
    for (let i = n - 1; i >= 0; i--) {
      if (list.chosen[i] !== 0 || list.x[i] === -1) continue;
      if (apart && !(list.chosenDist[i] > 9)) continue;
      const roll = this.dice(1, 15, 1);
      const near = list.cityDist[i] < 31 ? 30 - list.cityDist[i] : 0;
      if (bestScore < near + roll) { bestScore = near + roll; pick = i; }
    }
    return pick;
  }

  /** 5311:05b4: bridges over two-tile rivers. As the original has it, only
   *  a river running north and south (tiles 0x26 and 0x28) is ever bridged. */
  bridges(list) {
    let n = 0;
    for (let y = 1; y < H - 2; y++) {
      for (let x = 1; x < W - 2; x++) {
        const t = this.tile(x, y);
        const across = (t === 0x26 || t === 0x16) && (this.tile(x + 1, y) === 0x28 || this.tile(x + 1, y) === 0x18);
        const along = (t === 0x21 || t === 0x11) && (this.tile(x, y + 1) === 0x24 || this.tile(x, y + 1) === 0x14);
        if (!across && !along) continue;
        const ew = t === 0x26 ? 1 : 0;
        const ok = ew && ((this.firm(x - 1, y) && this.firm(x + 2, y))
          || (this.firm(x, y - 1) && this.firm(x, y + 2)));
        if (ok && n < 50) {
          list.x[n] = x; list.y[n] = y; list.ew[n] = ew;
          n++;
        }
      }
    }
    if (n === 0) return;
    for (let i; (i = this.pickSpot(list, n, true)) !== -1;) {
      list.chosen[i] = 1;
      const x = list.x[i], y = list.y[i];
      if (list.ew[i] === 0) {
        this.setTile(x, y, 0x84); this.setTile(x, y + 1, 0x94);
        this.put(x, y + 1, BRIDGE); this.put(x, y, BRIDGE);
        this.setRoad(x, y + 2, 1); this.setRoad(x, y - 1, 1);
      } else {
        this.setTile(x, y, 0x85); this.setTile(x + 1, y, 0x86);
        this.put(x + 1, y, BRIDGE); this.put(x, y, BRIDGE);
        this.setRoad(x + 2, y, 1); this.setRoad(x - 1, y, 1);
      }
    }
  }

  /** 5311:01f6: up to ten crossings -- shore that looks out on three tiles of
   *  open water, at least 15 from the others. */
  crossings(list) {
    const seaward = { 0x20: [-1, 0], 0x23: [-1, 0], 0x21: [0, -1], 0x22: [1, 0], 0x25: [1, 0], 0x24: [0, 1] };
    for (let y = 1; y < H - 2; y++) {
      for (let x = 1; x < W - 2; x++) {
        if (this.dice(1, 2, -1) !== 0) continue;
        const d = seaward[this.tile(x, y)];
        if (!d) continue;
        let open = true;
        for (let k = 1; k <= 3; k++) if (this.at(x + k * d[0], y + k * d[1]) !== WATER) open = false;
        if (!open) continue;
        let near = 10000;
        for (let i = 49; i >= 0; i--) {
          if (list.x[i] !== -1) near = Math.min(near, distance(x, y, list.x[i], list.y[i]));
        }
        if (near <= 14) continue;
        const free = list.x.indexOf(-1);
        if (free !== -1) { list.x[free] = x; list.y[free] = y; }
      }
    }
    list.cityDist[0] = -1;
    for (let n = 0, i; n < 10 && (i = this.pickSpot(list, 50, false)) !== -1; n++) {
      list.chosen[i] = 1;
      this.tiles[list.y[i] * W + list.x[i]] |= 0x8000;
    }
  }

  /** 5311:0e22: a pair of cities to join -- a random one not yet served, and
   *  of the unserved 30 to 70 away, the one that rolls highest. */
  roadPair(served) {
    for (;;) {
      let a = -1;
      for (let n = 80; n > 0; n--) {
        a = this.dice(1, this.cityCount, -1);
        if (!served[a]) break;
        a = -1;
      }
      if (a === -1) return null;
      served[a] = 1;
      let b = -1, best = -1;
      for (let c = this.cityCount - 1; c >= 0; c--) {
        if (served[c]) continue;
        const d = distance(this.cx(a), this.cy(a), this.cx(c), this.cy(c));
        if (d > 29 && d < 71) {
          const roll = this.dice(1, 1000, 0);
          if (best < roll) { best = roll; b = c; }
        }
      }
      if (b !== -1) { served[b] = 1; return [a, b]; }
    }
  }

  /** 5311:0ad0: a tile round a city's footprint to start a road from. */
  roadEnd(c) {
    for (let n = 8; n > 0; n--) {
      const [dx, dy] = ROUND_CITY[this.dice(1, 12, -1)];
      const x = this.cx(c) + dx, y = this.cy(c) + dy;
      const t = this.at(x, y);
      if (t === PLAIN || t === FOREST || t === HILLS) return [x, y];
    }
    return null;
  }

  /** 5311:0c1c: lay a road along the path the game's own pathfinder finds,
   *  as pseudo-player 14 -- not over water, shore or bridges -- and every six
   *  steps count the cities within 10 as served. */
  layRoad(served, from, to) {
    const ground = {
      terrain: (x, y) => this.terrainOf(this.tile(x, y)),
      road: (x, y) => this.road(x, y) !== 0,
      crossing: (x, y) => (this.tiles[y * W + x] & 0x8000) !== 0,
    };
    const route = move.roadRoute(ground, ROAD_COST, W, H, from[0], from[1], to[0], to[1]);
    if (!route) return;
    let x = from[0], y = from[1], since = 0;
    for (let i = 0; i < route.length && i < 200; i++) {
      since++;
      if (x === to[0] && y === to[1]) break;
      x = route[i].x; y = route[i].y;
      const t = this.at(x, y);
      if (t !== SHORE && t !== WATER && t !== BRIDGE) this.setRoad(x, y, 1);
      if (since > 5) {
        since = 0;
        for (let c = this.cityCount - 1; c >= 0; c--) {
          if (distance(x, y, this.cx(c), this.cy(c)) < 10) served[c] = 1;
        }
      }
    }
  }

  /** 5311:0000: tiles, then bridges, crossings and roads. */
  *roads() {
    yield 81;
    this.toTiles();
    this.shapes();
    this.shapes();
    for (let i = 0; i < W * H; i++) { this.tiles[i] &= 0x7fff; this.rd[i] &= 0xe0; }
    yield 82;
    const list = { x: [], y: [], ew: [], cityDist: [], chosen: [], chosenDist: [] };
    for (let i = 0; i < 50; i++) {
      list.x[i] = list.y[i] = list.ew[i] = list.cityDist[i] = list.chosenDist[i] = -1;
      list.chosen[i] = 0;
    }
    this.bridges(list);
    for (let i = 49; i >= 0; i--) {
      if (list.chosen[i] === 0) list.x[i] = list.y[i] = list.ew[i] = list.cityDist[i] = list.chosenDist[i] = -1;
    }
    this.crossings(list);
    yield 83;
    const served = new Uint8Array(100);
    let shown = 4;
    for (let pair; (pair = this.roadPair(served));) {
      if (shown < 10 && this.dice(1, 3, -1) === 0) yield 80 + shown++;
      const a = this.roadEnd(pair[0]), b = a && this.roadEnd(pair[1]);
      if (a && b && !(a[0] === b[0] && a[1] === b[1])) this.layRoad(served, a, b);
    }
    for (let i = 0; i < W * H; i++) if (this.m[GRID + i] === SITE) this.rd[i] &= 0xe0;
  }

  /** 4fef:0a4b: each road its shape from the roads and bridges round it; a
   *  shape there is no piece for leaves plain or hills. Then a dozen or two
   *  straight pieces take their other look. */
  roadShapes() {
    for (let y = 0; y < H; y++) {
      for (let x = 0; x < W; x++) {
        if (this.road(x, y) === 0) continue;
        let m = 0;
        for (let i = 0; i < 8; i++) {
          const ax = x + this.r16(NB_ROAD + 4 * i), ay = y + this.r16(NB_ROAD + 4 * i + 2);
          if (ax >= 0 && ay >= 0 && ax < W && ay < H
              && (this.road(ax, ay) !== 0 || this.terrainOf(this.tile(ax, ay)) === BRIDGE)) m |= 1 << i;
        }
        const v = i8(this.m[ROAD_SHAPE + m]);
        if (v >= 0) this.setRoad(x, y, v + 1);
        else if (this.dice(1, 10, 0) < 6) {
          this.setTile(x, y, this.tileFor(PLAIN, 0, x, y));
          this.put(x, y, PLAIN);
        } else {
          this.setTile(x, y, this.tileFor(HILLS, 13, x, y));
        }
      }
    }
    for (const [from, to] of [[2, 17], [1, 16]]) {
      const n = this.dice(1, 10, 10);
      for (let k = 0; k < n; k++) {
        for (let tries = 0; tries < 10000; tries++) {
          const x = this.dice(1, W, -1), y = this.dice(1, H, -1);
          if (this.road(x, y) === from) { this.setRoad(x, y, to); break; }
        }
      }
    }
  }

  // --- 513d / 5132: the sides and the cities' details ---------------------

  /** 513d:05e3: the cities, read back off the tile map in rows. */
  cityList() {
    let n = 0;
    for (let y = 0; y < H; y++) {
      for (let x = 0; x < W; x++) {
        if (this.tile(x, y) !== 0x60) continue;
        const o = this.crec(n);
        this.ws16(o, x); this.ws16(o + 2, y);
        this.scn[o + 21] = 15;
        this.scn[o + 20] = this.dice(1, 3, 2);
        n++;
      }
    }
    this.cityCount = n;
  }

  /** 513d:08f0: a random city in one of the map's eight regions, four
   *  across and two down; after 202 tries, wherever the last one was. */
  cityIn(region) {
    const x0 = (region % 4) * 28, y0 = idiv(region, 4) * 78;
    let c = 0;
    for (let tries = 0; tries < 202; tries++) {
      c = this.dice(1, this.cityCount, -1);
      const x = this.cx(c), y = this.cy(c);
      if (x >= x0 && x < x0 + 28 && y >= y0 && y < y0 + 78) break;
    }
    return c;
  }

  /** 513d:0689: each side a capital in a region of its own, 12 from the
   *  edge and 20 from the others both ways; ten rounds of 100 tries a side,
   *  the last round taking what it gets. A side's armies favour its region. */
  capitals() {
    const S = (s) => S_SIDE_REC + 20 * s;
    for (let round = 1; ; round++) {
      const used = [];
      for (let s = 0; s < 8; s++) { this.ws16(S(s) + 6, -100); this.ws16(S(s) + 8, -100); }
      let again = false;
      for (let s = 0; s < 8 && !again; s++) {
        let region;
        do region = this.dice(1, 8, -1); while (used[region]);
        used[region] = true;
        let c = 0, ok = false, tries = 0;
        for (; !ok && tries < 100; tries++) {
          c = this.cityIn(region);
          const x = this.cx(c), y = this.cy(c);
          if (x < 12 || x > 100 || y < 12 || y > 144) continue;
          ok = true;
          for (let o = 0; o < 8; o++) {
            if (abs(x - this.s16(S(o) + 6)) < 20 || abs(y - this.s16(S(o) + 8)) < 20) { ok = false; break; }
          }
        }
        // a find on the hundredth try counts as none, as the original has it
        if (tries > 99 && round < 10) { again = true; break; }
        this.w16(REGION_CLASS + 2 * region, this.r16(SIDE_CLASS + 2 * s));
        this.ws16(S(s) + 6, this.cx(c));
        this.ws16(S(s) + 8, this.cy(c));
      }
      if (!again) break;
    }
    for (let s = 0; s < 8; s++) {
      const x = this.s16(S(s) + 6), y = this.s16(S(s) + 8);
      for (let c = 0; c < this.cityCount; c++) {
        if (this.cx(c) === x && this.cy(c) === y) { this.scn[this.crec(c) + 21] = s; break; }
      }
      // 513d:09b3: the castle in the side's colours
      const t = s < 6 ? 0x62 + 2 * s : 0x80 + 2 * (s - 6);
      this.setTile(x, y, t); this.setTile(x + 1, y, t + 1);
      this.setTile(x, y + 1, t + 16); this.setTile(x + 1, y + 1, t + 17);
    }
  }

  /** 5132:0014 / 006b: each side a name from five, and 3d50+20 gold. */
  sides() {
    for (let s = 0; s < 8; s++) {
      const name = this.str(SIDE_NAMES + 100 * s + 20 * this.dice(1, 5, -1), 20);
      for (let i = 0; i < 20; i++) this.scn[20 * s + i] = 0;
      this.wstr(20 * s, name);
    }
    for (let s = 0; s < 8; s++) this.ws16(S_SIDE_REC + 20 * s + 2, this.dice(3, 50, 20));
  }

  /** What touches a city's footprint (513d:0ce0, into 4125:02d6-02de). */
  surroundings(c) {
    const x = this.cx(c), y = this.cy(c);
    const by = (t) => this.near(x, y, t) || this.near(x + 1, y, t)
      || this.near(x, y + 1, t) || this.near(x + 1, y + 1, t);
    return { shore: by(SHORE), forest: by(FOREST), road: by(ROAD), hills: by(HILLS), marsh: by(MARSH) };
  }

  /** random_city_value (513d:1104): a capital 9; else 1d4-1, +4 by the
   *  shore, +2 by a road, -1 by forest, -2 by marsh, within 0..9. */
  value(c, f) {
    if (this.scn[this.crec(c) + 21] !== 15) return 9;
    let v = this.dice(1, 4, -1);
    if (f.shore) v += 4;
    if (f.road) v += 2;
    if (f.forest) v -= 1;
    if (f.marsh) v -= 2;
    if (v > 8) v = 9;
    if (v < 1) v = 0;
    return v;
  }

  /** 513d:1468: a name of two or three syllables, ending most often in a
   *  word for what lies round it. */
  cityName(c, f) {
    const three = this.dice(1, 10, 0) < 7;
    let name = this.str(0xa86 + 10 * this.dice(1, 20, -1), 10);
    if (three) name += this.str(0xb4e + 10 * this.dice(1, 20, -1), 10);
    let end = null;
    if (this.dice(1, 10, 0) < 7) {
      if (f.marsh) end = 0xda6;
      else if (f.forest) end = 0xcde;
      else if (f.shore) end = 0xd42;
      else if (f.hills) end = 0xe0a;
      else if (this.dice(1, 10, 0) < 5) end = 0xe6e;
    }
    name += end !== null ? this.str(end + 10 * this.dice(1, 10, -1), 10)
      : this.str(0xc16 + 10 * this.dice(1, 20, -1), 10);
    const o = this.crec(c) + 4;
    for (let i = 0; i < 16; i++) this.scn[o + i] = 0;
    this.wstr(o, name.slice(0, 15));
    return name.slice(0, 15);
  }

  /** 513d:1171: the city's name, income -- value x 2 + 1d8 + 14 -- and its
   *  three lines of description, grander as the city is richer. */
  cityText(c, v, f) {
    const name = this.cityName(c, f);
    this.ws16(this.crec(c) + 42, v * 2 + this.dice(1, 8, 0) + 14);
    const grade = () => Math.min(9, Math.max(0, this.dice(1, 3, v - 2)));
    const g1 = grade(), g2 = grade();
    const w = (o, i) => this.str(o + 16 * i, 16);
    let a, b, cc;
    if ((!f.forest && !f.marsh && !f.hills) || this.dice(1, 100, 0) > 79) {
      if (this.dice(1, 100, 0) < 50) {
        a = w(0x16a8, this.dice(1, 10, -1)); b = w(0x1748, this.dice(1, 10, -1)); cc = w(0x17e8, this.dice(1, 10, -1));
      } else {
        a = w(0x14c8, this.dice(1, 10, -1)); b = w(0x1568, this.dice(1, 10, -1)); cc = w(0x1608, this.dice(1, 10, -1));
      }
    } else {
      let k;
      if (f.marsh) k = this.dice(1, 2, 7);
      else if (f.hills) k = this.dice(1, 4, 3);
      else k = this.dice(1, 4, -1);
      a = w(0x12e8, this.dice(1, 10, -1)); b = w(0x1388, this.dice(1, 10, -1)); cc = w(0x1428, k);
    }
    const kind = w(0x1248, g2);
    const adj = this.str(0x1068 + 0x30 * g1 + 16 * this.dice(1, 3, -1), 16);
    this.cty += fmt("#%03d|%s is a %s|%s, %s|%s %s|\n", c, name, adj, kind, a, b, cc);
  }

  /** The army types' flags the generator asks after (build_army_move_flags,
   *  6715:0000): ARMYTYPE +54 flies, +48 magical -- an ally. */
  flies(t) { const a = this.types.byId[t]; return !!a && a.bonus[54] !== 0; }
  magical(t) { const a = this.types.byId[t]; return !!a && a.bonus[48] !== 0; }

  setSlot(c, k, e) {
    const o = this.crec(c), a = this.types.byId[e] || {};
    this.scn[o + 22 + k] = e;
    this.scn[o + 26 + k] = a.time || 0;
    this.scn[o + 30 + k] = a.strength || 0;
    this.scn[o + 34 + k] = a.move || 0;
    this.scn[o + 38 + k] = a.cost || 0;
  }

  /** 513d:161d: what a city can make. value / 2 + 1d4 - 1 slots, one more
   *  each by forest and hills, at most 4, from RANDOM.DAT's list in order:
   *  each type rolls its chance, and must suit the city's region, or its
   *  forest, hills or shore, or be at home anywhere. A rich city sometimes
   *  adds a flier. Allies only where the option lets cities make them. */
  production(c, v, f) {
    const x = this.cx(c), y = this.cy(c);
    const region = idiv(x * 4, W) + idiv(y * 2, H) * 4;
    let slots = v / 2 | 0;
    slots += this.dice(1, 4, -1);
    if (f.forest) slots++;
    if (f.hills) slots++;
    if (slots > 3) slots = 4;
    if (slots < 1) slots = 0;
    const rec = (e) => PRODUCTION + 16 * e;
    const suits = (e) => {
      const terr = this.r16(rec(e) + 6);
      return terr === 7 || (terr === 4 && f.forest) || (terr === 5 && f.hills)
        || this.r16(rec(e) + 4) === this.r16(REGION_CLASS + 2 * region);
    };
    let k = 0;
    for (let e = 0; e < 29 && k < slots; e++) {
      const t = this.r16(rec(e));
      if (!this.allies && this.magical(t)) continue;
      if (this.dice(1, 10, -1) >= this.r16(rec(e) + 2)) continue;
      let ok = suits(e) || (this.r16(rec(e) + 6) === 3 && f.shore);
      if (t === 5 && !f.shore) ok = false;
      if (ok) this.setSlot(c, k++, t);
    }
    if (k === 0) this.setSlot(c, k++, this.r16(rec(0)));
    if (v > 6 && this.dice(1, 10, -1) < 5 && k < 4) {
      let flier = false;
      for (let j = 0; j < k; j++) if (this.flies(this.scn[this.crec(c) + 22 + j])) flier = true;
      if (!flier && k < slots) {
        for (let e = 0; e < 29; e++) {
          const t = this.r16(rec(e));
          if (this.flies(t) && !this.magical(t) && suits(e)) { this.setSlot(c, k++, t); break; }
        }
      }
    }
    for (; k < 4; k++) this.scn[this.crec(c) + 22 + k] = 255;
  }

  /** random_magical_type (6563:1a9b). */
  magicalType() {
    let n = 0;
    for (let t = 0; t < 28; t++) if (this.magical(t)) n++;
    const pick = this.dice(1, n, -1);
    let i = 0, t = 0;
    for (; t < 28; t++) {
      if (!this.magical(t)) continue;
      if (i === pick) break;
      i++;
    }
    return t > 27 || !this.magical(t) ? 25 : t;
  }

  /** random_add_production (513d:1b3f): with allies on, 2d3 cities may also
   *  make one. */
  allyProduction() {
    let n = this.dice(2, 3, 0);
    for (let tries = 0; n > 0 && tries < 100; tries++) {
      const c = this.dice(1, this.cityCount, -1), o = this.crec(c);
      let k = 0;
      for (let j = 0; j < 4; j++) if (this.scn[o + 22 + j] < 128) k++;
      if (k < 4) { this.setSlot(c, k, this.magicalType()); n--; }
    }
  }

  /** 513d:1c1d: a capital adds a strong army it cannot make yet -- strength
   *  5 or more, not a navy or an ally -- in its last empty slot. */
  capitalArmies() {
    for (let s = 0; s < 8; s++) {
      const x = this.s16(S_SIDE_REC + 20 * s + 6), y = this.s16(S_SIDE_REC + 20 * s + 8);
      let c = -1;
      for (let i = 0; i < this.cityCount && c < 0; i++) if (this.cx(i) === x && this.cy(i) === y) c = i;
      if (c < 0) continue;
      const o = this.crec(c);
      // the highest empty slot, else the last
      let k = 3;
      while (k >= 0 && this.scn[o + 22 + k] < 128) k--;
      if (k < 0) k = 3;
      let draws = 0, pick = -1;
      while (draws <= 39) {
        const t = this.dice(1, 28, -1);
        if (t === 5 || this.magical(t)) continue;
        draws++;
        let has = false;
        for (let j = 0; j < 4; j++) if (this.scn[o + 22 + j] === t) has = true;
        if (!has && (this.types.byId[t] || {}).strength >= 5) { pick = t; break; }
      }
      // one found on the fortieth draw is dropped, as the original has it
      if (pick >= 0 && draws < 40) this.scn[o + 22 + k] = pick;
    }
  }

  /** random_map_cities (513d:0ce0). */
  cityDetails() {
    for (let c = 0; c < this.cityCount; c++) {
      const f = this.surroundings(c);
      const v = this.value(c, f);
      this.cityText(c, v, f);
      this.production(c, v, f);
    }
    if (this.allies) this.allyProduction();
    this.capitalArmies();
  }

  /** auto_file_random_sgn (4fef:113d): 41-70 signposts on open plain off
   *  the roads, most pointing the way to the nearest city, a few with
   *  RANDOM.DAT's own words on them. */
  signs() {
    const n = this.dice(1, 30, 40);
    const out = new Uint8Array(2 + 104 * n);
    const dv = new DataView(out.buffer);
    dv.setUint16(0, n, true);
    const put = (o, s) => { for (let i = 0; i < s.length && i < 49; i++) out[o + i] = s.charCodeAt(i) & 0xff; };
    let fixed = 0;
    for (let i = 0; i < n; i++) {
      let x = 0, y = 0;
      for (let tries = 0; tries < SAFETY; tries++) {
        x = this.dice(1, W, -1); y = this.dice(1, H, -1);
        if (this.terrainOf(this.tile(x, y)) === PLAIN && !this.near(x, y, CITY)
            && !this.near(x, y, TOWER) && this.road(x, y) === 0) break;
      }
      this.setTile(x, y, 0);
      const o = 2 + 104 * i;
      dv.setInt16(o, x, true); dv.setInt16(o + 2, y, true);
      if (this.dice(1, 10, 0) < 4 && fixed < this.r16(SIGNS)) {
        put(o + 4, this.str(SIGNS + 2 + 60 * fixed, 30));
        put(o + 54, this.str(SIGNS + 2 + 60 * fixed + 30, 30));
        fixed++;
      } else {
        let c = -1, best = 1000;
        for (let j = 0; j < this.cityCount; j++) {
          const d = distance(x, y, this.cx(j), this.cy(j));
          if (d < best) { best = d; c = j; }
        }
        if (c < 0) continue;
        const cxy = [this.cx(c), this.cy(c)];
        put(o + 4, fmt(this.str(SIGN_CITY, 20), cstr(this.scn, this.crec(c) + 4, 16)));
        put(o + 54, fmt(this.str(SIGN_LEAGUES, 20),
          distance(x, y, cxy[0], cxy[1]) * 2, COMPASS[compass(x, y, cxy[0], cxy[1])]));
      }
    }
    return out;
  }

  // --- the whole -------------------------------------------------------------

  /** random_map_generate (4bed:011c), yielding as the original's progress
   *  bar moves; returns the scenario's files. */
  *run() {
    yield 0;
    // 4bed:036b: the map starts as sea
    for (let i = 0; i < W * H; i++) { this.m[GRID + i] = WATER; this.rd[i] &= 0xe0; }
    this.coast();
    yield 10;
    this.highlands();
    yield 20;
    this.erosion();
    yield 30;
    this.rivers();
    yield 40;
    this.forests();
    yield 50;
    this.marshes();
    yield 60;
    this.cities();
    yield 70;
    yield* this.sites();
    yield 80;
    yield* this.roads();
    yield 90;
    // 4fef:0014
    this.roadShapes();
    yield 92;
    this.cityList();
    this.capitals();
    this.sides();
    yield 94;
    this.cityDetails();
    yield 96;
    const sgn = this.signs();
    yield 98;
    this.scn[S_RANDOM] = 1; this.scn[S_RANDOM + 1] = 0;
    const map = new Uint8Array(2 * W * H);
    const mv = new DataView(map.buffer);
    for (let i = 0; i < W * H; i++) mv.setUint16(2 * i, this.tiles[i], true);
    const rd = new Uint8Array(W * H);
    for (let i = 0; i < W * H; i++) rd[i] = this.rd[i] & 0x1f;
    yield 100;
    return { SCN: this.scn, MAP: map, RD: rd, SGN: sgn, CTY: bytesOf(this.cty), SPC: bytesOf(this.spc) };
  }
}

const FOOTPRINT = [[0, 0], [1, 0], [0, 1], [1, 1]];       // DS:02c4, 02cc

/** The checkerboard: 1 where x + y is even (C's (n)/2 != (n-1)/2). */
function parity(x, y) {
  const n = x + y;
  return n === 0 || idiv(n, 2) !== idiv(n - 1, 2) ? 1 : 0;
}

function clamp(p) {                     // 4bed:0470
  if (p.x < 1) p.x = 0;
  if (p.y < 1) p.y = 0;
  if (p.x > W - 1) p.x = W - 1;
  if (p.y > H - 1) p.y = H - 1;
}

const i8 = (b) => (b > 127 ? b - 256 : b);

/** The sliders as random_map_setup (7bab:10e8) takes them: Water, Hills,
 *  Cities, Forest, each 0-6, 7 rolled as 1d7-1. */
export function settle(sliders, rng) {
  return sliders.map((v) => (v === RANDOM_SLIDER ? rng.dice(1, 7, -1) : v));
}

/** random_map_load_params (4bed:0000): RANDOM.DAT with the sliders added. */
export function params(dataDir, sliders) {
  const dat = vfs.read(dataDir + "/RANDOM/RANDOM.DAT");
  if (!dat) throw new Error("Random data not found");
  const m = new Uint8Array(dat);
  const v = new DataView(m.buffer);
  const add = (o, t, i) => v.setInt16(o, v.getInt16(o, true) + v.getInt16(t + 2 * i, true), true);
  const [water, hills, cities, forest] = sliders;
  add(P_CITIES, SLIDER_CITIES, cities);
  add(P_FOREST, SLIDER_FOREST, forest);
  add(P_HILLS, SLIDER_HILLS, hills);
  add(P_MOUNTAINS, SLIDER_HILLS, hills);
  add(P_WIDE_RIVERS, SLIDER_WATER, water);
  add(P_RIVERS, SLIDER_WATER, water);
  return m;
}

/** Make a random world. A generator: it yields the progress, 0-100, as the
 *  original's bar shows it, and returns the files, keyed by extension.
 *
 *  opts: { dataDir, rng, sliders: [water, hills, cities, forest] (0-7),
 *          allies, terrainSet } */
export function* generate(opts) {
  const sliders = settle(opts.sliders || [3, 3, 2, 3], opts.rng);
  const dat = params(opts.dataDir, sliders);
  // auto_file_x_scn (4fef:1001): the scenario of the chosen terrain set,
  // Erythea for the only one there is, is the template
  const template = vfs.read(opts.dataDir + "/ERYTHEA/ERYTHEA.SCN");
  if (!template) throw new Error("cannot open: ERYTHEA.SCN");
  const types = armytype.load(opts.dataDir + "/TERRAIN0/ARMYTYPE.DAT");
  const gen = new Generator(dat, template, types, opts.rng, opts);
  gen.scn[S_TERRAIN_SET] = opts.terrainSet || 0;
  return yield* gen.run();
}

/** Run a generation through to its end. */
export function generateNow(opts) {
  const it = generate(opts);
  for (;;) {
    const r = it.next();
    if (r.done) return r.value;
  }
}

/** Put a random world's files where scn.load will find them. */
export function install(dataDir, files) {
  for (const ext of Object.keys(files)) {
    vfs.install(`${dataDir}/${DIR}/${DIR}.${ext}`, files[ext]);
  }
}

/** The terrain set's name, as the start menu shows it (DATA\TERRAIN.DAT). */
export function terrainSetName(dataDir, set) {
  const s = vfs.read(dataDir + "/DATA/TERRAIN.DAT");
  if (!s || set >= u16(s, 0)) return "";
  return cstr(s, 2 + 52 * set + 2, 50);
}

/** How many terrain sets there are. */
export function terrainSets(dataDir) {
  const s = vfs.read(dataDir + "/DATA/TERRAIN.DAT");
  return s ? Math.max(1, u16(s, 0)) : 1;
}
