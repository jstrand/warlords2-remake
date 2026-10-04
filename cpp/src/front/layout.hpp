// Where the main screen's pieces go on a screen of any size.
//
// Not the original's: it only ever had 640x480. This keeps every piece of
// that screen at its own size and moves it as a whole, tied to an edge of the
// bigger screen, and gives all the room left over to the map. The popups are
// drawn in a 640x480 frame of their own, centred, so every dialog keeps the
// coordinates the original gives it. At 640x480 every offset is zero.
#pragma once

#include <string>

namespace layout {

constexpr int W = 640, H = 480;

struct Rect {
  int x = 0, y = 0, w = 0, h = 0;
  int id = 0;
  bool contains(int px, int py) const { return px >= x && py >= y && px < x + w && py < y + h; }
};
struct Offset { int x = 0, y = 0; };

struct Layout {
  int w = W, h = H, ex = 0, ey = 0;
  bool classic = true;
  Offset strat, panel, bar, map, dialog;
  Rect mapRect, panelRect, stratRect, barRect;
  const Offset& offset(const std::string& group) const;
};

/** The layout of a screen `w` x `h` UI pixels. */
Layout compute(int w, int h);
/** Which group a rect of the original screen belongs to. */
std::string group(int x, int y);
/** A copy of the original rect `r`, moved with group `g`. */
Rect move(const Layout& L, const std::string& g, Rect r);
/** One of the main screen's regions, placed: the map, its frame and the menu
 *  bar stretch, the rest move with their group. */
Rect region(const Layout& L, Rect r);

}  // namespace layout
