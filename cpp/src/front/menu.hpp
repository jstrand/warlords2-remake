// The original's menu bar.
//
// The menu lives in WARLORD2.EXE's data segment, not in a data file, so it is
// written out here (docs/re/ui.md > The menu). Layout follows 7ae8:0052 and
// 2372:049b; nothing is hard-coded, so the bar measures itself for its font.
#pragma once

#include <string>
#include <vector>

#include "front/font.hpp"

namespace menu {

constexpr int BAR_X = 8, BAR_Y = 1;
// The bar is 17 pixels deep and white (7ae8:02d8).
constexpr int BAR_H = 17;

/** "-" is a separator. The key is the accelerator, and is what the front end
 *  dispatches on; `act` is what an item with no accelerator does. */
struct Item {
  std::string label, key, act;
};
struct Menu {
  std::string title;
  std::vector<Item> items;
};
extern const std::vector<Menu> MENUS;

/** The menus with the screen's zooms added to View, and the music's
 *  synthesizer to Game -- not the original's. */
std::vector<Menu> withZooms(int maxUI, int maxMap);

struct Row {
  std::string label, key, act;
  int x = 0, y = 0, w = 0, h = 0;
};
struct Drop {
  int x = 0, y = 0, w = 0, h = 0, keyCol = 0;
  std::vector<Row> rows;
};
struct Title {
  std::string title;
  int x = 0, w = 0;
  Drop drop;
};
using Layout = std::vector<Title>;

/** Measure the bar and every dropdown for a font (7ae8:0052, 2372:049b). */
Layout layout(const Font& font, int screenWidth, const std::vector<Menu>& menus);
/** Which menu title is at this point (an index from 0), or -1. */
int titleAt(const Layout& lay, int x, int y);
/** Which row of an open menu is at this point, or null. */
const Row* rowAt(const Layout& lay, int index, int x, int y);

}  // namespace menu
