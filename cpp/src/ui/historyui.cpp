// History > City, Events, Gold, Winners (6d51:0000 with 0-3), and Triumphs.
//
// Plays back what warlords/history recorded, a round at a time. With
// nothing on record yet it says so (group 93). Popup 10, (32, 60) 576x312,
// dialog 19 (6d51:0096): the strategic map with the city shields as they
// stood that turn, the title, the four tabs (352-355); for City, Gold and
// Winners a graph of every side, one step a turn, scaled to the largest on
// record (6d51:034a); for Events that round's deeds and a timeline
// (6d51:16ba, 18da). The arrows (357/358) step a turn; a click on the graph
// or the timeline picks the turn under it (6d51:0908). Done (356).
#include <cmath>

#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/history.hpp"

namespace historyui {

namespace history = w2::history;
using w2::format;

namespace {
const kit::Rect R{32, 60, 576, 312};      // popup 10
const int DIALOG = 19, TAB = 352, DONE = 356, PREV = 357, NEXT = 358;
const int GX = 300, GY = 150, GW = 292, GH = 140;     // 4125:0dc6
const kit::Rect TL{275, 322, 314, 11};    // 4125:0dd6
const int CITY = 0, EVENTS = 1, GOLD = 2, WINNERS = 3;

int floorOf(int mode) { return mode == CITY ? 10 : mode == GOLD ? 500 : 100; }
const std::array<int, 8>& values(const w2::HistoryRecord& r, int mode) {
  if (mode == CITY) return r.cities;
  if (mode == GOLD) return r.gold;
  return r.score;
}
void hline(int x, int y, int n) { gfx::rectangle(gfx::FILL, x, y, n, 1); }
void vline(int x, int y, int n) { gfx::rectangle(gfx::FILL, x, y, 1, n); }

void shield(int s, int x, int y) {
  if (s < 0 || s > 7) return;
  gfx::setColor(1, 1, 1);
  gfx::draw(G.atransShields, gfx::newQuad(112 + 16 * (s / 4), 94 + 14 * (s % 4), 16, 14), x, y);
}

std::string eventText(const w2::Deed& e) {
  auto t = [](int i) { return kit::text(0x5e, i); };
  auto sideName = [](int i) { return i >= 0 && i < 8 ? G.g->map->sides[i].name : std::string(); };
  auto cityName = [](int i) { w2::City* c = G.g->map->city(i); return c ? c->name : std::string(); };
  int ty = e.type, v1 = e.v1, v2 = e.v2;
  if (ty == history::EMERGES) return format(t(0), e.name, cityName(v1));
  if (ty == history::KILLED) {
    if (v1 == history::IN_BATTLE) return format(t(1), e.name);
    if (v1 == history::SEARCHING) return format(t(2), e.name);
    return format(t(3), e.name, cityName(v1));
  }
  if (ty == history::QUEST_DONE) return format(t(4), e.name);
  if (ty == history::QUEST_GIVEN) return format(t(5), e.name);
  if (ty == history::VANQUISHED) return format(t(6), sideName(v1));
  if (ty == history::WON) {
    if (v1 == history::BY_NAME) return format(t(7), e.name, cityName(v2));
    return format(t(8), sideName(v1), cityName(v2));
  }
  if (ty == history::FINDS) {
    std::string what;
    if (v1 == history::ALLIES) what = t(13);
    else if (v1 == history::SAGE) what = t(14);
    else if (v1 == history::GOLD) what = t(15);
    else for (auto& it : G.g->map->items) if (it.index == v1) what = it.name;
    return format(t(9), e.name, what);
  }
  if (ty == history::VICTORIOUS) return format(t(10), sideName(v1));
  if (ty == history::TREACHERY) return format(t(16), sideName(v1));
  if (ty == history::WAR) return format(t(11), sideName(v1));
  if (ty == history::PEACE) return format(t(12), sideName(v1));
  return "";
}

struct History : kit::Modal {
  screen::View v;
  int mode = 0, turn = 1, n = 1;
  const w2::HistoryRecord& rec(int t) { return G.g->history[t - 1]; }
  void refresh() {
    auto& st = v.state;
    for (int i = 0; i <= 3; i++) st[TAB + i] = i == mode ? w2::uidata::ACTIVE : w2::uidata::NORMAL;
    st[PREV] = turn == 1 ? w2::uidata::DISABLED : w2::uidata::NORMAL;
    st[NEXT] = turn == n ? w2::uidata::DISABLED : w2::uidata::NORMAL;
    st[DONE] = w2::uidata::NORMAL;
  }
  int tx(int t) const { return GX + t * GW / n; }
  void drawGraph() {
    int top = floorOf(mode);
    for (int t = 1; t <= n; t++)
      for (int s = 0; s < 8; s++) top = std::max(top, values(rec(t), mode)[s]);
    auto axes = [](int c, int o) {
      kit::setPal(c);
      vline(GX - o, GY - o, GH);
      hline(GX - o, GY + GH - 1 - o, GW);
      for (int y : {GY, GY + GH / 2, GY + GH - 1}) hline(GX - 4 - o, y - o, 4);
      for (int x : {GX, GX + GW / 2, GX + GW - 1}) vline(x - o, GY + GH - 1 - o, 4);
    };
    axes(0, 1);
    axes(15, 0);
    const Font& f = kit::font(2);
    gfx::setColor(1, 1, 1);
    kit::right(f, w2::fmt("%d", top), GX - 8, GY - 6);
    kit::right(f, "0", GX - 8, GY + GH - 8);
    kit::centred(f, "0", GX, GY + GH + 8);
    kit::right(f, w2::fmt("%d", n), GX + GW + 8, GY + GH + 8);
    kit::centred(f, "Turns", GX + GW / 2, GY + GH + 8);
    int px[8], py[8];
    for (int s = 0; s < 8; s++) { px[s] = GX + 1; py[s] = GY + GH - 2; }
    for (int t = 1; t <= n; t++) {
      int x = tx(t);
      const auto& val = values(rec(t), mode);
      for (int s = 0; s < 8; s++) {
        int y = GY + GH - 1 - (int)std::floor((double)val[s] / top * GH);
        kit::line(G.g->map->sides[s].colour, px[s], py[s], x, y);
        px[s] = x;
        py[s] = y;
      }
    }
    int x = tx(turn);
    kit::line(13, x, GY - 2, x, GY + GH + 6);
    const auto& r = rec(turn);
    std::string text;
    if (mode == WINNERS) {
      int best = 0, lead = 0;
      for (int s = 0; s < 8; s++) if (r.score[s] > best) { best = r.score[s]; lead = s; }
      text = format(kit::text(0x5c, 3), turn, G.g->map->sides[lead].name);
    } else {
      text = format(kit::text(0x5c, mode), turn, values(r, mode)[G.player->index]);
    }
    gfx::setColor(1, 1, 1);
    kit::centred(f, text, 432, 318);
  }
  void drawEvents() {
    const Font& f = kit::font(2);
    const auto& events = rec(turn).events;
    for (size_t i = 0; i < events.size(); i++) {
      const w2::Deed& e = events[i];
      int y = 149 + 17 * (int)i;
      shield(e.side, 264, y + 1);
      std::string text = eventText(e);
      gfx::setColor(1, 1, 1);
      f.draw(text, 280, y);
      if (e.type == history::WAR || e.type == history::PEACE || e.type == history::TREACHERY)
        shield(e.v2, 280 + f.width(text) / 8 * 8 + 16, y + 1);
    }
    kit::bevel(272, 319, 320, 17, 4, 2);
    kit::bevel(273, 320, 318, 15, 4, 2);
    kit::setPal(0);
    kit::outline(274, 321, 316, 13);
    int w = std::min(TL.w, turn * TL.w / n);
    kit::setPal(G.player->colour);
    gfx::rectangle(gfx::FILL, TL.x, TL.y, w, TL.h);
    if (w < TL.w - 8) {
      kit::setPal(G.player->edge);
      gfx::rectangle(gfx::FILL, TL.x + w, TL.y, TL.w - w, TL.h);
    }
  }
  void draw() override {
    kit::popup(R);
    front::drawStrategicMap(R.x, R.y, nullptr, false, &rec(turn).owners);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(0x5b, mode), 432, 64);
    if (mode == EVENTS) drawEvents();
    else drawGraph();
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(2), w2::fmt("Turn %d", turn), 312, 350);
    kit::drawControls(v);
  }
  void pick(int x, int x0, int w) {
    int t = (int)std::floor((double)(x - x0) / w * n) + 1;
    turn = std::max(1, std::min(n, t));
  }
  void mousepressed(int x, int y, int) override {
    if (mode != EVENTS && x >= GX && x < GX + GW && y >= GY && y < GY + 160) { pick(x, GX, GW); refresh(); return; }
    if (mode == EVENTS && TL.contains(x, y)) { pick(x, TL.x, TL.w); refresh(); return; }
    auto c = kit::controlAt(v, x, y);
    if (!c || v.stateOf(c->id) == w2::uidata::DISABLED) return;
    if (c->id == DONE) { kit::pop(this); return; }
    if (c->id == PREV) turn--;
    else if (c->id == NEXT) turn++;
    else if (c->id >= TAB && c->id < TAB + 4) mode = c->id - TAB;
    refresh();
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};

// History > Triumphs (6d51:09eb): what the side has done to each other side,
// or lost itself. Popup 0, dialog 20 (6d51:0a21): eight tabs of BUTTON.PCK
// with the sides' shields on them (360-367), then five kinds of army with
// their counts (groups 81-85 for your own losses, 86-90 for the others').
const kit::Rect TRI{80, 60, 480, 320};    // popup 0
const int TRI_DIALOG = 20, TRI_DONE = 359, TRI_TAB = 360;
const int KINDS[5] = {4, 25, 28, 5, 29};

struct Triumphs : kit::Modal {
  screen::View v;
  int opp = 0;
  void refresh() {
    for (int i = 0; i < 8; i++) v.state[TRI_TAB + i] = i == opp ? w2::uidata::ACTIVE : w2::uidata::NORMAL;
    v.state[TRI_DONE] = w2::uidata::NORMAL;
  }
  void draw() override {
    int me = G.player->index;
    kit::popup(TRI);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(0x50, 0), 320, 64);
    auto button = G.screen->artFor(4);
    auto bshield = G.screen->artFor(38);
    for (int i = 0; i < 8; i++) {
      int x = 128 + 48 * i;
      gfx::setColor(1, 1, 1);
      if (button) gfx::draw(button->image, gfx::newQuad(400, i == opp ? 40 : 0, 48, 40), x, 109);
      if (bshield) gfx::draw(bshield->image, gfx::newQuad(i * 32, 0, 32, 36), x + 8, 111);
    }
    const Font& f = kit::font(2);
    int base = opp == me ? 0x51 : 0x56;
    for (int k = 0; k <= 4; k++) {
      int y = 168 + 35 * k;
      kit::army(KINDS[k], opp, 104, y, 1);
      int cnt = history::triumph(*G.g, me, opp, k);
      if (cnt > 0) {
        gfx::setColor(1, 1, 1);
        f.draw(format(kit::text(base + k, cnt == 1 ? 0 : 1), cnt), 144, y + 9);
      }
    }
    std::set<int> h;
    for (int i = 0; i < 8; i++) h.insert(TRI_TAB + i);
    kit::drawControls(v, h);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    if (c->id == TRI_DONE) { kit::pop(this); return; }
    if (c->id >= TRI_TAB && c->id < TRI_TAB + 8) {
      opp = c->id - TRI_TAB;
      refresh();
    }
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};
}  // namespace

void open(int which) {
  int n = std::min((int)G.g->history.size(), history::LAST_TURN);
  if (n < 1) {
    search::message(kit::text(0x5d, 0), kit::text(0x5d, 1));
    return;
  }
  // the turn shown counts from 1, as the original's does
  auto d = std::make_shared<History>();
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->mode = which;
  d->turn = n;
  d->n = n;
  d->refresh();
  kit::push(d);
}

void triumphs() {
  auto d = std::make_shared<Triumphs>();
  d->v = kit::view(TRI_DIALOG);
  d->view = &d->v;
  d->opp = G.player->index;
  d->refresh();
  kit::push(d);
}

}  // namespace historyui
