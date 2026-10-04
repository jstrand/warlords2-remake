#include "platform/display.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>

#include "platform/gfx.hpp"

namespace display {

State d;

namespace {
SDL_Window* win = nullptr;
}

SDL_Window* open(const char* title, bool windowed) {
  Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
  SDL_DisplayMode dm;
  int w = 1280, h = 960;
  if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
    w = std::max(W, dm.w * 4 / 5);
    h = std::max(H, dm.h * 4 / 5);
  }
  if (!windowed) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
  if (win) SDL_SetWindowMinimumSize(win, W, H);
  d.windowed = windowed;
  return win;
}

SDL_Window* window() { return win; }

void measure() {
  int ww = W, wh = H;
  if (win) SDL_GetWindowSize(win, &ww, &wh);
  int pw = ww, ph = wh;
  if (gfx::renderer()) SDL_GetRendererOutputSize(gfx::renderer(), &pw, &ph);
  d.dpi = ww > 0 ? (double)pw / ww : 1;
  d.pw = pw;
  d.ph = ph;
  d.maxScale = std::max(1, std::min(d.pw / W, d.ph / H));
  d.scale = std::min(d.chosen ? d.chosen : d.maxScale, d.maxScale);
}

void choose(int n) {
  d.chosen = std::max(1, n);
  measure();
}

std::pair<int, int> uiSize() {
  int w = d.pw / d.scale, h = d.ph / d.scale;
  if (d.wanted) {
    w = std::min(w, d.wanted->first);
    h = std::min(h, d.wanted->second);
  }
  return {w, h};
}

void setFrame(int w, int h) {
  d.w = w;
  d.h = h;
  d.ox = (d.pw - w * d.scale) / 2;
  d.oy = (d.ph - h * d.scale) / 2;
}

void pushUI() {
  gfx::push();
  gfx::origin();
  gfx::translate(d.ox, d.oy);
  gfx::scale(d.scale);
}

void pushMap(int x, int y, int camX, int camY, int zoom) {
  gfx::push();
  gfx::origin();
  gfx::translate(d.ox + x * d.scale - camX, d.oy + y * d.scale - camY);
  gfx::scale(zoom);
}

std::pair<int, int> toUI(double x, double y) {
  int ux = (int)std::floor((x * d.dpi - d.ox) / d.scale);
  int uy = (int)std::floor((y * d.dpi - d.oy) / d.scale);
  return {std::clamp(ux, 0, d.w - 1), std::clamp(uy, 0, d.h - 1)};
}

bool onFrame(double x, double y) {
  double px = x * d.dpi, py = y * d.dpi;
  return px >= d.ox && py >= d.oy && px < d.ox + d.w * d.scale && py < d.oy + d.h * d.scale;
}

void setWindowed(bool on) {
  if (!win) { d.windowed = on; return; }
  SDL_SetWindowFullscreen(win, on ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
  d.windowed = on;
  measure();
}

bool pointerAway() { return d.windowed && !d.mouseInside; }

}  // namespace display
