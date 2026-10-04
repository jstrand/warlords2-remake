// The screen, in real pixels, and the two scales everything is drawn at.
//
// Not the original's: it drew 640x480 and nothing else. Three kinds of
// coordinate, as in the Lua remake:
//
//   device  the window's own pixels -- on a high-density display, more than
//           the window's size in points, so nothing resamples them
//   UI      the interface's pixels: every rect in the game's data. One UI
//           pixel is `scale` device pixels across, a whole number
//   map     the map's own pixels, 40 to a tile, drawn at `zoom` device
//           pixels each, apart from the UI's
//
// Handlers and hit tests all work in UI pixels: the mouse is brought down to
// them before the front end sees it.
#pragma once

#include <optional>
#include <utility>

struct SDL_Window;

namespace display {

constexpr int W = 640, H = 480;   // the original's screen, and the smallest

struct State {
  double dpi = 1;            // device pixels to a window point
  int pw = 640, ph = 480;    // the window, in device pixels
  int scale = 1;
  int maxScale = 1;
  int chosen = 0;            // a scale picked from the menu, 0 for none
  bool windowed = false;     // in a window on the desktop, not full screen
  std::optional<std::pair<int, int>> wanted;   // the frame to keep to: View > 4:3
  int w = 640, h = 480;      // the frame being drawn, in UI pixels
  int ox = 0, oy = 0;        // where its (0, 0) sits, in device pixels
  bool mouseInside = true;
};
extern State d;

/** Open the game's window: full screen on the desktop, or a window. */
SDL_Window* open(const char* title, bool windowed);
SDL_Window* window();
/** Read the window's size, and pick the UI's scale: the one chosen from the
 *  menu, or else the biggest whole number that still fits 640x480. */
void measure();
/** Draw the UI at `n` device pixels a pixel from now on, as far as it fits. */
void choose(int n);
/** How many UI pixels the game gets. */
std::pair<int, int> uiSize();
/** Draw a frame of w x h UI pixels from here on, centred on the screen. */
void setFrame(int w, int h);
/** Draw in UI pixels until the matching gfx::pop. */
void pushUI();
/** Draw in map pixels until the matching pop, one map pixel to `zoom` device
 *  pixels, slid `camX, camY` device pixels from the UI point (x, y). */
void pushMap(int x, int y, int camX, int camY, int zoom);
/** A window position (in points) as a UI point, held to the frame. */
std::pair<int, int> toUI(double x, double y);
/** Is a window position on the frame? */
bool onFrame(double x, double y);
/** Full screen or a window (View > Full screen, Window). */
void setWindowed(bool on);
/** Is the pointer off the game, so that it draws none? */
bool pointerAway();

}  // namespace display
