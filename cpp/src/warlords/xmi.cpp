#include "warlords/xmi.hpp"

#include <algorithm>

namespace w2::xmi {

namespace {

uint32_t u32be(const std::string& s, size_t i) {
  return ((uint32_t)(uint8_t)s[i] << 24) | ((uint32_t)(uint8_t)s[i + 1] << 16) | ((uint32_t)(uint8_t)s[i + 2] << 8) |
         (uint8_t)s[i + 3];
}

// the EVNT chunk of the first sequence: {start, length}
bool walk(const std::string& s, size_t from, size_t to, size_t& at, size_t& len) {
  size_t p = from;
  while (p + 8 <= to) {
    std::string id = s.substr(p, 4);
    size_t n = u32be(s, p + 4);
    if (id == "FORM" || id == "CAT ") {
      if (walk(s, p + 12, std::min(to, p + 8 + n), at, len)) return true;
    } else if (id == "EVNT") {
      at = p + 8;
      len = n;
      return true;
    }
    p += 8 + n + n % 2;
  }
  return false;
}

struct Raw {
  int t, order;
  uint8_t st, a, b;
  size_t i;
};

}  // namespace

std::optional<Sequence> parse(const std::string& s) {
  size_t start, len;
  if (!walk(s, 0, s.size(), start, len)) return std::nullopt;
  size_t stop = std::min(s.size(), start + len);
  std::vector<Raw> raw;
  size_t p = start;
  int t = 0, order = 0, finish = 0;
  auto byte = [&](size_t i) -> uint8_t { return i < s.size() ? (uint8_t)s[i] : 0; };
  auto varlen = [&]() {
    int v = 0;
    uint8_t c;
    do {
      c = byte(p++);
      v = v * 128 + (c & 0x7f);
    } while (c >= 0x80 && p < stop);
    return v;
  };
  while (p < stop) {
    uint8_t c = byte(p);
    if (c < 0x80) {
      t += c;
      p++;
      continue;
    }
    p++;
    uint8_t hi = c & 0xf0;
    if (c == 0xff) {
      uint8_t kind = byte(p++);
      int n = varlen();
      p += n;
      if (kind == 0x2f) break;
    } else if (c == 0xf0 || c == 0xf7) {
      p += varlen();
    } else if (hi == 0x90) {
      uint8_t key = byte(p), vel = byte(p + 1);
      p += 2;
      int dur = varlen();
      raw.push_back({t, ++order, c, key, vel, 0});
      // the note's end sorts before anything else at its tick
      raw.push_back({t + dur, -1, (uint8_t)(0x80 | (c & 15)), key, 0, 0});
      finish = std::max(finish, t + dur);
    } else if (hi == 0xc0 || hi == 0xd0) {
      raw.push_back({t, ++order, c, byte(p), 0, 0});
      p++;
    } else {
      raw.push_back({t, ++order, c, byte(p), byte(p + 1), 0});
      p += 2;
    }
  }
  finish = std::max(finish, t);
  // a stable sort: by tick, note-offs first, then the file's own order
  for (size_t i = 0; i < raw.size(); i++) raw[i].i = i;
  std::sort(raw.begin(), raw.end(), [](const Raw& x, const Raw& y) {
    if (x.t != y.t) return x.t < y.t;
    if ((x.order < 0) != (y.order < 0)) return x.order < 0;
    return x.i < y.i;
  });
  Sequence seq;
  seq.length = finish;
  seq.events.reserve(raw.size());
  for (auto& e : raw) seq.events.push_back({e.t, e.st, e.a, e.b});
  return seq;
}

}  // namespace w2::xmi
