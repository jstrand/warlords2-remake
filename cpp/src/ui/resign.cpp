// Order > Resign (7721:150d).
//
// Popup 1, (160, 90) 320x200: "Resign!" (group 165) in font 1 centred on
// (320, 92), and three lines of font 2. Dialog 33: 487 resigns graciously,
// 488 resigns, 489 thinks better of it (default and cancel both). Either way
// the side's cities burn and its armies go (7721:1608). The gracious way is
// asked three times first; the other is told afterwards what has burned.
#include "ui/dialogs.hpp"

namespace resign {

namespace {
const kit::Rect R{160, 90, 320, 200};     // popup 1
const int DIALOG = 33, GRACIOUS = 487, RESIGN = 488, CANCEL = 489;
const int S = 0xa5;
std::string t(int i) { return kit::text(S, i); }

void burn(kit::Done after) {
  w2::game::resign(*G.g, *G.player);
  G.selection.reset();
  front::stratDirty();
  if (after) after();
}

struct Resign : kit::Modal {
  screen::View v;
  void draw() override {
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), t(0), 320, 92);
    for (int i = 1; i <= 3; i++) kit::centred(kit::font(2), t(i), 320, 120 + 20 * i);
    kit::drawControls(v);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    if (c->id == CANCEL) {
      kit::pop(this);
    } else if (c->id == GRACIOUS) {
      kit::pop(this);
      search::say(t(6), []() {
        search::say(t(7), []() {
          search::say(t(8), []() { search::message(t(9), t(10), []() { burn(nullptr); }); });
        });
      });
    } else if (c->id == RESIGN) {
      kit::pop(this);
      burn([]() { search::message(t(4), t(5)); });
    }
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};
}  // namespace

void open() {
  if (w2::game::sideCities(*G.g, *G.player).empty()) return;   // 4125:5dea
  auto d = std::make_shared<Resign>();
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  for (int id : {GRACIOUS, RESIGN, CANCEL}) d->v.state[id] = w2::uidata::NORMAL;
  kit::push(d);
}

}  // namespace resign
