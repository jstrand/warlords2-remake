// The handful of love.graphics calls the Lua remake draws with, over
// SDL_Renderer, so the drawing code ports nearly line for line.
//
// As in LÖVE:
//   - push/pop save the transform only (translate and scale are all the game
//     uses); the scissor is a state of its own that survives them, and is
//     given in the current transform's coordinates.
//   - setColor tints images as well as filling shapes (SDL's colour and alpha
//     mod multiply, as LÖVE does).
//   - Images are nearest-neighbour, always.
// Everything lands on whole device pixels, so the art stays sharp blocks.
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;

namespace gfx {

struct Color {
  float r = 1, g = 1, b = 1, a = 1;
};

/** An image on the GPU, and its size. */
struct Image {
  SDL_Texture* tex = nullptr;
  int w = 0, h = 0;
  bool wrap = false;   // a quad bigger than the image repeats it
  Image() = default;
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
  ~Image();
  int width() const { return w; }
  int height() const { return h; }
};
using ImageP = std::shared_ptr<Image>;

/** A source rect into an image. */
struct Quad {
  double x = 0, y = 0, w = 0, h = 0;
};
inline Quad newQuad(double x, double y, double w, double h) { return Quad{x, y, w, h}; }

void init(SDL_Renderer* r);
SDL_Renderer* renderer();

/** An image from w * h RGBA bytes. */
ImageP newImageRGBA(int w, int h, const uint8_t* rgba);

/** Start a frame: no transform, no scissor, white. */
void begin();
/** Fill the whole target with a colour. */
void clear(float r = 0, float g = 0, float b = 0);

void push();
void pop();
void origin();
void translate(double x, double y);
void scale(double s);
void scale(double sx, double sy);
void transformPoint(double x, double y, double& ox, double& oy);
void inverseTransformPoint(double x, double y, double& ox, double& oy);

void setColor(float r, float g, float b, float a = 1);
void setColor(const Color& c);
Color getColor();

/** The scissor, in the coordinates of the current transform. */
void setScissor(double x, double y, double w, double h);
void setScissor();   // none

enum Mode { FILL, LINE };
/** A "line" rectangle is one pixel wide, centred on the rect's edge, as
 *  LÖVE's rough line style draws it. */
void rectangle(Mode mode, double x, double y, double w, double h);
void line(double x1, double y1, double x2, double y2);

void draw(const Image* img, double x, double y);
void draw(const Image* img, const Quad& q, double x, double y);
inline void draw(const ImageP& img, double x, double y) { draw(img.get(), x, y); }
inline void draw(const ImageP& img, const Quad& q, double x, double y) { draw(img.get(), q, x, y); }

/** The frame drawn so far, as RGBA rows: for screenshots. */
std::vector<uint8_t> readPixels(int& w, int& h);

}  // namespace gfx
