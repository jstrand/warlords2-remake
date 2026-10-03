// The original's 640x480 screen: background, controls and hit regions.
//
// Everything here is driven by the game's own layout files (uidata.js) and
// its own art, so the chrome is the original's rather than a lookalike.
// docs/re/ui.md and docs/formats/screens.md.

import * as vfs from "../vfs.js";
import * as gfx from "../gfx.js";
import * as pck from "./pck.js";
import * as uidata from "./uidata.js";
import * as layout from "./layout.js";
import * as scn from "./scn.js";
import * as STONE from "./stonetile.js";

export const WIDTH = 640, HEIGHT = 480;

// The main screen's regions, by the id AREA.DAT gives them.
export const REGION = {
  STRATEGIC: 1,     // (400,30) 224x312
  MAP: 2,           // (16,30)  360x360
  MENUBAR: 3,       // (0,0)    640x18
  BOTTOMBAR: 9,     // (16,408) 360x56
  MAPPANEL: 13,
};

export const TILE = 40;

/** Find a .pck by name, wherever it lives: PICS/, TERRAIN<n>/ or the root. */
function findArt(dataDir, name, terrain) {
  const tries = [
    `${dataDir}/PICS/${name.toUpperCase()}`,
    `${dataDir}/TERRAIN${terrain || 0}/${name.toUpperCase()}`,
    `${dataDir}/${name.toUpperCase()}`,
  ];
  for (const p of tries) if (vfs.hasImage(p)) return p;
  return null;
}

/** Load the layout and every bitmap the given dialog needs. */
export function load(dataDir, palette, dialogId, terrain) {
  const ui = uidata.load(dataDir);
  const self = {
    ui,
    dialog: uidata.dialog(ui, dialogId || uidata.MAIN_SCREEN),
    backgroundPx: {},
    art: {},
    dataDir,
    palette,
  };

  self.art_for = (bitmapId) => {
    if (self.art[bitmapId]) return self.art[bitmapId];
    const name = ui.bitmaps[bitmapId];
    if (!name) return null;
    const path = findArt(dataDir, name, terrain);
    if (!path) return null;
    const [img, w, h] = pck.toImage(path, palette);
    self.art[bitmapId] = { image: img, w, h };
    return self.art[bitmapId];
  };

  // The background is four 320x240 quadrants of one 640x480 image.
  self.background = {};
  for (let i = 0; i < 4; i++) {
    const path = findArt(dataDir, `screen${i}.pck`, terrain);
    if (path) {
      const [img, , , px] = pck.toImage(path, palette);
      self.background[i] = { image: img, x: (i % 2) * 320, y: Math.floor(i / 2) * 240 };
      self.backgroundPx[i] = px;
    }
  }
  self.original = self.dialog;

  // TERRAIN<n>/MAPCOLOR.DAT, the strategic map's own colour table (834b:2a11):
  // a colour per tile id for each of a tile's four pixels, a remap, and the
  // same four pixels for a tile with a road on it.
  self.mapColour = { tile: {}, road: {}, remap: {} };
  const d = vfs.read(`${dataDir}/TERRAIN${terrain || 0}/MAPCOLOR.DAT`);
  if (d) {
    for (let q = 0; q < 4; q++) {
      const t = [];
      for (let i = 0; i < 256; i++) t[i] = d[q * 256 + i];
      self.mapColour.tile[q] = t;
      const r = [];
      for (let i = 0; i < 20; i++) r[i] = d[1120 + q * 20 + i];
      self.mapColour.road[q] = r;
    }
    for (let i = 0; i < 16; i++) self.mapColour.remap[i] = d[1024 + i * 2];
  }

  // Controls, with a live state each. State 1 is the resting look.
  self.state = {};
  for (const c of self.dialog.controls) self.state[c.id] = uidata.NORMAL;

  const quads = {};
  self.quad = (c, st) => {
    const key = c.id * 4 + st;
    if (quads[key]) return quads[key];
    const art = self.art_for(c.bitmap);
    if (!art) return null;
    const s = c.src[st];
    if (s.x + c.w > art.w || s.y + c.h > art.h) return null;
    quads[key] = gfx.newQuad(s.x, s.y, c.w, c.h);
    return quads[key];
  };
  return self;
}

/** Place the main screen's controls and regions for layout `L`. */
export function relayout(self, L) {
  const d = self.original;
  const out = Object.assign({}, d);
  out.controls = d.controls.map((c) => layout.move(L, layout.group(c), c));
  out.regions = d.regions.map((r) => layout.region(L, r));
  self.dialog = out;
}

// A bigger screen's ground is the original's speckled stone, a 244 x 220
// tile repeated from (0, 0); every panel is the original's own, copied whole
// and sunk into the stone by a one-pixel bevel.
const LIGHT = 2, DARK = 4;
const PANELS = [
  { group: "strat", x: 399, y: 29, w: 226, h: 314 },
  { group: "panel", x: 399, y: 354, w: 226, h: 116 },
  { group: "bar", x: 15, y: 402, w: 362, h: 68 },
];

function composedArt(self) {
  if (self.bgImage) return;
  const px = new Uint8Array(640 * 480);
  for (let i = 0; i < 4; i++) {
    const q = self.backgroundPx[i], qx = (i % 2) * 320, qy = Math.floor(i / 2) * 240;
    if (q) {
      for (let y = 0; y < 240; y++) {
        for (let x = 0; x < 320; x++) px[(qy + y) * 640 + qx + x] = q[y * 320 + x];
      }
    }
  }
  self.bgImage = pck.imageFromPixels(640, 480, px, self.palette);
  const tile = new Uint8Array(STONE.W * STONE.H);
  for (const [tx, ty, w, h, sx, sy] of STONE.RECTS) {
    for (let y = 0; y < h; y++) {
      for (let x = 0; x < w; x++) tile[(ty + y) * STONE.W + tx + x] = px[(sy + y) * 640 + sx + x];
    }
  }
  self.stoneImg = pck.imageFromPixels(STONE.W, STONE.H, tile, self.palette);
  self.stoneImg.setWrap("repeat", "repeat");
}

function setPal(self, i) {
  gfx.setColor(self.palette[i]);
}

function bevel(self, x, y, w, h) {
  setPal(self, DARK);
  gfx.rectangle("fill", x - 1, y - 1, w + 2, 1);
  gfx.rectangle("fill", x - 1, y, 1, h + 1);
  setPal(self, LIGHT);
  gfx.rectangle("fill", x, y + h, w + 1, 1);
  gfx.rectangle("fill", x + w, y, 1, h);
}

/** The main screen's ground for layout `L`. */
export function drawBackground(self, L) {
  gfx.setColor(1, 1, 1);
  if (!L || L.classic) {
    for (const k in self.background) {
      const q = self.background[k];
      gfx.draw(q.image, q.x, q.y);
    }
    return;
  }
  composedArt(self);
  gfx.draw(self.stoneImg, gfx.newQuad(0, 0, L.w, L.h), 0, 0);
  setPal(self, LIGHT);
  gfx.rectangle("fill", 0, 0, L.w - 1, 1);
  gfx.rectangle("fill", 0, 1, 1, L.h - 2);
  setPal(self, DARK);
  gfx.rectangle("fill", 0, L.h - 1, L.w, 1);
  gfx.rectangle("fill", L.w - 1, 0, 1, L.h - 1);
  const m = L.map;
  bevel(self, m.x - 1, m.y - 1, m.w + 2, m.h + 2);
  gfx.setColor(0, 0, 0);
  gfx.rectangle("fill", m.x - 1, m.y - 1, m.w + 2, 1);
  gfx.rectangle("fill", m.x - 1, m.y + m.h, m.w + 2, 1);
  gfx.rectangle("fill", m.x - 1, m.y - 1, 1, m.h + 2);
  gfx.rectangle("fill", m.x + m.w, m.y - 1, 1, m.h + 2);
  for (const p of PANELS) {
    const at = layout.move(L, p.group, p);
    bevel(self, at.x, at.y, p.w, p.h);
    gfx.setColor(1, 1, 1);
    gfx.draw(self.bgImage, gfx.newQuad(p.x, p.y, p.w, p.h), at.x, at.y);
  }
}

// Some controls share a rect exactly -- 183/184/185, 240/241. The live one
// is what shows and what a click reaches; with none live, the first.
const sameRect = (a, b) => a.x === b.x && a.y === b.y && a.w === b.w && a.h === b.h;

function isCovered(self, c, index) {
  const live = (self.state[c.id] ?? uidata.NORMAL) !== uidata.DISABLED;
  const cs = self.dialog.controls;
  for (let i = 0; i < cs.length; i++) {
    const o = cs[i];
    if (i !== index && sameRect(o, c)) {
      const oLive = (self.state[o.id] ?? uidata.NORMAL) !== uidata.DISABLED;
      if (oLive && (!live || i < index)) return true;
      if (!oLive && !live && i < index) return true;
    }
  }
  return false;
}

export function drawControls(self) {
  gfx.setColor(1, 1, 1);
  self.dialog.controls.forEach((c, i) => {
    if (c.bitmap !== 0 && c.w > 0 && c.h > 0 && !isCovered(self, c, i)) {
      const st = self.state[c.id] ?? uidata.NORMAL;
      const q = self.quad(c, st);
      const art = self.art_for(c.bitmap);
      if (q && art) gfx.draw(art.image, q, c.x, c.y);
    }
  });
}

/** Another dialog's layout, sharing this screen's art and bitmap table. */
export function dialog(self, id) {
  const d = uidata.dialog(self.ui, id);
  if (!d) return null;
  const state = {};
  for (const c of d.controls) state[c.id] = uidata.NORMAL;
  return { dialog: d, state, id };
}

// The default buttons: the controls Enter presses (DS:155c, 17be:0064).
export const DEFAULT_IDS = new Set([123, 103, 141, 174, 189, 192, 201, 223, 287, 242, 285, 251,
  292, 294, 330, 331, 332, 172, 356, 359, 368, 370, 396, 421,
  424, 425, 457, 469, 282, 485, 484, 483, 474, 489, 490, 495]);

/** The ring round a default button (1a0a:0005). */
function defaultRing(self, c) {
  gfx.setColor(self.palette ? self.palette[0] : [0, 0, 0]);
  const across = (x, y, n) => gfx.rectangle("fill", x, y, n, 1);
  const down = (x, y, n) => gfx.rectangle("fill", x, y, 1, n);
  let X = c.x - 2, Y = c.y - 2, W = c.w + 3, H = c.h + 3;
  for (let k = 0; k < 2; k++) {
    across(X + 1, Y, W - 2); down(X + W - 1, Y, 2);
    across(X + W - 1, Y + 1, 2); down(X + W, Y + 1, H - 2);
    across(X + W - 1, Y + H - 1, 2); down(X + W - 1, Y + H - 1, 2);
    across(X + 1, Y + H, W - 2); down(X + 1, Y + H - 1, 2);
    across(X, Y + H - 1, 2); down(X, Y + 1, H - 2);
    across(X, Y + 1, 2); down(X + 1, Y, 2);
    X--; Y--; W += 2; H += 2;
  }
  gfx.setColor(1, 1, 1);
}

/** The controls of a loaded dialog, each default button with its ring. */
export function drawDialogControls(self, view) {
  gfx.setColor(1, 1, 1);
  for (const c of view.dialog.controls) {
    if (c.bitmap !== 0 && c.w > 0 && c.h > 0) {
      const art = self.art_for(c.bitmap);
      const s = c.src[view.state[c.id] ?? uidata.NORMAL];
      if (art && s.x + c.w <= art.w && s.y + c.h <= art.h) {
        gfx.draw(art.image, gfx.newQuad(s.x, s.y, c.w, c.h), c.x, c.y);
      }
    }
    if (DEFAULT_IDS.has(c.id) && !view.screen && c.w > 0 && c.h > 0) defaultRing(self, c);
  }
}

export function dialogControl(view, id) {
  return view.dialog.controls.find((c) => c.id === id) || null;
}

export function dialogControlAt(view, x, y) {
  for (const c of view.dialog.controls) {
    if (c.w > 0 && c.h > 0 && x >= c.x && x < c.x + c.w && y >= c.y && y < c.y + c.h) return c;
  }
  return null;
}

/** The strategic map as one 224x312 image, two pixels a tile each way, as
 *  834b:2785 paints it from MAPCOLOR.DAT. Tile ids 0x50-0x5f get a random
 *  grey per pixel, from a generator of their own so the game's dice never
 *  move. Tiles `side` has not seen are black. */
export function strategicImage(self, g, side, seen) {
  const w = g.map.width, h = g.map.height;
  const mc = self.mapColour;
  let seed = 12345;
  const grey = () => {
    seed = (seed * 1103515245 + 12345) % 2147483648;
    return 2 + Math.floor(seed / 65536) % 3;
  };
  const rgb = [];
  for (let i = 0; i < 16; i++) {
    const c = self.palette[mc.remap[i] ?? i] || [0, 0, 0];
    rgb[i] = [Math.floor(c[0] * 255), Math.floor(c[1] * 255), Math.floor(c[2] * 255)];
  }
  const out = new Uint8Array(w * 2 * h * 2 * 4);
  const put = (x, y, i) => {
    const o = (y * w * 2 + x) * 4;
    if (i < 0) { out[o + 3] = 255; return; }
    const c = rgb[i] || [0, 0, 0];
    out[o] = c[0]; out[o + 1] = c[1]; out[o + 2] = c[2]; out[o + 3] = 255;
  };
  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      let px;
      if (seen && !seen(g, side, x, y)) {
        px = [-1, -1, -1, -1];
      } else {
        const road = scn.roadAt(g.map, x, y) % 32;
        const id = road !== 0 ? road - 1 : (scn.tileAt(g.map, x, y) % 256);
        if (road === 0 && id >= 0x50 && id <= 0x5f) {
          px = [grey(), grey(), grey(), grey()];
        } else {
          const t = road !== 0 ? mc.road : mc.tile;
          px = [0, 1, 2, 3].map((q) => (t[q] && t[q][id] !== undefined ? t[q][id] : 0));
        }
      }
      put(x * 2, y * 2, px[0]); put(x * 2 + 1, y * 2, px[1]);
      put(x * 2, y * 2 + 1, px[2]); put(x * 2 + 1, y * 2 + 1, px[3]);
    }
  }
  return gfx.newImageRGBA(w * 2, h * 2, out);
}

/** The region under a point, topmost first, as the original hit-tests. */
export function regionAt(self, x, y) {
  const rs = self.dialog.regions;
  for (let i = rs.length - 1; i >= 0; i--) {
    const r = rs[i];
    if (r.w > 0 && r.h > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return r;
  }
  return null;
}

export function region(self, id) {
  return self.dialog.regions.find((r) => r.id === id) || null;
}

export function control(self, id) {
  return self.dialog.controls.find((c) => c.id === id) || null;
}

/** The control under a point, or null. */
export function controlAt(self, x, y) {
  const cs = self.dialog.controls;
  for (let i = 0; i < cs.length; i++) {
    const c = cs[i];
    if (c.w > 0 && c.h > 0 && !isCovered(self, c, i)
        && x >= c.x && x < c.x + c.w && y >= c.y && y < c.y + c.h) {
      return c;
    }
  }
  return null;
}
