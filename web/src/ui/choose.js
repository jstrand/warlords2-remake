// The game's list chooser (796c:0000): pick one of a list of names.
//
// Popup 3, (96, 50) 200x200, with dialog 2: the title in font 2 centred on
// (196, 52), a colour-3 box sunk with a (4, 2) bevel, five names 20 apart,
// the chosen one in colour 15 and the rest in colour 2. Controls 114-118
// choose a row, 121/122 scroll a row and 119/120 five, hidden when the list
// fits; OK (123) hands the chosen entry over, Cancel (124) null.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as uidata from "../warlords/uidata.js";

const R = { x: 96, y: 50, w: 200, h: 200 };       // popup 3
const BOX = { x: 106, y: 70, w: 180, h: 110 };    // 4125:15a6
const DIALOG = 2;
const ROW = 114, PAGE_UP = 119, PAGE_DOWN = 120, UP = 121, DOWN = 122, OK = 123, CANCEL = 124;
const ROWS = 5;

/** Open the chooser on `list` (entries with a `name`), entry `start` (from
 *  0) chosen; `after(entry or null)` runs when it closes. */
export function open(title, list, start, after) {
  const d = { view: kit.view(DIALOG), top: 0, cur: start || 0 };
  const n = list.length;
  while (d.cur >= ROWS) { d.cur--; d.top++; }

  const refresh = () => {
    const st = d.view.state;
    for (let i = 0; i < ROWS; i++) st[ROW + i] = uidata.NORMAL;
    st[OK] = uidata.NORMAL; st[CANCEL] = uidata.NORMAL;
    d.hidden = {};
    if (n > ROWS) {
      const up = d.top > 0 ? uidata.NORMAL : uidata.DISABLED;
      const down = d.top + ROWS - 1 < n - 1 ? uidata.NORMAL : uidata.DISABLED;
      st[PAGE_UP] = up; st[UP] = up; st[PAGE_DOWN] = down; st[DOWN] = down;
    } else {
      for (const id of [PAGE_UP, PAGE_DOWN, UP, DOWN]) d.hidden[id] = true;
    }
  };

  const close = (entry) => {
    kit.pop(d);
    if (after) after(entry);
  };

  d.draw = () => {
    kit.popup(R);
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    kit.centred(f, title, 196, 52);
    kit.setPal(3);
    gfx.rectangle("fill", BOX.x, BOX.y, BOX.w, BOX.h);
    kit.bevel(BOX.x, BOX.y, BOX.w, BOX.h, 4, 2);
    gfx.setColor(1, 1, 1);
    for (let i = 0; i < ROWS; i++) {
      const e = list[d.top + i];
      if (e) f.colours(i === d.cur ? 15 : 2, 0).draw(e.name || "", 112, 74 + 20 * i);
    }
    kit.drawControls(d.view, d.hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (!c) return;
    const id = c.id;
    if (id >= ROW && id < ROW + ROWS) {
      if (list[d.top + id - ROW]) d.cur = id - ROW;
    } else if (id === UP) {
      d.top--; d.cur = Math.min(d.cur + 1, ROWS - 1);
    } else if (id === DOWN) {
      d.top++; d.cur = Math.max(d.cur - 1, 0);
    } else if (id === PAGE_UP) {
      d.top -= Math.min(ROWS, d.top);
    } else if (id === PAGE_DOWN) {
      d.top += Math.min(ROWS, n - (d.top + ROWS - 1) - 1);
    } else if (id === OK) {
      return close(list[d.top + d.cur] || null);
    } else if (id === CANCEL) {
      return close(null);
    }
    refresh();
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter") close(list[d.top + d.cur] || null);
    else if (key === "escape") close(null);
  };

  refresh();
  return kit.push(d);
}
