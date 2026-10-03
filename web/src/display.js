// The screen, in real pixels, and the two scales everything is drawn at.
//
// Not the original's: it drew 640x480 and nothing else. Three kinds of
// coordinate, as in the Lua remake:
//
//   device  the canvas's own pixels -- the page's CSS pixels times the
//           device pixel ratio, so nothing resamples them on the way
//   UI      the interface's pixels: every rect in the game's data. One UI
//           pixel is `scale` device pixels across, a whole number
//   map     the map's own pixels, 40 to a tile, drawn at `zoom` device
//           pixels each, apart from the UI's
//
// Handlers and hit tests all work in UI pixels: the mouse is brought down to
// them before main.js sees it.

import * as gfx from "./gfx.js";

export const W = 640, H = 480;

export const display = {
  W, H,
  dpi: 1,
  pw: 640, ph: 480,       // the canvas, in device pixels
  top: 0,
  scale: 1,
  maxScale: 1,
  chosen: null,           // a scale picked from the menu, if any
  windowed: true,         // not in the browser's full screen
  wanted: null,           // { w, h } to keep to: View > 4:3
  w: 640, h: 480,         // the frame being drawn, in UI pixels
  ox: 0, oy: 0,           // where its (0, 0) sits, in device pixels
  canvas: null,
  mouseInside: false,
};

/** Size the canvas to the window, in device pixels. */
export function fitCanvas() {
  const c = display.canvas;
  const dpr = window.devicePixelRatio || 1;
  const cw = Math.floor(window.innerWidth * dpr), ch = Math.floor(window.innerHeight * dpr);
  if (c.width !== cw || c.height !== ch) {
    c.width = cw;
    c.height = ch;
  }
  c.style.width = window.innerWidth + "px";
  c.style.height = window.innerHeight + "px";
  display.dpi = dpr;
}

/** Read the canvas's size, and pick the UI's scale: the one chosen from the
 *  menu, or else the biggest whole number that still fits 640x480. */
export function measure() {
  display.pw = display.canvas.width;
  display.ph = display.canvas.height - display.top;
  display.maxScale = Math.max(1, Math.floor(Math.min(display.pw / W, display.ph / H)));
  display.scale = Math.min(display.chosen || display.maxScale, display.maxScale);
}

/** Draw the UI at `n` device pixels a pixel from now on, as far as it fits. */
export function choose(n) {
  display.chosen = Math.max(1, Math.floor(n));
  measure();
}

/** How many UI pixels the game gets. */
export function uiSize() {
  let w = Math.floor(display.pw / display.scale);
  let h = Math.floor(display.ph / display.scale);
  const want = display.wanted;
  if (want) { w = Math.min(w, want.w); h = Math.min(h, want.h); }
  return [w, h];
}

/** Draw a frame of w x h UI pixels from here on, centred on the screen. */
export function setFrame(w, h) {
  display.w = w;
  display.h = h;
  display.ox = Math.floor((display.pw - w * display.scale) / 2);
  display.oy = display.top + Math.floor((display.ph - h * display.scale) / 2);
}

/** Draw in UI pixels until the matching pop. */
export function pushUI() {
  gfx.push();
  gfx.origin();
  gfx.translate(display.ox, display.oy);
  gfx.scale(display.scale);
}

/** Draw in map pixels until the matching pop, one map pixel to `zoom` device
 *  pixels, slid `camX, camY` device pixels from the UI point (x, y). */
export function pushMap(x, y, camX, camY, zoom) {
  gfx.push();
  gfx.origin();
  gfx.translate(display.ox + x * display.scale - camX, display.oy + y * display.scale - camY);
  gfx.scale(zoom);
}

/** A page position (CSS pixels) as a UI point, held to the frame. */
export function toUI(clientX, clientY) {
  const s = display.scale;
  const x = Math.floor((clientX * display.dpi - display.ox) / s);
  const y = Math.floor((clientY * display.dpi - display.oy) / s);
  return [Math.max(0, Math.min(display.w - 1, x)), Math.max(0, Math.min(display.h - 1, y))];
}

/** Is a page position on the frame? */
export function onFrame(clientX, clientY) {
  const x = clientX * display.dpi, y = clientY * display.dpi;
  return x >= display.ox && y >= display.oy
    && x < display.ox + display.w * display.scale && y < display.oy + display.h * display.scale;
}

/** The browser's full screen, or a window (View > Full screen, Window). */
export function setWindowed(on) {
  display.windowed = on;
  try {
    if (!on && !document.fullscreenElement) document.documentElement.requestFullscreen().catch(() => {});
    if (on && document.fullscreenElement) document.exitFullscreen().catch(() => {});
  } catch (e) { /* no full screen here */ }
}

/** Is the pointer off the game, so that it draws none? */
export function pointerAway() {
  return !display.mouseInside;
}
