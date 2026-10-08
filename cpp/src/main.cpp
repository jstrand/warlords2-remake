// Warlords II, in the original's own interface, on SDL2.
//
// A port of the Lua remake's front end (love2d/main.lua). The screen is
// 640x480 and every rect in it comes from the game's own layout files:
// DATA/JOIN.DAT names the dialog's controls and clickable regions, AREA.DAT
// places the regions, BUTTON.DAT the controls, and FILE.DAT says which .PCK
// each control is cut from. docs/re/ui.md and docs/formats/screens.md.
//
//     warlords2 [--data DIR] [--scenario NAME] [--seed N] [--window]
//               [--script FILE]
//
// Run from the repository root, or point --data at your copy of the game.
#include <SDL.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <map>
#include <random>
#include <regex>
#include <set>
#include <sstream>

#include "front/app.hpp"
#include "front/font.hpp"
#include "front/layout.hpp"
#include "front/menu.hpp"
#include "front/pckimage.hpp"
#include "front/prefs.hpp"
#include "front/screen.hpp"
#include "platform/coroutine.hpp"
#include "platform/display.hpp"
#include "platform/gfx.hpp"
#include "platform/keys.hpp"
#include "platform/sound.hpp"
#include "ui/dialogs.hpp"
#include "ui/kit.hpp"
#include "util/json.hpp"
#include "util/util.hpp"
#include "warlords/ai.hpp"
#include "warlords/armytype.hpp"
#include "warlords/cues.hpp"
#include "warlords/diplomacy.hpp"
#include "warlords/game.hpp"
#include "warlords/hero.hpp"
#include "warlords/history.hpp"
#include "warlords/move.hpp"
#include "warlords/pal.hpp"
#include "warlords/rules.hpp"
#include "warlords/randommap.hpp"
#include "warlords/save.hpp"
#include "warlords/scn.hpp"
#include "warlords/site.hpp"
#include "warlords/slots.hpp"
#include "warlords/uidata.hpp"

App G;

using w2::Army;
using w2::City;
using w2::NONE;
using w2::Side;
using w2::format;
using w2::uidata::ACTIVE;
using w2::uidata::DISABLED;
using w2::uidata::NORMAL;
namespace move = w2::move;
namespace game = w2::game;
namespace slotsMod = w2::slots;

static const int TILE = screen::TILE;
static const double CURS_RATE = 18.2 / 4;          // CURS.PCK frames a second (177b:011f)

static std::mt19937 looseDice((unsigned)time(nullptr));

namespace front {
int roll(int n) { return n <= 0 ? 1 : (int)(looseDice() % (unsigned)n) + 1; }
void say(const std::string& s) { G.status = s; }
}  // namespace front

using front::say;

// ------------------------------------------------------------------ forwards

static void beginGame();
static void showBanner(Side* side);
static void finishRound(Side* side);
static void playComputers(Side* side);
static void refreshControls();
static void moveSelection(int x, int y, bool attack = false);
static void startAssault(int x, int y, const w2::Battle& result);
static void presentVictory(City* city, const std::string& victor);
static void keypressed(const std::string& k);
static void select(int x, int y, Army* pick = nullptr);
static void refreshRoute();
static void syncSelection();
static bool menuEnabled(const std::string& k);
static void menuPick(const std::string& k);
static void resumeComputer();
static std::map<std::string, std::function<void()>>& menuDoes();

// ------------------------------------------------------------------ helpers

static gfx::Quad tileQuad(int t) {
  int i = t % 96;
  return gfx::newQuad((i % 16) * TILE, (i / 16) * TILE, TILE, TILE);
}
static gfx::Quad roadQuad(int i) { return gfx::newQuad((i % 13) * 48, (i / 13) * TILE, TILE, TILE); }
static void palColour(int i) { kit::setPal(i); }
static bool held(std::initializer_list<const char*> k) { return keys::isDown(k); }
static int ownerOr8(int o) { return o == NONE ? 8 : o; }

// ------------------------------------------------------------------ loading

static void loadArt() {
  G.palette = w2::pal::load(G.dataDir + "/TERRAIN0/WAR2.PAL");
  const auto& P = G.palette;
  auto img = [&](const std::string& path, int key = pckimage::NOKEY) { return pckimage::load(G.dataDir + path, P, key); };
  for (int i = 0; i < 2; i++) G.sheets[i] = img("/TERRAIN0/SCENERY" + std::to_string(i) + ".PCK");
  // The army sheets stand on 10, bright green -- all but the fifth side's,
  // keyed on its own ground, the colour of its corner.
  G.roadImg = img("/TERRAIN0/ROAD.PCK", 1);
  for (int i = 0; i <= 8; i++) G.armyImg[i] = img("/TERRAIN0/A" + std::to_string(i) + ".PCK", pckimage::CORNER);
  // the shadow sheet keys on white: it is two colours of ghost on white
  G.shadowImg = img("/TERRAIN0/ASHADOW.PCK", 15);
  G.marble = img("/PICS/MARBLE.PCK");
  G.bigArmy = img("/PICS/BIGARMY.PCK");
  G.cityPic = img("/PICS/CITY.PCK");
  G.warPic = img("/PICS/WAR.PCK", 1);
  G.shieldImg = img("/TERRAIN0/BSHIELD.PCK", 12);
  G.victoryPic = img("/PICS/VICTORY.PCK");
  G.heroPicM = img("/PICS/MHERO.PCK");
  G.heroPicF = img("/PICS/FHERO.PCK");
  // ATRANS2.PCK: map markers on a mask colour, 1 (1997:027e, 8cc6:0952)
  G.atransShields = img("/TERRAIN0/ATRANS2.PCK", 1);
  // HIDDEN.PCK: the hidden map's edges; colour 1 is where the map shows through
  G.fogImg = img("/PICS/HIDDEN.PCK", 1);
  // CURS.PCK: the marching box round the selected stack (177b:01a1); keyed
  // on 0, its 1 shows white and its 4 black
  G.cursImg = pckimage::fromPixels(*w2::pck::decode(G.dataDir + "/PICS/CURS.PCK"), P, {{1, 15}, {4, 0}}, {0});
  // the rings, drawn opaque as 1997:0129 blits every bitmap it is given
  G.abits = img("/PICS/ABITS.PCK");
  // STAND.PCK: the twelve mouse pointers, drawn by the game itself (22bf)
  G.pointerImg = img("/STAND.PCK", 10);
  G.screen = screen::load(G.dataDir, P, w2::uidata::MAIN_SCREEN, 0);
  // the player's own button shortcuts (ui/shortcuts)
  try {
    std::string sc = prefs::get("shortcuts");
    if (!sc.empty()) {
      auto j = w2::Json::parse(sc);
      for (size_t i = 0; i < j.size(); i++) G.screen->ui.shortcuts[(int)i] = (int)j[i].integer();
    }
  } catch (...) {
  }
  G.font = Font::load(G.dataDir, "TEXT", P, 3);
  G.bigFont = Font::load(G.dataDir, "CHANCE17", P, 3);
  G.titleFont = Font::load(G.dataDir, "CHANCE36", P, 3);
  G.shieldsImg = img("/TERRAIN0/SHIELDS.PCK");
  G.cityBack = img("/TERRAIN0/CITYBACK.PCK");
  G.searchPic = img("/PICS/SEARCH.PCK");
  G.templePic = img("/PICS/TEMPLE.PCK");
  G.scrollPic = img("/PICS/SCROLL.PCK", 10);
}

// ------------------------------------------------------------------ camera

// The view is as many tiles as the map's rect holds at the map's zoom, and
// slides smoothly: G.cx, G.cy is the tile at its top-left corner.

static std::pair<double, double> viewSize() {
  double k = (double)display::d.scale / (TILE * G.zoom);
  return {G.mapRect.w * k, G.mapRect.h * k};
}

// The camera in device pixels, rounded to one.
static std::pair<int, int> camDev() {
  double z = TILE * G.zoom;
  return {(int)std::floor(G.cx * z + 0.5), (int)std::floor(G.cy * z + 0.5)};
}

static std::pair<double, double> uiToMap(double x, double y) {
  const auto& r = G.mapRect;
  double s = display::d.scale, z = TILE * G.zoom;
  auto [cx, cy] = camDev();
  return {(cx + (x - r.x) * s) / z, (cy + (y - r.y) * s) / z};
}

std::pair<double, double> front::mapToUI(double tx, double ty) {
  const auto& r = G.mapRect;
  double s = display::d.scale, z = TILE * G.zoom;
  auto [cx, cy] = camDev();
  return {r.x + (tx * z - cx) / s, r.y + (ty * z - cy) / s};
}

std::pair<int, int> front::viewCentre() {
  auto [vw, vh] = viewSize();
  return {(int)std::floor(G.cx + vw / 2), (int)std::floor(G.cy + vh / 2)};
}

static std::array<int, 4> viewTiles() {
  auto [vw, vh] = viewSize();
  int mw = G.g->map->width, mh = G.g->map->height;
  return {std::max(0, (int)std::floor(G.cx)), std::max(0, (int)std::floor(G.cy)),
          std::min(mw - 1, (int)std::ceil(G.cx + vw) - 1), std::min(mh - 1, (int)std::ceil(G.cy + vh) - 1)};
}

static bool shown(int x, int y) { return x >= G.view[0] && y >= G.view[1] && x <= G.view[2] && y <= G.view[3]; }

static bool inView(int x, int y) {
  auto [vw, vh] = viewSize();
  return x + 0.5 > G.cx && x + 0.5 < G.cx + vw && y + 0.5 > G.cy && y + 0.5 < G.cy + vh;
}

// Keep the view on the map; a map smaller than the view sits in its middle.
static void clampCamera() {
  if (!G.g) return;
  auto [vw, vh] = viewSize();
  int mw = G.g->map->width, mh = G.g->map->height;
  G.cx = vw >= mw ? (mw - vw) / 2 : std::max(0.0, std::min(G.cx, mw - vw));
  G.cy = vh >= mh ? (mh - vh) / 2 : std::max(0.0, std::min(G.cy, mh - vh));
}

void front::centreOn(int x, int y) {
  auto [vw, vh] = viewSize();
  G.cx = x + 0.5 - vw / 2;
  G.cy = y + 0.5 - vh / 2;
  clampCamera();
}
using front::centreOn;

// The zoom the map may go to: up to two steps past the UI's biggest.
static int maxZoom() { return display::d.maxScale + 2; }

// Zoom the map to `z`, keeping the map under the UI point (x, y) put.
static void setZoomAt(int z, double x, double y, bool centre) {
  z = std::max(1, std::min(maxZoom(), z));
  if (z == G.zoom) return;
  prefs::set("zoom", z);
  const auto& r = G.mapRect;
  if (centre) { x = r.x + r.w / 2.0; y = r.y + r.h / 2.0; }
  auto [tx, ty] = uiToMap(x, y);
  G.zoom = z;
  double k = (double)display::d.scale / (TILE * z);
  G.cx = tx - (x - r.x) * k;
  G.cy = ty - (y - r.y) * k;
  clampCamera();
}
void front::setZoom(int z) { setZoomAt(z, 0, 0, true); }

static void applyLayout(const layout::Layout& L) {
  std::optional<std::pair<double, double>> mid;
  if (G.g && G.haveLayout) {
    auto [vw, vh] = viewSize();
    mid = std::make_pair(G.cx + vw / 2, G.cy + vh / 2);
  }
  G.layout = L;
  G.haveLayout = true;
  screen::relayout(*G.screen, L);
  auto reg = [](int id) {
    const auto* r = screen::region(*G.screen, id);
    return r ? layout::Rect{r->x, r->y, r->w, r->h, r->id} : layout::Rect{};
  };
  G.mapRect = reg(screen::MAP);
  G.stratRect = reg(screen::STRATEGIC);
  G.barRect = reg(screen::BOTTOMBAR);
  G.menuRect = reg(screen::MENUBAR);
  G.panelRect = reg(screen::MAPPANEL);
  G.menuLayout = menu::layout(*G.font, L.w, menu::withZooms(display::d.maxScale, maxZoom()));
  G.zoom = std::max(1, std::min(maxZoom(), G.zoom ? G.zoom : display::d.scale));
  if (mid) {
    auto [vw, vh] = viewSize();
    G.cx = mid->first - vw / 2;
    G.cy = mid->second - vh / 2;
    clampCamera();
  }
}

// The start screens are the original's 640x480, centred; the game has the
// whole screen. Run each frame, and each input.
void front::syncLayout() {
  int w = layout::W, h = layout::H;
  if (!G.starting) std::tie(w, h) = display::uiSize();
  display::setFrame(std::max(w, layout::W), std::max(h, layout::H));
  if (!G.haveLayout || G.layout.w != display::d.w || G.layout.h != display::d.h) {
    applyLayout(layout::compute(display::d.w, display::d.h));
  }
}
using front::syncLayout;

// Change how the screen is used, keeping the view centred where it was.
static void relayout(const std::function<void()>& change) {
  std::optional<std::pair<double, double>> mid;
  if (G.g && !G.starting && G.haveLayout) {
    auto [vw, vh] = viewSize();
    mid = std::make_pair(G.cx + vw / 2, G.cy + vh / 2);
  }
  change();
  G.haveLayout = false;
  syncLayout();
  if (mid) {
    auto [vw, vh] = viewSize();
    G.cx = mid->first - vw / 2;
    G.cy = mid->second - vh / 2;
    clampCamera();
  }
}

static void setUIScale(int n) {
  relayout([n]() { display::choose(n); });
  prefs::set("scale", display::d.chosen);
}

static void setOriginalSize(bool on) {
  relayout([on]() {
    if (on) display::d.wanted = std::make_pair(layout::W, layout::H);
    else display::d.wanted.reset();
  });
  prefs::set("4:3", on ? "on" : "off");
}

static void setScreen(bool window) {
  relayout([window]() { display::setWindowed(window); });
  prefs::set("screen", window ? "window" : "full");
}

static bool menuTicked(const std::string& k) {
  return k == "map zoom " + std::to_string(G.zoom) || k == "ui scale " + std::to_string(display::d.scale) ||
         k == std::string("screen ") + (display::d.windowed ? "window" : "full") ||
         (k == "screen 4:3" && display::d.wanted) || k == "music " + sound::synth();
}

// ------------------------------------------------------------------ walking

// A stack walks one tile at a time, and the map shows where it is going. The
// destination is kept in the army record itself, so it outlives a walk that
// could not finish. 8611:2ef5 marks the route: a ring on every tile but the
// last, plain while the stack can still reach it this turn and crossed once
// it cannot, and a ghost of the leading army on the last (docs/re/ui.md).
static const int WALK_RING[2] = {496, 31}, WALK_CROSSED[2] = {496, 48};
static const int WALK_RING_W = 16, WALK_RING_H = 14;
static const int WALK_RING_AT[2] = {16, 13}, WALK_GHOST_AT[2] = {4, 4};
static const double WALK_STEP = 0.1;                       // one tile of the walk

static void refreshRoute() {
  G.route.reset();
  if (!G.selection || G.selection->stack.empty()) return;
  auto& sel = *G.selection;
  auto target = sel.stack[0]->target;
  if (!target) return;
  if (sel.stack[0]->x == target->first && sel.stack[0]->y == target->second) {
    for (Army* a : sel.stack) a->target.reset();
    return;
  }
  G.route = move::preview(*G.g, sel.stack, target->first, target->second);
}

static void orderTo(const std::vector<Army*>& stack, int x, int y) {
  for (Army* a : stack) {
    if (x == NONE) a->target.reset();
    else a->target = std::make_pair(x, y);
  }
}

// Alt and a click (1c8c:0007): the route is drawn but not walked.
static void planRoute(int x, int y) {
  if (!G.selection || G.selection->stack.empty()) return;
  auto& sel = *G.selection;
  if (!game::seen(*G.g, G.player->index, x, y)) return;
  if (sel.stack[0]->x == x && sel.stack[0]->y == y) {
    orderTo(sel.stack, NONE, NONE);
    G.route.reset();
    return;
  }
  orderTo(sel.stack, x, y);
  refreshRoute();
  if (!G.route) sound::effect("chord");
}

// Play the walk back a tile at a time, centring on the stack as it goes.
static void startWalk(const std::vector<Army*>& armies, const std::vector<std::pair<int, int>>& tiles) {
  if (tiles.empty()) return;
  Walk w;
  w.armies = std::set<Army*>(armies.begin(), armies.end());
  w.tiles = tiles;
  w.i = 0;
  w.at = w2::now();
  w.top = armies.empty() ? nullptr : armies[0];
  G.walk = w;
  centreOn(tiles[0].first, tiles[0].second);
}

static void moveAllStep();
static void flushKeys();

static void advanceWalk() {
  if (!G.walk) return;
  Walk& w = *G.walk;
  double t = w2::now();
  while (w.i < w.tiles.size() - 1 && t - w.at >= WALK_STEP) {
    w.i++;
    w.at += WALK_STEP;
    centreOn(w.tiles[w.i].first, w.tiles[w.i].second);
  }
  if (w.i >= w.tiles.size() - 1 && t - w.at >= WALK_STEP) {
    bool computer = w.computer;
    G.walk.reset();
    refreshRoute();
    if (computer && G.aiRun) resumeComputer();
    if (G.moveAll) moveAllStep();
    if (!G.walk && !G.moveAll) flushKeys();
  }
}

static const std::pair<int, int>* walkingAt() { return G.walk ? &G.walk->tiles[G.walk->i] : nullptr; }

static void drawRoute() {
  if (!G.route) return;
  const auto& route = *G.route;
  Army* ghost = G.selection && !G.selection->stack.empty() ? G.selection->stack[0] : nullptr;
  // while the walk plays out the route is drawn from where it has got to
  size_t first = G.walk ? G.walk->i + 1 : 1;
  for (size_t i = first; i <= route.path.size(); i++) {
    const auto& step = route.path[i - 1];
    if (!shown(step.x, step.y)) continue;
    int sx = step.x * TILE, sy = step.y * TILE;
    gfx::setColor(1, 1, 1);
    if (i == route.path.size()) {
      if (ghost) gfx::draw(G.shadowImg, kit::armyQuad(ghost->type), sx + WALK_GHOST_AT[0], sy + WALK_GHOST_AT[1]);
    } else {
      const int* src = (int)i <= route.reach ? WALK_RING : WALK_CROSSED;
      gfx::draw(G.shadowImg, gfx::newQuad(src[0], src[1], WALK_RING_W, WALK_RING_H), sx + WALK_RING_AT[0], sy + WALK_RING_AT[1]);
    }
  }
}

// Screen point to map tile, or none outside the viewport.
static std::optional<std::pair<int, int>> tileAtPoint(int sx, int sy) {
  const auto& r = G.mapRect;
  if (sx < r.x || sy < r.y || sx >= r.x + r.w || sy >= r.y + r.h) return std::nullopt;
  auto [tx, ty] = uiToMap(sx, sy);
  return std::make_pair((int)std::floor(tx), (int)std::floor(ty));
}

// ------------------------------------------------------------------ selection

static std::vector<Army*> selectableAt(int x, int y) {
  std::vector<Army*> out;
  for (Army* a : game::armiesAt(*G.g, x, y))
    if (a->owner == G.player->index && (int)out.size() < w2::rules::MAX_STACK) out.push_back(a);
  return out;
}

// Pick up whatever the slots now say moves, and remember the grouping.
static void syncSelection() {
  if (!G.selection) return;
  auto& sel = *G.selection;
  slotsMod::commit(sel.slots, *G.g);
  sel.stack = slotsMod::selected(sel.slots);
  // being in the group that moves puts an army back in the cycle and marks
  // it offered for this pass (89e0:000a, 8c07:06eb)
  for (Army* a : sel.stack) { a->fortified = false; a->offered = true; }
}

// Put the slots back over a new list of armies, keeping the group.
static void reslot(const std::vector<Army*>& armies) {
  if (!G.selection) return;
  auto s = armies.empty() ? std::nullopt : slotsMod::keep(G.selection->slots, *G.g, armies);
  if (!s) { G.selection.reset(); return; }
  G.selection->slots = *s;
  G.selection->x = s->army[0]->x;
  G.selection->y = s->army[0]->y;
  syncSelection();
}

static void select(int x, int y, Army* pick) {
  auto stack = selectableAt(x, y);
  if (stack.empty()) {
    G.selection.reset();
    if (City* city = game::cityAt(*G.g, x, y)) front::openCity(city);
    return;
  }
  // a click selects one army, not the stack (docs/re/ui.md > The army slots)
  auto chosen = slotsMod::clicked(*G.g, stack, G.player->index, pick);
  Selection sel;
  sel.x = x;
  sel.y = y;
  sel.slots = slotsMod::build(*G.g, stack, G.player->index, &chosen);
  G.selection = sel;
  syncSelection();
  refreshRoute();
  // the tutorial on picking a stack up (1b62:051d-062a)
  std::vector<std::string> moments = {"move"};
  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      City* c = game::cityAt(*G.g, x + dx, y + dy);
      if (c && c->ownerIndex != G.player->index && !c->razed && moments.size() == 1) moments.push_back("fight");
    }
  }
  Army* lead = G.selection->stack.empty() ? nullptr : G.selection->stack[0];
  if (lead && lead->hero() && !G.g->map->siteAt.empty() && G.g->map->siteAt[y * G.g->map->width + x]) moments.push_back("search");
  tutorial::chain(moments);
}
void front::selectAt(int x, int y) { select(x, y); }

// ------------------------------------------------------------- the army cycle

// The five buttons above the pad walk you through your armies one stack at
// a time (8c07:040e). The next army is the nearest of the eligible ones to
// where the cycle last stopped; one exactly there counts as far away.

static void cycleReset() {
  G.cursor = G.player->capital ? std::make_pair(G.player->capital->x, G.player->capital->y) : std::make_pair(0, 0);
}

static int cycleRank(const Army* a) {
  int d = std::abs(a->x - G.cursor->first) + std::abs(a->y - G.cursor->second);
  return d == 0 ? 9000 : d;
}

static Army* nextArmy() {
  if (!G.cursor) cycleReset();
  Army* fresh = nullptr;
  Army* used = nullptr;
  for (Army* a : G.g->armies) {
    if (a->owner == G.player->index && !a->transit && !a->fortified && !a->done) {
      if (a->offered) {
        if (!used || cycleRank(a) < cycleRank(used)) used = a;
      } else if (!fresh || cycleRank(a) < cycleRank(fresh)) {
        fresh = a;
      }
    }
  }
  if (!fresh) {
    if (!used) return nullptr;
    for (Army* a : G.g->armies) if (a->owner == G.player->index) a->offered = false;
    fresh = used;
  }
  G.cursor = std::make_pair(fresh->x, fresh->y);
  return fresh;
}

static void selectNext() {
  Army* a = nextArmy();
  if (!a) {
    G.selection.reset();
    G.route.reset();
    say("Every army has moved.");
    sound::effect("chord");                     // 8c07:019b: nothing left
    return;
  }
  sound::effect("ding");
  select(a->x, a->y, a);
  centreOn(a->x, a->y);
}

// Out of the cycle for the rest of this turn (control 175, 8c07:0393).
static void quitArmy() {
  if (!G.selection) { selectNext(); return; }
  for (Army* a : G.selection->stack) a->done = true;
  G.route.reset();
  selectNext();
}

// Dig in: out of the cycle until picked up again (control 176, 8c07:03a6).
static void fortify() {
  if (!G.selection) { selectNext(); return; }
  for (Army* a : G.selection->stack) { a->fortified = true; a->target.reset(); }
  G.route.reset();
  selectNext();
}

// Put the selection down (control 178, 8065:0f26).
static void deselect() {
  G.selection.reset();
  G.route.reset();
  G.walk.reset();
}

// ------------------------------------------------------------------ the turn

static void afterBattle(const w2::Battle& result) {
  tutorial::show("fresult");
  if (G.selection) {
    std::vector<Army*> alive;
    for (int i = 0; i < G.selection->slots.n; i++) {
      Army* a = G.selection->slots.army[i];
      if (!result.deadByArmy.count(a)) alive.push_back(a);
    }
    reslot(alive);
  }
}

// Walk the selection towards (x, y). Only `attack` turns running into an
// enemy into an assault (740d:0179, 1c8c:041f); a walk just stops.
static void moveSelection(int x, int y, bool attack) {
  if (!G.selection) return;
  auto& sel = *G.selection;
  orderTo(sel.stack, x, y);
  G.route = move::preview(*G.g, sel.stack, x, y);
  auto r = move::moveTo(*G.g, sel.stack, x, y);
  if (r.stopped == "attack" && !(attack && r.steps == 0)) r.stopped = "blocked";
  if (r.stopped == "attack") {
    // decided first, shown, and only then taken effect (67cc:0a6b)
    w2::Battle result = game::decideAttack(*G.g, sel.stack, r.attack->x, r.attack->y);
    startAssault(r.attack->x, r.attack->y, result);
    G.assault->after = [result]() { afterBattle(result); };
  } else if (r.stopped == "no route") {
    say("There is no way there.");
    sound::effect("chord");                     // 1a8b:0c4f, 1c8c:0007
  } else if (r.steps == 0) {
    say("They cannot move: " + r.stopped + ".");
  } else {
    auto walked = sel.stack;
    sel.x = sel.stack[0]->x;
    sel.y = sel.stack[0]->y;
    reslot(selectableAt(sel.x, sel.y));
    front::stratDirty();
    startWalk(walked, r.walked);
  }
  if (G.selection && !G.selection->stack.empty()) {
    G.selection->x = G.selection->stack[0]->x;
    G.selection->y = G.selection->stack[0]->y;
    if (!G.walk) centreOn(G.selection->x, G.selection->y);
  }
  if (!G.walk) refreshRoute();
}

// ------------------------------------------------------------------ the pointer

// 18a9:0896 decides the mouse pointer from what is under it and what is
// selected, and a click on the map does whatever that pointer promises
// (740d:0037). STAND.PCK holds the twelve, with hotspots at 4125:045c.
enum Ptr { ARROW, VIEW, BOAT, CITY_P, HAND, SELECT, WALK, SITE_P, ATTACK, ADVISE, PEACE, ALT };
static const int PTR_HOT[12] = {0, 6, 8, 6, 8, 8, 8, 8, 0, 8, 8, 0};

static int pointerKind(int x, int y) {
  if (G.starting || G.over || G.banner || G.offer || G.assault || G.victory || G.aiRun || G.openMenu >= 0 || kit::top() ||
      !G.player || G.player->computer) {
    return ARROW;
  }
  if (G.drag) return HAND;                       // 4125:1176, while it drags
  bool alt = held({"lalt", "ralt"}), ctrl = held({"lctrl", "rctrl"});
  if (G.stratRect.contains(x, y)) return alt ? ALT : VIEW;
  auto tile = tileAtPoint(x, y);
  if (!tile) return G.panelRect.contains(x, y) ? HAND : ARROW;
  auto [tx, ty] = *tile;
  w2::Game& g = *G.g;
  int me = G.player->index;
  if (tx < 0 || ty < 0 || tx >= g.map->width || ty >= g.map->height) return HAND;
  auto here = game::armiesAt(g, tx, ty);
  City* city = game::cityAt(g, tx, ty);
  bool armies = !here.empty();
  // the tile's owner nibble: whoever stands there, else the city's, else none
  int owner = armies ? here[0]->owner : NONE;
  if (owner == NONE) owner = city ? city->ownerIndex : NONE;
  if (owner == NONE) owner = w2::rules::NEUTRAL;
  int t = city ? (city->razed ? move::SITE : move::CITY) : w2::scn::terrainAt(*g.map, tx, ty);
  const std::vector<Army*>* sel = G.selection && !G.selection->stack.empty() ? &G.selection->stack : nullptr;
  int dist = 0, mode = NONE, under = move::PLAIN;
  bool atSea = false;
  if (sel) {
    dist = std::max(std::abs(tx - (*sel)[0]->x), std::abs(ty - (*sel)[0]->y));
    mode = move::modeOf(g, *sel);
    under = w2::scn::terrainAt(*g.map, (*sel)[0]->x, (*sel)[0]->y);
    atSea = (*sel)[0]->atSea;
  }
  int cost = (sel && mode != move::FLYING) ? move::COST[t] : 1;
  if (!game::seen(g, me, tx, ty) || cost <= 0) return HAND;
  auto walk = [&]() { return (mode != move::FLYING && (t == move::WATER || t == move::SHORE)) ? BOAT : WALK; };
  auto look = [&]() {
    if (t == move::CITY) return CITY_P;
    if (t == move::SITE) return SITE_P;
    return HAND;
  };
  static const std::set<int> PTR_OK = {move::ROAD, move::BRIDGE, move::WATER, move::SHORE, move::CITY};
  bool enemyNear = sel && dist <= 1 && owner != me && (t == move::CITY || armies);
  if (held({"lshift", "rshift"})) {
    if (enemyNear && g.map->options.militaryAdvisor != 0) return ADVISE;
    return look();
  }
  if (armies && owner == me && !ctrl) {
    if (alt) return ALT;
    if (!sel || dist < 1) return SELECT;
    return walk();
  }
  if (enemyNear && !(atSea && mode == move::LAND && under == move::SHORE && !PTR_OK.count(t)) &&
      !(!atSea && mode == move::LAND && t == move::SHORE && !PTR_OK.count(under))) {
    if (g.map->options.diplomacy == 0 || owner == w2::rules::NEUTRAL) return ATTACK;
    int st = w2::diplomacy::state(g, me, owner);
    if (t == move::CITY) return st == w2::diplomacy::WAR ? ATTACK : PEACE;
    return st != w2::diplomacy::PEACE ? ATTACK : PEACE;
  }
  if (sel && (owner == me || (owner == w2::rules::NEUTRAL && t != move::CITY))) {
    if (alt) return ALT;
    if (dist >= 1 && ctrl && armies) return SELECT;
    return walk();
  }
  return look();
}

static void drawPointer() {
  if (display::pointerAway()) return;
  int k = pointerKind(G.mouseX, G.mouseY);
  gfx::setColor(1, 1, 1);
  gfx::draw(G.pointerImg, gfx::newQuad(k * 16, 0, 16, 16), G.mouseX - PTR_HOT[k], G.mouseY - PTR_HOT[k]);
}

// The start-of-turn banner (8cc6:0259): popup 6, (160, 60) 320x312 --
// exactly CITY.PCK -- framed in the side's own colour, with the side's name
// and "Turn %d" over it. 7ecb:0142 then blocks until any input arrives.
static const kit::Rect BANNER{160, 60, 320, 312};
static const int BANNER_NAME_Y = 85, BANNER_TURN_Y = 130;

static void showBanner(Side* side) {
  G.banner = Banner{side->name, G.g->turn, side->colour, side->edge};
  sound::effect("turn");                          // the fanfare, TURN.8SN
}

// The hero offer is popup 2 -- (80, 60) 480x312, MARBLE.PCK cropped -- with
// dialog 12's controls laid on it, every number from auto_ui_hero_emerges
// (6563:0d5c).
static const kit::Rect HERO_POPUP{80, 60, 480, 312};
static const int HERO_DIALOG = 12;
static const int HERO_MAP_X = 80, HERO_MAP_Y = 60;
static const int HERO_PIC_X = 320, HERO_PIC_Y = 110, HERO_PIC_W = 224, HERO_PIC_H = 170;
static const int HERO_TITLE_Y = 63, HERO_CENTRE = 432;
static const int HERO_LINE_Y[4] = {190, 210, 230, 250};
static const int MALE_LABEL[2] = {376, 315}, FEMALE_LABEL[2] = {480, 315};
static const int HERO_OK = 287, HERO_CANCEL = 288, HERO_FIELD = 289, HERO_MALE = 290, HERO_FEMALE = 291;
static const int HERO_TITLE_GROUP = 0x5f, HERO_LINE_GROUP = 0x61, HERO_SEX_GROUP = 0x62;
static const size_t HERO_NAME_MAX = 19;

static bool presentOffer(Side* side);
static void afterOffer();

// Close the banner, and only then let the turn's first dialog through.
static void dismissBanner() {
  G.banner.reset();
  if (G.centreAfterBanner && G.player->capital) centreOn(G.player->capital->x, G.player->capital->y);
  G.centreAfterBanner = false;
  // the advisor has his say (6dda:026f(5)), then 8cc6:04bd puts on the
  // hero's music or the turn's
  int says = sound::speechOn() ? w2::cues::advisor(*G.g, *G.player, front::roll) : NONE;
  advisor::say(says, []() {
    sound::music(G.player->heroOffer ? w2::cues::HERO : w2::cues::PLAY);
    std::vector<std::string> moments;
    if (G.g->turn == 2) moments.push_back("turn2");
    if (G.player->heroOffer) moments.push_back("hero");
    tutorial::chain(moments, []() {
      if (!presentOffer(G.player)) afterOffer();
    });
  });
}

// The four lines over the picture, as 6563:0d5c assembles them.
static std::vector<std::string> heroLines(const w2::HeroOffer& offer, bool female) {
  int first = female ? 8 : 0;
  auto s = [first](int i) { return kit::text(HERO_LINE_GROUP, first + i); };
  if (offer.first) return {s(0), s(1), s(2), format(s(3), offer.city->name)};
  return {format(s(0), offer.city->name), format(s(1), offer.price), format(s(2), G.player->gold), s(3)};
}

static bool presentOffer(Side* side) {
  if (!side->heroOffer) return false;
  if (!G.heroView) G.heroView = screen::dialog(*G.screen, HERO_DIALOG);
  G.offer = side->heroOffer;
  // the name and sex were rolled with the offer; the checkboxes only change
  // the wording and the picture
  G.offerFemale = G.offer->female;
  G.offerName = G.offer->name.empty() ? "Hero" : G.offer->name;
  G.heroView->state[HERO_CANCEL] = G.offer->first ? DISABLED : NORMAL;
  G.heroView->state[HERO_OK] = NORMAL;
  return true;
}

// Take the offer: the hero joins under the name and sex now in the dialog.
static void acceptOffer() {
  auto offer = G.offer;
  offer->name = G.offerName;
  offer->female = G.offerFemale;
  auto [h, allies] = w2::hero::recruit(*G.g, *G.player, *offer);
  G.offer.reset();
  G.player->heroOffer.reset();
  centreOn(h->x, h->y);
  afterOffer();
}

// The turn's opening, once any hero offer is settled: on turn 1 the
// capital's dialog opens in Production.
static void afterOffer() {
  if (G.g->turn == 1 && G.player->capital && !G.player->computer) city::open(G.player->capital, city::PRODUCTION);
}

static void refuseOffer() {
  G.offer.reset();
  G.player->heroOffer.reset();
  afterOffer();
}

// ------------------------------------------------------------ the computer

// The computer's turns run on a coroutine (the Lua remake's), so the screen
// keeps being drawn while they are played: it gives a frame back after every
// side's turn, and stops after each walk worth showing. A turn is shown when
// its side is observed or no human is left (8cc6:0000); a walk only if the
// player has seen any of it. Holding Shift or Alt between turns opens
// Settings (5db9:045e); any other key or a click runs the rest of the round
// through unwatched.

static Coroutine* aiCo = nullptr;     // the turn being played, from inside it
static Side* aiResult = nullptr;      // where the round ended

static bool humansLeft() {
  for (Side* s : G.g->sides)
    if (s->alive && !s->computer && !game::sideCities(*G.g, *s).empty()) return true;
  return false;
}

static bool shownSide(const Side* side) { return side && (side->observe || !humansLeft()); }

// A line in the status bar, and the time 8065:11d1 then waits, in BIOS ticks
static void status(const std::string& text, int ticks) {
  if (G.aiStatus) G.aiStatus->text = text;
  if (ticks > 0 && !G.aiSkip && aiCo) {
    Yield y;
    y.what = "pause";
    y.ticks = ticks;
    aiCo->yield(y);
  }
}

// ai_turn's progress bar after each phase (5db9:0000)
static int progressOf(const std::string& phase) {
  static const std::map<std::string, int> P = {
      {"diplomacy", 10}, {"move hero", 15}, {"move search", 20}, {"move explore", 25}, {"assault", 30},
      {"move #1", 40}, {"rescue", 45}, {"evaluate", 50}, {"clean city", 55}, {"neutral", 60},
      {"move #2", 65}, {"assault XX", 70}, {"specials", 75}, {"rebuilding", 80}, {"last rescue", 85},
      {"production", 90}, {"vectoring", 95}};
  auto it = P.find(phase);
  return it == P.end() ? (G.aiStatus ? G.aiStatus->progress : 0) : it->second;
}

static void installHooks() {
  w2::ai::hooks.onWalk = [](w2::Game& g, const std::vector<Army*>& stack, const move::WalkResult& r) {
    if (!aiCo || G.aiSkip || !shownSide(G.aiSide)) return;
    bool seen = !humansLeft();
    for (auto& t : r.walked) {
      if (seen) break;
      if (game::seen(g, G.player->index, t.first, t.second)) seen = true;
    }
    if (seen) {
      Yield y;
      y.what = "walk";
      y.armies = stack;
      y.tiles = r.walked;
      aiCo->yield(y);
    }
  };
  // A computer's battle (auto_ui_being_attacked, 67cc:124a): who is attacked
  // goes up in the status bar, and then how it went, five ticks each -- when
  // the attacker's turn is watched or the defender is a human.
  w2::ai::hooks.onFight = [](w2::Game& g, const std::vector<Army*>&, int x, int y, w2::Battle& result) {
    if (!aiCo || !G.aiStatus) return;
    int def = NONE;
    if (result.lines.city) def = result.lines.city->ownerIndex;
    else if (!result.lines.defenders.empty()) def = result.lines.defenders[0]->owner;
    Side* defSide = def != NONE && def < 8 ? &g.map->sides[def] : nullptr;
    bool human = defSide && !defSide->computer;
    if (!(shownSide(G.aiSide) || human)) return;
    std::vector<Side*> humans;
    for (Side* s : g.sides) if (s->alive && !s->computer) humans.push_back(s);
    if (g.map->options.hiddenMap != 0 && humans.size() == 1 && !game::seen(g, humans[0]->index, x, y)) return;
    auto t = [](int grp) { return kit::text(grp, 0); };
    Army* heroA = nullptr;
    for (Army* a : result.lines.attackers) if (a->hero()) { heroA = a; break; }
    std::string me = G.aiSide->name;
    std::string outcome;
    if (!result.won) outcome = format(t(0x98), me);
    else if (heroA) outcome = format(t(0x8e), heroA->name);
    else outcome = format(t(0x96), me);
    bool hiddenMany = g.map->options.hiddenMap != 0 && humans.size() > 1;
    bool window = !hiddenMany && ((shownSide(G.aiSide) && defSide) || human);
    bool cloud = !hiddenMany;
    status(defSide ? format(t(0x94), defSide->name) : t(0x95), 5);
    if (cloud && !G.aiSkip) {
      startAssault(x, y, result);
      G.assault->computer = true;
      G.assault->window = window;
      G.assault->outcome = outcome;
      G.assault->message = {outcome};
      Yield yl;
      yl.what = "assault";
      aiCo->yield(yl);
      if (window) return;
    }
    status(outcome, 5);
  };
  w2::ai::hooks.onSpoils = nullptr;
}

static void computerTurns(Coroutine& co, Side* side) {
  aiCo = &co;
  auto yieldWhat = [&co](const char* what) {
    Yield y;
    y.what = what;
    co.yield(y);
  };
  // What the round's end found, as the original tells it inside the
  // computer's turns: each side put out in a box, or -- with no human
  // playing -- in the status bar for 100 ticks (8065:18ab), then the last
  // human's fall in two boxes (8065:1c6f). The rest waits for the turn that
  // follows (ui/ending.cpp).
  auto announce = [&co, &yieldWhat]() {
    w2::Ending& e = G.g->ending;
    if (e.told) return;
    e.told = true;
    auto fallen = e.fallen;
    bool noHumans = e.noHumans;
    for (auto& f : fallen) {
      std::string text = ending::fallenText(f);
      if (f.boxed) {
        Yield y;
        y.what = "fallen";
        y.text = text;
        co.yield(y);
      } else {
        status(text, 100);
      }
    }
    if (noHumans) yieldWhat("nohumans");
  };
  announce();                       // a human's turn may have ended the round
  while (side && side->computer) {
    G.aiSide = side;
    // 8065:2123: each computer turn opens with a song that plays once
    sound::music(w2::cues::COMPUTER, G.g->sides);
    G.aiStatus = AiStatus{side, side->name, 5};
    yieldWhat("progress");
    if (shownSide(side)) {
      auto news = side->diploNews;
      for (auto& m : news) status(m, 20);
    }
    side->diploNews.clear();
    w2::ai::playTurn(*G.g, *side, [&](const char* phase) {
      G.aiStatus->progress = progressOf(phase);
      yieldWhat("progress");
    });
    G.aiStatus->progress = 100;
    yieldWhat("progress");
    side = game::endTurn(*G.g);
    announce();
    yieldWhat("turn");
  }
  aiResult = side;
}

static void resumeComputer() {
  if (!G.aiRun) return;
  G.aiWait = false;
  bool more = G.aiRun->resume();
  if (!more) {
    G.aiRun.reset();
    aiCo = nullptr;
    G.aiSkip = false;
    G.aiSide = nullptr;
    finishRound(aiResult);
    return;
  }
  front::stratDirty();
  const Yield& y = G.aiRun->value();
  if (y.what == "pause") {
    G.aiResumeAt = w2::now() + y.ticks / 18.2;
    G.aiWait = true;
  } else if (y.what == "walk") {
    startWalk(y.armies, y.tiles);
    if (G.walk) G.walk->computer = true;
    else G.aiWait = true;
  } else if (y.what == "nohumans") {
    // 8065:1c6f: the last human is gone, and the war goes on
    auto t = [](int i) { return kit::text(0xd, i); };
    search::message(t(0), t(1), [t]() { search::message(t(2), t(3), []() { G.aiWait = true; }); });
  } else if (y.what == "fallen") {
    search::say(y.text, []() { G.aiWait = true; });          // 8065:10fb
  } else {
    // a turn done, or a battle up: go on at the next frame free of it
    G.aiWait = true;
  }
}

// Called every frame: carry the computer on once nothing is in its way.
static void stepComputer() {
  if (!G.aiRun || !G.aiWait || G.walk || G.assault || kit::top()) return;
  if (G.aiResumeAt > 0) {
    if (w2::now() < G.aiResumeAt && !G.aiSkip) return;
    G.aiResumeAt = 0;
  }
  // only a Shift or Alt pressed since the round began: the Alt of Alt-E
  // is still down as the computer starts
  bool mods = held({"lshift", "rshift"}) || held({"lalt", "ralt"});
  if (!mods) G.aiModsHeld = false;
  if (mods && !G.aiModsHeld) {
    G.aiModsHeld = true;
    G.aiSkip = false;
    settings::open();
    return;
  }
  // several steps a frame while running through unwatched
  double until = w2::now() + 0.03;
  do {
    resumeComputer();
  } while (G.aiSkip && G.aiRun && G.aiWait && !G.walk && !G.assault && !kit::top() && G.aiResumeAt == 0 &&
           w2::now() < until);
}

// Play computer sides from `side` on, until a human's turn or the end.
static void playComputers(Side* side) {
  G.aiModsHeld = held({"lshift", "rshift"}) || held({"lalt", "ralt"});
  aiResult = nullptr;
  G.aiRun = std::make_unique<Coroutine>([side](Coroutine& co) { computerTurns(co, side); });
  resumeComputer();
}
void front::playComputer() { playComputers(G.player); }

static void endTurn() {
  G.selection.reset();
  Side* side = game::endTurn(*G.g);
  if (side && side->computer) return playComputers(side);
  finishRound(side);
}

// The round is over: the next human side takes the keyboard.
static void finishRound(Side* side) {
  if (!side) {
    G.aiStatus.reset();
    G.over = true;
    ending::over(G.g->ending);
    return;
  }
  G.player = side;
  G.aiStatus.reset();                   // 8065:0aeb: the bar is the player's again
  front::stratDirty();
  cycleReset();
  // 8cc6:0259 centres on the side's capital -- but where the map is hidden
  // and more than one human plays, only once the banner is gone
  int humans = 0;
  for (auto& s : G.g->map->sides) if (s.inUse && s.alive && !s.computer) humans++;
  G.centreAfterBanner = G.g->map->options.hiddenMap != 0 && humans > 1;
  if (!G.centreAfterBanner && side->capital) centreOn(side->capital->x, side->capital->y);
  ending::show(G.g->ending, [side]() { showBanner(side); });
}

void front::openCity(City* c) { city::open(c); }
void front::openQuest() { questui::open(); }

void front::takeLoaded(std::unique_ptr<w2::Game> loaded) {
  G.g = std::move(loaded);
  G.selection.reset();
  G.route.reset();
  G.walk.reset();
  G.over = false;
  front::stratDirty();
  G.player = G.g->sides[G.g->current];
  centreOn(G.player->capital->x, G.player->capital->y);
  sound::music(w2::cues::PLAY);               // 7721:02d3
}

// ------------------------------------------------------------------ drawing

static Army* topArmy(const std::vector<Army*>& stack) {
  Army* best = nullptr;
  int bestRank = NONE;
  for (Army* a : stack) {
    int rank = G.g->map->fightOrder[ownerOr8(a->owner)][a->type];
    if (bestRank == NONE || rank > bestRank) { best = a; bestRank = rank; }
  }
  return best;
}

// The figure a stack shows (8611:2985): its top army's, or a boat -- the
// Navy, type 5 -- when an army in it is at sea.
static const int NAVY = 5;
static int stackFigure(const std::vector<Army*>& stack) {
  for (Army* a : stack) if (a->atSea) return NAVY;
  Army* a = topArmy(stack);
  return a ? a->type : NONE;
}

// The hidden map (8611:24c2): over each unseen tile, black through its
// HIDDEN.PCK cell; cell 14, a tile with nothing seen round it, is black.
static void drawFog() {
  if (G.g->map->options.hiddenMap == 0) return;
  for (int my = G.view[1]; my <= G.view[3]; my++) {
    for (int mx = G.view[0]; mx <= G.view[2]; mx++) {
      int cell = game::fogCell(*G.g, G.player->index, mx, my);
      if (cell == NONE) continue;
      int sx = mx * TILE, sy = my * TILE;
      if (cell == game::FOG_BLACK) {
        gfx::setColor(0, 0, 0);
        gfx::rectangle(gfx::FILL, sx, sy, TILE, TILE);
      } else {
        gfx::setColor(1, 1, 1);
        gfx::draw(G.fogImg, gfx::newQuad((cell % 7) * 48, (cell / 7) * 41, 40, 40), sx, sy);
      }
    }
  }
  gfx::setColor(1, 1, 1);
}

// A stack on the map (8611:0335): the top army's figure at (8, 7) in the
// tile; the flag pole, three 40-pixel lines in colours 14, 13 and 14; and
// the flag, from the side's own sheet at (464, y) 48 x 8, bigger the more
// armies stand there (4125:2d4e). More than four fly the smallest 8 lower
// as well, and the flag for the rest at the top.
static const int FLAG_Y[4] = {29, 38, 47, 56};
static void drawStack(int owner, int armyType, int count, int sx, int sy) {
  if (owner < 0 || owner > 8) owner = 8;
  gfx::setColor(1, 1, 1);
  gfx::draw(G.armyImg[owner], kit::armyQuad(armyType), sx + 8, sy + 7);
  palColour(14);
  gfx::rectangle(gfx::FILL, sx + 2, sy, 1, TILE);
  gfx::rectangle(gfx::FILL, sx + 4, sy, 1, TILE);
  palColour(13);
  gfx::rectangle(gfx::FILL, sx + 3, sy, 1, TILE);
  gfx::setColor(1, 1, 1);
  auto flag = [&](int k, int y) { gfx::draw(G.armyImg[owner], gfx::newQuad(464, FLAG_Y[k - 1], 48, 8), sx, y); };
  if (count > 4) { flag(1, sy + 8); count -= 4; }
  flag(std::max(1, std::min(count, 4)), sy);
}

// The map, in map pixels: the caller has the transform and the scissor up.
static void drawMap() {
  G.view = viewTiles();
  const auto& v = G.view;
  std::map<int, std::vector<Army*>> armiesOn;
  for (Army* a : G.g->armies) {
    if (!a->transit && a->x != NONE && a->x >= v[0] && a->x <= v[2] && a->y >= v[1] && a->y <= v[3])
      armiesOn[a->x + a->y * 1000].push_back(a);
  }
  // one item a tile: the last in item order (8611:2d7c walks them from the end)
  std::map<int, w2::Item*> itemsOnTile;
  for (int i = (int)G.g->map->items.size() - 1; i >= 0; i--) {
    w2::Item& it = G.g->map->items[i];
    if (it.status == 1 && it.x != NONE && !itemsOnTile.count(it.x + it.y * 1000)) itemsOnTile[it.x + it.y * 1000] = &it;
  }
  struct Standing {
    std::vector<Army*> stack;
    int x, y, sx, sy;
    bool hidden = false;
  };
  std::vector<Standing> standing;
  for (int my = v[1]; my <= v[3]; my++) {
    for (int mx = v[0]; mx <= v[2]; mx++) {
      int sx = mx * TILE, sy = my * TILE;
      int t = w2::scn::tileAt(*G.g->map, mx, my);
      int sheet = std::min(1, t / 96);
      gfx::setColor(1, 1, 1);
      gfx::draw(G.sheets[sheet], tileQuad(t), sx, sy);
      int rd = w2::scn::roadAt(*G.g->map, mx, my);
      if (rd != 0) gfx::draw(G.roadImg, roadQuad(rd - 1), sx, sy);
      // items on the ground (8611:2d7c): a planted standard as its side's
      // flag, army cell 29; anything else as the bag
      auto here = itemsOnTile.find(mx + my * 1000);
      if (here != itemsOnTile.end()) {
        w2::Item* it = here->second;
        if (it->planted && it->standardOf != NONE) gfx::draw(G.armyImg[it->standardOf], kit::armyQuad(29), sx, sy);
        else gfx::draw(G.atransShields, gfx::newQuad(64, 0, 32, 29), sx, sy);
      }
      auto on = armiesOn.find(mx + my * 1000);
      if (on != armiesOn.end()) {
        std::vector<Army*> stack;
        for (Army* a : on->second) if (!(G.walk && G.walk->armies.count(a))) stack.push_back(a);
        if (!stack.empty()) standing.push_back(Standing{stack, mx, my, sx, sy});
      }
    }
  }
  // the ground first, then the encampments, then the stacks (8611:1a79)
  for (auto& s : standing) {
    if (game::towerAt(*G.g, s.x, s.y)) {
      int owner = ownerOr8(s.stack[0]->owner);
      if (owner == 15) owner = 8;
      gfx::setColor(1, 1, 1);
      gfx::draw(G.roadImg, gfx::newQuad(owner * 48 + 192, 40, 48, 40), s.sx, s.sy);
      s.hidden = !(G.selection && G.selection->x == s.x && G.selection->y == s.y);
    }
  }
  // the selected stack's tile shows the lead of the group that moves
  Army* lead = G.selection && !G.selection->stack.empty() ? topArmy(G.selection->stack) : nullptr;
  if (lead && G.walk && G.walk->armies.count(lead)) lead = nullptr;
  for (auto& s : standing) {
    if (s.hidden) continue;
    Army* a = topArmy(s.stack);
    int figure = stackFigure(s.stack);
    if (lead && G.selection->x == s.x && G.selection->y == s.y) {
      a = lead;
      figure = lead->atSea ? NAVY : lead->type;
    }
    drawStack(ownerOr8(a->owner), figure, (int)s.stack.size(), s.sx, s.sy);
  }
  drawFog();
  drawRoute();
  // the stack itself, wherever the walk has got to
  const auto* at = walkingAt();
  if (at) {
    Army* a = G.walk->top;
    if (a && shown(at->first, at->second)) {
      std::vector<Army*> walking(G.walk->armies.begin(), G.walk->armies.end());
      drawStack(ownerOr8(a->owner), stackFigure(walking), (int)walking.size(), at->first * TILE, at->second * TILE);
    }
  }
  // the selection box, which follows the walk
  if (G.selection) {
    int x = at ? at->first : G.selection->x, y = at ? at->second : G.selection->y;
    if (shown(x, y)) {
      gfx::setColor(1, 1, 1);
      int base = G.selection->stack.size() > 1 ? 4 : 0;
      int frame = base + (int)std::floor(w2::now() * CURS_RATE) % 4;
      gfx::draw(G.cursImg, gfx::newQuad((frame % 4) * 64, (frame / 4) * 40, 40, 40), x * TILE, y * TILE);
    }
  }
}

void front::drawStrategicMap(int x, int y, const City* mark, bool noCities, const std::vector<int>* owners) {
  if (!G.stratImage) G.stratImage = screen::strategicImage(*G.screen, *G.g, G.player->index);
  gfx::setScissor(x, y, 224, 312);
  gfx::setColor(1, 1, 1);
  gfx::draw(G.stratImage, x, y);
  if (!noCities) {
    auto& cities = G.g->map->cities;
    for (size_t i = 0; i < cities.size(); i++) {
      const City& c = cities[i];
      int owner = owners && i < owners->size() ? (*owners)[i] : NONE;
      bool gone = owners ? owner == 0xff : c.razed;
      if (!gone && game::seen(*G.g, G.player->index, c.x, c.y)) {
        int side = owners ? owner : ownerOr8(c.ownerIndex);
        gfx::draw(G.atransShields, gfx::newQuad(side * 16, 30, 8, 8), x + c.x * 2 - 1, y + c.y * 2 - 1);
      }
    }
  }
  if (mark) {
    int mx = x + mark->x * 2 - 1, my = y + mark->y * 2 - 1;
    palColour(15);
    gfx::rectangle(gfx::FILL, mx - 1, my - 1, 10, 1);
    gfx::rectangle(gfx::FILL, mx - 1, my + 9, 10, 1);
    gfx::rectangle(gfx::FILL, mx - 1, my - 1, 1, 10);
    gfx::rectangle(gfx::FILL, mx + 9, my - 1, 1, 10);
  }
  gfx::setScissor();
}

void front::drawSiteMarkers(int x, int y, const w2::Site* highlight) {
  gfx::setScissor(x, y, 224, 312);
  for (auto& s : G.g->map->sites) {
    if (!(w2::site::shownTo(s, *G.player) && game::seen(*G.g, G.player->index, s.x, s.y))) continue;
    int sx, sy;
    if (s.content == w2::site::TEMPLE) { sx = 112; sy = 20; }
    else if (s.searched) { sx = 112; sy = 10; }
    else if (s.rich) { sx = 128; sy = 0; }
    else { sx = 112; sy = 0; }
    int mx = (s.x * 2 - 1 + 4) / 8 * 8;
    int my = s.y * 2 - 1;
    gfx::setColor(1, 1, 1);
    gfx::draw(G.atransShields, gfx::newQuad(sx, sy, 16, 10), x + mx, y + my);
    if (&s == highlight) {
      kit::setPal(15);
      kit::outline(x + mx, y + my, 12, 10);
    }
  }
  gfx::setScissor();
}

static void drawStrategic() {
  const auto& r = G.stratRect;
  front::drawStrategicMap(r.x, r.y);
  gfx::setScissor(r.x, r.y, r.w, r.h);
  // the view box (8961:0698): whatever the view covers, to the device pixel
  palColour(15);
  double s = display::d.scale;
  auto snap = [s](double v) { return std::floor(v * s + 0.5) / s; };
  auto [vw, vh] = viewSize();
  double bx = r.x + snap(G.cx * 2), by = r.y + snap(G.cy * 2);
  double w = snap(vw * 2), h = snap(vh * 2);
  gfx::rectangle(gfx::FILL, bx, by, w, 2);
  gfx::rectangle(gfx::FILL, bx, by + h - 2, w, 2);
  gfx::rectangle(gfx::FILL, bx, by, 2, h);
  gfx::rectangle(gfx::FILL, bx + w - 2, by, 2, h);
  gfx::setScissor();
}

void front::stratDirty() { G.stratImage.reset(); }

// The four configurable buttons are painted a second time from MENUBUTT.PCK
// at the rect the assigned item carries in UDB.DAT (545c:030a).
static void drawShortcutIcons() {
  auto& ui = G.screen->ui;
  auto art = G.screen->artFor(w2::uidata::SHORTCUT_BITMAP);
  for (int i = 0; i < w2::uidata::SHORTCUT_COUNT; i++) {
    auto c = screen::control(*G.screen, w2::uidata::SHORTCUT_FIRST + i);
    auto sc = ui.shortcuts.find(i);
    if (!c || sc == ui.shortcuts.end()) continue;
    auto item = ui.shortcutItems.find(sc->second);
    if (item == ui.shortcutItems.end()) continue;
    if (art) {
      auto src = item->second.src[G.screen->stateOf(c->id)];
      gfx::setColor(1, 1, 1);
      gfx::draw(art->image, gfx::newQuad(src.x, src.y, item->second.w, item->second.h), c->x, c->y);
    } else {
      std::string shortName = item->second.name.substr(0, 6);
      gfx::setColor(1, 1, 1);
      G.font->draw(shortName, c->x + std::max(1, (c->w - G.font->width(shortName)) / 2), c->y + (c->h - G.font->lineHeight) / 2);
    }
  }
}

// Turn and the sides still in it, at the right of the bar (8cc6:0952).
static void drawTurnStrip() {
  palColour(15);
  gfx::rectangle(gfx::FILL, 437, 0, 202, 17);
  std::vector<Side*> alive;
  for (Side* s : G.g->sides) if (s->alive) alive.push_back(s);
  int n = (int)alive.size();
  std::string turn = w2::fmt("Turn %d", G.g->turn);
  const Font& f = G.bigFont->colours(0, 15);
  f.draw(turn, (8 - n) * 16 + 508 - f.width(turn), 0);
  for (int k = 0; k < n; k++) {
    int x = (8 - n + k) * 16 + 512;
    if (alive[k] == G.player) {
      palColour(0);
      gfx::rectangle(gfx::FILL, x - 3, 1, 17, 15);
    }
    int s = alive[k]->index;
    gfx::setColor(1, 1, 1);
    gfx::draw(G.atransShields, gfx::newQuad(112 + (s / 4) * 16, 94 + (s % 4) * 14, 16, 14), x, 2);
  }
}

// The menu bar (7ae8:02d8, 2372:132f): white, the titles in TEXT, black. An
// open title and the row under the pointer are XORed with colour 7; a greyed
// item's glyph is colour 3. The dropdown (2372:0803) is white with a black
// outline and a shadow.
static void drawMenuBar() {
  palColour(15);
  gfx::rectangle(gfx::FILL, 0, 0, G.layout.w, menu::BAR_H);
  for (size_t i = 0; i < G.menuLayout.size(); i++) {
    const auto& m = G.menuLayout[i];
    bool lit = (int)i == G.openMenu;
    if (lit) {
      palColour(8);
      gfx::rectangle(gfx::FILL, m.x, 0, m.w, menu::BAR_H);
    }
    gfx::setColor(1, 1, 1);
    G.font->colours(lit ? 7 : 0, lit ? 8 : 15).draw(m.title, m.x + 2, menu::BAR_Y);
  }
  if (!G.starting && G.g) {
    gfx::push();
    gfx::translate(G.layout.ex, 0);
    drawTurnStrip();
    gfx::pop();
  }
  if (G.openMenu < 0 || G.openMenu >= (int)G.menuLayout.size()) return;
  const auto& d = G.menuLayout[G.openMenu].drop;
  palColour(15);
  gfx::rectangle(gfx::FILL, d.x, d.y, d.w - 1, d.h - 1);
  palColour(0);
  gfx::rectangle(gfx::FILL, d.x, d.y + d.h - 2, d.w - 2, 1);
  gfx::rectangle(gfx::FILL, d.x + 2, d.y + d.h - 1, d.w - 2, 1);
  gfx::rectangle(gfx::FILL, d.x, d.y, 1, d.h - 2);
  gfx::rectangle(gfx::FILL, d.x + d.w - 2, d.y, 1, d.h - 1);
  gfx::rectangle(gfx::FILL, d.x + d.w - 1, d.y + 1, 1, d.h - 1);
  const menu::Row* hover = menu::rowAt(G.menuLayout, G.openMenu, G.mouseX, G.mouseY);
  for (const auto& r : d.rows) {
    if (r.label == "-") {
      palColour(0);
      gfx::rectangle(gfx::FILL, r.x + 1, r.y - 1, d.w - 2, 1);
      continue;
    }
    std::string k = r.key.empty() ? r.act : r.key;
    bool grey = !menuEnabled(k);
    bool lit = &r == hover && !grey;
    if (lit) {
      palColour(8);
      gfx::rectangle(gfx::FILL, r.x + 1, r.y, d.w - 3, r.h);
    }
    int glyph = grey ? 3 : (lit ? 7 : 0);
    const Font& f = G.font->colours(glyph, lit ? 8 : 15);
    gfx::setColor(1, 1, 1);
    f.draw(r.label, r.x + 3, r.y);
    if (!r.key.empty()) f.draw(r.key, r.x + d.keyCol, r.y);
    // the setting in use (not the original's): a diamond in the key column
    if (menuTicked(k)) {
      palColour(glyph);
      int cx = r.x + d.keyCol + 2, cy = r.y + r.h / 2;
      for (int j = -2; j <= 2; j++) {
        int half = 2 - std::abs(j);
        gfx::rectangle(gfx::FILL, cx - half, cy + j, half * 2 + 1, 1);
      }
    }
  }
}

// The bottom bar's eight army slots and the mark under each are real
// controls -- ids 224-231 the slots, 232-239 the marks, 240/241 the Grp
// button -- drawn from ABITS.PCK and the army sheets at the rects 89e0:0356
// and 8611:08be use (docs/re/ui.md > The army slots).
static const int SLOT_FIRST = 224, BAR_FIRST = 232, SLOT_COUNT = 8;
static const int GRP_ALL = 240, GRP_NONE = 241;
static const int SLOT_X = 24, SLOT_Y = 405, SLOT_STEP = 40;
static const int SLOT_MOVES_X = 8, SLOT_MOVES_Y = 31;
static const int MARK_Y = 449, MARK_W = 32, MARK_H = 16;

static void drawAbits(int sx, int sy, int w, int h, int x, int y) {
  gfx::setColor(1, 1, 1);
  gfx::draw(G.abits, gfx::newQuad(sx, sy, w, h), x, y);
}

// A number in ABITS's own 8x8 digits, always two of them ("%02d").
static void drawDigits(int n, int x, int y) {
  std::string text = w2::fmt("%02d", std::max(0, std::min(99, n)));
  for (size_t i = 0; i < text.size(); i++) drawAbits(64 + (text[i] - '0') * 8, 30, 8, 8, x + (int)i * 8, y);
}

// The ring a slot's army sits in: each group a colour of its own, counted
// round from the current player's.
static int groupRing(int group) { return (G.player->index + group) % 8 + 1; }

static void drawArmySlots() {
  const slotsMod::Slots* s = G.selection ? &G.selection->slots : nullptr;
  for (int i = 0; i < SLOT_COUNT; i++) {
    int x = SLOT_X + i * SLOT_STEP;
    Army* a = s && i < s->n ? s->army[i] : nullptr;
    gfx::setColor(1, 1, 1);
    int ring = a ? groupRing(s->group[i]) : 0;
    gfx::draw(G.abits, gfx::newQuad(ring * 32, 0, 32, 30), x, SLOT_Y);
    if (a) {
      // the armies not moving with the group are drawn as ghosts
      gfx::draw(s->inGroup[i] ? G.armyImg[G.player->index] : G.shadowImg, kit::armyQuad(a->type), x, SLOT_Y);
      drawDigits(a->moves, x + SLOT_MOVES_X, SLOT_Y + SLOT_MOVES_Y);
      if (s->mark[i] != slotsMod::NOMARK) drawAbits(448, s->mark[i] == slotsMod::TICK ? 16 : 0, MARK_W, MARK_H, x, MARK_Y);
    }
  }
  // "Group Move", the group's movement, and the Grp button (89e0:0567)
  drawAbits(0, 30, 32, 8, 344, 420);
  drawAbits(32, 30, 32, 8, 344, 428);
  drawDigits(s ? slotsMod::moves(*s) : 0, 352, 436);
  bool green = s && slotsMod::grouped(*s);
  drawAbits(288, green ? 19 : 0, 32, 19, 344, 447);
  // the way the moving group travels (89e0:070d)
  std::vector<Army*> moving;
  if (s) for (int i = 0; i < s->n; i++) if (s->inGroup[i]) moving.push_back(s->army[i]);
  int icon = NONE;
  if (!moving.empty()) {
    bool sea = true, woods = false, hills = false;
    for (Army* a : moving) {
      const w2::ArmyType* t = G.g->types.byId(a->type);
      if (t->woodsMove) woods = true;
      if (t->hillsMove) hills = true;
      if (!a->atSea) sea = false;
    }
    if (move::modeOf(*G.g, moving) == move::FLYING) icon = 184;
    else if (sea) icon = 424;
    else if (woods && hills) icon = 216;
    else if (woods) icon = 248;
    else if (hills) icon = 152;
  }
  if (icon != NONE) {
    drawAbits(icon, 30, 32, 10, 344, 407);
  } else {
    palColour(3);
    gfx::rectangle(gfx::FILL, 344, 407, 32, 10);
  }
}

// With nothing selected the bar shows the side's standing (89e0:05a3):
// cities, treasury, income and upkeep, four 40 x 20 cells of ABITS.PCK each
// with a number beside it (4125:3072, :3092, :30a2, :3128).
struct StatusCell {
  int sx, sy, x, tx;
  const char* fmt;
};
static const StatusCell STATUS_CELLS[4] = {
    {344, 0, 32, 72, "%d"}, {344, 20, 120, 144, "%dgp"}, {384, 0, 200, 232, "%dgp"}, {384, 20, 280, 320, "%dgp"}};
static const int STATUS_Y = 425;

static int statusValue(int k) {
  switch (k) {
    case 0: return (int)game::sideCities(*G.g, *G.player).size();
    case 1: return G.player->gold;
    case 2: return game::income(*G.g, *G.player);
    default: return game::upkeep(*G.g, *G.player);
  }
}

static void drawStatus() {
  for (int k = 0; k < 4; k++) {
    const auto& st = STATUS_CELLS[k];
    drawAbits(st.sx, st.sy, 40, 20, st.x, STATUS_Y);
    G.bigFont->draw(w2::format(st.fmt, statusValue(k)), st.tx, STATUS_Y);
  }
}

// A computer's turn in the status bar (8cc6:0000, 5db9:0000): a line centred
// on (196, 415) in the side's colours, a black frame at (36, 442) 320x22, and
// the first n / 5 * 16 pixels of the side's MOVEBAR<n>.PCK at (32, 444).
static void drawComputerStatus() {
  const AiStatus& st = *G.aiStatus;
  Side* side = st.side;
  gfx::setColor(1, 1, 1);
  kit::centred(G.bigFont->colours(side->colour, side->edge), st.text, 196, 415);
  palColour(0);
  kit::outline(36, 442, 320, 22);
  int w = st.progress / 5 * 16;
  if (w > 0) {
    int i = side->index;
    if (!G.moveBars.count(i)) {
      try {
        G.moveBars[i] = pckimage::load(G.dataDir + "/TERRAIN0/MOVEBAR" + std::to_string(i) + ".PCK", G.palette, pckimage::CORNER);
      } catch (...) {
        G.moveBars[i] = nullptr;
      }
    }
    gfx::setColor(1, 1, 1);
    if (G.moveBars[i]) gfx::draw(G.moveBars[i], gfx::newQuad(0, 0, std::min(w, 320), 18), 32, 444);
  }
}

// The right button on the bottom bar (89e0:0ad9).
static void barInfo(int x, int y) {
  int bx = x - G.layout.bar.x;
  if (G.aiStatus) return;
  if (G.selection) {
    if (bx < 336) {
      int n = (bx - 16) / 40;
      if (bx >= 16 && n < G.selection->slots.n) infobox::army(x, y, G.selection->slots.army[n]);
      else infobox::lines(x, y, "Select Army", "Select armies when present");     // 4125:3222
    } else {
      infobox::lines(x, y, "Group/Ungroup", "Manipulate all armies");            // 4125:3169
    }
    return;
  }
  static const char* const HELP[4][2] = {{"Number of Cities", "You have %d cities!"},
                                         {"Your Treasury", "You have %d gold!"},
                                         {"Your Income", "You earn %d gold!"},
                                         {"Your Upkeep", "You pay %d gold!"}};
  int k = (bx - 16) / 90;
  if (bx >= 16 && k >= 0 && k < 4) infobox::lines(x, y, HELP[k][0], w2::format(HELP[k][1], statusValue(k)));
}

static void drawBottomBar() {
  // 8065:0aeb blits MARBLE.PCK over the bar, (16, 403) 360x66 from (0, 60).
  gfx::setColor(1, 1, 1);
  gfx::draw(G.marble, gfx::newQuad(0, 60, 360, 66), 16, 403);
  if (G.aiStatus) drawComputerStatus();
  else if (G.selection) drawArmySlots();
  else drawStatus();
}

// The city dialog's Vector mode paints the map its own way (834b:08df, and
// 834b:1817 for See All): a marker on each of the side's cities from
// ATRANS2.PCK's row at y = 94 (4125:2c76), and lines for the vectors.
static const int VMARK_X[7] = {0, 32, 48, 16, 64, 80, 96};

// 2133:00b8 draws a plain Bresenham line, both ends included
static void linePixels(int x0, int y0, int x1, int y1) {
  int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;
  for (;;) {
    gfx::rectangle(gfx::FILL, x0, y0, 1, 1);
    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void front::drawVectorMap(int x, int y, City* city, int filter, bool seeAll) {
  if (!G.stratImage) G.stratImage = screen::strategicImage(*G.screen, *G.g, G.player->index);
  gfx::setScissor(x, y, 224, 312);
  gfx::setColor(1, 1, 1);
  gfx::draw(G.stratImage, x, y);
  auto marker = [&](const City* c, int k) {
    gfx::setColor(1, 1, 1);
    gfx::draw(G.atransShields, gfx::newQuad(VMARK_X[k], 94, 16, 10), x + c->x * 2 - 2, y + c->y * 2 - 1);
  };
  auto line = [&](const City* a, const City* b, int colour) {
    palColour(colour);
    linePixels(x + a->x * 2 + 2, y + a->y * 2 + 2, x + b->x * 2 + 2, y + b->y * 2 + 2);
  };
  auto mine = game::sideCities(*G.g, *G.player);
  auto building = [](const City* c) { return c->producing != NONE ? 0 : 3; };
  auto [sx, sy] = game::standardAt(*G.g, *G.player);
  auto toStandard = [&, sx = sx, sy = sy](const City* c, int colour) {
    palColour(colour);
    linePixels(x + c->x * 2 + 2, y + c->y * 2 + 2, x + sx * 2, y + sy * 2);
  };
  if (seeAll) {
    std::set<const City*> done;
    for (City* c : mine) {
      if (done.count(c)) continue;
      done.insert(c);
      auto incoming = game::vectoredTo(*G.g, c);
      int k = building(c);
      if (c->vectorTo != NONE) k = 2;
      if (!incoming.empty()) k = c->producing != NONE ? 1 : 5;
      marker(c, k);
      City* dest = c->vectorTo >= 0 ? G.g->map->city(c->vectorTo) : nullptr;
      if (dest) {
        done.insert(dest);
        marker(dest, dest->producing != NONE ? 1 : 5);
        line(dest, c, 7);
      } else if (c->vectorTo == game::STANDARD && sx != NONE) {
        toStandard(c, 8);
      }
      for (City* src : incoming) {
        done.insert(src);
        marker(src, 2);
        line(src, c, 8);
      }
    }
    palColour(15);
    int bx = x + (city ? city->x : -10) * 2 - 3, by = y + (city ? city->y : -10) * 2 - 2;
    gfx::rectangle(gfx::FILL, bx, by, 11, 1);
    gfx::rectangle(gfx::FILL, bx, by + 11, 11, 1);
    gfx::rectangle(gfx::FILL, bx, by, 1, 11);
    gfx::rectangle(gfx::FILL, bx + 11, by, 1, 11);
  } else if (city) {
    for (City* c : mine) {
      int n = (int)game::vectoredTo(*G.g, c).size();
      bool full = (filter == -1 && n + 1 > game::MAX_VECTORED_TO) || (filter >= 0 && filter != NONE && n + filter > game::MAX_VECTORED_TO);
      if (c == city || !full) {
        int k = building(c);
        if (c == city) k = c->producing != NONE ? 4 : 6;
        marker(c, k);
      }
    }
    City* dest = city->vectorTo >= 0 ? G.g->map->city(city->vectorTo) : nullptr;
    if (dest) {
      marker(dest, dest->producing != NONE ? 1 : 5);
      line(city, dest, 7);
    } else if (city->vectorTo == game::STANDARD && sx != NONE) {
      toStandard(city, 7);
    }
    for (City* src : game::vectoredTo(*G.g, city)) {
      marker(src, 2);
      line(src, city, 8);
    }
  }
  if (sx != NONE) {
    gfx::setColor(1, 1, 1);
    gfx::draw(G.atransShields, gfx::newQuad(96, 15, 16, 15), x + std::max(0, (sx * 2 - 2) / 8 * 8), y + std::max(0, sy * 2 - 6));
  }
  gfx::setScissor();
}

void front::drawHeroFigure(int x, int y, int tx, int ty) {
  gfx::setScissor(x, y, 224, 312);
  gfx::setColor(1, 1, 1);
  gfx::draw(G.atransShields, gfx::newQuad(96, 0, 16, 15), x + std::max(0, (tx * 2 - 2) / 8 * 8), y + std::max(0, ty * 2 - 6));
  gfx::setScissor();
}

void front::drawStrategicPanel(int x, int y, const City* city) { drawStrategicMap(x, y, city); }

// The start-of-turn banner: CITY.PCK framed in the side's colour, the frame
// painted into the picture (54f6:0000).
static void drawBanner() {
  const Banner& b = *G.banner;
  const auto& R = BANNER;
  auto fill = [](int x, int y, int w, int h) { gfx::rectangle(gfx::FILL, x, y, w, h); };
  kit::popupFrame(R);
  gfx::setColor(1, 1, 1);
  gfx::draw(G.cityPic, R.x, R.y);
  auto outline = [&](int x, int y, int w, int h) {
    fill(R.x + x, R.y + y, w, 1);
    fill(R.x + x, R.y + y + h - 1, w, 1);
    fill(R.x + x, R.y + y, 1, h);
    fill(R.x + x + w - 1, R.y + y, 1, h);
  };
  palColour(b.edge);
  outline(0, 0, R.w, R.h);
  palColour(b.colour);
  fill(R.x + 1, R.y + 1, R.w - 2, 9);
  fill(R.x + 1, R.y + 1, 9, R.h - 2);
  fill(R.x + 1, R.y + R.h - 10, R.w - 2, 9);
  fill(R.x + R.w - 10, R.y + 1, 9, R.h - 2);
  palColour(b.edge);
  outline(10, 10, R.w - 20, R.h - 20);
  gfx::setColor(1, 1, 1);
  kit::centred(*G.titleFont, b.name, 320, BANNER_NAME_Y);
  kit::centred(*G.titleFont, w2::fmt("Turn %d", b.turn), 320, BANNER_TURN_Y);
}

// The hero offer: the marble, the map with where the hero would appear,
// the portrait, four lines, the name field and the two checkboxes.
static void drawHeroOffer() {
  const auto& R = HERO_POPUP;
  const w2::HeroOffer& b = *G.offer;
  kit::popupFrame(R);
  gfx::setColor(1, 1, 1);
  gfx::setScissor(R.x, R.y, R.w, R.h);
  gfx::draw(G.marble, R.x, R.y);
  gfx::setScissor();
  front::drawStrategicMap(HERO_MAP_X, HERO_MAP_Y);
  gfx::setScissor(R.x, R.y, R.w, R.h);
  gfx::setColor(1, 1, 1);
  int fx = std::max(0, (b.city->x * 2 - 2) / 8 * 8);
  int fy = std::max(0, b.city->y * 2 - 6);
  gfx::draw(G.atransShields, gfx::newQuad(96, 0, 16, 15), HERO_MAP_X + fx, HERO_MAP_Y + fy);
  gfx::draw(G.offerFemale ? G.heroPicF : G.heroPicM, HERO_PIC_X, HERO_PIC_Y);
  gfx::setColor(0, 0, 0);
  gfx::rectangle(gfx::LINE, HERO_PIC_X - 0.5, HERO_PIC_Y - 0.5, HERO_PIC_W + 1, HERO_PIC_H + 1);
  gfx::setScissor();
  auto centred = [](const Font& f, const std::string& s, int y) {
    gfx::setColor(1, 1, 1);
    f.draw(s, HERO_CENTRE - f.width(s) / 2, y);
  };
  centred(*G.titleFont, kit::text(HERO_TITLE_GROUP, 0), HERO_TITLE_Y);
  auto lines = heroLines(b, G.offerFemale);
  for (int i = 0; i < 4; i++) {
    // font 2 in 15 with a colour-14 outline (78a8:06ae(2, 15, 14, 3))
    if (!lines[i].empty()) centred(G.bigFont->colours(15, 14), lines[i], HERO_LINE_Y[i]);
  }
  if (auto field = screen::dialogControl(*G.heroView, HERO_FIELD)) {
    gfx::setColor(0, 0, 0);
    kit::outline(field->x - 2, field->y - 2, field->w + 4, field->h + 4);
    kit::field(field->x, field->y, field->w, field->h, G.offerName, G.bigFont.get());
  }
  auto label = [](const int* at, const std::string& s) {
    gfx::setColor(1, 1, 1);
    G.bigFont->draw(s, at[0] - G.bigFont->width(s), at[1]);
  };
  label(MALE_LABEL, kit::text(HERO_SEX_GROUP, 0));
  label(FEMALE_LABEL, kit::text(HERO_SEX_GROUP, 1));
  auto box = [](int id, bool on) {
    auto c = screen::dialogControl(*G.heroView, id);
    if (!c) return;
    gfx::setColor(1, 1, 1);
    gfx::draw(G.abits, gfx::newQuad(320, on ? 0 : 20, 24, 20), c->x, c->y);
  };
  box(HERO_MALE, !G.offerFemale);
  box(HERO_FEMALE, G.offerFemale);
  screen::drawDialogControls(*G.screen, *G.heroView);
}

// ------------------------------------------------------------------ the assault

// A city is not walked into, it is assaulted, and a stack always fights from
// one of the eight tiles around it. What the player sees, in order: the fire
// cloud from WAR.PCK over the tile (67cc:1836), the battle window with both
// lines drawn up (6a35:0160), the fight played back one army at a time in
// the order combat_resolve logged (6a35:0094), how it ended (6a35:04c5), and
// if a city fell, what is to be done with it (63fa:0000). The fight is
// decided before any of it is drawn (docs/re/ui.md > The assault).
static const kit::Rect AS_WINDOW{160, 60, 320, 312};   // popup 8
static const int AS_CLOUD_W = 128, AS_CLOUD_H = 120;   // 4125:0cfa
static const int AS_SHIELD_W = 32, AS_SHIELD_H = 36;
static const int AS_SHIELD_DEF[2] = {176, 86}, AS_SHIELD_ATK[2] = {176, 246};
static const std::vector<int> AS_ROWS = {86, 116, 146, 176};   // the defender's rows
static const int AS_ATK_ROW = 246;
static const int AS_X_EVEN[8] = {216, 248, 280, 312, 344, 376, 408, 440};
static const int AS_X_ODD[7] = {232, 264, 296, 328, 360, 392, 424};
static const int AS_SEA[4] = {0, 162, 32, 18}, AS_SEA_DROP = 10;    // WAR.PCK's water
static const int AS_TEXT_Y = 298, AS_TEXT_STEP = 20;     // 4125:4366
static const double AS_CLOUD_TIME = 0.7;
// one casualty (6a35:0094): 5, 3 and 5 BIOS ticks; once hurried, 2 and 3
static const double AS_FELL_TIME = 13 / 18.2, AS_FELL_FAST = 5 / 18.2;
static const double AS_BLAST_TIME = 5 / 18.2, AS_BLAST_FAST = 2 / 18.2;
static const int AS_FLED = 141, AS_WON_CITY_HERO = 142, AS_WON_CITY = 143, AS_WON_HERO = 144, AS_WON = 145, AS_LOST = 146, AS_LOOT = 147;
static const int AS_V_TEXT = 65, AS_V_WHO = 66, AS_V_WHERE = 67;
static const kit::Rect AS_V_POPUP{160, 90, 320, 200};   // popup 7, exactly VICTORY.PCK
static const int AS_V_DIALOG = 11;
static const int AS_OCCUPY = 285, AS_PILLAGE = 283, AS_SACK = 286, AS_RAZE = 284;
static const int AS_V_TITLE_Y = 93;
static const int AS_V_LINE_Y[4] = {140, 160, 180, 200};

// Where slot k (from 1) of a line of n sits across the window (6a35:0160).
static int slotX(int n, int k) {
  if (n >= 8) return AS_X_EVEN[k - 1];
  int from = 4 - (n + 1) / 2;
  return n % 2 == 1 ? AS_X_ODD[from + k - 1] : AS_X_EVEN[from + k - 1];
}

// A whole line laid out: rows of eight from the top, the last centred.
static std::vector<std::pair<int, int>> lineSlots(int n, const std::vector<int>& rows) {
  std::vector<std::pair<int, int>> out(n);
  int i = 1;
  size_t row = 0;
  while (n - i + 1 >= 8 && row + 1 < rows.size()) {
    for (int k = 1; k <= 8; k++) out[i + k - 2] = {AS_X_EVEN[k - 1], rows[row]};
    i += 8;
    row++;
  }
  int y = row < rows.size() ? rows[row] : rows.back();
  for (int k = 1; k <= n - i + 1; k++) out[i + k - 2] = {slotX(n - i + 1, k), y};
  return out;
}

// The name the spoils dialog calls the victor by: the hero who led the
// assault, else the best army in the line.
static std::string victorName(const std::vector<Army*>& line) {
  for (Army* a : line) if (a->hero() && !a->name.empty()) return a->name;
  const w2::ArmyType* rec = line.empty() ? nullptr : G.g->types.byId(line[0]->type);
  return rec ? rec->name : "Your armies";
}

// How the fight ended, in the original's own words.
static std::vector<std::string> outcomeLines(const w2::Battle& result, const City* city, bool fled) {
  std::vector<std::string> out;
  if (!result.won) {
    out.push_back(kit::text(AS_LOST, 0));
    return out;
  }
  std::string heroName;
  for (Army* a : result.lines.attackers) if (a->hero() && !a->name.empty()) { heroName = a->name; break; }
  if (city) {
    if (fled) out.push_back(kit::text(AS_FLED, G.g->rng.dice(1, 4, -1)));
    if (!heroName.empty()) out.push_back(format(kit::text(AS_WON_CITY_HERO, 0), heroName));
    else out.push_back(kit::text(AS_WON_CITY, 0));
  } else if (!heroName.empty()) {
    out.push_back(format(kit::text(AS_WON_HERO, 0), heroName));
    out.push_back(kit::text(AS_WON_HERO, 1));
  } else {
    out.push_back(kit::text(AS_WON, 0));
  }
  if (result.loot > 0) out.push_back(format(kit::text(AS_LOOT, 0), result.loot));
  return out;
}

// Begin showing an assault; the fight is already decided.
static void startAssault(int x, int y, const w2::Battle& result) {
  const auto& lines = result.lines;
  bool fled = lines.defenders.empty();
  Assault a;
  a.x = x;
  a.y = y;
  a.result = result;
  a.def = lines.defenders;
  a.atk = lines.attackers;
  a.defSlots = lineSlots((int)lines.defenders.size(), AS_ROWS);
  a.atkSlots = lineSlots((int)lines.attackers.size(), {AS_ATK_ROW});
  a.defSide = lines.defenders.empty() ? 8 : ownerOr8(lines.defenders[0]->owner);
  a.atkSide = lines.attackers.empty() ? 8 : ownerOr8(lines.attackers[0]->owner);
  a.phase = "cloud";
  a.at = w2::now();
  // on a human's turn 67cc:1836 holds the cloud while WAR.8SN plays
  a.cloudTime = (G.g->side && !G.g->side->computer) ? sound::effect("war") : 0;
  a.victor = victorName(lines.attackers);
  a.message = outcomeLines(result, lines.city, fled);
  a.defFell.resize(a.def.size());
  a.atkFell.resize(a.atk.size());
  G.assault = a;
}
void front::startAssault(int x, int y, const w2::Battle& result) { ::startAssault(x, y, result); }

// Close the battle window and let the fight take effect (67cc:0a6b).
void front::closeAssault() {
  if (!G.assault) return;
  Assault a = std::move(*G.assault);
  G.assault.reset();
  game::applyAttack(*G.g, a.result);
  front::stratDirty();
  if (a.after) a.after();
  // a computer's assault is the coroutine's: it carries on
}

// Carry the playback on by the clock.
static void advanceAssault() {
  Assault& a = *G.assault;
  double t = w2::now();
  if (a.phase == "cloud") {
    if (t - a.at >= std::max(AS_CLOUD_TIME, a.cloudTime)) {
      // a computer's battle nobody is to see: the cloud was all of it
      if (a.computer && !a.window) { front::closeAssault(); return; }
      a.phase = "battle";
      a.at = t;
      // 67cc:124a waits five ticks on the drawn-up lines before the fight
      if (a.computer) a.at = t + 5 / 18.2;
    }
    return;
  }
  if (a.phase == "over" && a.computer) {
    if (a.closeAt == 0) {
      a.closeAt = t + 15 / 18.2;
      if (G.aiStatus) G.aiStatus->text = a.outcome;
    } else if (t >= a.closeAt) {
      front::closeAssault();
    }
    return;
  }
  if (a.phase != "battle") return;
  double each = a.fast ? AS_FELL_FAST : AS_FELL_TIME;
  const auto& log = a.result.log;
  while (a.step < log.size() && t >= a.at) {
    a.step++;
    Fell fell{a.at, a.fast ? AS_BLAST_FAST : AS_BLAST_TIME};
    if (log[a.step - 1] == 1) {
      if (a.atkDown < (int)a.atkFell.size()) a.atkFell[a.atkDown] = fell;
      a.atkDown++;
    } else {
      if (a.defDown < (int)a.defFell.size()) a.defFell[a.defDown] = fell;
      a.defDown++;
    }
    // 7dda:0181: a blow for each, until Space hurries the fight on
    if (!a.fast) sound::effect(log[a.step - 1] == 1 ? "army" : "army2");
    a.at += each;
  }
  if (a.step >= log.size() && t >= a.at) a.phase = "over";
}

// A key or a click: run the playback through, and when it is through, close
// the window and ask what is to be done with the city.
static void pressAssault() {
  if (!G.assault) return;
  Assault& a = *G.assault;
  if (a.phase != "over") {
    a.fast = true;
    a.at = std::min(a.at, w2::now() + AS_FELL_FAST);
    advanceAssault();
    return;
  }
  City* city = a.result.captured;
  bool computer = a.computer;
  std::string victor = a.victor;
  // captured is set once applied: apply, then ask
  front::closeAssault();
  if (!computer) {
    // the fight took effect in closeAssault; see whether a city fell
    if (G.selection && !G.selection->stack.empty()) {
      City* c = game::cityAt(*G.g, G.selection->stack[0]->x, G.selection->stack[0]->y);
      if (c && c->ownerIndex == G.player->index) city = c;
    }
    if (city) presentVictory(city, victor);
  }
}

// The cloud over the tile, drawn with the map, in map pixels.
static void drawCloud() {
  const Assault& a = *G.assault;
  int sx = a.x * TILE + (TILE - AS_CLOUD_W) / 2;
  int sy = a.y * TILE + (TILE - AS_CLOUD_H) / 2;
  gfx::setColor(1, 1, 1);
  gfx::draw(G.warPic, gfx::newQuad(0, 0, AS_CLOUD_W, AS_CLOUD_H), sx, sy);
}

// One line of armies, struck off from the front as they fall.
static void drawBattleLine(const std::vector<Army*>& armies, const std::vector<std::pair<int, int>>& slots, int side, int down,
                           const std::vector<Fell>& fell) {
  if (side < 0 || side > 8) side = 8;
  double t = w2::now();
  for (size_t i = 0; i < armies.size() && i < slots.size(); i++) {
    bool gone = (int)i < down && i < fell.size() && t >= fell[i].at + fell[i].burn;
    if (gone) continue;
    auto at = slots[i];
    gfx::setColor(1, 1, 1);
    if (armies[i]->atSea) gfx::draw(G.warPic, gfx::newQuad(AS_SEA[0], AS_SEA[1], AS_SEA[2], AS_SEA[3]), at.first, at.second + AS_SEA_DROP);
    gfx::draw(G.armyImg[side], kit::armyQuad(armies[i]->type), at.first, at.second);
    if ((int)i < down) gfx::draw(G.atransShields, gfx::newQuad(32, 0, 32, 29), at.first, at.second);
  }
}

static void drawBattle() {
  const Assault& a = *G.assault;
  const auto& R = AS_WINDOW;
  gfx::setColor(0, 0, 0);
  gfx::rectangle(gfx::LINE, R.x - 0.5, R.y - 0.5, R.w + 1, R.h + 1);
  gfx::rectangle(gfx::FILL, R.x + 1, R.y + R.h + 1, R.w + 2, 2);
  gfx::rectangle(gfx::FILL, R.x + R.w + 1, R.y + 1, 2, R.h + 2);
  gfx::setColor(1, 1, 1);
  gfx::draw(G.marble, gfx::newQuad(0, 0, R.w, R.h), R.x, R.y);
  auto shield = [](int side, const int* at) {
    gfx::setColor(1, 1, 1);
    gfx::draw(G.shieldImg, gfx::newQuad(side * AS_SHIELD_W, 0, AS_SHIELD_W, AS_SHIELD_H), at[0], at[1]);
  };
  shield(a.defSide, AS_SHIELD_DEF);
  shield(a.atkSide, AS_SHIELD_ATK);
  drawBattleLine(a.def, a.defSlots, a.defSide, a.defDown, a.defFell);
  drawBattleLine(a.atk, a.atkSlots, a.atkSide, a.atkDown, a.atkFell);
  if (a.phase == "over") {
    int y = AS_TEXT_Y;
    gfx::setColor(1, 1, 1);
    for (auto& line : a.message) {
      kit::centred(*G.bigFont, line, 320, y);       // 6a35:04c5, on x = 320
      y += AS_TEXT_STEP;
    }
  }
}

static void drawAssault() {
  advanceAssault();
  if (!G.assault) return;
  if (G.assault->phase != "cloud") drawBattle();
}

// Dialog 11 behind popup 7 -- exactly VICTORY.PCK -- with four buttons.
// Pillage needs a production type to strip and sack two (63fa:0000).
static void presentVictory(City* city, const std::string& victor) {
  if (!G.victoryView) G.victoryView = screen::dialog(*G.screen, AS_V_DIALOG);
  Victory v;
  v.city = city;
  v.who = format(kit::text(AS_V_WHO, G.g->rng.dice(1, 4, -1)), victor);
  v.where = format(kit::text(AS_V_WHERE, G.g->rng.dice(1, 3, -1)), city->name);
  G.victory = v;
  auto& st = G.victoryView->state;
  st[AS_OCCUPY] = NORMAL;
  st[AS_RAZE] = NORMAL;
  st[AS_PILLAGE] = city->slots.size() >= 1 ? NORMAL : DISABLED;
  st[AS_SACK] = city->slots.size() >= 2 ? NORMAL : DISABLED;
}
void front::presentVictory(City* city, const std::string& victor) { ::presentVictory(city, victor); }

static void drawVictory(const Victory& v) {
  const auto& R = AS_V_POPUP;
  gfx::setColor(0, 0, 0);
  gfx::rectangle(gfx::LINE, R.x - 0.5, R.y - 0.5, R.w + 1, R.h + 1);
  gfx::rectangle(gfx::FILL, R.x + 1, R.y + R.h + 1, R.w + 2, 2);
  gfx::rectangle(gfx::FILL, R.x + R.w + 1, R.y + 1, 2, R.h + 2);
  gfx::setColor(1, 1, 1);
  gfx::draw(G.victoryPic, R.x, R.y);
  kit::centred(*G.titleFont, kit::text(AS_V_TEXT, 0), 320, AS_V_TITLE_Y);
  kit::centred(*G.bigFont, v.who, 320, AS_V_LINE_Y[0]);
  kit::centred(*G.bigFont, v.where, 320, AS_V_LINE_Y[1]);
  kit::centred(*G.bigFont, kit::text(AS_V_TEXT, 1), 320, AS_V_LINE_Y[2]);
  kit::centred(*G.bigFont, kit::text(AS_V_TEXT, 2), 320, AS_V_LINE_Y[3]);
  screen::drawDialogControls(*G.screen, *G.victoryView);
}

// What the player chose to do with the city they have just taken (63fa).
static void takeCity(int what) {
  if (!G.victory) return;
  Victory v = *G.victory;
  City* c = v.city;
  std::vector<Army*> stack = G.selection ? G.selection->stack : std::vector<Army*>{};
  G.victory.reset();
  auto done = []() {
    G.victoryUnder.reset();
    front::stratDirty();
    refreshControls();
  };
  if (what == AS_PILLAGE || what == AS_SACK) {
    bool sacked = what == AS_SACK;
    auto spoils = sacked ? game::sack(*G.g, *G.player, *c, stack) : game::pillage(*G.g, *G.player, *c, stack);
    G.victoryUnder = v;
    spoils::open(sacked, c, spoils.gold, spoils.lost, (int)c->slots.size(), done);
  } else if (what == AS_RAZE) {
    G.victoryUnder = v;
    search::say(c->name + " is in ruins!", [c, stack, done]() {     // 4125:0a56
      game::raze(*G.g, *G.player, *c, stack);
      done();
    });
  } else {
    // quest_check(4): a quest done is its reward instead of the city
    auto news = game::occupy(*G.g, *G.player, *c, stack);
    auto production = [c]() { city::open(c, city::PRODUCTION); };
    if (news) {
      G.player->questNews.reset();
      if (!news->failed.empty()) questnews::show(news, production);
      else questnews::show(news);
    } else {
      production();
    }
  }
}

// Draw in the popups' 640x480 frame, centred on the screen.
static void inDialogFrame(const std::function<void()>& draw) {
  gfx::push();
  gfx::translate(G.layout.dialog.x, G.layout.dialog.y);
  draw();
  gfx::pop();
}

// Draw one of the fixed groups of the original screen where it now is.
static void inGroup(const layout::Offset& o, const std::function<void()>& draw) {
  gfx::push();
  gfx::translate(o.x, o.y);
  draw();
  gfx::pop();
}

static void drawModals() {
  // a dialog may push another while drawing is under way: a copy is drawn
  auto modals = G.modals;
  for (auto& d : modals) d->draw();
}

// The samples queue and the advisor blinks by the clock.
static void update() {
  sound::update();
  if (auto top = kit::topShared()) top->update();
}

// One frame: the chrome in UI pixels, the map in map pixels at its own
// zoom, then the dialogs and the pointer over both.
static void draw() {
  gfx::begin();
  gfx::clear(0, 0, 0);
  syncLayout();
  stepComputer();
  display::pushUI();
  if (G.starting || !G.g) {
    drawModals();
    drawMenuBar();
    drawPointer();
    gfx::pop();
    return;
  }
  advanceWalk();
  // a quest that ended is told once the screen is the player's again
  if (G.player && G.player->questNews &&
      !(G.over || G.banner || G.offer || G.assault || G.victory || G.victoryUnder || G.walk || G.moveAll || G.aiRun ||
        G.openMenu >= 0 || kit::top())) {
    questnews::poll();
  }
  refreshControls();
  screen::drawBackground(*G.screen, G.layout);
  const auto& r = G.mapRect;
  gfx::setScissor(r.x, r.y, r.w, r.h);
  auto [cx, cy] = camDev();
  display::pushMap(r.x, r.y, cx, cy, G.zoom);
  drawMap();
  if (G.assault && G.assault->phase == "cloud") drawCloud();
  gfx::pop();
  gfx::setScissor();
  drawStrategic();
  // 8065:0a9d refills the control panel with marble before the controls go on
  inGroup(G.layout.panel, []() {
    gfx::setColor(1, 1, 1);
    gfx::draw(G.marble, gfx::newQuad(0, 0, 224, 114), 400, 355);
  });
  screen::drawControls(*G.screen);
  drawShortcutIcons();
  inGroup(G.layout.bar, drawBottomBar);
  inDialogFrame([]() {
    if (G.victoryUnder) drawVictory(*G.victoryUnder);
    drawModals();
  });
  drawMenuBar();
  // the turn opens with the banner over the offer, and is dismissed first
  inDialogFrame([]() {
    if (G.offer) drawHeroOffer();
    if (G.assault) drawAssault();
    if (G.victory) drawVictory(*G.victory);
    if (G.banner) drawBanner();
  });
  drawPointer();
  gfx::pop();
}

// ------------------------------------------------------------------ input

// 1a8b:04c8 reads no input while a stack walks, and Move All walks one stack
// after another. A click meanwhile is lost; a key waits in the keyboard's
// buffer, fifteen of them, and is read once it is all over.
static bool busyWalking() { return ((G.walk && !G.walk->computer) || G.moveAll) && !kit::top(); }

static void flushKeys() {
  auto ks = G.keyBuffer;
  G.keyBuffer.clear();
  for (auto& k : ks) keypressed(k);
}

// While the computer plays: a key hurries a battle, or runs the rest of the
// round through unwatched. True when it took the input.
static bool computerInput(const std::string& k) {
  if (!(G.aiRun && !kit::top())) return false;
  if (G.assault && G.assault->computer) { pressAssault(); return true; }
  // Shift and Alt are held to reach Settings, not pressed to skip
  if (w2::endsWith(k, "shift") || w2::endsWith(k, "alt")) return true;
  G.aiSkip = true;
  if (G.walk && G.walk->computer) {
    G.walk.reset();
    resumeComputer();
  }
  return true;
}

// A control's id, minus 100, indexes the 397-entry jump table at 17be:0b0c:
// that is what turns a click into an action (docs/re/ui.md > Commands).
static std::map<int, std::function<void()>>& actions();

static void mousepressed(int x, int y, int button) {
  if (busyWalking()) return;
  syncLayout();
  int fx = x - G.layout.dialog.x, fy = y - G.layout.dialog.y;
  if (computerInput("")) return;
  // 7ecb:0142: the banner eats the click that dismisses it
  if (G.banner) { dismissBanner(); return; }
  if (G.assault) { pressAssault(); return; }
  // the right button is not a click on a dialog's button (18a9:012c)
  if ((G.victory || G.offer) && button == 2) return;
  if (G.victory) {
    auto c = screen::dialogControlAt(*G.victoryView, fx, fy);
    if (c && G.victoryView->stateOf(c->id) != DISABLED) takeCity(c->id);
    return;
  }
  if (G.offer) {
    auto c = screen::dialogControlAt(*G.heroView, fx, fy);
    if (!c) return;
    if (c->id == HERO_MALE) G.offerFemale = false;
    else if (c->id == HERO_FEMALE) G.offerFemale = true;
    else if (c->id == HERO_OK) acceptOffer();
    else if (c->id == HERO_CANCEL && !G.offer->first) refuseOffer();
    return;
  }
  // on the start screens the menu bar stays live over them (7f77:0200)
  auto top = kit::topShared();
  if (top && top->menuBar && (G.openMenu >= 0 || menu::titleAt(G.menuLayout, x, y) >= 0)) top = nullptr;
  if (top) {
    // the right button on a dialog: a control's help, or the dialog's regions
    if (button == 2 && top->view) {
      auto c = infobox::controlAt(*top->view, fx, fy, &top->hidden);
      if (!(c && infobox::control(x, y, c->id, top.get()))) top->rightpressed(fx, fy, x, y);
      return;
    }
    top->mousepressed(fx, fy, button);
    return;
  }
  if (G.over || !G.g) return;
  // the menu bar takes precedence over everything beneath it
  int hit = menu::titleAt(G.menuLayout, x, y);
  if (hit >= 0) {
    G.openMenu = G.openMenu == hit ? -1 : hit;
    return;
  }
  if (G.openMenu >= 0) {
    const menu::Row* row = menu::rowAt(G.menuLayout, G.openMenu, x, y);
    G.openMenu = -1;
    std::string k = row ? (row->key.empty() ? row->act : row->key) : "";
    if (!k.empty() && menuEnabled(k)) menuPick(k);
    return;
  }
  auto c = screen::controlAt(*G.screen, x, y);
  if (c && button == 2) {
    if (infobox::control(x, y, c->id)) return;
  } else if (c) {
    if (G.screen->stateOf(c->id) == DISABLED) return;
    G.pressed = c->id;
    G.screen->state[c->id] = ACTIVE;
    return;
  }
  // the hand drags the map (740d:00e4 -> 8065:0e04)
  if (button == 1 && pointerKind(x, y) == HAND) {
    G.drag = Drag{G.cx, G.cy, 0, 0};
    return;
  }
  auto r = screen::regionAt(*G.screen, x, y);
  if (!r) return;
  if (r->id == screen::MAP) {
    auto tile = tileAtPoint(x, y);
    if (!tile) return;
    auto [tx, ty] = *tile;
    if (button == 2) { tileinfo::open(tx, ty); return; }
    // the left button does what the pointer shows (740d:00ce)
    int k = pointerKind(x, y);
    if (k == WALK || k == BOAT) moveSelection(tx, ty);
    else if (k == ATTACK || k == PEACE) moveSelection(tx, ty, true);
    else if (k == CITY_P || k == SITE_P) {
      if (City* city = game::cityAt(*G.g, tx, ty)) front::openCity(city);
    } else if (k == ADVISE) {
      miladvisor::open(G.selection ? &G.selection->stack : nullptr, tx, ty);
    } else if (k == SELECT) {
      select(tx, ty);
    } else if (k == ALT) {
      planRoute(tx, ty);
      refreshControls();
    }
  } else if (r->id == screen::STRATEGIC) {
    int tx = (x - r->x) / 2, ty = (y - r->y) / 2;
    if (button != 1) return;
    if (held({"lalt", "ralt"})) {
      planRoute(tx, ty);
      refreshControls();
    } else {
      centreOn(tx, ty);
    }
  } else if (r->id == screen::BOTTOMBAR && button == 2) {
    barInfo(x, y);
  } else if (r->id == screen::MAPPANEL && button == 2) {
    infobox::lines(x, y, "- Drag Screen -", "Move mouse to drag the screen");
  }
}

void front::afterSlotChange() { syncSelection(); }

// Order > Move All (1c8c:04c4): every army of the side with a destination
// walks there in turn, each walk played out before the next.
static void moveAllStep() {
  while (G.moveAll) {
    if (G.walk) return;
    Army* lead = nullptr;
    for (Army* a : G.g->armies) {
      if (a->owner == G.player->index && a->target && !a->transit && !G.moveAll->seen.count(a)) { lead = a; break; }
    }
    if (!lead) {
      if (G.moveAll->moved == 0) say("Nothing is under orders.");
      G.moveAll.reset();
      refreshControls();
      return;
    }
    G.moveAll->seen.insert(lead);
    auto target = *lead->target;
    select(lead->x, lead->y, lead);
    if (G.selection) {
      for (Army* a : G.selection->stack) G.moveAll->seen.insert(a);
      moveSelection(target.first, target.second);
      if (G.moveAll) G.moveAll->moved++;
    }
  }
}

static void moveAll() {
  G.moveAll = App::MoveAll{};
  if (G.selection && !G.selection->stack.empty() && G.selection->stack[0]->target) {
    auto t = *G.selection->stack[0]->target;
    for (Army* a : G.selection->stack) G.moveAll->seen.insert(a);
    moveSelection(t.first, t.second);
    if (G.moveAll) G.moveAll->moved = 1;
  }
  moveAllStep();
}

// Hero > Search (6536:0000) with the selected stack.
static void doSearch() {
  if (!G.selection) return;
  int x = G.selection->x, y = G.selection->y;
  search::open(G.selection->stack);
  reslot(selectableAt(x, y));
  front::stratDirty();
}

// The key of each menu item UDB.DAT lets a button carry (545c:00aa).
static std::string shortcutKey(int n) {
  static const std::map<int, std::string> KEY = {
      {507, "m"}, {508, "q"}, {510, "a"}, {511, "k"}, {512, "g"}, {513, "n"}, {514, "w"}, {516, "="},
      {517, ","}, {518, "f"}, {521, "z"}, {524, "b"}, {525, "c"}, {526, "p"}, {527, "v"}, {528, "."},
      {529, "s"}, {530, "h"}, {531, "e"}, {534, "l"}, {535, "alt E"}};
  auto sc = G.screen->ui.shortcuts.find(n);
  if (sc == G.screen->ui.shortcuts.end()) return "";
  auto k = KEY.find(sc->second);
  return k == KEY.end() ? "" : k->second;
}

static std::map<int, std::function<void()>>& actions() {
  static std::map<int, std::function<void()>> A;
  if (!A.empty()) return A;
  for (int i = 0; i < SLOT_COUNT; i++) {
    A[SLOT_FIRST + i] = [i]() {
      if (!G.selection) return;
      slotsMod::toggle(G.selection->slots, *G.g, i);
      front::afterSlotChange();
    };
    A[BAR_FIRST + i] = [i]() {
      if (!G.selection) return;
      slotsMod::pickGroup(G.selection->slots, *G.g, i);
      front::afterSlotChange();
    };
  }
  A[GRP_ALL] = []() {
    if (!G.selection) return;
    auto& s = G.selection->slots;
    if (slotsMod::grouped(s)) slotsMod::single(s, *G.g);
    else slotsMod::all(s, *G.g);
    front::afterSlotChange();
  };
  A[GRP_NONE] = A[GRP_ALL];
  // The five buttons above the pad, 173-178: walk on, next, done, defend,
  // deselect (8065:0ec9 / 0ed7 / 0ef4 / 0f26).
  A[173] = []() {
    if (!G.selection || G.selection->stack.empty()) return;
    auto t = G.selection->stack[0]->target;
    if (!t) return;
    moveSelection(t->first, t->second);
  };
  A[174] = selectNext;
  A[175] = quitArmy;
  A[176] = fortify;
  A[178] = deselect;
  // The 3x3 pad, 320-327 (8611:0723), clockwise from north: it moves the view.
  static const int PAD[8][2] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}};
  for (int i = 0; i < 8; i++) {
    A[320 + i] = [i]() {
      G.cx += PAD[i][0];
      G.cy += PAD[i][1];
      clampCamera();
    };
  }
  for (int i = 0; i < w2::uidata::SHORTCUT_COUNT; i++) {
    A[w2::uidata::SHORTCUT_FIRST + i] = [i]() {
      std::string k = shortcutKey(i);
      if (!k.empty() && menuEnabled(k)) menuDoes()[k]();
    };
  }
  // the "?" (8065:104e): the mouse's help page, then the keys'
  A[188] = []() {
    help::open("HELP\\HMOUSE.GFX", []() { help::open("HELP\\HKEYS.GFX", nullptr, &help::POPUP4); }, &help::POPUP4);
  };
  // 186 (8065:0f3f) looks at where the stack is going, and back
  A[186] = []() {
    if (!G.selection || G.selection->stack.empty()) return;
    auto& sel = *G.selection;
    auto t = sel.stack[0]->target;
    double cx = G.cx, cy = G.cy;
    if (t && inView(sel.x, sel.y)) {
      centreOn(t->first, t->second);
      if (G.cx == cx && G.cy == cy) centreOn(sel.x, sel.y);
    } else {
      centreOn(sel.x, sel.y);
    }
  };
  // 187 (8065:0fe9) forgets the destination
  A[187] = []() {
    if (!G.selection || G.selection->stack.empty() || !G.selection->stack[0]->target) return;
    for (Army* a : G.selection->stack) a->target.reset();
    G.route.reset();
  };
  // 183-185 are one button in three faces: Diplomatic Action (484e:0346)
  for (int id = 183; id <= 185; id++) A[id] = diplomacyui::action;
  // the pad's centre, 177, shares its handler with Home (8065:0f02)
  A[177] = []() {
    if (G.selection) centreOn(G.selection->x, G.selection->y);
    else centreOn(G.player->capital->x, G.player->capital->y);
  };
  return A;
}

// Which buttons are live (8065:0174, the original's own refresh). A control
// with no handler here is greyed too.
static void refreshControls() {
  auto& st = G.screen->state;
  // under the banner and the hero offer every control is still greyed
  if (G.banner || G.offer) {
    for (auto& [id, s] : st) s = DISABLED;
    return;
  }
  auto& A = actions();
  auto set = [&](int id, bool live) {
    if (!st.count(id)) return;
    if (!(live && A.count(id))) st[id] = DISABLED;
    else if (id == G.pressed) st[id] = ACTIVE;
    else st[id] = NORMAL;
  };
  const Selection* sel = G.selection ? &*G.selection : nullptr;
  bool walkOn = sel && G.route && (int)G.route->path.size() > (G.walk ? (int)G.walk->i + 1 : 0);
  set(173, walkOn);
  bool more = false;
  for (Army* a : G.g->armies)
    if (a->owner == G.player->index && !a->transit && !a->fortified && !a->done) { more = true; break; }
  set(174, more);
  set(175, more && sel);
  set(176, sel);
  set(177, sel);
  set(178, sel);
  set(186, sel);
  set(187, sel && !sel->stack.empty() && sel->stack[0]->target);
  set(188, true);
  for (int i = 0; i < SLOT_COUNT; i++) {
    bool live = sel && i < sel->slots.n;
    set(SLOT_FIRST + i, live);
    set(BAR_FIRST + i, live);
  }
  bool grouped = sel && slotsMod::grouped(sel->slots);
  set(240, sel && !grouped);
  set(241, sel && grouped);
  for (int i = 0; i < w2::uidata::SHORTCUT_COUNT; i++) {
    std::string k = shortcutKey(i);
    set(w2::uidata::SHORTCUT_FIRST + i, !k.empty() && menuEnabled(k));
  }
  for (int i = 0; i < 8; i++) set(320 + i, true);
  int face = diplomacyui::buttonFor(*G.g, G.player->index);
  for (int id = 183; id <= 185; id++) set(id, id == face);
}

// The drag: the map goes the way the mouse does, by exactly as far.
static void mousemoved(double dx, double dy) {
  if (!G.drag) return;
  G.drag->dx += dx;
  G.drag->dy += dy;
  double k = (double)display::d.scale / (TILE * G.zoom);
  G.cx = G.drag->cx - G.drag->dx * k;
  G.cy = G.drag->cy - G.drag->dy * k;
  clampCamera();
}

// The wheel zooms the map about the tile under the pointer.
static void wheelmoved(int wy) {
  syncLayout();
  if (G.starting || !G.g || G.over || kit::top() || G.banner || G.offer || G.victory || wy == 0) return;
  if (G.mapRect.contains(G.mouseX, G.mouseY)) setZoomAt(G.zoom + (wy > 0 ? 1 : -1), G.mouseX, G.mouseY, false);
  else setZoomAt(G.zoom + (wy > 0 ? 1 : -1), 0, 0, true);
}

static void mousereleased(int x, int y, int button) {
  syncLayout();
  if (G.drag) {
    G.drag.reset();
    if (G.pressed == NONE) return;
  }
  if (auto top = kit::topShared()) {
    if (top->mousereleased(x - G.layout.dialog.x, y - G.layout.dialog.y, button)) return;
  }
  int id = G.pressed;
  if (id == NONE) return;
  G.pressed = NONE;
  G.screen->state[id] = NORMAL;
  auto c = screen::controlAt(*G.screen, x, y);
  if (!c || c->id != id) return;          // released off the button
  auto& A = actions();
  auto act = A.find(id);
  if (act != A.end()) act->second();
  if (G.g) refreshControls();
}

// The city nearest the view's centre, then the city dialog in that mode.
static void viewCity(int mode) {
  auto [cx, cy] = front::viewCentre();
  City* best = nullptr;
  int bestD = NONE;
  for (auto& c : G.g->map->cities) {
    bool ok = game::seen(*G.g, G.player->index, c.x, c.y) && (mode == city::INFO || c.ownerIndex == G.player->index);
    if (ok) {
      int d = std::max(std::abs(c.x - cx), std::abs(c.y - cy));
      if (bestD == NONE || d < bestD) { best = &c; bestD = d; }
    }
  }
  if (best) city::open(best, mode);
}

void front::quit() {
  G.modals.clear();
  G.aiRun.reset();
  aiCo = nullptr;
  G.aiStatus.reset();
  G.assault.reset();
  G.walk.reset();
  G.moveAll.reset();
  G.banner.reset();
  G.offer.reset();
  G.victory.reset();
  G.victoryUnder.reset();
  G.over = false;
  front::openStart();
}

// Each menu accelerator, as far as this engine can honour it.
static std::map<std::string, std::function<void()>>& menuDoes() {
  static std::map<std::string, std::function<void()>> M;
  if (!M.empty()) return M;
  M["alt E"] = endTurn;
  M["m"] = moveAll;
  // Game > Quit (7721:0000) and New game (7721:019d) ask first
  M["^Q"] = []() {
    input::Options o;
    o.title = kit::text(0x2c, 0);
    for (int i = 1; i <= 4; i++) o.lines.push_back(kit::text(0x2c, i));
    o.confirm = true;
    o.ok = [](const std::string&) {
      // 7721:0072: "Farewell, Warlord!" -- and the game closes
      advisor::say(w2::cues::QUIT, []() { G.quitRequested = true; });
    };
    input::open(o);
  };
  M["alt N"] = []() {
    input::Options o;
    o.title = kit::text(0x2d, 0);
    for (int i = 1; i <= 4; i++) o.lines.push_back(kit::text(0x2d, i));
    o.confirm = true;
    o.ok = [](const std::string&) {
      G.selection.reset();
      G.banner.reset();
      G.offer.reset();
      front::openStart();
    };
    input::open(o);
  };
  M["alt S"] = savegame::save;
  M["alt L"] = []() {
    savegame::load([](std::unique_ptr<w2::Game> g) {
      if (G.starting) {
        G.modals.clear();
        G.starting = false;
      }
      front::takeLoaded(std::move(g));
    });
  };
  M["z"] = doSearch;
  M["c"] = []() { viewCity(city::INFO); };
  M["b"] = []() { viewCity(city::CITY); };
  M["p"] = []() { viewCity(city::PRODUCTION); };
  M["v"] = []() { viewCity(city::VECTOR); };
  // Hero > Plant Flag (7563:09f7)
  M["f"] = []() {
    if (!G.selection) return;
    for (Army* a : G.selection->stack) {
      if (a->hero() && game::plantFlag(*G.g, *G.player, a)) { front::stratDirty(); return; }
    }
  };
  // Order > Disband (1b62:06bf)
  M["q"] = []() {
    if (!G.selection || G.selection->stack.empty()) return;
    auto stack = G.selection->stack;
    bool heroes = false;
    for (Army* a : stack) if (a->hero()) heroes = true;
    input::Options o;
    o.title = "Disband";                       // 4125:2bf8
    o.confirm = true;
    o.lines = {"Are you sure you", "want to disband this", "group?", heroes ? "It contains heroes" : ""};
    o.ok = [stack](const std::string&) {
      game::disband(*G.g, *G.player, stack);
      G.selection.reset();
      front::stratDirty();
    };
    input::open(o);
  };
  M["i"] = fightorder::open;
  M["r"] = resign::open;
  M["."] = []() {
    auto [cx, cy] = front::viewCentre();
    ruin::open(ruin::nearest(cx, cy));
  };
  M["s"] = stackui::open;
  M["o"] = armybonus::open;
  M["t"] = items::open;
  M["x"] = []() { if (G.selection) signpost::open(G.selection->stack); };
  M["h"] = []() { historyui::open(0); };
  M["e"] = []() { historyui::open(1); };
  M["j"] = []() { historyui::open(2); };
  M["y"] = []() { historyui::open(3); };
  M["alt U"] = shortcuts::open;
  M["alt X"] = []() { settings::open(); };
  M["?"] = about::open;
  M["l"] = historyui::triumphs;
  M["d"] = diplomacyui::open;
  M["="] = questui::open;
  M["u"] = levels::open;
  M[","] = heroinfo::open;
  M["a"] = []() { reports::open(0); };
  M["k"] = []() { reports::open(1); };
  M["g"] = []() { reports::open(2); };
  M["n"] = []() { reports::open(3); };
  M["w"] = []() { reports::open(4); };
  for (int n = 1; n <= 16; n++) {
    M["map zoom " + std::to_string(n)] = [n]() { front::setZoom(n); };
    M["ui scale " + std::to_string(n)] = [n]() { setUIScale(n); };
  }
  M["screen full"] = []() { setScreen(false); };
  M["screen window"] = []() { setScreen(true); };
  M["screen 4:3"] = []() { setOriginalSize(!display::d.wanted); };
  for (const char* s : {"fm", "mt32", "sc55"}) {
    std::string synth = s;
    M["music " + synth] = [synth]() { sound::setSynth(synth); };
  }
  return M;
}

static void menuPick(const std::string& k) {
  auto& M = menuDoes();
  auto it = M.find(k);
  if (it != M.end()) it->second();
}

// What greys a menu item (8065:0519, the table at 4125:1798).
static bool menuEnabled(const std::string& k) {
  auto& M = menuDoes();
  if (k.empty() || !M.count(k)) return false;
  bool music = w2::startsWith(k, "music ");
  if (music && !sound::synthAvailable(k.substr(6))) return false;
  // 7f77:0200: on the start screens every menu is greyed but Quit and Load
  if (G.starting || !G.g) {
    return k == "^Q" || (k == "alt L" && savegame::used() > 0) || w2::startsWith(k, "ui scale ") ||
           w2::startsWith(k, "screen ") || music;
  }
  w2::Game& g = *G.g;
  auto sideHasHero = [&]() {
    for (Army* a : g.armies) if (a->owner == G.player->index && a->hero()) return true;
    return false;
  };
  auto selected = []() { return G.selection && !G.selection->stack.empty(); };
  auto sideHasCities = [&]() {
    for (auto& c : g.map->cities) if (c.ownerIndex == G.player->index && !c.razed) return true;
    return false;
  };
  auto canSearch = [&]() {
    if (!selected()) return false;
    Army* lead = G.selection->stack[0];
    w2::Site* s = g.map->siteAt.empty() ? nullptr : g.map->siteAt[lead->y * g.map->width + lead->x];
    if (!s || s->searched) return false;
    return lead->hero() || s->type == w2::site::TEMPLE;
  };
  auto canPlantFlag = [&]() {
    if (!selected()) return false;
    Army* lead = G.selection->stack[0];
    if (!lead->hero()) return false;
    if (G.player->index >= (int)g.map->items.size()) return false;
    w2::Item* std = &g.map->items[G.player->index];
    if (std::find(lead->items.begin(), lead->items.end(), std) == lead->items.end()) return false;
    int t = w2::scn::terrainAt(*g.map, lead->x, lead->y);
    if (t == move::WATER || t == move::SHORE || t == move::CITY || t == move::SITE) return false;
    for (auto& it : g.map->items) if (it.planted && it.x == lead->x && it.y == lead->y) return false;
    return true;
  };
  if (k == "alt L") return savegame::used() > 0;
  if (k == "q" || k == "s") return selected();
  if (k == "x") return selected() && w2::scn::terrainAt(*g.map, G.selection->stack[0]->x, G.selection->stack[0]->y) == move::TOWER;
  if (k == "r" || k == "b" || k == "p" || k == "v") return sideHasCities();
  if (k == "d") return g.map->options.diplomacy != 0;
  if (k == "," || k == "u") return sideHasHero();
  if (k == "f") return canPlantFlag();
  if (k == "z") return canSearch();
  if (k == "h" || k == "e" || k == "j" || k == "y") return g.turn != 1;
  if (k == "alt E") return !g.won;
  return true;
}
bool front::menuEnabled(const std::string& k) { return ::menuEnabled(k); }

static void keypressed(const std::string& k) {
  if (busyWalking()) {
    if (G.keyBuffer.size() < 15) G.keyBuffer.push_back(k);
    return;
  }
  syncLayout();
  if (computerInput(k)) return;
  if (G.banner) { dismissBanner(); return; }
  if (G.assault) { pressAssault(); return; }
  if (G.victory) {
    // Occupy heads both the default and the cancel lists
    if (k == "return" || k == "kpenter" || k == "escape") takeCity(AS_OCCUPY);
    return;
  }
  if (G.offer) {
    if (k == "return" || k == "kpenter") acceptOffer();
    else if (k == "backspace" && !G.offerName.empty()) G.offerName.pop_back();
    else if (k == "escape" && !G.offer->first) refuseOffer();
    return;
  }
  bool alt = held({"lalt", "ralt"}), ctrl = held({"lctrl", "rctrl", "lgui", "rgui"}), shift = held({"lshift", "rshift"});
  auto top = kit::topShared();
  if (top) {
    if (top->menuBar) {
      std::string accel;
      if (ctrl && k == "q") accel = "^Q";
      else if (alt && k.size() == 1) accel = "alt " + w2::upper(k);
      if (!accel.empty()) {
        G.openMenu = -1;
        if (menuEnabled(accel)) menuDoes()[accel]();
        return;
      }
    }
    if (G.openMenu >= 0) { G.openMenu = -1; return; }
    top->keypressed(k);
    return;
  }
  if (G.openMenu >= 0) { G.openMenu = -1; return; }
  if (G.over || !G.g) return;
  // the main screen's keys (17be:0064, 17be:0444)
  auto press = [](int id) {
    auto& A = actions();
    if (G.screen->stateOf(id) != DISABLED && A.count(id)) A[id]();
    refreshControls();
  };
  std::string digit;
  if (k.size() == 1 && k[0] >= '0' && k[0] <= '9') digit = k;
  else if (k.size() == 3 && w2::startsWith(k, "kp") && k[2] >= '0' && k[2] <= '9') digit = k.substr(2);
  if (k == "return" || k == "kpenter") press(174);
  else if (k == "kp+" || k == "pageup" || k == "=" ) front::setZoom(G.zoom + 1);
  else if (k == "kp-" || k == "pagedown" || k == "-") front::setZoom(G.zoom - 1);
  else if (k == "escape") press(175);
  else if (k == "space") press(240);           // group the stack (89e0:0a55)
  else if (k == "tab") press(186);
  else if (k == "backspace") press(187);
  else if (k == "home") press(177);
  else if (k == "end") deselect();             // 1b62:08b3, as 178 is
  else if (k == "delete") press(173);
  else if (digit == "5") {
    if (G.selection) centreOn(G.selection->x, G.selection->y);
  } else if (!digit.empty()) {
    // 1-9 step the stack the way the numeric pad lies (1c8c:0197)
    static const std::map<char, std::pair<int, int>> STEP = {{'8', {0, -1}}, {'9', {1, -1}}, {'6', {1, 0}}, {'3', {1, 1}},
                                                            {'2', {0, 1}},  {'1', {-1, 1}}, {'4', {-1, 0}}, {'7', {-1, -1}}};
    auto st = STEP.find(digit[0]);
    if (st != STEP.end() && G.selection) {
      for (Army* a : G.selection->stack) a->target.reset();
      moveSelection(G.selection->x + st->second.first, G.selection->y + st->second.second);
    }
  } else if (k == "up") press(320);
  else if (k == "right") press(322);
  else if (k == "down") press(324);
  else if (k == "left") press(326);
  else if (k == "f5") {
    // not the original's: quick save and load
    if (savegame::writeSlot("quick", *G.g)) say("Saved.");
    else say("Could not save.");
  } else if (k == "f9") {
    if (auto loaded = savegame::readSlot("quick")) front::takeLoaded(std::move(loaded));
  } else {
    std::string accel;
    if (ctrl && k == "q") accel = "^Q";
    else if (alt && k.size() == 1) accel = "alt " + w2::upper(k);
    else if (shift && k == "/") accel = "?";
    else if (k.size() == 1) accel = k;
    auto& M = menuDoes();
    if (!accel.empty() && M.count(accel)) M[accel]();
  }
}

// Typing into the hero's name field, or a dialog's.
static void textinput(const std::string& text) {
  auto top = kit::topShared();
  if (top && !G.offer) {
    top->textinput(text);
    return;
  }
  if (!G.offer) return;
  static const std::regex ok("^[A-Za-z0-9\\s'\\-.]+$");
  if (G.offerName.size() < HERO_NAME_MAX && std::regex_match(text, ok)) G.offerName += text;
}

// ------------------------------------------------------------------ starting

// A fresh game: its first turn, the view on the capital.
static void beginGame() {
  G.player = game::begin(*G.g);
  if (!G.player) return;
  G.cursor = G.player->capital ? std::make_pair(G.player->capital->x, G.player->capital->y) : std::make_pair(0, 0);
  G.selection.reset();
  centreOn(G.player->capital->x, G.player->capital->y);
  // the banner is a human's (8cc6:0259)
  if (!G.player->computer) showBanner(G.player);
}

void front::openStart() {
  G.starting = true;
  G.haveLayout = false;
  sound::music(w2::cues::TITLE);
  start::open(
      [](const std::string& dir, const w2::game::NewGameOptions& opts) {
        // 7bab:0cfe: the war begins to its own music, and the advisor says so
        sound::music(w2::cues::BEGIN);
        advisor::say(w2::cues::BEGIN_WAR, [dir, opts]() {
          G.starting = false;
          G.haveLayout = false;
          G.scenario = dir;
          w2::game::NewGameOptions o = opts;
          o.seed = (double)(time(nullptr) % 1000000007);
          G.seed = o.seed;
          G.g = game::newGame(G.dataDir, dir, o);
          G.selection.reset();
          G.over = false;
          front::stratDirty();
          syncLayout();
          beginGame();
          if (G.player && G.player->computer) {
            // a game with no human in it says so as it begins (8065:00f1)
            bool human = false;
            for (Side* s : G.g->sides) if (!s->computer) human = true;
            if (human) playComputers(G.player);
            else search::message(kit::text(0xe, 0), kit::text(0xe, 1), []() { playComputers(G.player); });
          }
        });
      },
      [](std::unique_ptr<w2::Game> g) {
        G.starting = false;
        G.haveLayout = false;
        syncLayout();
        front::takeLoaded(std::move(g));
      });
}

// ------------------------------------------------------------------ scripts

// A script drives the game for testing: one command a line, the coordinates
// UI pixels.
//   wait N          let N frames go by
//   key NAME        press and release a key, by LÖVE's name for it
//   hold NAME / release NAME
//   click X Y [B]   press and release a mouse button (1 left, 2 right)
//   move X Y        move the pointer
//   text STRING     type
//   shot FILE       write the frame as a .bmp
//   eval            print the game's state (turn, side, modals)
//   select TX TY    pick up the stack on a tile
//   attack TX TY    send the selection at a tile, fighting if it must
//   near            list the cities nearest the selection not the player's
//   quit
struct Script {
  std::vector<std::string> lines;
  size_t at = 0;
  int wait = 0;
};
static std::optional<Script> script;

static void saveShot(const std::string& file) {
  int w, h;
  auto px = gfx::readPixels(w, h);
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(px.data(), w, h, 32, w * 4, SDL_PIXELFORMAT_ABGR8888);
  if (s) {
    SDL_SaveBMP(s, file.c_str());
    SDL_FreeSurface(s);
  }
}

static void stepScript() {
  if (!script) return;
  if (script->wait > 0) { script->wait--; return; }
  while (script->at < script->lines.size()) {
    std::istringstream in(script->lines[script->at++]);
    std::string cmd;
    in >> cmd;
    if (cmd.empty() || cmd[0] == '#') continue;
    if (cmd == "wait") {
      in >> script->wait;
      return;
    } else if (cmd == "key") {
      std::string k;
      in >> k;
      keys::press(k);
      keypressed(k);
      keys::release(k);
    } else if (cmd == "hold") {
      std::string k;
      in >> k;
      keys::press(k);
    } else if (cmd == "release") {
      std::string k;
      in >> k;
      keys::release(k);
    } else if (cmd == "click") {
      int x, y, b = 1;
      in >> x >> y;
      if (!(in >> b)) b = 1;
      G.mouseX = x;
      G.mouseY = y;
      display::d.mouseInside = true;
      mousepressed(x, y, b);
      mousereleased(x, y, b);
    } else if (cmd == "move") {
      in >> G.mouseX >> G.mouseY;
      display::d.mouseInside = true;
    } else if (cmd == "text") {
      std::string t;
      std::getline(in, t);
      textinput(w2::trim(t));
    } else if (cmd == "shot") {
      std::string f;
      in >> f;
      saveShot(f);
    } else if (cmd == "eval") {
      printf("turn=%d player=%s modals=%d banner=%d offer=%d ai=%d over=%d walk=%d assault=%d status=%s\n",
             G.g ? G.g->turn : 0, G.player ? G.player->name.c_str() : "-", (int)G.modals.size(), (bool)G.banner,
             (bool)G.offer, (bool)G.aiRun, G.over, (bool)G.walk, (bool)G.assault, G.aiStatus ? G.aiStatus->text.c_str() : "");
      if (G.g && G.player && G.player->capital)
        printf("  cam=%.2f,%.2f zoom=%d map=%d,%d,%d,%d capital=%d,%d\n", G.cx, G.cy, G.zoom, G.mapRect.x, G.mapRect.y,
               G.mapRect.w, G.mapRect.h, G.player->capital->x, G.player->capital->y);
      fflush(stdout);
    } else if (cmd == "select") {
      int x, y;
      in >> x >> y;
      select(x, y);
    } else if (cmd == "attack") {
      int x, y;
      in >> x >> y;
      moveSelection(x, y, true);
    } else if (cmd == "near" && G.g && G.selection) {
      std::vector<std::pair<int, City*>> found;
      for (auto& c : G.g->map->cities) {
        if (c.ownerIndex != G.player->index && !c.razed)
          found.push_back({std::max(std::abs(c.x - G.selection->x), std::abs(c.y - G.selection->y)), &c});
      }
      std::sort(found.begin(), found.end(), [](auto& a, auto& b) { return a.first < b.first; });
      for (size_t i = 0; i < found.size() && i < 3; i++)
        printf("  %s at %d,%d owner %d, %d away\n", found[i].second->name.c_str(), found[i].second->x, found[i].second->y,
               found[i].second->ownerIndex, found[i].first);
      fflush(stdout);
    } else if (cmd == "quit") {
      G.quitRequested = true;
      return;
    }
  }
}

// ------------------------------------------------------------------ main

int main(int argc, char** argv) {
  std::string scenario, scriptFile;
  double seed = 0;
  bool windowed = prefs::get("screen") == "window";
  G.dataDir = "original";
  for (int i = 1; i < argc; i++) {
    std::string a = argv[i];
    auto next = [&]() { return i + 1 < argc ? std::string(argv[++i]) : std::string(); };
    if (a == "--data") G.dataDir = next();
    else if (a == "--scenario") scenario = w2::upper(next());
    else if (a == "--seed") seed = atof(next().c_str());
    else if (a == "--window") windowed = true;
    else if (a == "--fullscreen") windowed = false;
    else if (a == "--script") scriptFile = next();
  }
  if (!w2::fileExists(G.dataDir + "/TERRAIN0/ARMYTYPE.DAT")) {
    fprintf(stderr, "cannot find the game's files in '%s': run from the repository root, or pass --data DIR\n", G.dataDir.c_str());
    return 1;
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    fprintf(stderr, "SDL: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Window* win = display::open("Warlords II", windowed);
  if (!win) {
    fprintf(stderr, "SDL window: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!ren) ren = SDL_CreateRenderer(win, -1, 0);
  if (!ren) {
    fprintf(stderr, "SDL renderer: %s\n", SDL_GetError());
    return 1;
  }
  gfx::init(ren);
  SDL_ShowCursor(SDL_DISABLE);
  if (!scriptFile.empty()) {
    Script s;
    std::ifstream in(scriptFile);
    std::string line;
    while (std::getline(in, line)) s.lines.push_back(line);
    script = s;
  }
  try {
    loadArt();
    installHooks();
    auto ui = w2::uidata::load(G.dataDir);
    sound::init(G.dataDir, ui.files);
    // the screen: its real pixels, the UI's scale, and the map's zoom
    if (prefs::get("4:3") == "on") display::d.wanted = std::make_pair(layout::W, layout::H);
    display::d.chosen = prefs::getInt("scale", 0);
    display::measure();
    G.zoom = prefs::getInt("zoom", display::d.scale);
    if (scenario.empty()) {
      front::openStart();
      // 7f77:0000 greets the player the first time round
      advisor::say(w2::cues::GREET);
    } else {
      G.starting = false;
      G.scenario = scenario;
      game::NewGameOptions o;
      o.seed = seed != 0 ? seed : (double)(time(nullptr) % 1000000007);
      if (scenario == w2::randommap::DIR) {
        // RANDOM: a random world with the start menu's first settings, made
        // from the seed, in place of the last one the original left on disk
        w2::Rng r(o.seed);
        w2::randommap::Options ro;
        ro.dataDir = G.dataDir;
        ro.rng = &r;
        w2::randommap::install(G.dataDir, w2::randommap::generateNow(ro));
      }
      G.g = game::newGame(G.dataDir, scenario, o);
      for (size_t i = 0; i < G.g->sides.size(); i++) G.g->sides[i]->computer = i > 0;
      syncLayout();
      beginGame();
    }
  } catch (const std::exception& e) {
    fprintf(stderr, "could not start: %s\n", e.what());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Warlords II", e.what(), win);
    return 1;
  }

  double lastX = 0, lastY = 0;
  {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    lastX = mx;
    lastY = my;
    display::d.mouseInside = (SDL_GetWindowFlags(win) & SDL_WINDOW_MOUSE_FOCUS) != 0;
    std::tie(G.mouseX, G.mouseY) = display::toUI(mx, my);
  }
  while (!G.quitRequested) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      try {
        switch (e.type) {
          case SDL_QUIT: G.quitRequested = true; break;
          case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || e.window.event == SDL_WINDOWEVENT_RESIZED) {
              display::measure();
              G.haveLayout = false;
            } else if (e.window.event == SDL_WINDOWEVENT_LEAVE) {
              display::d.mouseInside = false;
            } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
              keys::clear();
            }
            break;
          case SDL_KEYDOWN: {
            std::string k = keys::loveName(e.key.keysym.sym);
            if (k.empty()) break;
            // the Lua remake leaves LÖVE's key repeat off
            if (e.key.repeat) break;
            keys::press(k);
            keypressed(k);
            break;
          }
          case SDL_KEYUP: {
            std::string k = keys::loveName(e.key.keysym.sym);
            if (!k.empty()) keys::release(k);
            break;
          }
          case SDL_TEXTINPUT:
            if (!keys::isDown({"lalt", "ralt", "lctrl", "rctrl", "lgui", "rgui"})) textinput(w2::utf8ToLatin1(e.text.text));
            break;
          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEBUTTONUP: {
            auto [x, y] = display::toUI(e.button.x, e.button.y);
            G.mouseX = x;
            G.mouseY = y;
            int b = e.button.button == SDL_BUTTON_RIGHT ? 2 : e.button.button == SDL_BUTTON_MIDDLE ? 3 : 1;
            if (e.type == SDL_MOUSEBUTTONDOWN) mousepressed(x, y, b);
            else mousereleased(x, y, b);
            break;
          }
          case SDL_MOUSEMOTION: {
            auto [x, y] = display::toUI(e.motion.x, e.motion.y);
            G.mouseX = x;
            G.mouseY = y;
            display::d.mouseInside = display::onFrame(e.motion.x, e.motion.y);
            double k = display::d.dpi / display::d.scale;
            mousemoved((e.motion.x - lastX) * k, (e.motion.y - lastY) * k);
            lastX = e.motion.x;
            lastY = e.motion.y;
            break;
          }
          case SDL_MOUSEWHEEL:
            if (e.wheel.y != 0) wheelmoved(e.wheel.y > 0 ? 1 : -1);
            break;
        }
      } catch (const std::exception& ex) {
        fprintf(stderr, "error: %s\n", ex.what());
      }
    }
    try {
      display::measure();
      stepScript();
      update();
      draw();
    } catch (const std::exception& ex) {
      fprintf(stderr, "error: %s\n", ex.what());
    }
    SDL_RenderPresent(ren);
  }
  G.aiRun.reset();
  G.modals.clear();
  sound::shutdown();
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
