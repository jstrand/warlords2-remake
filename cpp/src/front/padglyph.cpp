#include "front/padglyph.hpp"

#include <map>

#include "platform/gfx.hpp"
#include "platform/pad.hpp"
#include "ui/kit.hpp"

namespace padglyph {

namespace {
// five rows of three bits, the left column the high bit
const std::map<char, int> LETTERS = {
    {'A', 0b010'101'111'101'101}, {'B', 0b110'101'110'101'110}, {'X', 0b101'101'010'101'101},
    {'Y', 0b101'101'010'010'010}, {'L', 0b100'100'100'100'111}, {'R', 0b110'101'110'101'101},
    {'1', 0b010'110'010'010'111}, {'2', 0b110'001'010'100'111}, {'3', 0b110'001'010'001'110},
    {'4', 0b101'101'111'001'001}, {'5', 0b111'100'110'001'110},
};

void dot(double x, double y, double w = 1, double h = 1) { gfx::rectangle(gfx::FILL, x, y, w, h); }

// A disc 11 across: how far each row is cut in from the sides.
const int DISC[H] = {3, 2, 1, 0, 0, 0, 0, 0, 1, 2, 3};
void disc(double x, double y) {
  for (int r = 0; r < H; r++) dot(x + DISC[r], y + r, 11 - 2 * DISC[r]);
}
// A lozenge 13 across, its corners rounded off.
const int LOZENGE[H] = {2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2};
void lozenge(double x, double y) {
  for (int r = 0; r < H; r++) dot(x + LOZENGE[r], y + r, 13 - 2 * LOZENGE[r]);
}

const char* nameOf(int button) {
  switch (button) {
    case pad::A: return "A";
    case pad::B: return "B";
    case pad::X: return "X";
    case pad::Y: return "Y";
    case pad::LEFT_SHOULDER: return "L1";
    case pad::RIGHT_SHOULDER: return "R1";
    case pad::LEFT_TRIGGER: return "L2";
    case pad::RIGHT_TRIGGER: return "R2";
    case pad::LEFT_STICK: return "L3";
    case pad::RIGHT_STICK: return "R3";
    case pad::PADDLE2: return "L4";
    case pad::PADDLE4: return "L5";
    case pad::PADDLE1: return "R4";
    case pad::PADDLE3: return "R5";
  }
  return "";
}

bool face(int b) { return b == pad::A || b == pad::B || b == pad::X || b == pad::Y; }
bool dpad(int b) { return b == pad::DPAD_UP || b == pad::DPAD_DOWN || b == pad::DPAD_LEFT || b == pad::DPAD_RIGHT; }
}  // namespace

int width(int button) { return face(button) || dpad(button) ? 11 : 13; }

void text(const std::string& s, double x, double y, int ink) {
  kit::setPal(ink);
  for (char c : s) {
    auto l = LETTERS.find(c);
    if (l != LETTERS.end()) {
      for (int r = 0; r < 5; r++)
        for (int k = 0; k < 3; k++)
          if (l->second >> (14 - r * 3 - k) & 1) dot(x + k, y + r);
    }
    x += 4;
  }
}

void draw(int button, double x, double y, int fill, int ink) {
  kit::setPal(fill);
  if (face(button)) {
    disc(x, y);
    text(nameOf(button), x + 4, y + 3, ink);
  } else if (dpad(button)) {
    // the cross, five wide, and the arm that is meant
    dot(x + 3, y, 5, 11);
    dot(x, y + 3, 11, 5);
    kit::setPal(ink);
    if (button == pad::DPAD_UP) dot(x + 4, y + 1, 3, 3);
    else if (button == pad::DPAD_DOWN) dot(x + 4, y + 7, 3, 3);
    else if (button == pad::DPAD_LEFT) dot(x + 1, y + 4, 3, 3);
    else dot(x + 7, y + 4, 3, 3);
  } else if (button == pad::BACK) {
    // View: one window over another
    lozenge(x, y);
    kit::setPal(ink);
    kit::outline(x + 3, y + 2, 6, 5);
    dot(x + 5, y + 4, 6, 5);
  } else if (button == pad::START) {
    // Menu: three lines
    lozenge(x, y);
    kit::setPal(ink);
    for (int r = 0; r < 3; r++) dot(x + 3, y + 3 + 2 * r, 7);
  } else {
    lozenge(x, y);
    text(nameOf(button), x + 3, y + 3, ink);
  }
  gfx::setColor(1, 1, 1);
}

}  // namespace padglyph
