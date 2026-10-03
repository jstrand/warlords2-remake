// Order > Fight Order (6a89:0de1): the order the side's armies fight in.
//
// Popup 11, (80, 60) 480x350 (6a89:0e4a): "Fighting Order" between the
// side's shields, "Order of combat for %s", three lines of help, and the 27
// places four to a row, the chosen one on the side's ring. Dialog 26: OK
// (425), Cancel (426), Default (427, the neutral row back), 428/429 move the
// chosen army a place earlier or later, and 430-456 the places. A click on a
// place chooses it; on the chosen one, lets it go; on another, swaps the two
// (6a89:1379). Cancel puts back the copy taken on opening (6a89:111c).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as infobox from "./infobox.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 350 };      // popup 11
const DIALOG = 26;
const OK = 425, CANCEL = 426, DEFAULT = 427, EARLIER = 428, LATER = 429, PLACE = 430;
const PLACES = 27;
const S = 0x7f;
const NEUTRAL = 8;

export function open() {
  const G = kit.G;
  const side = G.player;
  const row = G.g.map.fightOrder[side.index];
  const backup = row.slice();
  const d = { view: kit.view(DIALOG), chosen: -1 };

  const typeAt = (rank) => {
    for (let t = 0; t <= 28; t++) if (row[t] === rank) return t;
    return null;
  };
  const swap = (a, b) => {
    const ta = typeAt(a), tb = typeAt(b);
    if (ta != null) row[ta] = b;
    if (tb != null) row[tb] = a;
  };

  const refresh = () => {
    const st = d.view.state;
    st[OK] = uidata.NORMAL; st[CANCEL] = uidata.NORMAL; st[DEFAULT] = uidata.NORMAL;
    const c = d.chosen;
    st[EARLIER] = c > 0 ? uidata.NORMAL : uidata.DISABLED;
    st[LATER] = (c >= 0 && c < PLACES - 1) ? uidata.NORMAL : uidata.DISABLED;
    for (let i = 0; i < PLACES; i++) st[PLACE + i] = uidata.NORMAL;
  };

  const close = (keep) => {
    if (!keep) backup.forEach((r, t) => { row[t] = r; });
    kit.pop(d);
  };

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(S, 0), 320, 64);
    kit.shield(side.index, 88, 64);
    kit.shield(side.index, 512, 64);
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    kit.centred(f, fmt(kit.text(S, 1), side.name || ""), 320, 104);
    kit.centred(f, kit.text(S, 2), 320, 348);
    kit.centred(f, kit.text(S, 3), 320, 368);
    kit.centred(f, kit.text(S, 4), 320, 388);
    for (let i = 0; i < PLACES; i++) {
      const x = 120 * (i % 4), y = 31 * Math.floor(i / 4);
      const t = typeAt(i);
      if (t != null) kit.army(t, side.index, 88 + x, 128 + y, i === d.chosen ? side.index + 2 : 1);
      gfx.setColor(1, 1, 1);
      f.draw(fmt("%d.", i + 1), 128 + x, 134 + y);       // 4125:0d5c
    }
    const hidden = {};
    for (let i = 0; i < PLACES; i++) hidden[PLACE + i] = true;
    kit.drawControls(d.view, hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    const id = c.id;
    if (id === OK) return close(true);
    else if (id === CANCEL) return close(false);
    else if (id === DEFAULT) {
      G.g.map.fightOrder[NEUTRAL].forEach((r, t) => { row[t] = r; });
      d.chosen = -1;
    } else if (id === EARLIER && d.chosen > 0) {
      swap(d.chosen, d.chosen - 1);
      d.chosen--;
    } else if (id === LATER && d.chosen >= 0 && d.chosen < PLACES - 1) {
      swap(d.chosen, d.chosen + 1);
      d.chosen++;
    } else if (id >= PLACE && id < PLACE + PLACES) {
      const i = id - PLACE;
      if (d.chosen === i) d.chosen = -1;
      else if (d.chosen < 0) d.chosen = i;
      else { swap(d.chosen, i); d.chosen = -1; }
    }
    refresh();
  };

  // the right button on a place (sub-ids 45-71, 6a89:1475)
  d.info = (sub, sx, sy) => {
    const t = typeAt(sub - 45);
    const rec = t != null ? G.g.types.byId[t] : null;
    if (!rec) return false;
    infobox.armyType(sx, sy, t, rec, side.index);
    return true;
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter") close(true);
    else if (key === "escape") close(false);
  };

  refresh();
  return kit.push(d);
}
