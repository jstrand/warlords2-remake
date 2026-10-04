// Reading the original's little-endian records. Offsets are 0-based, as in
// docs/formats.
#pragma once

#include <cstdint>
#include <string>

namespace w2 {

inline int u8(const std::string& b, size_t off) { return off < b.size() ? (uint8_t)b[off] : 0; }
inline int u16(const std::string& b, size_t off) { return u8(b, off) + u8(b, off + 1) * 256; }
inline int i16(const std::string& b, size_t off) {
  int v = u16(b, off);
  return v >= 0x8000 ? v - 0x10000 : v;
}

/** A NUL-terminated string in a fixed field, one character a byte. */
inline std::string cstr(const std::string& b, size_t off, size_t len) {
  std::string s;
  for (size_t i = off; i < off + len && i < b.size(); i++) {
    if (b[i] == 0) break;
    s += b[i];
  }
  return s;
}

}  // namespace w2
