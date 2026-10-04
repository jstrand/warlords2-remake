#include "front/menu.hpp"

#include <algorithm>

namespace menu {

const std::vector<Menu> MENUS = {
    {"SSG", {{"About Warlords II", "", "?"}}},
    {"Game", {{"Settings", "alt X", ""}, {"Shortcuts", "alt U", ""}, {"-", "", ""},
              {"New game", "alt N", ""}, {"Save game", "alt S", ""}, {"Load game", "alt L", ""},
              {"-", "", ""}, {"Save map", "alt M", ""}, {"Load map", "alt Z", ""}, {"-", "", ""},
              {"Quit", "^Q", ""}}},
    {"Order", {{"Fight Order", "i", ""}, {"Move All", "m", ""}, {"Disband", "q", ""},
               {"Signpost", "x", ""}, {"-", "", ""}, {"Resign", "r", ""}}},
    {"Report", {{"Army", "a", ""}, {"City", "k", ""}, {"Gold", "g", ""}, {"Production", "n", ""},
                {"Winning", "w", ""}, {"-", "", ""}, {"Diplomacy", "d", ""}, {"-", "", ""}, {"Quest", "=", ""}}},
    {"Hero", {{"Inspect", ",", ""}, {"Plant Flag", "f", ""}, {"Levels", "u", ""}, {"Search", "z", ""}}},
    {"View", {{"Army Bonus", "o", ""}, {"Items", "t", ""}, {"-", "", ""}, {"Build", "b", ""},
              {"Cities", "c", ""}, {"Production", "p", ""}, {"Vectoring", "v", ""}, {"Ruins", ".", ""},
              {"Stack", "s", ""}}},
    {"History", {{"City", "h", ""}, {"Events", "e", ""}, {"Gold", "j", ""}, {"Winners", "y", ""},
                 {"-", "", ""}, {"Triumphs", "l", ""}}},
    {"Turn", {{"End Turn", "alt E", ""}}},
};

std::vector<Menu> withZooms(int maxUI, int maxMap) {
  std::vector<Menu> out;
  for (const Menu& m : MENUS) {
    Menu n{m.title, {}};
    if (m.title == "Game") {
      for (size_t j = 0; j < m.items.size(); j++) {
        n.items.push_back(m.items[j]);
        if (j == 1) {
          n.items.push_back({"-", "", ""});
          n.items.push_back({"AdLib music", "", "music fm"});
          n.items.push_back({"MT-32 music", "", "music mt32"});
          n.items.push_back({"SC-55 music", "", "music sc55"});
        }
      }
    } else if (m.title == "View") {
      n.items = m.items;
      n.items.push_back({"-", "", ""});
      for (int z = 1; z <= maxMap; z++) n.items.push_back({"Map " + std::to_string(z) + "x", "", "map zoom " + std::to_string(z)});
      n.items.push_back({"-", "", ""});
      for (int s = 1; s <= maxUI; s++) n.items.push_back({"Interface " + std::to_string(s) + "x", "", "ui scale " + std::to_string(s)});
      n.items.push_back({"-", "", ""});
      n.items.push_back({"Full screen", "", "screen full"});
      n.items.push_back({"Window", "", "screen window"});
      n.items.push_back({"-", "", ""});
      n.items.push_back({"4:3 (original)", "", "screen 4:3"});
    } else {
      n.items = m.items;
    }
    out.push_back(n);
  }
  return out;
}

Layout layout(const Font& font, int screenWidth, const std::vector<Menu>& menus) {
  Layout out;
  int x = BAR_X;
  int lh = font.lineHeight;
  for (const Menu& m : menus) {
    int tw = (font.width(m.title) + 7) / 8 * 8;
    int w = 0, keyCol = 0, keyW = 0;
    for (const Item& it : m.items) {
      if (it.label != "-") {
        int lw = 12 + font.width(it.label);
        if (!it.key.empty()) {
          keyCol = std::max(keyCol, lw + 5);
          keyW = std::max(keyW, font.width(it.key));
          lw = keyCol + keyW;
        }
        w = std::max(w, lw + 5);
      }
    }
    int y = BAR_H + 1;
    Title t;
    t.title = m.title;
    t.x = x;
    t.w = tw + 8;
    int at = 2;
    for (const Item& it : m.items) {
      int h = it.label == "-" ? 2 : lh + 2;
      t.drop.rows.push_back(Row{it.label, it.key, it.act, x, y + at, w, h});
      at += h;
    }
    int dropX = std::min(x, screenWidth - w);
    for (Row& r : t.drop.rows) r.x = dropX;
    t.drop.x = dropX;
    t.drop.y = y;
    t.drop.w = w;
    t.drop.h = at + 1;
    t.drop.keyCol = keyCol;
    out.push_back(t);
    x += tw + 16;
  }
  return out;
}

int titleAt(const Layout& lay, int x, int y) {
  if (y < 0 || y >= BAR_H) return -1;
  for (size_t i = 0; i < lay.size(); i++) {
    if (x >= lay[i].x && x < lay[i].x + lay[i].w) return (int)i;
  }
  return -1;
}

const Row* rowAt(const Layout& lay, int index, int x, int y) {
  if (index < 0 || index >= (int)lay.size()) return nullptr;
  for (const Row& r : lay[index].drop.rows) {
    if (r.label != "-" && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return &r;
  }
  return nullptr;
}

}  // namespace menu
