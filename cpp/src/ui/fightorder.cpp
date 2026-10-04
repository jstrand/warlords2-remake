// Order > Fight Order (6a89:0de1): the order the side's armies fight in.
//
// Popup 11, (80, 60) 480x350 (6a89:0e4a): "Fighting Order" between the
// side's shields, "Order of combat for %s", three lines of help, and the 27
// places four to a row, the chosen one on the side's ring. Dialog 26: OK
// (425), Cancel (426), Default (427, the neutral row back), 428/429 move the
// chosen army a place earlier or later, and 430-456 the places. A click on a
// place chooses it; on the chosen one, lets it go; on another, swaps the two
// (6a89:1379). Cancel puts back the copy taken on opening (6a89:111c).
#include "ui/dialogs.hpp"
#include "util/util.hpp"

namespace fightorder {

namespace {
const kit::Rect R{80, 60, 480, 350};      // popup 11
const int DIALOG = 26;
const int OK = 425, CANCEL = 426, DEFAULT = 427, EARLIER = 428, LATER = 429, PLACE = 430;
const int PLACES = 27;
const int S = 0x7f;
const int NEUTRAL = 8;

struct Order : kit::Modal {
  screen::View v;
  std::vector<int> backup;
  int chosen = -1;
  std::vector<int>& row() { return G.g->map->fightOrder[G.player->index]; }
  int typeAt(int rank) {
    auto& r = row();
    for (int t = 0; t <= 28 && t < (int)r.size(); t++) if (r[t] == rank) return t;
    return w2::NONE;
  }
  void swap(int a, int b) {
    int ta = typeAt(a), tb = typeAt(b);
    if (ta != w2::NONE) row()[ta] = b;
    if (tb != w2::NONE) row()[tb] = a;
  }
  void refresh() {
    auto& st = v.state;
    st[OK] = st[CANCEL] = st[DEFAULT] = w2::uidata::NORMAL;
    st[EARLIER] = chosen > 0 ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    st[LATER] = (chosen >= 0 && chosen < PLACES - 1) ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    for (int i = 0; i < PLACES; i++) st[PLACE + i] = w2::uidata::NORMAL;
  }
  void close(bool keep) {
    if (!keep) row() = backup;
    kit::pop(this);
  }
  void draw() override {
    w2::Side* side = G.player;
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(S, 0), 320, 64);
    kit::shield(side->index, 88, 64);
    kit::shield(side->index, 512, 64);
    const Font& f = kit::font(2);
    gfx::setColor(1, 1, 1);
    kit::centred(f, w2::format(kit::text(S, 1), side->name), 320, 104);
    kit::centred(f, kit::text(S, 2), 320, 348);
    kit::centred(f, kit::text(S, 3), 320, 368);
    kit::centred(f, kit::text(S, 4), 320, 388);
    for (int i = 0; i < PLACES; i++) {
      int x = 120 * (i % 4), y = 31 * (i / 4);
      int t = typeAt(i);
      if (t != w2::NONE) kit::army(t, side->index, 88 + x, 128 + y, i == chosen ? side->index + 2 : 1);
      gfx::setColor(1, 1, 1);
      f.draw(w2::fmt("%d.", i + 1), 128 + x, 134 + y);       // 4125:0d5c
    }
    std::set<int> h;
    for (int i = 0; i < PLACES; i++) h.insert(PLACE + i);
    kit::drawControls(v, h);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    int id = c->id;
    if (id == OK) return close(true);
    if (id == CANCEL) return close(false);
    if (id == DEFAULT) {
      row() = G.g->map->fightOrder[NEUTRAL];
      chosen = -1;
    } else if (id == EARLIER && chosen > 0) {
      swap(chosen, chosen - 1);
      chosen--;
    } else if (id == LATER && chosen >= 0 && chosen < PLACES - 1) {
      swap(chosen, chosen + 1);
      chosen++;
    } else if (id >= PLACE && id < PLACE + PLACES) {
      int i = id - PLACE;
      if (chosen == i) chosen = -1;
      else if (chosen < 0) chosen = i;
      else { swap(chosen, i); chosen = -1; }
    }
    refresh();
  }
  // the right button on a place (sub-ids 45-71, 6a89:1475)
  bool info(int sub, int sx, int sy) override {
    int t = typeAt(sub - 45);
    const w2::ArmyType* rec = t != w2::NONE ? G.g->types.byId(t) : nullptr;
    if (!rec) return false;
    infobox::armyType(sx, sy, t, w2::Slot{rec->id, rec->name, rec->strength, rec->time, rec->cost, rec->move, rec->price},
                      G.player->index);
    return true;
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter") close(true);
    else if (key == "escape") close(false);
  }
};
}  // namespace

void open() {
  auto d = std::make_shared<Order>();
  d->backup = G.g->map->fightOrder[G.player->index];
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->refresh();
  kit::push(d);
}

}  // namespace fightorder
