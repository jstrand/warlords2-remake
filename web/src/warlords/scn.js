// Warlords II scenario data: .SCN, .MAP, .RD, .ITM
// See docs/formats/scenario.md, docs/formats/map.md and docs/formats/itm.md.
//
// The .SCN file is loaded verbatim into one segment by the original, so every
// offset here is also a live memory address in WARLORD2.EXE.

import * as vfs from "../vfs.js";
import { u16, cstr } from "./bytes.js";

export const MAP_W = 112, MAP_H = 156;
export const TILE = 40;              // terrain tiles are 40x40
export const TILE_MASK = 0x7FFF;     // bit 15 of a map entry is a flag

const SIDE_NAMES = 0, SIDE_STRIDE = 20;
// 0xa0 is the side's own colour, 0xb0 the colour it outlines things in.
const SIDE_COLOURS = 0xa0, SIDE_EDGES = 0xb0;
const SIDE_RECS = 387, SIDE_REC_STRIDE = 20;
const LEVELS = 0xc0, CONTROLLERS = 0xd0, ENHANCED = 0xf0;
const OBSERVE = 0x147, CARDS = 0xe0;
const MONSTER_STRENGTH = 0x1007;
const FIGHT_ORDER = 0x60b, FIGHT_ROWS = 9, FIGHT_TYPES = 29;
const COMBAT_CAP = 0x112, DIPLO_SCORE = 0x10e3;
const TERRAIN_TABLE = 0x710, TERRAIN_COUNT = 255;
const SITES_COUNT = 0x80f, SITES = 0x811, SITE_STRIDE = 31;
const ITEMS = 3305, ITEM_STRIDE = 29, N_ITEMS = 22;
const MONSTERS = 3943, MONSTER_STRIDE = 16, N_MONSTERS = 10;
const CITIES_COUNT = 5499, CITIES = 5501, CITY_STRIDE = 65;

// Option words. They are NOT in menu order; see docs/rules.md > Game setup.
const OPTIONS = {
  neutralCities: 0x11a, diplomacy: 0x11c, quests: 0x11e,
  randomTurns: 0x122, hiddenMap: 0x124, intenseCombat: 0x126,
  quickStart: 0x128, viewEnemies: 0x12a, militaryAdvisor: 0x12c,
  tutorial: 0x12e, viewProduction: 0x132,
};

export function readAll(path) {
  const s = vfs.read(path);
  if (!s) throw new Error("cannot open: " + path);
  return s;
}

function lines(path) {
  const t = vfs.readText(path);
  if (t === null) return null;
  return t.split(/[\r\n]+/).filter((l) => l.length > 0);
}

/** The scenario's .ITM magic item pool (docs/formats/itm.md). */
export function loadItemPool(path) {
  const ls = lines(path);
  if (!ls) return null;
  const count = parseInt(ls[0], 10);
  const pool = [];
  for (let i = 1; i < Math.min(ls.length, count + 1); i++) {
    const line = ls[i];
    pool.push({
      name: line.slice(0, 20).replace(/_/g, " ").replace(/\s+$/, ""),
      type: parseInt(line.slice(21, 22), 10),
      value: parseInt(line.slice(23, 24), 10),
    });
  }
  return pool;
}

/** A .CTY or .SPC file: the three lines of description for each city or site,
 *  as `#NNN|line|line|line|`. Returns {[index]: [line, line, line]}. */
export function loadDescriptions(path) {
  const out = {};
  const ls = lines(path);
  if (!ls) return out;
  for (const line of ls) {
    const m = line.match(/^#(\d+)\|(.*)$/);
    if (m) {
      const parts = [];
      const re = /([^|]*)\|/g;
      let f;
      while ((f = re.exec(m[2])) !== null) parts.push(f[1]);
      out[parseInt(m[1], 10)] = [parts[0] || "", parts[1] || "", parts[2] || ""];
    }
  }
  return out;
}

/** The signposts (.SGN): a u16 count, then 104-byte records -- x, y, and two
 *  lines of 50 bytes. Each is { index, x, y, lines: [line, line] }. */
export function loadSigns(path) {
  const out = [];
  const s = vfs.read(path);
  if (!s) return out;
  for (let i = 0; i < u16(s, 0); i++) {
    const o = 2 + 104 * i;
    if (o + 104 > s.length) break;
    out.push({ index: i, x: u16(s, o), y: u16(s, o + 2),
               lines: [cstr(s, o + 4, 50), cstr(s, o + 54, 50)] });
  }
  return out;
}

export function load(dir, name) {
  const base = dir + "/" + name;
  const s = readAll(base + ".SCN");
  if (s.length !== 12001) throw new Error("unexpected .SCN size: " + s.length);

  const options = {};
  for (const k in OPTIONS) options[k] = u16(s, OPTIONS[k]);

  const sides = [];
  for (let i = 0; i < 8; i++) {
    const o = SIDE_RECS + SIDE_REC_STRIDE * i;
    sides.push({
      index: i,
      name: cstr(s, SIDE_NAMES + SIDE_STRIDE * i, SIDE_STRIDE),
      colour: u16(s, SIDE_COLOURS + 2 * i),
      edge: u16(s, SIDE_EDGES + 2 * i),
      gold: u16(s, o + 2),
      capX: u16(s, o + 6),
      capY: u16(s, o + 8),
      computer: u16(s, CONTROLLERS + 2 * i) !== 0,
      level: u16(s, LEVELS + 2 * i),
      enhanced: u16(s, ENHANCED + 2 * i) !== 0,
      observe: u16(s, OBSERVE + 2 * i) !== 0,
      diploScore: u16(s, DIPLO_SCORE + 2 * i),
      card: u16(s, CARDS + 2 * i),
    });
  }

  const cities = [], byPos = {};
  for (let i = 0; i < u16(s, CITIES_COUNT); i++) {
    const o = CITIES + CITY_STRIDE * i;
    const produces = [];
    for (let k = 0; k < 4; k++) {
      const t = s[o + 22 + k];
      // the game removes Navy (type 5) from every city at start
      if (t !== 255 && t !== 5) produces.push(t);
    }
    const c = {
      index: i,
      x: u16(s, o), y: u16(s, o + 2),
      name: cstr(s, o + 4, 16),
      income: s[o + 42],
      produces,
      defence: produces.length >= 3 ? 2 : 1,
    };
    cities.push(c);
    byPos[c.y * MAP_W + c.x] = c;
  }

  // A city covers a 2x2 footprint; byPos keys the top-left, cityTile every tile.
  const cityTile = {};
  for (const c of cities) {
    for (let dx = 0; dx <= 1; dx++) {
      for (let dy = 0; dy <= 1; dy++) cityTile[(c.y + dy) * MAP_W + (c.x + dx)] = c;
    }
  }

  // Ownership is derived: at scenario start a side owns only its capital.
  for (const sd of sides) {
    const c = byPos[sd.capY * MAP_W + sd.capX];
    sd.inUse = c !== undefined;
    sd.capital = c;
    if (c) c.owner = sd;
  }

  // fight order: 29 bytes per player, row 8 for neutral
  const fightOrder = [];
  for (let row = 0; row < FIGHT_ROWS; row++) {
    const r = [];
    for (let t = 0; t < FIGHT_TYPES; t++) r[t] = s[FIGHT_ORDER + row * FIGHT_TYPES + t];
    fightOrder[row] = r;
  }

  const sites = [];
  for (let i = 0; i < u16(s, SITES_COUNT); i++) {
    const o = SITES + SITE_STRIDE * i;
    sites.push({
      index: i,
      x: u16(s, o), y: u16(s, o + 2),
      name: cstr(s, o + 4, 20),
      type: u16(s, o + 24),          // 1 = temple, 2 = ruin
    });
  }

  const items = [];
  for (let i = 0; i < N_ITEMS; i++) {
    const o = ITEMS + ITEM_STRIDE * i;
    const nm = cstr(s, o, 20);
    if (nm !== "") items.push({ index: i, name: nm, type: s[o + 20], value: s[o + 21] });
  }

  const monsters = {};
  for (let i = 0; i < N_MONSTERS; i++) {
    const nm = cstr(s, MONSTERS + MONSTER_STRIDE * i, 12);
    // keyed by slot: a site's guardian byte indexes this directly
    if (nm !== "") monsters[i] = { index: i, name: nm, strength: u16(s, MONSTER_STRENGTH + 2 * i) };
  }

  // tile index -> terrain type id, the same table WARLORD2.EXE reads
  const terrainType = [];
  for (let i = 0; i < TERRAIN_COUNT; i++) terrainType[i] = s[TERRAIN_TABLE + i];

  // terrain grid + road overlay; bit 15 of a map word marks a crossing
  const m = readAll(base + ".MAP");
  const tiles = new Array(MAP_W * MAP_H), crossing = new Array(MAP_W * MAP_H);
  for (let i = 0; i < MAP_W * MAP_H; i++) {
    const w = u16(m, i * 2);
    tiles[i] = w % 0x8000;
    crossing[i] = w >= 0x8000;
  }
  const roads = readAll(base + ".RD");

  return {
    name, sides, cities, cityAt: byPos, cityTile,
    sites, items, monsters, crossing,
    itemPool: loadItemPool(base + ".ITM"),
    cityText: loadDescriptions(base + ".CTY"),
    siteText: loadDescriptions(base + ".SPC"),
    signs: loadSigns(base + ".SGN"),
    options, terrainType, fightOrder,
    combatCap: u16(s, COMBAT_CAP),
    tiles, roads,
    width: MAP_W, height: MAP_H,
  };
}

export function tileAt(g, x, y) {
  return g.tiles[y * MAP_W + x];
}

export function roadAt(g, x, y) {
  return g.roads[y * MAP_W + x];
}

/** Is this tile a crossing (map word bit 15)? */
export function isCrossing(g, x, y) {
  return g.crossing[y * MAP_W + x];
}

/** The terrain type id (0-11) of a map tile, via the scenario's own table. */
export function terrainAt(g, x, y) {
  return g.terrainType[tileAt(g, x, y) % 256];
}

// A city's castle is drawn from the map itself: the game rewrites its four
// tiles whenever the city changes hands (set_city_tiles, 6bd8:0000); ruins
// keep the colours of whoever held the city (city_make_ruins, 649c:016b).
export function cityTileBase(city) {
  if (city.razed) return 0xa0 + 2 * (city.razedBy || 0);
  const o = city.ownerIndex;
  if (o == null) return 96;          // the original passes owner -1
  if (o < 6) return 0x62 + 2 * o;
  return 0x80 + 2 * (o - 6);
}

/** Stamp a city's 2x2 castle onto the map in its owner's colours. */
export function setCityTiles(map, city) {
  const t = cityTileBase(city);
  const i = city.y * MAP_W + city.x;
  map.tiles[i] = t;
  map.tiles[i + 1] = t + 1;
  map.tiles[i + MAP_W] = t + 16;
  map.tiles[i + MAP_W + 1] = t + 17;
}

/** Restamp every city. */
export function refreshCityTiles(map) {
  for (const c of map.cities) setCityTiles(map, c);
}
