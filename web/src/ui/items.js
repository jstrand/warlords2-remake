// View > Items (66d4:0c21): the scenario's fourteen magic items.
//
// Popup 4, (120, 50) 400x360: "Items" (group 166) in font 1, "Items in this
// scenario" (group 167) at the foot, and a row for each item from record 8
// on, sorted by kind and then by value, 20 apart from y = 90: its name in
// colour 7 ending at x = 304, and what it does from x = 336. Dialog 35: Done
// (495) and Help (496), which shows HELP\HITEM.GFX.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as help from "./help.js";
import * as rules from "../warlords/rules.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 120, y: 50, w: 400, h: 360 };     // popup 4
const DIALOG = 35, DONE = 495, HELP = 496;
const FIRST = 8, LAST = 21;                      // records 8-21
const WHAT = {
  [rules.ITEM_BATTLE]: 1, [rules.ITEM_COMMAND]: 2, [rules.ITEM_FLIGHT]: 3,
  [rules.ITEM_DOUBLE_MOVE]: 4, [rules.ITEM_GOLD_PER_CITY]: 5,
};

export function open() {
  const G = kit.G;
  const d = { view: kit.view(DIALOG) };
  d.view.state[DONE] = uidata.NORMAL; d.view.state[HELP] = uidata.NORMAL;

  // an insertion sort, by kind then value: equal ones keep their order
  const list = G.g.map.items.filter((it) => it.index >= FIRST && it.index <= LAST);
  for (let i = 1; i < list.length; i++) {
    let j = i;
    while (j > 0 && ((list[j].type || 0) < (list[j - 1].type || 0)
           || ((list[j].type || 0) === (list[j - 1].type || 0)
               && (list[j].value || 0) < (list[j - 1].value || 0)))) {
      [list[j], list[j - 1]] = [list[j - 1], list[j]];
      j--;
    }
  }

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(0xa6, 0), 320, 52);
    kit.centred(kit.font(2), kit.text(0xa7, 0), 320, 382);
    const f = kit.font(2).colours(7, 0);
    // the kind's line carries over when an item is of no kind it knows
    let what = 0;
    list.forEach((it, i) => {
      const y = 90 + 20 * i;
      what = WHAT[it.type] || what;
      kit.right(f, it.name || "", 304, y);
      f.draw(fmt(kit.text(0xa7, what), it.value || 0), 336, y);
    });
    kit.drawControls(d.view);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === DONE) kit.pop(d);
    else if (c.id === HELP) help.open("HELP\\HITEM.GFX");
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };
  return kit.push(d);
}
