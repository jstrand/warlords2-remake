// The front end's state, shared with the dialogs: the Lua remake's G.
//
// main.cpp is the main screen (love2d/main.lua); the dialogs live in ui/ and
// reach the screen through the functions declared here.
#pragma once

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "front/font.hpp"
#include "front/layout.hpp"
#include "front/menu.hpp"
#include "front/screen.hpp"
#include "platform/coroutine.hpp"
#include "platform/gfx.hpp"
#include "warlords/combat.hpp"
#include "warlords/hero.hpp"
#include "warlords/move.hpp"
#include "warlords/pal.hpp"
#include "warlords/slots.hpp"
#include "warlords/types.hpp"

namespace kit {
struct Modal;
}

struct Selection {
  int x = 0, y = 0;
  w2::slots::Slots slots;
  std::vector<w2::Army*> stack;
};

struct Walk {
  std::set<w2::Army*> armies;
  std::vector<std::pair<int, int>> tiles;
  size_t i = 0;
  double at = 0;
  w2::Army* top = nullptr;
  bool computer = false;
};

struct Banner {
  std::string name;
  int turn = 0, colour = 15, edge = 0;
};

struct Fell {
  double at = 0, burn = 0;
};

struct Assault {
  int x = 0, y = 0;
  w2::Battle result;
  std::vector<w2::Army*> def, atk;
  std::vector<std::pair<int, int>> defSlots, atkSlots;
  int defSide = 8, atkSide = 8;
  size_t step = 0;
  int defDown = 0, atkDown = 0;
  std::vector<Fell> defFell, atkFell;
  std::string phase = "cloud";
  double at = 0, cloudTime = 0, closeAt = 0;
  bool fast = false, computer = false, window = false;
  std::string victor, outcome;
  std::vector<std::string> message;
  std::function<void()> after;
};

struct Victory {
  w2::City* city = nullptr;
  std::string who, where;
};

struct AiStatus {
  w2::Side* side = nullptr;
  std::string text;
  int progress = 0;
};

struct Drag {
  double cx = 0, cy = 0, dx = 0, dy = 0;
};

struct App {
  std::vector<std::shared_ptr<kit::Modal>> modals;
  int mouseX = -1, mouseY = -1;
  w2::pal::Palette palette;
  std::string dataDir;

  // the art
  std::array<gfx::ImageP, 2> sheets;
  gfx::ImageP roadImg, shadowImg, marble, bigArmy, cityPic, warPic, shieldImg, victoryPic, heroPicM, heroPicF;
  std::array<gfx::ImageP, 9> armyImg;
  gfx::ImageP atransShields, fogImg, cursImg, abits, pointerImg;
  gfx::ImageP shieldsImg, cityBack, searchPic, templePic, scrollPic;
  std::map<int, gfx::ImageP> moveBars;
  std::unique_ptr<screen::Screen> screen;
  FontP font, bigFont, titleFont;

  // the screen
  int openMenu = -1;
  layout::Layout layout;
  bool haveLayout = false;
  layout::Rect mapRect, stratRect, barRect, menuRect, panelRect;
  menu::Layout menuLayout;
  int zoom = 1;
  double cx = 0, cy = 0;
  std::array<int, 4> view{};   // the tiles drawn this frame: x0, y0, x1, y1
  gfx::ImageP stratImage;

  // the game
  bool starting = false;
  std::string scenario = "ERYTHEA";
  double seed = 0;
  std::unique_ptr<w2::Game> g;
  w2::Side* player = nullptr;
  std::optional<std::pair<int, int>> cursor;   // where the army cycle last stopped
  std::optional<Selection> selection;
  std::optional<w2::move::Preview> route;
  std::optional<Walk> walk;

  // the turn's openings
  std::optional<Banner> banner;
  std::shared_ptr<w2::HeroOffer> offer;
  bool offerFemale = false;
  std::string offerName;
  std::optional<screen::View> heroView;
  bool centreAfterBanner = false;

  // battles
  std::optional<Assault> assault;
  std::optional<Victory> victory, victoryUnder;
  std::optional<screen::View> victoryView;

  // the computer's turn
  std::unique_ptr<Coroutine> aiRun;
  bool aiWait = false, aiSkip = false, aiModsHeld = false;
  w2::Side* aiSide = nullptr;
  std::optional<AiStatus> aiStatus;
  double aiResumeAt = 0;

  bool over = false;
  std::optional<Drag> drag;
  int pressed = w2::NONE;
  struct MoveAll {
    std::set<w2::Army*> seen;
    int moved = 0;
  };
  std::optional<MoveAll> moveAll;
  std::vector<std::string> keyBuffer;
  std::string status;
  bool quitRequested = false;
};
extern App G;

// What the dialogs ask of the main screen.
namespace front {
constexpr int NONE = w2::NONE;
void stratDirty();
/** The strategic map as the original paints it anywhere it appears, every
 *  city it can see as an 8x8 shield (834b:0ed7). `mark` gets a white box;
 *  `owners` is who held each city, for History (834b:12d3). */
void drawStrategicMap(int x, int y, const w2::City* mark = nullptr, bool noCities = false,
                      const std::vector<int>* owners = nullptr);
/** Every site shown to the side, marked on a strategic map (834b:05e4). */
void drawSiteMarkers(int x, int y, const w2::Site* highlight = nullptr);
/** The map as the city dialog's Vector mode shows it. `filter` hides the
 *  cities that could take no more vectors (NONE for no filter). */
void drawVectorMap(int x, int y, w2::City* city, int filter, bool seeAll);
/** A hero's figure on the strategic map (834b:1f5f). */
void drawHeroFigure(int x, int y, int tx, int ty);
void drawStrategicPanel(int x, int y, const w2::City* city);
void openCity(w2::City* city);
void openQuest();
void quit();
void openStart();
void takeLoaded(std::unique_ptr<w2::Game> g);
void startAssault(int x, int y, const w2::Battle& result);
void closeAssault();
void presentVictory(w2::City* city, const std::string& victor);
void centreOn(int x, int y);
void setZoom(int z);
void selectAt(int x, int y);
void afterSlotChange();
std::pair<int, int> viewCentre();
std::pair<double, double> mapToUI(double tx, double ty);
bool menuEnabled(const std::string& k);
void playComputer();
void syncLayout();
void say(const std::string& s);
/** A random roll for what does not touch the game's dice: 1..n. */
int roll(int n);
}  // namespace front
