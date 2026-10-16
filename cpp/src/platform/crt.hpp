// The frame as an old monitor showed it (View > CRT screen): not the
// original's, which drew plain pixels for whatever screen the player had.
//
// SDL's renderer runs no shaders, so the tube is made of what it does draw:
// the canvas laid on a mesh bowed like the glass, each UI row darkened at its
// edges into a scanline, the phosphor's red, green and blue stripes, a glow
// from a blurred copy of the frame, and the corners rounded and shaded. The
// rows, the glow and the shade follow the bow; the stripes are the screen's
// own, as a real mask is.
#pragma once

#include <utility>

struct SDL_Renderer;
struct SDL_Texture;

namespace crt {

/** Show the canvas on the window through the tube. Leaves the window as the
 *  target. */
void present(SDL_Renderer* r, SDL_Texture* canvas);
/** The canvas point that shows at a point on the window, both in device
 *  pixels: the bow undone, so a click lands on what it is over. */
std::pair<double, double> unwarp(double x, double y);

}  // namespace crt
