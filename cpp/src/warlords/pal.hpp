// Warlords II .PAL reader: plain ASCII, 16 lines of "RR GG BB", the
// components PERCENTAGES (0-99), not VGA 0-63. See docs/formats/pck.md.
#pragma once

#include <array>
#include <string>
#include <vector>

namespace w2::pal {

using Colour = std::array<double, 3>;
using Palette = std::vector<Colour>;

/** The palette as 16 {r, g, b} in 0..1, indexed from 0. */
Palette load(const std::string& path);

}  // namespace w2::pal
