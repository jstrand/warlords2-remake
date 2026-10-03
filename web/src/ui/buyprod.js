// Build Production: buying a new army type for a city.
//
// The city dialog's Build Prod button (control 204, 7087:0978) pushes popup
// 11 -- (80, 60) 480x350, marble -- and dialog 23 over it, drawn by
// auto_ui_build_production (7087:09da): the title between the side's
// shields, "The %s city of %s", every type with a price four to a row --
// ghosted when the city already has it or the side cannot pay -- "Currently
// Producing" with the city's four slots, and the gold. The slot being bought
// into is framed in colours 0 and 9, the others in 3. Control 396 is Done,
// 397-400 pick the slot (live only once all four are full), 401 on are the
// types. Done puts the list back in price order (7087:0eca).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as infobox from "./infobox.js";
import * as game from "../warlords/game.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 350 };      // popup 11
const DIALOG = 23;
const DONE = 396, SLOT_FIRST = 397, TYPE_FIRST = 401;
const STR = 0x70;                                // STRING.DAT group 112

function refresh(d) {
  const G = kit.G, st = d.view.state;
  st[DONE] = uidata.NORMAL;
  const full = d.city.slots.length >= 4;
  for (let i = 0; i < 4; i++) st[SLOT_FIRST + i] = full ? uidata.NORMAL : uidata.DISABLED;
  d.types.forEach((a, n) => {
    st[TYPE_FIRST + n] = game.cannotBuy(G.g, G.player, d.city, a) ? uidata.DISABLED : uidata.NORMAL;
  });
}

/** Open the screen for a city. `done` runs when it closes. */
export function open(city, done) {
  const G = kit.G;
  const d = {
    city, done,
    view: kit.view(DIALOG),
    types: game.buyableTypes(G.g),
    slot: game.buySlot(city),                    // from 0
  };
  refresh(d);

  d.close = () => {
    game.sortProduction(city);
    kit.pop(d);
    if (d.done) d.done();
  };

  d.buy = (n) => {
    const a = d.types[n];
    if (!a || game.cannotBuy(G.g, G.player, city, a)) return;
    game.buyProduction(G.g, G.player, city, d.slot, a.id);
    // on to the next empty slot, if there is one (7087:0ee3)
    if (d.slot < 3 && !city.slots[d.slot + 1]) d.slot++;
    refresh(d);
  };

  d.draw = () => {
    const side = G.player.index;
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(STR, 0), 320, 64);
    kit.shield(side, 88, 64);
    kit.shield(side, 512, 64);
    const body = kit.font(2);
    gfx.setColor(1, 1, 1);
    kit.centred(body, fmt(kit.text(STR, 1), G.player.name, city.name), 320, 104);
    kit.centred(body, fmt(kit.text(STR, 2), G.player.gold), 352, 365);
    kit.centred(body, kit.text(STR, 3), 176, 348);

    for (let i = 0; i < 4; i++) {
      const slot = city.slots[i];
      const x = 104 + i * 40;
      kit.army(slot ? slot.type : null, side, x, 370, slot ? side + 2 : 1);
      const fx = x - 2, fy = 368;
      if (d.slot === i) {
        kit.setPal(0);
        kit.outline(fx, fy, 37, 35);
        kit.setPal(9);
        kit.outline(fx - 1, fy - 1, 37, 35);
      } else {
        kit.setPal(3);
        kit.outline(fx, fy, 37, 35);
        kit.outline(fx - 1, fy - 1, 37, 35);
      }
    }

    d.types.forEach((a, n) => {
      const col = n % 4, row = Math.floor(n / 4);
      const x = 88 + col * 120, y = 136 + row * 31;
      kit.army(a.id, side, x, y, 1, game.cannotBuy(G.g, G.player, city, a) != null);
      gfx.setColor(1, 1, 1);
      body.draw(fmt("%d gp", a.price), x + 40, y + 6);
    });
    kit.drawControls(d.view);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === DONE) d.close();
    else if (c.id >= SLOT_FIRST && c.id < SLOT_FIRST + 4) d.slot = c.id - SLOT_FIRST;
    else if (c.id >= TYPE_FIRST) d.buy(c.id - TYPE_FIRST);
  };

  // the right button on a type for sale (sub-ids 5-24, 7087:11f4)
  d.info = (sub, sx, sy) => {
    const a = d.types[sub - 5];
    if (!a) return false;
    infobox.armyType(sx, sy, a.id, a);
    return true;
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") d.close();
  };
  return kit.push(d);
}
