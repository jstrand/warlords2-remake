// The original's screen layout, read from its own data files.
//
// docs/formats/screens.md. DATA/JOIN.DAT names, for each dialog, one group
// of controls in BUTTON.DAT and one set of clickable regions in AREA.DAT.
// DATA/FILE.DAT group 3 turns a control's bitmap id into a .pck file name.
#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

namespace w2::uidata {

using Strings = std::vector<std::vector<std::string>>;

/** A file in STRING.DAT's layout: groups of strings, both from 0
 *  (docs/formats/string.md). */
Strings strings(const std::string& path);

struct Join { int button = 0, area = 0; };
struct Region { int id = 0, x = 0, y = 0, w = 0, h = 0; };
struct Area { int id = 0, enabled = 0; std::vector<Region> regions; };
struct Point { int x = 0, y = 0; };
// A control's three source rects are indexed by its state: 1 is the resting
// look, 0 the lit one and 2 the disabled one.
constexpr int ACTIVE = 0, NORMAL = 1, DISABLED = 2;
struct Control {
  int id = 0, x = 0, y = 0, w = 0, h = 0;
  std::array<Point, 3> src;
  int bitmap = 0;
};
struct ButtonGroup { int id = 0; std::vector<Control> controls; };
struct ShortcutItem {
  std::string name;
  int w = 0, h = 0;
  std::array<Point, 3> src;   // lit, resting, greyed
};

std::map<int, Join> joins(const std::string& path);
std::map<int, Area> areas(const std::string& path);
std::map<int, ButtonGroup> buttons(const std::string& path);
/** UDB/UDB.DAT: the menu items that may be put on the four configurable
 *  buttons. */
std::map<int, ShortcutItem> shortcutItems(const std::string& path);
/** UDB/UDB.CUR: which menu item sits on each of the four buttons. */
std::map<int, int> shortcuts(const std::string& path);

// The four configurable buttons, in order (545c:0072).
constexpr int SHORTCUT_FIRST = 179, SHORTCUT_COUNT = 4;
// Their art is bitmap 43, MENUBUTT.PCK, at the rect the item carries.
constexpr int SHORTCUT_BITMAP = 43;

struct UI {
  std::map<int, int> shortcuts;
  std::map<int, ShortcutItem> shortcutItems;
  std::map<int, std::string> shortcutNames;
  std::map<int, Join> joins;
  std::map<int, Area> areas;
  std::map<int, ButtonGroup> buttons;
  std::map<int, std::string> bitmaps;   // lower-cased file names
  Strings files;                        // DATA/FILE.DAT
  // DATA/STRING.DAT: the game's whole text corpus; get_string(group, i) in
  // the executable is uidata::text(ui, group, i) here, both from 0.
  Strings text;
};

/** Load every layout file under `dataDir`. */
UI load(const std::string& dataDir);
/** One string, numbered as the executable numbers them (both from 0). */
std::string text(const UI& ui, int group, int index);

struct Dialog {
  int id = 0;
  std::vector<Control> controls;
  std::vector<Region> regions;
  int enabled = 0;
  bool found = false;
};
/** The controls and regions of one dialog, following JOIN.DAT. */
Dialog dialog(const UI& ui, int id);

constexpr int MAIN_SCREEN = 0;

}  // namespace w2::uidata
