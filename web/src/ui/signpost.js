// Order > Signpost (540d:01a4): rewrite the sign the selected stack stands on.
//
// Only on a tile of terrain 9 that has a sign. Popup 1, (160, 90) 320x200:
// "A Signpost!" in font 1, two lines of instruction in font 2, and the
// sign's two lines in fields at (200, 190) and (200, 215), 240x22, over
// dialog 24: Done (421) and the two fields' hit areas (422, 423). Clicking a
// field types a new line into it, up to 29 characters and 216 pixels.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as input from "./input.js";
import * as game from "../warlords/game.js";
import * as move from "../warlords/move.js";
import * as scn from "../warlords/scn.js";
import * as uidata from "../warlords/uidata.js";

const R = { x: 160, y: 90, w: 320, h: 200 };     // popup 1
const DIALOG = 24, DONE = 421, LINE1 = 422, LINE2 = 423;
const FIELDS = [{ x: 200, y: 190 }, { x: 200, y: 215 }];
const FIELD_W = 240, FIELD_H = 22;
const MAX_CHARS = 30, MAX_WIDTH = 216;

/** Open on the sign under `stack`, or do nothing if there is none. */
export function open(stack) {
  const G = kit.G;
  const a = stack && stack[0];
  if (!a || scn.terrainAt(G.g.map, a.x, a.y) !== move.TOWER) return null;
  const sign = game.signAt(G.g, a.x, a.y);
  if (!sign) return null;

  const d = { view: kit.view(DIALOG) };
  for (const id of [DONE, LINE1, LINE2]) d.view.state[id] = uidata.NORMAL;

  const keep = () => {
    if (d.editing) { sign.lines[d.line] = d.editing.text; d.editing = null; }
  };

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), "A Signpost!", 320, 94);          // 4125:03eb
    kit.centred(kit.font(2), "Type the new message for", 320, 140);
    kit.centred(kit.font(2), "this signpost!", 320, 160);
    FIELDS.forEach((p, i) => {
      const text = (d.editing && d.line === i) ? d.editing.text : sign.lines[i];
      kit.field(p.x, p.y, FIELD_W, FIELD_H, text, kit.font(2));
      if (d.editing && d.line === i) d.editing.drawCursor(p.x, p.y);
    });
    kit.drawControls(d.view, { [LINE1]: true, [LINE2]: true });
  };

  d.mousepressed = (x, y) => {
    keep();
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === DONE) kit.pop(d);
    else if (c.id === LINE1 || c.id === LINE2) {
      d.line = c.id - LINE1;
      d.editing = input.editor(MAX_CHARS, MAX_WIDTH);
    }
  };

  d.keypressed = (key) => {
    if (d.editing) {
      const r = d.editing.key(key);
      if (r === "keep") keep(); else if (r === "undo") d.editing = null;
      return;
    }
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };

  d.textinput = (t) => { if (d.editing) d.editing.input(t); };
  return kit.push(d);
}
