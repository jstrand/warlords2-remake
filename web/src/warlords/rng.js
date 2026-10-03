// Dice, the way WARLORD2.EXE rolls them.
//
// Every random rule in the original goes through one function,
// `dice(n, sides, bonus)` (see docs/re/dice_callers.md), so this module is the
// single source of randomness for the engine too. The *sequence* is not
// bit-compatible with the original, but it is with the Lua remake: the same
// seed plays the same game in both.

// A small LCG. Numerical Recipes' constants, 32-bit. The product is done in
// two halves so it stays exact in a double.
const A = 1664525, C = 1013904223, M = 0x100000000;

export class Rng {
  constructor(seed = 0) {
    this.state = ((seed % M) + M) % M;
  }

  /** uniform integer in [0, n) */
  below(n) {
    const s = this.state;
    const lo = s % 65536, hi = Math.floor(s / 65536);
    // A * s mod 2^32, without losing bits: A * hi * 65536 + A * lo
    const prod = ((A * hi) % 65536) * 65536 + A * lo + C;
    this.state = prod % M;
    // take the high bits: the low ones of an LCG cycle far too visibly
    return Math.floor(this.state / 65536) % n;
  }

  /** Sum of `n` rolls of a `sides`-sided die, plus `bonus`. */
  dice(n, sides, bonus = 0) {
    let total = bonus || 0;
    if ((sides || 0) <= 0) return total;
    for (let i = 0; i < n; i++) total += this.below(sides) + 1;
    return total;
  }

  /** True with probability percent/100, i.e. the game's dice(1,100,0) < p. */
  chance(percent) {
    return this.dice(1, 100, 0) <= percent;
  }

  /** A random element of a list, or undefined if it is empty. */
  pick(list) {
    if (list.length === 0) return undefined;
    return list[this.below(list.length)];
  }

  /** Fisher-Yates, in place -- the Lua's order of draws exactly. */
  shuffle(list) {
    for (let i = list.length; i >= 2; i--) {
      const j = this.below(i) + 1;
      const t = list[i - 1]; list[i - 1] = list[j - 1]; list[j - 1] = t;
    }
    return list;
  }
}

export function newRng(seed) { return new Rng(seed); }
