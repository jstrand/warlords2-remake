// Game > Shortcuts (545c:0000): which menu items the four configurable
// buttons carry.
//
// Popup 0, (80, 60) 480x320, dialog 14 (545c:014a): the 21 items of UDB.DAT
// three to a row, each its button and its name, lit when it is on the slot
// being set; and the four slots, the one being set outlined in colour 9.
// 316-319 pick a slot, 295-315 put an item on it, OK (294) keeps them. The
// web port keeps the choice in the browser.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as uidata from "../warlords/uidata.js";
import * as vfs from "../vfs.js";
import { u16, cstr } from "../warlords/bytes.js";

const R = { x: 80, y: 60, w: 480, h: 320 };      // popup 0
const DIALOG = 14, OK = 294, ITEM = 295, SLOT = 316;
const BLANK = [256, 84];                         // 4125:049e

function readItems(dataDir) {
  const s = vfs.read(dataDir + "/UDB/UDB.DAT");
  if (!s) return [];
  const out = [];
  for (let i = 0; i < u16(s, 0); i++) {
    const o = 2 + 68 * i;
    if (o + 68 > s.length) break;
    out.push({
      id: u16(s, o), name: cstr(s, o + 2, 50),
      x: u16(s, o + 52), y: u16(s, o + 54), w: u16(s, o + 56), h: u16(s, o + 58),
    });
  }
  return out;
}

export function open() {
  const G = kit.G;
  const ui = G.screen.ui;
  const items = readItems(G.dataDir);
  const d = { view: kit.view(DIALOG), slot: 0 };
  const art = G.screen.art_for(uidata.SHORTCUT_BITMAP);

  const itemIndex = (id) => items.findIndex((it) => it.id === id);

  const refresh = () => {
    const st = d.view.state;
    st[OK] = uidata.NORMAL;
    for (let i = 0; i <= 20; i++) st[ITEM + i] = uidata.NORMAL;
    for (let i = 0; i <= 3; i++) st[SLOT + i] = uidata.NORMAL;
  };

  const button = (x, y, sx, sy) => {
    if (!art) return;
    gfx.setColor(1, 1, 1);
    gfx.draw(art.image, gfx.newQuad(sx, sy, 32, 29), x, y);
  };

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), "Menu Shortcuts", 320, 63);          // 4125:055c
    const f = kit.font(2);
    const current = itemIndex(ui.shortcuts[d.slot]);
    items.forEach((it, i) => {
      const x = 96 + 152 * (i % 3);
      const y = 100 + 30 * Math.floor(i / 3);
      button(x, y, it.x + (i === current ? it.w / 2 : 0), it.y);
      gfx.setColor(1, 1, 1);
      f.draw(it.name, x + 40, y + 5);
    });
    kit.centred(f, "Choose 4 buttons for shortcuts", 320, 320);   // 4125:056b
    for (let s = 0; s <= 3; s++) {
      const x = 96 + 40 * s;
      const it = items[itemIndex(ui.shortcuts[s])];
      if (it) button(x, 342, it.x, it.y); else button(x, 342, BLANK[0], BLANK[1]);
      if (s === d.slot) {
        kit.setPal(9);
        kit.outline(x, 342, 32, 29);
      }
    }
    const hidden = {};
    for (let i = 0; i <= 20; i++) hidden[ITEM + i] = true;
    for (let i = 0; i <= 3; i++) hidden[SLOT + i] = true;
    kit.drawControls(d.view, hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === OK) {
      try { localStorage.setItem("w2:shortcuts", JSON.stringify(ui.shortcuts)); } catch (e) { /* none */ }
      return kit.pop(d);
    }
    if (c.id >= SLOT && c.id < SLOT + 4) d.slot = c.id - SLOT;
    else if (c.id >= ITEM && c.id < ITEM + 21) {
      const it = items[c.id - ITEM];
      if (it) {
        ui.shortcuts[d.slot] = it.id;
        ui.shortcutNames[it.id] = ui.shortcutNames[it.id] || it.name;
      }
    }
    refresh();
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };

  refresh();
  return kit.push(d);
}
