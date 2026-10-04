// What Lua gave the remake for free and C++ does not: string.format, Lua
// 5.1's table.sort, files read whole, and a few string helpers.
#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace w2 {

/** string.format, as printf. */
std::string fmt(const char* f, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((format(printf, 1, 2)))
#endif
    ;

/**
 * Lua 5.1's table.sort, as LuaJIT runs it (lib_table.c's auxsort): not
 * stable, so arrays with equal keys come out in the order the Lua remake
 * leaves them, which keeps a seeded game the same in both. `lt(a, b)` is "a
 * sorts before b".
 */
template <class T, class Lt>
void luaSort(std::vector<T>& v, Lt lt) {
  // 1-based, as the original algorithm is written
  auto get = [&](long i) -> T& { return v[i - 1]; };
  auto swap = [&](long i, long j) { std::swap(v[i - 1], v[j - 1]); };
  std::function<void(long, long)> auxsort = [&](long l, long u) {
    while (l < u) {
      if (lt(get(u), get(l))) swap(l, u);
      if (u - l == 1) break;
      long i = (l + u) / 2;
      if (lt(get(i), get(l))) swap(i, l);
      else if (lt(get(u), get(i))) swap(i, u);
      if (u - l == 2) break;
      T P = get(i);
      swap(i, u - 1);
      i = l;
      long j = u - 1;
      for (;;) {
        while (lt(get(++i), P)) {
          if (i > u) throw std::runtime_error("invalid order function for sorting");
        }
        while (lt(P, get(--j))) {
          if (j < l) throw std::runtime_error("invalid order function for sorting");
        }
        if (j < i) break;
        swap(i, j);
      }
      swap(u - 1, i);
      if (i - l < u - i) {
        j = l; i = i - 1; l = i + 2;
      } else {
        j = i + 1; i = u; u = j - 2;
      }
      auxsort(j, i);
    }
  };
  auxsort(1, (long)v.size());
}

template <class T>
void luaSort(std::vector<T>& v) { luaSort(v, [](const T& a, const T& b) { return a < b; }); }

/** An argument to sfmt: a number or a string. */
struct FmtArg {
  bool str = false;
  long n = 0;
  std::string s;
  FmtArg(int v) : n(v) {}
  FmtArg(long v) : n(v) {}
  FmtArg(unsigned v) : n((long)v) {}
  FmtArg(unsigned long v) : n((long)v) {}
  FmtArg(long long v) : n((long)v) {}
  FmtArg(const std::string& v) : str(true), s(v) {}
  FmtArg(const char* v) : str(true), s(v ? v : "") {}
};
/** string.format for a format string from the game's own files: %d, %s, %x,
 *  %c and %%, with flags and widths, read safely whatever the arguments. */
std::string sfmt(const std::string& f, const std::vector<FmtArg>& args);
template <class... A>
std::string format(const std::string& f, A&&... a) {
  return sfmt(f, {FmtArg(std::forward<A>(a))...});
}

/** Lua's floor division and modulo, for numbers that may be negative. */
inline long floorDiv(long a, long b) { long q = a / b; if ((a % b != 0) && ((a < 0) != (b < 0))) q--; return q; }
inline long luaMod(long a, long b) { long m = a % b; if (m != 0 && ((m < 0) != (b < 0))) m += b; return m; }

// Files. A path is looked up ignoring case, component by component, as DOS
// did -- the data files' names are written every which way.

/** The real path a DOS-style path resolves to, or "" if there is none. */
std::string resolvePath(const std::string& path);
/** The whole file, or nothing. */
std::optional<std::string> readFile(const std::string& path);
/** The whole file, or an exception naming it. */
std::string mustRead(const std::string& path);
bool fileExists(const std::string& path);
bool writeFile(const std::string& path, const std::string& bytes);
bool removeFile(const std::string& path);

// Strings: bytes, one character a byte, as Lua strings are.
inline uint8_t byteAt(const std::string& s, size_t i) { return (uint8_t)s[i]; }
std::string upper(std::string s);
std::string lower(std::string s);
std::string trimRight(const std::string& s);
std::string trim(const std::string& s);
/** Split on runs of CR and LF, dropping empty lines. */
std::vector<std::string> splitLines(const std::string& s);
bool startsWith(const std::string& s, const std::string& p);
bool endsWith(const std::string& s, const std::string& p);
bool contains(const std::string& s, const std::string& p);
/** Latin-1 to UTF-8, and back (characters past Latin-1 become '?'). */
std::string latin1ToUtf8(const std::string& s);
std::string utf8ToLatin1(const std::string& s);
/** Lua's tonumber for a decimal integer, or nothing. */
std::optional<long> toInt(const std::string& s);

/** Wall time in seconds, for what plays out by the clock. */
double now();

}  // namespace w2
