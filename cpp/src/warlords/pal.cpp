#include "warlords/pal.hpp"

#include <cctype>
#include <stdexcept>

#include "util/util.hpp"

namespace w2::pal {

Palette load(const std::string& path) {
  auto text = readFile(path);
  if (!text) throw std::runtime_error("cannot open palette: " + path);
  // every run of three numbers separated by blanks, as the Lua's pattern
  std::vector<long> nums;
  Palette out;
  const std::string& s = *text;
  size_t i = 0;
  while (i < s.size()) {
    // find three integers separated by whitespace
    if (std::isdigit((unsigned char)s[i])) {
      size_t j = i;
      long v[3];
      bool ok = true;
      for (int k = 0; k < 3; k++) {
        if (j >= s.size() || !std::isdigit((unsigned char)s[j])) { ok = false; break; }
        long n = 0;
        while (j < s.size() && std::isdigit((unsigned char)s[j])) n = n * 10 + (s[j++] - '0');
        v[k] = n;
        if (k < 2) {
          size_t w = j;
          while (j < s.size() && std::isspace((unsigned char)s[j])) j++;
          if (j == w) { ok = false; break; }
        }
      }
      if (ok) {
        out.push_back({v[0] / 99.0, v[1] / 99.0, v[2] / 99.0});
        i = j;
        continue;
      }
      while (i < s.size() && std::isdigit((unsigned char)s[i])) i++;
      continue;
    }
    i++;
  }
  if (out.size() != 16) throw std::runtime_error(fmt("%s: expected 16 entries, got %d", path.c_str(), (int)out.size()));
  return out;
}

}  // namespace w2::pal
