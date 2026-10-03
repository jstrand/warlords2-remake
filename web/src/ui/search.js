// Hero > Search: what a ruin, a temple or a sage says (site_search,
// 6536:0000), and the game's plain message box.
//
// A ruin (6536:01ab) opens popup 4 with "Searching" and SEARCH.PCK in a black
// frame; its story is told a line at a time from (128, 300), each waiting
// for a key or a click; then dialog 13: Done (292) and, when an item was
// found and left on the ground, Take (293).
//
// A temple (4976:0000) is popup 9 -- TEMPLE.PCK -- with its name and two
// lines of greeting, over dialog 15: Bless (328) and Quest (329).
//
// A sage (6536:0aa0) is announced in the ruin popup, then has a popup of its
// own -- popup 2, the strategic map, dialog 10 -- offering where the rich
// sites nearby lie (Items, 279), a gem (Money, 280) or a patch of the hidden
// map (Maps, 281); one of them, then Done (282).
//
// The message box (8065:1160) is popup 5 -- (144, 179) 352x64 -- closed by
// any key or click.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as sound from "../sound.js";
import * as cues from "../warlords/cues.js";
import * as chooseUi from "./choose.js";
import * as game from "../warlords/game.js";
import * as hero from "../warlords/hero.js";
import * as quest from "../warlords/quest.js";
import * as site from "../warlords/site.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const MSG = { x: 144, y: 179, w: 352, h: 64 };  // popup 5

/** A message box. `after` runs when it is closed. */
export function message(line1, line2, after) {
  const d = {};
  const close = () => { kit.pop(d); if (after) after(); };
  d.draw = () => {
    kit.popup(MSG);
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    if (line1) kit.centred(f, line1, 320, 190);
    if (line2) kit.centred(f, line2, 320, 212);
  };
  d.mousepressed = () => close();
  d.keypressed = () => close();
  return kit.push(d);
}

/** A one-line message (8065:10fb), centred on (320, 201). */
export function say(line, after) {
  const d = {};
  const close = () => { kit.pop(d); if (after) after(); };
  d.draw = () => {
    kit.popup(MSG);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(2), line, 320, 201);
  };
  d.mousepressed = () => close();
  d.keypressed = () => close();
  return kit.push(d);
}

const RUIN = { x: 120, y: 50, w: 400, h: 360 }; // popup 4
const RUIN_DIALOG = 13, DONE = 292, TAKE = 293;

function ruinLines(r) {
  const lines = [];
  const name = r.hero ? r.hero.name || "" : "";
  const monster = r.monster || r.guardian;
  if (r.kind !== "allies") {
    if (monster) {
      lines.push(fmt(kit.text(0x33, 0), name, monster.name || ""));
      lines.push(kit.text(0x33, r.kind === "killed" ? 1 : 2));
    } else {
      lines.push(kit.text(0x32, 0));
    }
  }
  if (r.kind === "item" && r.item) {
    lines.push(fmt(kit.text(0x34, 0), name, r.item.name || ""));
  } else if (r.kind === "gold") {
    lines.push(fmt(kit.text(0x35, 0), name, r.gold));
  } else if (r.kind === "allies") {
    const n = r.armies.length;
    const tname = r.type ? r.type.name : "";
    lines.push(n === 1 ? fmt(kit.text(0x36, 0), tname, name) : fmt(kit.text(0x36, 1), n, tname, name));
  }
  return lines;
}

/** The ruin popup telling `r`'s story. With `after`, it has no buttons: the
 *  click after the last line closes it and runs `after`. */
function openRuin(r, after) {
  const G = kit.G;
  const d = { lines: r.lines || ruinLines(r), shown: 1, view: kit.view(RUIN_DIALOG) };
  // 6536:01ab: the orchestra comes in as the guardian's line is read on
  const guarded = !r.lines && r.kind !== "allies" && (r.monster || r.guardian);
  const advance = () => {
    if (guarded && d.shown === 1) sound.effect("orch");
    d.shown++;
  };
  const canTake = r.kind === "item" && r.item && r.item.status === 1;
  d.view.state[DONE] = uidata.NORMAL;
  d.view.state[TAKE] = uidata.NORMAL;
  d.hidden = canTake ? {} : { [TAKE]: true };

  const done = () => d.shown >= d.lines.length;
  const close = () => {
    kit.pop(d);
    quest.event(G.g, G.player, "item", {});
  };

  d.draw = () => {
    kit.popup(RUIN);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), "Searching", 320, 53);        // 4125:0c90
    gfx.draw(G.searchPic, 160, 92);
    kit.setPal(0);
    kit.outline(159, 91, 322, 202);
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    for (let i = 0; i < d.shown; i++) f.draw(d.lines[i] || "", 128, 300 + 20 * i);
    if (done() && !after) kit.drawControls(d.view, d.hidden);
  };

  d.mousepressed = (x, y) => {
    if (!done()) { advance(); return; }
    if (after) { kit.pop(d); return after(); }
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (!c) return;
    if (c.id === TAKE && r.hero) {
      for (const it of G.g.map.items) {
        if (it.status === 1 && it.x === r.hero.x && it.y === r.hero.y) hero.takeItem(G.g, r.hero, it);
      }
      close();
    } else if (c.id === DONE) close();
  };

  d.keypressed = (key) => {
    if (!done()) { advance(); return; }
    if (after) { kit.pop(d); return after(); }
    if (key === "return" || key === "kpenter" || key === "escape") close();
  };

  sound.effect("dramatic");
  return kit.push(d);
}

const TEMPLE = { x: 160, y: 60, w: 320, h: 280 }; // popup 9
const TEMPLE_DIALOG = 15, BLESS = 328, QUEST = 329;

function openTemple(r, stack) {
  const G = kit.G;
  sound.music(cues.TEMPLE);                       // 4976:0000
  const d = { view: kit.view(TEMPLE_DIALOG) };
  d.view.state[BLESS] = uidata.NORMAL;
  const canQuest = G.g.map.options.quests !== 0 && !G.player.quest && r.hero;
  d.view.state[QUEST] = canQuest ? uidata.NORMAL : uidata.DISABLED;

  d.draw = () => {
    kit.popupFrame(TEMPLE);
    gfx.setColor(1, 1, 1);
    gfx.draw(G.templePic, TEMPLE.x, TEMPLE.y);
    const f = kit.font(2).colours(7, 6);
    kit.centred(f, fmt(kit.text(0x13, 0), r.site.name || ""), 320, 270);
    kit.centred(f, kit.text(0x13, 1), 320, 290);
    kit.centred(f, kit.text(0x13, 2), 320, 310);
    kit.drawControls(d.view);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === BLESS) {
      // temple_bless (6536:08a1): how many were blessed, and a parting line
      kit.pop(d);
      const n = site.bless(G.g, r.site, stack);
      let line;
      if (n === 0) line = kit.text(0x37, 0);
      else if (n === 1) line = kit.text(0x38, 0);
      else line = fmt(kit.text(0x38, 1), n);
      message(line, kit.text(0x38, 2));
    } else if (c.id === QUEST) {
      kit.pop(d);
      quest.assign(G.g, G.player, r.hero);
      if (G.openQuest) G.openQuest();
    }
  };
  d.keypressed = () => {};
  return kit.push(d);
}

const SAGE = { x: 80, y: 60, w: 480, h: 312 };  // popup 2
const SAGE_MAP = { x: 80, y: 60 };
const SAGE_DIALOG = 10, ITEMS = 279, GEM = 280, MAP = 281, SAGE_DONE = 282;

function openSage(r) {
  const G = kit.G;
  const h = r.hero;
  const d = { view: kit.view(SAGE_DIALOG), lines: [] };
  const list = site.sageList(G.g, G.player, h.x, h.y);
  const hiddenMap = G.g.map.options.hiddenMap !== 0;

  const offers = (on) => {
    const st = d.view.state;
    if (on) {
      st[ITEMS] = list.length > 0 ? uidata.NORMAL : uidata.DISABLED;
      st[GEM] = uidata.NORMAL;
      st[MAP] = hiddenMap ? uidata.NORMAL : uidata.DISABLED;
    } else {
      st[ITEMS] = uidata.DISABLED; st[GEM] = uidata.DISABLED; st[MAP] = uidata.DISABLED;
    }
    d.hidden = { [SAGE_DONE]: true };
  };
  const finished = () => {
    d.view.state[SAGE_DONE] = uidata.NORMAL;
    d.hidden = { [ITEMS]: true, [GEM]: true, [MAP]: true };
    d.pointing = false;
  };
  const sayLine = (line) => d.lines.push(line);

  d.draw = () => {
    kit.popup(SAGE);
    G.drawStrategicMap(SAGE_MAP.x, SAGE_MAP.y);
    if (!d.target) G.drawSiteMarkers(SAGE_MAP.x, SAGE_MAP.y, r.site);
    if (d.target) {
      kit.mapTarget(SAGE_MAP.x, SAGE_MAP.y, h.x, h.y, d.target.x, d.target.y);
      G.drawHeroFigure(SAGE_MAP.x, SAGE_MAP.y, h.x, h.y);
    }
    if (d.patch) {
      kit.setPal(15);
      kit.outline(SAGE_MAP.x + 2 * d.patch[0], SAGE_MAP.y + 2 * d.patch[1], 2 * d.patch[2], 2 * d.patch[3]);
    }
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), "A Sage!", 432, 62);           // 4125:0c7c
    const f = kit.font(2);
    for (let i = 0; i <= 4; i++) kit.centred(f, kit.text(0x3f, i), 432, 120 + 20 * i);
    d.lines.forEach((line, i) => kit.centred(f, line, 432, 240 + 20 * i));
    kit.drawControls(d.view, d.hidden);
  };

  const items = () => {
    offers(false);
    chooseUi.open("Items", list, 0, (e) => {
      if (!e) return offers(true);
      if (e.kind === "gold") sayLine(kit.text(0x3e, 2));
      else if (e.kind === "allies") sayLine(kit.text(0x3e, 3));
      else sayLine(fmt(kit.text(0x3e, 0), e.name));
      sayLine(kit.text(0x3e, 1));
      const s = site.sageShow(G.g, G.player, e, h.x, h.y);
      if (s) {
        sayLine(fmt(kit.text(0x3e, 7), s.name || ""));
        d.target = s;
      }
      if (G.stratDirty) G.stratDirty();
      finished();
    });
  };

  d.mousepressed = (x, y) => {
    if (d.pointing) {
      // the map is region 15: a click on it picks the tile (6536:0cd6)
      const tx = Math.floor((x - SAGE_MAP.x) / 2), ty = Math.floor((y - SAGE_MAP.y) / 2);
      if (x >= SAGE_MAP.x && y >= SAGE_MAP.y && x < SAGE_MAP.x + 224 && y < SAGE_MAP.y + 312) {
        d.patch = site.sageMap(G.g, G.player, tx, ty);
        if (G.stratDirty) G.stratDirty();
        finished();
      }
      return;
    }
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (!c) return;
    if (c.id === ITEMS) items();
    else if (c.id === GEM) {
      const n = site.sageGem(G.g, G.player);
      sayLine(kit.text(0x3b, 0));
      sayLine(fmt(kit.text(0x3b, 1), n));
      finished();
    } else if (c.id === MAP) {
      offers(false);
      for (let i = 0; i <= 2; i++) sayLine(kit.text(0x3c, i));
      d.pointing = true;
    } else if (c.id === SAGE_DONE) {
      kit.pop(d);
      if (G.stratDirty) G.stratDirty();
    }
  };

  d.keypressed = (key) => {
    if (d.hidden[SAGE_DONE]) return;
    if (key === "escape" || key === "return" || key === "kpenter") {
      kit.pop(d);
      if (G.stratDirty) G.stratDirty();
    }
  };

  offers(true);
  return kit.push(d);
}

/** Hero > Search with the selected stack. */
export function open(stack) {
  const G = kit.G;
  if (!stack || stack.length === 0) return;
  const r = game.searchHere(G.g, stack, true);
  if (!r || r.kind === "no hero") return;
  if (r.kind === "temple") return openTemple(r, stack);
  if (r.kind === "sage") {
    sound.music(cues.SAGE);
    return openRuin({ lines: [fmt(kit.text(0x3a, 0), r.hero.name || "")] }, () => openSage(r));
  }
  if (G.stratDirty) G.stratDirty();
  return openRuin(r);
}
