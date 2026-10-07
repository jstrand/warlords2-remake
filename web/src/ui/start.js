// The start screens: the menu the game opens on, choosing a scenario, and
// setting its sides and options up (7f77:0000, 7f77:0725, 7bab:0000).
//
// The start menu (7f77:0000, dialog 1): STARTUP0-3.PCK with New Scenario
// (100), Load Game (101), Random Map (102) and Begin (103), and the
// scenario's own PICS\SCENARIO.PCK under a bar with its name. The scenario
// starts as Erythea (4125:2a6e).
//
// Random Map (7f77:05f5) puts "A Random World" on the bar and, in place of
// the picture (7f77:0332), its settings on colour 3: four sliders --
// Water, Hills, Cities, Forest, 106-109, STARTBU.PCK at (384, 216) 30 apart
// -- each with a "?" (110-113) that leaves it to chance, the terrain set
// (104) and "Cities can produce allies" (105). Begin then has the advisor say
// "One moment..." and makes the world (random_map_setup, 7bab:10e8) on
// BSCROLL.PCK with a bar (4bed:01ff). New Scenario's choice ends it.
//
// New Scenario (7f77:058d, 0725): popup 19, NEWSCEN.PCK, dialog 29 -- the
// scenarios of SCENARIO.DAT in a black box, seven rows, and on the crystal
// ball the chosen one's name, description, cities, ruins and players.
//
// Begin (7bab:0000) sets the sides up on the main screen's own frame,
// dialog 3: a box a side with its face, its button -- Human, Knight, Lord,
// Warlord or Off -- and its Character box (7bab:0634); Begin (141), Main
// Menu (142), I am the Greatest (143) / No! I really am Normal (144), the
// presets (145-147) and Edit Options (148); Recall Options (157) and Random
// Characters (158) at the foot; the difficulty rating (7bab:0bab); the SSG
// logo (7bab:10c7); and under it all a quote on war from QUOTES.DAT
// (7bab:0f88). The options are DATA\OPTIONS.DAT's, the ones the last game
// began with, not the scenario's own (7bab:223b).
//
// Edit Options (7bab:12a2): popup 4, dialog 4 -- the ten options of group 4
// two to a row; 159-168 change one, 169-171 are the presets, OK (172).
//
// Setup Side (7bab:16ea), from a side's Character box (133-140) or its face
// (149-156): popup 4, dialog 27 -- the side's name to retype, and for a
// computer the characters of its level's deck to choose from, with the
// chosen one's description (7bab:180b). OK (457), Cancel (458).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as pck from "../warlords/pck.js";
import * as scn from "../warlords/scn.js";
import * as screen from "../warlords/screen.js";
import * as uidata from "../warlords/uidata.js";
import * as vfs from "../vfs.js";
import * as savegame from "./savegame.js";
import * as advisorUi from "./advisor.js";
import * as cues from "../warlords/cues.js";
import * as randommap from "../warlords/randommap.js";
import * as aicard from "../warlords/aicard.js";
import * as input from "./input.js";
import { Rng } from "../warlords/rng.js";
import { u16, cstr } from "../warlords/bytes.js";
import { fmt } from "../util.js";

// The random world's settings, kept for the session as the original keeps
// them (4125:28c8-28d8): on, the terrain set, allies, and each slider's
// place and whether it is set (else "?", left to chance).
const world = { on: false, terrainSet: 0, allies: false, sliders: [3, 3, 2, 3], set: [1, 1, 1, 1] };

// the ten options, in the order of group 4 and the table at 4125:23b4
const OPTION_KEYS = ["neutralCities", "diplomacy", "quests", "hiddenMap", "viewEnemies",
  "viewProduction", "intenseCombat", "quickStart", "militaryAdvisor", "randomTurns"];
const PRESETS = [                                  // 4125:2378
  [0, 0, 0, 0, 1, 0, 0, 0, 1, 0],
  [1, 1, 1, 0, 0, 0, 0, 0, 1, 0],
  [2, 1, 1, 1, 0, 1, 0, 0, 1, 0],
];

// The options being set up (4125:23b4): one table for the session, the
// Beginner preset to begin with.
const OPTIONS = PRESETS[0].slice();

/** 7bab:223b: put back the options the last game began with, ten u16s in
 *  DATA\OPTIONS.DAT -- the setup screen as it opens, and Recall Options.
 *  With no file the table is left as it is. */
function recallOptions(dataDir) {
  const s = vfs.read(dataDir + "/DATA/OPTIONS.DAT");
  if (!s) return;
  for (let i = 0; i <= 9 && 2 * i + 2 <= s.length; i++) OPTIONS[i] = u16(s, 2 * i);
}

/** 7bab:2289: keep them for next time, as Begin starts the game. */
function keepOptions(dataDir) {
  const b = new Uint8Array(20);
  for (let i = 0; i <= 9; i++) { b[2 * i] = OPTIONS[i] & 255; b[2 * i + 1] = (OPTIONS[i] >> 8) & 255; }
  vfs.write(dataDir + "/DATA/OPTIONS.DAT", b);
}

/** SCENARIO.DAT's records. */
export function scenarios(dataDir) {
  const s = vfs.read(dataDir + "/DATA/SCENARIO.DAT");
  if (!s) return [];
  const out = [];
  for (let i = 0; i < Math.floor(s.length / 84); i++) {
    const o = 84 * i;
    out.push({
      name: cstr(s, o, 20), dir: cstr(s, o + 20, 8), text: cstr(s, o + 28, 30),
      cities: u16(s, o + 76), ruins: u16(s, o + 78), players: u16(s, o + 80),
    });
  }
  return out;
}

// where the quote's lines go, centred (4125:253c)
const QUOTE_Y = [366, 384, 402, 420, 440];

/** A quote from QUOTES.DAT, as 7bab:0f88 picks it: a two-digit count, then
 *  records of a three-digit length and that many bytes less one, lines split
 *  by "|" and each closed by one. dice(1, count) says which. */
export function quote(dataDir, roll) {
  const s = vfs.read(dataDir + "/DATA/QUOTES.DAT");
  if (!s) return [];
  const str = (o, n) => String.fromCharCode(...s.subarray(o, o + n));
  const count = parseInt(str(0, 2), 10) || 0;
  if (count < 1) return [];
  const pick = roll(count);
  let o = 4, text = "";
  for (let i = 0; i < pick && o + 3 <= s.length; i++) {
    const n = parseInt(str(o, 3), 10) || 0;
    text = str(o + 3, n - 1);
    o += 3 + n + 1;
  }
  // only text closed by a "|" is drawn
  return text.split("|").slice(0, -1).slice(0, QUOTE_Y.length);
}

function image(G, path, key) {
  G.startArt = G.startArt || {};
  if (G.startArt[path] === undefined) {
    try {
      G.startArt[path] = pck.toImage(path, G.palette, key)[0];
    } catch (e) {
      G.startArt[path] = false;
    }
  }
  return G.startArt[path] || null;
}

function newSetup(G, sc) {
  const map = scn.load(G.dataDir + "/" + sc.dir.toUpperCase(), sc.dir.toUpperCase());
  // 7bab:0000: the tutorial plays with the Beginner options, any other
  // scenario with the last game's
  if (map.options.tutorial) for (let i = 0; i <= 9; i++) OPTIONS[i] = PRESETS[0][i];
  else recallOptions(G.dataDir);
  const st = { sc, map, options: OPTIONS, sides: [], greatest: false };
  for (const s of map.sides) {
    st.sides[s.index] = { inUse: s.inUse, name: s.name, colour: s.colour, edge: s.edge,
                          computer: s.computer, level: s.computer ? (s.level || 0) : 0,
                          card: s.card || 0 };
  }
  return st;
}

function presetMatches(st, p) {
  for (let i = 0; i <= 5; i++) if (st.options[i] !== PRESETS[p][i]) return false;
  return true;
}

/** 7bab:0bab: the options' weight and the computers' strength, a percent. */
function rating(st) {
  const o = st.options;
  const cx = Math.min(20, o[0] * 4 + o[1] * 4 + o[2] * 3 + o[3] * 4 - o[4] + o[5]);
  let sum = 0, n = 0;
  for (let i = 0; i < 8; i++) {
    const s = st.sides[i];
    if (s && s.inUse && s.computer && s.level !== 3) { sum += s.level + 1; n++; }
  }
  let v = n === 0 ? 80 : Math.floor(sum * 80 / (n * 3));
  if (v >= 78) v = 80;
  return v + cx;
}

function openOptions(G, st, after) {
  const R = { x: 120, y: 50, w: 400, h: 360 };   // popup 4
  const d = { view: kit.view(4) };
  const refresh = () => {
    const s = d.view.state;
    for (let i = 0; i <= 9; i++) s[159 + i] = uidata.NORMAL;
    for (let p = 0; p <= 2; p++) s[169 + p] = presetMatches(st, p) ? uidata.ACTIVE : uidata.NORMAL;
    s[172] = uidata.NORMAL;
  };
  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), kit.text(8, 0), 320, 55);
    const f = kit.font(2);
    [111, 250].forEach((y, k) => {
      kit.setPal(15); gfx.rectangle("fill", 160, y, 320, 1);
      kit.setPal(0); gfx.rectangle("fill", 161, y + 1, 320, 1);
      const t = kit.text(9 + k, 0);
      const w = f.width(t);
      kit.setPal(3); gfx.rectangle("fill", 320 - Math.floor(w / 2) - 4, y, w + 8, 2);
      gfx.setColor(1, 1, 1);
      kit.centred(f, t, 320, y - 7);
    });
    for (let i = 0; i <= 9; i++) {
      let x, y;
      if (i < 6) { x = i % 2 === 0 ? 128 : 320; y = 131 + 30 * Math.floor(i / 2); }
      else { x = i % 2 === 0 ? 128 : 320; y = 270 + 30 * Math.floor((i - 6) / 2); }
      gfx.setColor(1, 1, 1);
      f.draw(kit.text(4, i), x, y);
      const v = st.options[i];
      const word = i === 0 ? kit.text(5, v) : (v !== 0 ? "On" : "Off");
      f.colours(7, 0).draw(word, x + 128, y);
    }
    kit.drawControls(d.view);
  };
  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    const id = c.id;
    if (id === 172) { kit.pop(d); return after(); }
    if (id >= 159 && id <= 168) {
      const i = id - 159;
      if (i === 0) st.options[0] = (st.options[0] + 1) % 3;
      else st.options[i] = st.options[i] !== 0 ? 0 : 1;
    } else if (id >= 169 && id <= 171) {
      for (let k = 0; k <= 5; k++) st.options[k] = PRESETS[id - 169][k];
    }
    refresh();
  };
  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") { kit.pop(d); after(); }
  };
  refresh();
  return kit.push(d);
}

// Setup Side's controls (dialog 27)
const SIDE_OK = 457, SIDE_CANCEL = 458, SIDE_NAME = 459;
const SIDE_UP = 460, SIDE_DOWN = 461, SIDE_PAGE_UP = 462, SIDE_PAGE_DOWN = 463, SIDE_ROW = 464;
const NAME_BOX = { x: 229, y: 138, w: 188, h: 22 };
// a Knight's characters are listed in colour 5, a Lord's 7, a Warlord's 9
const LEVEL_INK = [5, 7, 9];

/** Setup Side (7bab:16ea, drawn by 7bab:180b): popup 4, dialog 27. Side `i`'s
 *  name, retyped in the field (15 characters, 128 pixels, 7bab:1f8a); for a
 *  computer its level's characters, five rows at a time, and the chosen
 *  one's description from its .DSC. A human has no character: "N/A". OK
 *  keeps what was done, Cancel puts the name and character back. */
function openSide(G, st, i, after) {
  const R = { x: 120, y: 50, w: 400, h: 360 };   // popup 4
  const s = st.sides[i];
  const d = { view: kit.view(27), editing: null };
  const saved = { name: s.name, card: s.card };
  // Off has no deck: its letter is past the end of "KLW"
  const deck = s.computer && s.level < 3 ? aicard.deck(G.dataDir, s.level) : [];
  // the five rows' cards, -1 for none; the chosen one on the last row
  // when it is past the first five
  const rows = [];
  for (let r = 0; r < 5; r++) rows[r] = r < deck.length ? r : -1;
  if (s.card > 4) for (let r = 0; r < 5; r++) rows[r] += s.card - 4;
  let desc = null, descCard = -1;

  // 7bab:1ae4
  const refresh = () => {
    const v = d.view.state;
    for (let r = 0; r < 5; r++) v[SIDE_ROW + r] = rows[r] < 0 ? uidata.DISABLED : uidata.NORMAL;
    v[SIDE_OK] = uidata.NORMAL; v[SIDE_CANCEL] = uidata.NORMAL; v[SIDE_NAME] = uidata.NORMAL;
    const up = s.computer && rows[0] >= 1;
    const down = s.computer && rows[4] > 0 && rows[4] < deck.length - 1;
    v[SIDE_UP] = v[SIDE_PAGE_UP] = up ? uidata.NORMAL : uidata.DISABLED;
    v[SIDE_DOWN] = v[SIDE_PAGE_DOWN] = down ? uidata.NORMAL : uidata.DISABLED;
  };

  // an empty name is not taken: the setup screen knows a side by its name
  const keepName = () => {
    if (d.editing.text !== "") s.name = d.editing.text;
    d.editing = null;
  };

  const close = (ok) => {
    if (!ok) { s.name = saved.name; s.card = saved.card; }   // 7bab:1fd7
    kit.pop(d);
    after();
  };

  d.draw = () => {
    kit.popup(R);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), "Setup Side", 320, 55);
    const c = s.colour ?? 15, e = s.edge ?? 0;
    const f = kit.font(2);
    // two boxes, each outlined in the side's edge colour and twice more in
    // its colour, a pixel further up and left each time
    for (const [y, h] of [[107, 67], [194, 165]]) {
      kit.setPal(e);
      kit.outline(138, y, 368, h);
      kit.setPal(c);
      kit.outline(137, y - 1, 368, h);
      kit.outline(136, y - 2, 368, h);
    }
    for (const [label, y] of [["Side Name", 96], ["Leader", 181]]) {
      kit.setPal(3);
      gfx.rectangle("fill", 160, y, f.width(label), 17);
      gfx.setColor(1, 1, 1);
      f.colours(c, e).draw(label, 160, y);
    }
    kit.shield(i, 144, 122);
    kit.shield(i, 456, 122);
    kit.shield(i, 144, 202);
    gfx.setColor(1, 1, 1);
    kit.centred(f, "Retype the name of this side", 320, 116);
    kit.field(NAME_BOX.x, NAME_BOX.y, NAME_BOX.w, NAME_BOX.h, d.editing ? d.editing.text : s.name, f);
    if (d.editing) d.editing.drawCursor(NAME_BOX.x, NAME_BOX.y);
    kit.setPal(0);
    gfx.rectangle("fill", 196, 218, 72, 1);
    gfx.rectangle("fill", 384, 218, 96, 1);
    gfx.setColor(1, 1, 1);
    f.draw("Name", 196, 203);
    f.draw("Description", 384, 203);
    // the list (7bab:1be6): a sunk box, the chosen character in white
    kit.setPal(3);
    gfx.rectangle("fill", 189, 249, 186, 100);
    kit.bevel(188, 248, 188, 102, 4, 2);
    gfx.setColor(1, 1, 1);
    if (!s.computer) {
      f.draw("N/A", 196, 224);                    // 7bab:1d0f
      f.draw("N/A", 384, 224);
    } else {
      const ink = f.colours(LEVEL_INK[s.level] ?? 15, 0);
      for (let r = 0; r < 5 && rows[r] >= 0; r++) {
        (rows[r] === s.card ? f : ink).draw(deck[rows[r]] || "", 196, 252 + 19 * r);
      }
      // the name, and the .DSC's next six lines (7bab:20dc)
      ink.draw(deck[s.card] || "", 192, 224);
      if (descCard !== s.card) {
        descCard = s.card;
        desc = s.level < 3 ? aicard.describe(G.dataDir, s.level, s.card) : null;
      }
      const lines = desc ? desc[1] : [];
      for (let k = 0; k < 6; k++) if (lines[k]) ink.draw(lines[k], 384, 224 + 20 * k);
    }
    kit.drawControls(d.view);
  };

  d.mousepressed = (x, y) => {
    if (d.editing) keepName();
    const c = kit.controlAt(d.view, x, y);
    if (!c) return;
    const id = c.id;
    if (id === SIDE_OK) return close(true);                   // 7bab:2024
    if (id === SIDE_CANCEL) return close(false);
    if (id === SIDE_NAME) {
      d.editing = input.editor(15, 128);
    } else if (id === SIDE_UP || id === SIDE_DOWN) {
      // 7bab:1ed1: a row up or down, a row run off the deck left empty
      const step = id === SIDE_UP ? -1 : 1;
      for (let r = 0; r < 5; r++) {
        rows[r] += step;
        if (rows[r] < 0 || rows[r] >= deck.length) rows[r] = -1;
      }
    } else if (id === SIDE_PAGE_UP || id === SIDE_PAGE_DOWN) {
      // 7bab:1f23: five rows, or as many as there are
      const step = id === SIDE_PAGE_UP ? -Math.min(rows[0], 5)
        : Math.min(deck.length - rows[4] - 1, 5);
      rows[0] += step;
      for (let r = 1; r < 5; r++) rows[r] = rows[r - 1] + 1;
    } else if (id >= SIDE_ROW && id < SIDE_ROW + 5) {
      const card = rows[id - SIDE_ROW];                       // 7bab:1e8a
      if (card >= 0) s.card = card;
    }
    refresh();
  };

  d.keypressed = (key) => {
    if (d.editing) {
      const r = d.editing.key(key);
      if (r === "keep") keepName(); else if (r === "undo") d.editing = null;
      return;
    }
    if (key === "return" || key === "kpenter") close(true);
    else if (key === "escape") close(false);
  };

  d.textinput = (t) => { if (d.editing) d.editing.input(t); };
  refresh();
  return kit.push(d);
}

const RECTS = [];                                 // 4125:23c8
for (let i = 0; i < 8; i++) RECTS[i] = { x: i < 4 ? 24 : 208, y: 40 + 90 * (i % 4), w: 160, h: 70 };
// SETUPBU.PCK's faces (4125:2408): Knight, Lord, Warlord, Off, the two
// humans, a side not in the scenario, then a Knight, Lord and Warlord
// playing a character other than the Standard one
const FACES = [[0, 0], [0, 40], [0, 80], [0, 120], [0, 160], [0, 200], [440, 40],
  [424, 100], [424, 140], [424, 180]];
const LEVEL_BUTTON = [120, 200, 280, 360, 40];   // 4125:24b8: Knight .. Off, Human
// the Character box (4125:24ec): ticked with a character chosen, else empty,
// 24 x 20 at (x + 64, y + 39), its label at (x + 88, y + 41) (4125:24fc, 251c)
const CHECK = [[440, 0], [440, 20]];

/** A side's face (7bab:0634), an index into FACES. */
function faceOf(s, i) {
  if (!s.inUse) return 6;
  if (!s.computer) return i % 2 === 1 ? 5 : 4;
  if (s.card && s.level < 3) return s.level + 7;
  return s.level;
}

/** How many computers are in play: Random Characters needs one (7bab:0416). */
function computers(st) {
  let n = 0;
  for (let i = 0; i < 8; i++) {
    const s = st.sides[i];
    if (s && s.inUse && s.computer && s.level !== 3) n++;
  }
  return n;
}

function openSetup(G, st, begin, back) {
  // the menu bar stays live over it, as over the start menu (7bab:0034)
  const d = { view: kit.view(3), menuBar: true };
  d.view.screen = true;      // a screen, not a dialog: Begin has no ring
  const art = G.screen.art_for(31);              // SETUPBU.PCK
  const logo = G.screen.art_for(52);             // SSG.PCK
  // picked once as the screen is drawn whole; Edit Options draws over it
  const lines = quote(G.dataDir, (n) => 1 + Math.floor(Math.random() * n));

  const refresh = () => {
    const s = d.view.state;
    for (let i = 0; i < 8; i++) s[125 + i] = uidata.NORMAL;
    for (let p = 0; p <= 2; p++) s[145 + p] = presetMatches(st, p) ? uidata.ACTIVE : uidata.NORMAL;
    s[141] = uidata.NORMAL; s[142] = uidata.NORMAL; s[148] = uidata.NORMAL;
    s[143] = uidata.NORMAL; s[144] = uidata.NORMAL;
    s[157] = uidata.NORMAL;
    s[158] = computers(st) > 0 ? uidata.NORMAL : uidata.DISABLED;
    d.hidden = { [st.greatest ? 143 : 144]: true };
  };

  const blit = (sx, sy, w, h, x, y) => {
    if (!art) return;
    gfx.setColor(1, 1, 1);
    gfx.draw(art.image, gfx.newQuad(sx, sy, w, h), x, y);
  };

  d.draw = () => {
    screen.drawBackground(G.screen);
    gfx.setColor(1, 1, 1);
    gfx.draw(G.marble, gfx.newQuad(0, 0, 360, 360), 16, 30);
    gfx.draw(G.marble, gfx.newQuad(0, 0, 224, 312), 400, 30);
    gfx.draw(G.marble, gfx.newQuad(0, 60, 360, 66), 16, 403);
    gfx.draw(G.marble, gfx.newQuad(0, 0, 224, 114), 400, 355);
    const f = kit.font(2);
    for (let i = 0; i < 8; i++) {
      const s = st.sides[i];
      const R = RECTS[i];
      if (s && s.name !== "") {
        const c = s.colour ?? 15, e = s.edge ?? 0;
        kit.setPal(c);
        kit.outline(R.x, R.y, R.w, R.h);
        gfx.rectangle("fill", R.x - 1, R.y - 1, R.w + 2, 1);
        gfx.rectangle("fill", R.x - 1, R.y - 1, 1, R.h + 2);
        gfx.rectangle("fill", R.x + 1, R.y + R.h - 2, R.w - 2, 1);
        gfx.rectangle("fill", R.x + R.w - 2, R.y + 1, 1, R.h - 2);
        kit.setPal(e);
        gfx.rectangle("fill", R.x + 1, R.y + 1, R.w - 2, 1);
        gfx.rectangle("fill", R.x + 1, R.y + 1, 1, R.h - 2);
        gfx.rectangle("fill", R.x - 1, R.y + R.h, R.w + 2, 1);
        gfx.rectangle("fill", R.x + R.w, R.y - 1, 1, R.h + 2);
        const nx = R.x + 24, ny = R.y - 8;
        const w = f.width(s.name);
        kit.setPal(3);
        gfx.rectangle("fill", nx - 4, ny, w + 8, 20);
        gfx.setColor(1, 1, 1);
        f.colours(c, e).draw(s.name, nx, ny);
        // the face (7bab:0634): Off, a human (two faces, turn about), or the
        // computer's level, another face for a character; the button the
        // same, Off and Human mapped in
        const face = faceOf(s, i);
        const fr = FACES[face];
        blit(fr[0], fr[1], 40, 40, R.x + 16, R.y + 16);
        const lv = face >= 7 ? face - 7 : face;
        const btn = (lv === 6 || lv === 3) ? 3 : (lv >= 4 ? 4 : lv);
        blit(LEVEL_BUTTON[btn], 0, 80, 25, R.x + 72, R.y + 12);
        const ck = CHECK[s.card ? 0 : 1];
        blit(ck[0], ck[1], 24, 20, R.x + 64, R.y + 39);
        gfx.setColor(1, 1, 1);
        f.colours(c, e).draw("Character", R.x + 88, R.y + 41);   // 4125:258b
      }
    }
    // 7bab:10c7: the logo's (0, 0, 144, 64) to (440, 277) (4125:2550)
    gfx.setColor(1, 1, 1);
    if (logo) gfx.draw(logo.image, gfx.newQuad(0, 0, 144, 64), 440, 277);
    kit.centred(f, kit.text(7, 0), 512, 48);
    kit.centred(f, kit.text(7, 1), 512, 206);
    kit.centred(f, fmt(kit.text(6, 0), rating(st)), 196, 426);
    lines.forEach((line, i) => kit.centred(f, line, 512, QUOTE_Y[i]));
    kit.drawControls(d.view, d.hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (!c) return;
    const id = c.id;
    if (id >= 125 && id <= 132) {
      // 7bab:0a4e: Human, Knight, Lord, Warlord, Off, and round again
      const s = st.sides[id - 125];
      if (s && s.inUse) {
        if (!s.computer) { s.computer = true; s.level = 0; }
        else if (s.level === 3) { s.computer = false; s.level = 0; }
        else s.level++;
        s.card = 0;                    // the level's Standard character
      }
    } else if (id === 141) {
      let playing = 0;
      for (let i = 0; i < 8; i++) {
        const s = st.sides[i];
        if (s && s.inUse && !(s.computer && s.level === 3)) playing++;
      }
      if (playing < 1) return;
      keepOptions(G.dataDir);                             // 7bab:0cfe
      kit.pop(d);
      return begin(st);
    } else if (id === 142) {
      kit.pop(d);
      return back();
    } else if (id === 143) {
      st.greatest = true;
      for (let i = 0; i < 8; i++) {
        const s = st.sides[i];
        if (s && s.inUse) {
          if (!(s.computer && s.level === 2)) s.card = 0;
          s.computer = true; s.level = 2;
        }
      }
    } else if (id === 144) {
      st.greatest = false;
    } else if (id >= 145 && id <= 147) {
      for (let k = 0; k <= 5; k++) st.options[k] = PRESETS[id - 145][k];
    } else if (id === 148) {
      return openOptions(G, st, refresh);
    } else if ((id >= 133 && id <= 140) || (id >= 149 && id <= 156)) {
      // 7bab:16ea: the Character box or the face; a side not in the
      // scenario has neither
      const i = id >= 149 ? id - 149 : id - 133;
      const s = st.sides[i];
      if (s && s.inUse) return openSide(G, st, i, refresh);
    } else if (id === 157) {
      recallOptions(G.dataDir);                           // 7bab:2229
    } else if (id === 158) {
      // 7bab:2051: each computer in play gets 1d(n - 1) of its level's n
      // characters -- any but the Standard one
      for (let i = 0; i < 8; i++) {
        const s = st.sides[i];
        if (s && s.inUse && s.computer && s.level !== 3) {
          const n = aicard.deck(G.dataDir, s.level).length;
          s.card = n > 1 ? 1 + Math.floor(Math.random() * (n - 1)) : 0;
        }
      }
    }
    refresh();
  };
  d.keypressed = () => {};
  refresh();
  return kit.push(d);
}

function openChooser(G, list, current, after) {
  const R = { x: 80, y: 45, w: 480, h: 360 };    // popup 19
  const d = { view: kit.view(29), top: 0, cur: current };
  while (d.cur >= 7) { d.cur--; d.top++; }
  const refresh = () => {
    const s = d.view.state;
    for (const c of d.view.dialog.controls) s[c.id] = uidata.NORMAL;
    const up = d.top > 0 ? uidata.NORMAL : uidata.DISABLED;
    const down = d.top + 7 < list.length ? uidata.NORMAL : uidata.DISABLED;
    s[470] = up; s[472] = up; s[471] = down; s[473] = down;
  };
  d.draw = () => {
    kit.popupFrame(R);
    const pic = image(G, G.dataDir + "/PICS/NEWSCEN.PCK");
    gfx.setColor(1, 1, 1);
    if (pic) gfx.draw(pic, R.x, R.y);
    kit.setPal(0);
    gfx.rectangle("fill", 112, 153, 136, 140);
    gfx.setColor(1, 1, 1);
    const f = kit.font(2);
    for (let i = 0; i <= 6; i++) {
      const e = list[d.top + i];
      if (e) f.colours(i === d.cur ? 15 : 5, 0).draw(e.name, 112, 153 + 20 * i);
    }
    const e = list[d.top + d.cur];
    if (e) {
      const g5 = f.colours(5, 0);
      kit.centred(g5, e.name, 416, 101);
      kit.centred(g5, e.text, 416, 174);
      kit.right(g5, fmt("%d", e.cities), 368, 234);
      g5.draw(fmt("%d", e.ruins), 472, 234);
      kit.centred(g5, fmt("%d", e.players), 416, 254);
    }
    kit.drawControls(d.view, { 476: true, 477: true, 478: true, 479: true, 480: true, 481: true, 482: true });
  };
  const take = (ok) => {
    kit.pop(d);
    after(ok ? d.top + d.cur : null);
  };
  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c || d.view.state[c.id] === uidata.DISABLED) return;
    const id = c.id;
    if (id === 474) return take(true);
    if (id === 475) return take(false);
    if (id >= 476 && id <= 482) {
      if (list[d.top + id - 476]) d.cur = id - 476;
    } else if (id === 470 || id === 472) {
      d.top = Math.max(0, d.top - (id === 472 ? 7 : 1));
    } else if (id === 471 || id === 473) {
      d.top = Math.min(Math.max(0, list.length - 7), d.top + (id === 473 ? 7 : 1));
    }
    refresh();
  };
  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter") take(true);
    else if (key === "escape") take(false);
  };
  refresh();
  return kit.push(d);
}

const SLIDER_X = 384, SLIDER_W = 120;            // 4125:29ac

/** 7f77:0332: the random world's settings, in place of the picture. */
function drawWorld(G) {
  kit.setPal(3);
  gfx.rectangle("fill", 328, 200, 264, 225);     // 4125:2930
  const f = kit.font(2);
  const art = G.screen.art_for(1);               // STARTBU.PCK
  for (let i = 0; i < 4; i++) {
    const y = 215 + 30 * i;
    gfx.setColor(1, 1, 1);
    kit.right(f, kit.text(2, i), 376, y);
    // the slider at its place (4125:29cc), or bare for "?" (4125:2a04)
    const sy = world.set[i] ? 80 + 20 * world.sliders[i] : 220;
    if (art) gfx.draw(art.image, gfx.newQuad(496, sy, SLIDER_W, 20), SLIDER_X, y + 1);
    const shows = world.set[i]
      ? fmt(randommap.SLIDER_FORMATS[i], randommap.SLIDER_SHOWS[i][world.sliders[i]]) : "(?)";
    f.draw(shows, 504, y);
  }
  f.draw(randommap.terrainSetName(G.dataDir, world.terrainSet), 336, 340);
  f.draw(kit.text(3, world.allies ? 1 : 0), 336, 365);
}

/** random_map_setup (7bab:10e8): make the world, showing its progress as
 *  4bed:01ff does on popup 23 -- the scroll with group 136's lines, the bar
 *  RMAPBAR.PCK at (232, 257) growing a tenth at a time, the percentage over
 *  it -- then hand it on. A "?" slider is rolled, 1d7-1. */
function makeWorld(G, after) {
  const R = { x: 160, y: 55, w: 336, h: 347 };   // popup 23
  if (!G.bscroll) G.bscroll = pck.toImage(G.dataDir + "/PICS/BSCROLL.PCK", G.palette, 10)[0];
  const bar = image(G, G.dataDir + "/PICS/RMAPBAR.PCK", 10);
  const sliders = world.sliders.map((v, i) => (world.set[i] ? v : randommap.RANDOM_SLIDER));
  const it = randommap.generate({
    dataDir: G.dataDir, rng: new Rng(Date.now() % 1000000007), sliders,
    allies: world.allies, terrainSet: world.terrainSet,
  });
  const d = { pct: 0 };
  d.update = () => {
    // a step of the generator a frame, so the bar moves as it works
    const r = it.next();
    if (!r.done) { d.pct = r.value; return; }
    randommap.install(G.dataDir, r.value);
    kit.pop(d);
    after();
  };
  d.draw = () => {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.bscroll, gfx.newQuad(0, 0, R.w, R.h), R.x, R.y);
    const f = kit.font(2).colours(0, 7);
    [2, 3].forEach((k, i) => kit.centred(f, kit.text(0x88, k), 328, 181 + 20 * i));
    // 4bed:01b5: a black frame (216d:01fd), the scroll showing through it
    kit.setPal(0);
    gfx.rectangle("line", 232.5, 255.5, 191, 24);
    gfx.setColor(1, 1, 1);
    const w = Math.floor((d.pct + 10) / 10) * 16 + 16;
    if (bar) gfx.draw(bar, gfx.newQuad(0, 0, w, 21), 232, 257);
    kit.centred(f, fmt("%d%%", d.pct), 328, 237);
  };
  d.mousepressed = () => {};
  d.keypressed = () => {};
  return kit.push(d);
}

/** Open the start menu. `start(scenarioDir, options, sides, extra)` begins a
 *  game; `loaded(g)` takes a saved one. */
export function open(start, loaded) {
  const G = kit.G;
  const list = scenarios(G.dataDir);
  const d = { view: kit.view(1), cur: 0, menuBar: true };
  d.view.screen = true;      // a screen, not a dialog: Begin has no ring
  list.forEach((e, i) => { if (e.dir === "Erythea") d.cur = i; });   // 4125:2a6e
  // back from a random world, the menu is still on one (7f77:0000)
  if (G.scenario === randommap.DIR) world.on = true;

  // 7f77:011e
  const refresh = () => {
    const s = d.view.state;
    s[100] = uidata.NORMAL; s[101] = uidata.NORMAL; s[103] = uidata.NORMAL;
    s[102] = world.on ? uidata.DISABLED : uidata.NORMAL;
    d.hidden = {};
    if (!world.on) {
      for (let id = 104; id <= 113; id++) d.hidden[id] = true;
      return;
    }
    s[104] = uidata.NORMAL; s[105] = uidata.NORMAL;
    for (let i = 0; i < 4; i++) {
      s[106 + i] = uidata.NORMAL;
      s[110 + i] = world.set[i] ? uidata.NORMAL : uidata.ACTIVE;   // lit while "?"
    }
  };

  d.draw = () => {
    gfx.setColor(1, 1, 1);
    for (let q = 0; q < 4; q++) {
      const img = image(G, G.dataDir + "/PICS/STARTUP" + q + ".PCK");
      if (img) gfx.draw(img, (q % 2) * 320, Math.floor(q / 2) * 240);
    }
    const sc = list[d.cur];
    if (world.on) drawWorld(G);
    else if (sc) {
      const pic = image(G, G.dataDir + "/" + sc.dir.toUpperCase() + "/PICS/SCENARIO.PCK");
      if (pic) {
        gfx.setColor(1, 1, 1);
        gfx.draw(pic, gfx.newQuad(0, 0, 264, 225), 328, 200);
      }
    }
    if (world.on || sc) {
      // 7f77:02bf: the scenario's name, or "A Random World"
      kit.setPal(3);
      gfx.rectangle("fill", 336, 166, 248, 28);
      gfx.setColor(1, 1, 1);
      kit.centred(kit.font(2), world.on ? kit.text(0, 0) : sc.name, 460, 172);
    }
    kit.drawControls(d.view, d.hidden);
  };

  const reopen = () => { refresh(); kit.push(d); };

  const setUp = (sc) => {
    const st = newSetup(G, sc);
    openSetup(G, st, (s) => {
      const options = {}, sides = {};
      for (let i = 0; i <= 9; i++) options[OPTION_KEYS[i]] = s.options[i];
      for (let i = 0; i < 8; i++) {
        const e = s.sides[i];
        if (e) {
          sides[i] = { computer: e.computer, level: e.level, card: e.card, name: e.name,
                       off: e.computer && e.level === 3 };
        }
      }
      start(s.sc.dir.toUpperCase(), options, sides, { greatest: s.greatest });
    }, reopen);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (!c || d.view.state[c.id] === uidata.DISABLED) return;
    const id = c.id;
    if (id === 100) {
      // 7f77:067c: a scenario chosen ends the random world
      openChooser(G, list, d.cur, (i) => {
        if (i != null) { d.cur = i; world.on = false; }
        refresh();
      });
    } else if (id === 101) {
      savegame.load((g) => {
        kit.pop(d);
        loaded(g);
      });
    } else if (id === 102) {
      world.on = true;                                    // 7f77:05f5
    } else if (id === 104) {
      world.terrainSet = (world.terrainSet + 1) % randommap.terrainSets(G.dataDir);   // 7f77:063a
    } else if (id === 105) {
      world.allies = !world.allies;                       // 7f77:0661
    } else if (id >= 106 && id <= 109) {
      // 7f77:0512: a "?" slider is set again where it was; a set one moves
      // to where it was clicked
      const i = id - 106;
      if (!world.set[i]) world.set[i] = 1;
      else world.sliders[i] = Math.min(6, Math.trunc((x - SLIDER_X) * 7 / SLIDER_W));
    } else if (id >= 110 && id <= 113) {
      world.set[id - 110] = 0;                            // 7f77:0571
    } else if (id === 103 && world.on) {
      // 7f77:060f: "One moment...", the world, then the sides as for any scenario
      advisorUi.say(cues.MOMENT, () => makeWorld(G, () => {
        kit.pop(d);
        setUp({ name: kit.text(0, 0), dir: randommap.DIR });
      }));
    } else if (id === 103 && list[d.cur]) {
      kit.pop(d);
      setUp(list[d.cur]);
    }
    refresh();
  };
  d.keypressed = () => {};
  refresh();
  return kit.push(d);
}
