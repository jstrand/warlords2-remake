// Warlords II, in the original's own interface, in a browser.
//
// A port of the Lua remake's front end (love2d/main.lua). The screen is
// 640x480 and every rect in it comes from the game's own layout files:
// DATA/JOIN.DAT names the dialog's controls and clickable regions, AREA.DAT
// places the regions, BUTTON.DAT the controls, and FILE.DAT says which .PCK
// each control is cut from. docs/re/ui.md and docs/formats/screens.md.
//
// Open index.html from a web server; ?scenario=ISLADIA&seed=123 goes straight
// into a game.

import * as vfs from "./vfs.js";
import * as gfx from "./gfx.js";
import * as keys from "./keys.js";
import * as prefs from "./prefs.js";
import * as sound from "./sound.js";
import { display, fitCanvas, measure, uiSize, setFrame, pushUI, pushMap, toUI, onFrame } from "./display.js";
import * as displayMod from "./display.js";
import * as pal from "./warlords/pal.js";
import * as pck from "./warlords/pck.js";
import * as scn from "./warlords/scn.js";
import * as game from "./warlords/game.js";
import * as move from "./warlords/move.js";
import * as ai from "./warlords/ai.js";
import * as rules from "./warlords/rules.js";
import * as diplomacy from "./warlords/diplomacy.js";
import * as hero from "./warlords/hero.js";
import * as armytype from "./warlords/armytype.js";
import * as saveMod from "./warlords/save.js";
import * as screen from "./warlords/screen.js";
import * as layoutMod from "./warlords/layout.js";
import * as uidata from "./warlords/uidata.js";
import * as font from "./warlords/font.js";
import * as menuMod from "./warlords/menu.js";
import * as slotsMod from "./warlords/slots.js";
import * as cues from "./warlords/cues.js";
import * as siteMod from "./warlords/site.js";
import * as kit from "./ui/kit.js";
import * as cityUi from "./ui/city.js";
import * as reportsUi from "./ui/reports.js";
import * as heroInfo from "./ui/heroinfo.js";
import * as levelsUi from "./ui/levels.js";
import * as searchUi from "./ui/search.js";
import * as questUi from "./ui/quest.js";
import * as advisorUi from "./ui/advisor.js";
import * as tutorial from "./ui/tutorial.js";
import * as startUi from "./ui/start.js";
import * as endingUi from "./ui/ending.js";
import * as questnews from "./ui/questnews.js";
import * as infobox from "./ui/infobox.js";
import * as tileinfo from "./ui/tileinfo.js";
import * as miladvisor from "./ui/miladvisor.js";
import * as helpUi from "./ui/help.js";
import * as inputUi from "./ui/input.js";
import * as savegame from "./ui/savegame.js";
import * as diplomacyUi from "./ui/diplomacy.js";
import * as spoils from "./ui/spoils.js";
import * as settingsUi from "./ui/settings.js";
import * as aboutUi from "./ui/about.js";
import * as fightorder from "./ui/fightorder.js";
import * as resignUi from "./ui/resign.js";
import * as ruinUi from "./ui/ruin.js";
import * as stackUi from "./ui/stack.js";
import * as armybonus from "./ui/armybonus.js";
import * as itemsUi from "./ui/items.js";
import * as signpost from "./ui/signpost.js";
import * as historyUi from "./ui/history.js";
import * as shortcutsUi from "./ui/shortcuts.js";
import { fmt, now } from "./util.js";

const TILE = screen.TILE;
// An army sheet is 16 cells across on a 32-pixel stride; its rows are 30
// apart and 29 tall, which is what 8611:08be reads out of it.
const ARMY_CELL = 32, ARMY_COLS = 16, ARMY_ROW = 30, ARMY_H = 29;
const ROAD_STRIDE = 48, ROAD_COLS = 13;
// The army sheets stand on 10, bright green -- all but the fifth side's,
// keyed on its own ground, the colour of its corner.
const ROAD_KEY = 1, ARMY_KEY = "corner", SHADOW_KEY = 15;
const SCROLL_KEY = 10;
const WAR_KEY = 1, SHIELD_KEY = 12;
const RING_W = 32, RING_H = 30;      // one ABITS ring
const CURS_RATE = 18.2 / 4;          // CURS.PCK frames a second (177b:011f)
const DATA = "";                     // the game's files, as the vfs holds them

/** The front end's state, shared with the dialogs through kit.G. */
const G = { modals: [], mouse: { x: -1, y: -1 } };
window.W2 = G;                       // for looking at from the console

function quadsFor(cols, cellW, cellH, stride, count, rowStride) {
  const qs = [];
  for (let i = 0; i < count; i++) {
    qs[i] = gfx.newQuad((i % cols) * (stride || cellW), Math.floor(i / cols) * (rowStride || cellH), cellW, cellH);
  }
  return qs;
}

// Where the game's running commentary collects. Nothing draws it: the
// original says these things through popups and the advisor instead.
function say(f, ...args) {
  G.status = args.length > 0 ? fmt(f, ...args) : f;
}

// ------------------------------------------------------------------ loading

function load(params) {
  const palette = pal.load(DATA + "/TERRAIN0/WAR2.PAL");
  G.palette = palette;
  G.dataDir = DATA;

  const img = (path, key) => pck.toImage(DATA + path, palette, key)[0];

  // terrain, roads and armies
  G.sheets = [];
  G.tileQuads = [];
  for (let i = 0; i < 2; i++) {
    G.sheets[i] = img("/TERRAIN0/SCENERY" + i + ".PCK");
    G.tileQuads[i] = quadsFor(16, TILE, TILE, TILE, 96);
  }
  G.roadImg = img("/TERRAIN0/ROAD.PCK", ROAD_KEY);
  G.roadQuads = quadsFor(ROAD_COLS, TILE, TILE, ROAD_STRIDE, 26);
  // A0-A8 are the sides' armies and ASHADOW the same sheet drawn as a ghost
  G.armyImg = [];
  G.armyQuads = [];
  for (let i = 0; i <= 8; i++) {
    G.armyImg[i] = img("/TERRAIN0/A" + i + ".PCK", ARMY_KEY);
    G.armyQuads[i] = quadsFor(ARMY_COLS, ARMY_CELL, ARMY_H, ARMY_CELL, 32, ARMY_ROW);
  }
  // the shadow sheet keys on white: it is two colours of ghost on white
  G.shadowImg = img("/TERRAIN0/ASHADOW.PCK", SHADOW_KEY);

  G.marble = img("/PICS/MARBLE.PCK");
  G.bigArmy = img("/PICS/BIGARMY.PCK");
  G.cityPic = img("/PICS/CITY.PCK");
  G.warPic = img("/PICS/WAR.PCK", WAR_KEY);
  G.shieldImg = img("/TERRAIN0/BSHIELD.PCK", SHIELD_KEY);
  G.victoryPic = img("/PICS/VICTORY.PCK");
  G.heroPic = { m: img("/PICS/MHERO.PCK"), f: img("/PICS/FHERO.PCK") };
  // ATRANS2.PCK: map markers on a mask colour, 1 (1997:027e, 8cc6:0952)
  G.atransShields = img("/TERRAIN0/ATRANS2.PCK", 1);
  G.heroMark = gfx.newQuad(96, 0, 16, 15);     // 4125:2cb6
  G.bagQuad = gfx.newQuad(64, 0, 32, 29);
  G.blastQuad = gfx.newQuad(32, 0, 32, 29);    // 6a35:0000
  // HIDDEN.PCK: the hidden map's edges, fourteen 40 x 40 cells 48 apart in
  // two rows 41 apart; colour 1 is where the map shows through
  G.fogImg = img("/PICS/HIDDEN.PCK", 1);
  G.fogQuads = [];
  for (let c = 0; c <= 13; c++) G.fogQuads[c] = gfx.newQuad((c % 7) * 48, Math.floor(c / 7) * 41, 40, 40);
  // CURS.PCK: the marching box round the selected stack (177b:01a1); keyed
  // on 0, its 1 shows white and its 4 black
  {
    const [w, h, px] = pck.decode(DATA + "/PICS/CURS.PCK");
    G.cursImg = pck.imageFromPixels(w, h, px, palette, { 1: 15, 4: 0 }, { 0: true });
  }
  G.cursQuads = [];
  for (let k = 0; k <= 7; k++) G.cursQuads[k] = gfx.newQuad((k % 4) * 64, Math.floor(k / 4) * 40, 40, 40);
  // the rings, drawn opaque as 1997:0129 blits every bitmap it is given
  G.abits = img("/PICS/ABITS.PCK");
  G.ringQuads = [];
  for (let i = 0; i <= 8; i++) G.ringQuads[i] = gfx.newQuad(i * RING_W, 0, RING_W, RING_H);

  // STAND.PCK: the twelve mouse pointers, drawn by the game itself (22bf)
  G.pointerImg = img("/STAND.PCK", 10);
  G.pointerQuads = [];
  for (let i = 0; i <= 11; i++) G.pointerQuads[i] = gfx.newQuad(i * 16, 0, 16, 16);

  // the original's own chrome and fonts
  G.screen = screen.load(DATA, palette, uidata.MAIN_SCREEN, 0);
  G.font = font.load(DATA, "TEXT", palette, 3);
  G.bigFont = font.load(DATA, "CHANCE17", palette, 3);
  G.titleFont = font.load(DATA, "CHANCE36", palette, 3);

  G.openMenu = null;
  G.shieldsImg = img("/TERRAIN0/SHIELDS.PCK");
  G.cityBack = img("/TERRAIN0/CITYBACK.PCK");
  G.searchPic = img("/PICS/SEARCH.PCK");
  G.templePic = img("/PICS/TEMPLE.PCK");
  G.scrollPic = img("/PICS/SCROLL.PCK", SCROLL_KEY);
  kit.init(G);

  // the screen: its real pixels, the UI's scale, and the map's zoom
  if (prefs.get("4:3") === "on") display.wanted = { w: layoutMod.W, h: layoutMod.H };
  display.chosen = Number(prefs.get("scale")) || null;
  measure();
  G.zoom = Number(prefs.get("zoom")) || display.scale;
  syncLayout();

  const scenario = params.get("scenario");
  G.starting = !scenario;
  G.scenario = scenario || "ERYTHEA";
  if (G.starting) {
    openStart();
    // 7f77:0000 greets the player the first time round
    advisorUi.say(cues.GREET);
    return;
  }
  if (!vfs.exists(`${DATA}/${G.scenario}/${G.scenario}.SCN`)) throw new Error("no scenario " + G.scenario);
  newGame(Number(params.get("seed")) || Date.now() % 1000000007);
  beginGame();
  say("Click a stack, then click where to go.");
}

/** The start screens (7f77:0000): a new game set up there, or a saved one. */
function openStart() {
  G.starting = true;
  sound.music(cues.TITLE);
  startUi.open((dir, options, sides, extra) => {
    // 7bab:0cfe: the war begins to its own music, and the advisor says so
    sound.music(cues.BEGIN);
    advisorUi.say(cues.BEGIN_WAR, () => {
      G.starting = false;
      G.scenario = dir;
      G.seed = Date.now() % 1000000007;
      G.g = game.newGame(G.dataDir, dir, { seed: G.seed, options, sides, greatest: extra && extra.greatest });
      G.selection = null; G.over = null; G.stratImage = null;
      beginGame();
      if (G.player.computer) playComputers(G.player);
    });
  }, (g) => {
    G.starting = false;
    takeLoaded(g);
  });
}
G.openStart = openStart;
G.quit = () => {
  for (let i = G.modals.length - 1; i >= 0; i--) G.modals.pop();
  G.aiRun = null; G.aiStatus = null; G.assault = null; G.walk = null; G.moveAll = null;
  G.banner = null; G.offer = null; G.victory = null; G.victoryUnder = null; G.over = null;
  openStart();
};

/** A fresh game of the scenario: the first side the player's, the rest the
 *  computer's, until the new-game screens or Settings say otherwise. */
function newGame(seed) {
  G.seed = seed;
  G.g = game.newGame(G.dataDir, G.scenario, { seed });
  G.g.sides.forEach((s, i) => { s.computer = i > 0; });
  G.selection = null; G.over = null;
  G.stratImage = null;
}

/** Start its first turn, and centre the view on the capital. */
function beginGame() {
  G.player = game.begin(G.g);
  G.cursor = G.player.capital ? { x: G.player.capital.x, y: G.player.capital.y } : { x: 0, y: 0 };
  G.selection = null;
  centreOn(G.player.capital.x, G.player.capital.y);
  // the banner is a human's (8cc6:0259)
  if (!G.player.computer) showBanner(G.player);
}

// ------------------------------------------------------------------ camera

// The view is as many tiles as the map's rect holds at the map's zoom, and
// slides smoothly: G.cx, G.cy is the tile at its top-left corner.

function viewSize() {
  const k = display.scale / (TILE * G.zoom);
  return [G.mapRect.w * k, G.mapRect.h * k];
}
G.viewSize = viewSize;

/** The camera in device pixels, rounded to one. */
function camDev() {
  const z = TILE * G.zoom;
  return [Math.floor(G.cx * z + 0.5), Math.floor(G.cy * z + 0.5)];
}

/** A UI point as a map position, in tiles with their fractions. */
G.uiToMap = (x, y) => {
  const r = G.mapRect, s = display.scale, z = TILE * G.zoom;
  const [cx, cy] = camDev();
  return [(cx + (x - r.x) * s) / z, (cy + (y - r.y) * s) / z];
};

/** A map position, in tiles, as a UI point. */
G.mapToUI = (tx, ty) => {
  const r = G.mapRect, s = display.scale, z = TILE * G.zoom;
  const [cx, cy] = camDev();
  return [r.x + (tx * z - cx) / s, r.y + (ty * z - cy) / s];
};

/** The tile at the view's centre -- the original's "cursor". */
G.viewCentre = () => {
  const [vw, vh] = viewSize();
  return [Math.floor(G.cx + vw / 2), Math.floor(G.cy + vh / 2)];
};

function viewTiles() {
  const [vw, vh] = viewSize();
  const mw = G.g.map.width, mh = G.g.map.height;
  return [Math.max(0, Math.floor(G.cx)), Math.max(0, Math.floor(G.cy)),
          Math.min(mw - 1, Math.ceil(G.cx + vw) - 1), Math.min(mh - 1, Math.ceil(G.cy + vh) - 1)];
}
G.viewTiles = viewTiles;

/** Is (x, y) among the tiles being drawn this frame? */
G.shown = (x, y) => {
  const v = G.view;
  return v && x >= v[0] && y >= v[1] && x <= v[2] && y <= v[3];
};

/** Is (x, y) on screen? Its middle has to be. */
function inView(x, y) {
  const [vw, vh] = viewSize();
  return x + 0.5 > G.cx && x + 0.5 < G.cx + vw && y + 0.5 > G.cy && y + 0.5 < G.cy + vh;
}

/** Keep the view on the map; a map smaller than the view sits in its middle. */
function clampCamera() {
  const [vw, vh] = viewSize();
  const mw = G.g.map.width, mh = G.g.map.height;
  G.cx = vw >= mw ? (mw - vw) / 2 : Math.max(0, Math.min(G.cx, mw - vw));
  G.cy = vh >= mh ? (mh - vh) / 2 : Math.max(0, Math.min(G.cy, mh - vh));
}

function centreOn(x, y) {
  const [vw, vh] = viewSize();
  G.cx = x + 0.5 - vw / 2;
  G.cy = y + 0.5 - vh / 2;
  clampCamera();
}
G.centreOn = (x, y) => centreOn(x, y);

/** The zoom the map may go to: up to two steps past the UI's biggest. */
const maxZoom = () => display.maxScale + 2;

/** Zoom the map to `z`, keeping the map under the UI point (x, y) put. */
G.setZoom = (z, x, y) => {
  z = Math.max(1, Math.min(maxZoom(), z));
  if (z === G.zoom) return;
  prefs.set("zoom", z);
  const r = G.mapRect;
  if (x == null) { x = r.x + r.w / 2; y = r.y + r.h / 2; }
  const [tx, ty] = G.uiToMap(x, y);
  G.zoom = z;
  const k = display.scale / (TILE * z);
  G.cx = tx - (x - r.x) * k;
  G.cy = ty - (y - r.y) * k;
  clampCamera();
};

/** Lay the main screen out for layout `L`, keeping the view centred. */
function applyLayout(L) {
  let mid = null;
  if (G.g && G.cx != null && G.mapRect) {
    const [vw, vh] = viewSize();
    mid = [G.cx + vw / 2, G.cy + vh / 2];
  }
  G.layout = L;
  screen.relayout(G.screen, L);
  G.mapRect = screen.region(G.screen, screen.REGION.MAP);
  G.stratRect = screen.region(G.screen, screen.REGION.STRATEGIC);
  G.barRect = screen.region(G.screen, screen.REGION.BOTTOMBAR);
  G.menuRect = screen.region(G.screen, screen.REGION.MENUBAR);
  G.panelRect = screen.region(G.screen, screen.REGION.MAPPANEL);
  G.menuLayout = menuMod.layout(G.font, menuMod.BAR_H, L.w, menuMod.withZooms(display.maxScale, maxZoom()));
  G.zoom = Math.max(1, Math.min(maxZoom(), G.zoom || display.scale));
  if (mid) {
    const [vw, vh] = viewSize();
    G.cx = mid[0] - vw / 2;
    G.cy = mid[1] - vh / 2;
    clampCamera();
  }
}

/** The start screens are the original's 640x480, centred; the game has the
 *  whole screen. Run each frame, and each input. */
function syncLayout() {
  let w = layoutMod.W, h = layoutMod.H;
  if (!G.starting) [w, h] = uiSize();
  setFrame(Math.max(w, layoutMod.W), Math.max(h, layoutMod.H));
  if (!G.layout || G.layout.w !== display.w || G.layout.h !== display.h) {
    applyLayout(layoutMod.compute(display.w, display.h));
  }
}
G.syncLayout = () => syncLayout();

/** Change how the screen is used, keeping the view centred where it was. */
function relayout(change) {
  let mid = null;
  if (G.g && G.cx != null && !G.starting) {
    const [vw, vh] = viewSize();
    mid = [G.cx + vw / 2, G.cy + vh / 2];
  }
  change();
  G.layout = null;
  syncLayout();
  if (mid) {
    const [vw, vh] = viewSize();
    G.cx = mid[0] - vw / 2;
    G.cy = mid[1] - vh / 2;
    clampCamera();
  }
}

/** Draw the interface at `n` device pixels a pixel (View > Interface). */
G.setUIScale = (n) => {
  relayout(() => displayMod.choose(n));
  prefs.set("scale", display.chosen);
};

/** The browser's full screen or a window (View > Full screen, Window). */
G.setScreen = (how) => {
  displayMod.setWindowed(how === "window");
};
G.screenMode = () => (document.fullscreenElement ? "full" : "window");

/** All of the screen, or only the original's 640x480 (View > 4:3). */
G.setOriginalSize = (on) => {
  relayout(() => { display.wanted = on ? { w: layoutMod.W, h: layoutMod.H } : null; });
  prefs.set("4:3", on ? "on" : "off");
};

/** Is this menu item the setting in use? */
G.menuTicked = (k) => k === "map zoom " + G.zoom || k === "ui scale " + display.scale
  || k === "screen " + G.screenMode()
  || (k === "screen 4:3" && display.wanted != null)
  || k === "music " + sound.synth();

// ------------------------------------------------------------------ walking

// A stack walks one tile at a time, and the map shows where it is going. The
// destination is kept in the army record itself, so it outlives a walk that
// could not finish. 8611:2ef5 marks the route: a ring on every tile but the
// last, plain while the stack can still reach it this turn and crossed once
// it cannot, and a ghost of the leading army on the last (docs/re/ui.md).

const WALK = {
  ring: [496, 31],                       // ASHADOW.PCK, 16 x 14
  crossed: [496, 48],
  ringW: 16, ringH: 14,
  ringAt: [16, 13],                      // within the tile
  ghostAt: [4, 4],
  stepTime: 0.1,                         // one tile of the walk
};

/** Work out the route the selection is showing, if it has anywhere to be. */
function refreshRoute() {
  G.route = null;
  const sel = G.selection;
  if (!sel || sel.stack.length === 0) return;
  const target = sel.stack[0].target;
  if (!target) return;
  if (sel.stack[0].x === target.x && sel.stack[0].y === target.y) {
    for (const a of sel.stack) a.target = undefined;
    return;
  }
  G.route = move.preview(G.g, sel.stack, target.x, target.y);
}

/** Give the selection somewhere to be, or take it away again. */
function orderTo(stack, x, y) {
  for (const a of stack) a.target = x != null ? { x, y } : undefined;
}

/** Alt and a click (1c8c:0007): the route is drawn but not walked. */
function planRoute(x, y) {
  const sel = G.selection;
  if (!sel || sel.stack.length === 0) return;
  if (!game.seen(G.g, G.player, x, y)) return;
  if (sel.stack[0].x === x && sel.stack[0].y === y) {
    orderTo(sel.stack, null);
    G.route = null;
    return;
  }
  orderTo(sel.stack, x, y);
  refreshRoute();
  if (!G.route) sound.effect("chord");
}

/** Play the walk back a tile at a time, centring on the stack as it goes. */
function startWalk(armies, tiles) {
  if (!tiles || tiles.length === 0) return;
  G.walk = { armies: new Set(armies), tiles, i: 0, at: now(), top: armies[0] };
  centreOn(tiles[0].x, tiles[0].y);
}

function advanceWalk() {
  const w = G.walk;
  if (!w) return;
  const t = now();
  while (w.i < w.tiles.length - 1 && t - w.at >= WALK.stepTime) {
    w.i++;
    w.at += WALK.stepTime;
    centreOn(w.tiles[w.i].x, w.tiles[w.i].y);
  }
  if (w.i >= w.tiles.length - 1 && t - w.at >= WALK.stepTime) {
    G.walk = null;
    refreshRoute();
    if (w.computer && G.resumeComputer) G.resumeComputer();
    if (G.moveAll && G.moveAllStep) G.moveAllStep();
    if (!G.walk && !G.moveAll) G.flushKeys();
  }
}

/** Where the walking stack is drawn while the walk plays out. */
function walkingAt() {
  const w = G.walk;
  return w ? w.tiles[w.i] : null;
}

/** The route, as rings on the map (8611:2ef5). */
function drawRoute() {
  const route = G.route;
  if (!route) return;
  const ghost = G.selection && G.selection.stack[0];
  // while the walk plays out the route is drawn from where it has got to
  const first = G.walk ? G.walk.i + 1 : 1;
  for (let i = first; i <= route.path.length; i++) {
    const step = route.path[i - 1];
    if (G.shown(step.x, step.y)) {
      const sx = step.x * TILE, sy = step.y * TILE;
      gfx.setColor(1, 1, 1);
      if (i === route.path.length) {
        if (ghost) gfx.draw(G.shadowImg, G.armyQuads[8][ghost.type % 32], sx + WALK.ghostAt[0], sy + WALK.ghostAt[1]);
      } else {
        const src = i <= route.reach ? WALK.ring : WALK.crossed;
        gfx.draw(G.shadowImg, gfx.newQuad(src[0], src[1], WALK.ringW, WALK.ringH),
                 sx + WALK.ringAt[0], sy + WALK.ringAt[1]);
      }
    }
  }
}

/** Screen point to map tile, or null outside the viewport. */
function tileAtPoint(sx, sy) {
  const r = G.mapRect;
  if (sx < r.x || sy < r.y || sx >= r.x + r.w || sy >= r.y + r.h) return null;
  const [tx, ty] = G.uiToMap(sx, sy);
  return [Math.floor(tx), Math.floor(ty)];
}

// ------------------------------------------------------------------ selection

function selectableAt(x, y) {
  const out = [];
  for (const a of game.armiesAt(G.g, x, y)) {
    if (a.owner === G.player.index && out.length < rules.MAX_STACK) out.push(a);
  }
  return out;
}

/** Pick up whatever the slots now say moves, and remember the grouping. */
function syncSelection() {
  const sel = G.selection;
  if (!sel) return;
  slotsMod.commit(sel.slots, G.g);
  sel.stack = slotsMod.selected(sel.slots);
  // being in the group that moves puts an army back in the cycle and marks
  // it offered for this pass (89e0:000a, 8c07:06eb)
  for (const a of sel.stack) { a.fortified = undefined; a.offered = true; }
}

/** Put the slots back over a new list of armies, keeping the group. */
function reslot(armies) {
  const sel = G.selection;
  if (!sel) return;
  const s = armies && armies.length > 0 ? slotsMod.keep(sel.slots, G.g, armies) : null;
  if (!s) { G.selection = null; return; }
  sel.slots = s;
  sel.x = s.army[0].x; sel.y = s.army[0].y;
  syncSelection();
}

/** Pick up what is on a tile; `pick` names the army to take. */
function select(x, y, pick) {
  const stack = selectableAt(x, y);
  if (stack.length === 0) {
    G.selection = null;
    const city = game.cityAt(G.g, x, y);
    if (city) openCity(city);
    return;
  }
  // a click selects one army, not the stack (docs/re/ui.md > The army slots)
  const s = slotsMod.build(G.g, stack, G.player.index, slotsMod.clicked(G.g, stack, G.player.index, pick));
  G.selection = { x, y, slots: s };
  syncSelection();
  refreshRoute();
  say("%d of %d, %d movement", G.selection.stack.length, s.n, slotsMod.moves(s));
  // the tutorial on picking a stack up (1b62:051d-062a)
  const moments = ["move"];
  for (let dx = -1; dx <= 1; dx++) {
    for (let dy = -1; dy <= 1; dy++) {
      const c = game.cityAt(G.g, x + dx, y + dy);
      if (c && c.ownerIndex !== G.player.index && !c.razed) moments[1] = "fight";
    }
  }
  const lead = G.selection.stack[0];
  if (lead && lead.type === armytype.HERO && G.g.map.siteAt && G.g.map.siteAt[y * G.g.map.width + x]) {
    moments.push("search");
  }
  tutorial.chain(moments.filter((m) => m));
}
G.selectAt = (x, y) => select(x, y);

// ------------------------------------------------------------- the army cycle

// The five buttons above the pad walk you through your armies one stack at
// a time (8c07:040e). The next army is the nearest of the eligible ones to
// where the cycle last stopped; one exactly there counts as far away.

function cycleReset() {
  G.cursor = G.player.capital ? { x: G.player.capital.x, y: G.player.capital.y } : { x: 0, y: 0 };
}

function cycleRank(a) {
  const d = Math.abs(a.x - G.cursor.x) + Math.abs(a.y - G.cursor.y);
  return d === 0 ? 9000 : d;
}

/** The stack the cycle offers next, or null when every army is done. */
function nextArmy() {
  if (!G.cursor) cycleReset();
  let fresh = null, used = null;
  for (const a of G.g.armies) {
    if (a.owner === G.player.index && !a.transit && !a.fortified && !a.done) {
      if (a.offered) {
        if (!used || cycleRank(a) < cycleRank(used)) used = a;
      } else if (!fresh || cycleRank(a) < cycleRank(fresh)) fresh = a;
    }
  }
  if (!fresh) {
    if (!used) return null;
    for (const a of G.g.armies) if (a.owner === G.player.index) a.offered = undefined;
    fresh = used;
  }
  G.cursor = { x: fresh.x, y: fresh.y };
  return fresh;
}

function selectNext() {
  const a = nextArmy();
  if (!a) {
    G.selection = null; G.route = null;
    say("Every army has moved.");
    sound.effect("chord");                     // 8c07:019b: nothing left
    return;
  }
  sound.effect("ding");
  select(a.x, a.y, a);
  centreOn(a.x, a.y);
}

/** Out of the cycle for the rest of this turn (control 175, 8c07:0393). */
function quitArmy() {
  if (!G.selection) { selectNext(); return; }
  for (const a of G.selection.stack) a.done = true;
  G.route = null;
  selectNext();
}

/** Dig in: out of the cycle until picked up again (control 176, 8c07:03a6). */
function fortify() {
  if (!G.selection) { selectNext(); return; }
  const n = G.selection.stack.length;
  for (const a of G.selection.stack) { a.fortified = true; a.target = undefined; }
  say("%d armies dig in.", n);
  G.route = null;
  selectNext();
}

/** Put the selection down (control 178, 8065:0f26). */
function deselect() {
  G.selection = null; G.route = null; G.walk = null;
}

// ------------------------------------------------------------------ the turn

function afterBattle(result) {
  tutorial.show("fresult");
  if (result.captured) {
    say("%s is ours!%s", result.captured.name, result.loot && result.loot > 0 ? fmt(" Looted %d gold.", result.loot) : "");
  } else if (result.won) {
    say("Cleared: %d lost, %d killed.", result.deadAttackers.length, result.deadDefenders.length);
  } else {
    say("Beaten back: %d lost, %d killed.", result.deadAttackers.length, result.deadDefenders.length);
  }
  if (G.selection) {
    const alive = [];
    for (let i = 0; i < G.selection.slots.n; i++) {
      const a = G.selection.slots.army[i];
      if (!result.deadByArmy.has(a)) alive.push(a);
    }
    reslot(alive);
  }
}

/** Walk the selection towards (x, y). Only `attack` turns running into an
 *  enemy into an assault (740d:0179, 1c8c:041f); a walk just stops. */
function moveSelection(x, y, attack) {
  const sel = G.selection;
  if (!sel) return;
  orderTo(sel.stack, x, y);
  G.route = move.preview(G.g, sel.stack, x, y);
  const r = move.moveTo(G.g, sel.stack, x, y);
  if (r.stopped === "attack" && !(attack && r.steps === 0)) r.stopped = "blocked";
  if (r.stopped === "attack") {
    // decided first, shown, and only then taken effect (67cc:0a6b)
    const result = game.decideAttack(G.g, sel.stack, r.attack.x, r.attack.y);
    startAssault(r.attack.x, r.attack.y, result);
    G.assault.after = () => afterBattle(result);
  } else if (r.stopped === "no route") {
    say("There is no way there.");
    sound.effect("chord");                     // 1a8b:0c4f, 1c8c:0007
  } else if (r.steps === 0) {
    say("They cannot move: %s.", r.stopped);
  } else {
    const walked = sel.stack;
    sel.x = sel.stack[0].x; sel.y = sel.stack[0].y;
    reslot(selectableAt(sel.x, sel.y));
    stratDirty();
    say("Moved %d for %d. %d left.", r.steps, r.spent, move.stackMoves(sel.stack));
    startWalk(walked, r.walked || []);
  }
  if (G.selection && G.selection.stack.length > 0) {
    G.selection.x = G.selection.stack[0].x; G.selection.y = G.selection.stack[0].y;
    if (!G.walk) centreOn(G.selection.x, G.selection.y);
  }
  if (!G.walk) refreshRoute();
}

// ------------------------------------------------------------------ the pointer

// 18a9:0896 decides the mouse pointer from what is under it and what is
// selected, and a click on the map does whatever that pointer promises
// (740d:0037). STAND.PCK holds the twelve, with hotspots at 4125:045c.
const PTR = { ARROW: 0, VIEW: 1, BOAT: 2, CITY: 3, HAND: 4, SELECT: 5,
              WALK: 6, SITE: 7, ATTACK: 8, ADVISE: 9, PEACE: 10, ALT: 11 };
const PTR_HOT = [0, 6, 8, 6, 8, 8, 8, 8, 0, 8, 8, 0];
const PTR_OK = new Set([move.ROAD, move.BRIDGE, move.WATER, move.SHORE, move.CITY]);

const inRect = (r, x, y) => x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h;
const held = (...k) => keys.isDown(...k);

function pointerKind(x, y) {
  if (G.starting || G.over || G.banner || G.offer || G.assault || G.victory || G.aiRun
      || G.openMenu != null || kit.top() || !G.player || G.player.computer) {
    return PTR.ARROW;
  }
  if (G.drag) return PTR.HAND;                 // 4125:1176, while it drags
  const alt = held("lalt", "ralt"), ctrl = held("lctrl", "rctrl");
  if (inRect(G.stratRect, x, y)) return alt ? PTR.ALT : PTR.VIEW;
  const tile = tileAtPoint(x, y);
  if (!tile) return inRect(G.panelRect, x, y) ? PTR.HAND : PTR.ARROW;
  const [tx, ty] = tile;
  const g = G.g, me = G.player.index;
  if (tx < 0 || ty < 0 || tx >= g.map.width || ty >= g.map.height) return PTR.HAND;
  const here = game.armiesAt(g, tx, ty);
  const city = game.cityAt(g, tx, ty);
  const armies = here.length > 0;
  // the tile's owner nibble: whoever stands there, else the city's, else none
  let owner = armies ? here[0].owner : undefined;
  if (owner == null) owner = city ? city.ownerIndex : undefined;
  if (owner == null) owner = rules.NEUTRAL;
  const t = city ? (city.razed ? move.SITE : move.CITY) : scn.terrainAt(g.map, tx, ty);

  const sel = G.selection && G.selection.stack.length > 0 ? G.selection.stack : null;
  let dist = 0, mode = null, under = move.PLAIN, atSea = false;
  if (sel) {
    dist = Math.max(Math.abs(tx - sel[0].x), Math.abs(ty - sel[0].y));
    mode = move.modeOf(g, sel);
    under = scn.terrainAt(g.map, sel[0].x, sel[0].y);
    atSea = !!sel[0].atSea;
  }
  const cost = (sel && mode !== move.FLYING) ? (move.COST[t] || 0) : 1;
  if (!game.seen(g, G.player, tx, ty) || cost <= 0) return PTR.HAND;

  const walk = () => (mode !== move.FLYING && (t === move.WATER || t === move.SHORE) ? PTR.BOAT : PTR.WALK);
  const look = () => {
    if (t === move.CITY) return PTR.CITY;
    if (t === move.SITE) return PTR.SITE;
    return PTR.HAND;
  };
  const enemyNear = sel && dist <= 1 && owner !== me && (t === move.CITY || armies);

  if (held("lshift", "rshift")) {
    if (enemyNear && g.map.options.militaryAdvisor !== 0) return PTR.ADVISE;
    return look();
  }
  if (armies && owner === me && !ctrl) {
    if (alt) return PTR.ALT;
    if (!sel || dist < 1) return PTR.SELECT;
    return walk();
  }
  if (enemyNear
      && !(atSea && mode === move.LAND && under === move.SHORE && !PTR_OK.has(t))
      && !(!atSea && mode === move.LAND && t === move.SHORE && !PTR_OK.has(under))) {
    if (g.map.options.diplomacy === 0 || owner === rules.NEUTRAL) return PTR.ATTACK;
    const st = diplomacy.state(g, me, owner);
    if (t === move.CITY) return st === diplomacy.WAR ? PTR.ATTACK : PTR.PEACE;
    return st !== diplomacy.PEACE ? PTR.ATTACK : PTR.PEACE;
  }
  if (sel && (owner === me || (owner === rules.NEUTRAL && t !== move.CITY))) {
    if (alt) return PTR.ALT;
    if (dist >= 1 && ctrl && armies) return PTR.SELECT;
    return walk();
  }
  return look();
}
G.pointerKind = pointerKind;

function drawPointer() {
  if (displayMod.pointerAway()) return;
  const mx = G.mouse.x, my = G.mouse.y;
  const k = pointerKind(mx, my);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.pointerImg, G.pointerQuads[k], mx - PTR_HOT[k], my - PTR_HOT[k]);
}

// The start-of-turn banner (8cc6:0259): popup 6, (160, 60) 320x312 --
// exactly CITY.PCK -- framed in the side's own colour, with the side's name
// and "Turn %d" over it. 7ecb:0142 then blocks until any input arrives.
const BANNER = { x: 160, y: 60, w: 320, h: 312 };
const BANNER_NAME_Y = 85, BANNER_TURN_Y = 130;

function showBanner(side) {
  G.banner = { name: side.name, turn: G.g.turn, colour: side.colour ?? 15, edge: side.edge ?? 0 };
  sound.effect("turn");                          // the fanfare, TURN.8SN
}

/** Close the banner, and only then let the turn's first dialog through. */
function dismissBanner() {
  G.banner = null;
  if (G.centreAfterBanner && G.player.capital) centreOn(G.player.capital.x, G.player.capital.y);
  G.centreAfterBanner = null;
  // the advisor has his say (6dda:026f(5)), then 8cc6:04bd puts on the
  // hero's music or the turn's
  const says = sound.speechOn() ? cues.advisor(G.g, G.player, (n) => Math.floor(Math.random() * n) + 1) : null;
  advisorUi.say(says, () => {
    sound.music(G.player.heroOffer ? cues.HERO : cues.PLAY);
    const moments = [];
    if (G.g.turn === 2) moments.push("turn2");
    if (G.player.heroOffer) moments.push("hero");
    tutorial.chain(moments, () => {
      if (!presentOffer(G.player)) afterOffer();
    });
  });
}

// The hero offer is popup 2 -- (80, 60) 480x312, MARBLE.PCK cropped -- with
// dialog 12's controls laid on it, every number from auto_ui_hero_emerges
// (6563:0d5c).
const HERO = {
  POPUP: { x: 80, y: 60, w: 480, h: 312 },
  DIALOG: 12,
  MAP: { x: 80, y: 60, w: 224, h: 312 },
  PIC: { x: 320, y: 110, w: 224, h: 170 },
  TITLE_Y: 63,
  CENTRE: 432,
  LINE_Y: [190, 210, 230, 250],
  MALE_LABEL: { x: 376, y: 315 }, FEMALE_LABEL: { x: 480, y: 315 },
  BOX_W: 24, BOX_H: 20,
  CHECKED: { x: 320, y: 0 }, CLEAR: { x: 320, y: 20 },
  OK: 287, CANCEL: 288, FIELD: 289, MALE: 290, FEMALE: 291,
  TITLE_GROUP: 0x5f, LINE_GROUP: 0x61, SEX_GROUP: 0x62,
  NAME_MAX: 19,
};

/** The four lines over the picture, as 6563:0d5c assembles them. */
function heroLines(offer, female) {
  const ui = G.screen.ui, side = G.player;
  const first = female ? 8 : 0;
  const s = (i) => uidata.text(ui, HERO.LINE_GROUP, first + i);
  if (offer.first) return [s(0), s(1), s(2), fmt(s(3), offer.city.name)];
  return [fmt(s(0), offer.city.name), fmt(s(1), offer.price), fmt(s(2), side.gold), s(3)];
}

function presentOffer(side) {
  if (!side.heroOffer) return false;
  if (!G.heroView) G.heroView = screen.dialog(G.screen, HERO.DIALOG);
  const offer = side.heroOffer;
  G.offer = offer;
  // the name and sex were rolled with the offer; the checkboxes only change
  // the wording and the picture
  G.offerFemale = offer.female || false;
  G.offerName = offer.name || "Hero";
  G.heroView.state[HERO.CANCEL] = offer.first ? uidata.DISABLED : uidata.NORMAL;
  G.heroView.state[HERO.OK] = uidata.NORMAL;
  return true;
}

/** Take the offer: the hero joins under the name and sex now in the dialog. */
function acceptOffer() {
  const offer = G.offer;
  offer.name = G.offerName; offer.female = G.offerFemale;
  const [h, allies] = hero.recruit(G.g, G.player, offer);
  G.offer = null; G.player.heroOffer = null;
  centreOn(h.x, h.y);
  if (allies.length > 0) {
    say("%s joins at %s, with %d %s.", h.name, G.g.map.cities[h.homeCity].name, allies.length, allies[0].name);
  } else {
    say("%s joins at %s.", h.name, G.g.map.cities[h.homeCity].name);
  }
  afterOffer();
}

/** The turn's opening, once any hero offer is settled: on turn 1 the
 *  capital's dialog opens in Production. */
function afterOffer() {
  if (G.g.turn === 1 && G.player.capital && !G.player.computer) cityUi.open(G.player.capital, cityUi.PRODUCTION);
}

function refuseOffer() {
  G.offer = null; G.player.heroOffer = null;
  afterOffer();
  say("The hero rides away.");
}

// ------------------------------------------------------------ the computer

// The computer's turns run as a generator (the Lua remake's coroutine), so
// the screen keeps being drawn while they are played: it gives a frame back
// after every side's turn, and stops after each walk worth showing. A turn
// is shown when its side is observed or no human is left (8cc6:0000); a walk
// only if the player has seen any of it. Holding Shift or Alt between turns
// opens Settings (5db9:045e); any other key or a click runs the rest of the
// round through unwatched.

function humansLeft() {
  return G.g.sides.some((s) => s.alive && !s.computer && game.sideCities(G.g, s).length > 0);
}

function shownSide(side) {
  return side && (side.observe || !humansLeft());
}

ai.hooks.onWalk = function* (g, stack, r) {
  if (!G.aiRun || G.aiSkip || !shownSide(G.aiSide)) return;
  let seen = !humansLeft();
  for (const t of r.walked || []) {
    if (seen) break;
    if (game.seen(g, G.player, t.x, t.y)) seen = true;
  }
  if (seen) yield ["walk", stack.slice(), r.walked];
};

// A line in the status bar, and the time 8065:11d1 then waits, in BIOS ticks
function* status(text, ticks) {
  if (G.aiStatus) G.aiStatus.text = text;
  if (ticks && ticks > 0 && !G.aiSkip) yield ["pause", ticks];
}

// ai_turn's progress bar after each phase (5db9:0000)
const PROGRESS = {
  diplomacy: 10, "move hero": 15, "move search": 20, "move explore": 25,
  assault: 30, "move #1": 40, rescue: 45, evaluate: 50, "clean city": 55,
  neutral: 60, "move #2": 65, "assault XX": 70, specials: 75,
  rebuilding: 80, "last rescue": 85, production: 90, vectoring: 95,
};

// A computer's battle (auto_ui_being_attacked, 67cc:124a): who is attacked
// goes up in the status bar, and then how it went, five ticks each -- when
// the attacker's turn is watched or the defender is a human.
ai.hooks.onFight = function* (g, stack, x, y, result) {
  if (!G.aiRun || !G.aiStatus) return;
  const lines = result.lines || {};
  let def;
  if (lines.city) def = lines.city.ownerIndex;
  else if (lines.defenders && lines.defenders[0]) def = lines.defenders[0].owner;
  const defSide = def != null && def < 8 ? g.map.sides[def] : null;
  const human = defSide && !defSide.computer;
  if (!(shownSide(G.aiSide) || human)) return;
  const humans = g.sides.filter((s) => s.alive && !s.computer);
  if (g.map.options.hiddenMap !== 0 && humans.length === 1 && !game.seen(g, humans[0], x, y)) return;
  const t = (grp) => uidata.text(G.screen.ui, grp, 0);
  const heroA = (lines.attackers || []).find((a) => a.type === armytype.HERO);
  const me = G.aiSide.name;
  let outcome;
  if (!result.won) outcome = fmt(t(0x98), me);
  else if (heroA) outcome = fmt(t(0x8e), heroA.name || "");
  else outcome = fmt(t(0x96), me);
  const hiddenMany = g.map.options.hiddenMap !== 0 && humans.length > 1;
  const window_ = !hiddenMany && ((shownSide(G.aiSide) && defSide != null) || human);
  const cloud = !hiddenMany;
  yield* status(defSide ? fmt(t(0x94), defSide.name) : t(0x95), 5);
  if (cloud && !G.aiSkip) {
    startAssault(x, y, result);
    const a = G.assault;
    a.computer = true; a.window = window_; a.outcome = outcome;
    a.message = [outcome];
    yield ["assault"];
    if (window_) return;
  }
  yield* status(outcome, 5);
};

function resumeComputer() {
  const co = G.aiRun;
  if (!co) return;
  G.aiWait = null;
  let r;
  try {
    r = co.next();
  } catch (e) {
    G.aiRun = null;
    throw e;
  }
  if (r.done) {
    G.aiRun = null; G.aiSkip = null; G.aiSide = null;
    finishRound(r.value);
    return;
  }
  stratDirty();
  const [what, a, b] = r.value || [];
  if (what === "pause") {
    G.aiResumeAt = now() + a / 18.2;
    G.aiWait = true;
  } else if (what === "walk") {
    startWalk(a, b);
    if (G.walk) G.walk.computer = true; else G.aiWait = true;
  } else if (what === "nohumans") {
    // 8065:1c6f: the last human is gone, and the war goes on
    const t = (i) => uidata.text(G.screen.ui, 0xd, i);
    searchUi.message(t(0), t(1), () => {
      searchUi.message(t(2), t(3), () => { G.aiWait = true; });
    });
  } else {
    // a turn done, or a battle up: go on at the next frame free of it
    G.aiWait = true;
  }
}
G.resumeComputer = () => resumeComputer();

/** Called every frame: carry the computer on once nothing is in its way. */
function stepComputer() {
  if (!G.aiRun || !G.aiWait || G.walk || G.assault || kit.top()) return;
  if (G.aiResumeAt) {
    if (now() < G.aiResumeAt && !G.aiSkip) return;
    G.aiResumeAt = null;
  }
  if (held("lshift", "rshift") || held("lalt", "ralt")) {
    G.aiSkip = null;
    settingsUi.open();
    return;
  }
  // several steps a frame while running through unwatched
  const until = now() + 0.03;
  do {
    resumeComputer();
  } while (G.aiSkip && G.aiRun && G.aiWait && !G.walk && !G.assault && !kit.top()
           && !G.aiResumeAt && now() < until);
}

function* computerTurns(side) {
  while (side && side.computer) {
    G.aiSide = side;
    // 8065:2123: each computer turn opens with a song that plays once
    sound.music(cues.COMPUTER, G.g.sides);
    G.aiStatus = { side, text: side.name, progress: 5 };
    yield ["progress"];
    if (shownSide(side)) {
      for (const m of side.diploNews || []) yield* status(m, 20);
    }
    side.diploNews = null;
    yield* ai.playTurn(G.g, side, function* (phase) {
      G.aiStatus.progress = PROGRESS[phase] ?? G.aiStatus.progress;
      yield ["progress"];
    });
    G.aiStatus.progress = 100;
    yield ["progress"];
    side = game.endTurn(G.g);
    if (G.g.ending && G.g.ending.noHumans) yield ["nohumans"];
    yield ["turn"];
  }
  return side;
}

/** Play computer sides from `side` on, until a human's turn or the end. */
function playComputers(side) {
  G.aiRun = computerTurns(side);
  resumeComputer();
}
G.playComputer = () => playComputers(G.player);

function endTurn() {
  G.selection = null;
  const side = game.endTurn(G.g);
  if (side && side.computer) return playComputers(side);
  finishRound(side);
}

/** The round is over: the next human side takes the keyboard. */
function finishRound(side) {
  if (!side) {
    G.aiStatus = null;
    G.over = true;
    say("%s", G.g.log[G.g.log.length - 1] || "The game is over.");
    endingUi.over(G.g.ending);
    return;
  }
  G.player = side;
  G.aiStatus = null;                    // 8065:0aeb: the bar is the player's again
  stratDirty();
  cycleReset();
  // 8cc6:0259 centres on the side's capital -- but where the map is hidden
  // and more than one human plays, only once the banner is gone
  let humans = 0;
  for (const s of G.g.map.sides) if (s.inUse && s.alive !== false && !s.computer) humans++;
  G.centreAfterBanner = G.g.map.options.hiddenMap !== 0 && humans > 1;
  if (!G.centreAfterBanner && side.capital) centreOn(side.capital.x, side.capital.y);
  say("Turn %d. %d gold, income %d.", G.g.turn, side.gold, side.income || 0);
  endingUi.show(G.g.ending, () => showBanner(side));
}

function openCity(city) {
  cityUi.open(city);
  say("%s", city.name);
}
G.openCity = (city) => openCity(city);

function takeLoaded(loaded) {
  G.g = loaded;
  G.selection = null;
  stratDirty();
  G.player = loaded.sides[loaded.current];
  centreOn(G.player.capital.x, G.player.capital.y);
  sound.music(cues.PLAY);               // 7721:02d3
}
G.takeLoaded = (g) => takeLoaded(g);
G.startAssault = (x, y, result) => startAssault(x, y, result);
G.presentVictory = (city, victor) => presentVictory(city, victor);

function loadQuick() {
  const loaded = savegame.readSlot("w2:quick", G.dataDir);
  if (!loaded) { say("Nothing to load."); return; }
  takeLoaded(loaded);
  say("Loaded: turn %d, %s to play.", loaded.turn, G.player.name);
}

// ------------------------------------------------------------------ drawing

function topArmy(stack) {
  let best = null, bestRank = null;
  for (const a of stack) {
    const rank = G.g.map.fightOrder[a.owner ?? 8][a.type] || 0;
    if (bestRank === null || rank > bestRank) { best = a; bestRank = rank; }
  }
  return best;
}

// The figure a stack shows (8611:2985): its top army's, or a boat -- the
// Navy, type 5 -- when an army in it is at sea.
const NAVY = 5;
function stackFigure(stack) {
  if (stack.some((a) => a.atSea)) return NAVY;
  const a = topArmy(stack);
  return a ? a.type : null;
}

// The hidden map (8611:24c2): over each unseen tile, black through its
// HIDDEN.PCK cell; cell 14, a tile with nothing seen round it, is black.
function drawFog() {
  if (G.g.map.options.hiddenMap === 0) return;
  const v = G.view;
  for (let my = v[1]; my <= v[3]; my++) {
    for (let mx = v[0]; mx <= v[2]; mx++) {
      const cell = game.fogCell(G.g, G.player, mx, my);
      if (cell != null) {
        const sx = mx * TILE, sy = my * TILE;
        if (cell === game.FOG_BLACK) {
          gfx.setColor(0, 0, 0);
          gfx.rectangle("fill", sx, sy, TILE, TILE);
        } else {
          gfx.setColor(1, 1, 1);
          gfx.draw(G.fogImg, G.fogQuads[cell], sx, sy);
        }
      }
    }
  }
  gfx.setColor(1, 1, 1);
}

/** The map, in map pixels: the caller has the transform and the scissor up. */
function drawMap() {
  G.view = viewTiles();
  const v = G.view;
  const armiesOn = new Map();
  for (const a of G.g.armies) {
    if (!a.transit && a.x != null && a.x >= v[0] && a.x <= v[2] && a.y >= v[1] && a.y <= v[3]) {
      const k = a.x + a.y * 1000;
      if (!armiesOn.has(k)) armiesOn.set(k, []);
      armiesOn.get(k).push(a);
    }
  }
  // one item a tile: the last in item order (8611:2d7c walks them from the end)
  G.itemsOnTile = new Map();
  for (let i = G.g.map.items.length - 1; i >= 0; i--) {
    const it = G.g.map.items[i];
    if (it.status === 1 && it.x != null && !G.itemsOnTile.has(it.x + it.y * 1000)) G.itemsOnTile.set(it.x + it.y * 1000, it);
  }
  const standing = [];
  for (let my = v[1]; my <= v[3]; my++) {
    for (let mx = v[0]; mx <= v[2]; mx++) {
      const sx = mx * TILE, sy = my * TILE;
      const t = scn.tileAt(G.g.map, mx, my);
      const sheet = Math.floor(t / 96);
      gfx.setColor(1, 1, 1);
      gfx.draw(G.sheets[sheet], G.tileQuads[sheet][t % 96], sx, sy);
      const rd = scn.roadAt(G.g.map, mx, my);
      if (rd !== 0) gfx.draw(G.roadImg, G.roadQuads[rd - 1], sx, sy);
      // items on the ground (8611:2d7c): a planted standard as its side's
      // flag, army cell 29; anything else as the bag
      const here = G.itemsOnTile.get(mx + my * 1000);
      if (here) {
        if (here.planted && here.standardOf != null) {
          gfx.draw(G.armyImg[here.standardOf], G.armyQuads[here.standardOf][29], sx, sy);
        } else {
          gfx.draw(G.atransShields, G.bagQuad, sx, sy);
        }
      }
      let stack = armiesOn.get(mx + my * 1000) || [];
      // a stack still walking is drawn where the walk has got to
      if (G.walk) stack = stack.filter((a) => !G.walk.armies.has(a));
      if (stack.length > 0) standing.push({ stack, x: mx, y: my, sx, sy });
    }
  }
  // the ground first, then the encampments, then the stacks (8611:1a79)
  const sel = G.selection;
  for (const s of standing) {
    if (game.towerAt(G.g, s.x, s.y)) {
      let owner = s.stack[0].owner ?? 8;
      if (owner === 15) owner = 8;
      gfx.setColor(1, 1, 1);
      gfx.draw(G.roadImg, gfx.newQuad(owner * 48 + 192, 40, 48, 40), s.sx, s.sy);
      s.hidden = !(sel && sel.x === s.x && sel.y === s.y);
    }
  }
  // the selected stack's tile shows the lead of the group that moves
  let lead = sel && sel.stack.length > 0 ? topArmy(sel.stack) : null;
  if (lead && G.walk && G.walk.armies.has(lead)) lead = null;
  G.figures = {};
  for (const s of standing) {
    if (!s.hidden) {
      let a = topArmy(s.stack), figure = stackFigure(s.stack);
      if (lead && sel.x === s.x && sel.y === s.y) {
        a = lead;
        figure = lead.atSea ? NAVY : lead.type;
      }
      G.figures[s.x + s.y * 1000] = figure;
      drawStack(a.owner ?? 8, figure, s.stack.length, s.sx, s.sy);
    }
  }
  drawFog();
  drawRoute();

  // the stack itself, wherever the walk has got to
  const at = walkingAt();
  if (at) {
    const a = G.walk.top;
    if (a && G.shown(at.x, at.y)) {
      const walking = [...G.walk.armies];
      drawStack(a.owner ?? 8, stackFigure(walking), walking.length, at.x * TILE, at.y * TILE);
    }
  }

  // the selection box, which follows the walk
  if (G.selection) {
    const x = at ? at.x : G.selection.x, y = at ? at.y : G.selection.y;
    if (G.shown(x, y)) {
      gfx.setColor(1, 1, 1);
      const base = G.selection.stack.length > 1 ? 4 : 0;
      const frame = base + Math.floor(now() * CURS_RATE) % 4;
      gfx.draw(G.cursImg, G.cursQuads[frame], x * TILE, y * TILE);
    }
  }
}

/** The strategic map as the original paints it anywhere it appears, with
 *  every city it can see as an 8x8 shield (834b:0ed7). `mark` gets a white
 *  box; `owners` is who held each city, for History (834b:12d3). */
G.drawStrategicMap = (x, y, mark, noCities, owners) => {
  if (!G.stratImage) G.stratImage = screen.strategicImage(G.screen, G.g, G.player, game.seen);
  gfx.setScissor(x, y, 224, 312);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.stratImage, x, y);
  if (!noCities) {
    G.g.map.cities.forEach((c, i) => {
      const owner = owners ? owners[i] : null;
      const gone = owners ? owner === 0xff : c.razed;
      if (!gone && game.seen(G.g, G.player, c.x, c.y)) {
        const side = owners ? owner : (c.ownerIndex ?? 8);
        gfx.draw(G.atransShields, gfx.newQuad(side * 16, 30, 8, 8), x + c.x * 2 - 1, y + c.y * 2 - 1);
      }
    });
  }
  if (mark) {
    const mx = x + mark.x * 2 - 1, my = y + mark.y * 2 - 1;
    gfx.setColor(G.palette[15]);
    gfx.rectangle("fill", mx - 1, my - 1, 10, 1);
    gfx.rectangle("fill", mx - 1, my + 9, 10, 1);
    gfx.rectangle("fill", mx - 1, my - 1, 1, 10);
    gfx.rectangle("fill", mx + 9, my - 1, 1, 10);
  }
  gfx.setScissor();
};

/** Every site shown to the side, marked on a strategic map (834b:05e4). */
G.drawSiteMarkers = (x, y, highlight) => {
  gfx.setScissor(x, y, 224, 312);
  for (const s of G.g.map.sites) {
    if (siteMod.shownTo(s, G.player) && game.seen(G.g, G.player, s.x, s.y)) {
      let src;
      if (s.content === siteMod.TEMPLE) src = [112, 20];
      else if (s.searched) src = [112, 10];
      else if (s.rich) src = [128, 0];
      else src = [112, 0];
      const mx = Math.floor((s.x * 2 - 1 + 4) / 8) * 8;
      const my = s.y * 2 - 1;
      gfx.setColor(1, 1, 1);
      gfx.draw(G.atransShields, gfx.newQuad(src[0], src[1], 16, 10), x + mx, y + my);
      if (s === highlight) {
        kit.setPal(15);
        kit.outline(x + mx, y + my, 12, 10);
      }
    }
  }
  gfx.setScissor();
};

function drawStrategic() {
  const r = G.stratRect;
  G.drawStrategicMap(r.x, r.y);
  gfx.setScissor(r.x, r.y, r.w, r.h);
  // the view box (8961:0698): whatever the view covers, to the device pixel
  gfx.setColor(G.palette[15]);
  const s = display.scale;
  const snap = (v) => Math.floor(v * s + 0.5) / s;
  const [vw, vh] = viewSize();
  const bx = r.x + snap(G.cx * 2), by = r.y + snap(G.cy * 2);
  const w = snap(vw * 2), h = snap(vh * 2);
  gfx.rectangle("fill", bx, by, w, 2);
  gfx.rectangle("fill", bx, by + h - 2, w, 2);
  gfx.rectangle("fill", bx, by, 2, h);
  gfx.rectangle("fill", bx + w - 2, by, 2, h);
  gfx.setScissor();
}

/** The fog only ever opens, so the strategic map is rebuilt on demand. */
function stratDirty() { G.stratImage = null; }
G.stratDirty = () => stratDirty();
G.openQuest = () => questUi.open();

// The four configurable buttons are painted a second time from MENUBUTT.PCK
// at the rect the assigned item carries in UDB.DAT (545c:030a).
function drawShortcutIcons() {
  const ui = G.screen.ui;
  const art = G.screen.art_for(uidata.SHORTCUT_BITMAP);
  for (let i = 0; i < uidata.SHORTCUT_COUNT; i++) {
    const c = screen.control(G.screen, uidata.SHORTCUT_FIRST + i);
    const item = ui.shortcutItems[ui.shortcuts[i] ?? -1];
    if (c && item && art) {
      const src = item.src[G.screen.state[c.id] ?? uidata.NORMAL];
      gfx.setColor(1, 1, 1);
      gfx.draw(art.image, gfx.newQuad(src.x, src.y, item.w, item.h), c.x, c.y);
    } else if (c && item) {
      const short = item.name.slice(0, 6);
      gfx.setColor(1, 1, 1);
      G.font.draw(short, c.x + Math.max(1, Math.floor((c.w - G.font.width(short)) / 2)),
                  c.y + Math.floor((c.h - G.font.lineHeight) / 2));
    }
  }
}

function palColour(i) {
  gfx.setColor(G.palette[i]);
}

// A stack on the map (8611:0335): the top army's figure at (8, 7) in the
// tile; the flag pole, three 40-pixel lines in colours 14, 13 and 14; and
// the flag, from the side's own sheet at (464, y) 48 x 8, bigger the more
// armies stand there (4125:2d4e). More than four fly the smallest 8 lower
// as well, and the flag for the rest at the top.
const FLAG_Y = [29, 38, 47, 56];
function drawStack(owner, armyType, count, sx, sy) {
  gfx.setColor(1, 1, 1);
  gfx.draw(G.armyImg[owner], G.armyQuads[owner][armyType % 32], sx + 8, sy + 7);
  palColour(14);
  gfx.rectangle("fill", sx + 2, sy, 1, TILE);
  gfx.rectangle("fill", sx + 4, sy, 1, TILE);
  palColour(13);
  gfx.rectangle("fill", sx + 3, sy, 1, TILE);
  gfx.setColor(1, 1, 1);
  const flag = (k, y) => gfx.draw(G.armyImg[owner], gfx.newQuad(464, FLAG_Y[k - 1], 48, 8), sx, y);
  if (count > 4) { flag(1, sy + 8); count -= 4; }
  flag(Math.max(1, Math.min(count, 4)), sy);
}

// Turn and the sides still in it, at the right of the bar (8cc6:0952).
function drawTurnStrip() {
  palColour(15);
  gfx.rectangle("fill", 437, 0, 202, 17);
  const alive = G.g.sides.filter((s) => s.alive);
  const n = alive.length;
  const turn = fmt("Turn %d", G.g.turn);
  const f = G.bigFont.colours(0, 15);
  f.draw(turn, (8 - n) * 16 + 508 - f.width(turn), 0);
  alive.forEach((side, k) => {
    const x = (8 - n + k) * 16 + 512;
    if (side === G.player) {
      palColour(0);
      gfx.rectangle("fill", x - 3, 1, 17, 15);
    }
    const s = side.index;
    gfx.setColor(1, 1, 1);
    gfx.draw(G.atransShields, gfx.newQuad(112 + Math.floor(s / 4) * 16, 94 + (s % 4) * 14, 16, 14), x, 2);
  });
}

// The menu bar (7ae8:02d8, 2372:132f): white, the titles in TEXT, black. An
// open title and the row under the pointer are XORed with colour 7; a greyed
// item's glyph is colour 3. The dropdown (2372:0803) is white with a black
// outline and a shadow.
function drawMenuBar() {
  palColour(15);
  gfx.rectangle("fill", 0, 0, G.layout.w, menuMod.BAR_H);
  G.menuLayout.forEach((m, i) => {
    const lit = i === G.openMenu;
    if (lit) {
      palColour(8);
      gfx.rectangle("fill", m.x, 0, m.w, menuMod.BAR_H);
    }
    gfx.setColor(1, 1, 1);
    G.font.colours(lit ? 7 : 0, lit ? 8 : 15).draw(m.title, m.x + 2, menuMod.BAR_Y);
  });
  if (!G.starting) {
    gfx.push();
    gfx.translate(G.layout.ex, 0);
    drawTurnStrip();
    gfx.pop();
  }
  const open = G.openMenu != null ? G.menuLayout[G.openMenu] : null;
  if (!open) return;
  const d = open.drop;
  palColour(15);
  gfx.rectangle("fill", d.x, d.y, d.w - 1, d.h - 1);
  palColour(0);
  gfx.rectangle("fill", d.x, d.y + d.h - 2, d.w - 2, 1);
  gfx.rectangle("fill", d.x + 2, d.y + d.h - 1, d.w - 2, 1);
  gfx.rectangle("fill", d.x, d.y, 1, d.h - 2);
  gfx.rectangle("fill", d.x + d.w - 2, d.y, 1, d.h - 1);
  gfx.rectangle("fill", d.x + d.w - 1, d.y + 1, 1, d.h - 1);
  const hover = menuMod.rowAt(G.menuLayout, G.openMenu, G.mouse.x, G.mouse.y);
  for (const r of d.rows) {
    if (r.label === "-") {
      palColour(0);
      gfx.rectangle("fill", r.x + 1, r.y - 1, d.w - 2, 1);
    } else {
      const grey = !G.menuEnabled(r.key || r.act);
      const lit = r === hover && !grey;
      if (lit) {
        palColour(8);
        gfx.rectangle("fill", r.x + 1, r.y, d.w - 3, r.h);
      }
      const glyph = grey ? 3 : (lit ? 7 : 0);
      const f = G.font.colours(glyph, lit ? 8 : 15);
      gfx.setColor(1, 1, 1);
      f.draw(r.label, r.x + 3, r.y);
      if (r.key) f.draw(r.key, r.x + d.keyCol, r.y);
      // the setting in use (not the original's): a diamond in the key column
      if (G.menuTicked(r.key || r.act)) {
        palColour(glyph);
        const cx = r.x + d.keyCol + 2, cy = r.y + Math.floor(r.h / 2);
        for (let k = -2; k <= 2; k++) {
          const half = 2 - Math.abs(k);
          gfx.rectangle("fill", cx - half, cy + k, half * 2 + 1, 1);
        }
      }
    }
  }
}

// The bottom bar's eight army slots and the mark under each are real
// controls -- ids 224-231 the slots, 232-239 the marks, 240/241 the Grp
// button -- drawn from ABITS.PCK and the army sheets at the rects 89e0:0356
// and 8611:08be use (docs/re/ui.md > The army slots).
const SLOT_FIRST = 224, BAR_FIRST = 232, SLOT_COUNT = 8;
const GRP_ALL = 240, GRP_NONE = 241;
const SLOT_X = 24, SLOT_Y = 405, SLOT_STEP = 40;
const SLOT_MOVES_X = 8, SLOT_MOVES_Y = 31;
const MARK_Y = 449;
const MARK_W = 32, MARK_H = 16;
const MARK_SRC = { [slotsMod.CROSS]: [448, 0], [slotsMod.TICK]: [448, 16] };
const DIGIT_SRC = [64, 30], DIGIT_W = 8;
const GROUP_SRC = [0, 30, 32, 8], MOVE_SRC = [32, 30, 32, 8];
const GROUP_AT = [344, 420], MOVE_AT = [344, 428];
const GROUP_MOVES_AT = [352, 436];
const GRP_SRC = { red: [288, 0], green: [288, 19] };
const MOVE_ICON_AT = [344, 407], MOVE_ICON_W = 32, MOVE_ICON_H = 10;
const MOVE_ICON = { fly: [184, 30], sea: [424, 30], both: [216, 30], woods: [248, 30], hills: [152, 30] };
const GRP_AT = [344, 447], GRP_W = 32, GRP_H = 19;

function drawAbits(src, w, h, x, y) {
  gfx.setColor(1, 1, 1);
  gfx.draw(G.abits, gfx.newQuad(src[0], src[1], w, h), x, y);
}

/** A number in ABITS's own 8x8 digits, always two of them ("%02d"). */
function drawDigits(n, x, y) {
  const text = fmt("%02d", Math.max(0, Math.min(99, n)));
  for (let i = 0; i < text.length; i++) {
    const d = text.charCodeAt(i) - 48;
    drawAbits([DIGIT_SRC[0] + d * DIGIT_W, DIGIT_SRC[1]], DIGIT_W, DIGIT_W, x + i * DIGIT_W, y);
  }
}

/** The ring a slot's army sits in: each group a colour of its own, counted
 *  round from the current player's. */
function groupRing(group) {
  if (group == null) return 0;
  return (G.player.index + group) % 8 + 1;
}

function drawArmySlots() {
  const s = G.selection && G.selection.slots;
  for (let i = 0; i < SLOT_COUNT; i++) {
    const x = SLOT_X + i * SLOT_STEP;
    const a = s ? s.army[i] : null;
    gfx.setColor(1, 1, 1);
    gfx.draw(G.abits, G.ringQuads[a ? groupRing(s.group[i]) : 0], x, SLOT_Y);
    if (a) {
      // the armies not moving with the group are drawn as ghosts
      const img = s.inGroup[i] ? G.armyImg[G.player.index] : G.shadowImg;
      gfx.draw(img, G.armyQuads[G.player.index][a.type % 32], x, SLOT_Y);
      drawDigits(a.moves || 0, x + SLOT_MOVES_X, SLOT_Y + SLOT_MOVES_Y);
      const mark = s.mark[i] != null ? MARK_SRC[s.mark[i]] : null;
      if (mark) drawAbits(mark, MARK_W, MARK_H, x, MARK_Y);
    }
  }
  // "Group Move", the group's movement, and the Grp button (89e0:0567)
  drawAbits(GROUP_SRC, GROUP_SRC[2], GROUP_SRC[3], GROUP_AT[0], GROUP_AT[1]);
  drawAbits(MOVE_SRC, MOVE_SRC[2], MOVE_SRC[3], MOVE_AT[0], MOVE_AT[1]);
  drawDigits(s ? slotsMod.moves(s) : 0, GROUP_MOVES_AT[0], GROUP_MOVES_AT[1]);
  drawAbits(s && slotsMod.grouped(s) ? GRP_SRC.green : GRP_SRC.red, GRP_W, GRP_H, GRP_AT[0], GRP_AT[1]);

  // the way the moving group travels (89e0:070d)
  const moving = [];
  if (s) s.army.forEach((a, i) => { if (s.inGroup[i]) moving.push(a); });
  let icon = null;
  if (moving.length > 0) {
    let sea = true, woods = false, hills = false;
    for (const a of moving) {
      const t = G.g.types.byId[a.type];
      if (t.woodsMove) woods = true;
      if (t.hillsMove) hills = true;
      if (!a.atSea) sea = false;
    }
    if (move.modeOf(G.g, moving) === move.FLYING) icon = MOVE_ICON.fly;
    else if (sea) icon = MOVE_ICON.sea;
    else if (woods && hills) icon = MOVE_ICON.both;
    else if (woods) icon = MOVE_ICON.woods;
    else if (hills) icon = MOVE_ICON.hills;
  }
  if (icon) {
    drawAbits(icon, MOVE_ICON_W, MOVE_ICON_H, MOVE_ICON_AT[0], MOVE_ICON_AT[1]);
  } else {
    palColour(3);
    gfx.rectangle("fill", MOVE_ICON_AT[0], MOVE_ICON_AT[1], MOVE_ICON_W, MOVE_ICON_H);
  }
}

// With nothing selected the bar shows the side's standing (89e0:05a3):
// cities, treasury, income and upkeep, four 40 x 20 cells of ABITS.PCK each
// with a number beside it (4125:3072, :3092, :30a2, :3128).
const STATUS_W = 40, STATUS_H = 20;
const STATUS = [
  { sx: 344, sy: 0, x: 32, tx: 72, fmt: "%d", value: () => game.sideCities(G.g, G.player).length },
  { sx: 344, sy: 20, x: 120, tx: 144, fmt: "%dgp", value: () => G.player.gold },
  { sx: 384, sy: 0, x: 200, tx: 232, fmt: "%dgp", value: () => game.income(G.g, G.player) },
  { sx: 384, sy: 20, x: 280, tx: 320, fmt: "%dgp", value: () => game.upkeep(G.g, G.player) },
];
const STATUS_Y = 425;

function drawStatus() {
  for (const st of STATUS) {
    drawAbits([st.sx, st.sy], STATUS_W, STATUS_H, st.x, STATUS_Y);
    G.bigFont.draw(fmt(st.fmt, st.value()), st.tx, STATUS_Y);
  }
}

// 8065:0aeb blits MARBLE.PCK over the bar, (16, 403) 360x66 from (0, 60).
const BAR_GROUND = { x: 16, y: 403, w: 360, h: 66, sx: 0, sy: 60 };

// A computer's turn in the status bar (8cc6:0000, 5db9:0000): a line centred
// on (196, 415) in the side's colours, a black frame at (36, 442) 320x22, and
// the first n / 5 * 16 pixels of the side's MOVEBAR<n>.PCK at (32, 444).
G.drawComputerStatus = () => {
  const st = G.aiStatus;
  const side = st.side;
  gfx.setColor(1, 1, 1);
  kit.centred(G.bigFont.colours(side.colour ?? 15, side.edge ?? 0), st.text || "", 196, 415);
  palColour(0);
  kit.outline(36, 442, 320, 22);
  const w = Math.floor((st.progress || 0) / 5) * 16;
  if (w > 0) {
    G.moveBars = G.moveBars || {};
    const i = side.index;
    if (!G.moveBars[i]) G.moveBars[i] = pck.toImage(fmt("%s/TERRAIN0/MOVEBAR%d.PCK", G.dataDir, i), G.palette, "corner")[0];
    gfx.setColor(1, 1, 1);
    gfx.draw(G.moveBars[i], gfx.newQuad(0, 0, Math.min(w, 320), 18), 32, 444);
  }
};

/** The right button on the bottom bar (89e0:0ad9). */
const STATUS_HELP = [
  ["Number of Cities", "You have %d cities!"],
  ["Your Treasury", "You have %d gold!"],
  ["Your Income", "You earn %d gold!"],
  ["Your Upkeep", "You pay %d gold!"],
];
function barInfo(x, y) {
  const bx = x - G.layout.offset.bar.x;
  const sel = G.selection;
  if (G.aiStatus) return;
  if (sel) {
    if (bx < 336) {
      const n = Math.floor((bx - 16) / 40);
      if (n >= 0 && n < sel.slots.n) infobox.army(x, y, sel.slots.army[n]);
      else infobox.lines(x, y, "Select Army", "Select armies when present");     // 4125:3222
    } else {
      infobox.lines(x, y, "Group/Ungroup", "Manipulate all armies");            // 4125:3169
    }
    return;
  }
  const k = Math.floor((bx - 16) / 90);
  const help = STATUS_HELP[k];
  if (help) infobox.lines(x, y, help[0], fmt(help[1], STATUS[k].value()));
}

function drawBottomBar() {
  gfx.setColor(1, 1, 1);
  const b = BAR_GROUND;
  gfx.draw(G.marble, gfx.newQuad(b.sx, b.sy, b.w, b.h), b.x, b.y);
  if (G.aiStatus) G.drawComputerStatus();
  else if (G.selection) drawArmySlots();
  else drawStatus();
}

// The city dialog's Vector mode paints the map its own way (834b:08df, and
// 834b:1817 for See All): a marker on each of the side's cities from
// ATRANS2.PCK's row at y = 94 (4125:2c76), and lines for the vectors.
const VMARK_X = [0, 32, 48, 16, 64, 80, 96];

function linePixels(x0, y0, x1, y1) {
  // 2133:00b8 draws a plain Bresenham line, both ends included
  const dx = Math.abs(x1 - x0), dy = -Math.abs(y1 - y0);
  const sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
  let err = dx + dy;
  for (;;) {
    gfx.rectangle("fill", x0, y0, 1, 1);
    if (x0 === x1 && y0 === y1) break;
    const e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

/** Draw the map as the city dialog's Vector mode shows it. `filter` hides
 *  the cities that could take no more vectors. */
G.drawVectorMap = (x, y, city, filter, seeAll) => {
  if (!G.stratImage) G.stratImage = screen.strategicImage(G.screen, G.g, G.player, game.seen);
  gfx.setScissor(x, y, 224, 312);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.stratImage, x, y);
  const marker = (c, k) => {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.atransShields, gfx.newQuad(VMARK_X[k], 94, 16, 10), x + c.x * 2 - 2, y + c.y * 2 - 1);
  };
  const line = (a, b, colour) => {
    gfx.setColor(G.palette[colour]);
    linePixels(x + a.x * 2 + 2, y + a.y * 2 + 2, x + b.x * 2 + 2, y + b.y * 2 + 2);
  };
  const mine = game.sideCities(G.g, G.player);
  const building = (c) => (c.producing != null ? 0 : 3);
  const [sx, sy] = game.standardAt(G.g, G.player);
  const toStandard = (c, colour) => {
    gfx.setColor(G.palette[colour]);
    linePixels(x + c.x * 2 + 2, y + c.y * 2 + 2, x + sx * 2, y + sy * 2);
  };

  if (seeAll) {
    const done = new Set();
    for (const c of mine) {
      if (!done.has(c)) {
        done.add(c);
        const incoming = game.vectoredTo(G.g, c);
        let k = building(c);
        if (c.vectorTo != null) k = 2;
        if (incoming.length > 0) k = c.producing != null ? 1 : 5;
        marker(c, k);
        const dest = c.vectorTo != null && c.vectorTo >= 0 ? G.g.map.cities[c.vectorTo] : null;
        if (dest) {
          done.add(dest);
          marker(dest, dest.producing != null ? 1 : 5);
          line(dest, c, 7);
        } else if (c.vectorTo === game.STANDARD && sx != null) {
          toStandard(c, 8);
        }
        for (const src of incoming) {
          done.add(src);
          marker(src, 2);
          line(src, c, 8);
        }
      }
    }
    gfx.setColor(G.palette[15]);
    const bx = x + (city ? city.x : -10) * 2 - 3, by = y + (city ? city.y : -10) * 2 - 2;
    gfx.rectangle("fill", bx, by, 11, 1);
    gfx.rectangle("fill", bx, by + 11, 11, 1);
    gfx.rectangle("fill", bx, by, 1, 11);
    gfx.rectangle("fill", bx + 11, by, 1, 11);
  } else {
    for (const c of mine) {
      const n = game.vectoredTo(G.g, c).length;
      const full = (filter === -1 && n + 1 > game.MAX_VECTORED_TO)
                || (filter != null && filter >= 0 && n + filter > game.MAX_VECTORED_TO);
      if (c === city || !full) {
        let k = building(c);
        if (c === city) k = c.producing != null ? 4 : 6;
        marker(c, k);
      }
    }
    const dest = city.vectorTo != null && city.vectorTo >= 0 ? G.g.map.cities[city.vectorTo] : null;
    if (dest) {
      marker(dest, dest.producing != null ? 1 : 5);
      line(city, dest, 7);
    } else if (city.vectorTo === game.STANDARD && sx != null) {
      toStandard(city, 7);
    }
    for (const src of game.vectoredTo(G.g, city)) {
      marker(src, 2);
      line(src, city, 8);
    }
  }
  if (sx != null) {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.atransShields, gfx.newQuad(96, 15, 16, 15),
             x + Math.max(0, Math.floor((sx * 2 - 2) / 8) * 8), y + Math.max(0, sy * 2 - 6));
  }
  gfx.setScissor();
};

/** A hero's figure on the strategic map (834b:1f5f). */
G.drawHeroFigure = (x, y, tx, ty) => {
  gfx.setScissor(x, y, 224, 312);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.atransShields, G.heroMark, x + Math.max(0, Math.floor((tx * 2 - 2) / 8) * 8), y + Math.max(0, ty * 2 - 6));
  gfx.setScissor();
};

/** The strategic map in the city dialog's left-hand panel, the city marked. */
G.drawStrategicPanel = (x, y, city) => G.drawStrategicMap(x, y, city);

/** The start-of-turn banner: CITY.PCK framed in the side's colour, the frame
 *  painted into the picture (54f6:0000). */
function drawBanner() {
  const b = G.banner, R = BANNER;
  const fill = (x, y, w, h) => gfx.rectangle("fill", x, y, w, h);
  const setPal = (i) => gfx.setColor(G.palette[i] || G.palette[0]);
  kit.popupFrame(R);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.cityPic, R.x, R.y);
  const outline = (x, y, w, h) => {
    fill(R.x + x, R.y + y, w, 1);
    fill(R.x + x, R.y + y + h - 1, w, 1);
    fill(R.x + x, R.y + y, 1, h);
    fill(R.x + x + w - 1, R.y + y, 1, h);
  };
  setPal(b.edge);
  outline(0, 0, R.w, R.h);
  setPal(b.colour);
  fill(R.x + 1, R.y + 1, R.w - 2, 9);
  fill(R.x + 1, R.y + 1, 9, R.h - 2);
  fill(R.x + 1, R.y + R.h - 10, R.w - 2, 9);
  fill(R.x + R.w - 10, R.y + 1, 9, R.h - 2);
  setPal(b.edge);
  outline(10, 10, R.w - 20, R.h - 20);
  gfx.setColor(1, 1, 1);
  const f = G.titleFont;
  kit.centred(f, b.name, 320, BANNER_NAME_Y);
  kit.centred(f, fmt("Turn %d", b.turn), 320, BANNER_TURN_Y);
}

/** The hero offer: the marble, the map with where the hero would appear,
 *  the portrait, four lines, the name field and the two checkboxes. */
function drawHeroOffer() {
  const R = HERO.POPUP, b = G.offer;
  kit.popupFrame(R);
  gfx.setColor(1, 1, 1);
  gfx.setScissor(R.x, R.y, R.w, R.h);
  gfx.draw(G.marble, R.x, R.y);
  gfx.setScissor();
  G.drawStrategicMap(HERO.MAP.x, HERO.MAP.y);
  gfx.setScissor(R.x, R.y, R.w, R.h);
  gfx.setColor(1, 1, 1);
  const fx = Math.max(0, Math.floor((b.city.x * 2 - 2) / 8) * 8);
  const fy = Math.max(0, b.city.y * 2 - 6);
  gfx.draw(G.atransShields, G.heroMark, HERO.MAP.x + fx, HERO.MAP.y + fy);
  gfx.draw(G.heroPic[G.offerFemale ? "f" : "m"], HERO.PIC.x, HERO.PIC.y);
  gfx.setColor(0, 0, 0);
  gfx.rectangle("line", HERO.PIC.x - 0.5, HERO.PIC.y - 0.5, HERO.PIC.w + 1, HERO.PIC.h + 1);
  gfx.setScissor();

  const ui = G.screen.ui;
  const centred = (f, s, y) => {
    gfx.setColor(1, 1, 1);
    f.draw(s, HERO.CENTRE - Math.floor(f.width(s) / 2), y);
  };
  centred(G.titleFont, uidata.text(ui, HERO.TITLE_GROUP, 0), HERO.TITLE_Y);
  heroLines(b, G.offerFemale).forEach((line, i) => {
    // font 2 in 15 with a colour-14 outline (78a8:06ae(2, 15, 14, 3))
    if (line !== "") centred(G.bigFont.colours(15, 14), line, HERO.LINE_Y[i]);
  });
  const field = screen.dialogControl(G.heroView, HERO.FIELD);
  if (field) {
    gfx.setColor(0, 0, 0);
    kit.outline(field.x - 2, field.y - 2, field.w + 4, field.h + 4);
    kit.field(field.x, field.y, field.w, field.h, G.offerName, G.bigFont);
  }
  const label = (f, at, s) => {
    gfx.setColor(1, 1, 1);
    f.draw(s, at.x - f.width(s), at.y);
  };
  label(G.bigFont, HERO.MALE_LABEL, uidata.text(ui, HERO.SEX_GROUP, 0));
  label(G.bigFont, HERO.FEMALE_LABEL, uidata.text(ui, HERO.SEX_GROUP, 1));
  const box = (id, on) => {
    const c = screen.dialogControl(G.heroView, id);
    if (!c) return;
    const s = on ? HERO.CHECKED : HERO.CLEAR;
    gfx.setColor(1, 1, 1);
    gfx.draw(G.abits, gfx.newQuad(s.x, s.y, HERO.BOX_W, HERO.BOX_H), c.x, c.y);
  };
  box(HERO.MALE, !G.offerFemale);
  box(HERO.FEMALE, G.offerFemale);
  screen.drawDialogControls(G.screen, G.heroView);
}

// ------------------------------------------------------------------ the assault

// A city is not walked into, it is assaulted, and a stack always fights from
// one of the eight tiles around it. What the player sees, in order: the fire
// cloud from WAR.PCK over the tile (67cc:1836), the battle window with both
// lines drawn up (6a35:0160), the fight played back one army at a time in
// the order combat_resolve logged (6a35:0094), how it ended (6a35:04c5), and
// if a city fell, what is to be done with it (63fa:0000). The fight is
// decided before any of it is drawn (docs/re/ui.md > The assault).

const AS = {
  window: { x: 160, y: 60, w: 320, h: 312 },   // popup 8
  cloudW: 128, cloudH: 120,                    // 4125:0cfa
  warW: 240, warH: 180,
  shieldW: 32, shieldH: 36,
  shieldDef: [176, 86], shieldAtk: [176, 246],
  rows: [86, 116, 146, 176],                   // the defender's rows
  atkRow: 246,
  xEven: [216, 248, 280, 312, 344, 376, 408, 440],
  xOdd: [232, 264, 296, 328, 360, 392, 424],
  sea: [0, 162, 32, 18], seaDrop: 10,          // WAR.PCK's water
  textY: 298, textStep: 20,                    // 4125:4366
  cloudTime: 0.7,
  // one casualty (6a35:0094): 5, 3 and 5 BIOS ticks; once hurried, 2 and 3
  fellTime: 13 / 18.2, fellFast: 5 / 18.2,
  blastTime: 5 / 18.2, blastFast: 2 / 18.2,
  fled: 141, wonCityHero: 142, wonCity: 143, wonHero: 144, won: 145, lost: 146, loot: 147,
  vText: 65, vWho: 66, vWhere: 67,
  vPopup: { x: 160, y: 90, w: 320, h: 200 },   // popup 7, exactly VICTORY.PCK
  vDialog: 11,
  occupy: 285, pillage: 283, sack: 286, raze: 284,
  vTitleY: 93, vLineY: [140, 160, 180, 200],
};

function popupFrame(R) {
  gfx.setColor(0, 0, 0);
  gfx.rectangle("line", R.x - 0.5, R.y - 0.5, R.w + 1, R.h + 1);
  gfx.rectangle("fill", R.x + 1, R.y + R.h + 1, R.w + 2, 2);
  gfx.rectangle("fill", R.x + R.w + 1, R.y + 1, 2, R.h + 2);
}

/** Where slot k (from 1) of a line of n sits across the window (6a35:0160). */
function slotX(n, k) {
  if (n >= 8) return AS.xEven[k - 1];
  const from = 4 - Math.floor((n + 1) / 2);
  const t = n % 2 === 1 ? AS.xOdd : AS.xEven;
  return t[from + k - 1];
}

/** A whole line laid out: rows of eight from the top, the last centred. */
function lineSlots(n, rows) {
  const out = [];
  let i = 1, row = 0;
  while (n - i + 1 >= 8 && rows[row + 1] !== undefined) {
    for (let k = 1; k <= 8; k++) out[i + k - 2] = { x: AS.xEven[k - 1], y: rows[row] };
    i += 8; row++;
  }
  const y = rows[row] !== undefined ? rows[row] : rows[rows.length - 1];
  for (let k = 1; k <= n - i + 1; k++) out[i + k - 2] = { x: slotX(n - i + 1, k), y };
  return out;
}

/** The name the spoils dialog calls the victor by: the hero who led the
 *  assault, else the best army in the line. */
function victorName(line) {
  for (const a of line) if (a.type === armytype.HERO && a.name) return a.name;
  const best = line[0];
  const rec = best ? G.g.types.byId[best.type] : null;
  return rec ? rec.name : "Your armies";
}

/** How the fight ended, in the original's own words. */
function outcomeLines(result, city, fled) {
  const ui = G.screen.ui;
  const out = [];
  if (!result.won) {
    out.push(uidata.text(ui, AS.lost, 0));
    return out;
  }
  let heroName = null;
  for (const a of result.lines.attackers) if (a.type === armytype.HERO && a.name) { heroName = a.name; break; }
  if (city) {
    if (fled) out.push(uidata.text(ui, AS.fled, G.g.rng.dice(1, 4, -1)));
    if (heroName) out.push(fmt(uidata.text(ui, AS.wonCityHero, 0), heroName));
    else out.push(uidata.text(ui, AS.wonCity, 0));
  } else if (heroName) {
    out.push(fmt(uidata.text(ui, AS.wonHero, 0), heroName));
    out.push(uidata.text(ui, AS.wonHero, 1));
  } else {
    out.push(uidata.text(ui, AS.won, 0));
  }
  if (result.loot && result.loot > 0) out.push(fmt(uidata.text(ui, AS.loot, 0), result.loot));
  return out;
}

/** Begin showing an assault; the fight is already decided. */
function startAssault(x, y, result) {
  const lines = result.lines || { attackers: [], defenders: [] };
  const fled = lines.defenders.length === 0;
  G.assault = {
    x, y, result,
    def: lines.defenders, atk: lines.attackers,
    defSlots: lineSlots(lines.defenders.length, AS.rows),
    atkSlots: lineSlots(lines.attackers.length, [AS.atkRow]),
    defSide: lines.defenders[0] ? (lines.defenders[0].owner ?? 8) : 8,
    atkSide: lines.attackers[0] ? (lines.attackers[0].owner ?? 8) : 8,
    step: 0, defDown: 0, atkDown: 0,
    defFell: [], atkFell: [],
    phase: "cloud", at: now(),
    // on a human's turn 67cc:1836 holds the cloud while WAR.8SN plays
    cloudTime: (G.g.side && !G.g.side.computer) ? sound.effect("war") : 0,
    victor: victorName(lines.attackers),
    message: outcomeLines(result, lines.city, fled),
  };
}

/** Close the battle window and let the fight take effect (67cc:0a6b). */
G.closeAssault = () => {
  const a = G.assault;
  G.assault = null;
  if (!a) return;
  game.applyAttack(G.g, a.result);
  stratDirty();
  if (a.after) a.after();
};

/** Carry the playback on by the clock. */
function advanceAssault() {
  const a = G.assault;
  const t = now();
  if (a.phase === "cloud") {
    if (t - a.at >= Math.max(AS.cloudTime, a.cloudTime)) {
      // a computer's battle nobody is to see: the cloud was all of it
      if (a.computer && !a.window) { G.closeAssault(); return; }
      a.phase = "battle"; a.at = t;
      // 67cc:124a waits five ticks on the drawn-up lines before the fight
      if (a.computer) a.at = t + 5 / 18.2;
    }
    return;
  }
  if (a.phase === "over" && a.computer) {
    if (!a.closeAt) {
      a.closeAt = t + 15 / 18.2;
      if (G.aiStatus) G.aiStatus.text = a.outcome;
    } else if (t >= a.closeAt) {
      G.closeAssault();
    }
    return;
  }
  if (a.phase !== "battle") return;
  const each = a.fast ? AS.fellFast : AS.fellTime;
  const log = a.result.log || [];
  while (a.step < log.length && t >= a.at) {
    a.step++;
    const fell = { at: a.at, burn: a.fast ? AS.blastFast : AS.blastTime };
    if (log[a.step - 1] === 1) {
      a.atkFell[a.atkDown++] = fell;
    } else {
      a.defFell[a.defDown++] = fell;
    }
    // 7dda:0181: a blow for each, until Space hurries the fight on
    if (!a.fast) sound.effect(log[a.step - 1] === 1 ? "army" : "army2");
    a.at += each;
  }
  if (a.step >= log.length && t >= a.at) a.phase = "over";
}

/** A key or a click: run the playback through, and when it is through, close
 *  the window and ask what is to be done with the city. */
function pressAssault() {
  const a = G.assault;
  if (!a) return;
  if (a.phase !== "over") {
    a.fast = true;
    a.at = Math.min(a.at, now() + AS.fellFast);
    advanceAssault();
    return;
  }
  G.closeAssault();
  const city = a.result.captured;
  if (city && !a.computer) presentVictory(city, a.victor);
}

/** The cloud over the tile, drawn with the map, in map pixels. */
function drawCloud() {
  const a = G.assault;
  const sx = a.x * TILE + Math.floor((TILE - AS.cloudW) / 2);
  const sy = a.y * TILE + Math.floor((TILE - AS.cloudH) / 2);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.warPic, gfx.newQuad(0, 0, AS.cloudW, AS.cloudH), sx, sy);
}

/** One line of armies, struck off from the front as they fall. */
function drawBattleLine(armies, slots, side, down, fell) {
  const sheet = G.armyImg[side] || G.armyImg[8];
  const quads = G.armyQuads[side] || G.armyQuads[8];
  const t = now();
  armies.forEach((army, i) => {
    const at = slots[i];
    const gone = i < down && fell[i] && t >= fell[i].at + fell[i].burn;
    if (at && !gone) {
      gfx.setColor(1, 1, 1);
      if (army.atSea) gfx.draw(G.warPic, gfx.newQuad(AS.sea[0], AS.sea[1], AS.sea[2], AS.sea[3]), at.x, at.y + AS.seaDrop);
      gfx.draw(sheet, quads[army.type % 32], at.x, at.y);
      if (i < down) gfx.draw(G.atransShields, G.blastQuad, at.x, at.y);
    }
  });
}

function drawBattle() {
  const a = G.assault, R = AS.window;
  popupFrame(R);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.marble, gfx.newQuad(0, 0, R.w, R.h), R.x, R.y);
  const shield = (side, at) => {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.shieldImg, gfx.newQuad((side ?? 8) * AS.shieldW, 0, AS.shieldW, AS.shieldH), at[0], at[1]);
  };
  shield(a.defSide, AS.shieldDef);
  shield(a.atkSide, AS.shieldAtk);
  drawBattleLine(a.def, a.defSlots, a.defSide, a.defDown, a.defFell);
  drawBattleLine(a.atk, a.atkSlots, a.atkSide, a.atkDown, a.atkFell);
  if (a.phase === "over") {
    let y = AS.textY;
    gfx.setColor(1, 1, 1);
    for (const line of a.message) {
      kit.centred(G.bigFont, line, 320, y);       // 6a35:04c5, on x = 320
      y += AS.textStep;
    }
  }
}

function drawAssault() {
  advanceAssault();
  if (!G.assault) return;
  if (G.assault.phase !== "cloud") drawBattle();
}

// Dialog 11 behind popup 7 -- exactly VICTORY.PCK -- with four buttons.
// Pillage needs a production type to strip and sack two (63fa:0000).
function presentVictory(city, victor) {
  const ui = G.screen.ui;
  if (!G.victoryView) G.victoryView = screen.dialog(G.screen, AS.vDialog);
  G.victory = {
    city,
    who: fmt(uidata.text(ui, AS.vWho, G.g.rng.dice(1, 4, -1)), victor),
    where: fmt(uidata.text(ui, AS.vWhere, G.g.rng.dice(1, 3, -1)), city.name),
  };
  const v = G.victoryView;
  v.state[AS.occupy] = uidata.NORMAL;
  v.state[AS.raze] = uidata.NORMAL;
  v.state[AS.pillage] = city.slots.length >= 1 ? uidata.NORMAL : uidata.DISABLED;
  v.state[AS.sack] = city.slots.length >= 2 ? uidata.NORMAL : uidata.DISABLED;
}

function drawVictory(v) {
  const R = AS.vPopup, ui = G.screen.ui;
  popupFrame(R);
  gfx.setColor(1, 1, 1);
  gfx.draw(G.victoryPic, R.x, R.y);
  const centred = (f, text, y) => kit.centred(f, text, 320, y);
  centred(G.titleFont, uidata.text(ui, AS.vText, 0), AS.vTitleY);
  centred(G.bigFont, v.who, AS.vLineY[0]);
  centred(G.bigFont, v.where, AS.vLineY[1]);
  centred(G.bigFont, uidata.text(ui, AS.vText, 1), AS.vLineY[2]);
  centred(G.bigFont, uidata.text(ui, AS.vText, 2), AS.vLineY[3]);
  screen.drawDialogControls(G.screen, G.victoryView);
}

/** What the player chose to do with the city they have just taken (63fa). */
function takeCity(what) {
  const v = G.victory;
  if (!v) return;
  const c = v.city, stack = G.selection ? G.selection.stack : [];
  G.victory = null;
  const done = () => {
    G.victoryUnder = null;
    stratDirty();
    refreshControls();
  };
  if (what === AS.pillage || what === AS.sack) {
    const sacked = what === AS.sack;
    const [gold, lost] = (sacked ? game.sack : game.pillage)(G.g, G.player, c, stack);
    G.victoryUnder = v;
    spoils.open(sacked, c, gold, lost, c.slots.length, done);
  } else if (what === AS.raze) {
    G.victoryUnder = v;
    searchUi.say(fmt("%s is in ruins!", c.name), () => {     // 4125:0a56
      game.raze(G.g, G.player, c, stack);
      done();
    });
  } else {
    // quest_check(4): a quest done is its reward instead of the city
    const news = game.occupy(G.g, G.player, c, stack);
    const production = () => cityUi.open(c, cityUi.PRODUCTION);
    if (news) {
      G.player.questNews = null;
      questnews.show(news, news.failed ? production : null);
    } else {
      production();
    }
  }
}

/** Draw in the popups' 640x480 frame, centred on the screen. */
function inDialogFrame(draw) {
  const d = G.layout.dialog;
  gfx.push();
  gfx.translate(d.x, d.y);
  draw();
  gfx.pop();
}

/** Draw one of the fixed groups of the original screen where it now is. */
function inGroup(group, draw) {
  const o = G.layout.offset[group];
  gfx.push();
  gfx.translate(o.x, o.y);
  draw();
  gfx.pop();
}

function drawModals() {
  for (const d of G.modals) d.draw();
}

/** The samples queue and the advisor blinks by the clock. */
function update() {
  sound.update();
  const top = kit.top();
  if (top && top.update) top.update();
}

/** One frame: the chrome in UI pixels, the map in map pixels at its own
 *  zoom, then the dialogs and the pointer over both. */
function draw() {
  gfx.begin();
  gfx.clear(0, 0, 0);
  syncLayout();
  stepComputer();
  pushUI();
  if (G.starting) {
    drawModals();
    drawMenuBar();
    drawPointer();
    gfx.pop();
    return;
  }
  advanceWalk();
  // a quest that ended is told once the screen is the player's again
  if (G.player && G.player.questNews && !(G.over || G.banner || G.offer || G.assault || G.victory
      || G.victoryUnder || G.walk || G.moveAll || G.aiRun || G.openMenu != null || kit.top())) {
    questnews.poll(G);
  }
  refreshControls();
  screen.drawBackground(G.screen, G.layout);

  const r = G.mapRect;
  gfx.setScissor(r.x, r.y, r.w, r.h);
  const [cx, cy] = camDev();
  pushMap(r.x, r.y, cx, cy, G.zoom);
  drawMap();
  if (G.assault && G.assault.phase === "cloud") drawCloud();
  gfx.pop();
  gfx.setScissor();

  drawStrategic();
  // 8065:0a9d refills the control panel with marble before the controls go on
  inGroup("panel", () => {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.marble, gfx.newQuad(0, 0, 224, 114), 400, 355);
  });
  screen.drawControls(G.screen);
  drawShortcutIcons();
  inGroup("bar", drawBottomBar);
  inDialogFrame(() => {
    if (G.victoryUnder) drawVictory(G.victoryUnder);
    drawModals();
  });
  drawMenuBar();
  // the turn opens with the banner over the offer, and is dismissed first
  inDialogFrame(() => {
    if (G.offer) drawHeroOffer();
    if (G.assault) drawAssault();
    if (G.victory) drawVictory(G.victory);
    if (G.banner) drawBanner();
  });
  drawPointer();
  gfx.pop();
}

// ------------------------------------------------------------------ input

// A menu item dispatches on its accelerator, so a menu pick and a key press
// reach the same place, as in the original (docs/re/ui.md > Commands).
let MENU_DOES = null;     // filled in below

function menuPick(k) {
  const act = MENU_DOES[k];
  if (act) act(); else say("%s is not implemented yet.", k);
}

// 1a8b:04c8 reads no input while a stack walks, and Move All walks one stack
// after another. A click meanwhile is lost; a key waits in the keyboard's
// buffer, fifteen of them, and is read once it is all over.
G.busyWalking = () => ((G.walk != null && !G.walk.computer) || G.moveAll != null) && !kit.top();

G.keyBuffer = [];
G.flushKeys = () => {
  const ks = G.keyBuffer;
  G.keyBuffer = [];
  for (const k of ks) keypressed(k);
};

/** While the computer plays: a key hurries a battle, or runs the rest of the
 *  round through unwatched. True when it took the input. */
function computerInput(k) {
  if (!(G.aiRun && !kit.top())) return false;
  if (G.assault && G.assault.computer) { pressAssault(); return true; }
  // Shift and Alt are held to reach Settings, not pressed to skip
  if (k && (/shift$/.test(k) || /alt$/.test(k))) return true;
  G.aiSkip = true;
  if (G.walk && G.walk.computer) { G.walk = null; resumeComputer(); }
  return true;
}

function mousepressed(x, y, button) {
  if (G.busyWalking()) return;
  syncLayout();
  const fx = x - G.layout.dialog.x, fy = y - G.layout.dialog.y;
  if (computerInput(null)) return;
  // 7ecb:0142: the banner eats the click that dismisses it
  if (G.banner) { dismissBanner(); return; }
  if (G.assault) { pressAssault(); return; }
  // the right button is not a click on a dialog's button (18a9:012c)
  if ((G.victory || G.offer) && button === 2) return;
  if (G.victory) {
    const c = screen.dialogControlAt(G.victoryView, fx, fy);
    if (c && G.victoryView.state[c.id] !== uidata.DISABLED) takeCity(c.id);
    return;
  }
  if (G.offer) {
    const c = screen.dialogControlAt(G.heroView, fx, fy);
    if (!c) return;
    if (c.id === HERO.MALE) G.offerFemale = false;
    else if (c.id === HERO.FEMALE) G.offerFemale = true;
    else if (c.id === HERO.OK) acceptOffer();
    else if (c.id === HERO.CANCEL && !G.offer.first) refuseOffer();
    return;
  }

  // on the start screens the menu bar stays live over them (7f77:0200)
  let top = kit.top();
  if (top && top.menuBar && (G.openMenu != null || menuMod.titleAt(G.menuLayout, x, y, menuMod.BAR_H) != null)) top = null;
  if (top) {
    // the right button on a dialog: a control's help, or the dialog's regions
    if (button === 2 && top.view) {
      const c = infobox.controlAt(top.view, fx, fy, top.hidden);
      if (!(c && infobox.control(x, y, c.id, top)) && top.rightpressed) top.rightpressed(fx, fy, x, y);
      return;
    }
    if (top.mousepressed) top.mousepressed(fx, fy, button);
    return;
  }
  if (G.over) return;

  // the menu bar takes precedence over everything beneath it
  const hit = menuMod.titleAt(G.menuLayout, x, y, menuMod.BAR_H);
  if (hit != null) {
    G.openMenu = G.openMenu === hit ? null : hit;
    return;
  }
  if (G.openMenu != null) {
    const row = menuMod.rowAt(G.menuLayout, G.openMenu, x, y);
    G.openMenu = null;
    const k = row ? (row.key || row.act) : null;
    if (k && G.menuEnabled(k)) menuPick(k);
    return;
  }

  const c = screen.controlAt(G.screen, x, y);
  if (c && button === 2) {
    if (infobox.control(x, y, c.id)) return;
  } else if (c) {
    if (G.screen.state[c.id] === uidata.DISABLED) return;
    G.pressed = c.id;
    G.screen.state[c.id] = uidata.ACTIVE;
    return;
  }

  // the hand drags the map (740d:00e4 -> 8065:0e04)
  if (button === 1 && pointerKind(x, y) === PTR.HAND) {
    G.drag = { cx: G.cx, cy: G.cy, dx: 0, dy: 0 };
    return;
  }

  const r = screen.regionAt(G.screen, x, y);
  if (!r) return;
  if (r.id === screen.REGION.MAP) {
    const tile = tileAtPoint(x, y);
    if (!tile) return;
    const [tx, ty] = tile;
    if (button === 2) { tileinfo.open(tx, ty); return; }
    // the left button does what the pointer shows (740d:00ce)
    const k = pointerKind(x, y);
    if (k === PTR.WALK || k === PTR.BOAT) moveSelection(tx, ty);
    else if (k === PTR.ATTACK || k === PTR.PEACE) moveSelection(tx, ty, true);
    else if (k === PTR.CITY || k === PTR.SITE) {
      const city = game.cityAt(G.g, tx, ty);
      if (city) openCity(city);
    } else if (k === PTR.ADVISE) {
      miladvisor.open(G.selection ? G.selection.stack : null, tx, ty);
    } else if (k === PTR.SELECT) {
      select(tx, ty);
    } else if (k === PTR.ALT) {
      planRoute(tx, ty);
      refreshControls();
    }
  } else if (r.id === screen.REGION.STRATEGIC) {
    const tx = Math.floor((x - r.x) / 2), ty = Math.floor((y - r.y) / 2);
    if (button !== 1) return;
    if (held("lalt", "ralt")) { planRoute(tx, ty); refreshControls(); } else centreOn(tx, ty);
  } else if (r.id === screen.REGION.BOTTOMBAR && button === 2) {
    barInfo(x, y);
  } else if (r.id === screen.REGION.MAPPANEL && button === 2) {
    infobox.lines(x, y, "- Drag Screen -", "Move mouse to drag the screen");
  }
}

// A control's id, minus 100, indexes the 397-entry jump table at 17be:0b0c:
// that is what turns a click into an action (docs/re/ui.md > Commands).
const ACTION = {};

function afterSlotChange() {
  syncSelection();
  say("%d of %d, %d movement", G.selection.stack.length, G.selection.slots.n, slotsMod.moves(G.selection.slots));
}
G.afterSlotChange = afterSlotChange;

for (let i = 0; i < SLOT_COUNT; i++) {
  ACTION[SLOT_FIRST + i] = () => {
    if (!G.selection) return;
    slotsMod.toggle(G.selection.slots, G.g, i);
    afterSlotChange();
  };
  ACTION[BAR_FIRST + i] = () => {
    if (!G.selection) return;
    slotsMod.pickGroup(G.selection.slots, G.g, i);
    afterSlotChange();
  };
}
ACTION[GRP_ALL] = () => {
  if (!G.selection) return;
  const s = G.selection.slots;
  if (slotsMod.grouped(s)) slotsMod.single(s, G.g); else slotsMod.all(s, G.g);
  afterSlotChange();
};
ACTION[GRP_NONE] = ACTION[GRP_ALL];

// The five buttons above the pad, 173-178: walk on, next, done, defend,
// deselect (8065:0ec9 / 0ed7 / 0ef4 / 0f26).
ACTION[173] = () => {
  const sel = G.selection;
  if (!sel || sel.stack.length === 0) { say("Nothing is selected."); return; }
  const target = sel.stack[0].target;
  if (!target) { say("They have nowhere to be."); return; }
  moveSelection(target.x, target.y);
};
ACTION[174] = () => selectNext();
ACTION[175] = () => quitArmy();
ACTION[176] = () => fortify();
ACTION[178] = () => deselect();

// The 3x3 pad, 320-327 (8611:0723), clockwise from north: it moves the view.
const PAD_STEP = [[0, -1], [1, -1], [1, 0], [1, 1], [0, 1], [-1, 1], [-1, 0], [-1, -1]];
for (let i = 0; i < 8; i++) {
  const step = PAD_STEP[i];
  ACTION[320 + i] = () => {
    G.cx += step[0]; G.cy += step[1];
    clampCamera();
  };
}

/** Order > Move All (1c8c:04c4): every army of the side with a destination
 *  walks there in turn, each walk played out before the next. */
G.moveAllStep = () => {
  const m = G.moveAll;
  while (m) {
    if (G.walk) return;
    const lead = G.g.armies.find((a) => a.owner === G.player.index && a.target && !a.transit && !m.seen.has(a));
    if (!lead) {
      G.moveAll = null;
      if (m.moved === 0) say("Nothing is under orders.");
      refreshControls();
      return;
    }
    m.seen.add(lead);
    select(lead.x, lead.y, lead);
    const sel = G.selection;
    if (sel) {
      for (const a of sel.stack) m.seen.add(a);
      moveSelection(lead.target.x, lead.target.y);
      m.moved++;
    }
    if (G.moveAll !== m) return;
  }
};

function moveAll() {
  G.moveAll = { seen: new Set(), moved: 0 };
  const sel = G.selection;
  const t = sel && sel.stack[0] ? sel.stack[0].target : null;
  if (t) {
    for (const a of sel.stack) G.moveAll.seen.add(a);
    moveSelection(t.x, t.y);
    G.moveAll.moved = 1;
  }
  G.moveAllStep();
}

/** Hero > Search (6536:0000) with the selected stack. */
function search() {
  if (!G.selection) return;
  const x = G.selection.x, y = G.selection.y;
  searchUi.open(G.selection.stack);
  reslot(selectableAt(x, y));
  stratDirty();
}

// The key of each menu item UDB.DAT lets a button carry (545c:00aa).
const SHORTCUT_KEY = {
  507: "m", 508: "q", 510: "a", 511: "k", 512: "g", 513: "n",
  514: "w", 516: "=", 517: ",", 518: "f", 521: "z", 524: "b",
  525: "c", 526: "p", 527: "v", 528: ".", 529: "s", 530: "h",
  531: "e", 534: "l", 535: "alt E",
};

const shortcutKey = (n) => SHORTCUT_KEY[G.screen.ui.shortcuts[n] ?? -1];

for (let i = 0; i < uidata.SHORTCUT_COUNT; i++) {
  ACTION[uidata.SHORTCUT_FIRST + i] = () => {
    const k = shortcutKey(i);
    if (k && G.menuEnabled(k)) MENU_DOES[k]();
  };
}

// the "?" (8065:104e): the mouse's help page, then the keys'
ACTION[188] = () => {
  helpUi.open("HELP\\HMOUSE.GFX", () => helpUi.open("HELP\\HKEYS.GFX", null, helpUi.POPUP4), helpUi.POPUP4);
};

// 186 (8065:0f3f) looks at where the stack is going, and back
ACTION[186] = () => {
  const sel = G.selection;
  if (!sel || sel.stack.length === 0) return;
  const t = sel.stack[0].target;
  const cx = G.cx, cy = G.cy;
  if (t && inView(sel.x, sel.y)) {
    centreOn(t.x, t.y);
    if (G.cx === cx && G.cy === cy) centreOn(sel.x, sel.y);
  } else {
    centreOn(sel.x, sel.y);
  }
};
// 187 (8065:0fe9) forgets the destination
ACTION[187] = () => {
  const sel = G.selection;
  if (!sel || !sel.stack[0] || !sel.stack[0].target) return;
  for (const a of sel.stack) a.target = undefined;
  G.route = null;
};

// 183-185 are one button in three faces: Diplomatic Action (484e:0346)
for (let id = 183; id <= 185; id++) ACTION[id] = () => diplomacyUi.action();

// the pad's centre, 177, shares its handler with Home (8065:0f02)
ACTION[177] = () => {
  if (G.selection) centreOn(G.selection.x, G.selection.y);
  else centreOn(G.player.capital.x, G.player.capital.y);
};

/** Which buttons are live (8065:0174, the original's own refresh). A
 *  control with no handler here is greyed too. */
function refreshControls() {
  const st = G.screen.state;
  const sel = G.selection;
  // under the banner and the hero offer every control is still greyed
  if (G.banner || G.offer) {
    for (const id in st) st[id] = uidata.DISABLED;
    return;
  }
  const set = (id, live) => {
    if (st[id] === undefined) return;
    if (!(live && ACTION[id])) st[id] = uidata.DISABLED;
    else if (id === G.pressed) st[id] = uidata.ACTIVE;
    else st[id] = uidata.NORMAL;
  };
  const walkOn = sel != null && G.route != null && G.route.path.length > (G.walk ? G.walk.i + 1 : 0);
  set(173, walkOn);
  const more = G.g.armies.some((a) => a.owner === G.player.index && !a.transit && !a.fortified && !a.done);
  set(174, more);
  set(175, more && sel != null);
  set(176, sel != null);
  set(177, sel != null);
  set(178, sel != null);
  set(186, sel != null);
  set(187, sel != null && sel.stack[0] != null && sel.stack[0].target != null);
  set(188, true);
  for (let i = 0; i < SLOT_COUNT; i++) {
    const live = sel != null && i < sel.slots.n;
    set(SLOT_FIRST + i, live);
    set(BAR_FIRST + i, live);
  }
  const grouped = sel && slotsMod.grouped(sel.slots);
  set(240, sel != null && !grouped);
  set(241, sel != null && !!grouped);
  for (let i = 0; i < uidata.SHORTCUT_COUNT; i++) {
    const k = shortcutKey(i);
    set(uidata.SHORTCUT_FIRST + i, k != null && G.menuEnabled(k));
  }
  for (let i = 0; i < 8; i++) set(320 + i, true);
  const face = diplomacyUi.buttonFor(G.g, G.player.index);
  for (let id = 183; id <= 185; id++) set(id, id === face);
}

/** The drag: the map goes the way the mouse does, by exactly as far. */
function mousemoved(dx, dy) {
  const d = G.drag;
  if (!d) return;
  d.dx += dx; d.dy += dy;
  const k = display.scale / (TILE * G.zoom);
  G.cx = d.cx - d.dx * k;
  G.cy = d.cy - d.dy * k;
  clampCamera();
}

/** The wheel zooms the map about the tile under the pointer. */
function wheelmoved(wy) {
  syncLayout();
  if (G.starting || G.over || kit.top() || G.banner || G.offer || G.victory || wy === 0) return;
  let mx = G.mouse.x, my = G.mouse.y;
  if (!inRect(G.mapRect, mx, my)) { mx = null; my = null; }
  G.setZoom(G.zoom + (wy > 0 ? 1 : -1), mx, my);
}

function mousereleased(x, y, button) {
  syncLayout();
  if (G.drag) {
    G.drag = null;
    if (!G.pressed) return;
  }
  const top = kit.top();
  if (top && top.mousereleased) {
    top.mousereleased(x - G.layout.dialog.x, y - G.layout.dialog.y, button);
    return;
  }
  const id = G.pressed;
  if (id == null) return;
  G.pressed = null;
  G.screen.state[id] = uidata.NORMAL;
  const c = screen.controlAt(G.screen, x, y);
  if (!c || c.id !== id) return;          // released off the button
  const act = ACTION[id];
  if (act) act();
  refreshControls();
}

/** The city nearest the view's centre, then the city dialog in that mode. */
function viewCity(mode) {
  const [cx, cy] = G.viewCentre();
  let best = null, bestD = null;
  for (const c of G.g.map.cities) {
    const ok = game.seen(G.g, G.player, c.x, c.y) && (mode === cityUi.INFO || c.ownerIndex === G.player.index);
    if (ok) {
      const d = Math.max(Math.abs(c.x - cx), Math.abs(c.y - cy));
      if (bestD === null || d < bestD) { best = c; bestD = d; }
    }
  }
  if (best) cityUi.open(best, mode);
}

// Each menu accelerator, as far as this engine can honour it.
MENU_DOES = {
  "alt E": () => endTurn(),
  "m": () => moveAll(),
  // Game > Quit (7721:0000) and New game (7721:019d) ask first
  "^Q": () => {
    const lines = [1, 2, 3, 4].map((i) => uidata.text(G.screen.ui, 0x2c, i));
    inputUi.open({ title: uidata.text(G.screen.ui, 0x2c, 0), lines, confirm: true, ok: () => {
      // 7721:0072: "Farewell, Warlord!" -- and back to the start screens
      advisorUi.say(cues.QUIT, () => G.quit());
    } });
  },
  "alt N": () => {
    const lines = [1, 2, 3, 4].map((i) => uidata.text(G.screen.ui, 0x2d, i));
    inputUi.open({ title: uidata.text(G.screen.ui, 0x2d, 0), lines, confirm: true, ok: () => {
      G.selection = null; G.banner = null; G.offer = null;
      openStart();
    } });
  },
  "alt S": () => savegame.save(),
  "alt L": () => {
    savegame.load((g) => {
      if (G.starting) {
        G.modals.length = 0;
        G.starting = false;
      }
      takeLoaded(g);
    });
  },
  "z": () => search(),
  "c": () => viewCity(cityUi.INFO),
  "b": () => viewCity(cityUi.CITY),
  "p": () => viewCity(cityUi.PRODUCTION),
  "v": () => viewCity(cityUi.VECTOR),
  // Hero > Plant Flag (7563:09f7)
  "f": () => {
    if (!G.selection) return;
    for (const a of G.selection.stack) {
      if (a.type === armytype.HERO && game.plantFlag(G.g, G.player, a)) { stratDirty(); return; }
    }
  },
  // Order > Disband (1b62:06bf)
  "q": () => {
    if (!G.selection || G.selection.stack.length === 0) return;
    const stack = G.selection.stack;
    const heroes = stack.some((a) => a.type === armytype.HERO);
    inputUi.open({
      title: "Disband", confirm: true,                       // 4125:2bf8
      lines: ["Are you sure you", "want to disband this", "group?", heroes ? "It contains heroes" : ""],
      ok: () => {
        game.disband(G.g, G.player, stack);
        G.selection = null;
        stratDirty();
      },
    });
  },
  "i": () => fightorder.open(),
  "r": () => resignUi.open(),
  ".": () => { const [cx, cy] = G.viewCentre(); ruinUi.open(ruinUi.nearest(cx, cy)); },
  "s": () => stackUi.open(),
  "o": () => armybonus.open(),
  "t": () => itemsUi.open(),
  "x": () => { if (G.selection) signpost.open(G.selection.stack); },
  "h": () => historyUi.open(0),
  "e": () => historyUi.open(1),
  "j": () => historyUi.open(2),
  "y": () => historyUi.open(3),
  "alt U": () => shortcutsUi.open(),
  "alt X": () => settingsUi.open(),
  "?": () => aboutUi.open(),
  "l": () => historyUi.triumphs(),
  "d": () => diplomacyUi.open(),
  "=": () => questUi.open(),
  "u": () => levelsUi.open(),
  ",": () => heroInfo.open(),
  "a": () => reportsUi.open(0),
  "k": () => reportsUi.open(1),
  "g": () => reportsUi.open(2),
  "n": () => reportsUi.open(3),
  "w": () => reportsUi.open(4),
};
for (let n = 1; n <= 16; n++) {
  MENU_DOES["map zoom " + n] = () => G.setZoom(n);
  MENU_DOES["ui scale " + n] = () => G.setUIScale(n);
}
MENU_DOES["screen full"] = () => G.setScreen("full");
MENU_DOES["screen window"] = () => G.setScreen("window");
MENU_DOES["screen 4:3"] = () => G.setOriginalSize(display.wanted == null);
for (const synth of sound.SYNTHS) MENU_DOES["music " + synth] = () => sound.setSynth(synth);

// What greys a menu item (8065:0519, the table at 4125:1798).
const L = {
  sideHasHero: () => G.g.armies.some((a) => a.owner === G.player.index && a.type === armytype.HERO),
  canSearch: () => {
    const lead = G.selection && G.selection.stack[0];
    if (!lead) return false;
    const m = G.g.map;
    const s = m.siteAt ? m.siteAt[lead.y * m.width + lead.x] : null;
    if (!s || s.searched) return false;
    return lead.type === armytype.HERO || s.type === siteMod.TEMPLE;
  },
  canPlantFlag: () => {
    const lead = G.selection && G.selection.stack[0];
    if (!lead || lead.type !== armytype.HERO) return false;
    const std = G.g.map.items[G.player.index];
    if (!(lead.items || []).includes(std)) return false;
    const t = scn.terrainAt(G.g.map, lead.x, lead.y);
    if (t === move.WATER || t === move.SHORE || t === move.CITY || t === move.SITE) return false;
    return !G.g.map.items.some((it) => it.planted && it.x === lead.x && it.y === lead.y);
  },
  sideHasCities: () => G.g.map.cities.some((c) => c.ownerIndex === G.player.index && !c.razed),
  selected: () => G.selection != null && G.selection.stack[0] != null,
  notFirstTurn: () => G.g.turn !== 1,
};
const MENU_LIVE = {
  "alt L": () => savegame.used() > 0,
  "q": L.selected,
  "x": () => L.selected() && scn.terrainAt(G.g.map, G.selection.stack[0].x, G.selection.stack[0].y) === move.TOWER,
  "r": L.sideHasCities,
  "d": () => G.g.map.options.diplomacy !== 0,
  ",": L.sideHasHero,
  "u": L.sideHasHero,
  "f": L.canPlantFlag,
  "z": L.canSearch,
  "b": L.sideHasCities,
  "p": L.sideHasCities,
  "v": L.sideHasCities,
  "s": L.selected,
  "h": L.notFirstTurn, "e": L.notFirstTurn, "j": L.notFirstTurn, "y": L.notFirstTurn,
  "alt E": () => !G.g.won,
};

G.menuEnabled = (k) => {
  if (k == null || MENU_DOES[k] == null) return false;
  const m = /^music (\w+)$/.exec(k);
  if (m && !sound.synthAvailable(m[1])) return false;
  // 7f77:0200: on the start screens every menu is greyed but Quit and Load
  if (G.starting) {
    return k === "^Q" || (k === "alt L" && savegame.used() > 0)
      || k.startsWith("ui scale ") || k.startsWith("screen ") || m != null;
  }
  const live = MENU_LIVE[k];
  return live == null || live();
};

function keypressed(k) {
  if (G.busyWalking()) {
    if (G.keyBuffer.length < 15) G.keyBuffer.push(k);
    return;
  }
  syncLayout();
  if (computerInput(k)) return;
  if (G.banner) { dismissBanner(); return; }
  if (G.assault) { pressAssault(); return; }
  if (G.victory) {
    // Occupy heads both the default and the cancel lists
    if (k === "return" || k === "kpenter" || k === "escape") takeCity(AS.occupy);
    return;
  }
  if (G.offer) {
    if (k === "return" || k === "kpenter") acceptOffer();
    else if (k === "backspace") G.offerName = G.offerName.slice(0, -1);
    else if (k === "escape" && !G.offer.first) refuseOffer();
    return;
  }

  const top = kit.top();
  if (top) {
    if (top.menuBar) {
      let accel = null;
      if (held("lctrl", "rctrl") && k === "q") accel = "^Q";
      else if (held("lalt", "ralt") && k.length === 1) accel = "alt " + k.toUpperCase();
      if (accel) {
        G.openMenu = null;
        if (G.menuEnabled(accel)) MENU_DOES[accel]();
        return;
      }
    }
    if (G.openMenu != null) { G.openMenu = null; return; }
    if (top.keypressed) top.keypressed(k);
    return;
  }

  if (G.openMenu != null) { G.openMenu = null; return; }
  if (G.over) return;

  // the main screen's keys (17be:0064, 17be:0444)
  const alt = held("lalt", "ralt"), ctrl = held("lctrl", "rctrl"), shift = held("lshift", "rshift");
  const press = (id) => {
    if (G.screen.state[id] !== uidata.DISABLED && ACTION[id]) ACTION[id]();
    refreshControls();
  };
  const dm = /^kp(\d)$/.exec(k) || /^(\d)$/.exec(k);
  const digit = dm ? dm[1] : null;

  if (k === "return" || k === "kpenter") press(174);
  else if (k === "kp+" || k === "pageup") G.setZoom(G.zoom + 1);
  else if (k === "kp-" || k === "pagedown") G.setZoom(G.zoom - 1);
  else if (k === "escape") press(175);
  else if (k === "space") press(240);           // group the stack (89e0:0a55)
  else if (k === "tab") press(186);
  else if (k === "backspace") press(187);
  else if (k === "home") press(177);
  else if (k === "end") deselect();             // 1b62:08b3, as 178 is
  else if (k === "delete") press(173);
  else if (digit === "5") {
    if (G.selection) centreOn(G.selection.x, G.selection.y);
  } else if (digit) {
    // 1-9 step the stack the way the numeric pad lies (1c8c:0197)
    const STEP = { 8: [0, -1], 9: [1, -1], 6: [1, 0], 3: [1, 1], 2: [0, 1], 1: [-1, 1], 4: [-1, 0], 7: [-1, -1] };
    const st = STEP[digit];
    if (st && G.selection) {
      for (const a of G.selection.stack) a.target = undefined;
      moveSelection(G.selection.x + st[0], G.selection.y + st[1]);
    }
  } else if (k === "up") press(320);
  else if (k === "right") press(322);
  else if (k === "down") press(324);
  else if (k === "left") press(326);
  else if (k === "f5") {
    // not the original's: quick save and load
    if (savegame.writeSlot("w2:quick", G.g)) say("Saved."); else say("Could not save.");
  } else if (k === "f9") {
    loadQuick();
  } else {
    let accel = null;
    if (ctrl && k === "q") accel = "^Q";
    else if (alt && k.length === 1) accel = "alt " + k.toUpperCase();
    else if (shift && k === "/") accel = "?";
    else if (k.length === 1) accel = k;
    if (accel && MENU_DOES[accel]) MENU_DOES[accel]();
  }
}

/** Typing into the hero's name field, or a dialog's. */
function textinput(text) {
  const top = kit.top();
  if (top && !G.offer) {
    if (top.textinput) top.textinput(text);
    return;
  }
  if (!G.offer) return;
  if (G.offerName.length < HERO.NAME_MAX && /^[A-Za-z0-9\s'\-.]+$/.test(text)) G.offerName += text;
}

// ------------------------------------------------------------------ the page

function frame() {
  fitCanvas();
  measure();
  try {
    update();
    draw();
  } catch (e) {
    console.error(e);
    showError(e);
  }
  requestAnimationFrame(frame);
}

let errorShown = false;
function showError(e) {
  if (errorShown) return;
  errorShown = true;
  const el = document.getElementById("error");
  if (el) {
    el.textContent = "Something went wrong: " + (e && e.stack ? e.stack : e);
    el.style.display = "block";
  }
}

function wire(canvas) {
  const pos = (e) => toUI(e.clientX, e.clientY);
  canvas.addEventListener("contextmenu", (e) => e.preventDefault());
  let last = null;
  canvas.addEventListener("pointerdown", (e) => {
    sound.unlock();
    canvas.setPointerCapture(e.pointerId);
    const [x, y] = pos(e);
    G.mouse = { x, y };
    last = [e.clientX, e.clientY];
    try { mousepressed(x, y, e.button === 2 ? 2 : (e.button === 1 ? 3 : 1)); } catch (err) { console.error(err); showError(err); }
    e.preventDefault();
  });
  canvas.addEventListener("pointerup", (e) => {
    const [x, y] = pos(e);
    G.mouse = { x, y };
    try { mousereleased(x, y, e.button === 2 ? 2 : (e.button === 1 ? 3 : 1)); } catch (err) { console.error(err); showError(err); }
    e.preventDefault();
  });
  canvas.addEventListener("pointermove", (e) => {
    const [x, y] = pos(e);
    G.mouse = { x, y };
    display.mouseInside = onFrame(e.clientX, e.clientY);
    if (last) {
      const k = display.dpi / display.scale;
      mousemoved((e.clientX - last[0]) * k, (e.clientY - last[1]) * k);
    }
    last = [e.clientX, e.clientY];
  });
  canvas.addEventListener("pointerleave", () => { display.mouseInside = false; });
  canvas.addEventListener("wheel", (e) => {
    e.preventDefault();
    if (e.deltaY !== 0) wheelmoved(e.deltaY < 0 ? 1 : -1);
  }, { passive: false });
  window.addEventListener("keydown", (e) => {
    sound.unlock();
    const k = keys.loveName(e);
    keys.press(k);
    // the browser's own uses of these keys are not wanted over the game
    if (k === "tab" || k === "backspace" || k === "space" || k === "f5" || k === "f9" || k === "/"
        || k.startsWith("kp") || k === "up" || k === "down" || k === "left" || k === "right"
        || ((e.altKey || e.ctrlKey) && k.length === 1 && k !== "c" && k !== "v")) {
      e.preventDefault();
    }
    try {
      keypressed(k);
      if (e.key && e.key.length === 1 && !e.ctrlKey && !e.altKey && !e.metaKey) textinput(e.key);
    } catch (err) { console.error(err); showError(err); }
  });
  window.addEventListener("keyup", (e) => keys.release(keys.loveName(e)));
  window.addEventListener("blur", () => keys.clear());
  document.addEventListener("fullscreenchange", () => {
    display.windowed = !document.fullscreenElement;
    G.layout = null;
  });
}

async function boot() {
  const canvas = document.getElementById("screen");
  display.canvas = canvas;
  gfx.setContext(canvas.getContext("2d", { alpha: false }));
  fitCanvas();
  const status = document.getElementById("loading");
  const ctx = canvas.getContext("2d");
  const bar = (done, total) => {
    ctx.setTransform(1, 0, 0, 1, 0, 0);
    ctx.fillStyle = "#000";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    const w = Math.min(400 * display.dpi, canvas.width * 0.6);
    const x = (canvas.width - w) / 2, y = canvas.height / 2;
    ctx.strokeStyle = "#888";
    ctx.strokeRect(x, y, w, 12 * display.dpi);
    ctx.fillStyle = "#ccc";
    ctx.fillRect(x, y, w * done / Math.max(1, total), 12 * display.dpi);
  };
  try {
    await vfs.boot("assets/", bar);
    const params = new URLSearchParams(location.search);
    G.screenFiles = null;
    const ui = uidata.load(DATA);
    await sound.init(DATA, ui.files);
    if (status) status.style.display = "none";
    load(params);
  } catch (e) {
    console.error(e);
    showError(e);
    return;
  }
  wire(canvas);
  requestAnimationFrame(frame);
}

boot();
