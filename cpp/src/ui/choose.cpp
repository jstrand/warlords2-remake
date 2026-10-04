// The game's list chooser (796c:0000): pick one of a list of names.
//
// Popup 3, (96, 50) 200x200, with dialog 2: the title in font 2 centred on
// (196, 52), a colour-3 box sunk with a (4, 2) bevel, five names 20 apart,
// the chosen one in colour 15 and the rest in colour 2. Controls 114-118
// choose a row, 121/122 scroll a row and 119/120 five, hidden when the list
// fits; OK (123) hands the chosen entry over, Cancel (124) none.
#include <algorithm>

#include "ui/dialogs.hpp"

namespace choose {

namespace {
const kit::Rect R{96, 50, 200, 200};       // popup 3
const kit::Rect BOX{106, 70, 180, 110};    // 4125:15a6
const int DIALOG = 2;
const int ROW = 114, PAGE_UP = 119, PAGE_DOWN = 120, UP = 121, DOWN = 122, OK = 123, CANCEL = 124;
const int ROWS = 5;

struct Dialog : kit::Modal {
  std::string title;
  std::vector<std::string> names;
  std::function<void(int)> after;
  screen::View v;
  int topRow = 0, cur = 0;

  int n() const { return (int)names.size(); }

  void refresh() {
    auto& st = v.state;
    for (int i = 0; i < ROWS; i++) st[ROW + i] = w2::uidata::NORMAL;
    st[OK] = w2::uidata::NORMAL;
    st[CANCEL] = w2::uidata::NORMAL;
    hidden.clear();
    if (n() > ROWS) {
      int up = topRow > 0 ? w2::uidata::NORMAL : w2::uidata::DISABLED;
      int down = topRow + ROWS - 1 < n() - 1 ? w2::uidata::NORMAL : w2::uidata::DISABLED;
      st[PAGE_UP] = up; st[UP] = up; st[PAGE_DOWN] = down; st[DOWN] = down;
    } else {
      for (int id : {PAGE_UP, PAGE_DOWN, UP, DOWN}) hidden.insert(id);
    }
  }

  int chosen() const { return topRow + cur < n() ? topRow + cur : w2::NONE; }

  void close(int entry) {
    auto a = after;
    kit::pop(this);
    if (a) a(entry);
  }

  void draw() override {
    kit::popup(R);
    const Font& f = kit::font(2);
    gfx::setColor(1, 1, 1);
    kit::centred(f, title, 196, 52);
    kit::setPal(3);
    gfx::rectangle(gfx::FILL, BOX.x, BOX.y, BOX.w, BOX.h);
    kit::bevel(BOX.x, BOX.y, BOX.w, BOX.h, 4, 2);
    gfx::setColor(1, 1, 1);
    for (int i = 0; i < ROWS; i++) {
      if (topRow + i < n()) f.colours(i == cur ? 15 : 2, 0).draw(names[topRow + i], 112, 74 + 20 * i);
    }
    kit::drawControls(v, hidden);
  }

  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y, hidden);
    if (!c) return;
    int id = c->id;
    if (id >= ROW && id < ROW + ROWS) {
      if (topRow + id - ROW < n()) cur = id - ROW;
    } else if (id == UP) {
      topRow--; cur = std::min(cur + 1, ROWS - 1);
    } else if (id == DOWN) {
      topRow++; cur = std::max(cur - 1, 0);
    } else if (id == PAGE_UP) {
      topRow -= std::min(ROWS, topRow);
    } else if (id == PAGE_DOWN) {
      topRow += std::min(ROWS, n() - (topRow + ROWS - 1) - 1);
    } else if (id == OK) {
      return close(chosen());
    } else if (id == CANCEL) {
      return close(w2::NONE);
    }
    refresh();
  }

  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter") close(chosen());
    else if (key == "escape") close(w2::NONE);
  }
};
}  // namespace

void open(const std::string& title, const std::vector<std::string>& names, int start, std::function<void(int)> after) {
  auto d = std::make_shared<Dialog>();
  d->title = title;
  d->names = names;
  d->after = after;
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->cur = std::max(0, start);
  while (d->cur >= ROWS) { d->cur--; d->topRow++; }
  d->refresh();
  kit::push(d);
}

}  // namespace choose
