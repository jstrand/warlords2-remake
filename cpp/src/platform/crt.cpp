#include "platform/crt.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "platform/display.hpp"

namespace crt {

namespace {

// How far the glass bows: at a corner of the frame the canvas shows from this
// much further out, across and down, as a share of half the frame.
constexpr double BOW_X = 1.0 / 64, BOW_Y = 1.0 / 48;
constexpr int GRID_X = 64, GRID_Y = 48;   // the mesh's cells
constexpr double SCANLINE = 0.45;         // how dark between rows, at full strength
constexpr double MASK = 0.2;              // how much of the other colours a stripe holds back
constexpr int GLOW_NEAR = 70, GLOW_FAR = 90;   // the two halos added back, out of 255
constexpr double CORNER = 0.05, EDGE = 0.012;  // the glass's corner and its soft rim, in frame heights

struct Tex {
  SDL_Texture* t = nullptr;
  int w = 0, h = 0;
  double key = 0;   // what it was made for, besides its size
};

Tex half, quarter, eighth;   // the frame, smaller and smaller, for the glow
Tex lines;                   // one column: a UI row's brightness, top to bottom
Tex stripes;                 // one row: the mask, across the window
Tex shade;                   // the glass's darkening towards its rim

// (Re)make a texture of this size; true when it is new, so wants filling.
bool need(SDL_Renderer* r, Tex& x, int w, int h, int access, double key = 0) {
  w = std::max(1, w);
  h = std::max(1, h);
  if (x.t && x.w == w && x.h == h && x.key == key) return false;
  if (x.t) SDL_DestroyTexture(x.t);
  x.t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, access, w, h);
  x.w = w;
  x.h = h;
  x.key = key;
  if (x.t) SDL_SetTextureScaleMode(x.t, SDL_ScaleModeLinear);
  return x.t != nullptr;
}

void fill(Tex& x, const std::vector<Uint32>& px) {
  if (x.t) SDL_UpdateTexture(x.t, nullptr, px.data(), x.w * 4);
}

Uint32 grey(double v) {
  Uint32 c = (Uint32)std::clamp((int)std::lround(v * 255), 0, 255);
  return 0xff000000u | c << 16 | c << 8 | c;
}

// Where a canvas point shows on the glass: the bow's inverse, in the frame's
// own -1..1, found by going round until it settles.
std::pair<double, double> bow(double cx, double cy) {
  double sx = cx, sy = cy;
  for (int i = 0; i < 8; i++) {
    sx = cx / (1 + BOW_X * sy * sy);
    sy = cy / (1 + BOW_Y * sx * sx);
  }
  return {sx, sy};
}

// A UI row's brightness at `k` texels a row, `s` its strength: the beam
// lights the row's top k - 1 texels and leaves the last the darkest, so at
// two pixels a row it is one bright and one dark, as on the glass.
void makeLines(int rows, int k, double s) {
  std::vector<Uint32> px((size_t)rows * k);
  double mid = 0.5 - 0.5 / k;
  for (size_t i = 0; i < px.size(); i++) {
    double t = (i % k + 0.5) / k - mid;
    double beam = std::exp(-(t * t) / (0.3 * 0.3));
    px[i] = grey(1 - s * (1 - beam));
  }
  fill(lines, px);
}

// The aperture grille: red, green and blue stripes, each `sw` pixels wide.
void makeStripes(int sw) {
  std::vector<Uint32> px(stripes.w);
  Uint32 off = (Uint32)std::lround(255 * (1 - MASK));
  for (int x = 0; x < stripes.w; x++) {
    int c = x / sw % 3;
    Uint32 r = c == 0 ? 255 : off, g = c == 1 ? 255 : off, b = c == 2 ? 255 : off;
    px[x] = 0xff000000u | r << 16 | g << 8 | b;
  }
  fill(stripes, px);
}

// Dark at the glass's rounded rim, and a little towards it everywhere.
void makeShade(double aspect) {
  std::vector<Uint32> px((size_t)shade.w * shade.h);
  for (int y = 0; y < shade.h; y++) {
    for (int x = 0; x < shade.w; x++) {
      double u = (x + 0.5) / shade.w, v = (y + 0.5) / shade.h;
      // how far inside the rounded rim, in frame heights
      double qx = std::abs(u * 2 - 1) * aspect - (aspect - CORNER), qy = std::abs(v * 2 - 1) - (1 - CORNER);
      double out = std::hypot(std::max(qx, 0.0), std::max(qy, 0.0)) + std::min(std::max(qx, qy), 0.0) - CORNER;
      double rim = std::clamp(-out / EDGE, 0.0, 1.0);
      rim = rim * rim * (3 - 2 * rim);
      double vig = std::pow(std::max(0.0, 16 * u * (1 - u) * v * (1 - v)), 0.12);
      px[(size_t)y * shade.w + x] = grey(rim * vig);
    }
  }
  fill(shade, px);
}

}  // namespace

std::pair<double, double> unwarp(double x, double y) {
  const auto& d = display::d;
  double fw = d.w * d.scale, fh = d.h * d.scale;
  if (fw <= 0 || fh <= 0) return {x, y};
  double nx = (x - d.ox) / fw * 2 - 1, ny = (y - d.oy) / fh * 2 - 1;
  double cx = nx * (1 + BOW_X * ny * ny), cy = ny * (1 + BOW_Y * nx * nx);
  return {d.ox + (cx + 1) / 2 * fw, d.oy + (cy + 1) / 2 * fh};
}

void present(SDL_Renderer* r, SDL_Texture* canvas) {
  const auto& d = display::d;
  int fx = d.ox, fy = d.oy, fw = d.w * d.scale, fh = d.h * d.scale;
  SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_NONE);

  // the glow: the frame halved three times over, each step a box blur
  SDL_Rect frame{fx, fy, fw, fh};
  need(r, half, fw / 2, fh / 2, SDL_TEXTUREACCESS_TARGET);
  need(r, quarter, fw / 4, fh / 4, SDL_TEXTUREACCESS_TARGET);
  need(r, eighth, fw / 8, fh / 8, SDL_TEXTUREACCESS_TARGET);
  bool glow = half.t && quarter.t && eighth.t;
  if (glow) {
    SDL_RenderSetClipRect(r, nullptr);
    SDL_SetRenderTarget(r, half.t);
    SDL_RenderCopy(r, canvas, &frame, nullptr);
    SDL_SetTextureBlendMode(half.t, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(r, quarter.t);
    SDL_RenderCopy(r, half.t, nullptr, nullptr);
    SDL_SetTextureBlendMode(quarter.t, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(r, eighth.t);
    SDL_RenderCopy(r, quarter.t, nullptr, nullptr);
  }

  SDL_SetRenderTarget(r, nullptr);
  SDL_RenderSetClipRect(r, nullptr);
  int ow = 0, oh = 0;
  SDL_GetRendererOutputSize(r, &ow, &oh);
  double kx = (double)ow / d.pw, ky = (double)oh / d.ph;   // window pixels to a device pixel

  // off the frame there is only black, and the pointer when it is there
  SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
  SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
  SDL_RenderClear(r);
  SDL_RenderCopy(r, canvas, nullptr, nullptr);
  SDL_Rect onWindow{(int)std::floor(fx * kx), (int)std::floor(fy * ky), (int)std::ceil(fw * kx) + 1, (int)std::ceil(fh * ky) + 1};
  SDL_RenderFillRect(r, &onWindow);

  // the mesh: a grid on the canvas, each point where the glass shows it
  std::vector<SDL_Vertex> onCanvas, onFrame, onLines;
  std::vector<int> idx;
  int rowTexels = 2;
  double rowPx = d.scale * ky;   // window pixels to a UI row
  {
    SDL_RendererInfo info{};
    SDL_GetRendererInfo(r, &info);
    int maxH = info.max_texture_height > 0 ? info.max_texture_height : 8192;
    rowTexels = std::clamp((int)std::lround(rowPx), 2, 8);
    rowTexels = std::max(2, std::min(rowTexels, maxH / std::max(1, d.h)));
  }
  for (int j = 0; j <= GRID_Y; j++) {
    for (int i = 0; i <= GRID_X; i++) {
      double u = (double)i / GRID_X, v = (double)j / GRID_Y;
      auto [sx, sy] = bow(u * 2 - 1, v * 2 - 1);
      SDL_Vertex vx{};
      vx.position = {(float)((fx + (sx + 1) / 2 * fw) * kx), (float)((fy + (sy + 1) / 2 * fh) * ky)};
      vx.color = {255, 255, 255, 255};
      vx.tex_coord = {(float)((fx + u * fw) / d.pw), (float)((fy + v * fh) / d.ph)};
      onCanvas.push_back(vx);
      vx.tex_coord = {(float)u, (float)v};
      onFrame.push_back(vx);
      vx.tex_coord = {0.5f, (float)v};
      onLines.push_back(vx);
    }
  }
  for (int j = 0; j < GRID_Y; j++) {
    for (int i = 0; i < GRID_X; i++) {
      int a = j * (GRID_X + 1) + i, b = a + 1, c = a + GRID_X + 1, e = c + 1;
      idx.insert(idx.end(), {a, b, c, b, e, c});
    }
  }
  auto draw = [&](SDL_Texture* t, std::vector<SDL_Vertex>& vs, SDL_BlendMode mode, Uint8 tint = 255) {
    if (!t) return;
    for (SDL_Vertex& vx : vs) vx.color = {tint, tint, tint, 255};
    SDL_SetTextureBlendMode(t, mode);
    SDL_RenderGeometry(r, t, vs.data(), (int)vs.size(), idx.data(), (int)idx.size());
  };

  draw(canvas, onCanvas, SDL_BLENDMODE_NONE);

  // scanlines: on whole pixels at any scale, else fading out where a row is
  // too few pixels to show one without beating against them
  bool whole = rowPx >= 2 && std::abs(rowPx - std::round(rowPx)) < 0.01;
  double strength = SCANLINE * (whole ? 1 : std::clamp((rowPx - 1.5) / 1.5, 0.0, 1.0));
  if (need(r, lines, 1, d.h * rowTexels, SDL_TEXTUREACCESS_STATIC, rowTexels * 1000 + strength)) makeLines(d.h, rowTexels, strength);
  draw(lines.t, onLines, SDL_BLENDMODE_MOD);

  // the mask, a stripe to a pixel of the screen (more on a dense one)
  int sw = std::max(1, (int)std::lround(d.dpi * d.shrink));
  if (need(r, stripes, ow, 1, SDL_TEXTUREACCESS_STATIC, sw)) {
    SDL_SetTextureScaleMode(stripes.t, SDL_ScaleModeNearest);
    makeStripes(sw);
  }
  if (stripes.t) {
    SDL_Rect src{0, 0, std::min(onWindow.w, stripes.w), 1}, dst = onWindow;
    dst.w = src.w;
    SDL_SetTextureBlendMode(stripes.t, SDL_BLENDMODE_MOD);
    SDL_RenderCopy(r, stripes.t, &src, &dst);
  }

  if (glow) {
    draw(quarter.t, onFrame, SDL_BLENDMODE_ADD, GLOW_NEAR);
    draw(eighth.t, onFrame, SDL_BLENDMODE_ADD, GLOW_FAR);
  }

  double aspect = fh > 0 ? (double)fw / fh : 4.0 / 3;
  if (need(r, shade, 256, std::clamp((int)std::lround(256 / aspect), 64, 512), SDL_TEXTUREACCESS_STATIC, aspect)) makeShade(aspect);
  draw(shade.t, onFrame, SDL_BLENDMODE_MOD);
}

}  // namespace crt
