// Game > Controller: what each of a game controller's buttons does. Not the
// original's, which knew no controller.
//
// A Steam Deck seen from the front, each button that can be set drawn where
// it lies (the back grips under the handles they are behind), and beside it
// the buttons of each hand with what they do now. A button is picked by a
// click on either, or by pressing it -- but for one that clicks, which goes
// on clicking, so that the dialog can be worked from the controller alone.
// Under them is every action there is, the picked button's lit and each
// with the first button that carries it; a click gives it to the picked
// button. The last button that clicks keeps its click.
//
// The settings are kept as they are made (front/padmap); OK only closes.
#include <algorithm>
#include <cmath>

#include "front/padglyph.hpp"
#include "front/padmap.hpp"
#include "platform/pad.hpp"
#include "platform/sound.hpp"
#include "ui/dialogs.hpp"

namespace padconfig {

namespace {
// under the menu bar where the screen is 400 rows, and clear of its foot
const kit::Rect R{40, 58, 560, 376};
const int OK_DIALOG = 14, OK = 294;         // Shortcuts' OK button, moved
const int WHITE = 15, BLACK = 0, YELLOW = 7, BODY = 4, DARK = 3;

// the Deck: its body, its screen, and the left hand's pieces; the right
// hand's are theirs mirrored about the middle
const kit::Rect BODY_R{180, 126, 280, 114}, SCREEN_R{245, 136, 150, 94};
const int MIDDLE = 320;
const int DPAD_X = 187, DPAD_Y = 144, ARM = 9;
const int STICK_X = 229, STICK_Y = 157, STICK_R = 8;
const kit::Rect TRACKPAD{212, 180, 28, 28};
const kit::Rect BUMPER{186, 117, 44, 9}, TRIGGER{192, 105, 32, 10};
const int FACE_X = 436, FACE_Y = 152, FACE_STEP = 10;    // the middle of A B X Y, its top left
const int VIEW_X = 228, VIEW_Y = 131, GRIP_X = 188, GRIP_Y = 243, GRIP_STEP = 16;

// the lists and the actions
const int LIST_Y = 102, ROW = 15, LEFT_X = 46, RIGHT_X = 466, LIST_W = 130;
const int GRID_X = 48, GRID_Y = 260, GRID_ROWS = 9, GRID_W = 136;
const int FOOT_Y = 404;
const kit::Rect HINTS_BOX{48, FOOT_Y, 24, 20}, SWAP_BOX{210, FOOT_Y, 24, 20}, DEFAULTS{434, FOOT_Y - 1, 70, 22};

const int LEFT_HAND[10] = {pad::LEFT_TRIGGER, pad::LEFT_SHOULDER, pad::BACK,      pad::DPAD_UP,   pad::DPAD_LEFT,
                           pad::DPAD_RIGHT,   pad::DPAD_DOWN,     pad::LEFT_STICK, pad::PADDLE2,  pad::PADDLE4};
const int RIGHT_HAND[10] = {pad::RIGHT_TRIGGER, pad::RIGHT_SHOULDER, pad::START,       pad::Y,       pad::X,
                            pad::B,             pad::A,              pad::RIGHT_STICK, pad::PADDLE1, pad::PADDLE3};

int mirror(const kit::Rect& r) { return 2 * MIDDLE - r.x - r.w; }

// Where a button is on the drawing: the rect a click picks it by.
struct Spot {
  int button;
  kit::Rect r;
};
std::vector<Spot> spots() {
  auto at = [](int x, int y, int w, int h) { return kit::Rect{x, y, w, h}; };
  auto right = [](kit::Rect r) { r.x = mirror(r); return r; };
  kit::Rect stick = at(STICK_X - STICK_R, STICK_Y - STICK_R, 2 * STICK_R + 1, 2 * STICK_R + 1);
  kit::Rect view = at(VIEW_X, VIEW_Y, 13, padglyph::H), l4 = at(GRIP_X, GRIP_Y, 13, padglyph::H);
  kit::Rect l5 = at(GRIP_X + GRIP_STEP, GRIP_Y, 13, padglyph::H);
  return {
      {pad::DPAD_UP, at(DPAD_X + ARM, DPAD_Y, ARM, ARM)},
      {pad::DPAD_DOWN, at(DPAD_X + ARM, DPAD_Y + 2 * ARM, ARM, ARM)},
      {pad::DPAD_LEFT, at(DPAD_X, DPAD_Y + ARM, ARM, ARM)},
      {pad::DPAD_RIGHT, at(DPAD_X + 2 * ARM, DPAD_Y + ARM, ARM, ARM)},
      {pad::LEFT_STICK, stick},
      {pad::RIGHT_STICK, right(stick)},
      // Steam's Gamepad layout: a press on the right trackpad is R3 too
      {pad::RIGHT_STICK, right(TRACKPAD)},
      {pad::BACK, view},
      {pad::START, right(view)},
      {pad::LEFT_SHOULDER, BUMPER},
      {pad::RIGHT_SHOULDER, right(BUMPER)},
      {pad::LEFT_TRIGGER, TRIGGER},
      {pad::RIGHT_TRIGGER, right(TRIGGER)},
      {pad::Y, at(FACE_X, FACE_Y - FACE_STEP, 11, 11)},
      {pad::A, at(FACE_X, FACE_Y + FACE_STEP, 11, 11)},
      {pad::X, at(FACE_X - FACE_STEP, FACE_Y, 11, 11)},
      {pad::B, at(FACE_X + FACE_STEP, FACE_Y, 11, 11)},
      {pad::PADDLE2, l4},
      {pad::PADDLE4, l5},
      {pad::PADDLE1, right(l4)},
      {pad::PADDLE3, right(l5)},
  };
}

const char* labelOf(int button) {
  for (const auto& b : padmap::buttons()) if (b.code == button) return b.label;
  return "";
}

void fill(int c, const kit::Rect& r) {
  kit::setPal(c);
  gfx::rectangle(gfx::FILL, r.x, r.y, r.w, r.h);
}

// A rect with its corners rounded: `top` and `bottom` are the radii.
void rounded(int c, const kit::Rect& r, int top, int bottom) {
  kit::setPal(c);
  for (int y = 0; y < r.h; y++) {
    int rad = y < top ? top : y >= r.h - bottom ? bottom : 0;
    int in = 0;
    if (rad > 0) {
      double dy = y < top ? top - y - 0.5 : y - (r.h - bottom) + 0.5;
      in = (int)std::ceil(rad - std::sqrt(std::max(0.0, rad * rad - dy * dy)));
    }
    gfx::rectangle(gfx::FILL, r.x + in, r.y + y, r.w - 2 * in, 1);
  }
}

void disc(int c, int cx, int cy, int rad) {
  kit::setPal(c);
  for (int y = -rad; y <= rad; y++) {
    int half = (int)std::floor(std::sqrt((rad + 0.5) * (rad + 0.5) - y * y));
    gfx::rectangle(gfx::FILL, cx - half, cy + y, 2 * half + 1, 1);
  }
}

struct Config : kit::Modal {
  screen::View v;
  int picked = -1;            // the button being set, or -1
  std::vector<Spot> where = spots();

  // ---------------------------------------------------------------- drawing
  void drawDeck() {
    // the back grips, behind the handles: only their ends show
    for (int b : {pad::PADDLE2, pad::PADDLE4, pad::PADDLE1, pad::PADDLE3}) {
      for (const Spot& s : where) {
        if (s.button != b) continue;
        fill(BLACK, {s.r.x + 5, BODY_R.y + BODY_R.h - 4, 3, 8});
      }
    }
    // the triggers behind the bumpers, then the body over both
    for (bool rightHand : {false, true}) {
      kit::Rect t = TRIGGER, b = BUMPER;
      if (rightHand) { t.x = mirror(t); b.x = mirror(b); }
      bool tLit = picked == (rightHand ? pad::RIGHT_TRIGGER : pad::LEFT_TRIGGER);
      bool bLit = picked == (rightHand ? pad::RIGHT_SHOULDER : pad::LEFT_SHOULDER);
      rounded(BLACK, {t.x - 1, t.y - 1, t.w + 2, t.h + 12}, 4, 0);
      rounded(tLit ? YELLOW : DARK, {t.x, t.y, t.w, t.h + 10}, 3, 0);
      padglyph::text(rightHand ? "R2" : "L2", t.x + t.w / 2 - 3, t.y + 2, tLit ? BLACK : WHITE);
      rounded(BLACK, {b.x - 1, b.y - 1, b.w + 2, b.h + 4}, 4, 0);
      rounded(bLit ? YELLOW : DARK, {b.x, b.y, b.w, b.h + 3}, 3, 0);
      padglyph::text(rightHand ? "R1" : "L1", b.x + b.w / 2 - 3, b.y + 2, bLit ? BLACK : WHITE);
    }
    rounded(BLACK, {BODY_R.x - 1, BODY_R.y - 1, BODY_R.w + 2, BODY_R.h + 2}, 9, 25);
    rounded(BODY, BODY_R, 8, 24);
    // the screen, and what it has to say
    fill(BLACK, {SCREEN_R.x - 3, SCREEN_R.y - 3, SCREEN_R.w + 6, SCREEN_R.h + 6});
    fill(6, SCREEN_R);
    gfx::setColor(1, 1, 1);
    const Font& big = kit::font(2);
    const Font& small = kit::font(0);
    int mid = SCREEN_R.x + SCREEN_R.w / 2;
    if (picked >= 0) {
      kit::centred(big.colours(YELLOW, BLACK), labelOf(picked), mid, SCREEN_R.y + 14);
      kit::centred(small, padmap::bound(picked).label, mid, SCREEN_R.y + 40);
      kit::centred(small.colours(1, BLACK), "pick what it does", mid, SCREEN_R.y + 68);
    } else {
      kit::centred(big, "Controller", mid, SCREEN_R.y + 14);
      kit::centred(small, "Press a button,", mid, SCREEN_R.y + 44);
      kit::centred(small, "or pick one", mid, SCREEN_R.y + 59);
    }
    // the trackpads, and Steam's two buttons, which are Steam's own
    for (bool rightHand : {false, true}) {
      kit::Rect t = TRACKPAD;
      if (rightHand) t.x = mirror(t);
      bool lit = rightHand && picked == pad::RIGHT_STICK;
      rounded(BLACK, {t.x - 1, t.y - 1, t.w + 2, t.h + 2}, 3, 3);
      rounded(lit ? YELLOW : DARK, t, 2, 2);
      if (rightHand) padglyph::text("R3", t.x + t.w / 2 - 3, t.y + t.h / 2 - 2, lit ? BLACK : WHITE);
      int sx = rightHand ? 2 * MIDDLE - 234 : 234;
      disc(BLACK, sx, BODY_R.y + 96, 3);
    }
    // the D-pad
    fill(BLACK, {DPAD_X + ARM - 1, DPAD_Y - 1, ARM + 2, 3 * ARM + 2});
    fill(BLACK, {DPAD_X - 1, DPAD_Y + ARM - 1, 3 * ARM + 2, ARM + 2});
    fill(DARK, {DPAD_X + ARM, DPAD_Y, ARM, 3 * ARM});
    fill(DARK, {DPAD_X, DPAD_Y + ARM, 3 * ARM, ARM});
    for (const Spot& s : where) {
      bool lit = s.button == picked;
      const kit::Rect& r = s.r;
      switch (s.button) {
        case pad::DPAD_UP:
        case pad::DPAD_DOWN:
        case pad::DPAD_LEFT:
        case pad::DPAD_RIGHT: {
          if (lit) fill(YELLOW, r);
          // an arrowhead, pointing the arm's way
          kit::setPal(lit ? BLACK : WHITE);
          int cx = r.x + ARM / 2, cy = r.y + ARM / 2;
          for (int k = 0; k < 3; k++) {
            if (s.button == pad::DPAD_UP) gfx::rectangle(gfx::FILL, cx - k, cy - 1 + k, 2 * k + 1, 1);
            else if (s.button == pad::DPAD_DOWN) gfx::rectangle(gfx::FILL, cx - k, cy + 1 - k, 2 * k + 1, 1);
            else if (s.button == pad::DPAD_LEFT) gfx::rectangle(gfx::FILL, cx - 1 + k, cy - k, 1, 2 * k + 1);
            else gfx::rectangle(gfx::FILL, cx + 1 - k, cy - k, 1, 2 * k + 1);
          }
          break;
        }
        case pad::LEFT_STICK:
        case pad::RIGHT_STICK:
          if (r.w != 2 * STICK_R + 1) break;      // the trackpad, drawn above
          disc(BLACK, r.x + STICK_R, r.y + STICK_R, STICK_R + 1);
          disc(lit ? YELLOW : DARK, r.x + STICK_R, r.y + STICK_R, STICK_R - 1);
          padglyph::text(s.button == pad::LEFT_STICK ? "L3" : "R3", r.x + STICK_R - 3, r.y + STICK_R - 2,
                         lit ? BLACK : WHITE);
          break;
        case pad::LEFT_SHOULDER:
        case pad::RIGHT_SHOULDER:
        case pad::LEFT_TRIGGER:
        case pad::RIGHT_TRIGGER: break;           // drawn above, under the body
        default: padglyph::draw(s.button, r.x, r.y, lit ? YELLOW : BLACK, lit ? BLACK : WHITE);
      }
    }
  }

  void drawList(const int* hand, int x) {
    const Font& f = kit::font(0);
    for (int i = 0; i < 10; i++) {
      int b = hand[i], y = LIST_Y + ROW * i;
      bool lit = b == picked;
      padglyph::draw(b, x + (13 - padglyph::width(b)) / 2, y + 2, lit ? YELLOW : BLACK, lit ? BLACK : WHITE);
      const padmap::Action& a = padmap::bound(b);
      gfx::setColor(1, 1, 1);
      gfx::setScissor(x, y, LIST_W, ROW);
      (lit ? f.colours(YELLOW, BLACK) : a.kind == padmap::NOTHING ? f.colours(1, BLACK) : f).draw(a.label, x + 17, y);
      gfx::setScissor();
    }
  }

  void drawGrid() {
    const Font& f = kit::font(0);
    const auto& all = padmap::actions();
    const padmap::Action* now = picked >= 0 ? &padmap::bound(picked) : nullptr;
    for (size_t i = 0; i < all.size(); i++) {
      int x = GRID_X + GRID_W * ((int)i / GRID_ROWS), y = GRID_Y + ROW * ((int)i % GRID_ROWS);
      gfx::setColor(1, 1, 1);
      (&all[i] == now ? f.colours(YELLOW, BLACK) : f).draw(all[i].label, x + 16, y);
      int b = all[i].kind == padmap::NOTHING ? -1 : padmap::buttonFor(all[i].id);
      if (b >= 0) padglyph::draw(b, x + (13 - padglyph::width(b)) / 2, y + 2);
    }
  }

  static void box(bool on, const kit::Rect& r, const char* label) {
    gfx::setColor(1, 1, 1);
    gfx::draw(G.abits, gfx::newQuad(320, on ? 0 : 20, 24, 20), r.x, r.y);
    gfx::setColor(1, 1, 1);
    kit::font(0).draw(label, r.x + 28, r.y + 3);
  }

  void draw() override {
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), "Controller", MIDDLE, R.y + 2);
    drawDeck();
    drawList(LEFT_HAND, LEFT_X);
    drawList(RIGHT_HAND, RIGHT_X);
    kit::bevel(R.x + 8, GRID_Y - 5, R.w - 16, 2, 4, 1);
    drawGrid();
    kit::bevel(R.x + 8, FOOT_Y - 6, R.w - 16, 2, 4, 1);
    box(padmap::hints(), HINTS_BOX, "Symbols on screen");
    box(padmap::swapped(), SWAP_BOX, "Left stick is the pointer");
    fill(DARK, DEFAULTS);
    kit::bevel(DEFAULTS.x, DEFAULTS.y, DEFAULTS.w, DEFAULTS.h, 1, 4);
    kit::setPal(BLACK);
    kit::outline(DEFAULTS.x - 1, DEFAULTS.y - 1, DEFAULTS.w + 2, DEFAULTS.h + 2);
    gfx::setColor(1, 1, 1);
    kit::font(0).drawCentred("Defaults", DEFAULTS.x, DEFAULTS.y, DEFAULTS.w, DEFAULTS.h);
    kit::drawControls(v);
  }

  // ------------------------------------------------------------------ input
  void changed() { G.haveLayout = false; }      // the menus show the symbols too

  void give(const padmap::Action& a) {
    if (picked < 0) {
      sound::effect("chord");
      return;
    }
    // the pointer's click is how every dialog is worked: one button keeps it
    const padmap::Action& was = padmap::bound(picked);
    if (std::string(was.id) == "click" && std::string(a.id) != "click" && padmap::count("click") == 1) {
      sound::effect("chord");
      return;
    }
    padmap::bind(picked, a.id);
    changed();
  }

  void mousepressed(int x, int y, int button) override {
    if (button != 1) return;
    auto c = kit::controlAt(v, x, y);
    if (c && c->id == OK) {
      kit::pop(this);
      return;
    }
    if (DEFAULTS.contains(x, y)) {
      padmap::reset();
      changed();
      return;
    }
    if (kit::Rect{HINTS_BOX.x, HINTS_BOX.y, 150, HINTS_BOX.h}.contains(x, y)) {
      padmap::setHints(!padmap::hints());
      changed();
      return;
    }
    if (kit::Rect{SWAP_BOX.x, SWAP_BOX.y, 190, SWAP_BOX.h}.contains(x, y)) {
      padmap::setSwapped(!padmap::swapped());
      return;
    }
    for (const Spot& s : where) {
      if (s.r.contains(x, y)) {
        picked = s.button;
        return;
      }
    }
    for (int i = 0; i < 10; i++) {
      int ly = LIST_Y + ROW * i;
      if (y < ly || y >= ly + ROW) continue;
      if (x >= LEFT_X && x < LEFT_X + LIST_W) picked = LEFT_HAND[i];
      else if (x >= RIGHT_X && x < RIGHT_X + LIST_W) picked = RIGHT_HAND[i];
    }
    const auto& all = padmap::actions();
    if (x >= GRID_X && y >= GRID_Y && y < GRID_Y + ROW * GRID_ROWS) {
      size_t i = (size_t)((x - GRID_X) / GRID_W * GRID_ROWS + (y - GRID_Y) / ROW);
      if (i < all.size()) give(all[i]);
    }
  }

  bool padpressed(int button) override {
    if (!*labelOf(button)) return false;
    if (std::string(padmap::bound(button).id) == "click") return false;
    picked = button;
    return true;
  }

  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") kit::pop(this);
  }
};
}  // namespace

void open() {
  auto d = std::make_shared<Config>();
  d->v = kit::view(OK_DIALOG);
  // Shortcuts' OK button alone, at this dialog's foot
  auto& cs = d->v.dialog.controls;
  cs.erase(std::remove_if(cs.begin(), cs.end(), [](const w2::uidata::Control& c) { return c.id != OK; }), cs.end());
  for (auto& c : cs) {
    c.x = R.x + R.w - 12 - c.w;
    c.y = FOOT_Y + 10 - c.h / 2;
  }
  d->v.state[OK] = w2::uidata::NORMAL;
  kit::push(d);
}

}  // namespace padconfig
