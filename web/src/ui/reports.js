// The Report menu: Army, City, Gold, Production and Winning.
//
// Dialog 7 over popup 2, (80, 60) 480x312, the five reports as tabs (218-222)
// and Done (223); auto_ui_reports_menu (6ef3:0030) draws the strategic map
// -- a banner on every stack outside a city for Army (834b:158d), See All's
// vectors for Production -- the title, what it measures, a bar per side or
// the list of what was built this turn, and the summary in the side's
// colours. The bars (6ef3:05ff) are tiled 8 pixels at a time from
// SHIELDS.PCK; the Production list (6f8c:086e) shows five at a time, and
// 206-209 scroll it.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as report from "../warlords/report.js";
import * as game from "../warlords/game.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 312 };      // popup 2
const MAP = { x: 80, y: 60 };
const DIALOG = 7;
const TAB_FIRST = 218, DONE = 223;
const UP = 206, DOWN = 207, PAGE_UP = 208, PAGE_DOWN = 209;
const S_TITLE = 0x49, S_WHAT = 0x4a;
const S_SUMMARY = [0x4b, 0x4c, 0x4d, 0x4e, 0x4f];
const ROWS = 5;

function summary(n, f) {
  const r = f.result;
  if (n === report.ARMY || n === report.CITY) {
    return r === 1 ? kit.text(S_SUMMARY[n], 0) : fmt(kit.text(S_SUMMARY[n], 1), r);
  }
  if (n === report.GOLD) return fmt(kit.text(S_SUMMARY[n], 0), r);
  if (n === report.PRODUCTION) return fmt(kit.text(S_SUMMARY[n], r === 1 ? 1 : 0), r);
  return kit.text(S_SUMMARY[n], r);
}

function drawArmyBanners() {
  const G = kit.G;
  const done = new Set();
  gfx.setScissor(MAP.x, MAP.y, 224, 312);
  gfx.setColor(1, 1, 1);
  for (const a of G.g.armies) {
    if (!a.transit && a.x != null) {
      const k = a.y * 1000 + a.x;
      if (!done.has(k) && game.seen(G.g, G.player, a.x, a.y) && !game.cityAt(G.g, a.x, a.y)) {
        done.add(k);
        const side = a.owner ?? 8;
        gfx.draw(G.atransShields, gfx.newQuad(side * 16, 164, 16, 10),
                 MAP.x + Math.max(0, a.x * 2 - 1), MAP.y + Math.max(0, a.y * 2 - 1));
      }
    }
  }
  gfx.setScissor();
}

function drawBars(f) {
  const G = kit.G;
  for (let i = 0; i < 8; i++) {
    if (!f.out[i]) {
      const y = 200 + 14 * i;
      let w = 1;
      if (f.max > 0) w = Math.floor(Math.max(0, f.value[i]) * 240 / f.max);
      w = Math.max(1, w);
      gfx.setColor(1, 1, 1);
      const sx = 320 + 8 * (i % 4), sy = 40 + 8 * Math.floor(i / 4);
      for (let x = 0; x < w; x += 8) {
        const piece = Math.min(8, w - x);
        gfx.draw(G.shieldsImg, gfx.newQuad(sx, sy, piece, 8), 312 + x, y);
      }
      kit.bevel(311, y - 1, w + 2, 10, 0, 1);
    }
  }
  const axis = (colour, d) => {
    kit.setPal(colour);
    gfx.rectangle("fill", 312 - d, 196 - d, 240, 1);
    [312, 372, 432, 492, 552].forEach((tx, k) => {
      const long = k % 2 === 0;
      gfx.rectangle("fill", tx - d, (long ? 190 : 192) - d, 1, long ? 6 : 4);
    });
  };
  axis(1, 0);
  axis(0, 1);
  const font = kit.font(2);
  gfx.setColor(1, 1, 1);
  kit.right(font, fmt("%d", f.max), 556, 172);
  kit.centred(font, fmt("%d", Math.floor(f.max / 2)), 432, 172);
  kit.centred(font, "0", 308, 172);
}

function drawProduction(d) {
  const G = kit.G;
  const list = G.player.produced || [];
  const font = kit.font(2);
  for (let i = 0; i < ROWS; i++) {
    const e = list[d.top + i];
    if (!e) break;
    const y = 176 + 30 * i;
    gfx.setColor(1, 1, 1);
    font.draw(fmt("%d", d.top + i + 1), 312, y);
    kit.army(e.type, G.player.index, 328, 170 + 30 * i, 1);
    const city = e.city != null ? G.g.map.cities[e.city] : null;
    const name = e.standard ? "Standard" : (city ? city.name : "");
    let text;
    if (e.kind === "sent") text = fmt("%s ...", name);
    else if (e.kind === "arrived") text = fmt("... %s", name);
    else text = name;
    gfx.setColor(1, 1, 1);
    font.draw(text, 368, y);
  }
}

function refresh(d) {
  const st = d.view.state;
  for (let i = 0; i <= 4; i++) st[TAB_FIRST + i] = i === d.n ? uidata.ACTIVE : uidata.NORMAL;
  st[DONE] = uidata.NORMAL;
  d.hidden = {};
  if (d.n === report.PRODUCTION) {
    const count = (kit.G.player.produced || []).length;
    st[UP] = d.top > 0 ? uidata.NORMAL : uidata.DISABLED;
    st[PAGE_UP] = st[UP];
    st[DOWN] = d.top + ROWS < count ? uidata.NORMAL : uidata.DISABLED;
    st[PAGE_DOWN] = st[DOWN];
  } else {
    for (const id of [UP, DOWN, PAGE_UP, PAGE_DOWN]) d.hidden[id] = true;
  }
}

/** Open the reports on report `n` (report.ARMY ... report.WINNING). */
export function open(n) {
  const G = kit.G;
  const d = { view: kit.view(DIALOG), n, top: 0 };

  d.show = (k) => { d.n = k; d.top = 0; refresh(d); };
  d.close = () => kit.pop(d);
  d.scroll = (by) => {
    const count = (G.player.produced || []).length;
    d.top = Math.max(0, Math.min(Math.max(0, count - ROWS), d.top + by));
    refresh(d);
  };

  d.draw = () => {
    const f = report.figures(G.g, G.player, d.n);
    kit.popup(R);
    if (d.n === report.PRODUCTION) {
      G.drawVectorMap(MAP.x, MAP.y, null, null, true);
    } else {
      G.drawStrategicMap(MAP.x, MAP.y);
      if (d.n === report.ARMY) drawArmyBanners();
    }
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(S_TITLE, d.n), 432, 62);
    kit.centred(kit.font(2), kit.text(S_WHAT, d.n), 432, 149);
    if (d.n === report.PRODUCTION) drawProduction(d); else drawBars(f);
    const side = G.player;
    kit.centred(kit.font(2).colours(side.colour ?? 15, side.edge ?? 0), summary(d.n, f), 432, 322);
    kit.drawControls(d.view, d.hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (!c) return;
    if (c.id === DONE) d.close();
    else if (c.id >= TAB_FIRST && c.id < TAB_FIRST + 5) d.show(c.id - TAB_FIRST);
    else if (c.id === UP) d.scroll(-1);
    else if (c.id === DOWN) d.scroll(1);
    else if (c.id === PAGE_UP) d.scroll(-ROWS);
    else if (c.id === PAGE_DOWN) d.scroll(ROWS);
  };

  d.keypressed = (key) => {
    if (key === "escape" || key === "return" || key === "kpenter") d.close();
  };

  refresh(d);
  return kit.push(d);
}
