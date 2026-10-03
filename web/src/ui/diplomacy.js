// Report > Diplomacy (484e:0000): the Diplomatic Report and, behind its
// Action button, the Diplomatic Action screen. Only with Diplomacy on. Both
// are popup 11, (80, 60) 480x350.
//
// The report (484e:0039, dialog 21): a grid of every pair, each side's
// shield along the top and down the side, in each cell what DIPLOM.PCK shows
// for the state, and beside it the sides best first with their titles.
// Done (368) and Action (369).
//
// The action screen (484e:0382, dialog 22): for every other side its shield,
// the state between you, its proposal to you, and your three proposals to it
// -- peace, uneasy, war -- the chosen one lit. The buttons under them
// (372-378, 380-386, 388-394) set it (484e:0a69). OK (370); Report (371).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as diplomacy from "../warlords/diplomacy.js";
import * as uidata from "../warlords/uidata.js";
import { luaSort } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 350 };      // popup 11
const REPORT = 21, DONE = 368, ACTION = 369;
const ACT = 22, OK = 370, BACK = 371;
const OFFER = [372, 380, 388];                   // a row of 7 each

const playing = (g, i) => {
  const s = g.map.sides[i];
  return s && s.inUse;
};

function diplom(sx, sy, w, h, x, y) {
  const art = kit.G.screen.art_for(47);
  if (!art) return;
  gfx.setColor(1, 1, 1);
  gfx.draw(art.image, gfx.newQuad(sx, sy, w, h), x, y);
}

function smallShield(side, x, y) {
  const G = kit.G;
  gfx.setColor(1, 1, 1);
  gfx.draw(G.shieldsImg, gfx.newQuad(side * 40 + 24, 46, 16, 16), x, y);
}

function blank(x, y) {
  kit.setPal(0);
  kit.outline(x, y, 40, 40);
  kit.setPal(2);
  gfx.rectangle("fill", x + 1, y + 1, 38, 38);
}

function proposal(g, a, b) {
  let p = diplomacy.proposal(g, a, b);
  if (p == null) p = diplomacy.state(g, a, b);
  return p;
}

function openReport() {
  const G = kit.G;
  const g = G.g;
  const d = { view: kit.view(REPORT) };
  d.view.state[DONE] = uidata.NORMAL; d.view.state[ACTION] = uidata.NORMAL;
  const titles = diplomacy.ratings(g);
  const order = luaSort(g.sides.slice(), (p, q) => {
    const sp = p.diploScore || 0, sq = q.diploScore || 0;
    if (sp !== sq) return sp < sq;
    return p.index < q.index;
  });

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(0x6b, 0), 320, 64);
    kit.setPal(1);
    gfx.rectangle("fill", 88, 139, 1, 232);
    for (let i = 0; i <= 8; i++) gfx.rectangle("fill", 120 + 32 * i, 110, 1, 261);
    gfx.rectangle("fill", 120, 110, 256, 1);
    for (let i = 0; i <= 8; i++) gfx.rectangle("fill", 88, 139 + 29 * i, 288, 1);
    for (let i = 0; i < 8; i++) {
      if (playing(g, i)) {
        smallShield(i, 128 + 32 * i, 117);
        smallShield(i, 96, 146 + 29 * i);
      }
    }
    for (let col = 0; col < 8; col++) {
      for (let row = 0; row < 8; row++) {
        const x = 120 + 32 * col, y = 139 + 29 * row;
        if (col === row) diplom(384, 29, 32, 29, x, y);
        else if (playing(g, col) && playing(g, row)) {
          const st = diplomacy.state(g, col, row);
          if (st === diplomacy.WAR) diplom(384, 0, 32, 29, x, y);
          else if (st === diplomacy.PEACE) diplom(320, 0, 32, 29, x, y);
        }
      }
    }
    kit.setPal(0);
    kit.outline(392, 110, 160, 262);
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    kit.centred(f, kit.text(0x6d, 0), 472, 116);
    order.forEach((s, i) => {
      const y = 146 + 29 * i;
      smallShield(s.index, 400, y);
      gfx.setColor(1, 1, 1);
      f.draw(titles[s.index] || "", 432, y);
    });
    kit.drawControls(d.view);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === DONE) kit.pop(d);
    else if (c.id === ACTION) { kit.pop(d); openAction(); }
  };
  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };
  return kit.push(d);
}

function openAction() {
  const G = kit.G;
  const g = G.g;
  const me = G.player.index;
  const d = { view: kit.view(ACT) };
  for (const c of d.view.dialog.controls) d.view.state[c.id] = uidata.NORMAL;
  const others = [];
  for (let i = 0; i < 8; i++) if (i !== me) others.push(i);

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(0x6c, 0), 320, 64);
    kit.shield(me, 96, 111);
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    f.draw(G.player.name || "", 144, 122);
    [165, 205, 251, 271, 291, 311, 331].forEach((y, i) => f.draw(kit.text(0x6e, i), 104, y));
    others.forEach((other, col) => {
      const x = 272 + 40 * col;
      kit.shield(other, x, 111);
      if (!playing(g, other)) {
        for (const y of [151, 191, 245, 285, 325]) blank(x, y);
      } else {
        const st = diplomacy.state(g, other, me);
        diplom(80 + 120 * st, 45, 40, 40, x, 151);
        const theirs = proposal(g, other, me);
        if (theirs === st) blank(x, 191); else diplom(80 + 120 * theirs, 45, 40, 40, x, 191);
        const mine = proposal(g, me, other);
        for (let k = 0; k <= 2; k++) diplom(120 * k + (mine === k ? 40 : 0), 45, 40, 40, x, 245 + 40 * k);
      }
    });
    const hidden = {};
    for (let k = 0; k <= 2; k++) for (let i = 0; i <= 6; i++) hidden[OFFER[k] + i] = true;
    kit.drawControls(d.view, hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    if (c.id === OK) return kit.pop(d);
    if (c.id === BACK) { kit.pop(d); return openReport(); }
    for (let k = 0; k <= 2; k++) {
      const i = c.id - OFFER[k];
      if (i >= 0 && i < 7 && others[i] != null) diplomacy.propose(g, me, others[i], k);
    }
  };
  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };
  return kit.push(d);
}

/** Report > Diplomacy: nothing with the option off. */
export function open() {
  if (kit.G.g.map.options.diplomacy === 0) return null;
  return openReport();
}

/** The control panel's diplomacy button (183-185, 484e:0346). */
export function action() {
  if (kit.G.g.map.options.diplomacy === 0) return null;
  return openAction();
}

/** Which of the three faces the button wears (8065:0174, 484e:0cc7): 183
 *  when nobody proposes a change, 185 when every change is friendlier, 184
 *  when any is more hostile; null with the option off. */
export function buttonFor(g, me) {
  if (g.map.options.diplomacy === 0) return null;
  let friendlier = false, hostile = false;
  for (let s = 0; s < 8; s++) {
    if (s !== me) {
      const st = diplomacy.state(g, s, me), p = proposal(g, s, me);
      if (p !== st) { if (p < st) friendlier = true; else hostile = true; }
    }
  }
  if (!(friendlier || hostile)) return 183;
  return hostile ? 184 : 185;
}
