// A hero's promotion and an army's medal, as the original shows them to a
// human: a picture popup at (160, 90) 320x200 with lines of text over it,
// closed by any key or click.
//
// A promotion (hero_promote, 7563:0672) is popup 16, 17 or 18 -- LEVEL2-4.PCK
// -- with music cue 7: STRING.DAT group 101-103 (lines 6 on for a heroine),
// the title in font 1 at y 93, the hero's name at 140 and the rest in font 2
// at 160, 180, 200, 232 and 252, all centred on 320.
//
// A medal (auto_ui_medal_effect, 67cc:2274) is popup 24-27 -- MEDAL0-3.PCK
// for the army's first to fourth -- with music cue 8 and then 1 again: the
// army at (176, 155), and in font 2 a line of group 158 at (216, 155), "your
// unit of %s" (159) at (216, 175), a line of 160 at (176, 195), the medal's
// name (161) at (176, 215), and what grew (162) at (176, 245) and (176, 265).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as sound from "../sound.js";
import * as cues from "../warlords/cues.js";
import { fmt } from "../util.js";

const R = { x: 160, y: 90, w: 320, h: 200 };
const PROMOTION_GROUP = 101, MEDAL_FOR = 158, MEDAL_UNIT = 159, MEDAL_GIVEN = 160;
const MEDAL_NAME = 161, MEDAL_GREW = 162;

function show(pic, draw, after) {
  const d = {};
  const close = () => { kit.pop(d); if (after) after(); };
  d.draw = () => {
    kit.popupFrame(R);
    gfx.setColor(1, 1, 1);
    if (pic) gfx.draw(pic, R.x, R.y);
    draw();
  };
  d.mousepressed = () => close();
  d.keypressed = () => close();
  return kit.push(d);
}

/** Show each of `list` -- hero.checkPromotions's { hero, gain } -- in turn,
 *  then `after`. */
export function promotions(list, after) {
  const G = kit.G;
  const next = (i) => {
    if (!list || i >= list.length) { if (after) after(); return; }
    const { hero, gain } = list[i];
    const level = hero.level || 2;                       // the new level, 2-4
    const group = PROMOTION_GROUP + level - 2;
    const base = hero.female ? 6 : 0;
    sound.music(cues.PROMOTION);
    show(G.levelPic[level], () => {
      kit.centred(kit.font(1), kit.text(group, base), 320, 93);
      const f = kit.font(2);
      kit.centred(f, hero.name || "", 320, 140);
      kit.centred(f, kit.text(group, base + 1), 320, 160);
      kit.centred(f, kit.text(group, base + 2), 320, 180);
      kit.centred(f, kit.text(group, base + 3), 320, 200);
      kit.centred(f, kit.text(group, base + 4), 320, 232);
      kit.centred(f, fmt(kit.text(group, base + 5), level, hero.strength || 0, gain || 0,
                         hero.maxMoves || 0, 2), 320, 252);
    }, () => next(i + 1));
  };
  next(0);
}

/** An army's new medal (game.applyAttack's result.medal), then `after`. */
export function medal(a, after) {
  const G = kit.G;
  const n = a.medals || 1;
  const grewMove = (a.strength || 0) >= 15;
  const now = grewMove ? a.maxMoves || 0 : a.strength || 0;
  const pick = (group, count) => kit.text(group, G.g.rng.dice(1, count, -1));
  const line1 = pick(MEDAL_FOR, 7), line3 = pick(MEDAL_GIVEN, 4);
  const t = G.g.types.byId[a.type];
  sound.music(cues.MEDAL);
  show(G.medalPic[n - 1], () => {
    kit.army(a.type, a.owner, 176, 155, 0);
    gfx.setColor(1, 1, 1);
    const f = kit.font(2);
    f.draw(line1, 216, 155);
    f.draw(fmt(kit.text(MEDAL_UNIT, 0), t ? t.name : ""), 216, 175);
    f.draw(line3, 176, 195);
    f.draw(kit.text(MEDAL_NAME, n - 1), 176, 215);
    f.draw(kit.text(MEDAL_GREW, grewMove ? 1 : 0), 176, 245);
    f.draw(fmt(kit.text(MEDAL_GREW, 2), now - 1, now), 176, 265);
  }, () => {
    sound.music(cues.PLAY);
    if (after) after();
  });
}
