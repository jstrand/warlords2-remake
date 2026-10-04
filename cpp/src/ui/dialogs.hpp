// Every dialog's way in. Each lives in a file of its own (love2d/ui/*.lua).
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ui/kit.hpp"
#include "warlords/game.hpp"
#include "warlords/quest.hpp"
#include "warlords/types.hpp"

namespace input {
struct Options {
  std::string title;
  std::vector<std::string> lines;
  std::string text;
  int maxChars = 15, maxWidth = 232;
  bool cancel = true;     // false greys Cancel
  bool confirm = false;   // a yes-or-no question: no field
  std::function<void(const std::string&)> ok;
  std::function<void()> cancelled;
};
/** Ask for a line of text, or a yes-or-no question (7b4c:0000). */
void open(const Options& opts);
/** A line being typed into a field (7b4c:03a6). */
struct Editor {
  std::string text;
  int maxChars = 15, maxWidth = 232;
  /** "keep" for Enter, "undo" for Escape, "" otherwise. */
  std::string key(const std::string& k);
  void input(const std::string& t);
  void drawCursor(double x, double y) const;
};
}  // namespace input

namespace search {
/** A message box (8065:1160); `after` runs when it is closed. */
void message(const std::string& line1, const std::string& line2, kit::Done after = nullptr);
/** A one-line message (8065:10fb). */
void say(const std::string& line, kit::Done after = nullptr);
/** Hero > Search with the selected stack. */
void open(const std::vector<w2::Army*>& stack);
}  // namespace search

namespace choose {
/** The game's list chooser (796c:0000): pick one of `names`, entry `start`
 *  chosen; `after(index or NONE)` runs when it closes. */
void open(const std::string& title, const std::vector<std::string>& names, int start, std::function<void(int)> after);
}

namespace help {
extern const kit::Rect POPUP4;
/** Show a help file -- "HELP\HITEM.GFX" -- in popup `R` (or the one its #D
 *  picks); false when there is none. */
bool open(const std::string& name, kit::Done after = nullptr, const kit::Rect* R = nullptr);
}

namespace infobox {
struct Board {
  int bitmap;
  const char* file;
  int w, h, key;
};
extern const Board LINES, ARMY, POPUP2;
/** A board, loaded once and keyed on its mask colour. */
gfx::ImageP board(const Board& which);
/** Where a board goes for a point on the screen (740d:131a), in the dialogs'
 *  frame. */
std::pair<int, int> place(const Board& which, int sx, int sy);
/** Two lines (740d:1201 and 740d:1158). */
void lines(int sx, int sy, const std::string& title, const std::string& text);
/** An army (ui_army_info, 740d:0626). */
void army(int sx, int sy, const w2::Army* a);
/** An army type (ui_army_type_info, 740d:032a). */
void armyType(int sx, int sy, int typeId, const w2::Slot& s, int side = w2::NONE);
/** The help for control `id` at a point on the screen, asking `d` about
 *  the controls that stand for something. True when a box went up. */
bool control(int sx, int sy, int id, kit::Modal* d = nullptr);
/** The control on a dialog's view under a point, disabled ones included. */
const w2::uidata::Control* controlAt(const screen::View& v, int x, int y, const std::set<int>* hidden = nullptr);
}  // namespace infobox

namespace advisor {
/** Have him say a FILE.DAT group, and run `after` once he has finished. */
void say(int group, kit::Done after = nullptr);
}

namespace tutorial {
/** Show the page for `moment` once, if the tutorial is on. */
void show(const std::string& moment, kit::Done after = nullptr);
void chain(const std::vector<std::string>& moments, kit::Done after = nullptr);
}

namespace about { void open(); }
namespace spoils {
void open(bool sacked, w2::City* city, int gold, const std::vector<std::pair<int, int>>& lost, int left, kit::Done after);
}
namespace resign { void open(); }
namespace items { void open(); }
namespace levels { void open(); }
namespace miladvisor { void open(const std::vector<w2::Army*>* stack, int tx, int ty); }
namespace signpost { void open(const std::vector<w2::Army*>& stack); }
namespace questnews {
void show(std::shared_ptr<w2::QuestResult> news, kit::Done after = nullptr);
/** A quest that ended is told once the screen is the player's again. */
void poll();
}
namespace questui { void open(); }
namespace stackui { void open(); }
namespace armybonus {
void open();
/** 89e0:1a07: the first of the type's bonuses, as group 163 words it. */
std::string bonusText(const w2::ArmyType& t, const w2::Army* army = nullptr);
}
namespace city {
constexpr int INFO = 0, CITY = 1, PRODUCTION = 2, VECTOR = 3;
/** Production for one of the side's own by default, Info for any other. */
void open(w2::City* c, int mode = w2::NONE);
}
namespace buyprod { void open(w2::City* c, kit::Done after = nullptr); }
namespace fightorder { void open(); }
namespace ending {
/** The game's end, or nearly: told, then `after`. */
void show(w2::Ending& e, kit::Done after);
void over(const w2::Ending& e);
}
namespace diplomacyui {
void open();
/** Diplomatic Action, the shield button. */
void action();
/** Which face the shield button shows: 183, 184 or 185. */
int buttonFor(w2::Game& g, int side);
}
namespace heroinfo { void open(); }
namespace historyui {
void open(int which);
void triumphs();
}
namespace reports { void open(int which); }
namespace ruin {
w2::Site* nearest(int x, int y);
void open(w2::Site* s);
}
namespace savegame {
void save();
void load(std::function<void(std::unique_ptr<w2::Game>)> after);
int used();
bool writeSlot(const std::string& key, const w2::Game& g);
std::unique_ptr<w2::Game> readSlot(const std::string& key);
}
namespace settings {
/** `after` runs when OK is pressed. */
void open(kit::Done after = nullptr);
}
namespace shortcuts { void open(); }
namespace tileinfo { void open(int tx, int ty); }
namespace start {
/** The start screens (7f77:0000): a new game set up there, or a saved one. */
void open(std::function<void(const std::string& dir, const w2::game::NewGameOptions& opts)> begin,
          std::function<void(std::unique_ptr<w2::Game>)> loaded);
}
