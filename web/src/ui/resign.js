// Order > Resign (7721:150d).
//
// Popup 1, (160, 90) 320x200: "Resign!" (group 165) in font 1 centred on
// (320, 92), and three lines of font 2. Dialog 33: 487 resigns graciously,
// 488 resigns, 489 thinks better of it (default and cancel both). Either way
// the side's cities burn and its armies go (7721:1608). The gracious way is
// asked three times first; the other is told afterwards what has burned.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as searchUi from "./search.js";
import * as game from "../warlords/game.js";
import * as uidata from "../warlords/uidata.js";

const R = { x: 160, y: 90, w: 320, h: 200 };     // popup 1
const DIALOG = 33, GRACIOUS = 487, RESIGN = 488, CANCEL = 489;
const S = 0xa5;

/** Only for a side that still holds a city (4125:5dea). */
export function open() {
  const G = kit.G;
  if (game.sideCities(G.g, G.player).length === 0) return null;
  const d = { view: kit.view(DIALOG) };
  for (const id of [GRACIOUS, RESIGN, CANCEL]) d.view.state[id] = uidata.NORMAL;
  const t = (i) => kit.text(S, i);

  const burn = (after) => {
    game.resign(G.g, G.player);
    G.selection = null;
    if (G.stratDirty) G.stratDirty();
    if (after) after();
  };

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), t(0), 320, 92);
    for (let i = 1; i <= 3; i++) kit.centred(kit.font(2), t(i), 320, 120 + 20 * i);
    kit.drawControls(d.view);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === CANCEL) kit.pop(d);
    else if (c.id === GRACIOUS) {
      kit.pop(d);
      searchUi.say(t(6), () => {
        searchUi.say(t(7), () => {
          searchUi.say(t(8), () => {
            searchUi.message(t(9), t(10), () => burn());
          });
        });
      });
    } else if (c.id === RESIGN) {
      kit.pop(d);
      burn(() => searchUi.message(t(4), t(5)));
    }
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };
  return kit.push(d);
}
