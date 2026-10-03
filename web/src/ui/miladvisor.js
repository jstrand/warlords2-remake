// The Military Advisor (military_advisor, 67cc:1f19): Shift and a click on an
// enemy beside the selected stack asks how the fight would go.
//
// Popup 12, (176, 60) 288x300, with dialog 25's one button, 424. "Advisor!"
// in font 1, ADVISOR.PCK at (256, 117), and in font 2 a random line of group
// 124, one of group 125, and the verdict -- group 126's line wins / 2 out of
// 19 battles fought in secret, on the game's own dice as the original's are.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as combat from "../warlords/combat.js";
import * as uidata from "../warlords/uidata.js";

const R = { x: 176, y: 60, w: 288, h: 300 };     // popup 12
const DIALOG = 25, OK = 424;
const TITLE = 0x7b, HAIL = 0x7c, BATTLE = 0x7d, VERDICT = 0x7e;
const PICTURE = 48;                              // ADVISOR.PCK

/** The advice for the stack attacking (x, y), or null when the option is
 *  off or nothing is selected. */
export function open(stack, x, y) {
  const G = kit.G;
  const g = G.g;
  if (g.map.options.militaryAdvisor === 0 || !stack || stack.length === 0) return null;
  const [attackers, defenders] = combat.lines(g, stack, x, y);
  const [, wins] = combat.advise(g, attackers, defenders, x, y);
  const rng = g.rng;
  const d = {
    view: kit.view(DIALOG),
    wins,
    hail: kit.text(HAIL, rng.dice(1, 5, -1)),
    battle: kit.text(BATTLE, rng.dice(1, 5, -1)),
    verdict: kit.text(VERDICT, Math.floor(wins / 2)),
  };
  d.view.state[OK] = uidata.NORMAL;
  const close = () => kit.pop(d);

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(TITLE, 0), 320, 67);
    const art = G.screen.art_for(PICTURE);
    if (art) {
      gfx.setColor(1, 1, 1);
      gfx.draw(art.image, gfx.newQuad(0, 0, 128, 130), 256, 117);
    }
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    kit.centred(f, d.hail, 320, 258);
    kit.centred(f, d.battle, 320, 278);
    kit.centred(f, d.verdict, 320, 298);
    kit.drawControls(d.view);
  };

  d.mousepressed = (mx, my) => {
    const c = kit.controlAt(d.view, mx, my);
    if (c && c.id === OK) close();
  };
  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") close();
  };
  return kit.push(d);
}
