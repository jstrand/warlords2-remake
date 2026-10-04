#include "ui/kit.hpp"

#include <algorithm>
#include <cmath>

#include "util/util.hpp"

namespace kit {

ModalP push(ModalP d) {
  G.modals.push_back(d);
  return d;
}

void pop(const Modal* d) {
  for (int i = (int)G.modals.size() - 1; i >= 0; i--) {
    if (G.modals[i].get() == d) {
      G.modals.erase(G.modals.begin() + i);
      return;
    }
  }
}

Modal* top() { return G.modals.empty() ? nullptr : G.modals.back().get(); }
ModalP topShared() { return G.modals.empty() ? nullptr : G.modals.back(); }

void setPal(int i) {
  const auto& c = i >= 0 && i < (int)G.palette.size() ? G.palette[i] : G.palette[0];
  gfx::setColor((float)c[0], (float)c[1], (float)c[2]);
}

void outline(double x, double y, double w, double h) {
  gfx::rectangle(gfx::FILL, x, y, w, 1);
  gfx::rectangle(gfx::FILL, x, y + h - 1, w, 1);
  gfx::rectangle(gfx::FILL, x, y, 1, h);
  gfx::rectangle(gfx::FILL, x + w - 1, y, 1, h);
}

void popupFrame(const Rect& R) {
  gfx::setColor(0, 0, 0);
  outline(R.x - 1, R.y - 1, R.w + 2, R.h + 2);
  gfx::rectangle(gfx::FILL, R.x + 1, R.y + R.h + 1, R.w + 2, 2);
  gfx::rectangle(gfx::FILL, R.x + R.w + 1, R.y + 1, 2, R.h + 2);
}

void popup(const Rect& R) {
  popupFrame(R);
  gfx::setColor(1, 1, 1);
  gfx::setScissor(R.x, R.y, R.w, R.h);
  gfx::draw(G.marble, R.x, R.y);
  // past the marble's 480 columns the blit reads on into the next row
  int mw = G.marble->w, mh = G.marble->h;
  if (R.w > mw) gfx::draw(G.marble, gfx::newQuad(0, 1, R.w - mw, std::min(R.h, mh - 1)), R.x + mw, R.y);
  gfx::setScissor();
}

const Font& font(int n) {
  if (n == 1) return *G.titleFont;
  if (n == 2) return *G.bigFont;
  return *G.font;
}

void centred(const Font& f, const std::string& text, double x, double y) {
  f.draw(text, x - std::floor(f.width(text) / 2.0), y);
}

void right(const Font& f, const std::string& text, double x, double y) { f.draw(text, x - f.width(text), y); }

void bevel(double x, double y, double w, double h, int a, int b) {
  auto topLeft = [&]() {
    setPal(a);
    gfx::rectangle(gfx::FILL, x, y, w, 1);
    gfx::rectangle(gfx::FILL, x, y, 1, h);
  };
  auto bottomRight = [&]() {
    setPal(b);
    gfx::rectangle(gfx::FILL, x + w - 1, y, 1, h);
    gfx::rectangle(gfx::FILL, x, y + h - 1, w, 1);
  };
  if (b < a) { bottomRight(); topLeft(); }
  else { topLeft(); bottomRight(); }
}

void field(double x, double y, double w, double h, const std::string* text, const Font* f) {
  setPal(3);
  gfx::rectangle(gfx::FILL, x, y, w, h);
  bevel(x, y, w, h, 4, 2);
  gfx::setColor(1, 1, 1);
  if (text) (f ? *f : *G.bigFont).draw(*text, x + 3, y + 2);
}

gfx::Quad armyQuad(int typeId) {
  int i = ((typeId % 32) + 32) % 32;
  // An army sheet is 16 cells across on a 32-pixel stride; its rows are 30
  // apart and 29 tall, which is what 8611:08be reads out of it.
  return gfx::newQuad((i % 16) * 32, (i / 16) * 30, 32, 29);
}

void army(int typeId, int side, double x, double y, int ring, bool shadow) {
  if (side < 0 || side == 15 || side > 8) side = 8;
  gfx::setColor(1, 1, 1);
  if (ring > 0) gfx::draw(G.abits, gfx::newQuad((ring - 1) * 32, 0, 32, 30), x, y);
  if (typeId != w2::NONE) gfx::draw(shadow ? G.shadowImg : G.armyImg[side], armyQuad(typeId), x, y);
}

void shield(int side, double x, double y) {
  if (side < 0 || side == 15 || side > 8) side = 8;
  if (!G.shieldsImg) return;
  gfx::setColor(1, 1, 1);
  gfx::draw(G.shieldsImg, gfx::newQuad(side * 40, 0, 40, 40), x, y);
}

void line(int c, int x0, int y0, int x1, int y1) {
  setPal(c);
  int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;
  for (;;) {
    gfx::rectangle(gfx::FILL, x0, y0, 1, 1);
    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void mapTarget(int mx, int my, int hx, int hy, int tx, int ty) {
  gfx::setScissor(mx, my, 224, 312);
  line(8, mx + hx * 2 - 2, my + hy * 2 - 2, mx + tx * 2 - 2, my + ty * 2 - 2);
  setPal(8);
  int bx = mx + tx * 2, by = my + ty * 2 - 1;
  gfx::rectangle(gfx::FILL, bx, by, 4, 5);
  setPal(0);
  gfx::rectangle(gfx::FILL, bx - 1, by, 6, 1);
  gfx::rectangle(gfx::FILL, bx, by + 5, 4, 1);
  gfx::rectangle(gfx::FILL, bx - 1, by, 1, 5);
  gfx::rectangle(gfx::FILL, bx + 4, by, 1, 5);
  setPal(8);
  outline(mx + tx * 2 - 2, my + ty * 2 - 2, 8, 8);
  gfx::setScissor();
}

screen::View view(int dialogId) { return screen::dialog(*G.screen, dialogId); }

void drawControls(const screen::View& v, const std::set<int>* hidden) { screen::drawDialogControls(*G.screen, v, hidden); }

const w2::uidata::Control* controlAt(const screen::View& v, int x, int y, const std::set<int>* hidden) {
  for (const auto& c : v.dialog.controls) {
    if (c.w > 0 && c.h > 0 && !(hidden && hidden->count(c.id)) && x >= c.x && x < c.x + c.w && y >= c.y && y < c.y + c.h) {
      if (v.stateOf(c.id) == w2::uidata::DISABLED) return nullptr;
      return &c;
    }
  }
  return nullptr;
}

const w2::uidata::Control* control(const screen::View& v, int id) { return screen::dialogControl(v, id); }

std::string text(int group, int i) { return w2::uidata::text(G.screen->ui, group, i); }

double now() { return w2::now(); }

}  // namespace kit
