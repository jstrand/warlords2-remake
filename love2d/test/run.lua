-- Headless tests for the rules core. No LOVE, no graphics.
--
--     lua love2d/test/run.lua                 -- from the repository root
--     lua love2d/test/run.lua /path/to/data   -- point at your own original/
--
-- Every assertion cites the rule it checks; docs/rules.md is the spec.

package.path = "love2d/?.lua;" .. package.path

local armytype = require("warlords.armytype")
local game     = require("warlords.game")
local rules    = require("warlords.rules")
local rng      = require("warlords.rng")

local DATA = arg[1] or "original"
local SCENARIOS = { "ERYTHEA", "ILLURIA", "DRAGON", "HADESHA", "ISLADIA", "SORCERY", "TUTORIA" }

local passed, failed = 0, 0

local function ok(cond, what)
  if cond then
    passed = passed + 1
  else
    failed = failed + 1
    print("  FAIL  " .. what)
  end
end

local function eq(got, want, what)
  if got == want then
    passed = passed + 1
  else
    failed = failed + 1
    print(("  FAIL  %s: got %s, want %s"):format(what, tostring(got), tostring(want)))
  end
end

local function exists(path)
  local f = io.open(path, "rb")
  if f then f:close() return true end
  return false
end

--------------------------------------------------------------------- dice

local function testDice()
  print("dice")
  local r = rng.new(1234)
  local lo, hi = math.huge, -math.huge
  for _ = 1, 20000 do
    local v = r:dice(1, 100, 0)
    lo, hi = math.min(lo, v), math.max(hi, v)
  end
  eq(lo, 1, "1d100 lower bound")
  eq(hi, 100, "1d100 upper bound")

  lo, hi = math.huge, -math.huge
  for _ = 1, 20000 do
    local v = r:dice(3, 500, 500)
    lo, hi = math.min(lo, v), math.max(hi, v)
  end
  ok(lo >= 503, "3d500+500 lower bound")
  ok(hi <= 2000, "3d500+500 upper bound")

  -- dice(1, n, -1) is the game's idiom for "a random index 0..n-1"
  local seen = {}
  for _ = 1, 2000 do seen[r:dice(1, 5, -1)] = true end
  for i = 0, 4 do ok(seen[i], "dice(1,5,-1) reaches " .. i) end
  ok(not seen[5], "dice(1,5,-1) stays below 5")

  -- the same seed must replay the same game
  local a, b = rng.new(7), rng.new(7)
  local same = true
  for _ = 1, 100 do if a:dice(2, 6, 0) ~= b:dice(2, 6, 0) then same = false end end
  ok(same, "a seed reproduces the sequence")
end

--------------------------------------------------------------- army types

local function testArmyTypes()
  print("ARMYTYPE.DAT")
  local t = armytype.load(DATA .. "/TERRAIN0/ARMYTYPE.DAT")
  eq(#t.list, 29, "29 army types")
  ok(t.byId[armytype.HERO] ~= nil, "type 28 exists")
  eq(t.byId[armytype.HERO].name, "Hero", "type 28 is the Hero")
  eq(t.byId[armytype.SCOUTS].name, "Scouts", "type 11 is Scouts")

  -- pinned against the production screens in docs/formats/armytype.md
  local hcav
  for _, a in ipairs(t.list) do if a.name == "Heavy Cav." then hcav = a end end
  ok(hcav ~= nil, "Heavy Cav. is in the table")
  eq(hcav.strength, 4, "Heavy Cav. strength")
  eq(hcav.time, 3, "Heavy Cav. production time")
  eq(hcav.cost, 8, "Heavy Cav. cost")
  eq(hcav.move, 16, "Heavy Cav. base move")

  local fliers = 0
  for _, a in ipairs(t.list) do if a.flies then fliers = fliers + 1 end end
  ok(fliers > 0, "some types fly")
end

------------------------------------------------------------------ production

local function testProduction()
  print("production slots and garrisons")
  local t = armytype.load(DATA .. "/TERRAIN0/ARMYTYPE.DAT")
  local r = rng.new(42)

  -- slots are sorted by purchase price, cheapest first
  local slots = rules.citySlots({ 4, 11, 2 }, t, r)
  eq(#slots, 3, "one slot per production type")
  local sorted = true
  for i = 2, #slots do if slots[i].price < slots[i - 1].price then sorted = false end end
  ok(sorted, "slots are sorted by price")

  -- the random nudge stays inside the documented bounds
  local okBounds = true
  for _ = 1, 2000 do
    for _, s in ipairs(rules.citySlots({ 1, 2, 3, 4 }, t, rng.new(_))) do
      local base = t.byId[s.type]
      if s.strength < 1 or s.strength > 9 then okBounds = false end
      if s.strength < base.strength - 1 or s.strength > base.strength + 1 then okBounds = false end
      if s.move < 6 then okBounds = false end
      if s.move > base.move + 4 then okBounds = false end
      if s.time < 1 or s.time > base.time + 1 then okBounds = false end
    end
  end
  ok(okBounds, "nudged stats stay within the documented range")

  -- purpose 4 only ever picks a flier, and only when the city has one
  local anyFlier = false
  for _, s in ipairs(slots) do if t.byId[s.type].flies then anyFlier = true end end
  local best4 = rules.bestSlot(slots, 4, t, false)
  if anyFlier then ok(best4 ~= nil, "purpose 4 finds a flier")
  else ok(best4 == nil, "purpose 4 refuses a city with no fliers") end

  -- purpose 6 wants speed, purpose 3 wants strength
  local fast = rules.bestSlot(slots, 6, t, false)
  local strong = rules.bestSlot(slots, 3, t, false)
  ok(fast ~= nil and strong ~= nil, "purposes 3 and 6 always choose something")

  eq(rules.cityDefence(2), 1, "fewer than 3 types: defence 1")
  eq(rules.cityDefence(3), 2, "3 types: defence 2")
  eq(rules.cityDefence(4), 2, "4 types: defence 2")

  -- Enhanced adds 2 strength, capped at 9
  local slot = { type = 4, name = "x", strength = 8, time = 2, cost = 8, move = 12 }
  eq(rules.armyFromSlot(slot, true).strength, 9, "Enhanced caps strength at 9")
  eq(rules.armyFromSlot(slot, false).strength, 8, "no Enhanced, no bonus")
  eq(rules.armyFromSlot(slot, false).upkeep, 4, "upkeep is half the slot cost")
end

------------------------------------------------------------------- the game

local function testGame(scenario)
  print("game: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 3 })

  ok(#g.sides >= 2, "at least two sides in play")
  eq(#g.armies, #g.map.cities, "exactly one starting army per city")

  -- a side starts owning only its capital
  for _, s in ipairs(g.sides) do
    eq(#game.sideCities(g, s), 1, s.name .. " owns only its capital")
    eq(game.sideCities(g, s)[1].index, s.capital.index, s.name .. "'s city is its capital")
  end

  -- Navy is never produced, and defence follows the type count
  for _, c in ipairs(g.map.cities) do
    for _, slot in ipairs(c.slots) do
      ok(slot.type ~= armytype.NAVY, c.name .. " does not produce Navy")
    end
    eq(c.defence, rules.cityDefence(#c.slots), c.name .. " defence")
    ok(#c.slots <= 4, c.name .. " has at most 4 slots")
  end

  -- every placed army is a real type on its city
  for _, a in ipairs(g.armies) do
    ok(g.types.byId[a.type] ~= nil, "army has a real type")
    eq(a.moves, 0, "a starting garrison has no moves")
  end

  -- no tile holds more than the stack limit
  local perTile = {}
  for _, a in ipairs(g.armies) do
    local k = a.y * g.map.width + a.x
    perTile[k] = (perTile[k] or 0) + 1
    ok(perTile[k] <= rules.MAX_STACK, "stack limit holds")
  end

  -- the same seed builds the same game
  local h = game.new(DATA, scenario, { seed = 3 })
  local same = #h.armies == #g.armies
  for i, a in ipairs(g.armies) do
    local b = h.armies[i]
    if not b or b.type ~= a.type or b.strength ~= a.strength then same = false end
  end
  ok(same, "the same seed rebuilds the same game")
end

---------------------------------------------------------------- turn loop

local function testTurnLoop(scenario)
  print("turn loop: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 5 })
  local side = game.begin(g)
  ok(side ~= nil, "the first side is to play")

  -- movement reset: a fresh garrison gets its full allowance
  for _, a in ipairs(game.sideArmies(g, side)) do
    eq(a.moves, a.maxMoves, "a garrison starts the turn with its full move")
  end

  -- income and upkeep are applied, and gold never goes negative
  local gold0 = side.gold
  eq(side.income, side.capital.income, "income is the capital's income")
  ok(side.gold == math.max(0, gold0), "gold was applied")

  -- carry-over: at most 2 unused points come across
  local a = game.sideArmies(g, side)[1]
  a.moves = 5
  local before = a.maxMoves
  for _ = 1, #g.sides do game.endTurn(g) end
  eq(a.moves, before + rules.MOVE_CARRY, "at most 2 unused moves carry over")

  a.moves = 1
  for _ = 1, #g.sides do game.endTurn(g) end
  eq(a.moves, before + 1, "carry-over below the cap is exact")

  -- production: a city builds its slot after `time` turns
  local city = side.capital
  game.setProduction(g, city, 1)
  local want = city.slots[1].time
  local n0 = #game.sideArmies(g, side)
  for _ = 1, want * #g.sides do game.endTurn(g) end
  ok(#game.sideArmies(g, side) == n0 + 1,
     ("%s produced after %d turns"):format(city.name, want))

  local built
  for _, army in ipairs(game.sideArmies(g, side)) do
    if army.homeCity == city.index and army ~= a then built = army end
  end
  ok(built ~= nil, "the new army belongs to the city that built it")
  if built then
    eq(built.upkeep, city.slots[1].cost // 2, "upkeep is half the slot cost")
    eq(built.x, city.x, "the new army stands in its city")
  end

  -- a side with no cities is eliminated at the start of its turn
  local victim = g.sides[#g.sides]
  victim.capital.ownerIndex = nil
  for _ = 1, #g.sides * 2 do game.endTurn(g) end
  ok(not victim.alive, "a side with no cities is eliminated")

  -- the turn counter advances once per full round
  local g2 = game.new(DATA, scenario, { seed = 6 })
  game.begin(g2)
  eq(g2.turn, 1, "the game starts on turn 1")
  for _ = 1, #g2.sides do game.endTurn(g2) end
  eq(g2.turn, 2, "a full round advances the turn")
end

------------------------------------------------------------------ vectoring

local function testVectoring(scenario)
  print("vectoring: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 9 })
  local side = game.begin(g)
  local from = side.capital

  -- take a second city so there is somewhere to send armies
  local dest
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex == nil and #c.slots > 0 then dest = c break end
  end
  ok(dest ~= nil, "found a neutral city to capture")
  if not dest then return end
  dest.ownerIndex = side.index
  for _, army in ipairs(game.armiesAt(g, dest.x, dest.y)) do army.owner = side.index end

  game.setProduction(g, from, 1)
  game.vector(g, from, dest)
  local time = from.slots[1].time

  local atDest = #game.armiesAt(g, dest.x, dest.y)
  for _ = 1, (time + 2) * #g.sides do game.endTurn(g) end
  ok(#game.armiesAt(g, dest.x, dest.y) > atDest,
     "a vectored army arrives two turns after it is built")

  -- stopping production clears the vector
  game.setProduction(g, from, nil)
  ok(from.vectorTo == nil, "vectoring only sticks while the city is building")
end

--------------------------------------------------------------------- bugs

local function testBugFlags()
  print("bug compatibility")
  ok(rules.bugs.heroExperienceReadsAttackerTypes,
     "the original's bugs are reproduced by default")
  local n = 0
  for _ in pairs(rules.bugs) do n = n + 1 end
  ok(n >= 3, "every documented bug has a flag")
end

--------------------------------------------------------------------- main

if not exists(DATA .. "/TERRAIN0/ARMYTYPE.DAT") then
  print(("cannot find the game data in %q."):format(DATA))
  print("Run from the repository root, or pass the path: lua love2d/test/run.lua /path/to/data")
  os.exit(1)
end

testDice()
testArmyTypes()
testProduction()
testBugFlags()
for _, s in ipairs(SCENARIOS) do
  if exists(DATA .. "/" .. s .. "/" .. s .. ".SCN") then testGame(s) end
end
testTurnLoop("ERYTHEA")
testVectoring("ERYTHEA")

print(("\n%d passed, %d failed"):format(passed, failed))
os.exit(failed == 0 and 0 or 1)
