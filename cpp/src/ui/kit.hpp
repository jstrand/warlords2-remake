// The pieces every dialog is made of, and the stack they are shown on.
//
// A popup (54f6:0000) is a black outline and a two-pixel shadow round a crop
// of MARBLE.PCK, text is drawn centred (7ecb:00d6) or from its left edge, an
// army by 8611:08be with a ring under it, a shield by 8611:0bf7, and a
// dialog's buttons are its BUTTON.DAT controls.
//
// A dialog is a Modal: draw() and, as it needs them, the input handlers.
// kit::push puts one on top; the front end sends input to the top one only.
#pragma once

#include <functional>
#include <memory>
#include <set>
#include <string>

#include "front/app.hpp"
#include "front/font.hpp"
#include "front/layout.hpp"
#include "front/screen.hpp"

namespace kit {

using layout::Rect;
using Done = std::function<void()>;

struct Modal : std::enable_shared_from_this<Modal> {
  virtual ~Modal() = default;
  virtual void draw() = 0;
  virtual void update() {}
  virtual void mousepressed(int x, int y, int button) {}
  /** True when the dialog took the release (else the screen's buttons do). */
  virtual bool mousereleased(int x, int y, int button) { return false; }
  virtual void keypressed(const std::string& key) {}
  virtual void textinput(const std::string& text) {}
  /** The right button on the dialog, where no control's help answered. */
  virtual void rightpressed(int fx, int fy, int sx, int sy) {}
  /** A help record's sub-id that stands for something on this dialog
   *  (infobox::control): true when a box went up. */
  virtual bool info(int sub, int sx, int sy) { return false; }
  screen::View* view = nullptr;   // the controls the right button can ask about
  std::set<int> hidden;           // controls taken off the dialog's face
  bool menuBar = false;           // the menu bar stays live over it
  bool infobox = false;
};
using ModalP = std::shared_ptr<Modal>;

ModalP push(ModalP d);
void pop(const Modal* d);
Modal* top();
/** The top dialog, held: a handler may pop it while it runs. */
ModalP topShared();

/** Set a palette colour by its index, 0-15. */
void setPal(int i);
/** A one-pixel outline, x..x+w-1 by y..y+h-1, in the current colour. */
void outline(double x, double y, double w, double h);
/** The frame 54f6:0000 gives a popup. */
void popupFrame(const Rect& R);
/** A popup with no picture of its own: MARBLE.PCK cropped to the rect. */
void popup(const Rect& R);
/** 78a8:06ae picks the font by number: 1 is CHANCE36, 2 CHANCE17, else TEXT. */
const Font& font(int n);
/** Centred on x, with y the top of the line (7ecb:00d6). */
void centred(const Font& f, const std::string& text, double x, double y);
/** Right-aligned, ending at x (7ecb:0103). */
void right(const Font& f, const std::string& text, double x, double y);
/** A one-pixel bevel (24d0:02e5). */
void bevel(double x, double y, double w, double h, int a, int b);
/** A text field (7ecb:0058). */
void field(double x, double y, double w, double h, const std::string* text, const Font* f = nullptr);
inline void field(double x, double y, double w, double h, const std::string& text, const Font* f = nullptr) {
  field(x, y, w, h, &text, f);
}
/** An army as 8611:08be draws one: the ring (0 none, 1 grey, 2-9 a side's),
 *  then the army's cell -- of ASHADOW.PCK when `shadow`. */
void army(int typeId, int side, double x, double y, int ring = 0, bool shadow = false);
/** A side's big shield (8611:0bf7): SHIELDS.PCK's 40x40 cell. */
void shield(int side, double x, double y);
/** A one-pixel line in palette colour c, end to end (Bresenham). */
void line(int c, int x0, int y0, int x1, int y1);
/** The way to a place on a strategic map drawn at (mx, my) (828e:0a3f). */
void mapTarget(int mx, int my, int hx, int hy, int tx, int ty);
/** A dialog's BUTTON.DAT controls, each in its own state. */
screen::View view(int dialogId);
void drawControls(const screen::View& v, const std::set<int>* hidden = nullptr);
inline void drawControls(const screen::View& v, const std::set<int>& hidden) { drawControls(v, &hidden); }
/** The live control under a point: not disabled and not hidden. */
const w2::uidata::Control* controlAt(const screen::View& v, int x, int y, const std::set<int>* hidden = nullptr);
inline const w2::uidata::Control* controlAt(const screen::View& v, int x, int y, const std::set<int>& hidden) {
  return controlAt(v, x, y, &hidden);
}
const w2::uidata::Control* control(const screen::View& v, int id);
/** A string from STRING.DAT, numbered as get_string numbers them. */
std::string text(int group, int i);
/** An army sheet's cell for a type. */
gfx::Quad armyQuad(int typeId);
/** The time, in seconds. */
double now();

}  // namespace kit
