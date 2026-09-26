-- Headless tests for the rules core. No LOVE, no graphics.
--
--     lua love2d/test/run.lua                 -- from the repository root
--     luajit love2d/test/run.lua              -- what LOVE actually runs
--     lua love2d/test/run.lua /path/to/data   -- point at your own original/
--
-- **Run it under luajit too.** LOVE embeds LuaJIT, which is Lua 5.1: it has
-- neither the `//` integer division operator nor the `&` and `|` bitwise
-- operators. Both parse fine under the `lua` binary on most machines and then
-- fail to load in the game, so the engine avoids them -- `math.floor` for
-- division, and the arithmetic helpers in move.lua for the cost grid's flags.
--
-- Every assertion cites the rule it checks; docs/rules.md is the spec.

package.path = "love2d/?.lua;" .. package.path

local armytype = require("warlords.armytype")
local game     = require("warlords.game")
local movement = require("warlords.move")
local combat   = require("warlords.combat")
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

  -- The army cycle's per-turn marks. 8c07:0113 wipes "done for this turn"
  -- and "already offered this pass" for the side whose turn is starting, and
  -- leaves being fortified alone -- which is what makes digging in outlast
  -- the turn. docs/re/ui.md > The army cycle.
  do
    local one = game.sideArmies(g, side)[1]
    one.done, one.offered, one.fortified = true, true, true
    for _ = 1, #g.sides do game.endTurn(g) end
    eq(one.done, nil, "done for this turn wears off with the turn")
    eq(one.offered, nil, "and so does having been offered this pass")
    eq(one.fortified, true, "but digging in outlasts it")
    one.fortified = nil
  end

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
    eq(built.upkeep, math.floor(city.slots[1].cost / 2), "upkeep is half the slot cost")
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

  -- no destination takes more than four (7087:028b)
  local senders = {}
  for _, c in ipairs(g.map.cities) do
    if c ~= dest and #senders < 5 then
      c.ownerIndex = side.index
      senders[#senders + 1] = c
    end
  end
  for i = 1, 4 do ok(game.vector(g, senders[i], dest), "vector " .. i .. " is taken") end
  ok(not game.vector(g, senders[5], dest), "a fifth is refused")
  eq(senders[5].vectorTo, nil, "and the fifth city keeps no vector")
  eq(#game.vectoredTo(g, dest), 4, "four cities send to the destination")
  ok(game.vector(g, senders[1], dest), "a city already sending there may be set again")
  ok(game.vector(g, senders[1], senders[1]), "vectoring a city to itself lifts the vector")
  eq(senders[1].vectorTo, nil, "and leaves it with none")

  -- the map picks the side's nearest city to a click
  eq(game.nearestCity(g, dest.x, dest.y, side), dest, "a click on a city picks it")
  eq(game.nearestCity(g, dest.x + 1, dest.y - 1, side), dest, "and a click next to it too")
end

------------------------------------------------------------- buying types

local function testBuyProduction(scenario)
  print("buying production: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 17 })
  local side = game.begin(g)
  local city = side.capital
  side.gold = 5000

  -- the list is every type with a price, in the file's order
  local list = game.buyableTypes(g)
  ok(#list > 0 and #list <= 20, "the Build Production screen has room for every type")
  for _, a in ipairs(list) do ok(a.price >= 0, a.name .. " is for sale") end

  -- a type the city already builds cannot be bought again
  eq(game.cannotBuy(g, side, city, g.types.byId[city.slots[1].type]), "has it",
     "a type already built is greyed out")

  -- the screen starts on the first empty slot, or the first when all are full
  local n0 = #city.slots
  eq(game.buySlot(city), n0 < 4 and n0 + 1 or 1, "the starting slot")

  -- buy the dearest affordable type the city lacks into a new slot
  local pick
  for _, a in ipairs(list) do
    if not game.cannotBuy(g, side, city, a) and (not pick or a.price > pick.price) then pick = a end
  end
  ok(pick ~= nil, "something to buy")
  if n0 == 4 then table.remove(city.slots) n0 = 3 end
  game.setProduction(g, city, 1)
  local building = city.slots[1]
  local gold = side.gold
  game.buyProduction(g, side, city, n0 + 1, pick.id)
  eq(side.gold, gold - pick.price, "the price comes off the treasury")
  local slot = city.slots[n0 + 1]
  eq(slot.type, pick.id, "the bought type fills the slot")
  eq(slot.strength, pick.strength, "a bought type takes ARMYTYPE's strength unmodified")
  eq(slot.time, pick.time, "and its time")
  eq(slot.move, pick.move, "and its move")
  eq(city.defence, rules.cityDefence(#city.slots), "defence follows the number of types")
  eq(city.producing, 1, "buying into a new slot leaves production alone")

  -- sorting puts the list in price order and production follows its type
  game.sortProduction(city)
  for i = 2, #city.slots do
    ok(city.slots[i - 1].price <= city.slots[i].price, "sorted by price, cheapest first")
  end
  eq(city.slots[city.producing], building, "the city builds the same type after sorting")

  -- buying over the type being built stops production and the vector
  game.vector(g, city, g.map.cities[1] ~= city and g.map.cities[1] or g.map.cities[2])
  local other
  for _, a in ipairs(list) do if not game.cannotBuy(g, side, city, a) then other = a break end end
  game.buyProduction(g, side, city, city.producing, other.id)
  eq(city.producing, nil, "production stops when its type is bought over")
  eq(city.vectorTo, nil, "and the vector goes with it")

  -- a new name survives a save
  local saveMod = require("warlords.save")
  game.renameCity(g, city, "Newtown")
  local h = saveMod.decode(saveMod.encode(g), DATA)
  eq(h.map.cities[city.index + 1].name, "Newtown", "a renamed city keeps its name")
  eq(h.map.cities[city.index + 1].slots[#city.slots].type, city.slots[#city.slots].type,
     "bought production survives a save")
end

------------------------------------------------------------------ reports

local function testReports(scenario)
  print("reports: " .. scenario)
  local report = require("warlords.report")
  local g = game.new(DATA, scenario, { seed = 23 })
  local side = game.begin(g)

  local f = report.figures(g, side, report.CITY)
  eq(f.result, #game.sideCities(g, side), "the City report counts the side's cities")
  eq(f.max % 2, 0, "the top of the scale is even")
  local unused = 0
  for i = 0, 7 do if f.out[i] then unused = unused + 1 end end
  eq(unused, 8 - #g.sides, "only the sides in the game get a bar")

  local a = report.figures(g, side, report.ARMY)
  local mine = 0
  for _, army in ipairs(g.armies) do if army.owner == side.index then mine = mine + 1 end end
  eq(a.result, mine, "the Army report counts the side's armies")

  eq(report.figures(g, side, report.GOLD).result, side.gold, "the Gold report is the treasury")

  local w = report.figures(g, side, report.WINNING)
  eq(w.max, 100, "the Winning report's scale is fixed")
  ok(w.result >= 0 and w.result <= 7, "and says where the side comes")
  side.gold = 100000
  eq(report.figures(g, side, report.WINNING).result, 0, "a rich enough side comes first")
  eq(report.score(g, side), 500, "and its score stops at 500")

  -- the production log: build something, end enough turns, and it is there
  local city = side.capital
  game.setProduction(g, city, 1)
  city.countdown = 1
  local turns = 0
  repeat game.endTurn(g); turns = turns + 1 until g.side == side or turns > 20
  ok(side.produced and #side.produced >= 1, "the side's production is logged")
  if side.produced and side.produced[1] then
    eq(side.produced[1].city, city.index, "with the city that built it")
    eq(side.produced[1].kind, "built", "kept at home")
  end
  eq(report.figures(g, side, report.PRODUCTION).result, #side.produced,
     "the Production report counts what was built")
end

local function testDisband(scenario)
  print("disband: " .. scenario)
  local hero = require("warlords.hero")
  local quest = require("warlords.quest")
  local g = game.new(DATA, scenario, { seed = 29 })
  local side = game.begin(g)
  local h = hero.recruit(g, side, side.heroOffer)
  local stack = game.armiesAt(g, h.x, h.y)
  local combat = require("warlords.combat")
  h.strength = 5
  local fight = combat.stackStrengths(g, { h }, h.x, h.y)
  eq(fight[h], 5 + combat.HERO_TABLE[5] + 1, "View > Stack: a hero's strength, its table value and its standard")
  local n = #g.armies
  side.quest = { type = quest.OCCUPY, hero = h }
  local std = h.items[1]
  game.disband(g, side, stack)
  eq(#g.armies, n - #stack, "the stack is gone")
  eq(#game.armiesAt(g, h.x, h.y), 0, "from its tile")
  eq(std.status, 1, "the hero's standard lies on the ground")
  eq(side.quest, nil, "and the quest is lost with the hero")

  -- signposts: read from the .SGN, two lines each, and saved when edited
  local sg = game.signAt(g, 18, 12)
  ok(sg, "a sign at (18, 12)")
  eq(sg[1], "Traveller! Pray excuse the ", "its first line")
  eq(sg[2], "sulphur fumes. Keep On!", "and its second")
  sg[1] = "Hello there"
  local saveMod = require("warlords.save")
  local back = saveMod.decode(saveMod.encode(g), DATA)
  eq(game.signAt(back, 18, 12)[1], "Hello there", "an edited sign survives a save")

  g.map.fightOrder[side.index][3] = 26
  back = saveMod.decode(saveMod.encode(g), DATA)
  eq(back.map.fightOrder[side.index][3], 26, "so does an edited fight order")

  -- resigning burns every city and sends every army home
  local diplo = side.diploScore
  game.resign(g, side)
  eq(#game.sideCities(g, side), 0, "a side that resigns holds no city")
  eq(#game.sideArmies(g, side), 0, "and has no army")
  ok(side.capital.razed, "its capital is ruins")
  eq(side.diploScore, diplo, "burning its own on resigning costs nothing")
end

local function testHistory(scenario)
  print("history: " .. scenario)
  local history = require("warlords.history")
  local g = game.new(DATA, scenario, { seed = 41 })
  local side = game.begin(g)
  history.deed(g, side, history.FINDS, history.SAGE, 0, "Hero")
  history.deed(g, side, history.WON, side.index, 3, side.name)
  history.deed(g, side, history.EMERGES, 1, 0, "Hero")
  eq(#g.deeds[side.index], 2, "two deeds a side")
  local types = { g.deeds[side.index][1].type, g.deeds[side.index][2].type }
  table.sort(types)
  eq(types[1], history.EMERGES, "a lower type pushes out the higher")
  eq(types[2], history.WON, "and the lower of the two stays")
  history.deed(g, side, history.PEACE, 0, 1, "")
  eq(#g.deeds[side.index], 2, "a higher type does not get in")

  local guard = 0
  while g.turn < 3 and guard < 50 do game.endTurn(g) guard = guard + 1 end
  ok(g.history and #g.history >= 1, "a record for the round")
  local r = g.history[1]
  eq(#r.owners, #g.map.cities, "every city's owner")
  ok(#r.events >= 2, "with the deeds")
  eq(g.deeds[side.index], nil, "and the slots cleared")
  local saveMod = require("warlords.save")
  local back = saveMod.decode(saveMod.encode(g), DATA)
  eq(#back.history, #g.history, "history survives a save")

  -- triumphs: a side's own row is what it lost, its row for another what it killed
  local foe = g.sides[2]
  history.tally(g, { owner = foe.index, type = 28, items = { { index = foe.index } } }, side.index)
  eq(history.triumph(g, side.index, foe.index, history.HEROES), 1, "a hero killed")
  eq(history.triumph(g, side.index, foe.index, history.STANDARDS), 1, "and the standard it carried")
  eq(history.triumph(g, foe.index, foe.index, history.HEROES), 1, "the loser counts its own loss")
end

local function testSetup()
  print("new-game setup")
  local g = game.new(DATA, "ERYTHEA", { seed = 5, options = { quests = 0 },
    sides = { [0] = { computer = false }, [1] = { computer = true, level = 2 }, [2] = { off = true } } })
  eq(g.map.options.quests, 0, "the options chosen")
  ok(not g.map.sides[1].computer, "side 0 human")
  ok(g.map.sides[2].computer and g.map.sides[2].level == 2, "side 1 a Warlord")
  ok(not g.map.sides[3].inUse, "side 2 left out")
  local cap = g.map.sides[3].capital
  eq(cap and cap.ownerIndex, nil, "and its capital neutral")
  for _, s in ipairs(g.sides) do ok(s.index ~= 2, "not among the sides in play") end
end

local function testSage(scenario)
  print("sage: " .. scenario)
  local site = require("warlords.site")
  local g = game.new(DATA, scenario, { seed = 31 })
  local side = game.begin(g)
  local hx, hy = side.capital.x, side.capital.y
  for _, s in ipairs(g.map.sites) do s.rich, s.revealed, s.searched = false, 0xff, false end
  eq(#site.sageList(g, side, hx, hy), 0, "no rich site unshown: nothing to tell")

  -- two gold sites and an item site near, one gold site far
  local near = {}
  for _, s in ipairs(g.map.sites) do
    local d = math.sqrt((s.x - hx) ^ 2 + (s.y - hy) ^ 2)
    if d < site.SAGE_RANGE then near[#near + 1] = s end
  end
  table.sort(near, function(a, b)
    return (a.x - hx) ^ 2 + (a.y - hy) ^ 2 < (b.x - hx) ^ 2 + (b.y - hy) ^ 2
  end)
  ok(#near >= 3, "three sites within reach")
  local g1, g2, isite = near[3], near[1], near[2]
  for _, s in ipairs({ g1, g2, isite }) do s.rich, s.revealed = true, 0 end
  g1.content, g2.content = site.GOLD, site.GOLD
  isite.content, isite.item = site.ITEM, g.map.items[10].index
  local list = site.sageList(g, side, hx, hy)
  eq(#list, 2, "gold once however many, and the item")
  local gold
  for _, e in ipairs(list) do if e.kind == "gold" then gold = e end end
  ok(gold, "a gold entry")
  eq(site.sageShow(g, side, gold, hx, hy), g2, "the nearest gold site is shown")
  ok(math.floor(g2.revealed / 2 ^ side.index) % 2 == 1, "and marked shown to the side")
  eq(site.sageShow(g, side, gold, hx, hy), g1, "then the next one")
  local items = site.sageList(g, side, hx, hy)
  eq(#items, 1, "only the item is left")
  eq(items[1].item, g.map.items[10], "by its record")

  local before = side.gold
  local n = site.sageGem(g, side)
  ok(n >= 503 and n <= 2000, "a gem is 3d500 + 500")
  eq(side.gold, before + n, "and is paid")

  g.map.options.hiddenMap = 1
  g.explored = nil
  local x0, y0, w, h = site.sageMap(g, side, 3, 3)
  eq(x0, 0, "the patch is kept on the map")
  ok(w >= 16 and w <= 25 and h >= 16 and h <= 25, "16-25 tiles a side")
  ok(game.seen(g, side, x0 + w - 1, y0 + h - 1), "and uncovered")
end

local function testHeroItems(scenario)
  print("hero items: " .. scenario)
  local hero = require("warlords.hero")
  local g = game.new(DATA, scenario, { seed = 29 })
  local side = game.begin(g)
  local h = hero.recruit(g, side, side.heroOffer)
  eq(h.items[1], g.map.items[side.index + 1], "the first hero carries the side's own standard record")
  eq(h.items[1].status, 3, "which is carried")
  eq(#hero.itemsHere(g, h), 1, "nothing else is listed yet")

  local it = g.map.items[9]
  it.status, it.x, it.y = 1, h.x, h.y
  local list = hero.itemsHere(g, h)
  eq(#list, 2, "an item on the hero's tile is listed")
  eq(list[2], it, "after what the hero carries")

  hero.takeItem(g, h, it)
  eq(it.status, 3, "taking an item carries it")
  eq(#h.items, 2, "the hero has it")
  hero.dropItem(g, h, it)
  eq(it.status, 1, "dropping puts it on the ground")
  eq(it.x, h.x, "where the hero stands")
  eq(#h.items, 1, "and the hero no longer has it")

  -- Plant Flag: not in the city, but a step out of it on dry land
  ok(not game.plantFlag(g, side, h), "no flag in a city")
  local move = require("warlords.move")
  local scn = require("warlords.scn")
  local fx, fy
  for dy = -3, 4 do for dx = -3, 4 do
    local x, y = h.x + dx, h.y + dy
    local t = scn.terrainAt(g.map, x, y)
    if not fx and (t == move.PLAIN or t == move.ROAD) and not game.cityAt(g, x, y) then fx, fy = x, y end
  end end
  ok(fx ~= nil, "found open ground near the capital")
  h.x, h.y = fx, fy
  ok(game.plantFlag(g, side, h), "the flag is planted")
  local sx, sy = game.standardAt(g, side)
  eq(sx, fx, "where the hero stands")
  ok(not game.plantFlag(g, side, h), "and only once")

  -- a city vectoring there sends its armies to the flag
  local city = side.capital
  game.setProduction(g, city, 1)
  city.countdown = 1
  ok(game.vectorToStandard(g, city, side), "a city may vector to the standard")
  local before = #game.armiesAt(g, fx, fy)
  for _ = 1, 3 * #g.sides + 1 do game.endTurn(g) end
  ok(#game.armiesAt(g, fx, fy) > before, "a vectored army arrives at the standard")

  -- picking the flag up again takes it off the map
  hero.takeItem(g, h, g.map.items[side.index + 1])
  eq(game.standardAt(g, side), nil, "a standard picked up is no longer planted")
end

------------------------------------------------------------------- movement

local function testMovement(scenario)
  print("movement: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 11 })
  local side = game.begin(g)
  local army = game.sideArmies(g, side)[1]
  local stack = { army }

  -- terrain costs come straight from the table, with roads at 1
  eq(movement.COST[movement.MOUNTAINS], 0, "mountains are impassable")
  eq(movement.COST[movement.ROAD], 1, "a road costs 1")
  eq(movement.COST[movement.HILLS], 6, "hills cost 6")

  -- the cost grid: a tile with a road costs 1 whatever its terrain
  local grid = movement.grid(g, side.index)
  local roaded, roadCostOk = 0, true
  for y = 0, g.map.height - 1 do
    for x = 0, g.map.width - 1 do
      if require("warlords.scn").roadAt(g.map, x, y) % 0x20 ~= 0 then
        roaded = roaded + 1
        if grid[y * g.map.width + x] % 8 ~= 1 then roadCostOk = false end
      end
    end
  end
  ok(roaded > 0, "the scenario has roads")
  ok(roadCostOk, "every road tile costs 1")

  -- a land stack cannot step into open water or onto a mountain
  local W = movement.WATER_F
  local land, water = 2, 2 | W       -- cost-2 land, cost-2 open water
  eq(movement.stepCost(land, water, movement.LAND, false, false, 10), nil,
     "a land stack cannot enter open water")
  eq(movement.stepCost(land, 0, movement.LAND, false, false, 10), nil,
     "nothing enters a cost-0 tile on land")
  eq(movement.stepCost(land, 6 | movement.HILLS_F, movement.LAND, false, false, 10), 6,
     "hills cost 6 without the bonus")
  eq(movement.stepCost(land, 6 | movement.HILLS_F, movement.LAND, false, true, 10), 2,
     "the hills bonus brings them to 2")
  eq(movement.stepCost(land, 4 | movement.FOREST_F, movement.LAND, true, false, 10), 2,
     "the woods bonus brings forest to 2")

  -- crossing the shoreline: only at a crossing tile, and it costs extra
  local cross = 1 | W | movement.CROSS_F
  eq(movement.stepCost(land, cross, movement.LAND, false, false, 10), 1,
     "a crossing tile is free of the water charge")
  eq(movement.stepCost(cross, water, movement.LAND, false, false, 10), 2 + 10,
     "stepping from a crossing into water costs the penalty")
  eq(movement.stepCost(cross, water, movement.LAND, false, false, 20), 2 + 20,
     "a move aimed at water pays 20")

  -- flying pays 2 everywhere, 1 where the tile costs 1
  eq(movement.stepCost(land, 0, movement.FLYING, false, false, 10), 2,
     "a flier crosses mountains at 2")
  eq(movement.stepCost(land, water, movement.FLYING, false, false, 10), 2,
     "a flier crosses water at 2")
  eq(movement.stepCost(land, 1, movement.FLYING, false, false, 10), 1,
     "a flier pays 1 on a road")
  eq(movement.stepCost(land, 6 | movement.HILLS_F, movement.FLYING, false, false, 10), 2,
     "a flier pays 2 over hills")

  -- a boat keeps to water and cities
  eq(movement.stepCost(water, land, movement.BOAT, false, false, 10), nil,
     "a boat cannot go ashore")
  eq(movement.stepCost(water, water, movement.BOAT, false, false, 10), 2,
     "a boat moves on water")

  -- the stack shares the lowest movement allowance
  army.moves = 7
  eq(movement.stackMoves({ army, { moves = 3 }, { moves = 9 } }), 3,
     "a stack moves at the pace of its slowest army")

  -- a real path out of the capital, and the cost charged to the army
  army.moves = army.maxMoves
  local from = { x = army.x, y = army.y }
  local target
  for dx = -6, 6 do
    for dy = -6, 6 do
      local x, y = from.x + dx, from.y + dy
      if not target and x >= 0 and y >= 0 and x < g.map.width and y < g.map.height
         and not game.cityAt(g, x, y) then
        local p = movement.findPath(g, stack, from.x, from.y, x, y)
        if p and #p >= 2 then target = { x = x, y = y, path = p } end
      end
    end
  end
  ok(target ~= nil, "found somewhere to walk to")
  if target then
    local before = army.moves
    local r = movement.moveTo(g, stack, target.x, target.y)
    ok(r.steps > 0, "the stack moved")
    eq(army.moves, math.max(0, before - r.spent), "the cost was charged to the army")
    ok(army.x ~= from.x or army.y ~= from.y, "the army is somewhere else")
  end

  -- a stack with no movement left goes nowhere
  army.moves = 0
  local r = movement.moveTo(g, stack, from.x, from.y + 1)
  eq(r.steps, 0, "an army with no moves stays put")

  -- walking into a city the side does not own is an attack, not a move
  local enemy
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex ~= side.index then enemy = c break end
  end
  ok(enemy ~= nil, "there is a city to attack")
  if enemy then
    army.moves = 99
    army.x, army.y = enemy.x, enemy.y - 1
    local path = movement.findPath(g, stack, army.x, army.y, enemy.x, enemy.y)
    if path and #path > 0 then
      local res = movement.walk(g, stack, path)
      eq(res.stopped, "attack", "stepping into an enemy city starts an attack")
      ok(res.attack and res.attack.city == enemy, "the attack names the city")
      eq(army.x, enemy.x, "the attacker has not entered the city")
      ok(army.y == enemy.y - 1, "the attacker stayed where it was")
    end
  end

  -- a path may not run *through* a city the side does not own
  if enemy then
    local grid2 = movement.grid(g, side.index)
    eq(grid2[enemy.y * g.map.width + enemy.x] % 8, 0,
       "an enemy city is impassable in the cost grid")
  end

  -- What the map draws while a stack is under orders: the route, and how far
  -- along it this turn's movement reaches. docs/re/ui.md > Walking.
  do
    local a = { x = army.x, y = army.y, owner = side.index, type = 11,
                name = "Scouts", strength = 9, maxMoves = 20, moves = 20, upkeep = 1 }
    g.armies[#g.armies + 1] = a
    -- somewhere far enough that one turn cannot finish it
    local far, route
    for d = 12, 30, 2 do
      far = { x = math.min(g.map.width - 2, a.x + d), y = a.y }
      route = movement.preview(g, { a }, far.x, far.y)
      if route and route.reach < #route.path then break end
    end
    if route then
      ok(#route.path > 0, "a stack under orders has a route to draw")
      ok(route.reach <= #route.path, "it can walk no more of it than there is")
      eq(route.path[#route.path].x, far.x, "the route ends where it was sent")
      eq(route.path[#route.path].y, far.y, "on that row")
      -- every tile of the route is a step from the one before it
      local px, py = a.x, a.y
      local straight = true
      for _, step in ipairs(route.path) do
        if math.max(math.abs(step.x - px), math.abs(step.y - py)) ~= 1 then straight = false end
        px, py = step.x, step.y
      end
      ok(straight, "and it is a chain of single steps")

      -- the walk reports the tiles it actually covered, which is what the
      -- map plays back as the stack moves
      local r = movement.moveTo(g, { a }, far.x, far.y)
      ok(r.walked ~= nil, "a walk reports the tiles it covered")
      eq(#r.walked, r.steps, "one per step taken")
      if r.steps > 0 then
        eq(r.walked[#r.walked].x, a.x, "ending where the stack now stands")
        eq(r.walked[#r.walked].y, a.y, "on that row")
      end
      -- and with the movement spent, nothing more of the route is in reach
      local left = movement.preview(g, { a }, far.x, far.y)
      if left then
        ok(left.reach < #left.path, "what is left of it is out of reach this turn")
      end
    end
    for i = #g.armies, 1, -1 do
      if g.armies[i] == a then table.remove(g.armies, i) end
    end
  end
end

local function testStackLimit(scenario)
  print("stack limit: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 13 })
  local side = game.begin(g)
  local army = game.sideArmies(g, side)[1]

  -- fill the tile north of the army with eight of the side's armies
  local tx, ty = army.x, army.y - 1
  if ty < 0 then tx, ty = army.x, army.y + 1 end
  for _ = 1, rules.MAX_STACK do
    g.armies[#g.armies + 1] = {
      x = tx, y = ty, owner = side.index, type = army.type, name = army.name,
      strength = 1, maxMoves = 10, moves = 10, upkeep = 0, homeCity = army.homeCity,
    }
  end
  eq(#game.armiesAt(g, tx, ty), rules.MAX_STACK, "the tile is full")

  army.moves = 99
  local r = movement.moveTo(g, { army }, tx, ty)
  eq(r.steps, 0, "a stack cannot stop on a full tile")
  ok(army.x ~= tx or army.y ~= ty, "the army stayed off the full tile")
end

--------------------------------------------------------------------- combat

local function testCombat(scenario)
  print("combat: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 17 })
  local side = game.begin(g)

  -- the hero command table
  eq(combat.HERO_TABLE[0], 0, "a strength-0 hero commands nothing")
  eq(combat.HERO_TABLE[4], 1, "strength 4 gives +1")
  eq(combat.HERO_TABLE[9], 3, "strength 9 gives +3")

  -- terrain classes, in the order the ARMYTYPE bonus fields are stored
  eq(combat.CITY, 0, "city is class 0")
  eq(combat.OPEN, 1, "open is class 1")
  eq(combat.WOODS, 2, "woods is class 2")
  eq(combat.HILLS, 3, "hills is class 3")

  local scnmod = require("warlords.scn")
  local seen = {}
  for y = 0, g.map.height - 1, 7 do
    for x = 0, g.map.width - 1, 7 do
      local class = combat.terrainClass(g, x, y)
      local t = scnmod.terrainAt(g.map, x, y)
      seen[class] = true
      if t == movement.FOREST then eq(class, combat.WOODS, "forest fights as woods") end
      if t == movement.HILLS or t == movement.MOUNTAINS then
        eq(class, combat.HILLS, "hills and mountains fight as hills")
      end
      if t == movement.CITY or t == movement.SITE then
        eq(class, combat.CITY, "cities and sites fight as city")
      end
      if t == movement.PLAIN or t == movement.MARSH or t == movement.WATER then
        eq(class, combat.OPEN, "plain, marsh and water fight as open")
      end
    end
  end
  ok(seen[combat.OPEN], "the map has open ground")

  -- the combat cap comes from the scenario and is 5 in every shipped one
  eq(g.map.combatCap, 5, "the combat cap is 5")

  -- fight order: every army type has a rank, and the ranks are a permutation
  local row, used = g.map.fightOrder[0], {}
  for t = 0, 28 do
    ok(row[t] ~= nil, "type " .. t .. " has a fight order")
    used[row[t]] = true
  end
  local distinct = 0
  for _ in pairs(used) do distinct = distinct + 1 end
  eq(distinct, 29, "fight order is a permutation of 29 ranks")

  -- a neutral city's fortify bonus is halved
  local neutral
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex == nil and c.defence == 2 then neutral = c break end
  end
  if neutral then
    local cls = combat.terrainClass(g, neutral.x, neutral.y)
    eq(combat.fortify(g, {}, neutral.x, neutral.y, cls), 1,
       "a neutral city's defence 2 fortifies for 1")
    game.setCityOwner(g, neutral, g.sides[#g.sides].index)
    eq(combat.fortify(g, {}, neutral.x, neutral.y, cls), 2,
       "an owned city's defence 2 fortifies for 2")
    game.setCityOwner(g, neutral, nil)
  end

  -- Siege cancels the city bonus entirely
  local siege
  for _, a in ipairs(g.types.list) do
    if a.bonus[52] == combat.SIEGE then siege = a end
  end
  if siege and neutral then
    local cls = combat.terrainClass(g, neutral.x, neutral.y)
    eq(combat.fortify(g, { { type = siege.id } }, neutral.x, neutral.y, cls), 0,
       "a Siege attacker cancels the city bonus")
  end

  -- a boat at sea fights at exactly 4, whatever its strength
  local boatArmy = { type = 11, strength = 9, atSea = true }
  eq(combat.strength(g, boatArmy, 5, combat.OPEN, movement.WATER), 4,
     "an army at sea on water fights at 4")
  eq(combat.strength(g, boatArmy, 5, combat.OPEN, movement.SHORE), 4,
     "an army at sea on shore fights at 4")
  boatArmy.atSea = false
  ok(combat.strength(g, boatArmy, 5, combat.OPEN, movement.PLAIN) > 4,
     "ashore it fights normally")

  -- strength is capped at 15
  eq(combat.strength(g, { type = 11, strength = 9 }, 99, combat.OPEN, movement.PLAIN), 15,
     "strength is capped at 15")

  -- the lines: defenders are everyone on the tile who is not the attacker
  local a1 = game.sideArmies(g, side)[1]
  local enemy
  for _, c in ipairs(g.map.cities) do if c.ownerIndex ~= side.index then enemy = c break end end
  local att, def, defOwner, city = combat.lines(g, { a1 }, enemy.x, enemy.y)
  eq(#att, 1, "the attacking line is the stack")
  ok(#def >= 1, "the city has defenders")
  eq(city, enemy, "the battle names the city")
  for _, d in ipairs(def) do ok(d.owner ~= side.index, "no defender belongs to the attacker") end

  -- lines come out sorted by fight order, lowest first
  local sorted = true
  for i = 2, #def do
    local ownerRow = g.map.fightOrder[defOwner or 8]
    if ownerRow[def[i].type] < ownerRow[def[i - 1].type] then sorted = false end
  end
  ok(sorted, "the defending line is sorted by fight order")

  -- a battle always ends, and its log records one death per entry
  local r = combat.resolve(g, att, def, enemy.x, enemy.y)
  ok(#r.log >= 1, "the battle logged at least one death")
  eq(#r.log, #r.deadAttackers + #r.deadDefenders, "one log entry per dead army")
  ok(r.won == (#r.defenders == 0), "the attacker wins only if every defender is dead")

  -- overwhelming force wins nearly always; hopeless odds nearly never
  local function odds(atkStrength, nAtk, defStrength, nDef)
    local A, D = {}, {}
    for _ = 1, nAtk do A[#A + 1] = { type = 11, strength = atkStrength, owner = side.index } end
    for _ = 1, nDef do D[#D + 1] = { type = 11, strength = defStrength, owner = nil } end
    local wins = 0
    for _ = 1, 200 do
      if combat.resolve(g, A, D, 0, 0).won then wins = wins + 1 end
    end
    return wins
  end
  ok(odds(9, 8, 1, 1) > 190, "eight strong armies beat one weak one")
  ok(odds(1, 1, 9, 8) < 10, "one weak army loses to eight strong ones")
  local many = odds(3, 8, 9, 1)
  ok(many > 150, "many weak armies beat one strong one: " .. many)

  -- the Military Advisor runs 19 battles and reports one of ten verdicts
  eq(combat.ADVICE_BATTLES, 19, "the advisor fights 19 battles")
  local verdict, wins = combat.advise(g, att, def, enemy.x, enemy.y)
  ok(verdict ~= nil, "the advisor has a verdict")
  ok(wins >= 0 and wins <= 19, "the advisor's win count is in range")
  eq(verdict, combat.ADVICE[math.floor(wins / 2)], "the verdict is wins/2 into the table")
end

local function testCapture(scenario)
  print("capture: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 23 })
  local side = game.begin(g)

  -- give the attacker an overwhelming stack next to a neutral city
  local target
  for _, c in ipairs(g.map.cities) do if c.ownerIndex == nil then target = c break end end
  ok(target ~= nil, "found a neutral city")
  local stack = {}
  for _ = 1, 8 do
    local a = { x = target.x, y = target.y - 1, owner = side.index, type = 11,
                name = "Scouts", strength = 9, maxMoves = 20, moves = 20, upkeep = 1 }
    g.armies[#g.armies + 1] = a
    stack[#stack + 1] = a
  end

  -- A city is assaulted from beside it, never walked into: the walk stops on
  -- the last tile before it and asks for an attack. Any of the eight tiles
  -- will do -- the path steps diagonally like any other move.
  do
    local far = { owner = side.index, type = 11, name = "Scouts", strength = 9,
                  maxMoves = 20, moves = 20, upkeep = 1 }
    g.armies[#g.armies + 1] = far
    -- start a few tiles off, wherever the ground allows it
    local walk
    for _, off in ipairs({ { -3, -3 }, { 3, -3 }, { -3, 3 }, { 3, 3 },
                           { 0, -3 }, { 0, 3 }, { -3, 0 }, { 3, 0 } }) do
      far.x, far.y, far.moves = target.x + off[1], target.y + off[2], 20
      walk = movement.moveTo(g, { far }, target.x, target.y)
      if walk.stopped == "attack" then break end
    end
    eq(walk.stopped, "attack", "walking at a city ends in an assault")
    eq(walk.attack.x, target.x, "on the city's own tile")
    eq(walk.attack.y, target.y, "and row")
    eq(walk.attack.city, target, "which is the city")
    ok(math.max(math.abs(far.x - target.x), math.abs(far.y - target.y)) == 1,
       "and the stack is left standing beside it, not in it")
    ok(far.x ~= target.x or far.y ~= target.y, "it did not walk in")
    eq(target.ownerIndex, nil, "and the city is still the defender's")
    for i = #g.armies, 1, -1 do
      if g.armies[i] == far then table.remove(g.armies, i) end
    end
  end

  local before = #game.sideCities(g, side)
  local r = game.resolveAttack(g, stack, target.x, target.y)
  ok(r.won, "the overwhelming stack took the city")
  -- the battle window is drawn from the lines as they were drawn up, and
  -- replays result.log over them
  ok(r.lines ~= nil, "the attack reports the lines that fought")
  eq(#r.lines.attackers, #stack, "every attacker is in the line")
  eq(r.lines.city, target, "and the line knows it was a city")
  do
    local down = 0
    for _, side_ in ipairs(r.log or {}) do
      ok(side_ == 0 or side_ == 1, "the log says which side lost an army")
      down = down + 1
    end
    eq(down, #r.deadAttackers + #r.deadDefenders,
       "the log has one entry per army that fell")
  end
  eq(target.ownerIndex, side.index, "the city changed hands")
  eq(#game.sideCities(g, side), before + 1, "the side owns one more city")
  eq(target.producing, nil, "a captured city is not building anything")
  for _, a in ipairs(r.attackers) do
    eq(a.x, target.x, "the survivors moved in")
  end
  -- the dead are gone from the game
  for _, dead in ipairs(r.deadDefenders) do
    local stillThere = false
    for _, a in ipairs(g.armies) do if a == dead then stillThere = true end end
    ok(not stillThere, "a dead defender is removed")
  end

  -- loot: taking a city from a side pays, taking a neutral one does not
  local victim = g.sides[#g.sides]
  victim.gold = 400
  eq(game.loot(g, victim), 200, "one city: half its gold")
  local extra = nil
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex == nil and c ~= target then extra = c break end
  end
  if extra then
    game.setCityOwner(g, extra, victim.index)
    eq(game.loot(g, victim), math.floor(math.floor(400 / 2) / 2), "two cities: half the per-city share")
  end
end

local function testTutorialHero()
  print("tutorial: a hero cannot die attacking neutrals")
  local g = game.new(DATA, "TUTORIA", { seed = 31 })
  local side = game.begin(g)
  eq(g.map.options.tutorial, 1, "TUTORIA sets the tutorial flag")
  ok(not side.computer, "the tutorial player is human")

  local hero = { type = armytype.HERO, strength = 1, owner = side.index }
  local defenders = {}
  for _ = 1, 8 do defenders[#defenders + 1] = { type = 11, strength = 9, owner = nil } end
  local deaths = 0
  for _ = 1, 50 do
    local r = combat.resolve(g, { hero }, defenders, 0, 0)
    deaths = deaths + #r.deadAttackers
  end
  eq(deaths, 0, "the tutorial hero never dies against neutrals")

  -- with the flag off the same hero dies readily
  g.map.options.tutorial = 0
  deaths = 0
  for _ = 1, 50 do
    deaths = deaths + #combat.resolve(g, { hero }, defenders, 0, 0).deadAttackers
  end
  ok(deaths > 0, "without the tutorial flag the hero can die")
end

------------------------------------------------- what to do with a captured city

local function testCityChoices(scenario)
  print("pillage, sack and raze: " .. scenario)
  local g = game.new(DATA, scenario, { seed = 29 })
  local side = game.begin(g)

  local function captured(want)
    local city
    for _, c in ipairs(g.map.cities) do
      if c.ownerIndex == nil and #c.slots >= want then city = c break end
    end
    if city then game.setCityOwner(g, city, side.index) end
    return city
  end

  -- pillage takes the most expensive type and pays half its purchase price
  local city = captured(2)
  ok(city ~= nil, "found a city with something to pillage")
  if city then
    local n, dear = #city.slots, city.slots[#city.slots]
    local want = math.floor(math.abs(g.types.byId[dear.type].price) / 2)
    local gold0, score0 = side.gold, side.diploScore or 0
    local got = game.pillage(g, side, city)
    eq(got, want, "pillage pays half the type's purchase price")
    eq(side.gold, gold0 + want, "the gold was paid")
    eq(#city.slots, n - 1, "pillage removed one type")
    eq(city.defence, rules.cityDefence(#city.slots), "defence was recomputed")
    local d = (side.diploScore or 0) - score0
    ok(d >= 1 and d <= 5, "pillage costs 1d5 diplomatic score: " .. d)
    for _, s2 in ipairs(city.slots) do
      ok(s2.type ~= dear.type or s2 ~= dear, "the pillaged type is gone")
    end
  end

  -- sack strips everything but the cheapest
  local city2 = captured(3)
  if city2 then
    local cheapest = city2.slots[1]
    local want = 0
    for i = 2, #city2.slots do
      want = want + math.floor(math.abs(g.types.byId[city2.slots[i].type].price) / 2)
    end
    local gold0, score0 = side.gold, side.diploScore or 0
    local got = game.sack(g, side, city2)
    eq(got, want, "sack pays for every type it strips")
    eq(side.gold, gold0 + want, "the gold was paid")
    eq(#city2.slots, 1, "only the cheapest type is left")
    eq(city2.slots[1], cheapest, "and it is the cheapest one")
    eq(city2.defence, 1, "one type means defence 1")
    local d = (side.diploScore or 0) - score0
    ok(d >= 6 and d <= 15, "sack costs 1d10+5 diplomatic score: " .. d)
  end

  -- raze leaves neutral ruins that produce nothing
  local city3 = captured(1)
  if city3 then
    local before = #game.sideCities(g, side)
    local other
    for _, c in ipairs(g.map.cities) do
      if c.ownerIndex == side.index and c ~= city3 then other = c break end
    end
    if other then game.vector(g, other, city3) end
    local score0 = side.diploScore or 0
    game.raze(g, side, city3)
    eq(city3.ownerIndex, nil, "a razed city is neutral")
    eq(#city3.slots, 0, "a razed city produces nothing")
    eq(city3.income, 0, "a razed city earns nothing")
    eq(#game.sideCities(g, side), before - 1, "the side no longer owns it")
    if other then eq(other.vectorTo, nil, "vectoring to it was cancelled") end
    local d = (side.diploScore or 0) - score0
    ok(d >= 11 and d <= 25, "raze costs 1d15+10 diplomatic score: " .. d)
  end

  -- pillaging a city with nothing to take does nothing
  local empty = { slots = {}, index = -1 }
  eq(game.pillage(g, side, empty), 0, "pillaging an empty city pays nothing")
  eq(game.sack(g, side, { slots = { { type = 11 } }, index = -1 }), 0,
     "sacking a one-type city pays nothing")
end

--------------------------------------------------------------------- heroes

local function testHeroes(scenario)
  print("heroes: " .. scenario)
  local heroMod = require("warlords.hero")
  local g = game.new(DATA, scenario, { seed = 37 })
  local side = game.begin(g)

  -- turn 1 always offers a free hero at the capital, carrying the standard
  local offer = side.heroOffer
  ok(offer ~= nil, "a hero offers itself on turn 1")
  eq(offer.price, 0, "the first hero is free")
  eq(offer.city, side.capital, "the first hero appears at the capital")
  -- the name is rolled with the offer, out of the side's own hundred
  local names = heroMod.names(g, side)
  eq(#names, 100, "a side has a hundred candidate heroes")
  ok(offer.name ~= nil and offer.name ~= "", "the offer carries a name")
  local found = false
  for _, n in ipairs(names) do
    if n.name == offer.name then found = true end
  end
  ok(found, "and the name comes from that side's file")

  -- side 0's list carries no #1 line at all, so the Sirians field no heroines
  local anyFemale = false
  for _, n in ipairs(heroMod.names(g, g.map.sides[1])) do
    if n.female then anyFemale = true end
  end
  ok(not anyFemale, "side 0 has no heroines to offer")

  local h, allies = heroMod.recruit(g, side, offer)
  eq(h.type, armytype.HERO, "the recruit is a hero")
  eq(h.name, offer.name, "the hero keeps the name it was offered under")
  eq(h.strength, 5, "a new hero has strength 5")
  eq(h.maxMoves, 14, "a new hero has 14 movement")
  -- hero_recruit writes the 14 to both +6 and +7, so the hero can ride on the
  -- turn it joins. A produced army is the one that has to wait.
  eq(h.moves, 14, "a new hero joins with its moves already full")
  eq(#allies, 0, "the first hero brings no allies")
  eq(#h.items, 1, "the first hero carries one item")
  eq(h.items[1].type, rules.ITEM_STANDARD, "and it is the side's standard")

  -- promotions: one step at a time, +1 strength and +2 movement each
  h.experience = 15
  local promoted = heroMod.checkPromotions(g, side)
  eq(#promoted, 1, "15 experience promotes a Hero")
  eq(h.level, 2, "the hero is now level 2")
  eq(h.title, "Cavalier", "level 2 is a Cavalier")
  eq(h.strength, 6, "promotion adds a strength")
  eq(h.maxMoves, 16, "promotion adds 2 movement")
  eq(#heroMod.checkPromotions(g, side), 0, "no second promotion on the same experience")
  h.experience = 60
  heroMod.checkPromotions(g, side)
  eq(h.level, 3, "60 experience promotes one step only")
  heroMod.checkPromotions(g, side)
  eq(h.title, "Paladin", "the next check reaches Paladin")
  eq(#heroMod.checkPromotions(g, side), 0, "a Paladin is not promoted again")

  -- experience is capped at 60
  heroMod.addExperience(g, h, 99)
  eq(h.experience, rules.MAX_HERO_XP, "experience is capped at 60")

  -- allies: a later hero brings 1-3 of one magical type
  local type, n = heroMod.allies(g)
  ok(type ~= nil, "the allies have a type")
  ok(n >= 1 and n <= 3, "1 to 3 allies arrive")

  -- create_ally_army (6536:12e6) gives an ally the type's own move in both the
  -- maximum and the moves left, and no upkeep at all
  g.turn = 4
  local _, escort = heroMod.recruit(g, side, { price = 0, city = side.capital })
  ok(#escort >= 1, "a later hero arrives with an escort")
  for _, a in ipairs(escort) do
    eq(a.moves, a.maxMoves, "an ally joins with its moves already full")
    eq(a.upkeep, 0, "an ally costs no upkeep")
  end
  g.turn = 1

  -- the hero cap per side
  eq(heroMod.MAX_PER_SIDE, 5, "a side may hold 5 heroes")
  eq(heroMod.MAX_IN_GAME, 40, "the game holds 40 heroes")
  local blocked = true
  for _ = 1, 5 do
    g.armies[#g.armies + 1] = { type = armytype.HERO, owner = side.index, x = 0, y = 0 }
  end
  g.turn = 2
  for _ = 1, 50 do if heroMod.offer(g, side) then blocked = false end end
  ok(blocked, "no hero is offered once the side is at its limit")

  -- death: items drop where the hero fell, and drown in water
  local carrier = { type = armytype.HERO, owner = side.index,
                    items = { { name = "Firesword", type = rules.ITEM_BATTLE, value = 1 } } }
  local dry
  for y = 0, g.map.height - 1 do
    for x = 0, g.map.width - 1 do
      if not dry and require("warlords.scn").terrainAt(g.map, x, y) == movement.PLAIN then
        dry = { x = x, y = y }
      end
    end
  end
  local dropped = heroMod.dropItems(g, carrier, dry.x, dry.y)
  eq(#dropped, 1, "the item was dropped")
  eq(dropped[1].status, 1, "a dropped item lies on the ground")
  eq(dropped[1].x, dry.x, "it lies where the hero fell")
  eq(#carrier.items, 0, "the dead hero carries nothing")

  local wet
  for y = 0, g.map.height - 1 do
    for x = 0, g.map.width - 1 do
      if not wet and require("warlords.scn").terrainAt(g.map, x, y) == movement.WATER then
        wet = { x = x, y = y }
      end
    end
  end
  if wet then
    local drowner = { type = armytype.HERO, owner = side.index,
                      items = { { name = "Icesword", type = rules.ITEM_BATTLE, value = 1 } } }
    local lost = drowner.items[1]
    eq(#heroMod.dropItems(g, drowner, wet.x, wet.y), 0, "nothing is dropped in water")
    eq(lost.status, 0, "an item lost at sea is gone for good")
  end
end

local function testHeroExperienceBug()
  print("hero experience: the original's bug")
  local heroMod = require("warlords.hero")
  local g = game.new(DATA, "ERYTHEA", { seed = 41 })

  local function run()
    local defHero = { type = armytype.HERO, strength = 5, owner = 1, experience = 0 }
    local attackers = { { type = 11, strength = 3, owner = 0 } }
    local defenders = { defHero }
    local result = { deadByArmy = {} }
    heroMod.battleExperience(g, attackers, defenders, result, false)
    return defHero.experience
  end

  rules.bugs.heroExperienceReadsAttackerTypes = true
  eq(run(), 0, "with the bug on, a defending hero facing a non-hero gains nothing")

  rules.bugs.heroExperienceReadsAttackerTypes = false
  eq(run(), 1, "with the bug off, the defender is credited")
  rules.bugs.heroExperienceReadsAttackerTypes = true

  -- an attacking hero is always credited, and a city is worth 2
  local h = { type = armytype.HERO, strength = 5, owner = 0, experience = 0 }
  heroMod.battleExperience(g, { h }, {}, { deadByArmy = {} }, false)
  eq(h.experience, 1, "an attacking hero gains 1 in the open")
  heroMod.battleExperience(g, { h }, {}, { deadByArmy = {} }, true)
  eq(h.experience, 3, "attacking a city is worth 2")

  -- a dead hero is credited nothing
  local dead = { type = armytype.HERO, strength = 5, owner = 0, experience = 0 }
  heroMod.battleExperience(g, { dead }, {}, { deadByArmy = { [dead] = true } }, true)
  eq(dead.experience, 0, "a hero that died gains nothing")
end

------------------------------------------------------------ a whole AI game

local function testAIGame(scenario, turns)
  print(("AI game: %s, %d turns"):format(scenario, turns))
  local aiMod = require("warlords.ai")
  local g = game.new(DATA, scenario, { seed = 101 })
  local side = game.begin(g)

  local captures, battles = 0, 0
  local ownedAtStart = {}
  for _, s in ipairs(g.sides) do ownedAtStart[s.index] = #game.sideCities(g, s) end

  while side and g.turn <= turns do
    aiMod.playTurn(g, side)
    side = game.endTurn(g)
  end

  ok(g.turn > turns or side == nil, "the game ran to the turn limit")

  -- every invariant that must hold at any point in a game
  local perTile = {}
  for _, a in ipairs(g.armies) do
    ok(g.types.byId[a.type] ~= nil, "every army has a real type")
    if not a.transit then
      ok(a.x and a.y, "a placed army has a position")
      ok(a.x >= 0 and a.x < g.map.width and a.y >= 0 and a.y < g.map.height,
         "every army is on the map")
      local k = a.y * g.map.width + a.x
      perTile[k] = (perTile[k] or 0) + 1
    end
    ok((a.moves or 0) >= 0, "movement never goes negative")
    ok((a.moves or 0) <= rules.MAX_MOVE, "movement never exceeds the cap")
  end
  local worst = 0
  for _, n in pairs(perTile) do worst = math.max(worst, n) end
  ok(worst <= rules.MAX_STACK, "no tile ever holds more than 8 armies: " .. worst)

  for _, s in ipairs(g.sides) do
    ok(s.gold >= 0, s.name .. " never goes into debt")
    if not s.alive then ok(#game.sideCities(g, s) == 0, "a dead side owns nothing") end
  end

  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex ~= nil then
      ok(g.map.sides[c.ownerIndex + 1] ~= nil, "a city's owner is a real side")
    end
    ok(#c.slots <= 4, "a city never gains production slots")
  end

  -- the game has actually moved on: somebody owns more than they started with
  local moved = false
  for _, s in ipairs(g.sides) do
    local now = #game.sideCities(g, s)
    if now ~= ownedAtStart[s.index] then moved = true end
    captures = captures + math.max(0, now - ownedAtStart[s.index])
  end
  ok(moved, "cities changed hands over " .. turns .. " turns")
  ok(captures > 0, ("the computer players took %d cities"):format(captures))

  -- and armies were actually produced
  ok(#g.armies > #g.map.cities, ("armies were built: %d from %d cities")
     :format(#g.armies, #g.map.cities))
  battles = battles
end

----------------------------------------------------------------- army slots

-- The army slots: which armies a click puts in the moving group, and what
-- the bar draws under them. docs/re/ui.md > The army slots.
local function testSlots()
  print("army slots")
  local slots = require("warlords.slots")

  -- a stand-in game: all the model wants is the fight-order table and the
  -- army list, so a real scenario is not needed to pin the rules down
  local function stack(n)
    local g = { map = { fightOrder = { [0] = {} } }, armies = {} }
    local armies = {}
    for i = 1, n do
      armies[i] = { type = i, moves = 10 + i, group = 0 }
      g.map.fightOrder[0][i] = n - i          -- the first army ranks highest
      g.armies[i] = armies[i]
    end
    return g, armies
  end

  do                                           -- a fresh pick-up
    local g, a = stack(3)
    local s = slots.build(g, a, 0)
    eq(s.n, 3, "three armies fill three slots")
    eq(s.army[1], a[1], "the highest in the fight order takes the first slot")
    ok(s.inGroup[1] and not s.inGroup[2] and not s.inGroup[3],
       "clicking a tile selects one army, not the stack")
    eq(s.group[1], 0, "ungrouped armies are each their own group")
    eq(s.group[3], 2, "numbered off down the bar")
    eq(s.mark[1], slots.TICK, "the one that moves is ticked")
    eq(s.mark[2], slots.CROSS, "the others are crossed")
    eq(slots.moves(s), 11, "and Group Move is that army's own")
    eq(slots.grouped(s), false, "so the Grp button is not green yet")
    eq(#slots.selected(s), 1, "one army moves")
  end

  do                                           -- adding and dropping
    local g, a = stack(3)
    local s = slots.build(g, a, 0)
    slots.toggle(s, g, 2)
    ok(s.inGroup[2] and s.group[2] == s.group[1], "a click adds an army to the group")
    eq(s.mark[2], nil, "only the head of a group is marked")
    eq(slots.moves(s), 11, "the group moves at its slowest army's pace")
    slots.toggle(s, g, 2)
    ok(not s.inGroup[2], "clicking it again drops it out")
    ok(s.group[2] ~= s.group[1], "into a group of its own")
    slots.toggle(s, g, 1)
    ok(s.inGroup[1], "the last army of a group cannot be dropped")
  end

  do                                           -- the bulk handlers
    local g, a = stack(3)
    local s = slots.build(g, a, 0)
    slots.all(s, g)
    eq(#slots.selected(s), 3, "Grp takes the whole stack")
    eq(slots.grouped(s), true, "and turns the button green")
    eq(s.mark[2], nil, "one mark for the one group")
    slots.single(s, g)
    eq(#slots.selected(s), 1, "and Grp again breaks it up")
    eq(slots.grouped(s), false, "leaving the button red")
    slots.pickGroup(s, g, 3)
    ok(s.inGroup[3] and not s.inGroup[1], "a mark picks out that group alone")
    eq(s.mark[3], slots.TICK, "which is then the ticked one")
  end

  do                                           -- a part-selected group
    local g, a = stack(3)
    local s = slots.build(g, a, 0)
    slots.all(s, g)
    s.inGroup[3] = false                       -- as no handler would leave it
    slots.pickGroup(s, g, 1)
    ok(s.inGroup[3], "picking a group takes all of it")
  end

  do                                           -- the grouping is remembered
    local g, a = stack(3)
    local s = slots.build(g, a, 0)
    slots.all(s, g)
    slots.commit(s, g)
    local id = a[1].group
    ok(id ~= slots.UNGROUPED and id == a[2].group and id == a[3].group,
       "a group of more than one is written back into its armies")

    -- picking the stack up again, this turn or the next, takes the group
    local again = slots.build(g, a, 0)
    eq(#slots.selected(again), 3, "so picking it up again takes the whole group")
    eq(slots.grouped(again), true, "with the Grp button still green")
    eq(again.group[1], again.group[3], "and all three in one group in the bar")
    slots.commit(again, g)
    eq(a[1].group, id, "picking it up does not renumber it")

    slots.toggle(again, g, 3)
    slots.commit(again, g)
    local third = slots.build(g, a, 0)
    eq(#slots.selected(third), 2, "dropping one out leaves the rest grouped")
    eq(a[3].group, slots.UNGROUPED, "and the one dropped out is on its own")
    eq(a[1].group, id, "while the group keeps its id")

    slots.single(third, g)
    slots.commit(third, g)
    eq(a[1].group, slots.UNGROUPED, "and breaking it up clears the grouping")
    eq(#slots.selected(slots.build(g, a, 0)), 1,
       "so a click picks up one army again")
  end

  do                                           -- survivors
    local g, a = stack(3)
    local s = slots.build(g, a, 0)
    slots.all(s, g)
    local left = slots.keep(s, g, { a[1], a[3] })
    eq(left.n, 2, "the dead leave the bar")
    ok(left.inGroup[1] and left.inGroup[2], "the survivors keep moving together")
    eq(slots.keep(s, g, {}), nil, "and a wiped-out stack is no selection at all")
  end
end

---------------------------------------------------------------------- sites

-- The screen layout comes out of the game's own files, so these are the
-- numbers docs/formats/screens.md records, checked against the shipped data.
local function testScreenLayout()
  print("screen layout")
  local uidata = require("warlords.uidata")
  local ui = uidata.load(DATA)

  eq(ui.bitmaps[0], "startup.pck", "bitmap 0 is the startup screen")
  eq(ui.bitmaps[4], "button.pck", "bitmap 4 is the button sheet")
  eq(ui.bitmaps[78], "movebar7.pck", "there are 79 bitmaps, 0 to 78")

  local n = 0
  for _ in pairs(ui.joins) do n = n + 1 end
  eq(n, 36, "JOIN.DAT names 36 dialogs")
  eq(ui.joins[0].button, 0, "dialog 0 uses button group 0")
  eq(ui.joins[0].area, 2, "dialog 0 uses area screen 2")
  -- the join is not the identity: three pairs of dialogs swap button groups
  eq(ui.joins[9].button, 11, "dialog 9 takes button group 11")
  eq(ui.joins[12].button, 9, "and dialog 12 takes group 9")

  local d = uidata.dialog(ui, uidata.MAIN_SCREEN)
  eq(#d.controls, 43, "the main screen has 43 controls")
  eq(#d.regions, 6, "and 6 regions")

  local want = {
    [1] = { 400, 30, 224, 312 },   -- strategic map
    [2] = { 16, 30, 360, 360 },    -- map viewport: 9x9 tiles of 40
    [3] = { 0, 0, 640, 18 },       -- menu bar
    [9] = { 16, 408, 360, 56 },    -- bottom bar
  }
  for _, r in ipairs(d.regions) do
    local w = want[r.id]
    if w then
      eq(r.x, w[1], ("region %d x"):format(r.id))
      eq(r.y, w[2], ("region %d y"):format(r.id))
      eq(r.w, w[3], ("region %d width"):format(r.id))
      eq(r.h, w[4], ("region %d height"):format(r.id))
    end
  end

  -- the four configurable buttons, from UDB/UDB.CUR and UDB/UDB.DAT
  eq(ui.shortcutNames[507], "Move All", "UDB.DAT names menu item 507")
  eq(ui.shortcutNames[535], "End Turn", "and item 535")
  local want = { [0] = "Search", "Move All", "Heroes", "End Turn" }
  for i = 0, 3 do
    eq(ui.shortcutNames[ui.shortcuts[i]], want[i],
       ("button %d (control %d) is %s"):format(i, 179 + i, want[i]))
  end

  -- and their icons, which UDB.DAT gives as rects into MENUBUTT.PCK: a 10x7
  -- grid of 32x29 cells 32 and 28 apart. Every one of the 21 items names
  -- three cells -- resting, lit, greyed -- and no two items share a cell.
  do
    local w, h = require("warlords.pck").decode(DATA .. "/PICS/MENUBUTT.PCK")
    eq(w, 320, "MENUBUTT.PCK is 320 wide")
    eq(h, 197, "and 197 tall: 7 rows 28 apart, plus the last row's border")
    local cells, n = {}, 0
    for id, item in pairs(ui.shortcutItems) do
      eq(item.w, 32, ("item %d icon width"):format(id))
      eq(item.h, 29, ("item %d icon height"):format(id))
      for st = 0, 2 do
        local src = item.src[st]
        eq(src.x % 32, 0, ("item %d state %d sits on a column"):format(id, st))
        eq(src.y % 28, 0, ("item %d state %d sits on a row"):format(id, st))
        ok(src.x + 32 <= w and src.y + 29 <= h,
           ("item %d state %d is inside the sheet"):format(id, st))
        local key = src.x .. "," .. src.y
        ok(not cells[key], ("cell %s is used once"):format(key))
        cells[key], n = true, n + 1
      end
    end
    eq(n, 63, "21 items, three cells each")
    -- End Turn is the shield in the top-left corner, lit to its right
    local endTurn = ui.shortcutItems[535]
    eq(endTurn.src[uidata.NORMAL].x, 0, "End Turn rests at the sheet's origin")
    eq(endTurn.src[uidata.NORMAL].y, 0, "at the top of it")
    eq(endTurn.src[uidata.ACTIVE].x, 32, "and lights up one cell right")
  end

  -- ABITS.PCK's rings: nine 32x30 cells on a 32-pixel stride, grey then one
  -- per side. A 40x40 cell drags in the strip of other bits below them.
  do
    local pckM = require("warlords.pck")
    local w, h, px = pckM.decode(DATA .. "/PICS/ABITS.PCK")
    eq(w, 480, "ABITS.PCK is 480 wide")
    eq(h, 40, "and 40 tall")
    local BG, seen = 3, {}
    for cell = 0, 8 do
      local count = {}
      for y = 0, 29 do
        for x = cell * 32, cell * 32 + 31 do
          local v = px[y * w + x + 1]
          if v ~= BG then count[v] = (count[v] or 0) + 1 end
        end
      end
      local best, bestN = nil, 0
      for v, n in pairs(count) do if n > bestN then best, bestN = v, n end end
      ok(bestN > 40, ("ring %d has ink"):format(cell))
      if cell > 0 then
        ok(not seen[best], ("ring %d has its own colour"):format(cell))
        seen[best] = true
      end
    end
    -- the strip below the rings is something else, so the cell must stop at 30
    local ringRow, stripRow = 0, 0
    for x = 0, 287 do
      if px[15 * w + x + 1] ~= BG then ringRow = ringRow + 1 end
      if px[38 * w + x + 1] ~= BG then stripRow = stripRow + 1 end
    end
    ok(stripRow > ringRow * 2,
       "row 38 is far denser than row 15, so it is not part of the rings")
  end

  -- and its right-hand end: the four 40x20 status-bar icons 89e0:05a3 blits,
  -- past the rings. Each must have ink and stand clear of its neighbours.
  do
    local pckM = require("warlords.pck")
    local w, h, px = pckM.decode(DATA .. "/PICS/ABITS.PCK")
    local BG = 3
    local want = {
      { 344,  0, "cities" }, { 344, 20, "treasury" },
      { 384,  0, "income" }, { 384, 20, "upkeep" },
    }
    for _, cell in ipairs(want) do
      local sx, sy, name = cell[1], cell[2], cell[3]
      ok(sx >= 288, ("the %s icon is past the nine rings"):format(name))
      ok(sx + 40 <= w and sy + 20 <= h,
         ("the %s icon is inside the sheet"):format(name))
      local ink = 0
      for y = sy, sy + 19 do
        for x = sx, sx + 39 do
          if px[y * w + x + 1] ~= BG then ink = ink + 1 end
        end
      end
      ok(ink > 40, ("the %s icon has ink"):format(name))
      -- nothing of the neighbouring cell leaks in: the column before it is bare
      local edge = 0
      for y = sy, sy + 19 do
        if px[y * w + sx] ~= BG then edge = edge + 1 end
      end
      eq(edge, 0, ("the %s icon starts on a clear column"):format(name))
    end
  end

  -- every control's three source rects must lie inside its own bitmap
  local pckMod = require("warlords.pck")
  local sizes, checked = {}, 0
  for _, group in pairs(ui.buttons) do
    for _, c in ipairs(group.controls) do
      if c.bitmap ~= 0 and c.w > 0 then
        local name = ui.bitmaps[c.bitmap]
        if sizes[name] == nil then
          sizes[name] = false
          for _, dir in ipairs({ "/PICS/", "/TERRAIN0/", "/" }) do
            local p = DATA .. dir .. name:upper()
            local f = io.open(p, "rb")
            if f then
              f:close()
              local w, h = pckMod.decode(p)
              sizes[name] = { w = w, h = h }
              break
            end
          end
        end
        local s = sizes[name]
        ok(s ~= false, "bitmap resolves to a file: " .. tostring(name))
        if s then
          for st = 0, 2 do
            ok(c.src[st].x + c.w <= s.w and c.src[st].y + c.h <= s.h,
               ("control %d state %d lies inside %s"):format(c.id, st, name))
            checked = checked + 1
          end
        end
      end
    end
  end
  ok(checked >= 400, ("checked %d source rects"):format(checked))
end

-- A city's castle is the map's own terrain, restamped when it changes hands
-- (scn.setCityTiles, from set_city_tiles 6bd8:0000 and city_make_ruins
-- 649c:016b). The scenario's terrain table is an independent witness: it
-- marks every castle cell as terrain 10 and every razed cell as 11, with
-- the mountain sprite sitting in the gap the owner arithmetic skips.
local function testCityCastles()
  print("city castles")
  local scn = require("warlords.scn")
  local g = game.new(DATA, "TUTORIA", { seed = 11 })
  local map = g.map

  local WANT = { [-1] = 96, [0] = 98, [1] = 100, [2] = 102, [3] = 104,
                 [4] = 106, [5] = 108, [6] = 128, [7] = 130 }
  local city = map.cities[1]
  for owner, base in pairs(WANT) do
    city.ownerIndex = owner >= 0 and owner or nil
    city.razed = nil
    ok(scn.cityTileBase(city) == base,
       ("owner %d takes castle block %d"):format(owner, base))
    scn.setCityTiles(map, city)
    ok(scn.tileAt(map, city.x, city.y) == base, "top-left cell is the block")
    ok(scn.tileAt(map, city.x + 1, city.y) == base + 1, "top-right is +1")
    ok(scn.tileAt(map, city.x, city.y + 1) == base + 16, "bottom-left is +16")
    ok(scn.tileAt(map, city.x + 1, city.y + 1) == base + 17, "bottom-right is +17")
    -- and all four cells must still count as city terrain, or movement,
    -- defence and the cost grid would all read the castle as open ground
    for dx = 0, 1 do
      for dy = 0, 1 do
        ok(scn.terrainAt(map, city.x + dx, city.y + dy) == 10,
           ("castle cell %d,%d is city terrain"):format(dx, dy))
      end
    end
  end

  -- the razed row is contiguous, eight sides from 0xa0, and keeps the
  -- colours of whoever held the city
  for owner = 0, 7 do
    city.ownerIndex, city.razed, city.razedBy = nil, true, owner
    ok(scn.cityTileBase(city) == 0xa0 + 2 * owner,
       ("ruins of side %d take block %d"):format(owner, 0xa0 + 2 * owner))
    scn.setCityTiles(map, city)
    for dx = 0, 1 do
      for dy = 0, 1 do
        ok(scn.terrainAt(map, city.x + dx, city.y + dy) == 11,
           ("ruin cell %d,%d is ruins terrain"):format(dx, dy))
      end
    end
  end

  -- capture and raze must restamp through the game, not just by hand
  local g2 = game.new(DATA, "TUTORIA", { seed = 12 })
  local c2 = g2.map.cities[1]
  for _, side in ipairs(g2.sides) do
    game.setCityOwner(g2, c2, side.index)
    scn.setCityTiles(g2.map, c2)
  end
  local mine = game.sideCities(g2, g2.sides[1])[1]
  ok(mine ~= nil, "the first side holds a city to raze")
  if mine then
    local was = mine.ownerIndex
    game.raze(g2, g2.sides[1], mine, {})
    ok(mine.razedBy == was, "raze remembers who held the city")
    ok(scn.tileAt(g2.map, mine.x, mine.y) == 0xa0 + 2 * was,
       "raze restamps the map with that side's ruins")
  end
end

local function testSites(scenario)
  print("sites: " .. scenario)
  local siteMod = require("warlords.site")
  local heroMod = require("warlords.hero")
  local g = game.new(DATA, scenario, { seed = 53 })

  -- every site holds something, and only ruins are rich
  local rich, temples, contents = 0, 0, {}
  for _, s in ipairs(g.map.sites) do
    ok(s.content ~= nil, "every site has contents")
    ok(s.content ~= siteMod.EMPTY, "no site is left empty")
    if s.rich then
      rich = rich + 1
      ok(s.type ~= siteMod.TEMPLE, "a temple is never rich")
    end
    if s.content == siteMod.TEMPLE then temples = temples + 1 end
    contents[s.content] = (contents[s.content] or 0) + 1
    if s.content == siteMod.ALLIES then
      ok(s.allyType ~= nil, "an ally ruin knows what joins")
      ok(g.types.byId[s.allyType] ~= nil, "and it is a real army type")
    end
    if s.content ~= siteMod.TEMPLE then
      ok(s.guardian and s.guardian >= 1 and s.guardian <= 9, "a ruin has a guardian")
    end
  end
  eq(rich, math.floor(#g.map.sites * 3 / 10), "three in ten sites are rich")

  -- items: each hidden item is in exactly one ruin
  local placed = {}
  for _, s in ipairs(g.map.sites) do
    if s.content == siteMod.ITEM then
      ok(not placed[s.item], "an item is hidden in only one ruin")
      placed[s.item] = true
    end
  end
  for _, it in ipairs(g.map.items) do
    if placed[it.index] then eq(it.status, 2, "a hidden item is marked hidden") end
  end

  -- reserved items only ever go in rich ruins, ordinary ones never do
  local byIndex = {}
  for _, it in ipairs(g.map.items) do byIndex[it.index] = it end
  for _, s in ipairs(g.map.sites) do
    if s.content == siteMod.ITEM then
      eq(siteMod.itemReserved(byIndex[s.item]), s.rich,
         "a reserved item needs a rich ruin")
    end
  end

  -- the pool refilled records 8..21 from the .ITM file
  if g.map.itemPool then
    local names = {}
    for _, p in ipairs(g.map.itemPool) do names[p.name] = true end
    for i = 8, 21 do
      if byIndex[i] then ok(names[byIndex[i].name], "item " .. i .. " came from the pool") end
    end
    for i = 0, 7 do
      if byIndex[i] then eq(byIndex[i].type, rules.ITEM_STANDARD, "records 0-7 are standards") end
    end
  end

  -- searching a ruin needs a hero
  local ruin
  for _, s in ipairs(g.map.sites) do
    if s.content == siteMod.GOLD then ruin = s break end
  end
  if ruin then
    local side = g.sides[1]
    local grunt = { x = ruin.x, y = ruin.y, owner = side.index, type = 11,
                    strength = 3, maxMoves = 10, moves = 10 }
    g.armies[#g.armies + 1] = grunt
    local r = siteMod.search(g, { grunt }, ruin.x, ruin.y)
    eq(r.kind, "no hero", "a stack without a hero cannot search a ruin")
    ok(not ruin.searched, "and the ruin is not used up")

    -- with a strong hero it pays out
    local h = { x = ruin.x, y = ruin.y, owner = side.index, type = armytype.HERO,
                strength = 9, maxMoves = 14, moves = 14, experience = 0, items = {} }
    g.armies[#g.armies + 1] = h
    local gold0 = side.gold
    r = siteMod.search(g, { h, grunt }, ruin.x, ruin.y)
    ok(r.kind == "gold" or r.kind == "killed", "a hero gets a result: " .. r.kind)
    if r.kind == "gold" then
      ok(r.gold >= 503 and r.gold <= 4000, "the gold is in range: " .. r.gold)
      eq(side.gold, gold0 + r.gold, "the gold was paid")
      eq(h.experience, 3, "searching is worth 3 experience")
    end
    ok(ruin.searched, "a searched ruin is used up")
    eq(siteMod.search(g, { h }, ruin.x, ruin.y), nil, "and cannot be searched again")
  end

  -- a temple blesses each army once, and only the first four temples bless
  local temple
  for _, s in ipairs(g.map.sites) do
    if s.content == siteMod.TEMPLE and s.templeIndex == 0 then temple = s break end
  end
  if temple then
    local side = g.sides[1]
    local a = { x = temple.x, y = temple.y, owner = side.index, type = 11, strength = 3 }
    local r = siteMod.search(g, { a }, temple.x, temple.y)
    eq(r.kind, "temple", "a temple blesses")
    eq(r.blessed, 1, "one army was blessed")
    eq(a.strength, 4, "the blessing adds a strength")
    eq(siteMod.search(g, { a }, temple.x, temple.y).blessed, 0,
       "the same temple does not bless twice")
    a.strength = 9
    a.blessings = {}
    siteMod.search(g, { a }, temple.x, temple.y)
    eq(a.strength, 9, "a blessing never passes 9")
  end

  -- the guardian formula
  local strong = { strength = 9, items = {} }
  local weak = { strength = 1, items = {} }
  local survived, died = 0, 0
  for _ = 1, 400 do
    if siteMod.survivesGuardian(g, strong, { strong }, 3) then survived = survived + 1 end
    if not siteMod.survivesGuardian(g, weak, { weak }, 8) then died = died + 1 end
  end
  eq(survived, 400, "a strong hero always beats a weak guardian")
  ok(died > 0, "a weak hero sometimes dies to a strong one")
end

----------------------------------------------------------------- diplomacy

local function testDiplomacy()
  print("diplomacy")
  local d = require("warlords.diplomacy")

  -- the option decides where everyone starts
  local off = game.new(DATA, "ERYTHEA", { seed = 61 })
  eq(off.map.options.diplomacy, 0, "the shipped scenarios have diplomacy off")
  eq(d.state(off, 0, 1), d.WAR, "with the option off every pair starts at war")
  ok(d.mayAttack(off, 0, 1), "and may attack freely")

  local g = game.new(DATA, "ERYTHEA", { seed = 61, options = { diplomacy = 1 } })
  eq(d.state(g, 0, 1), d.PEACE, "with the option on every pair starts at peace")
  ok(not d.mayAttack(g, 0, 1), "a side at peace may not attack")
  ok(d.mayAttack(g, 0, nil), "neutrals may always be attacked")
  eq(d.state(g, 0, 0), d.PEACE, "a side is at peace with itself")

  -- escalation lands at once, for both sides
  d.propose(g, 0, 1, d.WAR)
  local msgs = d.apply(g, g.sides[1])
  eq(d.state(g, 0, 1), d.WAR, "declaring war takes effect at once")
  eq(d.state(g, 1, 0), d.WAR, "and binds the other side too")
  ok(msgs[1] and msgs[1]:find("War declared"), "and is announced")
  eq(d.proposal(g, 1, 0), d.WAR, "the other side's proposal is raised to match")

  -- de-escalation needs both
  local g2 = game.new(DATA, "ERYTHEA", { seed = 62, options = { diplomacy = 1 } })
  d.propose(g2, 0, 1, d.WAR)
  d.apply(g2, g2.sides[1])
  g2.diplomacy.proposal[1 * 8 + 0] = nil                -- forget their matching proposal
  d.propose(g2, 0, 1, d.PEACE)
  d.apply(g2, g2.sides[1])
  eq(d.state(g2, 0, 1), d.WAR, "one-sided peace does not land")
  d.propose(g2, 0, 1, d.PEACE)
  d.propose(g2, 1, 0, d.PEACE)
  local m2 = d.apply(g2, g2.sides[1])
  eq(d.state(g2, 0, 1), d.PEACE, "matched proposals make peace")
  ok(m2[1] and m2[1]:find("Peace negotiated"), "and it is announced")

  -- suing for peace raises the diplomatic score, exactly as an atrocity
  -- does: as the turn ends, a proposal below both the state and the other
  -- side's proposal is scored (diplomacy_score_update, 484e:1063)
  local g3 = game.new(DATA, "ERYTHEA", { seed = 63, options = { diplomacy = 1 } })
  d.propose(g3, 0, 1, d.WAR)
  d.apply(g3, g3.sides[1])
  d.propose(g3, 0, 1, d.PEACE)
  local before = g3.sides[1].diploScore or 0
  d.scoreUpdate(g3, g3.sides[1])
  local gain = (g3.sides[1].diploScore or 0) - before
  ok(gain >= 11 and gain <= 20, "peace offered from war is worth 1d10+10: " .. gain)
  d.propose(g3, 1, 0, d.PEACE)
  before = g3.sides[1].diploScore
  d.scoreUpdate(g3, g3.sides[1])
  eq(g3.sides[1].diploScore, before, "not once the other side offers it too")

  -- ratings are relative and lowest-first
  local g3 = game.new(DATA, "ERYTHEA", { seed = 63 })
  for i, s in ipairs(g3.sides) do s.diploScore = i * 10 end
  local r = d.ratings(g3)
  eq(r[g3.sides[1].index], "Statesman", "the lowest score is the Statesman")
  eq(r[g3.sides[#g3.sides].index], "Running Dog", "the highest is the Running Dog")

  -- with two sides in play only the two extremes are used
  local ranks = d.RATING_RANKS[2]
  eq(#ranks, 2, "two sides take two titles")
  eq(d.TITLES[ranks[1]], "Statesman", "best of two")
  eq(d.TITLES[ranks[2]], "Running Dog", "worst of two")

  -- a stack at peace is blocked rather than drawn into a fight
  local g4 = game.new(DATA, "ERYTHEA", { seed = 64, options = { diplomacy = 1 } })
  local side = game.begin(g4)
  local victim
  for _, s in ipairs(g4.sides) do if s.index ~= side.index then victim = s end end
  local city = victim.capital
  local army = { x = city.x, y = city.y - 1, owner = side.index, type = 11,
                 name = "Scouts", strength = 3, maxMoves = 20, moves = 20, upkeep = 1 }
  g4.armies[#g4.armies + 1] = army
  local path = movement.findPath(g4, { army }, army.x, army.y, city.x, city.y)
  if path and #path > 0 then
    local r2 = movement.walk(g4, { army }, path)
    eq(r2.stopped, "at peace", "a stack at peace cannot walk into their city")
    eq(army.x, city.x, "and has not moved into it")
  end
end

-------------------------------------------------------------------- quests

local function testQuests()
  print("quests")
  local q = require("warlords.quest")
  local siteMod = require("warlords.site")

  -- with the option off no quest is ever assigned
  local off = game.new(DATA, "ERYTHEA", { seed = 71 })
  local sideOff = game.begin(off)
  local heroOff = { x = 10, y = 10, owner = sideOff.index, type = armytype.HERO,
                    strength = 5, experience = 0, items = {} }
  eq(q.assign(off, sideOff, heroOff), nil, "no quests when the option is off")

  local g = game.new(DATA, "ERYTHEA", { seed = 71, options = { quests = 1 } })
  local side = game.begin(g)
  local h = { x = side.capital.x, y = side.capital.y, owner = side.index,
              type = armytype.HERO, strength = 5, experience = 0, items = {} }
  g.armies[#g.armies + 1] = h

  local quest1 = q.assign(g, side, h)
  ok(quest1 ~= nil, "a quest is assigned")
  ok(quest1.type >= 0 and quest1.type <= 6, "with a known type")
  ok(quest1.target ~= nil, "and a target")
  eq(q.assign(g, side, h), nil, "only one quest at a time")
  ok(q.describe(quest1):len() > 0, "a quest describes itself")

  -- the type table: 4, 5 and 6 come up twice as often as 0..3
  local counts = {}
  for _, t in ipairs(q.TYPE_TABLE) do counts[t] = (counts[t] or 0) + 1 end
  eq(counts[0], 1, "type 0 has one slot")
  eq(counts[4], 2, "type 4 has two")
  eq(counts[6], 2, "type 6 has two")
  eq(#q.TYPE_TABLE, 10, "the table is rolled with 1d10")

  -- occupy: taking the target city with the hero completes it
  side.quest = { type = q.OCCUPY, hero = h, target = g.map.cities[2], done = 0 }
  side.gold = 5000
  local r = q.event(g, side, "occupy", { city = g.map.cities[2], stack = { h } })
  ok(r and not r.failed, "occupying the target completes the quest")
  eq(side.quest, nil, "and the quest is cleared")
  eq(h.experience, q.EXPERIENCE, "the hero gains 10 experience")
  ok(r.reward ~= nil, "a reward was given")

  -- ... but not without the hero
  h.experience = 0
  side.quest = { type = q.OCCUPY, hero = h, target = g.map.cities[3], done = 0 }
  r = q.event(g, side, "occupy", { city = g.map.cities[3], stack = {} })
  ok(r and r.failed, "occupying without the hero fails the quest: " .. tostring(r.failed))
  eq(h.experience, 0, "and pays no experience")

  -- raze: razing the occupy target is the wrong thing to do
  side.quest = { type = q.OCCUPY, hero = h, target = g.map.cities[4], done = 0 }
  r = q.event(g, side, "raze", { city = g.map.cities[4], stack = { h } })
  ok(r and r.failed, "razing a city you were to keep fails the quest")

  -- slaughter: dead armies of the target side count up to the total
  local victim = g.sides[#g.sides]
  side.quest = { type = q.SLAUGHTER, hero = h, target = victim, required = 3, done = 0 }
  local dead = { { owner = victim.index }, { owner = victim.index } }
  eq(q.event(g, side, "battle", { stack = { h }, killed = dead }), nil,
     "two of three is not enough")
  eq(side.quest.done, 2, "the count rises")
  r = q.event(g, side, "battle", { stack = { h }, killed = dead })
  ok(r and not r.failed, "the third kill completes it")

  -- ... and kills without the hero do not count
  side.quest = { type = q.SLAUGHTER, hero = h, target = victim, required = 2, done = 0 }
  q.event(g, side, "battle", { stack = {}, killed = dead })
  eq(side.quest.done, 0, "kills away from the hero do not count")

  -- pillage: gold adds up
  side.quest = { type = q.PILLAGE_GOLD, hero = h, target = true, required = 100, done = 0 }
  q.event(g, side, "pillage", { stack = { h }, gold = 60 })
  eq(side.quest.done, 60, "pillaged gold counts")
  r = q.event(g, side, "pillage", { stack = { h }, gold = 60 })
  ok(r and not r.failed, "reaching the total completes it")

  -- retrieve: the priests take the item back
  local item = { index = 99, name = "Testsword", type = rules.ITEM_BATTLE, value = 1, status = 3 }
  h.items = { item }
  side.quest = { type = q.RETRIEVE_ITEM, hero = h, target = item, done = 0 }
  r = q.event(g, side, "item", { hero = h })
  ok(r and not r.failed, "carrying the item completes the quest")
  local stillCarried = false
  for _, it in ipairs(h.items) do if it == item then stillCarried = true end end
  ok(not stillCarried, "the quest item is taken away (the reward may add another)")
  eq(item.status, 0, "it leaves play")

  -- slay hero: the target must die in a battle the quest hero was in
  local prey = { type = armytype.HERO, owner = victim.index }
  side.quest = { type = q.SLAY_HERO, hero = h, target = prey, done = 0 }
  eq(q.event(g, side, "battle", { stack = {}, killed = { prey } }), nil,
     "someone else killing the quarry does not count")
  r = q.event(g, side, "battle", { stack = { h }, killed = { prey } })
  ok(r and not r.failed, "the hero killing the quarry completes it")

  -- start of turn: losing the hero abandons the quest
  side.quest = { type = q.PILLAGE_GOLD, hero = { type = armytype.HERO }, required = 1, done = 0 }
  r = q.event(g, side, "turn")
  ok(r and r.failed, "a quest with a dead hero is abandoned: " .. tostring(r.failed))

  -- rewards: a poor side is given gold
  side.gold = 50
  local reward = q.reward(g, side, { hero = h })
  eq(reward.kind, "gold", "a side with no gold is given gold")
  ok(reward.gold >= 1002 and reward.gold <= 3000, "2d1000+1000: " .. reward.gold)

  -- a small side past turn 15 is given allies
  g.turn = 20
  side.gold = 5000
  reward = q.reward(g, side, { hero = h })
  eq(reward.kind, "allies", "a small side past turn 15 is given allies")
  ok(#reward.armies >= 1 and #reward.armies <= 8, "1d3+5 allies arrive")
  siteMod = siteMod
end

------------------------------------------------------------ end of the game

local function testEndGame()
  print("end of the game")

  -- one computer side left: it has triumphed, and is handed to the player
  local g = game.new(DATA, "ERYTHEA", { seed = 81 })
  for _, s in ipairs(g.sides) do s.computer = true end
  local winner = g.sides[1]
  for _, c in ipairs(g.map.cities) do
    c.ownerIndex = (c.ownerIndex ~= nil) and winner.index or nil
  end
  for i = 2, #g.sides do g.sides[i].alive = false end
  local r = game.checkEnd(g)
  ok(r.over, "the game is over")
  eq(r.winner, winner, "the last side standing wins")
  ok(not winner.computer, "and is switched to human control")
  ok(g.won, "the game-won flag is set")

  -- nobody left at all
  local g2 = game.new(DATA, "ERYTHEA", { seed = 82 })
  for _, c in ipairs(g2.map.cities) do c.ownerIndex = nil end
  local r2 = game.checkEnd(g2)
  ok(r2.over, "with no cities owned the game is over")
  eq(r2.winner, nil, "and nobody won")
  ok(r2.message:find("No more players"), "with the right message")

  -- a lone human needs more than half the standing cities
  local g3 = game.new(DATA, "ERYTHEA", { seed = 83 })
  for _, s in ipairs(g3.sides) do s.computer = false end
  local me = g3.sides[1]
  for i = 2, #g3.sides do g3.sides[i].alive = false end
  local standing, given = #g3.map.cities, 0
  for _, c in ipairs(g3.map.cities) do c.ownerIndex = nil end
  for _, c in ipairs(g3.map.cities) do
    if given < math.floor(standing / 2) then c.ownerIndex, given = me.index, given + 1 end
  end
  ok(not game.checkEnd(g3).over, "exactly half is not enough")
  for _, c in ipairs(g3.map.cities) do
    if c.ownerIndex == nil then c.ownerIndex = me.index break end
  end
  local r3 = game.checkEnd(g3)
  ok(r3.won, "more than half wins")
  ok(not r3.over, "and the game goes on, to be looked over")
  eq(r3.winner, me, "and names the winner")
  ok(not game.checkEnd(g3).won, "which is said once")

  -- surrender is offered while computers still play
  local g4 = game.new(DATA, "ERYTHEA", { seed = 84 })
  local human = g4.sides[1]
  human.computer = false
  for i = 2, #g4.sides do g4.sides[i].computer = true end
  for _, c in ipairs(g4.map.cities) do c.ownerIndex = nil end
  local rival = g4.sides[2]
  local n = #g4.map.cities
  for i, c in ipairs(g4.map.cities) do
    if i <= math.floor(n * 3 / 4) then c.ownerIndex = human.index
    elseif i == n then c.ownerIndex = rival.index end
  end
  local r4 = game.checkEnd(g4)
  ok(r4.surrender, "a dominant human is offered surrender")
  ok(not r4.over, "but the game is not over")
  ok(g4.surrenderOffered, "and the flag is set")

  -- razed cities do not count towards the total
  local g5 = game.new(DATA, "ERYTHEA", { seed = 85 })
  local side5 = g5.sides[1]
  for i = 2, #g5.sides do g5.sides[i].alive = false end
  for _, s in ipairs(g5.sides) do s.computer = false end
  for _, c in ipairs(g5.map.cities) do c.ownerIndex = nil end
  g5.map.cities[1].ownerIndex = side5.index
  for i = 2, #g5.map.cities do g5.map.cities[i].razed = true end
  ok(game.checkEnd(g5).won, "one city among ruins is still more than half")

  -- surrender taken: the computers are out and the game is won
  game.acceptSurrender(g4, human)
  ok(g4.won, "surrender accepted wins the game")
  ok(not rival.alive, "and the computer sides are out")
end

-------------------------------------------------------------- saving a game

local function testSave()
  print("save and load")
  local saveMod = require("warlords.save")
  local aiMod = require("warlords.ai")
  local q = require("warlords.quest")

  local g = game.new(DATA, "ERYTHEA", { seed = 91, options = { quests = 1, diplomacy = 1 } })
  local side = game.begin(g)
  while side and g.turn <= 8 do aiMod.playTurn(g, side); side = game.endTurn(g) end

  -- give a hero an item and a quest, so the references are exercised
  local h
  for _, a in ipairs(g.armies) do if a.type == armytype.HERO then h = a end end
  ok(h ~= nil, "the game has a hero to save")
  local owner = g.map.sides[h.owner + 1]
  owner.quest = { type = q.OCCUPY, hero = h, target = g.map.cities[5],
                  targetKind = "city", done = 0 }
  h.items = { g.map.items[1] }
  g.map.items[1].status = 3

  -- a grouped stack must come back grouped, so the group survives a save
  g.armies[1].group, g.armies[2].group = 4, 4
  -- and a dug-in army is still dug in when the game is picked up again
  g.armies[1].fortified = true

  local text = saveMod.encode(g)
  ok(#text > 1000, "the save has content")
  local h2 = saveMod.decode(text, DATA)

  eq(h2.turn, g.turn, "the turn survives")
  eq(h2.current, g.current, "whose turn it is survives")
  eq(#h2.armies, #g.armies, "every army survives")
  eq(h2.rng.state, g.rng.state, "the dice carry on where they left off")

  for i, a in ipairs(g.armies) do
    local b = h2.armies[i]
    eq(b.x, a.x, "army " .. i .. " keeps its place")
    eq(b.type, a.type, "army " .. i .. " keeps its type")
    eq(b.strength, a.strength, "army " .. i .. " keeps its strength")
    eq(b.moves, a.moves, "army " .. i .. " keeps its movement")
    eq(b.owner, a.owner, "army " .. i .. " keeps its owner")
    eq(b.group or 0, a.group or 0, "army " .. i .. " keeps the group it moves with")
    eq(b.fortified or false, a.fortified or false, "army " .. i .. " stays dug in")
  end

  for i, c in ipairs(g.map.cities) do
    eq(h2.map.cities[i].ownerIndex, c.ownerIndex, "city " .. i .. " keeps its owner")
    eq(h2.map.cities[i].producing, c.producing, "city " .. i .. " keeps its production")
    eq(#h2.map.cities[i].slots, #c.slots, "city " .. i .. " keeps its slots")
  end

  for i, s in ipairs(g.sides) do
    eq(h2.sides[i].gold, s.gold, s.name .. " keeps its gold")
    eq(h2.sides[i].alive, s.alive, s.name .. " keeps its standing")
    eq(h2.sides[i].diploScore or 0, s.diploScore or 0, s.name .. " keeps its score")
  end

  for i, s in ipairs(g.map.sites) do
    eq(h2.map.sites[i].content, s.content, "site " .. i .. " keeps its contents")
    eq(h2.map.sites[i].searched, s.searched, "site " .. i .. " remembers being searched")
    eq(h2.map.sites[i].rich, s.rich, "site " .. i .. " keeps its rich flag")
  end

  -- the references between objects are rebuilt, not copied
  local owner2 = h2.map.sides[owner.index + 1]
  ok(owner2.quest ~= nil, "the quest survives")
  eq(owner2.quest.type, q.OCCUPY, "with its type")
  eq(owner2.quest.target.index, g.map.cities[5].index, "and its target city")
  ok(owner2.quest.hero ~= nil, "and its hero")
  eq(owner2.quest.hero.type, armytype.HERO, "which is a hero")
  local carried = false
  for _, a in ipairs(h2.armies) do
    if a.items and #a.items > 0 then carried = true end
  end
  ok(carried, "carried items survive")

  -- diplomacy survives
  local d = require("warlords.diplomacy")
  eq(d.state(h2, 0, 1), d.state(g, 0, 1), "the diplomatic state survives")

  -- and the reloaded game keeps playing
  local side2 = h2.sides[h2.current]
  local before = h2.turn
  for _ = 1, #h2.sides * 2 do
    if not side2 then break end
    aiMod.playTurn(h2, side2)
    side2 = game.endTurn(h2)
  end
  ok(h2.turn > before, "the reloaded game plays on")

  -- a file round trip
  local path = os.tmpname()
  saveMod.write(g, path)
  local h3 = saveMod.read(path, DATA)
  eq(#h3.armies, #g.armies, "a save written to a file reads back")
  os.remove(path)
end

------------------------------------------------------------- the hidden map

local function testHiddenMap()
  print("hidden map")

  -- with the option off everything is visible and nothing is tracked
  local off = game.new(DATA, "ERYTHEA", { seed = 101 })
  ok(game.seen(off, 0, 5, 5), "with the option off every tile is seen")
  eq(game.reveal(off, 0, 5, 5, false), 0, "and nothing is revealed")

  local g = game.new(DATA, "ERYTHEA", { seed = 101, options = { hiddenMap = 1 } })
  local side = game.begin(g)

  -- a side starts seeing its own cities and armies
  ok(game.seen(g, side.index, side.capital.x, side.capital.y), "its capital is seen")
  ok(not game.seen(g, side.index, 0, 0), "the far corner is not")

  -- radius 1 in the open, 2 on a city or flying
  local g2 = game.new(DATA, "ERYTHEA", { seed = 102, options = { hiddenMap = 1 } })
  local open
  for y = 20, 60 do
    for x = 20, 60 do
      if not open and not game.cityAt(g2, x, y) then open = { x = x, y = y } end
    end
  end
  -- side 20 is nobody, so its map starts completely dark
  eq(game.reveal(g2, 20, open.x, open.y, false), 9, "in the open a stack sees 3x3")
  eq(game.reveal(g2, 20, open.x, open.y, false), 0, "seeing it again reveals nothing new")
  local far = { x = open.x + 20, y = open.y }
  eq(game.reveal(g2, 20, far.x, far.y, true), 25, "a flying stack sees 5x5")
  local city = g2.map.cities[1]
  eq(game.reveal(g2, 21, city.x, city.y, false), 25, "standing on a city sees 5x5")

  -- unseen tiles are impassable
  local army = game.sideArmies(g, side)[1]
  army.moves = 99
  local unseen
  for _, c in ipairs(g.map.cities) do
    if not unseen and not game.seen(g, side.index, c.x, c.y)
       and c.ownerIndex ~= side.index then unseen = c end
  end
  ok(unseen ~= nil, "there is a city the side has not seen")
  eq(movement.findPath(g, { army }, army.x, army.y, unseen.x, unseen.y), nil,
     "a side cannot path into the dark")

  -- ... and walking uncovers as it goes
  local before = 0
  for _ in pairs(g.explored[side.index] or {}) do before = before + 1 end
  local target
  for dx = -3, 3 do
    for dy = -3, 3 do
      local x, y = army.x + dx, army.y + dy
      if not target and x >= 0 and y >= 0 and game.seen(g, side.index, x, y)
         and (x ~= army.x or y ~= army.y) then
        local p = movement.findPath(g, { army }, army.x, army.y, x, y)
        if p and #p > 0 then target = { x = x, y = y } end
      end
    end
  end
  if target then
    movement.moveTo(g, { army }, target.x, target.y)
    local after = 0
    for _ in pairs(g.explored[side.index] or {}) do after = after + 1 end
    ok(after > before, ("walking uncovered %d more tiles"):format(after - before))
  end

  -- each side keeps its own map
  local other
  for _, s in ipairs(g.sides) do if s.index ~= side.index then other = s end end
  ok(not game.seen(g, other.index, side.capital.x, side.capital.y),
     "another side has not seen our capital")
end

--------------------------------------------------------------------- bugs

local function testBugFlags()
  print("bug compatibility")
  ok(rules.bugs.heroExperienceReadsAttackerTypes,
     "the original's bugs are reproduced by default")
  -- every flag must actually control something: a flag nothing reads is a
  -- promise the engine does not keep
  local used = {}
  for _, module in ipairs({ "hero", "combat", "game", "move", "ai", "site", "quest" }) do
    local f = io.open("love2d/warlords/" .. module .. ".lua", "r")
    if f then
      local text = f:read("*a")
      f:close()
      for name in text:gmatch("rules%.bugs%.([%w_]+)") do used[name] = true end
    end
  end
  for name in pairs(rules.bugs) do
    ok(used[name], "the " .. name .. " flag is read by the engine")
  end
end

------------------------------------------------------------ the computer AI

-- The computer players: their character cards, what the level and the card
-- set up, and the pieces of the original's AI that can be pinned down
-- without playing a whole game. docs/re/ai.md, docs/formats/crd.md.
local function testComputerPlayers()
  print("computer players")
  local aicard = require("warlords.aicard")
  local aiMod = require("warlords.ai")
  local core = require("warlords.ai.core")
  local groups = require("warlords.ai.groups")
  local move = require("warlords.move")

  -- the decks: nine cards a level
  for level = 0, 2 do eq(aicard.count(DATA, level), 9, "nine cards for level " .. level) end
  local w = aicard.load(DATA, 2, 0)
  eq(w.groups, 4, "the Standard Warlord runs four assault groups")
  eq(w.raze, 5, "and razes 5 in 1000")
  eq(w.humanShare, 3, "and turns on a human with 3x10+1d10 percent")
  eq(w.solidarity, 1, "and stands with the other computers")
  local k = aicard.load(DATA, 0, 0)
  eq(k.cautious, 1, "the Standard Knight is cautious")
  eq(k.groups, 1, "and runs one group")
  local name = aicard.describe(DATA, 2, 1)
  eq(name, "Attila the Hun", "a card's name is the first line of its .DSC")

  -- a die of no sides gives its bonus, as the original's dice does
  local r = require("warlords.rng").new(5)
  eq(r:dice(1, 0, 3), 3, "dice(1, 0, 3) is 3")

  -- straight-line distance, truncated (map_distance, 2012:1199)
  eq(core.dist(0, 0, 3, 4), 5, "distance is Euclidean")
  eq(core.dist(0, 0, 1, 1), 1, "and truncated")

  -- levels and cards fill the AI data
  local sides = {}
  for i = 0, 7 do sides[i] = { computer = true, level = i % 3 } end
  sides[0] = { computer = false, level = 0 }
  local g = game.new(DATA, "ERYTHEA", { seed = 71, sides = sides })
  local knight, lord, warlord = g.map.sides[4], g.map.sides[2], g.map.sides[3]
  eq(knight.level, 0, "side 3 plays as a Knight")
  eq(knight.ai.maxGroups, 1, "a Knight runs one group")
  eq(knight.ai.cautious, 1, "and is cautious")
  ok(not knight.ai.bold, "and never attacks a side it is not at war with")
  eq(lord.ai.maxGroups, 3, "a Lord three")
  ok(lord.ai.bold, "a Lord is bold -- the level sets it and a card cannot clear it")
  eq(warlord.ai.maxGroups, 4, "a Warlord four")
  ok(warlord.ai.humanShare >= 31 and warlord.ai.humanShare <= 40,
     "a Warlord turns on a human holding 31-40% of the world: " .. warlord.ai.humanShare)
  ok(knight.ai.humanShare >= 81 and knight.ai.humanShare <= 90, "a Knight at 81-90%")
  eq(g.map.fightOrder[3][8], w.fightOrder[8], "the card sets the side's fight order")

  -- every city is somebody's ground once the computers have shared them out
  local unclaimed = 0
  for _, c in ipairs(g.map.cities) do
    if c.claim == nil or c.claim == core.NEUTRAL then unclaimed = unclaimed + 1 end
  end
  eq(unclaimed, 0, "the computers claim every city between them (623c:010b)")
  for _, s in ipairs(g.sides) do
    eq(s.capital.claim, s.index, s.name .. " claims its capital")
  end
  for _, s in ipairs(g.sides) do
    ok(s.diploScore >= 1 and s.diploScore <= 8, "a diplomatic score starts at 1d8")
  end

  -- I am the Greatest: a human starts 400 worse off, and the computers spare
  -- each other while any of them fights a human
  local gg = game.new(DATA, "ERYTHEA", { seed = 72, sides = sides, greatest = true,
                                         options = { diplomacy = 1 } })
  ok(gg.map.sides[1].diploScore > 400, "I am the Greatest: a human's score starts above 400")
  ok(gg.map.sides[2].diploScore <= 8, "a computer's does not")
  local dip = require("warlords.diplomacy")
  gg.diplomacy.state[1 * 8 + 0], gg.diplomacy.state[0 * 8 + 1] = dip.WAR, dip.WAR
  local fights = groups.fightingHumans(gg)
  ok(fights[1], "a computer at war with the human is marked")
  require("warlords.ai.diplomacy").phase(gg, gg.map.sides[2])
  eq(dip.proposal(gg, 1, 2), dip.PEACE, "and no computer proposes war on it")

  -- a computer's path ignores the fog; a human's does not (1555:08bf)
  local h = game.new(DATA, "ERYTHEA", { seed = 73, sides = sides, options = { hiddenMap = 1 } })
  local human, comp = h.map.sides[1], h.map.sides[2]
  local hg, cg = move.grid(h, human.index), move.grid(h, comp.index)
  local fogged, open = 0, 0
  for k2 = 0, h.map.width * h.map.height - 1 do
    if hg[k2] % 8 == 0 and cg[k2] % 8 ~= 0 then fogged = fogged + 1 end
    if cg[k2] % 8 ~= 0 then open = open + 1 end
  end
  ok(fogged > 1000, "unseen ground blocks the human's paths only: " .. fogged)

  -- the odds of taking an empty tile are certain
  local army
  for _, a in ipairs(g.armies) do if a.owner == warlord.index then army = a end end
  local sel = core.select(g, { army })
  eq(core.odds(g, sel, 0, 0), 100, "nothing to fight is a sure thing")

  -- the neighbour table: six a city, nearest first unless one side is full
  local nb, nd = core.neighbours(g, g.map.cities[1])
  eq(#nb, 6, "a city has six neighbours")
  for j = 1, #nd do ok(nd[j] < 100, "each within reach") end

  -- nobody picks an enemy before turn 8 without having fought it
  eq(groups.pickEnemy(g, warlord), nil, "no enemy is picked in the first turns")
  warlord.ai.battles[4] = 3
  warlord.ai.citiesLost[4] = 2
  local picked = groups.pickEnemy(g, warlord)
  ok(picked ~= nil, "a side that has fought us can be picked")

  -- early vengeance: a Warlord razes or sacks a human's city taken early
  local c = g.map.cities[1]
  for _, cc in ipairs(g.map.cities) do if cc ~= knight.capital and cc ~= warlord.capital then c = cc break end end
  c.ownerIndex = warlord.index
  local before = warlord.diploScore
  ok(groups.earlyVengeance(g, warlord.index, c, 0), "a Warlord takes vengeance on a human's city")
  ok(warlord.diploScore > before, "and it is an atrocity")
  ok(not groups.earlyVengeance(g, knight.index, c, 0), "a Knight does not")

  -- a whole computer turn in each level keeps the game sound, and a save
  -- keeps the AI's data, groups and all
  local saveMod = require("warlords.save")
  local side = game.begin(g)
  while side and g.turn <= 12 do
    if side.computer then aiMod.playTurn(g, side) end
    side = game.endTurn(g)
  end
  local grp = warlord.ai.groups[1]
  grp.active, grp.target, grp.rally = 2, 1, warlord.capital.index
  grp.staged[2] = g.armies[#g.armies]
  local g2 = saveMod.decode(saveMod.encode(g), DATA)
  local w2 = g2.map.sides[warlord.index + 1].ai
  eq(w2.maxGroups, warlord.ai.maxGroups, "the AI data survives a save")
  eq(w2.groups[1].rally, warlord.capital.index, "and its groups")
  eq(w2.groups[1].staged[2], g2.armies[#g2.armies], "with their staged armies")
  eq(g2.map.cities[5].claim, g.map.cities[5].claim, "and the cities' claims")
end

------------------------------------------------------------- going to sea

-- Going to sea and coming ashore (1a8b:04c8): only a land move changes it,
-- a flier never goes to sea, and a hero flying with a flier does not either.
local function testSea()
  print("going to sea")
  local move = require("warlords.move")
  local scn = require("warlords.scn")
  local g = game.new(DATA, "ERYTHEA", { seed = 81 })

  -- a land tile beside open water, and that water
  local lx, ly, wx, wy
  for y = 1, g.map.height - 2 do
    for x = 1, g.map.width - 2 do
      local t2 = scn.terrainAt(g.map, x + 1, y)
      if not lx and scn.terrainAt(g.map, x, y) == move.PLAIN
         and (t2 == move.WATER or t2 == move.SHORE)
         and #game.armiesAt(g, x, y) == 0 and #game.armiesAt(g, x + 1, y) == 0 then
        lx, ly, wx, wy = x, y, x + 1, y
      end
    end
  end
  ok(lx ~= nil, "Erythea has a plain beside the sea")

  local dragon, hero
  for _, t in ipairs(g.types.list) do
    if t.flies and t.name:find("Dragon") then dragon = t end
  end
  local function army(typeId)
    local a = { type = typeId, owner = 0, x = lx, y = ly, strength = 5,
                maxMoves = 20, moves = 20, upkeep = 0 }
    g.armies[#g.armies + 1] = a
    return a
  end
  hero = army(armytype.HERO)
  local d = army(dragon.id)
  local stack = { hero, d }
  eq(move.stackMode(g, stack), move.FLYING, "a hero with a dragon flies")
  move.walk(g, stack, { { x = wx, y = wy, cost = 2 } })
  eq(hero.x, wx, "and flies out over the water")
  ok(not d.atSea and not hero.atSea, "without either going to sea")
  eq(move.stackMode(g, stack), move.FLYING, "so it still flies")
  move.walk(g, stack, { { x = lx, y = ly, cost = 2 } })
  eq(hero.x, lx, "and can come back to land")

  -- a land stack puts to sea on water and comes ashore on land
  local foot = army(1)
  move.settleSea(g, { foot }, wx, wy, move.LAND, false)
  ok(foot.atSea, "a land army ending on water is at sea")
  move.settleSea(g, { foot }, lx, ly, move.LAND, true)
  ok(not foot.atSea, "and ashore again on land")
  -- a flier walking with a land army stays a flier; the hero goes to sea
  local walkers = { hero, d, foot }
  move.settleSea(g, walkers, wx, wy, move.LAND, false)
  ok(not d.atSea, "a flier never goes to sea")
  ok(hero.atSea and foot.atSea, "the hero and the footman with it do")
  -- a boat move changes nothing
  hero.atSea, foot.atSea = false, false
  move.settleSea(g, { foot }, wx, wy, move.BOAT, false)
  ok(not foot.atSea, "a boat move leaves the flag as it was")

  -- an older save's flier at sea is put right
  local saveMod = require("warlords.save")
  hero.x, hero.y, d.x, d.y = wx, wy, wx, wy
  hero.atSea, d.atSea = true, true
  for i = #g.armies, 1, -1 do if g.armies[i] == foot then table.remove(g.armies, i) end end
  local g2 = saveMod.decode(saveMod.encode(g), DATA)
  for _, a in ipairs(g2.armies) do
    if a.x == wx and a.y == wy then ok(not a.atSea, "a loaded flier and its hero are not at sea") end
  end
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
testBuyProduction("ERYTHEA")
testReports("ERYTHEA")
testHeroItems("ERYTHEA")

-- The music and the advisor: docs/re/sound.md. The synthesis itself is only
-- run for a moment -- plain Lua is slow at it -- but the driver's decisions
-- are what is checked.
local function testSound()
  print("sound")
  local xmi = require("warlords.xmi")
  local ailfm = require("warlords.ailfm")
  local cues = require("warlords.cues")
  local uidata = require("warlords.uidata")
  local function read(p) local f = assert(io.open(p, "rb")) local s = f:read("*a") f:close() return s end

  local seq = xmi.parse(read(DATA .. "/SOUND/SINT0.XMI"))
  eq(seq.length, 17730, "INT0 runs 17730 intervals of 120 Hz")
  local ons, offs, sorted = 0, 0, true
  for i = 1, seq.n do
    local hi = seq.st[i] - seq.st[i] % 16
    if hi == 0x90 then ons = ons + 1 elseif hi == 0x80 then offs = offs + 1 end
    if i > 1 and seq.t[i] < seq.t[i - 1] then sorted = false end
  end
  ok(ons > 0 and ons == offs, "every XMIDI note gets its note-off from its duration")
  ok(sorted, "the events come out in time order")

  local drv = ailfm.new(read(DATA .. "/ADLIB.ADV"), read(DATA .. "/MIDPAK.AD"))
  local lib = drv.lib
  ok(lib[0] and lib[0].len == 14, "MIDPAK.AD holds 14-byte OPL2 timbres")
  eq(drv.T.velocity[0], 82, "the velocity curve starts at 82 (ADLIB.ADV 0x52b)")
  eq(drv.T.velocity[15], 127, "and ends at 127")

  -- the driver's octave: MIDI 69 sounds at 220 Hz, an octave under the
  -- General MIDI pitch, and so does everything else
  drv:message(0xc1, 0, 0)
  drv:message(0x91, 69, 100)
  local s = drv.slots[0]
  ok(s.active and s.voice == 1, "a note takes the voice after the last one used")
  local fnum = drv.fw % 1024
  local hz = fnum * ailfm.RATE / 2 ^ (20 - drv.block)
  ok(math.abs(hz - 220) < 2, ("MIDI 69 sounds at 220 Hz (%.1f)"):format(hz))
  -- velocity 100 is 118 on the curve: the carrier's level (63 - TL) is
  -- scaled by it, over 127
  local level = 63 - lib[0][9] % 64
  eq(drv.chip.slots[4].tl, 63 - math.floor(level * 118 / 127), "velocity scales the carrier's level")
  drv:message(0x81, 69, 0)
  ok(not s.active and not drv.owner[1], "a note-off frees the slot and the voice")

  -- a drum plays at the pitch its timbre names, whatever the key
  drv:message(0x99, 36, 100)
  local d = drv.slots[0]
  eq(d.note, lib[127 * 128 + 36][1], "a drum's pitch is its timbre's transpose byte")
  drv:message(0x89, 36, 0)

  -- ten notes on one channel: nine voices, and the tenth takes one over
  for k = 60, 69 do drv:message(0x92, k, 100) end
  local sounding, waiting = 0, 0
  for i = 0, 15 do
    local sl = drv.slots[i]
    if sl.active then
      if sl.voice then sounding = sounding + 1 else waiting = waiting + 1 end
    end
  end
  eq(sounding, 9, "nine OPL2 voices sound at once")
  eq(waiting, 0, "and the tenth note cuts another off rather than waiting")
  local buf = {}
  drv:render(buf, 500)
  local loud = false
  for i = 1, 500 do if buf[i] ~= 0 then loud = true break end end
  ok(loud, "the chip makes a sound")

  -- what plays when
  local files = uidata.strings(DATA .. "/DATA/FILE.DAT")
  local first = function() return 1 end
  local name, loop = cues.song(files, cues.TITLE, {}, first)
  eq(name, "STARTUP.XMI", "the start screens play STARTUP.XMI")
  ok(loop, "and it starts again when it ends")
  name, loop = cues.song(files, cues.COMPUTER, { { computer = false, alive = true } }, first)
  eq(name, "INT2.XMI", "a computer's turn beside a human plays FILE.DAT group 11")
  ok(not loop, "once")
  eq(cues.song(files, cues.BEGIN, {}, first), "INT22.XMI", "the war begins to INT22")

  -- the advisor's marks
  local g = { turn = 1, map = { cities = {} }, armies = {} }
  local side = { index = 0, gold = 500 }
  for i = 1, 6 do g.map.cities[i] = { ownerIndex = 0 } end
  eq(cues.advisor(g, side, first), nil, "the advisor never speaks on turn 1")
  eq(side.advisor.mark, 5, "but he has marked six cities down as five")
  g.turn = 2
  eq(cues.advisor(g, side, first), nil, "nothing new, not a seventh turn: silence")
  for i = 1, 5 do g.map.cities[i].ownerIndex = 1 end
  eq(cues.advisor(g, side, first), 40, "down to one city: 'Thy sorry efforts...' (VLOSE05)")
  eq(side.advisor.dir, 2, "and he remembers saying so")
  for i = 1, 5 do g.map.cities[i].ownerIndex = 0 end
  eq(cues.advisor(g, side, first), 47, "back to six: 'Thou art doing well... so far!'")
  g.turn = 7
  side.gold = 50
  eq(cues.advisor(g, side, first), 54, "on a seventh turn, short of gold: VGOLD00")
end

testSage("ERYTHEA")
testSetup()
testHistory("ERYTHEA")
testDisband("ERYTHEA")
testMovement("ERYTHEA")
testMovement("ISLADIA")
testStackLimit("ERYTHEA")
testSea()
testCombat("ERYTHEA")
testCapture("ERYTHEA")
testCityChoices("ERYTHEA")
testHeroes("ERYTHEA")
testHeroExperienceBug()
testDiplomacy()
testQuests()
testEndGame()
testHiddenMap()
testSave()
testSites("ERYTHEA")
testSites("DRAGON")
testSlots()
testScreenLayout()
testCityCastles()
testComputerPlayers()
testAIGame("TUTORIA", 30)
testAIGame("ERYTHEA", 25)
if exists(DATA .. "/TUTORIA/TUTORIA.SCN") then testTutorialHero() end
testSound()

print(("\n%d passed, %d failed"):format(passed, failed))
os.exit(failed == 0 and 0 or 1)
