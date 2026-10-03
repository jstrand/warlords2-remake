// What a pillage or a sack took (auto_ui_pillage_sack_raze, 63fa:0508),
// shown over the spoils dialog and closed, the two together, by any key or
// click. Popup 12, (176, 60) 288x300, marble.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import { fmt } from "../util.js";

const R = { x: 176, y: 60, w: 288, h: 300 };     // popup 12
const TITLE = 0x44, LINE = 0x45, GOLD = 0x46, LOST = 0x47, LEFT = 0x48;

/** `sacked` false for a pillage; `lost` is game.pillage's or game.sack's
 *  list; `left` the production types the city still has. */
export function open(sacked, city, gold, lost, left, after) {
  const G = kit.G;
  const d = {};
  const which = sacked ? 1 : 0;
  const close = () => { kit.pop(d); if (after) after(); };

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(TITLE, which), 320, 64);
    const f = kit.font(2);
    kit.centred(f, fmt(kit.text(LINE, which), city.name), 320, 115);
    kit.centred(f, fmt(kit.text(GOLD, 0), gold), 320, 135);
    kit.centred(f, fmt(kit.text(LOST, lost.length === 1 ? 0 : 1), lost.length), 320, 165);
    kit.centred(f, fmt(kit.text(LEFT, left === 1 ? 0 : 1), left), 320, 185);
    kit.bevel(184, 220, 272, 130, 4, 2);
    kit.bevel(185, 221, 270, 128, 2, 4);
    gfx.setColor(1, 1, 1);
    f.draw(kit.text(TITLE, 2), 192, 230);
    f.draw(kit.text(TITLE, 3), 368, 230);
    for (let i = 0; i <= 2; i++) {
      const l = lost[i];
      const y = 250 + 30 * i;
      kit.army(l ? l.type : null, G.player.index, 208, y, 1);
      if (l) {
        gfx.setColor(1, 1, 1);
        const t = G.g.types.byId[l.type];
        f.draw(t ? t.name : "", 248, y + 6);
        f.draw(fmt("%d gp", l.gold), 368, y + 6);
      }
    }
  };
  d.mousepressed = () => close();
  d.keypressed = () => close();
  return kit.push(d);
}
