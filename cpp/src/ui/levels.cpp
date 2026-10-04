// Hero > Levels (auto_ui_hero_levels, 7563:1652).
//
// Popup 0, (80, 60) 480x320, and dialog 28: Done (469). "Hero Levels" in
// font 1, the column heads in the side's colours at y = 105, and a row per
// hero, 30 apart from y = 128, highest level first: the hero on a ring --
// its side's colour at the top level, grey below -- its name, its title, its
// experience, what the next level needs, its strength and its moves.
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/armytype.hpp"

namespace levels {

namespace {
const kit::Rect R{80, 60, 480, 320};      // popup 0
const int DIALOG = 28, DONE = 469;
const int S = 0x68, S_MALE = 0x63, S_FEMALE = 0x64;

int needs(int level) { return level == 1 ? 15 : level == 2 ? 30 : level == 3 ? 60 : w2::NONE; }

struct Levels : kit::Modal {
  screen::View v;
  std::vector<w2::Army*> rows;
  void draw() override {
    w2::Side* side = G.player;
    const Font& f = kit::font(2);
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(S, 0), 320, 63);
    const Font& head = f.colours(side->colour, side->edge);
    head.draw(kit::text(S, 1), 128, 105);
    head.draw(kit::text(S, 2), 272, 105);
    const int xs[4] = {392, 440, 488, 536};
    for (int k = 0; k < 4; k++) kit::centred(head, kit::text(S, k + 3), xs[k], 105);
    for (size_t i = 0; i < rows.size(); i++) {
      w2::Army* h = rows[i];
      int y = 128 + 30 * (int)i;
      int level = h->level ? h->level : 1;
      kit::army(w2::armytype::HERO, side->index, 88, y, level == 4 ? side->index + 2 : 1);
      gfx::setColor(1, 1, 1);
      f.draw(h->name, 128, y + 6);
      f.draw(kit::text(h->female ? S_FEMALE : S_MALE, level - 1), 272, y + 6);
      kit::centred(f, w2::fmt("%d", h->experience), 392, y + 6);
      kit::centred(f, needs(level) != w2::NONE ? w2::fmt("%d", needs(level)) : "-", 440, y + 6);
      kit::centred(f, w2::fmt("%d", h->strength), 488, y + 6);
      kit::centred(f, w2::fmt("%d", h->maxMoves), 536, y + 6);
    }
    for (int i = (int)rows.size(); i < 6; i++) kit::army(w2::NONE, side->index, 88, 128 + 30 * i, 1);
    kit::drawControls(v);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (c && c->id == DONE) kit::pop(this);
  }
  void keypressed(const std::string& key) override {
    if (key == "escape" || key == "return" || key == "kpenter") kit::pop(this);
  }
};
}  // namespace

void open() {
  auto d = std::make_shared<Levels>();
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->v.state[DONE] = w2::uidata::NORMAL;
  for (int level = 4; level >= 1; level--) {
    for (int i = (int)G.g->armies.size() - 1; i >= 0; i--) {
      w2::Army* a = G.g->armies[i];
      if (a->hero() && a->owner == G.player->index && (a->level ? a->level : 1) == level) d->rows.push_back(a);
    }
  }
  kit::push(d);
}

}  // namespace levels
