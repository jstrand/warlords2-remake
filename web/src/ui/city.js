// The city dialog: dialog 6 over popup 2, (80, 60) 480x312.
//
// One frame, four modes, switched by the row of buttons along the foot
// (193-196) -- 7204:0000 takes the mode as its argument:
//
//   0  Info        any city; what anyone can see of it
//   1  City        Rename, Raze, Build Prod
//   2  Production  what it builds
//   3  Vector      where what it builds goes
//
// Modes 1-3 are only for a city of the side's own (7204:03a9). The left half
// is the strategic map; the right half is auto_ui_city_info (7204:06de), one
// case per mode, every position out of its data segment (4125:0ea0 on).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as input from "./input.js";
import * as buy from "./buyprod.js";
import * as infobox from "./infobox.js";
import * as tutorial from "./tutorial.js";
import * as game from "../warlords/game.js";
import * as scn from "../warlords/scn.js";
import * as uidata from "../warlords/uidata.js";
import * as keys from "../keys.js";
import { fmt } from "../util.js";

export const INFO = 0, CITY = 1, PRODUCTION = 2, VECTOR = 3;

const R = { x: 80, y: 60, w: 480, h: 312 };     // popup 2
const MAP = { x: 80, y: 60, w: 224, h: 312 };   // area screen 3, region 6
const DIALOG = 6;
const DONE = 192, DONE_PROD = 201, STOP = 202;
const MODE_FIRST = 193;
const SLOT_FIRST = 197;                          // 197-200, Production only
const RENAME = 203, BUILD = 204, RAZE = 205;
const V_SEND = 210, V_MOVE = 211, V_ALL = 212;
const V_SEND_LIT = 214, V_MOVE_LIT = 215, V_ALL_LIT = 216;

const S_STATS = 0x74, S_CURRENT = 0x75;
const S_RENAME = 0x77, S_RAZE = 0x78, S_BUILD = 0x79;
const S_VECTOR = 0x9a, S_HELP1 = 0x9b, S_HELP2 = 0x9c;

// Rename and Raze keep their words in the executable (4125:1003, 4125:0a6c).
const RENAME_TITLE = "Rename City";
const RENAME_LINES = ["Type the new name for", "this city"];
const RAZE_TITLE = "Raze City";
const RAZE_LINES = ["Are you sure that you", "want to", "raze %s?", "You won't be popular!"];

const SHIELD_L = [312, 102], SHIELD_R = [512, 102];
const STAT_AT = [[356, 106], [356, 126], [356, 150]];
const TEXT_AT = [[310, 259], [310, 279], [310, 299]];
const INFO_SLOTS = [[320, 190], [376, 190], [432, 190], [488, 190]];
const PROD_SLOTS = [[312, 142], [360, 142], [408, 142], [456, 142]];
const CITY_TEXT = [[376, 183], [376, 203], [376, 231], [376, 251], [376, 279], [376, 299]];

function owned(d) {
  const G = kit.G;
  return d.city.ownerIndex === G.player.index && game.sideCities(G.g, G.player).length > 0;
}

function capitalOf(city) {
  for (const s of kit.G.g.map.sides) if (s.capital === city) return s.index;
  return null;
}

/** The small shield (8611:0bf7, size 4): BSHIELD.PCK's bottom row. */
function smallShield(side, x, y) {
  const G = kit.G;
  gfx.setColor(1, 1, 1);
  gfx.draw(G.shieldImg, gfx.newQuad(side * 32, 36, 32, 23), x, y);
}

function building(city) {
  return city.producing != null ? city.slots[city.producing] : null;
}

/** Income, defence and owner (7204:0727, :1769). */
function stats(city) {
  const f = kit.font(2);
  gfx.setColor(1, 1, 1);
  const razed = city.razed;
  f.draw(fmt(kit.text(S_STATS, 0), razed ? 0 : city.income), STAT_AT[0][0], STAT_AT[0][1]);
  f.draw(fmt(kit.text(S_STATS, 1), razed ? 0 : city.defence), STAT_AT[1][0], STAT_AT[1][1]);
  let owner;
  if (razed) owner = kit.text(S_STATS, 2);
  else if (city.ownerIndex == null) owner = kit.text(S_STATS, 3);
  else owner = fmt(kit.text(S_STATS, 4), kit.G.g.map.sides[city.ownerIndex].name);
  f.draw(owner, STAT_AT[2][0], STAT_AT[2][1]);
}

function shields(city) {
  const side = city.ownerIndex ?? 8;
  kit.shield(side, SHIELD_L[0], SHIELD_L[1]);
  kit.shield(side, SHIELD_R[0], SHIELD_R[1]);
}

function tile(mx, my, x, y) {
  const G = kit.G;
  const t = scn.tileAt(G.g.map, mx, my);
  const sheet = Math.floor(t / 96);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.sheets[sheet], G.tileQuads[sheet][t % 96], x, y);
}

// 0: Info -- the production list, or a picture of the city where it may not
// be seen (7204:0a66), and the city's three lines of description.
function drawInfo(d) {
  const G = kit.G, city = d.city;
  shields(city);
  stats(city);
  const owner = city.ownerIndex ?? 8;
  if (!city.razed && (G.g.map.options.viewProduction === 0 || city.ownerIndex === G.player.index)) {
    INFO_SLOTS.forEach((at, i) => {
      const slot = city.slots[i];
      kit.army(slot ? slot.type : null, owner, at[0], at[1], 1);
    });
  } else {
    kit.setPal(0);
    kit.outline(311, 169, 242, 88);
    kit.bevel(310, 168, 244, 90, 4, 2);
    gfx.setColor(1, 1, 1);
    if (G.cityBack) gfx.draw(G.cityBack, gfx.newQuad(0, 0, 240, 86), 312, 170);
    tile(city.x, city.y, 392, 176);
    tile(city.x, city.y + 1, 392, 216);
    tile(city.x + 1, city.y, 432, 176);
    tile(city.x + 1, city.y + 1, 432, 216);
  }
  const cap = capitalOf(city);
  if (cap != null) smallShield(cap, 408, 170);
  const lines = G.g.map.cityText[city.index] || [];
  const f = kit.font(2);
  gfx.setColor(1, 1, 1);
  TEXT_AT.forEach((at, i) => { if (lines[i]) f.draw(lines[i], at[0], at[1]); });
}

// 1: City -- the three buttons, each with two lines beside it.
function drawCityMode(d) {
  shields(d.city);
  stats(d.city);
  const f = kit.font(2);
  gfx.setColor(1, 1, 1);
  let k = 0;
  for (const group of [S_RENAME, S_RAZE, S_BUILD]) {
    for (let i = 0; i <= 1; i++) {
      f.draw(kit.text(group, i), CITY_TEXT[k][0], CITY_TEXT[k][1]);
      k++;
    }
  }
}

function countdown(city) {
  if (city.producing != null) return fmt("%dt", city.countdown || 0);
  return "-";
}

// 2: Production (7204:0ef8).
function drawProduction(d) {
  const G = kit.G, city = d.city;
  const f = kit.font(2);
  const owner = city.ownerIndex ?? 8;
  const cap = capitalOf(city);
  if (cap != null) smallShield(cap, 312, 110);
  gfx.setColor(1, 1, 1);
  kit.right(f, kit.text(S_CURRENT, 0), 408, 110);
  const b = building(city);
  kit.army(b ? b.type : null, owner, 416, 104, 1);
  const text = countdown(city);
  gfx.setColor(1, 1, 1);
  if (city.vectorTo != null) {
    let where;
    if (city.vectorTo === game.STANDARD) {
      where = kit.text(S_CURRENT, game.standardAt(G.g, G.player)[0] != null ? 2 : 3);
    } else {
      const dest = G.g.map.cities[city.vectorTo];
      where = dest ? dest.name : kit.text(S_CURRENT, 3);
    }
    f.draw(text + kit.text(S_CURRENT, 1), 456, 102);
    f.draw(where, 456, 122);
  } else {
    f.draw(text, 456, 110);
  }

  let chosen = null;
  PROD_SLOTS.forEach((at, i) => {
    const slot = city.slots[i];
    const mine = slot && slot.type === d.chosen;
    if (mine) chosen = slot;
    kit.army(slot ? slot.type : null, owner, at[0], at[1], mine ? G.player.index + 2 : 1);
  });

  if (chosen) {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.bigArmy, 320, 182);
    const x = 320 + 136;                                // 4125:060a
    f.draw(chosen.name, x, 182);
    f.draw(fmt("Time: %d", chosen.time), x, 212);
    f.draw(fmt("Cost: %d", chosen.cost), x, 232);
    f.draw(fmt("Strength: %d", chosen.strength), x, 252);
    f.draw(fmt("Move: %d", chosen.move), x, 272);
  }
}

// 3: Vector (7204:12b7).
function drawVector(d) {
  const G = kit.G, city = d.city;
  const f = kit.font(2);
  const owner = city.ownerIndex ?? 8;
  gfx.setColor(1, 1, 1);
  kit.right(f, kit.text(S_VECTOR, 0), 360, 109);
  const b = building(city);
  kit.army(b ? b.type : null, owner, 368, 103, 1);
  gfx.setColor(1, 1, 1);
  f.draw(city.producing != null ? fmt(kit.text(S_VECTOR, 1), city.countdown || 0) : "-", 408, 109);

  let out = null, onRoad = null;
  const next = [], after = [];
  for (const a of G.g.armies) {
    if (a.transit && a.owner === G.player.index) {
      if (a.homeCity === city.index && a.transit.dest === city.vectorTo) {
        if (a.transit.turns >= 2) out = a; else onRoad = a;
      }
      if (a.transit.dest === city.index && a.homeCity !== city.index) {
        const row = a.transit.turns >= 2 ? after : next;
        if (row.length < 4) row.push(a);
      }
    }
  }
  kit.army(out ? out.type : null, owner, 432, 103, 1);
  kit.army(onRoad ? onRoad.type : null, owner, 472, 103, 1);

  gfx.setColor(1, 1, 1);
  kit.right(f, kit.text(S_VECTOR, 2), 392, 155);
  kit.right(f, kit.text(S_VECTOR, 3), 392, 188);
  for (let i = 0; i < 4; i++) {
    const x = 400 + i * 40;
    kit.army(next[i] ? next[i].type : null, G.player.index, x, 149, 1);
    kit.army(after[i] ? after[i].type : null, G.player.index, x, 182, 1);
  }

  gfx.setColor(1, 1, 1);
  const a0 = d.sub === 1 ? 2 : 0;
  f.draw(kit.text(S_HELP1, a0), 368, 221);
  f.draw(kit.text(S_HELP1, a0 + 1), 368, 241);
  const b0 = d.sub === 2 ? 2 : 0;
  f.draw(kit.text(S_HELP2, b0), 368, 272);
  f.draw(kit.text(S_HELP2, b0 + 1), 368, 292);
}

const DRAW = { [INFO]: drawInfo, [CITY]: drawCityMode, [PRODUCTION]: drawProduction, [VECTOR]: drawVector };

/** Which controls this mode shows, and in what state (7204:03a9). */
function refresh(d) {
  const st = d.view.state;
  const mine = owned(d);
  for (let i = 0; i <= 3; i++) {
    const id = MODE_FIRST + i;
    if (i === d.mode) st[id] = uidata.ACTIVE;
    else if (i > 0 && !mine) st[id] = uidata.DISABLED;
    else st[id] = uidata.NORMAL;
  }
  const shown = { 193: true, 194: true, 195: true, 196: true };
  if (d.mode === PRODUCTION) {
    for (let i = 0; i <= 3; i++) { shown[SLOT_FIRST + i] = true; st[SLOT_FIRST + i] = uidata.NORMAL; }
    shown[DONE_PROD] = true; st[DONE_PROD] = uidata.NORMAL;
    shown[STOP] = true;
    st[STOP] = d.city.producing != null ? uidata.NORMAL : uidata.DISABLED;
  } else {
    shown[DONE] = true; st[DONE] = uidata.NORMAL;
  }
  if (d.mode === CITY) {
    for (const id of [RENAME, RAZE, BUILD]) { shown[id] = true; st[id] = uidata.NORMAL; }
  }
  if (d.mode === VECTOR) {
    const G = kit.G;
    const all = G.vectorSeeAll ? V_ALL_LIT : V_ALL;
    shown[all] = true; st[all] = uidata.NORMAL;
    const send = d.sub === 1 ? V_SEND_LIT : V_SEND;
    shown[send] = true;
    st[send] = (d.sub === 1 || d.city.producing != null) ? uidata.NORMAL : uidata.DISABLED;
    const mv = d.sub === 2 ? V_MOVE_LIT : V_MOVE;
    shown[mv] = true;
    st[mv] = (d.sub === 2 || game.vectoredTo(G.g, d.city).length > 0) ? uidata.NORMAL : uidata.DISABLED;
  }
  d.hidden = {};
  for (const c of d.view.dialog.controls) if (!shown[c.id]) d.hidden[c.id] = true;
}

/** Open the dialog on a city: Production for one of your own by default,
 *  Info for any other. */
export function open(city, mode) {
  const G = kit.G;
  if (!G.cityView) G.cityView = kit.view(DIALOG);
  const d = { city, view: G.cityView };
  G.city = city;

  d.setMode = (m) => {
    if (m !== INFO && city.ownerIndex !== G.player.index) m = INFO;
    // 7204:0000: going into Production chooses what the city builds now
    if (m === PRODUCTION && d.mode !== PRODUCTION) {
      const b = building(city);
      d.chosen = b ? b.type : null;
    }
    d.mode = m;
    d.sub = 0;
    G.cityMode = m;
    refresh(d);
  };

  d.close = () => {
    kit.pop(d);
    G.city = null;
    tutorial.show("select");                       // 7204:0321
  };

  /** Choose what the city builds: slot n of the list, from 0 (7087:00c8). */
  d.pick = (n) => {
    const slot = city.slots[n];
    if (slot) d.chosen = slot.type;
    if (d.chosen == null) return;
    city.slots.forEach((s, i) => { if (s.type === d.chosen) game.setProduction(G.g, city, i); });
    refresh(d);
  };

  d.stop = () => {
    game.setProduction(G.g, city, null);
    d.chosen = null;
    refresh(d);
  };

  d.rename = () => {
    input.open({
      title: RENAME_TITLE, lines: RENAME_LINES, text: city.name,
      maxChars: 15, maxWidth: 128,                     // 7204:2013
      ok: (name) => { game.renameCity(G.g, city, name); d.setMode(CITY); },
      cancelled: () => d.setMode(CITY),
    });
  };

  d.raze = () => {
    const lines = RAZE_LINES.map((l, i) => (i === 2 ? fmt(l, city.name) : l));
    input.open({
      title: RAZE_TITLE, lines, confirm: true,
      ok: () => {
        game.raze(G.g, G.player, city, null, true);
        if (G.stratDirty) G.stratDirty();
        d.setMode(INFO);
      },
      cancelled: () => d.setMode(CITY),
    });
  };

  d.build = () => buy.open(city, () => d.setMode(CITY));

  d.draw = () => {
    kit.popup(R);
    if (d.mode === VECTOR && G.drawVectorMap) {
      let filter = null;
      if (d.sub === 1) filter = -1;
      else if (d.sub === 2) filter = game.vectoredTo(G.g, city).length;
      G.drawVectorMap(MAP.x, MAP.y, city, filter, G.vectorSeeAll && d.sub === 0);
    } else if (G.drawStrategicPanel) {
      G.drawStrategicPanel(MAP.x, MAP.y, city);
    }
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), city.name, 432, 62);
    DRAW[d.mode](d);
    kit.drawControls(d.view, d.hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (c) {
      if (c.id === DONE || c.id === DONE_PROD) d.close();
      else if (c.id >= MODE_FIRST && c.id < MODE_FIRST + 4) d.setMode(c.id - MODE_FIRST);
      else if (c.id >= SLOT_FIRST && c.id < SLOT_FIRST + 4) d.pick(c.id - SLOT_FIRST);
      else if (c.id === STOP) d.stop();
      else if (c.id === RENAME) d.rename();
      else if (c.id === RAZE) d.raze();
      else if (c.id === BUILD) d.build();
      else if (c.id === V_SEND || c.id === V_SEND_LIT) { d.sub = d.sub === 1 ? 0 : 1; refresh(d); }
      else if (c.id === V_MOVE || c.id === V_MOVE_LIT) { d.sub = d.sub === 2 ? 0 : 2; refresh(d); }
      else if (c.id === V_ALL || c.id === V_ALL_LIT) { G.vectorSeeAll = !G.vectorSeeAll; refresh(d); }
      return;
    }
    if (x >= MAP.x && x < MAP.x + MAP.w && y >= MAP.y && y < MAP.y + MAP.h) {
      const mx = Math.floor((x - MAP.x) / 2), my = Math.floor((y - MAP.y) / 2);
      if (d.mode === VECTOR) d.mapClick(mx, my); else d.pickOnMap(mx, my);
    }
  };

  /** Move the dialog to another city, in the mode it is in (7204:0000). */
  d.switchTo = (target, m) => {
    kit.pop(d);
    G.city = null;
    open(target, m ?? d.mode);
  };

  /** A click on the map in Info, City or Production (7204:1afa). */
  d.pickOnMap = (mx, my) => {
    const shift = keys.isDown("lshift", "rshift");
    if (d.mode === PRODUCTION && shift) {
      d.sendTo(mx, my);
      refresh(d);
      return;
    }
    let target;
    if (d.mode === INFO) target = game.nearestCity(G.g, mx, my, null, G.player);
    else if (d.mode === PRODUCTION) target = game.nearestCity(G.g, mx, my, G.player);
    else target = game.nearestCity(G.g, mx, my, G.player, G.player);
    if (target && target !== city) d.switchTo(target);
  };

  /** Send what this city builds to the side's city nearest (mx, my), or its
   *  planted standard if that is nearer (828e:0651). */
  d.sendTo = (mx, my) => {
    if (city.producing == null) return;
    const target = game.nearestCity(G.g, mx, my, G.player);
    if (!target) return;
    const [sx, sy] = game.standardAt(G.g, G.player);
    const toStandard = sx != null && Math.max(Math.abs(sx - mx), Math.abs(sy - my))
                       < Math.max(Math.abs(target.x - mx), Math.abs(target.y - my));
    if (toStandard) game.vectorToStandard(G.g, city, G.player);
    else game.vector(G.g, city, target);
  };

  /** A click on the map in Vector mode (7087:072e, 7087:028b). */
  d.mapClick = (mx, my) => {
    const target = game.nearestCity(G.g, mx, my, G.player);
    if (!target) return;
    const shift = keys.isDown("lshift", "rshift");
    const sub = d.sub;
    if (shift || sub === 1) {
      d.sendTo(mx, my);
      if (shift) { d.sub = 0; refresh(d); return; }
    } else if (sub === 2) {
      const incoming = game.vectoredTo(G.g, city);
      const there = game.vectoredTo(G.g, target);
      let ok = incoming.length > 0 && incoming.length + there.length <= game.MAX_VECTORED_TO;
      if (incoming.includes(target)) ok = false;
      if (ok) for (const c of incoming) c.vectorTo = target.index;
    }
    d.sub = 0;
    if (sub !== 1 && target !== city) {
      d.switchTo(target, VECTOR);
      return;
    }
    refresh(d);
  };

  // the right button on a production slot (sub-ids 1-4, 54bd:00df)
  d.info = (sub, sx, sy) => {
    const slot = city.slots[sub - 1];
    if (!slot) return false;
    infobox.armyType(sx, sy, slot.type, slot);
    return true;
  };

  /** The right button off the controls (1726:0009, screen 3). */
  d.rightpressed = (x, y, sx, sy) => {
    if (x >= MAP.x && x < MAP.x + MAP.w && y >= MAP.y && y < MAP.y + MAP.h) {
      const first = { [INFO]: 0, [CITY]: 0, [PRODUCTION]: 4, [VECTOR]: 6 }[d.mode];
      infobox.lines(sx, sy, kit.text(0x7a, first), kit.text(0x7a, first + 1));
      return;
    }
    if (d.mode === INFO && x >= 308 && x < 532 && y >= 180 && y < 230) {
      if (city.razed || (G.g.map.options.viewProduction !== 0 && city.ownerIndex !== G.player.index)) {
        infobox.lines(sx, sy, city.name, "A picture of the city!");          // 4125:0fe7
        return;
      }
      const slot = city.slots[Math.min(3, Math.floor((x - 308) / 56))];
      if (slot) infobox.armyType(sx, sy, slot.type, slot, city.ownerIndex ?? 8);
      else infobox.lines(sx, sy, city.name, "Info about city production"); // 4125:0fcc
    }
  };

  d.keypressed = (key) => {
    if (key === "escape" || key === "return" || key === "kpenter") d.close();
  };

  kit.push(d);
  if (mode == null) mode = city.ownerIndex === G.player.index ? PRODUCTION : INFO;
  d.mode = null;
  d.setMode(mode);
  // the tutorial on production (7204:025d)
  const moments = ["prod"];
  if (game.sideCities(G.g, G.player).length >= 2) moments.push("prod2");
  tutorial.chain(moments);
  return d;
}
