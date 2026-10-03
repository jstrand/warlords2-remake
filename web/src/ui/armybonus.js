// View > Army Bonus (89e0:1e3b): every army type, in the side's fight order,
// with its strength, moves, way of moving and bonus.
//
// Popup 0, (80, 60) 480x320 (89e0:1fd2): "Army Bonus" in font 1, a box raised
// with a (4, 2) bevel and outlined in black, STACK.PCK's column heads, and
// six rows 30 apart from y = 148: the army on a ring of the side's colour,
// name, strength, moves, how it moves, and its bonus (89e0:1a07, group 163).
// Dialog 34: Done (490), 491/492 a row up and down and 493/494 six. Of the
// 29 places only the first 27 are shown.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as armytype from "../warlords/armytype.js";
import * as combat from "../warlords/combat.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 320 };      // popup 0
const DIALOG = 34, DONE = 490, UP = 491, DOWN = 492, PAGE_UP = 493, PAGE_DOWN = 494;
const ROWS = 6, PLACES = 27;
const S = 0xa3;

/** 89e0:1a07: the first of the type's bonuses, as group 163 words it. Given
 *  the army itself, a boat at sea says so, and a hero gives its command. */
export function bonusText(t, army) {
  const b = t.bonus;
  const say = (i, v) => fmt(kit.text(S, i), v || 0);
  if (army && army.atSea) return say(1);
  if (army && army.type === armytype.HERO) {
    const s = Math.min(9, (army.strength || 0) + combat.battleItems(army));
    return say(2, Math.min(6, combat.HERO_TABLE[s] + combat.commandItems(army)));
  }
  if (b[52] === 2) return say(3);
  if (b[52] === 3) return say(4);
  if ((b[48] || 0) !== 0) return say(5, b[42]);
  if ((b[46] || 0) !== 0 && (b[44] || 0) !== 0 && (b[42] || 0) !== 0 && (b[40] || 0) !== 0) return say(16, b[40]);
  if (b[52] === 1) return say(6);
  const order = [[50, 7], [46, 8], [44, 9], [42, 10], [40, 11], [38, 12], [36, 13], [34, 14], [32, 15]];
  for (const o of order) if ((b[o[0]] || 0) !== 0) return say(o[1], b[o[0]]);
  return say(0);
}

export function open() {
  const G = kit.G;
  const side = G.player;
  const row = G.g.map.fightOrder[side.index];
  const byRank = {};
  for (let t = 0; t <= 28; t++) if (row[t] != null) byRank[row[t]] = t;
  const d = { view: kit.view(DIALOG), top: 0 };

  const refresh = () => {
    const st = d.view.state;
    st[DONE] = uidata.NORMAL;
    const up = d.top > 0 ? uidata.NORMAL : uidata.DISABLED;
    const down = d.top + ROWS < PLACES ? uidata.NORMAL : uidata.DISABLED;
    st[UP] = up; st[PAGE_UP] = up; st[DOWN] = down; st[PAGE_DOWN] = down;
  };

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(0xa4, 0), 320, 62);
    kit.bevel(124, 146, 434, 186, 4, 2);
    kit.setPal(0);
    kit.outline(125, 147, 432, 184);
    const stack = G.screen.art_for(46);
    if (stack) {
      gfx.setColor(1, 1, 1);
      gfx.draw(stack.image, gfx.newQuad(188, 0, 216, 22), 256, 125);
    }
    const f = kit.font(2);
    for (let i = 0; i < ROWS; i++) {
      const tid = byRank[d.top + i];
      const t = tid != null ? G.g.types.byId[tid] : null;
      if (t) {
        const y = 148 + 30 * i;
        kit.army(tid, side.index, 128, y, side.index + 2);
        gfx.setColor(1, 1, 1);
        f.draw(t.name || "", 168, y + 5);
        f.draw(fmt("%d", t.strength || 0), 280, y + 5);
        f.draw(fmt("%d", t.move || 0), 328, y + 5);
        let src = null;
        if (t.flies) src = 184;
        else if (t.woodsMove && t.hillsMove) src = 216;
        else if (t.woodsMove) src = 248;
        else if (t.hillsMove) src = 152;
        if (src !== null) {
          gfx.setColor(1, 1, 1);
          gfx.draw(G.abits, gfx.newQuad(src, 30, 32, 10), 352, y + 8);
        }
        gfx.setColor(1, 1, 1);
        f.draw(bonusText(t), 400, y + 6);
      }
    }
    kit.drawControls(d.view);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === DONE) return kit.pop(d);
    else if (c.id === UP) d.top--;
    else if (c.id === DOWN) d.top++;
    else if (c.id === PAGE_UP) d.top = Math.max(0, d.top - ROWS);
    else if (c.id === PAGE_DOWN) d.top = Math.min(PLACES - ROWS, d.top + ROWS);
    refresh();
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };

  refresh();
  return kit.push(d);
}
