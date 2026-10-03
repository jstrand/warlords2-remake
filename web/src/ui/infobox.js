// What the right button shows off the map: a box that stays up while the
// button is held, and goes when it comes up (740d:11cf).
//
// Two boards, blitted through their masks:
//   POPUP.PCK (bitmap 29), 256x75: two lines -- the first in colour 7, the
//     second in white, centred on the box's middle less 8, at y + 11 and
//     y + 35 (740d:1201, 740d:1158).
//   POPUP3.PCK (bitmap 40), 240x128: an army (ui_army_info, 740d:0626) or an
//     army type (ui_army_type_info, 740d:032a): the name, the army on the
//     grey ring with a non-hero's medals round it, four numbers, and how the
//     type moves.
//
// A right-button press on a control looks it up in HELP\WARLORD2.HLP
// (54bd:0000): when the record's sub-id is 0 its title and description are
// the two lines; otherwise the sub-id says what the control stands for and
// the dialog it is on answers through `d.info(sub, sx, sy)`.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as pck from "../warlords/pck.js";
import * as armytype from "../warlords/armytype.js";
import * as vfs from "../vfs.js";
import { u16 as rd16, cstr } from "../warlords/bytes.js";
import { fmt } from "../util.js";

export const LINES = { bitmap: 29, file: "POPUP.PCK", w: 256, h: 75, key: 1 };
export const ARMY = { bitmap: 40, file: "POPUP3.PCK", w: 240, h: 128, key: 10 };
export const POPUP2 = { bitmap: 36, file: "POPUP2.PCK", w: 256, h: 75, key: 1 };

const MOVE_ICON = { fly: [184, 30], both: [216, 30], woods: [248, 30], hills: [152, 30] };
const MEDALS = [[424, 22], [432, 22], [440, 22], [456, 32]];
const MEDAL_AT = [[128, 36], [88, 36], [128, 45], [88, 45]];

/** A board, loaded once and keyed on its mask colour. */
export function board(which) {
  const G = kit.G;
  G.tileBoards = G.tileBoards || {};
  if (!G.tileBoards[which.bitmap]) {
    const [img, w, h] = pck.toImage(G.dataDir + "/PICS/" + which.file, G.palette, which.key);
    G.tileBoards[which.bitmap] = { image: img, w, h };
  }
  return G.tileBoards[which.bitmap];
}

/** Where a board goes for a point on the screen (740d:131a, 740d:1486):
 *  centred on it, x on the byte grid, kept on the screen; in the dialogs'
 *  frame, where it is drawn. */
export function place(which, sx, sy) {
  const L = kit.G.layout;
  const hw = Math.floor(which.w / 2), hh = Math.floor(which.h / 2);
  let cx = Math.floor((Math.floor(sx) + 4) / 8) * 8;
  cx = Math.max(hw, Math.min(L.w - hw, cx));
  const cy = Math.max(hh, Math.min(L.h - 2 - hh, Math.floor(sy)));
  return [cx - hw - L.dialog.x, cy - hh - L.dialog.y];
}

function show(which, sx, sy, draw, what) {
  const [bx, by] = place(which, sx, sy);
  const d = { infobox: what };
  d.draw = () => {
    const b = board(which);
    gfx.setColor(1, 1, 1);
    if (b) gfx.draw(b.image, gfx.newQuad(0, 0, which.w, which.h), bx, by);
    gfx.setColor(1, 1, 1);
    draw(bx, by);
  };
  d.mousereleased = () => kit.pop(d);
  d.mousepressed = () => kit.pop(d);
  d.keypressed = () => kit.pop(d);
  return kit.push(d);
}

/** Two lines (740d:1201 and 740d:1158): a title in colour 7, then white. */
export function lines(sx, sy, title, text) {
  const f = kit.font(2);
  return show(LINES, sx, sy, (x, y) => {
    kit.centred(f.colours(7, 0), title || "", x + LINES.w / 2 - 8, y + 11);
    kit.centred(f, text || "", x + LINES.w / 2 - 8, y + 35);
  }, { title, text });
}

function armyBoard(x, y, name, typeId, side, medals) {
  const G = kit.G;
  const f = kit.font(2);
  kit.centred(f, name || "", x + 112, y + 8);
  kit.army(typeId, side, x + 96, y + 31, 1);
  const abits = (src, w, h, ax, ay) => {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.abits, gfx.newQuad(src[0], src[1], w, h), ax, ay);
  };
  for (let m = 0; m < Math.min(4, medals || 0); m++) abits(MEDALS[m], 8, 8, x + MEDAL_AT[m][0], y + MEDAL_AT[m][1]);
  const t = G.g.types.byId[typeId];
  let src = null;
  if (t && t.flies) src = MOVE_ICON.fly;
  else if (t && t.woodsMove && t.hillsMove) src = MOVE_ICON.both;
  else if (t && t.woodsMove) src = MOVE_ICON.woods;
  else if (t && t.hillsMove) src = MOVE_ICON.hills;
  if (src) abits(src, 32, 10, x + 96, y + 103);
}

function numbers(x, y, a, b, c, d) {
  const f = kit.font(2);
  gfx.setColor(1, 1, 1);
  kit.right(f, a, x + 104, y + 66);
  f.draw(b, x + 120, y + 66);
  kit.right(f, c, x + 104, y + 86);
  f.draw(d, x + 120, y + 86);
}

/** An army (ui_army_info, 740d:0626). */
export function army(sx, sy, a) {
  const G = kit.G;
  const t = G.g.types.byId[a.type] || {};
  const hero = a.type === armytype.HERO;
  return show(ARMY, sx, sy, (x, y) => {
    armyBoard(x, y, hero ? a.name : t.name, a.type, G.player.index, !hero ? a.medals : 0);
    numbers(x, y, fmt("Strength: %d", a.strength || 0),     // 4125:10a8
                  fmt("Movement: %d", a.maxMoves || 0),     // 4125:10b5
                  fmt("Remain: %d", a.moves || 0),          // 4125:10c2
                  fmt("Upkeep: %d", a.upkeep || 0));        // 4125:10cd
  }, { army: a });
}

/** An army type (ui_army_type_info, 740d:032a). */
export function armyType(sx, sy, typeId, s, side) {
  const G = kit.G;
  const t = G.g.types.byId[typeId] || {};
  return show(ARMY, sx, sy, (x, y) => {
    armyBoard(x, y, t.name, typeId, side ?? G.player.index, 0);
    numbers(x, y, fmt("Strength: %d", s.strength || 0),     // 4125:107c
                  fmt("Movement: %d", s.move || 0),         // 4125:1089
                  fmt("Time: %d", s.time || 0),             // 4125:1096
                  fmt("Cost: %d", s.cost || 0));            // 4125:109f
  }, { type: typeId });
}

/** HELP\WARLORD2.HLP (docs/formats/hlp.md): a u16 count, then 104-byte
 *  records -- u16 id, u16 sub-id, title and description, 50 bytes each. Only
 *  the first record with an id counts. */
export function helpFile() {
  const G = kit.G;
  if (G.helpRecords) return G.helpRecords;
  const out = {};
  const s = vfs.read(G.dataDir + "/HELP/WARLORD2.HLP");
  if (s) {
    for (let i = 0; i < rd16(s, 0); i++) {
      const at = 2 + i * 104;
      if (at + 104 > s.length) break;
      const id = rd16(s, at);
      if (!out[id]) out[id] = { sub: rd16(s, at + 2), title: cstr(s, at + 4, 50), text: cstr(s, at + 54, 50) };
    }
  }
  G.helpRecords = out;
  return out;
}

/** The help for control `id` at a point on the screen, asking `d` about the
 *  controls that stand for something. True when a box went up. */
export function control(sx, sy, id, d) {
  const rec = helpFile()[id];
  if (!rec) return false;
  if (rec.sub === 0) {
    lines(sx, sy, rec.title, rec.text);
    return true;
  }
  const sub = rec.sub;
  if (sub >= 33 && sub <= 36) {
    // the configurable buttons (545c:03f8): the menu item each one runs
    const ui = kit.G.screen.ui;
    const item = ui.shortcuts ? ui.shortcuts[sub - 33] : null;
    const name = (item != null && ui.shortcutNames && ui.shortcutNames[item]) || "";
    lines(sx, sy, "- User-Defined Button -", name);            // 4125:05e6
    return true;
  }
  if (d && d.info) return !!d.info(sub, sx, sy);
  return false;
}

/** The control on a dialog's view under a point, disabled ones included
 *  (18a9:0756 walks back to front), but not those taken off its face. */
export function controlAt(view, x, y, hidden) {
  let found = null;
  for (const c of view.dialog.controls) {
    if (c.w > 0 && c.h > 0 && !(hidden && hidden[c.id])
        && x >= c.x && x < c.x + c.w && y >= c.y && y < c.y + c.h) {
      found = c;
    }
  }
  return found;
}
