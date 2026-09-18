-- Headless tests for the rules core. No LOVE, no graphics.
--
--     lua love2d/test/run.lua                 -- from the repository root
--     lua love2d/test/run.lua /path/to/data   -- point at your own original/
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
  eq(verdict, combat.ADVICE[wins // 2], "the verdict is wins/2 into the table")
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

  local before = #game.sideCities(g, side)
  local r = game.resolveAttack(g, stack, target.x, target.y)
  ok(r.won, "the overwhelming stack took the city")
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
    eq(game.loot(g, victim), (400 // 2) // 2, "two cities: half the per-city share")
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
    local want = math.abs(g.types.byId[dear.type].price) // 2
    local gold0, atrocity0 = side.gold, side.atrocity or 0
    local got = game.pillage(g, side, city)
    eq(got, want, "pillage pays half the type's purchase price")
    eq(side.gold, gold0 + want, "the gold was paid")
    eq(#city.slots, n - 1, "pillage removed one type")
    eq(city.defence, rules.cityDefence(#city.slots), "defence was recomputed")
    local d = (side.atrocity or 0) - atrocity0
    ok(d >= 1 and d <= 5, "pillage costs 1d5 atrocity: " .. d)
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
      want = want + math.abs(g.types.byId[city2.slots[i].type].price) // 2
    end
    local gold0, atrocity0 = side.gold, side.atrocity or 0
    local got = game.sack(g, side, city2)
    eq(got, want, "sack pays for every type it strips")
    eq(side.gold, gold0 + want, "the gold was paid")
    eq(#city2.slots, 1, "only the cheapest type is left")
    eq(city2.slots[1], cheapest, "and it is the cheapest one")
    eq(city2.defence, 1, "one type means defence 1")
    local d = (side.atrocity or 0) - atrocity0
    ok(d >= 6 and d <= 15, "sack costs 1d10+5 atrocity: " .. d)
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
    local atrocity0 = side.atrocity or 0
    game.raze(g, side, city3)
    eq(city3.ownerIndex, nil, "a razed city is neutral")
    eq(#city3.slots, 0, "a razed city produces nothing")
    eq(city3.income, 0, "a razed city earns nothing")
    eq(#game.sideCities(g, side), before - 1, "the side no longer owns it")
    if other then eq(other.vectorTo, nil, "vectoring to it was cancelled") end
    local d = (side.atrocity or 0) - atrocity0
    ok(d >= 11 and d <= 25, "raze costs 1d15+10 atrocity: " .. d)
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
  local h, allies = heroMod.recruit(g, side, offer)
  eq(h.type, armytype.HERO, "the recruit is a hero")
  eq(h.strength, 5, "a new hero has strength 5")
  eq(h.maxMoves, 14, "a new hero has 14 movement")
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
testMovement("ERYTHEA")
testMovement("ISLADIA")
testStackLimit("ERYTHEA")
testCombat("ERYTHEA")
testCapture("ERYTHEA")
testCityChoices("ERYTHEA")
testHeroes("ERYTHEA")
testHeroExperienceBug()
if exists(DATA .. "/TUTORIA/TUTORIA.SCN") then testTutorialHero() end

print(("\n%d passed, %d failed"):format(passed, failed))
os.exit(failed == 0 and 0 or 1)
