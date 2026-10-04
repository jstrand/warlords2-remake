// Decoded .PCK sheets as images in a palette, with colours keyed out or
// remapped as the original blits them. See docs/formats/pck.md.
#pragma once

#include <map>
#include <set>
#include <string>

#include "platform/gfx.hpp"
#include "warlords/pal.hpp"
#include "warlords/pck.hpp"

namespace pckimage {

/** An image from decoded indices, each passed through `map` (index -> index)
 *  if given; `keys` is the sheet's own indices to leave clear. This is how a
 *  font is drawn in colours other than its own (78a8:0839). */
gfx::ImageP fromPixels(const w2::pck::Pixels& px, const w2::pal::Palette& palette,
                       const std::map<int, int>& map = {}, const std::set<int>& keys = {});

constexpr int CORNER = -2;   // key out whatever colour the sheet's top left is
constexpr int NOKEY = -1;

/** Decode a .PCK into an image; `key` is the index to make transparent,
 *  NOKEY or CORNER. */
gfx::ImageP load(const std::string& path, const w2::pal::Palette& palette, int key = NOKEY);
gfx::ImageP load(const std::string& path, const w2::pal::Palette& palette, const std::set<int>& keys);

}  // namespace pckimage
