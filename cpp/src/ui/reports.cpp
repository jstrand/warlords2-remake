// The Report menu: Army, City, Gold, Production and Winning.
//
// Dialog 7 over popup 2, (80, 60) 480x312, the five reports as tabs (218-222)
// and Done (223); auto_ui_reports_menu (6ef3:0030) draws the strategic map
// -- a banner on every stack outside a city for Army (834b:158d), See All's
// vectors for Production -- the title, what it measures, a bar per side or
// the list of what was built this turn, and the summary in the side's
// colours. The bars (6ef3:05ff) are tiled 8 pixels at a time from
// SHIELDS.PCK; the Production list (6f8c:086e) shows five at a time, and
// 206-209 scroll it.
#include <set>

#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/report.hpp"

namespace reports {

namespace report = w2::report;
using w2::format;

namespace {
const kit::Rect R{80, 60, 480, 312};      // popup 2
const int MAP_X = 80, MAP_Y = 60;
const int DIALOG = 7;
const int TAB_FIRST = 218, DONE = 223;
const int UP = 206, DOWN = 207, PAGE_UP = 208, PAGE_DOWN = 209;
const int S_TITLE = 0x49, S_WHAT = 0x4a;
const int S_SUMMARY[5] = {0x4b, 0x4c, 0x4d, 0x4e, 0x4f};
const int ROWS = 5;

std::string summary(int n, const report::Figures& f) {
  int r = f.result;
  if (n == report::ARMY || n == report::CITY) return r == 1 ? kit::text(S_SUMMARY[n], 0) : format(kit::text(S_SUMMARY[n], 1), r);
  if (n == report::GOLD) return format(kit::text(S_SUMMARY[n], 0), r);
  if (n == report::PRODUCTION) return format(kit::text(S_SUMMARY[n], r == 1 ? 1 : 0), r);
  return kit::text(S_SUMMARY[n], r);
}

void drawArmyBanners() {
  std::set<int> done;
  gfx::setScissor(MAP_X, MAP_Y, 224, 312);
  gfx::setColor(1, 1, 1);
  for (w2::Army* a : G.g->armies) {
    if (!a->transit && a->x != w2::NONE) {
      int k = a->y * 1000 + a->x;
      if (!done.count(k) && w2::game::seen(*G.g, G.player->index, a->x, a->y) && !w2::game::cityAt(*G.g, a->x, a->y)) {
        done.insert(k);
        int side = a->owner == w2::NONE ? 8 : a->owner;
        gfx::draw(G.atransShields, gfx::newQuad(side * 16, 164, 16, 10), MAP_X + std::max(0, a->x * 2 - 1),
                  MAP_Y + std::max(0, a->y * 2 - 1));
      }
    }
  }
  gfx::setScissor();
}

void drawBars(const report::Figures& f) {
  for (int i = 0; i < 8; i++) {
    if (!f.out[i]) {
      int y = 200 + 14 * i;
      int w = 1;
      if (f.max > 0) w = std::max(0, f.value[i]) * 240 / f.max;
      w = std::max(1, w);
      gfx::setColor(1, 1, 1);
      int sx = 320 + 8 * (i % 4), sy = 40 + 8 * (i / 4);
      for (int x = 0; x < w; x += 8) {
        int piece = std::min(8, w - x);
        gfx::draw(G.shieldsImg, gfx::newQuad(sx, sy, piece, 8), 312 + x, y);
      }
      kit::bevel(311, y - 1, w + 2, 10, 0, 1);
    }
  }
  auto axis = [](int colour, int d) {
    kit::setPal(colour);
    gfx::rectangle(gfx::FILL, 312 - d, 196 - d, 240, 1);
    const int tx[5] = {312, 372, 432, 492, 552};
    for (int k = 0; k < 5; k++) {
      bool lng = k % 2 == 0;
      gfx::rectangle(gfx::FILL, tx[k] - d, (lng ? 190 : 192) - d, 1, lng ? 6 : 4);
    }
  };
  axis(1, 0);
  axis(0, 1);
  const Font& font = kit::font(2);
  gfx::setColor(1, 1, 1);
  kit::right(font, w2::fmt("%d", f.max), 556, 172);
  kit::centred(font, w2::fmt("%d", f.max / 2), 432, 172);
  kit::centred(font, "0", 308, 172);
}

struct Reports : kit::Modal {
  screen::View v;
  int n = 0, topRow = 0;
  void refresh() {
    auto& st = v.state;
    for (int i = 0; i <= 4; i++) st[TAB_FIRST + i] = i == n ? w2::uidata::ACTIVE : w2::uidata::NORMAL;
    st[DONE] = w2::uidata::NORMAL;
    hidden.clear();
    if (n == report::PRODUCTION) {
      int count = (int)G.player->produced.size();
      st[UP] = topRow > 0 ? w2::uidata::NORMAL : w2::uidata::DISABLED;
      st[PAGE_UP] = st[UP];
      st[DOWN] = topRow + ROWS < count ? w2::uidata::NORMAL : w2::uidata::DISABLED;
      st[PAGE_DOWN] = st[DOWN];
    } else {
      for (int id : {UP, DOWN, PAGE_UP, PAGE_DOWN}) hidden.insert(id);
    }
  }
  void scroll(int by) {
    int count = (int)G.player->produced.size();
    topRow = std::max(0, std::min(std::max(0, count - ROWS), topRow + by));
    refresh();
  }
  void drawProduction() {
    const auto& list = G.player->produced;
    const Font& font = kit::font(2);
    for (int i = 0; i < ROWS; i++) {
      if (topRow + i >= (int)list.size()) break;
      const w2::Produced& e = list[topRow + i];
      int y = 176 + 30 * i;
      gfx::setColor(1, 1, 1);
      font.draw(w2::fmt("%d", topRow + i + 1), 312, y);
      kit::army(e.type, G.player->index, 328, 170 + 30 * i, 1);
      w2::City* city = e.city != w2::NONE ? G.g->map->city(e.city) : nullptr;
      std::string name = e.standard ? "Standard" : (city ? city->name : "");
      std::string text;
      if (e.kind == "sent") text = name + " ...";
      else if (e.kind == "arrived") text = "... " + name;
      else text = name;
      gfx::setColor(1, 1, 1);
      font.draw(text, 368, y);
    }
  }
  void draw() override {
    auto f = report::figures(*G.g, *G.player, n);
    kit::popup(R);
    if (n == report::PRODUCTION) {
      front::drawVectorMap(MAP_X, MAP_Y, nullptr, w2::NONE, true);
    } else {
      front::drawStrategicMap(MAP_X, MAP_Y);
      if (n == report::ARMY) drawArmyBanners();
    }
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(S_TITLE, n), 432, 62);
    kit::centred(kit::font(2), kit::text(S_WHAT, n), 432, 149);
    if (n == report::PRODUCTION) drawProduction();
    else drawBars(f);
    kit::centred(kit::font(2).colours(G.player->colour, G.player->edge), summary(n, f), 432, 322);
    kit::drawControls(v, hidden);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y, hidden);
    if (!c) return;
    if (c->id == DONE) kit::pop(this);
    else if (c->id >= TAB_FIRST && c->id < TAB_FIRST + 5) { n = c->id - TAB_FIRST; topRow = 0; refresh(); }
    else if (c->id == UP) scroll(-1);
    else if (c->id == DOWN) scroll(1);
    else if (c->id == PAGE_UP) scroll(-ROWS);
    else if (c->id == PAGE_DOWN) scroll(ROWS);
  }
  void keypressed(const std::string& key) override {
    if (key == "escape" || key == "return" || key == "kpenter") kit::pop(this);
  }
};
}  // namespace

void open(int which) {
  auto d = std::make_shared<Reports>();
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->n = which;
  d->refresh();
  kit::push(d);
}

}  // namespace reports
