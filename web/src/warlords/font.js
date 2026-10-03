// The original's proportional fonts: TEXT, CHANCE17 and CHANCE36.
//
// A .FNT is a .PCK image -- a sheet of glyphs -- and the .FIN beside it holds
// the metrics. docs/formats/font.md. Glyphs are packed left to right into rows
// `lineHeight` tall, each taking a slot rounded up to a multiple of 8.

import * as vfs from "../vfs.js";
import * as gfx from "../gfx.js";
import * as pck from "./pck.js";

function readMetrics(path) {
  const s = vfs.read(path);
  if (!s) throw new Error("cannot open font metrics: " + path);
  const m = {
    count: s[0],
    first: s[1],
    sheetWidth: s[2] * 256 + s[3],   // big-endian, unlike everything else
    lineHeight: s[4],
    baseline: s[5],
    ink: {}, width: {},
  };
  // how wide each glyph's art is, then how far the pen moves on (78a8:03ea)
  for (let i = 0; i < m.count; i++) {
    m.ink[m.first + i] = s[11 + i];
    m.width[m.first + i] = s[11 + m.count + i];
  }
  return m;
}

/** Load a font: { draw, width, lineHeight, colours(glyph, outline), ... }.
 *  Text is a string of byte characters, as the game's files give it. */
export function load(dataDir, name, palette, keyIndex) {
  const m = readMetrics(dataDir + "/" + name + ".FIN");
  const [img, w, h, px] = pck.toImage(dataDir + "/" + name + ".FNT", palette, keyIndex);

  const quad = {};
  let x = 0, row = 0;
  for (let i = 0; i <= m.count - 2; i++) {      // the last entry has no glyph
    const c = m.first + 1 + i;
    const gw = m.ink[c];
    const slot = Math.ceil(gw / 8) * 8;
    if (x + slot > m.sheetWidth) { x = 0; row++; }
    if (gw > 0) quad[c] = gfx.newQuad(x, row * m.lineHeight, gw, m.lineHeight);
    x += slot;
  }

  const self = { lineHeight: m.lineHeight, baseline: m.baseline, image: img, metrics: m };

  self.width = (text) => {
    let n = 0;
    text = String(text);
    for (let i = 0; i < text.length; i++) n += m.width[text.charCodeAt(i)] || 0;
    return n;
  };

  const drawWith = (image, text, x0, y0) => {
    let cx = x0;
    text = String(text);
    for (let i = 0; i < text.length; i++) {
      const c = text.charCodeAt(i);
      if (quad[c]) gfx.draw(image, quad[c], cx, y0);
      cx += m.width[c] || 0;
    }
    return cx - x0;
  };

  self.draw = (text, x0, y0) => drawWith(img, text, x0, y0);

  // The same font in other colours, as 78a8:06ae asks for it: the sheet's
  // glyph colour (15) becomes `glyph` and its outline (0) `outline`.
  const variants = {};
  self.colours = (glyph, outline) => {
    const k = glyph * 16 + outline;
    if (!variants[k]) {
      const vimg = (glyph === 15 && outline === 0) ? img
        : pck.imageFromPixels(w, h, px, palette, { 15: glyph, 0: outline },
                              keyIndex != null ? { [keyIndex]: true } : null);
      const v = Object.assign({}, self);
      v.draw = (text, x0, y0) => drawWith(vimg, text, x0, y0);
      variants[k] = v;
    }
    return variants[k];
  };

  /** Draw centred in a rect, the way the original centres a control's text. */
  self.drawCentred = (text, rx, ry, rw, rh) => {
    self.draw(text, rx + Math.floor((rw - self.width(text)) / 2),
              ry + Math.floor((rh - m.lineHeight) / 2) + 1);
  };
  return self;
}
