// Report > Quest (auto_ui_no_quest, 4976:0167; the text, 4976:0320).
//
// Popup 2 with dialog 16 -- Done (330). "Quest" in font 1, and the strategic
// map with no city shields. With no quest, one of four lines of group 20.
// With one, SCROLL.PCK and on it, black on yellow, "%s's Quest", a black rule
// and the quest's own lines (groups 21-27). The map shows where to go: a
// line from the hero to the target with a little shield in a box -- or, for
// a quest to slay a kind of army or a side's armies, a banner on every such
// stack (834b:158d). The hero's figure is drawn over it all.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as game from "../warlords/game.js";
import * as quest from "../warlords/quest.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 312 };      // popup 2
const MAP = { x: 80, y: 60 };
const DIALOG = 16, DONE = 330;
const COMPASS = ["north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest"];

/** 828e:0b51: the compass point from (x1, y1) to (x2, y2). */
function direction(x1, y1, x2, y2) {
  if (x1 === x2) return y1 < y2 ? 4 : 0;
  if (y1 === y2) return x1 < x2 ? 2 : 6;
  if (x2 < x1 && y2 < y1) return 7;
  if (x2 < x1 && y1 < y2) return 5;
  if (x1 < x2 && y2 < y1) return 1;
  if (x1 < x2 && y1 < y2) return 3;
  return 0;
}

/** Where an item is: its carrier, the ruin it lies in, or the ground. */
function itemPlace(g, it) {
  if (it.status === 3) {
    for (const a of g.armies) {
      for (const c of a.items || []) if (c === it && a.x != null) return [a.x, a.y];
    }
  } else if (it.status === 2) {
    for (const s of g.map.sites) if (s.item === it.index) return [s.x, s.y];
  }
  return [it.x, it.y];
}

/** The quest's lines, as [y, text] pairs, and where its target is. */
function questText(g, side, q) {
  const t = (grp, i) => kit.text(grp, i);
  const h = q.hero, lines = [];
  let tx = null, ty = null;
  const add = (y, s) => lines.push([y, s]);
  const fabled = (c) => g.map.options.hiddenMap !== 0 && !game.seen(g, side, c.x, c.y);
  if (q.type === quest.SLAY_HERO) {
    const foe = q.target;
    add(175, t(0x15, 0)); add(195, t(0x15, 1));
    add(215, fmt(t(0x15, 2), g.map.sides[foe.owner].name));
    add(235, foe.name || "");
    add(265, t(0x15, 3));
    tx = foe.x; ty = foe.y;
    add(285, COMPASS[direction(h.x, h.y, tx, ty)]);
  } else if (q.type === quest.RETRIEVE_ITEM) {
    add(175, t(0x16, 0)); add(195, t(0x16, 1)); add(215, q.target.name || "");
    add(245, t(0x16, 2));
    [tx, ty] = itemPlace(g, q.target);
    if (tx != null) add(265, COMPASS[direction(h.x, h.y, tx, ty)]);
  } else if (q.type === quest.SLAY_TYPE) {
    add(175, t(0x17, 0)); add(195, t(0x17, 1)); add(215, t(0x17, 2));
    add(235, q.target.name || "");
  } else if (q.type === quest.SLAUGHTER) {
    add(175, t(0x18, 0)); add(195, fmt(t(0x18, 1), q.required || 0));
    add(215, q.target.name || "");
    add(245, t(0x18, 2)); add(265, fmt(t(0x18, 3), q.done || 0));
  } else if (q.type === quest.OCCUPY || q.type === quest.RAZE) {
    const grp = q.type === quest.OCCUPY ? 0x19 : 0x1a;
    const c = q.target;
    add(175, t(grp, 0));
    add(195, fmt(t(grp, fabled(c) ? 2 : 1), c.name));
    add(215, t(grp, 3)); add(235, t(grp, 4)); add(265, t(grp, 5));
    tx = c.x; ty = c.y;
    add(285, COMPASS[direction(h.x, h.y, tx, ty)]);
  } else if (q.type === quest.PILLAGE_GOLD) {
    add(175, t(0x1b, 0)); add(195, fmt(t(0x1b, 1), q.required || 0));
    add(215, t(0x1b, 2)); add(235, t(0x1b, 3)); add(265, t(0x1b, 4));
    add(285, fmt(t(0x1b, 5), q.done || 0));
  }
  return [lines, tx, ty];
}

export function open() {
  const G = kit.G;
  const d = { view: kit.view(DIALOG) };
  d.view.state[DONE] = uidata.NORMAL;
  let q = G.player.quest;
  let alive = false;
  if (q && q.hero) alive = G.g.armies.some((a) => a === q.hero && a.x != null);
  if (!alive) q = null;
  d.noQuest = kit.text(0x14, G.g.rng.dice(1, 4, -1));

  const mapMarks = (tx, ty) => {
    gfx.setScissor(MAP.x, MAP.y, 224, 312);
    if (q.type === quest.SLAY_TYPE || q.type === quest.SLAUGHTER) {
      const done = new Set();
      for (const a of G.g.armies) {
        const match = (q.type === quest.SLAY_TYPE && a.type === q.target.id)
                   || (q.type === quest.SLAUGHTER && a.owner === q.target.index);
        if (match && a.x != null && a.owner !== G.player.index && !done.has(a.x + a.y * 1000)
            && game.seen(G.g, G.player, a.x, a.y)) {
          done.add(a.x + a.y * 1000);
          gfx.setColor(1, 1, 1);
          gfx.draw(G.atransShields, gfx.newQuad((a.owner ?? 8) * 16, 164, 16, 10),
                   MAP.x + Math.max(0, a.x * 2 - 1), MAP.y + Math.max(0, a.y * 2 - 1));
        }
      }
    }
    gfx.setScissor();
    if (tx != null && q.type !== quest.SLAY_TYPE && q.type !== quest.SLAUGHTER) {
      kit.mapTarget(MAP.x, MAP.y, q.hero.x, q.hero.y, tx, ty);
    }
  };

  d.draw = () => {
    kit.popup(R);
    G.drawStrategicMap(MAP.x, MAP.y, null, true);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), "Quest", 432, 62);                 // 4125:00b4
    if (!q) {
      kit.centred(kit.font(2), d.noQuest, 432, 134);
    } else {
      gfx.draw(G.scrollPic, 304, 95);
      const f = kit.font(2).colours(0, 7);
      kit.centred(f, `${q.hero.name || ""}'s Quest`, 432, 145);
      kit.setPal(0);
      gfx.rectangle("fill", 360, 165, 160, 1);
      const [lines, tx, ty] = questText(G.g, G.player, q);
      for (const l of lines) kit.centred(f, l[1], 432, l[0]);
      mapMarks(tx, ty);
      G.drawHeroFigure(MAP.x, MAP.y, q.hero.x, q.hero.y);
    }
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
