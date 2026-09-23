-- The figures behind the Report menu's five reports: Army, City, Gold,
-- Production and Winning (6ef3:02fb). The drawing is ui/reports.lua; this is
-- only the arithmetic, so it can be checked without a window.

local game = require("warlords.game")

local report = {}

report.ARMY, report.CITY, report.GOLD, report.PRODUCTION, report.WINNING = 0, 1, 2, 3, 4

--- The Winning report's score for a side: its gold, five times its income,
--- its upkeep, and each of its cities' income times its defence, all over
--- 30, and never below 1 nor above 500.
function report.score(g, side)
  local cities = 0
  for _, c in ipairs(game.sideCities(g, side)) do
    cities = cities + (c.defence or 0) * (c.income or 0)
  end
  local s = math.floor((side.gold + game.income(g, side) * 5 + game.upkeep(g, side) + cities) / 30)
  return math.max(1, math.min(500, s))
end

--- One report's figures. Returns a table with
---   value[i]  the bar for side i (0-7)
---   out[i]    true for a side not in the game, which gets no bar
---   max       the top of the scale
---   result    what the summary line says: the side's own figure, its rank
---             (0 = first) for Winning, or how many armies it produced
function report.figures(g, side, n)
  local r = { value = {}, out = {}, max = 0 }
  for i = 0, 7 do
    local s = g.map.sides[i + 1]
    r.out[i] = not (s and s.alive)
    r.value[i] = 0
  end

  if n == report.PRODUCTION then
    r.result = #(side.produced or {})
    return r
  end

  if n == report.ARMY then
    for _, a in ipairs(g.armies) do
      if a.owner and a.owner >= 0 and a.owner < 8 then
        r.value[a.owner] = r.value[a.owner] + 1
      end
    end
  elseif n == report.CITY then
    for i = 0, 7 do
      local s = g.map.sides[i + 1]
      r.value[i] = s and #game.sideCities(g, s) or 0
    end
  elseif n == report.GOLD then
    for i = 0, 7 do
      local s = g.map.sides[i + 1]
      r.value[i] = s and s.gold or 0
    end
  elseif n == report.WINNING then
    local scores = {}
    for i = 0, 7 do
      local s = g.map.sides[i + 1]
      scores[i] = (s and s.inUse) and report.score(g, s) or 1
    end
    local rank = 0
    for i = 0, 7 do
      if scores[side.index] < scores[i] then rank = rank + 1 end
    end
    for i = 0, 7 do r.value[i] = math.floor(scores[i] / 5) end
    -- the scale is fixed: a score of 500 fills the bar
    r.max, r.result = 100, rank
    return r
  end

  for i = 0, 7 do r.max = math.max(r.max, r.value[i]) end
  -- The top of the scale is the largest figure made even, so that its half
  -- is a whole number: an odd one is raised by 1 (6ef3:02fb compares max / 2
  -- with (max - 1) / 2).
  if r.max > 0 and math.floor(r.max / 2) == math.floor((r.max - 1) / 2) then
    r.max = r.max + 1
  end
  r.result = r.value[side.index]
  return r
end

return report
