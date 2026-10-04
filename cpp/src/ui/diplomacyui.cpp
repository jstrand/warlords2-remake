// Report > Diplomacy (484e:0000): the Diplomatic Report and, behind its
// Action button, the Diplomatic Action screen. Only with Diplomacy on. Both
// are popup 11, (80, 60) 480x350.
//
// The report (484e:0039, dialog 21): a grid of every pair, each side's
// shield along the top and down the side, in each cell what DIPLOM.PCK shows
// for the state, and beside it the sides best first with their titles.
// Done (368) and Action (369).
//
// The action screen (484e:0382, dialog 22): for every other side its shield,
// the state between you, its proposal to you, and your three proposals to it
// -- peace, uneasy, war -- the chosen one lit. The buttons under them
// (372-378, 380-386, 388-394) set it (484e:0a69). OK (370); Report (371).
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/diplomacy.hpp"

namespace diplomacyui {

namespace dip = w2::diplomacy;

namespace {
const kit::Rect R{80, 60, 480, 350};      // popup 11
const int REPORT = 21, DONE = 368, ACTION = 369;
const int ACT = 22, OK = 370, BACK = 371;
const int OFFER[3] = {372, 380, 388};     // a row of 7 each

bool playing(const w2::Game& g, int i) { return i >= 0 && i < 8 && g.map->sides[i].inUse; }

void diplom(int sx, int sy, int w, int h, int x, int y) {
  if (auto art = G.screen->artFor(47)) {
    gfx::setColor(1, 1, 1);
    gfx::draw(art->image, gfx::newQuad(sx, sy, w, h), x, y);
  }
}

void smallShield(int side, int x, int y) {
  gfx::setColor(1, 1, 1);
  gfx::draw(G.shieldsImg, gfx::newQuad(side * 40 + 24, 46, 16, 16), x, y);
}

void blank(int x, int y) {
  kit::setPal(0);
  kit::outline(x, y, 40, 40);
  kit::setPal(2);
  gfx::rectangle(gfx::FILL, x + 1, y + 1, 38, 38);
}

void openAction();

struct Report : kit::Modal {
  screen::View v;
  std::map<int, std::string> titles;
  std::vector<w2::Side*> order;
  void draw() override {
    w2::Game& g = *G.g;
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(0x6b, 0), 320, 64);
    kit::setPal(1);
    gfx::rectangle(gfx::FILL, 88, 139, 1, 232);
    for (int i = 0; i <= 8; i++) gfx::rectangle(gfx::FILL, 120 + 32 * i, 110, 1, 261);
    gfx::rectangle(gfx::FILL, 120, 110, 256, 1);
    for (int i = 0; i <= 8; i++) gfx::rectangle(gfx::FILL, 88, 139 + 29 * i, 288, 1);
    for (int i = 0; i < 8; i++) {
      if (playing(g, i)) {
        smallShield(i, 128 + 32 * i, 117);
        smallShield(i, 96, 146 + 29 * i);
      }
    }
    for (int col = 0; col < 8; col++) {
      for (int row = 0; row < 8; row++) {
        int x = 120 + 32 * col, y = 139 + 29 * row;
        if (col == row) diplom(384, 29, 32, 29, x, y);
        else if (playing(g, col) && playing(g, row)) {
          int st = dip::state(g, col, row);
          if (st == dip::WAR) diplom(384, 0, 32, 29, x, y);
          else if (st == dip::PEACE) diplom(320, 0, 32, 29, x, y);
        }
      }
    }
    kit::setPal(0);
    kit::outline(392, 110, 160, 262);
    const Font& f = kit::font(2);
    gfx::setColor(1, 1, 1);
    kit::centred(f, kit::text(0x6d, 0), 472, 116);
    for (size_t i = 0; i < order.size(); i++) {
      int y = 146 + 29 * (int)i;
      smallShield(order[i]->index, 400, y);
      gfx::setColor(1, 1, 1);
      f.draw(titles[order[i]->index], 432, y);
    }
    kit::drawControls(v);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    if (c->id == DONE) kit::pop(this);
    else if (c->id == ACTION) {
      kit::pop(this);
      openAction();
    }
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};

void openReport() {
  auto d = std::make_shared<Report>();
  d->v = kit::view(REPORT);
  d->view = &d->v;
  d->v.state[DONE] = d->v.state[ACTION] = w2::uidata::NORMAL;
  d->titles = dip::ratings(*G.g);
  d->order = G.g->sides;
  w2::luaSort(d->order, [](w2::Side* p, w2::Side* q) {
    if (p->diploScore != q->diploScore) return p->diploScore < q->diploScore;
    return p->index < q->index;
  });
  kit::push(d);
}

struct Action : kit::Modal {
  screen::View v;
  std::vector<int> others;
  void draw() override {
    w2::Game& g = *G.g;
    int me = G.player->index;
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(0x6c, 0), 320, 64);
    kit::shield(me, 96, 111);
    const Font& f = kit::font(2);
    gfx::setColor(1, 1, 1);
    f.draw(G.player->name, 144, 122);
    const int ys[7] = {165, 205, 251, 271, 291, 311, 331};
    for (int i = 0; i < 7; i++) f.draw(kit::text(0x6e, i), 104, ys[i]);
    for (size_t col = 0; col < others.size(); col++) {
      int other = others[col];
      int x = 272 + 40 * (int)col;
      kit::shield(other, x, 111);
      if (!playing(g, other)) {
        for (int y : {151, 191, 245, 285, 325}) blank(x, y);
      } else {
        int st = dip::state(g, other, me);
        diplom(80 + 120 * st, 45, 40, 40, x, 151);
        int theirs = dip::proposal(g, other, me);
        if (theirs == st) blank(x, 191);
        else diplom(80 + 120 * theirs, 45, 40, 40, x, 191);
        int mine = dip::proposal(g, me, other);
        for (int k = 0; k <= 2; k++) diplom(120 * k + (mine == k ? 40 : 0), 45, 40, 40, x, 245 + 40 * k);
      }
    }
    std::set<int> h;
    for (int k = 0; k <= 2; k++) for (int i = 0; i <= 6; i++) h.insert(OFFER[k] + i);
    kit::drawControls(v, h);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    if (c->id == OK) { kit::pop(this); return; }
    if (c->id == BACK) {
      kit::pop(this);
      openReport();
      return;
    }
    for (int k = 0; k <= 2; k++) {
      int i = c->id - OFFER[k];
      if (i >= 0 && i < 7 && i < (int)others.size()) dip::propose(*G.g, G.player->index, others[i], k);
    }
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};

void openAction() {
  auto d = std::make_shared<Action>();
  d->v = kit::view(ACT);
  d->view = &d->v;
  for (auto& c : d->v.dialog.controls) d->v.state[c.id] = w2::uidata::NORMAL;
  for (int i = 0; i < 8; i++) if (i != G.player->index) d->others.push_back(i);
  kit::push(d);
}
}  // namespace

void open() {
  if (G.g->map->options.diplomacy == 0) return;
  openReport();
}

void action() {
  if (G.g->map->options.diplomacy == 0) return;
  openAction();
}

int buttonFor(w2::Game& g, int me) {
  if (g.map->options.diplomacy == 0) return w2::NONE;
  bool friendlier = false, hostile = false;
  for (int s = 0; s < 8; s++) {
    if (s != me) {
      int st = dip::state(g, s, me), p = dip::proposal(g, s, me);
      if (p != st) {
        if (p < st) friendlier = true;
        else hostile = true;
      }
    }
  }
  if (!(friendlier || hostile)) return 183;
  return hostile ? 184 : 185;
}

}  // namespace diplomacyui
