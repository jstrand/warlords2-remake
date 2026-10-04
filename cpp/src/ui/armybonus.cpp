// View > Army Bonus (89e0:1e3b): every army type, in the side's fight order,
// with its strength, moves, way of moving and bonus.
//
// Popup 0, (80, 60) 480x320 (89e0:1fd2): "Army Bonus" in font 1, a box raised
// with a (4, 2) bevel and outlined in black, STACK.PCK's column heads, and
// six rows 30 apart from y = 148: the army on a ring of the side's colour,
// name, strength, moves, how it moves, and its bonus (89e0:1a07, group 163).
// Dialog 34: Done (490), 491/492 a row up and down and 493/494 six. Of the
// 29 places only the first 27 are shown.
#include <map>

#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/combat.hpp"

namespace armybonus {

namespace {
const kit::Rect R{80, 60, 480, 320};      // popup 0
const int DIALOG = 34, DONE = 490, UP = 491, DOWN = 492, PAGE_UP = 493, PAGE_DOWN = 494;
const int ROWS = 6, PLACES = 27;
const int S = 0xa3;

struct Bonus : kit::Modal {
  screen::View v;
  std::map<int, int> byRank;
  int topRow = 0;
  void refresh() {
    auto& st = v.state;
    st[DONE] = w2::uidata::NORMAL;
    int up = topRow > 0 ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    int down = topRow + ROWS < PLACES ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    st[UP] = up; st[PAGE_UP] = up; st[DOWN] = down; st[PAGE_DOWN] = down;
  }
  void draw() override {
    w2::Side* side = G.player;
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(0xa4, 0), 320, 62);
    kit::bevel(124, 146, 434, 186, 4, 2);
    kit::setPal(0);
    kit::outline(125, 147, 432, 184);
    if (auto stack = G.screen->artFor(46)) {
      gfx::setColor(1, 1, 1);
      gfx::draw(stack->image, gfx::newQuad(188, 0, 216, 22), 256, 125);
    }
    const Font& f = kit::font(2);
    for (int i = 0; i < ROWS; i++) {
      auto it = byRank.find(topRow + i);
      const w2::ArmyType* t = it != byRank.end() ? G.g->types.byId(it->second) : nullptr;
      if (t) {
        int y = 148 + 30 * i;
        kit::army(t->id, side->index, 128, y, side->index + 2);
        gfx::setColor(1, 1, 1);
        f.draw(t->name, 168, y + 5);
        f.draw(w2::fmt("%d", t->strength), 280, y + 5);
        f.draw(w2::fmt("%d", t->move), 328, y + 5);
        int src = -1;
        if (t->flies) src = 184;
        else if (t->woodsMove && t->hillsMove) src = 216;
        else if (t->woodsMove) src = 248;
        else if (t->hillsMove) src = 152;
        if (src >= 0) {
          gfx::setColor(1, 1, 1);
          gfx::draw(G.abits, gfx::newQuad(src, 30, 32, 10), 352, y + 8);
        }
        gfx::setColor(1, 1, 1);
        f.draw(bonusText(*t), 400, y + 6);
      }
    }
    kit::drawControls(v);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    if (c->id == DONE) { kit::pop(this); return; }
    if (c->id == UP) topRow--;
    else if (c->id == DOWN) topRow++;
    else if (c->id == PAGE_UP) topRow = std::max(0, topRow - ROWS);
    else if (c->id == PAGE_DOWN) topRow = std::min(PLACES - ROWS, topRow + ROWS);
    refresh();
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};
}  // namespace

std::string bonusText(const w2::ArmyType& t, const w2::Army* army) {
  const auto& b = t.bonus;
  auto say = [](int i, int v = 0) { return w2::format(kit::text(S, i), v); };
  if (army && army->atSea) return say(1);
  if (army && army->hero()) {
    int s = std::min(9, army->strength + w2::combat::battleItems(*army));
    return say(2, std::min(6, w2::combat::HERO_TABLE[std::max(0, s)] + w2::combat::commandItems(*army)));
  }
  if (b[52] == 2) return say(3);
  if (b[52] == 3) return say(4);
  if (b[48] != 0) return say(5, b[42]);
  if (b[46] != 0 && b[44] != 0 && b[42] != 0 && b[40] != 0) return say(16, b[40]);
  if (b[52] == 1) return say(6);
  const int order[9][2] = {{50, 7}, {46, 8}, {44, 9}, {42, 10}, {40, 11}, {38, 12}, {36, 13}, {34, 14}, {32, 15}};
  for (auto& o : order) if (b[o[0]] != 0) return say(o[1], b[o[0]]);
  return say(0);
}

void open() {
  auto d = std::make_shared<Bonus>();
  const auto& row = G.g->map->fightOrder[G.player->index];
  for (int t = 0; t <= 28 && t < (int)row.size(); t++) d->byRank[row[t]] = t;
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->refresh();
  kit::push(d);
}

}  // namespace armybonus
