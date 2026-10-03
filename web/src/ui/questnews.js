// A quest's end, as quest_check (4976:1ded) tells a human it.
//
// Failed: the message box with the two lines of its cause, STRING.DAT groups
// 32-42. Done: the triumph's music, and the reward (4976:1520): popup 2 with
// dialog 17 -- Done (331); the strategic map with no city shields and the
// hero's figure on it; "Quest" in font 1; SCROLL.PCK at (304, 95) and on it,
// black on yellow, the hero's quest, the priests' words and what was given.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as uidata from "../warlords/uidata.js";
import * as searchUi from "./search.js";
import * as sound from "../sound.js";
import * as cues from "../warlords/cues.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 312 };      // popup 2
const MAP = { x: 80, y: 60 };
const DIALOG = 17, DONE = 331;

function reward(news, after) {
  const G = kit.G;
  const q = news.quest, r = news.reward;
  const d = { view: kit.view(DIALOG) };
  d.view.state[DONE] = uidata.NORMAL;
  const h = q.hero;
  const close = () => { kit.pop(d); if (after) after(); };

  let grp, last;
  if (r.kind === "revealed") { grp = 0x1d; last = r.site ? r.site.name : ""; }
  else if (r.kind === "item") { grp = 0x1e; last = r.item ? r.item.name : ""; }
  else if (r.kind === "allies") {
    grp = 0x1f; last = fmt(kit.text(0x1f, 2), (r.armies || []).length, r.type ? r.type.name : "");
  } else {
    grp = 0x1f; last = fmt(kit.text(0x1f, 2), r.gold || 0, "gold");     // 4125:00d9
  }

  d.draw = () => {
    kit.popup(R);
    G.drawStrategicMap(MAP.x, MAP.y, null, true);
    if (r.kind === "revealed" && r.site && h && h.x != null) kit.mapTarget(MAP.x, MAP.y, h.x, h.y, r.site.x, r.site.y);
    if (h && h.x != null) G.drawHeroFigure(MAP.x, MAP.y, h.x, h.y);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), "Quest", 432, 64);                 // 4125:00c8
    gfx.draw(G.scrollPic, 304, 95);
    const f = kit.font(2).colours(0, 7);
    kit.centred(f, `${h ? h.name || "" : ""}'s Quest`, 440, 145);  // 4125:00ce
    kit.setPal(0);
    gfx.rectangle("fill", 360, 165, 160, 1);
    kit.centred(f, kit.text(0x1c, 0), 440, 175);
    kit.centred(f, kit.text(0x1c, 1), 440, 195);
    kit.centred(f, kit.text(grp, 0), 440, 225);
    kit.centred(f, kit.text(grp, 1), 440, 245);
    kit.centred(f, last, 440, 265);
    kit.drawControls(d.view);
  };
  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (c && c.id === DONE) close();
  };
  d.keypressed = (key) => {
    if (key === "escape" || key === "return" || key === "kpenter") close();
  };
  return kit.push(d);
}

/** Tell the player how the quest ended; `after` runs once it is read. */
export function show(news, after) {
  if (news.failed) {
    const grp = news.why || 0x20;
    return searchUi.message(kit.text(grp, 0), kit.text(grp, 1), after);
  }
  sound.music(cues.TRIUMPH);
  return reward(news, after);
}

/** Show the player's news, if there is any: the front end calls this when
 *  the screen is its own again. */
export function poll(G) {
  const side = G.player;
  const news = side && !side.computer && side.questNews;
  if (!news) return;
  side.questNews = null;
  show(news);
}
