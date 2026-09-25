-- Dice, the way WARLORD2.EXE rolls them.
--
-- Every random rule in the original goes through one function,
-- `dice(n, sides, bonus)` (see docs/re/dice_callers.md), so this module is the
-- single source of randomness for the engine too. The *sequence* is not
-- bit-compatible with the original -- that would mean reproducing Borland's
-- rand() and every call in order -- but the distributions are.

local rng = {}
rng.__index = rng

-- A small LCG, so a seed reproduces a game exactly whatever Lua version runs.
-- Numerical Recipes' constants, 32-bit.
local A, C, M = 1664525, 1013904223, 0x100000000

function rng.new(seed)
  return setmetatable({ state = (seed or 0) % M }, rng)
end

-- uniform integer in [0, n)
function rng:below(n)
  self.state = (A * self.state + C) % M
  -- take the high bits: the low ones of an LCG cycle far too visibly
  return math.floor(self.state / 65536) % n
end

--- Sum of `n` rolls of a `sides`-sided die, plus `bonus`.
-- dice(1, 100, 0) is 1..100; dice(1, n, -1) is 0..n-1; dice(3, 500, 500) is
-- the sage's gem.
function rng:dice(n, sides, bonus)
  local total = bonus or 0
  -- a die of no sides rolls nothing and gives the bonus (dice, as the
  -- character cards' zero dice use it)
  if (sides or 0) <= 0 then return total end
  for _ = 1, n do total = total + self:below(sides) + 1 end
  return total
end

--- True with probability `percent`/100, i.e. the game's `dice(1,100,0) < p`.
function rng:chance(percent)
  return self:dice(1, 100, 0) <= percent
end

--- A random element of a list (1-based), or nil if it is empty.
function rng:pick(list)
  if #list == 0 then return nil end
  return list[self:below(#list) + 1]
end

--- Fisher-Yates, in place.
function rng:shuffle(list)
  for i = #list, 2, -1 do
    local j = self:below(i) + 1
    list[i], list[j] = list[j], list[i]
  end
  return list
end

return rng
