// History > City, Events, Gold, Winners (6d51:0000 with 0-3), and Triumphs.
//
// Plays back what warlords/history.js recorded, a round at a time. With
// nothing on record yet it says so (group 93). Popup 10, (32, 60) 576x312,
// dialog 19 (6d51:0096): the strategic map with the city shields as they
// stood that turn, the title, the four tabs (352-355); for City, Gold and
// Winners a graph of every side, one step a turn, scaled to the largest on
// record (6d51:034a); for Events that round's deeds and a timeline
// (6d51:16ba, 18da). The arrows (357/358) step a turn; a click on the graph
// or the timeline picks the turn under it (6d51:0908). Done (356).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as history from "../warlords/history.js";
import * as uidata from "../warlords/uidata.js";
import * as searchUi from "./search.js";
import { fmt } from "../util.js";

const R = { x: 32, y: 60, w: 576, h: 312 };      // popup 10
const DIALOG = 19, TAB = 352, DONE = 356, PREV = 357, NEXT = 358;
const GX = 300, GY = 150, GW = 292, GH = 140;    // 4125:0dc6
const TL = { x: 275, y: 322, w: 314, h: 11 };    // 4125:0dd6
export const CITY = 0, EVENTS = 1, GOLD = 2, WINNERS = 3;
const FLOOR = { 0: 10, 2: 500, 3: 100 };

function values(r, mode) {
  if (mode === CITY) return r.cities;
  if (mode === GOLD) return r.gold;
  return r.score;
}

const hline = (x, y, n) => gfx.rectangle("fill", x, y, n, 1);
const vline = (x, y, n) => gfx.rectangle("fill", x, y, 1, n);
const line = kit.line;

export function open(mode) {
  const G = kit.G;
  const recs = G.g.history || [];
  const n = Math.min(recs.length, history.LAST_TURN);
  if (n < 1) return searchUi.message(kit.text(0x5d, 0), kit.text(0x5d, 1));
  // the turn shown counts from 1, as the original's does
  const d = { view: kit.view(DIALOG), mode, turn: n };
  const side = G.player;
  const rec = (t) => recs[t - 1];

  const refresh = () => {
    const st = d.view.state;
    for (let i = 0; i <= 3; i++) st[TAB + i] = i === d.mode ? uidata.ACTIVE : uidata.NORMAL;
    st[PREV] = d.turn === 1 ? uidata.DISABLED : uidata.NORMAL;
    st[NEXT] = d.turn === n ? uidata.DISABLED : uidata.NORMAL;
    st[DONE] = uidata.NORMAL;
  };

  const tx = (t) => GX + Math.floor(t * GW / n);

  const drawGraph = () => {
    let top = FLOOR[d.mode];
    for (let t = 1; t <= n; t++) {
      for (let s = 0; s < 8; s++) top = Math.max(top, values(rec(t), d.mode)[s] || 0);
    }
    const axes = (c, o) => {
      kit.setPal(c);
      vline(GX - o, GY - o, GH);
      hline(GX - o, GY + GH - 1 - o, GW);
      for (const y of [GY, GY + Math.floor(GH / 2), GY + GH - 1]) hline(GX - 4 - o, y - o, 4);
      for (const x of [GX, GX + Math.floor(GW / 2), GX + GW - 1]) vline(x - o, GY + GH - 1 - o, 4);
    };
    axes(0, 1);
    axes(15, 0);
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    kit.right(f, fmt("%d", top), GX - 8, GY - 6);
    kit.right(f, "0", GX - 8, GY + GH - 8);
    kit.centred(f, "0", GX, GY + GH + 8);
    kit.right(f, fmt("%d", n), GX + GW + 8, GY + GH + 8);
    kit.centred(f, "Turns", GX + Math.floor(GW / 2), GY + GH + 8);
    const px = [], py = [];
    for (let s = 0; s < 8; s++) { px[s] = GX + 1; py[s] = GY + GH - 2; }
    for (let t = 1; t <= n; t++) {
      const x = tx(t);
      const v = values(rec(t), d.mode);
      for (let s = 0; s < 8; s++) {
        const y = GY + GH - 1 - Math.floor((v[s] || 0) / top * GH);
        const sd = G.g.map.sides[s];
        line(sd ? sd.colour : 15, px[s], py[s], x, y);
        px[s] = x; py[s] = y;
      }
    }
    const x = tx(d.turn);
    line(13, x, GY - 2, x, GY + GH + 6);
    const r = rec(d.turn);
    let text;
    if (d.mode === WINNERS) {
      let best = 0, lead = 0;
      for (let s = 0; s < 8; s++) if ((r.score[s] || 0) > best) { best = r.score[s]; lead = s; }
      const sd = G.g.map.sides[lead];
      text = fmt(kit.text(0x5c, 3), d.turn, sd ? sd.name : "");
    } else {
      text = fmt(kit.text(0x5c, d.mode), d.turn, values(r, d.mode)[side.index] || 0);
    }
    gfx.setColor(1, 1, 1);
    kit.centred(f, text, 432, 318);
  };

  const shield = (s, x, y) => {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.atransShields, gfx.newQuad(112 + 16 * Math.floor(s / 4), 94 + 14 * (s % 4), 16, 14), x, y);
  };

  const eventText = (e) => {
    const t = (i, ...a) => fmt(kit.text(0x5e, i), ...a);
    const sides = G.g.map.sides, cities = G.g.map.cities;
    const sideName = (i) => (sides[i] ? sides[i].name : "");
    const cityName = (i) => (cities[i] ? cities[i].name : "");
    const ty = e.type, v1 = e.v1, v2 = e.v2;
    if (ty === history.EMERGES) return t(0, e.name, cityName(v1));
    if (ty === history.KILLED) {
      if (v1 === history.IN_BATTLE) return t(1, e.name);
      if (v1 === history.SEARCHING) return t(2, e.name);
      return t(3, e.name, cityName(v1));
    }
    if (ty === history.QUEST_DONE) return t(4, e.name);
    if (ty === history.QUEST_GIVEN) return t(5, e.name);
    if (ty === history.VANQUISHED) return t(6, sideName(v1));
    if (ty === history.WON) {
      if (v1 === history.BY_NAME) return t(7, e.name, cityName(v2));
      return t(8, sideName(v1), cityName(v2));
    }
    if (ty === history.FINDS) {
      let what;
      if (v1 === history.ALLIES) what = kit.text(0x5e, 13);
      else if (v1 === history.SAGE) what = kit.text(0x5e, 14);
      else if (v1 === history.GOLD) what = kit.text(0x5e, 15);
      else for (const it of G.g.map.items) if (it.index === v1) what = it.name;
      return t(9, e.name, what || "");
    }
    if (ty === history.VICTORIOUS) return t(10, sideName(v1));
    if (ty === history.TREACHERY) return t(16, sideName(v1));
    if (ty === history.WAR) return t(11, sideName(v1));
    if (ty === history.PEACE) return t(12, sideName(v1));
    return "";
  };

  const drawEvents = () => {
    const f = kit.font(2);
    (rec(d.turn).events || []).forEach((e, i) => {
      const y = 149 + 17 * i;
      shield(e.side, 264, y + 1);
      const text = eventText(e);
      gfx.setColor(1, 1, 1);
      f.draw(text, 280, y);
      if (e.type === history.WAR || e.type === history.PEACE || e.type === history.TREACHERY) {
        shield(e.v2, 280 + Math.floor(f.width(text) / 8) * 8 + 16, y + 1);
      }
    });
    kit.bevel(272, 319, 320, 17, 4, 2);
    kit.bevel(273, 320, 318, 15, 4, 2);
    kit.setPal(0);
    kit.outline(274, 321, 316, 13);
    const w = Math.min(TL.w, Math.floor(d.turn * TL.w / n));
    kit.setPal(side.colour ?? 15);
    gfx.rectangle("fill", TL.x, TL.y, w, TL.h);
    if (w < TL.w - 8) {
      kit.setPal(side.edge ?? 0);
      gfx.rectangle("fill", TL.x + w, TL.y, TL.w - w, TL.h);
    }
  };

  d.draw = () => {
    kit.popup(R);
    G.drawStrategicMap(R.x, R.y, null, false, rec(d.turn).owners);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(0x5b, d.mode), 432, 64);
    if (d.mode === EVENTS) drawEvents(); else drawGraph();
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(2), fmt("Turn %d", d.turn), 312, 350);
    kit.drawControls(d.view);
  };

  const pick = (x, x0, w) => {
    const t = Math.floor((x - x0) / w * n) + 1;
    d.turn = Math.max(1, Math.min(n, t));
  };

  d.mousepressed = (x, y) => {
    if (d.mode !== EVENTS && x >= GX && x < GX + GW && y >= GY && y < GY + 160) { pick(x, GX, GW); return refresh(); }
    if (d.mode === EVENTS && x >= TL.x && x < TL.x + TL.w && y >= TL.y && y < TL.y + TL.h) { pick(x, TL.x, TL.w); return refresh(); }
    const c = kit.controlAt(d.view, x, y);
    if (!c || d.view.state[c.id] === uidata.DISABLED) return;
    if (c.id === DONE) return kit.pop(d);
    else if (c.id === PREV) d.turn--;
    else if (c.id === NEXT) d.turn++;
    else if (c.id >= TAB && c.id < TAB + 4) d.mode = c.id - TAB;
    refresh();
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };

  refresh();
  return kit.push(d);
}

// History > Triumphs (6d51:09eb): what the side has done to each other side,
// or lost itself. Popup 0, dialog 20 (6d51:0a21): eight tabs of BUTTON.PCK
// with the sides' shields on them (360-367), then five kinds of army with
// their counts (groups 81-85 for your own losses, 86-90 for the others').
const TRI = { x: 80, y: 60, w: 480, h: 320 };    // popup 0
const TRI_DIALOG = 20, TRI_DONE = 359, TRI_TAB = 360;
const KINDS = [4, 25, 28, 5, 29];

export function triumphs() {
  const G = kit.G;
  const me = G.player.index;
  const d = { view: kit.view(TRI_DIALOG), opp: me };

  const refresh = () => {
    for (let i = 0; i < 8; i++) d.view.state[TRI_TAB + i] = i === d.opp ? uidata.ACTIVE : uidata.NORMAL;
    d.view.state[TRI_DONE] = uidata.NORMAL;
  };

  d.draw = () => {
    kit.popup(TRI);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(0x50, 0), 320, 64);
    const button = G.screen.art_for(4), bshield = G.screen.art_for(38);
    for (let i = 0; i < 8; i++) {
      const x = 128 + 48 * i;
      gfx.setColor(1, 1, 1);
      if (button) gfx.draw(button.image, gfx.newQuad(400, i === d.opp ? 40 : 0, 48, 40), x, 109);
      if (bshield) gfx.draw(bshield.image, gfx.newQuad(i * 32, 0, 32, 36), x + 8, 111);
    }
    const f = kit.font(2);
    const base = d.opp === me ? 0x51 : 0x56;
    for (let k = 0; k <= 4; k++) {
      const y = 168 + 35 * k;
      kit.army(KINDS[k], d.opp, 104, y, 1);
      const cnt = history.triumph(G.g, me, d.opp, k);
      if (cnt > 0) {
        gfx.setColor(1, 1, 1);
        f.draw(fmt(kit.text(base + k, cnt === 1 ? 0 : 1), cnt), 144, y + 9);
      }
    }
    const hidden = {};
    for (let i = 0; i < 8; i++) hidden[TRI_TAB + i] = true;
    kit.drawControls(d.view, hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === TRI_DONE) return kit.pop(d);
    if (c.id >= TRI_TAB && c.id < TRI_TAB + 8) { d.opp = c.id - TRI_TAB; refresh(); }
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };

  refresh();
  return kit.push(d);
}
