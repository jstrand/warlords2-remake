// The original's one text-entry dialog, used for renaming a city and for
// anything else that asks for a line of text (7b4c:0000).
//
// Popup 1, (160, 90) 320x200: the title in font 1 centred on x = 320 at
// y = 92, the prompt in font 2, and the field at (184, 197) 256x20 over
// dialog 5: OK (189), Cancel (190) and the field's own hit area (191).
// Clicking the field starts an edit from an EMPTY line (7b4c:03a6); a
// character from 0x20 to 0x7a is taken while the line is short and narrow
// enough; Backspace takes one back; Enter keeps it, Escape puts the old text
// back. The cursor is a "`", the font's own, blinking between colours 15 and
// 9. The same dialog asks yes-or-no questions (7b4c:0088): no field.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as uidata from "../warlords/uidata.js";
import { now } from "../util.js";

const R = { x: 160, y: 90, w: 320, h: 200 };     // popup 1
const DIALOG = 5;
const OK = 189, CANCEL = 190, FIELD = 191;
const TITLE_Y = 92;
// Where the prompt goes, by how many lines there are (4125:2316). The
// text-entry form counts two more lines than it has, for the field.
const ROWS = { 1: [150], 2: [150, 173], 3: [150, 173, 196], 4: [140, 163, 186, 209] };
const BOX = { x: 184, y: 197, w: 256, h: 20 };
const CURSOR = "`";
const BLINK = 0.25;

/** A line being typed into a field (7b4c:03a6). key() answers "keep" for
 *  Enter, "undo" for Escape. */
export function editor(maxChars, maxWidth) {
  const e = { text: "" };
  e.key = (key) => {
    if (key === "return" || key === "kpenter") return "keep";
    if (key === "escape") return "undo";
    if (key === "backspace") e.text = e.text.slice(0, -1);
    return null;
  };
  e.input = (t) => {
    for (const ch of t) {
      const b = ch.charCodeAt(0);
      if (b >= 0x20 && b < 0x7b && e.text.length < maxChars - 1 && kit.font(2).width(e.text) < maxWidth) {
        e.text += ch;
      }
    }
  };
  e.drawCursor = (x, y) => {
    const f = kit.font(2);
    if (Math.floor(now() / BLINK) % 2 === 0) kit.setPal(9); else gfx.setColor(1, 1, 1);
    f.draw(CURSOR, x + f.width(e.text) + 5, y + 2);
  };
  return e;
}

/** Ask for a line of text, or a yes-or-no question (opts.confirm).
 *    title, lines, text, maxChars (15), maxWidth (232), cancel (false greys
 *    it), ok(text), cancelled() */
export function open(opts) {
  const d = { view: kit.view(DIALOG), text: opts.text || "", editing: null };
  const maxChars = opts.maxChars || 15;
  const maxWidth = opts.maxWidth || 232;
  d.view.state[OK] = uidata.NORMAL;
  d.view.state[CANCEL] = opts.cancel === false ? uidata.DISABLED : uidata.NORMAL;

  const finish = (ok) => {
    kit.pop(d);
    if (ok) { if (opts.ok) opts.ok(d.text); } else if (opts.cancelled) opts.cancelled();
  };

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), opts.title || "", 320, TITLE_Y);
    const lines = opts.lines || [];
    const ys = ROWS[opts.confirm ? lines.length : lines.length + 2];
    if (ys) lines.forEach((line, i) => kit.centred(kit.font(2), line, 320, ys[i]));
    if (opts.confirm) { kit.drawControls(d.view, { [FIELD]: true }); return; }
    kit.field(BOX.x, BOX.y, BOX.w, BOX.h, d.editing ? d.editing.text : d.text, kit.font(2));
    if (d.editing) d.editing.drawCursor(BOX.x, BOX.y);
    kit.drawControls(d.view);
  };

  d.mousepressed = (x, y) => {
    if (d.editing) { d.text = d.editing.text; d.editing = null; }
    const c = kit.controlAt(d.view, x, y, opts.confirm ? { [FIELD]: true } : null);
    if (!c) return;
    if (c.id === OK) finish(true);
    else if (c.id === CANCEL) finish(false);
    else if (c.id === FIELD) d.editing = editor(maxChars, maxWidth);
  };

  d.keypressed = (key) => {
    if (d.editing) {
      const r = d.editing.key(key);
      if (r === "keep") { d.text = d.editing.text; d.editing = null; } else if (r === "undo") d.editing = null;
      return;
    }
    if (key === "return" || key === "kpenter") finish(true);
    else if (key === "escape" && d.view.state[CANCEL] !== uidata.DISABLED) finish(false);
  };

  d.textinput = (t) => { if (d.editing) d.editing.input(t); };
  return kit.push(d);
}
