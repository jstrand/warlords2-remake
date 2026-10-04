// Dice, the way WARLORD2.EXE rolls them.
//
// Every random rule in the original goes through one function,
// `dice(n, sides, bonus)` (see docs/re/dice_callers.md), so this is the
// single source of randomness for the engine too. The *sequence* is not
// bit-compatible with the original, but it is with the Lua remake: the same
// seed plays the same game in both.
#pragma once

#include <cstdint>
#include <vector>

namespace w2 {

class Rng {
 public:
  explicit Rng(double seed = 0) {
    // ((seed % M) + M) % M, on the double the Lua and JS hold
    long long s = (long long)seed;
    long long m = 0x100000000LL;
    state = (uint32_t)(((s % m) + m) % m);
  }

  // A small LCG with Numerical Recipes' constants, 32-bit.
  /** uniform integer in [0, n) */
  int below(int n) {
    state = state * 1664525u + 1013904223u;
    // take the high bits: the low ones of an LCG cycle far too visibly
    return (int)((state >> 16) % (uint32_t)n);
  }

  /** Sum of `n` rolls of a `sides`-sided die, plus `bonus`. */
  int dice(int n, int sides, int bonus = 0) {
    int total = bonus;
    if (sides <= 0) return total;
    for (int i = 0; i < n; i++) total += below(sides) + 1;
    return total;
  }

  /** True with probability percent/100, i.e. the game's dice(1,100,0) < p. */
  bool chance(int percent) { return dice(1, 100, 0) <= percent; }

  /** A random index into a list of `n`, or -1 for an empty one (no draw). */
  int pickIndex(size_t n) { return n == 0 ? -1 : below((int)n); }

  /** A random element of a list, or `none` if it is empty. */
  template <class T>
  T pick(const std::vector<T>& list, T none = T()) {
    int i = pickIndex(list.size());
    return i < 0 ? none : list[i];
  }

  /** Fisher-Yates, in place -- the Lua's order of draws exactly. */
  template <class T>
  void shuffle(std::vector<T>& list) {
    for (int i = (int)list.size(); i >= 2; i--) {
      int j = below(i) + 1;
      std::swap(list[i - 1], list[j - 1]);
    }
  }

  uint32_t state = 0;
};

}  // namespace w2
