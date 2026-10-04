#include "platform/gfx.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>

namespace gfx {

namespace {
SDL_Renderer* R = nullptr;
Color color;
struct T {
  double sx = 1, sy = 1, tx = 0, ty = 0;   // x' = sx * x + tx, in device pixels
};
T t;
std::vector<T> stack;
bool clipped = false;
SDL_Rect clip{};

inline int dev(double v) { return (int)std::floor(v + 0.5); }

void applyColor(SDL_Texture* tex) {
  auto c8 = [](float v) { return (Uint8)std::clamp((int)std::lround(v * 255), 0, 255); };
  SDL_SetTextureColorMod(tex, c8(color.r), c8(color.g), c8(color.b));
  SDL_SetTextureAlphaMod(tex, c8(color.a));
}

void setDrawColor() {
  auto c8 = [](float v) { return (Uint8)std::clamp((int)std::lround(v * 255), 0, 255); };
  SDL_SetRenderDrawColor(R, c8(color.r), c8(color.g), c8(color.b), c8(color.a));
  SDL_SetRenderDrawBlendMode(R, color.a < 1 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
}

// a rect in the current transform, on whole device pixels
SDL_Rect devRect(double x, double y, double w, double h) {
  double x1 = t.sx * x + t.tx, y1 = t.sy * y + t.ty;
  double x2 = t.sx * (x + w) + t.tx, y2 = t.sy * (y + h) + t.ty;
  int ix1 = dev(std::min(x1, x2)), iy1 = dev(std::min(y1, y2));
  int ix2 = dev(std::max(x1, x2)), iy2 = dev(std::max(y1, y2));
  return SDL_Rect{ix1, iy1, ix2 - ix1, iy2 - iy1};
}

void blit(const Image* img, int sx, int sy, int sw, int sh, double dx, double dy) {
  if (sw <= 0 || sh <= 0) return;
  SDL_Rect src{sx, sy, sw, sh};
  SDL_Rect dst = devRect(dx, dy, sw, sh);
  if (dst.w <= 0 || dst.h <= 0) return;
  SDL_RenderCopy(R, img->tex, &src, &dst);
}
}  // namespace

Image::~Image() {
  if (tex) SDL_DestroyTexture(tex);
}

void init(SDL_Renderer* r) {
  R = r;
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
}

SDL_Renderer* renderer() { return R; }

ImageP newImageRGBA(int w, int h, const uint8_t* rgba) {
  auto img = std::make_shared<Image>();
  img->w = w;
  img->h = h;
  if (w <= 0 || h <= 0 || !R) return img;
  img->tex = SDL_CreateTexture(R, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, w, h);
  if (img->tex) {
    SDL_UpdateTexture(img->tex, nullptr, rgba, w * 4);
    SDL_SetTextureBlendMode(img->tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(img->tex, SDL_ScaleModeNearest);
  }
  return img;
}

void begin() {
  setScissor();
  t = T{};
  stack.clear();
  setColor(1, 1, 1);
}

void clear(float r, float g, float b) {
  SDL_RenderSetClipRect(R, nullptr);
  SDL_SetRenderDrawBlendMode(R, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(R, (Uint8)(r * 255), (Uint8)(g * 255), (Uint8)(b * 255), 255);
  SDL_RenderClear(R);
  if (clipped) SDL_RenderSetClipRect(R, &clip);
}

void push() { stack.push_back(t); }
void pop() {
  if (stack.empty()) { t = T{}; return; }
  t = stack.back();
  stack.pop_back();
}
void origin() { t = T{}; }
void translate(double x, double y) { t.tx += t.sx * x; t.ty += t.sy * y; }
void scale(double s) { scale(s, s); }
void scale(double sx, double sy) { t.sx *= sx; t.sy *= sy; }
void transformPoint(double x, double y, double& ox, double& oy) { ox = t.sx * x + t.tx; oy = t.sy * y + t.ty; }
void inverseTransformPoint(double x, double y, double& ox, double& oy) { ox = (x - t.tx) / t.sx; oy = (y - t.ty) / t.sy; }

void setColor(float r, float g, float b, float a) { color = Color{r, g, b, a}; }
void setColor(const Color& c) { color = c; }
Color getColor() { return color; }

void setScissor(double x, double y, double w, double h) {
  clip = devRect(x, y, w, h);
  clipped = true;
  SDL_RenderSetClipRect(R, &clip);
}
void setScissor() {
  clipped = false;
  if (R) SDL_RenderSetClipRect(R, nullptr);
}

void rectangle(Mode mode, double x, double y, double w, double h) {
  setDrawColor();
  if (mode == FILL) {
    SDL_Rect r = devRect(x, y, w, h);
    if (r.w > 0 && r.h > 0) SDL_RenderFillRect(R, &r);
    return;
  }
  SDL_Rect rs[4] = {devRect(x - 0.5, y - 0.5, w + 1, 1), devRect(x - 0.5, y + h - 0.5, w + 1, 1),
                    devRect(x - 0.5, y - 0.5, 1, h + 1), devRect(x + w - 0.5, y - 0.5, 1, h + 1)};
  for (auto& r : rs) if (r.w > 0 && r.h > 0) SDL_RenderFillRect(R, &r);
}

void line(double x1, double y1, double x2, double y2) {
  // only ever straight in this game: a one-pixel rect along it
  if (y1 == y2) rectangle(FILL, std::min(x1, x2), y1 - 0.5, std::fabs(x2 - x1), 1);
  else if (x1 == x2) rectangle(FILL, x1 - 0.5, std::min(y1, y2), 1, std::fabs(y2 - y1));
  else {
    setDrawColor();
    double ax, ay, bx, by;
    transformPoint(x1, y1, ax, ay);
    transformPoint(x2, y2, bx, by);
    SDL_RenderDrawLine(R, dev(ax), dev(ay), dev(bx), dev(by));
  }
}

void draw(const Image* img, double x, double y) {
  if (!img || !img->tex) return;
  applyColor(img->tex);
  blit(img, 0, 0, img->w, img->h, x, y);
}

void draw(const Image* img, const Quad& q, double x, double y) {
  if (!img || !img->tex) return;
  applyColor(img->tex);
  int qx = (int)q.x, qy = (int)q.y, qw = (int)q.w, qh = (int)q.h;
  if (img->wrap && (qx + qw > img->w || qy + qh > img->h)) {
    // a repeating image, as LÖVE draws one with a quad bigger than it
    for (int yy = 0; yy < qh; yy += img->h) {
      for (int xx = 0; xx < qw; xx += img->w) {
        int w = std::min(img->w, qw - xx), h = std::min(img->h, qh - yy);
        blit(img, 0, 0, w, h, x + xx, y + yy);
      }
    }
    return;
  }
  // clip the source to the image: a quad past its edge draws nothing there
  int sx = std::max(0, qx), sy = std::max(0, qy);
  int sw = std::min(img->w, qx + qw) - sx, sh = std::min(img->h, qy + qh) - sy;
  blit(img, sx, sy, sw, sh, x + (sx - qx), y + (sy - qy));
}

std::vector<uint8_t> readPixels(int& w, int& h) {
  SDL_GetRendererOutputSize(R, &w, &h);
  std::vector<uint8_t> px((size_t)w * h * 4);
  SDL_RenderSetClipRect(R, nullptr);
  SDL_RenderReadPixels(R, nullptr, SDL_PIXELFORMAT_ABGR8888, px.data(), w * 4);
  if (clipped) SDL_RenderSetClipRect(R, &clip);
  return px;
}

}  // namespace gfx
