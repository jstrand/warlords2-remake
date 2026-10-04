#include "front/layout.hpp"

#include <algorithm>

namespace layout {

static const Rect MAP{16, 30, 360, 360, 2};       // region 2
static const Rect PANEL{0, 17, 392, 386, 13};     // region 13, the map's frame
static const Rect STRAT{400, 30, 224, 312, 1};    // region 1
static const Rect BAR{16, 408, 360, 56, 9};       // region 9

const Offset& Layout::offset(const std::string& g) const {
  if (g == "strat") return strat;
  if (g == "panel") return panel;
  if (g == "bar") return bar;
  return map;
}

Layout compute(int w, int h) {
  Layout L;
  L.w = std::max(W, w);
  L.h = std::max(H, h);
  L.ex = L.w - W;
  L.ey = L.h - H;
  L.classic = L.ex == 0 && L.ey == 0;
  L.strat = {L.ex, 0};
  L.panel = {L.ex, L.ey};
  L.bar = {L.ex / 2, L.ey};
  L.map = {0, 0};
  L.dialog = {L.ex / 2, L.ey / 2};
  L.mapRect = Rect{MAP.x, MAP.y, MAP.w + L.ex, MAP.h + L.ey, 2};
  L.panelRect = Rect{PANEL.x, PANEL.y, PANEL.w + L.ex, PANEL.h + L.ey, 13};
  L.stratRect = move(L, "strat", STRAT);
  L.barRect = move(L, "bar", BAR);
  return L;
}

std::string group(int x, int y) {
  if (x >= 392) return y < 348 ? "strat" : "panel";
  if (y >= 396) return "bar";
  return "map";
}

Rect move(const Layout& L, const std::string& g, Rect r) {
  const Offset& o = L.offset(g);
  r.x += o.x;
  r.y += o.y;
  return r;
}

Rect region(const Layout& L, Rect r) {
  Rect out = move(L, group(r.x, r.y), r);
  if (r.id == 2) { out = L.mapRect; out.id = 2; }
  else if (r.id == 13) { out = L.panelRect; out.id = 13; }
  else if (r.id == 3) { out.x = r.x; out.y = r.y; out.w = L.w; }
  return out;
}

}  // namespace layout
