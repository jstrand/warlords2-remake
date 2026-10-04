// View > Items (66d4:0c21): the scenario's fourteen magic items.
//
// Popup 4, (120, 50) 400x360: "Items" (group 166) in font 1, "Items in this
// scenario" (group 167) at the foot, and a row for each item from record 8
// on, sorted by kind and then by value, 20 apart from y = 90: its name in
// colour 7 ending at x = 304, and what it does from x = 336. Dialog 35: Done
// (495) and Help (496), which shows HELP\HITEM.GFX.
#include <map>

#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/rules.hpp"

namespace items {

namespace {
const kit::Rect R{120, 50, 400, 360};     // popup 4
const int DIALOG = 35, DONE = 495, HELP = 496;
const int FIRST = 8, LAST = 21;           // records 8-21
const std::map<int, int> WHAT = {
    {w2::rules::ITEM_BATTLE, 1}, {w2::rules::ITEM_COMMAND, 2}, {w2::rules::ITEM_FLIGHT, 3},
    {w2::rules::ITEM_DOUBLE_MOVE, 4}, {w2::rules::ITEM_GOLD_PER_CITY, 5}};

struct Items : kit::Modal {
  screen::View v;
  std::vector<w2::Item*> list;
  void draw() override {
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(0xa6, 0), 320, 52);
    kit::centred(kit::font(2), kit::text(0xa7, 0), 320, 382);
    const Font& f = kit::font(2).colours(7, 0);
    // the kind's line carries over when an item is of no kind it knows
    int what = 0;
    for (size_t i = 0; i < list.size(); i++) {
      int y = 90 + 20 * (int)i;
      auto w = WHAT.find(list[i]->type);
      if (w != WHAT.end()) what = w->second;
      kit::right(f, list[i]->name, 304, y);
      f.draw(w2::format(kit::text(0xa7, what), list[i]->value), 336, y);
    }
    kit::drawControls(v);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    if (c->id == DONE) kit::pop(this);
    else if (c->id == HELP) help::open("HELP\\HITEM.GFX");
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};
}  // namespace

void open() {
  auto d = std::make_shared<Items>();
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->v.state[DONE] = w2::uidata::NORMAL;
  d->v.state[HELP] = w2::uidata::NORMAL;
  for (auto& it : G.g->map->items) if (it.index >= FIRST && it.index <= LAST) d->list.push_back(&it);
  // an insertion sort, by kind then value: equal ones keep their order
  auto& l = d->list;
  for (size_t i = 1; i < l.size(); i++) {
    size_t j = i;
    while (j > 0 && (l[j]->type < l[j - 1]->type || (l[j]->type == l[j - 1]->type && l[j]->value < l[j - 1]->value))) {
      std::swap(l[j], l[j - 1]);
      j--;
    }
  }
  kit::push(d);
}

}  // namespace items
