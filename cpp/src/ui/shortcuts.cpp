// Game > Shortcuts (545c:0000): which menu items the four configurable
// buttons carry.
//
// Popup 0, (80, 60) 480x320, dialog 14 (545c:014a): the 21 items of UDB.DAT
// three to a row, each its button and its name, lit when it is on the slot
// being set; and the four slots, the one being set outlined in colour 9.
// 316-319 pick a slot, 295-315 put an item on it, OK (294) keeps them, in
// the remake's own settings.
#include "front/prefs.hpp"
#include "ui/dialogs.hpp"
#include "util/json.hpp"
#include "util/util.hpp"
#include "warlords/bytes.hpp"

namespace shortcuts {

namespace {
const kit::Rect R{80, 60, 480, 320};      // popup 0
const int DIALOG = 14, OK = 294, ITEM = 295, SLOT = 316;
const int BLANK[2] = {256, 84};           // 4125:049e

struct Entry {
  int id = 0;
  std::string name;
  int x = 0, y = 0, w = 0, h = 0;
};

std::vector<Entry> readItems() {
  std::vector<Entry> out;
  auto s = w2::readFile(G.dataDir + "/UDB/UDB.DAT");
  if (!s) return out;
  int n = w2::u16(*s, 0);
  for (int i = 0; i < n; i++) {
    size_t o = 2 + 68 * (size_t)i;
    if (o + 68 > s->size()) break;
    out.push_back(Entry{w2::u16(*s, o), w2::cstr(*s, o + 2, 50), w2::u16(*s, o + 52), w2::u16(*s, o + 54),
                        w2::u16(*s, o + 56), w2::u16(*s, o + 58)});
  }
  return out;
}

struct Shortcuts : kit::Modal {
  screen::View v;
  std::vector<Entry> items;
  int slot = 0;
  int itemIndex(int id) const {
    for (size_t i = 0; i < items.size(); i++) if (items[i].id == id) return (int)i;
    return w2::NONE;
  }
  int shortcut(int s) const {
    auto it = G.screen->ui.shortcuts.find(s);
    return it == G.screen->ui.shortcuts.end() ? w2::NONE : it->second;
  }
  void refresh() {
    auto& st = v.state;
    st[OK] = w2::uidata::NORMAL;
    for (int i = 0; i <= 20; i++) st[ITEM + i] = w2::uidata::NORMAL;
    for (int i = 0; i <= 3; i++) st[SLOT + i] = w2::uidata::NORMAL;
  }
  void button(int x, int y, int sx, int sy) {
    auto art = G.screen->artFor(w2::uidata::SHORTCUT_BITMAP);
    if (!art) return;
    gfx::setColor(1, 1, 1);
    gfx::draw(art->image, gfx::newQuad(sx, sy, 32, 29), x, y);
  }
  void draw() override {
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), "Menu Shortcuts", 320, 63);          // 4125:055c
    const Font& f = kit::font(2);
    int current = itemIndex(shortcut(slot));
    for (size_t i = 0; i < items.size(); i++) {
      const Entry& it = items[i];
      int x = 96 + 152 * ((int)i % 3);
      int y = 100 + 30 * ((int)i / 3);
      button(x, y, it.x + ((int)i == current ? it.w / 2 : 0), it.y);
      gfx::setColor(1, 1, 1);
      f.draw(it.name, x + 40, y + 5);
    }
    kit::centred(f, "Choose 4 buttons for shortcuts", 320, 320);   // 4125:056b
    for (int s = 0; s <= 3; s++) {
      int x = 96 + 40 * s;
      int k = itemIndex(shortcut(s));
      if (k != w2::NONE) button(x, 342, items[k].x, items[k].y);
      else button(x, 342, BLANK[0], BLANK[1]);
      if (s == slot) {
        kit::setPal(9);
        kit::outline(x, 342, 32, 29);
      }
    }
    std::set<int> h;
    for (int i = 0; i <= 20; i++) h.insert(ITEM + i);
    for (int i = 0; i <= 3; i++) h.insert(SLOT + i);
    kit::drawControls(v, h);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    auto& ui = G.screen->ui;
    if (c->id == OK) {
      w2::Json j = w2::Json::array();
      for (int s = 0; s < 4; s++) j.push(shortcut(s));
      prefs::set("shortcuts", j.dump());
      kit::pop(this);
      return;
    }
    if (c->id >= SLOT && c->id < SLOT + 4) {
      slot = c->id - SLOT;
    } else if (c->id >= ITEM && c->id < ITEM + 21) {
      int k = c->id - ITEM;
      if (k < (int)items.size()) {
        ui.shortcuts[slot] = items[k].id;
        if (!ui.shortcutNames.count(items[k].id)) ui.shortcutNames[items[k].id] = items[k].name;
      }
    }
    refresh();
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};
}  // namespace

void open() {
  auto d = std::make_shared<Shortcuts>();
  d->items = readItems();
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->refresh();
  kit::push(d);
}

}  // namespace shortcuts
