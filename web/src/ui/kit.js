// The pieces every dialog is made of, and the stack they are shown on.
//
// A popup (54f6:0000) is a black outline and a two-pixel shadow round a crop
// of MARBLE.PCK, text is drawn centred (7ecb:00d6) or from its left edge, an
// army by 8611:08be with a ring under it, a shield by 8611:0bf7, and a
// dialog's buttons are its BUTTON.DAT controls.
//
// A dialog is an object with draw() and, as it needs them,
// mousepressed(x, y, button), keypressed(key) and textinput(text). kit.push
// puts one on top; the front end sends input to the top one only.

import * as gfx from "../gfx.js";
import * as screen from "../warlords/screen.js";
import * as uidata from "../warlords/uidata.js";

export let G = null;                   // the front end's state, from init

export function init(state) {
  G = state;
  G.modals = G.modals || [];
}

export function push(d) {
  G.modals.push(d);
  return d;
}

export function pop(d) {
  for (let i = G.modals.length - 1; i >= 0; i--) {
    if (G.modals[i] === d) { G.modals.splice(i, 1); return; }
  }
}

export function top() {
  return G.modals[G.modals.length - 1];
}

/** Set a palette colour by its index, 0-15. */
export function setPal(i) {
  gfx.setColor(G.palette[i] || G.palette[0]);
}

/** A one-pixel outline, x..x+w-1 by y..y+h-1, in the current colour. */
export function outline(x, y, w, h) {
  gfx.rectangle("fill", x, y, w, 1);
  gfx.rectangle("fill", x, y + h - 1, w, 1);
  gfx.rectangle("fill", x, y, 1, h);
  gfx.rectangle("fill", x + w - 1, y, 1, h);
}

/** The frame 54f6:0000 gives a popup. */
export function popupFrame(R) {
  gfx.setColor(0, 0, 0);
  outline(R.x - 1, R.y - 1, R.w + 2, R.h + 2);
  gfx.rectangle("fill", R.x + 1, R.y + R.h + 1, R.w + 2, 2);
  gfx.rectangle("fill", R.x + R.w + 1, R.y + 1, 2, R.h + 2);
}

/** A popup with no picture of its own: MARBLE.PCK cropped to the rect. Past
 *  the marble's 480 columns the blit reads on into the next row. */
export function popup(R) {
  popupFrame(R);
  gfx.setColor(1, 1, 1);
  gfx.setScissor(R.x, R.y, R.w, R.h);
  gfx.draw(G.marble, R.x, R.y);
  const [mw, mh] = G.marble.getDimensions();
  if (R.w > mw) {
    gfx.draw(G.marble, gfx.newQuad(0, 1, R.w - mw, Math.min(R.h, mh - 1)), R.x + mw, R.y);
  }
  gfx.setScissor();
}

// 78a8:06ae picks the font by number: 1 is CHANCE36, 2 CHANCE17, else TEXT.
export function font(n) {
  if (n === 1) return G.titleFont;
  if (n === 2) return G.bigFont;
  return G.font;
}

/** Centred on x, with y the top of the line (7ecb:00d6). */
export function centred(f, text, x, y) {
  f.draw(text, x - Math.floor(f.width(text) / 2), y);
}

/** Right-aligned, ending at x (7ecb:0103). */
export function right(f, text, x, y) {
  f.draw(text, x - f.width(text), y);
}

/** A one-pixel bevel (24d0:02e5): colour `a` along the top and left, `b`
 *  along the right and bottom, the higher number drawn second. */
export function bevel(x, y, w, h, a, b) {
  const topLeft = () => {
    setPal(a);
    gfx.rectangle("fill", x, y, w, 1);
    gfx.rectangle("fill", x, y, 1, h);
  };
  const bottomRight = () => {
    setPal(b);
    gfx.rectangle("fill", x + w - 1, y, 1, h);
    gfx.rectangle("fill", x, y + h - 1, w, 1);
  };
  if (b < a) { bottomRight(); topLeft(); } else { topLeft(); bottomRight(); }
}

/** A text field (7ecb:0058). */
export function field(x, y, w, h, text, f) {
  setPal(3);
  gfx.rectangle("fill", x, y, w, h);
  bevel(x, y, w, h, 4, 2);
  gfx.setColor(1, 1, 1);
  if (text != null) (f || G.bigFont).draw(text, x + 3, y + 2);
}

/** An army as 8611:08be draws one: the ring (0 none, 1 grey, 2-9 a side's),
 *  then the army's cell -- of ASHADOW.PCK when `shadow`. */
export function army(typeId, side, x, y, ring, shadow) {
  if (side == null || side === 15) side = 8;
  gfx.setColor(1, 1, 1);
  if (ring && ring > 0) gfx.draw(G.abits, G.ringQuads[ring - 1], x, y);
  if (typeId != null) {
    const img = shadow ? G.shadowImg : G.armyImg[side];
    gfx.draw(img, G.armyQuads[side][typeId % 32], x, y);
  }
}

/** A side's big shield (8611:0bf7): SHIELDS.PCK's 40x40 cell. */
export function shield(side, x, y) {
  if (side == null || side === 15) side = 8;
  if (!G.shieldsImg) return;
  gfx.setColor(1, 1, 1);
  gfx.draw(G.shieldsImg, gfx.newQuad(side * 40, 0, 40, 40), x, y);
}

/** A one-pixel line in palette colour c, end to end (Bresenham). */
export function line(c, x0, y0, x1, y1) {
  setPal(c);
  const dx = Math.abs(x1 - x0), dy = -Math.abs(y1 - y0);
  const sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
  let err = dx + dy;
  for (;;) {
    gfx.rectangle("fill", x0, y0, 1, 1);
    if (x0 === x1 && y0 === y1) break;
    const e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

/** The way to a place on a strategic map drawn at (mx, my) (828e:0a3f). */
export function mapTarget(mx, my, hx, hy, tx, ty) {
  gfx.setScissor(mx, my, 224, 312);
  line(8, mx + hx * 2 - 2, my + hy * 2 - 2, mx + tx * 2 - 2, my + ty * 2 - 2);
  setPal(8);
  const bx = mx + tx * 2, by = my + ty * 2 - 1;
  gfx.rectangle("fill", bx, by, 4, 5);
  setPal(0);
  gfx.rectangle("fill", bx - 1, by, 6, 1);
  gfx.rectangle("fill", bx, by + 5, 4, 1);
  gfx.rectangle("fill", bx - 1, by, 1, 5);
  gfx.rectangle("fill", bx + 4, by, 1, 5);
  setPal(8);
  outline(mx + tx * 2 - 2, my + ty * 2 - 2, 8, 8);
  gfx.setScissor();
}

/** A dialog's BUTTON.DAT controls, each in its own state. */
export function view(dialogId) {
  return screen.dialog(G.screen, dialogId);
}

export function drawControls(v, hidden) {
  if (hidden) {
    const keep = v.dialog.controls;
    v.dialog.controls = keep.filter((c) => !hidden[c.id]);
    screen.drawDialogControls(G.screen, v);
    v.dialog.controls = keep;
  } else {
    screen.drawDialogControls(G.screen, v);
  }
}

/** The live control under a point: not disabled and not hidden. */
export function controlAt(v, x, y, hidden) {
  for (const c of v.dialog.controls) {
    if (c.w > 0 && c.h > 0 && !(hidden && hidden[c.id])
        && x >= c.x && x < c.x + c.w && y >= c.y && y < c.y + c.h) {
      if (v.state[c.id] === uidata.DISABLED) return null;
      return c;
    }
  }
  return null;
}

export function control(v, id) {
  return screen.dialogControl(v, id);
}

/** A string from STRING.DAT, numbered as get_string numbers them. */
export function text(group, i) {
  return uidata.text(G.screen.ui, group, i);
}
