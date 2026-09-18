-- Game state and the turn loop.
--
-- Headless on purpose: this module never touches love.*, so the rules can be
-- run and asserted from plain Lua (see love2d/test/). Rendering and input sit
-- on top of it.

local armytype = require("warlords.armytype")
local rules    = require("warlords.rules")
local rng      = require("warlords.rng")
local scn      = require("warlords.scn")

local game = {}

local TRANSIT_TURNS = 2       -- a vectored army arrives two turns later
local STANDARD_DEST = -2      -- vector destination meaning "the side's standard"

--------------------------------------------------------------------- helpers

local function key(g, x, y) return y * g.map.width + x end

--- Every army standing on a tile (in transit armies are nowhere).
function game.armiesAt(g, x, y)
  local out = {}
  for _, a in ipairs(g.armies) do
    if not a.transit and a.x == x and a.y == y then out[#out + 1] = a end
  end
  return out
end

--- The city standing on a tile, anywhere in its 2x2 footprint.
function game.cityAt(g, x, y)
  return g.map.cityTile[key(g, x, y)]
end

--- Give a city to a side (nil for neutral) and drop the cached cost grids.
function game.setCityOwner(g, city, sideIndex)
  city.ownerIndex = sideIndex
  require("warlords.move").invalidate(g)
end

function game.sideCities(g, side)
  local out = {}
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex == side.index then out[#out + 1] = c end
  end
  return out
end

function game.sideArmies(g, side)
  local out = {}
  for _, a in ipairs(g.armies) do
    if a.owner == side.index then out[#out + 1] = a end
  end
  return out
end

--- Items carried by a side's heroes, summed by effect type.
function game.itemBonus(g, side, itemType)
  local total = 0
  for _, a in ipairs(g.armies) do
    if a.owner == side.index and a.items then
      for _, it in ipairs(a.items) do
        if it.type == itemType then total = total + math.max(1, it.value) end
      end
    end
  end
  return total
end

--------------------------------------------------------------------- economy

--- docs/rules.md > Start of a side's turn, step 4.
function game.income(g, side)
  local total = 0
  local cities = game.sideCities(g, side)
  for _, c in ipairs(cities) do total = total + c.income end
  local perCity = game.itemBonus(g, side, rules.ITEM_GOLD_PER_CITY)
  return total + #cities * perCity
end

function game.upkeep(g, side)
  local total = 0
  for _, a in ipairs(g.armies) do
    if a.owner == side.index and not a.transit then
      local u = a.upkeep or 0
      if a.atSea then u = math.max(rules.SEA_MIN_UPKEEP, u) end
      total = total + u
    end
  end
  return total
end

--------------------------------------------------------------------- setup

local function placeArmy(g, army)
  g.armies[#g.armies + 1] = army
  return army
end

--- One starting army per city (docs/rules.md > Starting garrisons).
local function setupGarrisons(g)
  for _, c in ipairs(g.map.cities) do
    local owned = c.ownerIndex ~= nil
    local level = rules.garrisonLevel(owned, g.map.options.neutralCities, g.rng)
    local slot
    if level and #c.slots > 0 then
      local side = owned and g.map.sides[c.ownerIndex + 1] or nil
      slot = rules.bestSlot(c.slots, rules.GARRISON_PURPOSE[level], g.types,
                            side and side.enhanced)
    end
    if slot then
      local a = rules.armyFromSlot(slot, owned and g.map.sides[c.ownerIndex + 1].enhanced)
      a.x, a.y, a.owner, a.moves, a.homeCity = c.x, c.y, c.ownerIndex, 0, c.index
      placeArmy(g, a)
    else
      -- Neutral Cities off: a placeholder Scouts army of strength 1.
      local a = g.types.byId[armytype.SCOUTS]
      placeArmy(g, {
        x = c.x, y = c.y, owner = nil, type = armytype.SCOUTS, name = a.name,
        strength = 1, maxMoves = a.move, moves = 0, upkeep = 0, homeCity = c.index,
      })
    end
  end
end

--- Build a fresh game from the original data files.
-- dataDir is the directory holding TERRAIN0/ and the scenario folders.
function game.new(dataDir, scenario, opts)
  opts = opts or {}
  local g = {
    rng = rng.new(opts.seed or 0),
    types = armytype.load(dataDir .. "/TERRAIN0/ARMYTYPE.DAT"),
    map = scn.load(dataDir .. "/" .. scenario, scenario),
    armies = {},
    turn = 1,
    log = {},
  }
  if opts.options then
    for k, v in pairs(opts.options) do g.map.options[k] = v end
  end

  -- sides: the live gold, and who is still in the game
  g.sides = {}
  for _, s in ipairs(g.map.sides) do
    s.alive = s.inUse
    s.ownerIndex = s.index
    if s.inUse then g.sides[#g.sides + 1] = s end
  end

  -- cities: ownership, production slots, defence
  for _, c in ipairs(g.map.cities) do
    c.ownerIndex = c.owner and c.owner.index or nil
    c.slots = rules.citySlots(c.produces, g.types, g.rng)
    c.defence = rules.cityDefence(#c.slots)
    c.producing = nil       -- slot index being built
    c.countdown = 0
    c.vectorTo = nil
  end

  setupGarrisons(g)

  g.current = 1             -- index into g.sides
  g.side = g.sides[1]
  return g
end

--------------------------------------------------------------- turn sequence

local function eliminate(g, side)
  side.alive = false
  g.log[#g.log + 1] = ("%s has been eliminated."):format(side.name)
end

--- Step 4: gold += income - upkeep, never below 0.
local function applyIncome(g, side)
  side.income = game.income(g, side)
  side.upkeepTotal = game.upkeep(g, side)
  side.gold = math.max(0, side.gold + side.income - side.upkeepTotal)
end

--- Place a produced army, or send it back. Returns the army, or nil if it was
--- disbanded. docs/rules.md > Start of a side's turn, step 5.
local function deliver(g, army, city)
  if #game.armiesAt(g, city.x, city.y) >= rules.MAX_STACK
     or city.ownerIndex ~= army.owner then
    if army.returning then
      return nil                      -- already heading home: disbanded
    end
    local home = g.map.cities[army.homeCity + 1]
    army.returning = true
    army.transit = { turns = TRANSIT_TURNS, dest = home.index }
    return army
  end
  army.x, army.y, army.transit, army.returning = city.x, city.y, nil, nil
  army.moves = 0
  return army
end

--- Step 5: run every producing city the side owns.
local function runProduction(g, side)
  for _, c in ipairs(game.sideCities(g, side)) do
    if c.producing then
      c.countdown = c.countdown - 1
      if c.countdown <= 0 then
        if side.gold > 0 then
          local slot = c.slots[c.producing]
          local a = rules.armyFromSlot(slot, side.enhanced)
          a.owner, a.homeCity, a.moves = side.index, c.index, 0
          a.x, a.y = c.x, c.y
          if c.vectorTo and c.vectorTo ~= c.index and c.vectorTo ~= STANDARD_DEST then
            a.transit = { turns = TRANSIT_TURNS, dest = c.vectorTo }
            a.x, a.y = nil, nil
          end
          placeArmy(g, a)
          c.countdown = slot.time       -- the city starts the next one
        else
          c.countdown = 0               -- no gold: it waits, built but unpaid
        end
      end
    end
  end

  -- armies already on the road
  for _, a in ipairs(g.armies) do
    if a.transit and a.owner == side.index then
      a.transit.turns = a.transit.turns - 1
      if a.transit.turns <= 0 then
        local dest = g.map.cities[a.transit.dest + 1]
        a.transit = nil
        if not deliver(g, a, dest) then a.disbanded = true end
      end
    end
  end

  for i = #g.armies, 1, -1 do
    if g.armies[i].disbanded then table.remove(g.armies, i) end
  end
end

--- Step 6: movement reset.
local function resetMovement(g, side)
  local doubleMove = game.itemBonus(g, side, rules.ITEM_DOUBLE_MOVE) > 0
  for _, a in ipairs(g.armies) do
    if a.owner == side.index and not a.transit then
      local carry = math.min(a.moves or 0, rules.MOVE_CARRY)
      local base = a.atSea and rules.SEA_MOVES or a.maxMoves
      -- A hero with a double-movement item adds each army's maximum again
      -- for every army on its tile.
      if doubleMove and game.heroWithDoubleMoveAt(g, side, a.x, a.y) then
        base = base + a.maxMoves
      end
      a.moves = math.min(rules.MAX_MOVE, base + carry)
    end
  end
end

--- Is one of the side's heroes carrying a double-movement item on this tile?
function game.heroWithDoubleMoveAt(g, side, x, y)
  for _, a in ipairs(g.armies) do
    if a.owner == side.index and not a.transit and a.x == x and a.y == y
       and a.type == armytype.HERO and a.items then
      for _, it in ipairs(a.items) do
        if it.type == rules.ITEM_DOUBLE_MOVE then return true end
      end
    end
  end
  return false
end

--- Run the start of `side`'s turn. Returns false if the side was eliminated.
function game.startTurn(g, side)
  if #game.sideCities(g, side) == 0 then
    eliminate(g, side)
    return false
  end
  applyIncome(g, side)
  runProduction(g, side)
  resetMovement(g, side)
  return true
end

--- Hand the turn to the next living side, starting a new game turn when the
--- list wraps. Returns the side now to play, or nil if the game is over.
function game.endTurn(g)
  for _ = 1, #g.sides do
    g.current = g.current + 1
    if g.current > #g.sides then
      g.current = 1
      g.turn = g.turn + 1
    end
    local side = g.sides[g.current]
    if side.alive then
      g.side = side
      if game.startTurn(g, side) then return side end
    end
  end
  g.side = nil
  return nil
end

--- Start the very first turn of a new game.
function game.begin(g)
  g.current, g.side = 1, g.sides[1]
  if not game.startTurn(g, g.side) then return game.endTurn(g) end
  return g.side
end

--------------------------------------------------------------- city commands

--- Choose what a city builds. `slotIndex` is nil to stop producing.
function game.setProduction(g, city, slotIndex)
  city.producing = slotIndex
  city.countdown = slotIndex and city.slots[slotIndex].time or 0
  if not slotIndex then city.vectorTo = nil end   -- vectoring only sticks while building
end

--- Send what a city builds to another city. Free, and with no range limit.
function game.vector(g, city, destCity)
  city.vectorTo = destCity and destCity.index or nil
end

return game
