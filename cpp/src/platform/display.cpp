#include "platform/display.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>

#include "platform/gfx.hpp"

namespace display {

State d;

namespace {
SDL_Window* win = nullptr;
SDL_Texture* canvas = nullptr;   // the frame, when it is bigger than the window
bool noCanvas = false;           // the renderer cannot draw to one
}

SDL_Window* open(const char* title, bool windowed, int sizeW, int sizeH) {
  Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
  SDL_DisplayMode dm;
  int w = 1280, h = 960;
  if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
    w = std::max(W, dm.w * 4 / 5);
    h = std::max(H, dm.h * 4 / 5);
  }
  bool sized = sizeW > 0 && sizeH > 0;
  if (sized) {
    windowed = true;
    w = sizeW;
    h = sizeH;
  }
  if (!windowed) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
  if (win && sized) {
    // the size is in device pixels: on a high-density display, fewer points
    int ww, wh, pw, ph;
    SDL_GetWindowSize(win, &ww, &wh);
    SDL_GetWindowSizeInPixels(win, &pw, &ph);
    double k = ww > 0 ? (double)pw / ww : 1;
    w = (int)std::lround(sizeW / k);
    h = (int)std::lround(sizeH / k);
    SDL_SetWindowMinimumSize(win, std::min(W, w), std::min(H, h));
    SDL_SetWindowSize(win, w, h);
    SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  } else if (win) {
    SDL_SetWindowMinimumSize(win, W, H);
  }
  d.windowed = windowed;
  return win;
}

SDL_Window* window() { return win; }

void measure() {
  int ww = W, wh = H;
  if (win) SDL_GetWindowSize(win, &ww, &wh);
  int rw = ww, rh = wh;
  // the window's own pixels, not the renderer's output: with the canvas set
  // as the target, SDL2 gives the canvas's size for that
  if (win) SDL_GetWindowSizeInPixels(win, &rw, &rh);
  double fit = std::min((double)rw / W, (double)rh / H);
  d.maxScale = std::max(1, (int)fit);
  // the smooth view, the next scale up drawn whole and shrunk to fit, when
  // the biggest whole one leaves a quarter over
  d.smoothScale = !noCanvas && fit > 1 && d.maxScale / fit < 0.75 ? fit : 0;
  d.smooth = d.smoothScale > 0 && (d.chosen == SMOOTH || d.chosen == 0);
  d.scale = d.smooth ? d.maxScale + 1 : std::min(d.chosen > 0 ? d.chosen : d.maxScale, d.maxScale);
  d.shrink = d.smooth ? fit / d.scale : 1;
  d.pw = d.shrink < 1 ? std::max(W * d.scale, (int)std::lround(rw / d.shrink)) : rw;
  d.ph = d.shrink < 1 ? std::max(H * d.scale, (int)std::lround(rh / d.shrink)) : rh;
  d.dpi = ww > 0 ? (double)d.pw / ww : 1;
}

void beginFrame() {
  SDL_Renderer* r = gfx::renderer();
  if (!r) return;
  int cw = 0, ch = 0;
  if (canvas) SDL_QueryTexture(canvas, nullptr, nullptr, &cw, &ch);
  if (canvas && (d.shrink >= 1 || cw != d.pw || ch != d.ph)) {
    SDL_SetRenderTarget(r, nullptr);
    SDL_DestroyTexture(canvas);
    canvas = nullptr;
  }
  if (d.shrink >= 1) return;
  if (!canvas) {
    canvas = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, d.pw, d.ph);
    if (canvas) SDL_SetTextureScaleMode(canvas, SDL_ScaleModeLinear);
  }
  if (!canvas || SDL_SetRenderTarget(r, canvas) != 0) {
    if (canvas) SDL_DestroyTexture(canvas);
    canvas = nullptr;
    noCanvas = true;
    measure();
  }
}

void present() {
  SDL_Renderer* r = gfx::renderer();
  if (!r) return;
  if (!canvas || SDL_GetRenderTarget(r) != canvas) {
    SDL_RenderPresent(r);
    return;
  }
  SDL_SetRenderTarget(r, nullptr);
  SDL_RenderSetClipRect(r, nullptr);
  SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
  SDL_RenderClear(r);
  SDL_RenderCopy(r, canvas, nullptr, nullptr);
  SDL_RenderPresent(r);
  // left on the canvas, so a script's shot between frames reads it
  SDL_SetRenderTarget(r, canvas);
}

void choose(int n) {
  d.chosen = n == SMOOTH ? SMOOTH : std::max(1, n);
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

std::optional<std::pair<int, int>> offFrame(double x, double y) {
  double px = x * d.dpi, py = y * d.dpi;
  if (px >= d.ox && py >= d.oy && px < d.ox + d.w * d.scale && py < d.oy + d.h * d.scale) return std::nullopt;
  return std::make_pair((int)std::floor((px - d.ox) / d.scale), (int)std::floor((py - d.oy) / d.scale));
}

void setWindowed(bool on) {
  if (!win) { d.windowed = on; return; }
  SDL_SetWindowFullscreen(win, on ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
  d.windowed = on;
  measure();
}

bool pointerAway() { return d.windowed && !d.mouseInside; }

}  // namespace display
