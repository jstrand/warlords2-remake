// Hero > Levels (auto_ui_hero_levels, 7563:1652).
//
// Popup 0, (80, 60) 480x320, and dialog 28: Done (469). "Hero Levels" in
// font 1, the column heads in the side's colours at y = 105, and a row per
// hero, 30 apart from y = 128, highest level first: the hero on a ring --
// its side's colour at the top level, grey below -- its name, its title, its
// experience, what the next level needs, its strength and its moves.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as armytype from "../warlords/armytype.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 320 };      // popup 0
const DIALOG = 28, DONE = 469;
const S = 0x68, S_MALE = 0x63, S_FEMALE = 0x64;
const NEEDS = { 1: 15, 2: 30, 3: 60 };

export function open() {
  const G = kit.G;
  const d = { view: kit.view(DIALOG) };
  d.view.state[DONE] = uidata.NORMAL;

  const rows = [];
  for (let level = 4; level >= 1; level--) {
    for (let i = G.g.armies.length - 1; i >= 0; i--) {
      const a = G.g.armies[i];
      if (a.type === armytype.HERO && a.owner === G.player.index && (a.level || 1) === level) rows.push(a);
    }
  }

  d.draw = () => {
    const side = G.player;
    const f = kit.font(2);
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(S, 0), 320, 63);
    const head = f.colours(side.colour ?? 15, side.edge ?? 0);
    head.draw(kit.text(S, 1), 128, 105);
    head.draw(kit.text(S, 2), 272, 105);
    [392, 440, 488, 536].forEach((x, k) => kit.centred(head, kit.text(S, k + 3), x, 105));

    rows.forEach((h, i) => {
      const y = 128 + 30 * i;
      const level = h.level || 1;
      kit.army(armytype.HERO, side.index, 88, y, level === 4 ? side.index + 2 : 1);
      gfx.setColor(1, 1, 1);
      f.draw(h.name || "", 128, y + 6);
      f.draw(kit.text(h.female ? S_FEMALE : S_MALE, level - 1), 272, y + 6);
      kit.centred(f, fmt("%d", h.experience || 0), 392, y + 6);
      kit.centred(f, NEEDS[level] ? fmt("%d", NEEDS[level]) : "-", 440, y + 6);
      kit.centred(f, fmt("%d", h.strength || 0), 488, y + 6);
      kit.centred(f, fmt("%d", h.maxMoves || 0), 536, y + 6);
    });
    for (let i = rows.length; i < 6; i++) kit.army(null, side.index, 88, 128 + 30 * i, 1);
    kit.drawControls(d.view);
  };

  d.close = () => kit.pop(d);
  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (c && c.id === DONE) d.close();
  };
  d.keypressed = (key) => {
    if (key === "escape" || key === "return" || key === "kpenter") d.close();
  };
  return kit.push(d);
}
