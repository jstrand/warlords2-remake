// Warlords II .PCK image decoder.
//
//   u16 version (1), u16 width, u16 height, then four compressed bitplanes.
//
// Each plane is LZ77 with a signed big-endian offset:
//   cmd < 0x80   literal run of (cmd + 1) bytes
//   cmd >= 0x80  match, 3 bytes: [cmd][mid][len]
//                distance = 65536 - ((cmd << 8) | mid)   -- 1..32768
//                length   = len + 1                      -- 1..256
// Copies are byte-at-a-time so they may overlap. Reads before the start of the
// plane yield 0. The window resets at every plane boundary.
//
// Plane p supplies bit p of the 4bpp colour index; within a byte the MSB is the
// leftmost pixel. See docs/formats/pck.md.
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace w2::pck {

/** A decoded sheet: w * h colour indices, 0-15. */
struct Pixels {
  int w = 0, h = 0;
  std::vector<uint8_t> px;
  uint8_t at(int x, int y) const { return px[(size_t)y * w + x]; }
};

/** Decode a .PCK; decoded sheets are kept, as they never change. */
std::shared_ptr<const Pixels> decode(const std::string& path);

}  // namespace w2::pck
