// The end of the game (8065:1aed and the screens it leads to).
//
// The three pictures are popups 20-22, (136, 40) 368x390, each its own
// bitmap with the words painted in:
//   20 RESIGN.PCK   "An offer of Peace" -- dialog 30: No (486), Yes (485)
//   21 RESIGNNO.PCK "Peace is not an option!" -- dialog 31: Done (484)
//   22 RESIGNYE.PCK "Congratulations!" -- dialog 32: Done (483)
// Each comes with its own music.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as sound from "../sound.js";
import * as cues from "../warlords/cues.js";
import * as searchUi from "./search.js";
import * as game from "../warlords/game.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 136, y: 40, w: 368, h: 390 };     // popups 20-22
const PICTURE = { 20: 12, 21: 14, 22: 13 };      // FILE.DAT group 3

const t = (g, i) => kit.text(g, i);

function picture(popup, dialog, ids, dflt, cancel, pick) {
  const G = kit.G;
  const d = { view: kit.view(dialog) };
  for (const id of ids) d.view.state[id] = uidata.NORMAL;
  d.draw = () => {
    kit.popupFrame(R);
    const art = G.screen.art_for(PICTURE[popup]);
    if (art) {
      gfx.setColor(1, 1, 1);
      gfx.draw(art.image, gfx.newQuad(0, 0, R.w, R.h), R.x, R.y);
    }
    kit.drawControls(d.view);
  };
  const press = (id) => { kit.pop(d); pick(id); };
  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (c) press(c.id);
  };
  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter") press(dflt);
    else if (key === "escape") press(cancel);
  };
  return kit.push(d);
}

/** Congratulations (8065:1fbd), then the map shown whole (8065:2004). */
function victory(after) {
  const G = kit.G;
  sound.music(cues.TRIUMPH);
  picture(22, 32, [483], 483, 483, () => {
    G.g.map.options.hiddenMap = 0;
    if (G.stratDirty) G.stratDirty();
    searchUi.message(t(0x10, 0), t(0x10, 1), after);
  });
}

/** What the round's end found (g.ending), shown before the turn goes on. */
export function show(ending, after) {
  const G = kit.G;
  after = after || (() => {});
  if (!ending) return after();
  if (ending.surrender && !ending.shown) {
    ending.shown = true;
    sound.music(cues.SURRENDER);                  // 8065:1f68
    return picture(20, 30, [485, 486], 485, 486, (id) => {
      if (id === 485) {
        game.acceptSurrender(G.g, G.player);
        victory(after);
      } else {
        sound.music(cues.DEFIANCE);               // 8065:1ecd
        picture(21, 31, [484], 484, 484, after);
      }
    });
  }
  if (ending.won && ending.winner === G.player) {
    return searchUi.message(fmt(t(0xf, 0), G.player.name || ""), t(0xf, 1), () => victory(after));
  }
  after();
}

/** The game has stopped: nobody left, no human left, or a computer won. */
export function over(ending) {
  const G = kit.G;
  ending = ending || {};
  if (ending.winner) return searchUi.say(fmt(t(0xf, 0), ending.winner.name || ""));
  let humans = false;
  for (const s of G.g.sides) {
    if (s.alive && !s.computer && game.sideCities(G.g, s).length > 0) humans = true;
  }
  if (!humans && ending.message && ending.message.includes("No more players")) {
    return searchUi.message(t(0xc, 0), t(0xc, 1), () => {
      // "return to DOS" -- here, the start screens
      searchUi.message(t(0xc, 2), t(0xc, 3), () => { if (G.quit) G.quit(); });
    });
  }
  return searchUi.message(t(0xd, 0), t(0xd, 1), () => searchUi.message(t(0xd, 2), t(0xd, 3)));
}
