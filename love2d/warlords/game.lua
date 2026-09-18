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

--- A tile in the city with room for another army: the four tiles of its
--- footprint in order, then -- if `spill` is set -- up to 20 tries on a random
--- tile within one step of it. Returns x, y or nil.
-- FUN_6f8c_0bff, the guard on every army the game places in a city.
function game.freeTileIn(g, city, spill)
  for dx = 0, 1 do
    for dy = 0, 1 do
      local x, y = city.x + dx, city.y + dy
      if x < g.map.width and y < g.map.height
         and #game.armiesAt(g, x, y) < rules.MAX_STACK then
        return x, y
      end
    end
  end
  if not spill then return nil end
  for _ = 1, 20 do
    local x = city.x + g.rng:dice(1, 3, -2)
    local y = city.y + g.rng:dice(1, 3, -2)
    if x >= 0 and y >= 0 and x < g.map.width and y < g.map.height
       and #game.armiesAt(g, x, y) < rules.MAX_STACK then
      return x, y
    end
  end
  return nil
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

  require("warlords.diplomacy").init(g)
  setupGarrisons(g)
  require("warlords.site").setup(g)
  for _, s in ipairs(g.sides) do game.revealStart(g, s) end

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
  local x, y = game.freeTileIn(g, city, true)
  if not x or city.ownerIndex ~= army.owner then
    if army.returning then
      return nil                      -- already heading home: disbanded
    end
    local home = g.map.cities[army.homeCity + 1]
    army.returning = true
    army.transit = { turns = TRANSIT_TURNS, dest = home.index }
    return army
  end
  army.x, army.y, army.transit, army.returning = x, y, nil, nil
  army.moves = 0
  return army
end

--- Step 5: run every producing city the side owns.
local function runProduction(g, side)
  for _, c in ipairs(game.sideCities(g, side)) do
    if c.producing then
      c.countdown = c.countdown - 1
      if c.countdown <= 0 then
        local vectored = c.vectorTo and c.vectorTo ~= c.index
                         and c.vectorTo ~= STANDARD_DEST
        local x, y = game.freeTileIn(g, c, true)
        if side.gold <= 0 then
          c.countdown = 0               -- no gold: it waits, built but unpaid
        elseif not (x or vectored) then
          c.countdown = 0               -- nowhere to stand: it waits too
        else
          local slot = c.slots[c.producing]
          local a = rules.armyFromSlot(slot, side.enhanced)
          a.owner, a.homeCity, a.moves = side.index, c.index, 0
          a.x, a.y = x, y
          if vectored then
            a.transit = { turns = TRANSIT_TURNS, dest = c.vectorTo }
            a.x, a.y = nil, nil
          end
          placeArmy(g, a)
          c.countdown = slot.time       -- the city starts the next one
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
-- The order is the original's (start_of_turn, Ghidra 8cc6:0000).
function game.startTurn(g, side)
  if #game.sideCities(g, side) == 0 then
    eliminate(g, side)
    return false
  end
  local heroMod = require("warlords.hero")
  for _, m in ipairs(require("warlords.diplomacy").apply(g, side)) do
    g.log[#g.log + 1] = m
  end
  side.heroOffer = heroMod.offer(g, side)
  heroMod.checkPromotions(g, side)
  local questResult = require("warlords.quest").event(g, side, "turn")
  if questResult and questResult.failed then
    g.log[#g.log + 1] = ("Quest abandoned: %s."):format(questResult.failed)
  end
  applyIncome(g, side)
  runProduction(g, side)
  resetMovement(g, side)
  return true
end

--- Hand the turn to the next living side, starting a new game turn when the
--- list wraps. Returns the side now to play, or nil if the game is over.
function game.endTurn(g)
  local ending = game.checkEnd(g)
  if ending.message then g.log[#g.log + 1] = ending.message end
  if ending.over then
    g.side = nil
    return nil
  end
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

--------------------------------------------------------------------- combat

local function removeArmies(g, dead)
  local gone = {}
  for _, a in ipairs(dead) do gone[a] = true end
  for i = #g.armies, 1, -1 do
    if gone[g.armies[i]] then table.remove(g.armies, i) end
  end
end

--- Gold taken with a city won from another side. Neutral cities pay nothing.
-- docs/rules.md > Capturing a city (67cc:0a6b).
function game.loot(g, loser)
  if not loser then return 0 end
  local n = #game.sideCities(g, loser)
  local share = n > 1 and (loser.gold // n) or loser.gold
  return share // 2
end

--- Fight for a tile and apply the outcome: the dead are removed, and a city
--- whose last defender falls changes hands. Returns the combat result with
--- `captured` set to the city, if any.
function game.resolveAttack(g, stack, x, y)
  local combat = require("warlords.combat")
  local move = require("warlords.move")
  local heroMod = require("warlords.hero")
  local attackers, defenders, defOwner, city = combat.lines(g, stack, x, y)
  local result = combat.resolve(g, attackers, defenders, x, y)

  -- a dead hero drops what it carried where it fell
  for _, a in ipairs(result.deadAttackers) do
    if a.type == armytype.HERO then heroMod.dropItems(g, a, a.x, a.y) end
  end
  for _, d in ipairs(result.deadDefenders) do
    if d.type == armytype.HERO then heroMod.dropItems(g, d, x, y) end
  end
  heroMod.battleExperience(g, attackers, defenders, result, city ~= nil)
  local questMod = require("warlords.quest")
  local mySide = g.map.sides[(stack[1] and stack[1].owner or 0) + 1]
  result.quest = questMod.event(g, mySide, "battle",
                                { stack = attackers, killed = result.deadDefenders })

  removeArmies(g, result.deadAttackers)
  removeArmies(g, result.deadDefenders)

  if result.won and city then
    local winner = g.map.sides[stack[1].owner + 1]
    local loser = defOwner and g.map.sides[defOwner + 1] or nil
    if loser then
      local loot = game.loot(g, loser)
      winner.gold = winner.gold + loot
      loser.gold = math.max(0, loser.gold - 2 * loot)
      result.loot = loot
    end
    city.previousOwner = (city.ownerIndex == winner.index) and rules.NEUTRAL or city.ownerIndex
    city.producing, city.countdown, city.vectorTo = nil, 0, nil
    city.ownerIndex = winner.index
    move.invalidate(g)
    result.captured = city
    result.quest = questMod.event(g, winner, "occupy",
                                  { city = city, stack = result.attackers })
                   or result.quest
  end

  -- The survivors walk into the tile they just cleared -- but only as many as
  -- fit. The rest stay where they are, as they would if the walk had been
  -- blocked (docs/rules.md > Moving a stack).
  if result.won then
    local room = rules.MAX_STACK - #game.armiesAt(g, x, y)
    for _, a in ipairs(result.attackers) do
      if room <= 0 then break end
      a.x, a.y, room = x, y, room - 1
    end
  end
  return result
end

----------------------------------------------------- what to do with a city

-- A production type's "value" is half its purchase price (ARMYTYPE +30).
local function slotValue(g, slot)
  return math.abs(g.types.byId[slot.type].price) // 2
end

local function recompute(g, city)
  city.defence = rules.cityDefence(#city.slots)
  if city.producing and city.producing > #city.slots then
    city.producing, city.countdown = nil, 0
  end
end

--- Raise a side's **diplomatic score**, the u16 at .SCN 0x10e3 + 2*side that
--- the diplomatic rating reads. Both atrocities and peace overtures raise it;
--- see diplomacy.addScore. docs/rules.md > Diplomacy.
function game.addDiploScore(g, side, n)
  side.diploScore = (side.diploScore or 0) + n
end

--- Strip the most expensive production type for gold. docs/rules.md >
--- Capturing a city.
function game.pillage(g, side, city, stack)
  if #city.slots < 1 then return 0 end
  local slot = table.remove(city.slots)          -- slots are sorted cheapest first
  local gold = slotValue(g, slot)
  side.gold = side.gold + gold
  game.addDiploScore(g, side, g.rng:dice(1, 5, 0))
  recompute(g, city)
  require("warlords.quest").event(g, side, "pillage",
                                  { city = city, gold = gold, stack = stack or {} })
  return gold
end

--- Strip every production type but the cheapest.
function game.sack(g, side, city, stack)
  if #city.slots < 2 then return 0 end
  local gold = 0
  while #city.slots > 1 do
    gold = gold + slotValue(g, table.remove(city.slots))
  end
  side.gold = side.gold + gold
  game.addDiploScore(g, side, g.rng:dice(1, 10, 5))
  recompute(g, city)
  require("warlords.quest").event(g, side, "pillage",
                                  { city = city, gold = gold, stack = stack or {} })
  return gold
end

--- Burn the city to the ground: it becomes neutral ruins and produces nothing.
function game.raze(g, side, city, stack)
  local move = require("warlords.move")
  city.razed = true
  city.ownerIndex = nil
  city.previousOwner = rules.NEUTRAL
  city.slots = {}
  city.producing, city.countdown, city.vectorTo = nil, 0, nil
  city.defence = 0
  city.income = 0
  for _, c in ipairs(g.map.cities) do
    if c.vectorTo == city.index then c.vectorTo = nil end
  end
  game.addDiploScore(g, side, g.rng:dice(1, 15, 10))
  move.invalidate(g)
  require("warlords.quest").event(g, side, "raze", { city = city, stack = stack or {} })
end

--------------------------------------------------------------- the hidden map

-- Tiles a side has seen. The original keeps one bit per tile for the human
-- player (flag 0x20 of the overlay byte) and gives its computer players a
-- per-city "not seen yet" flag instead; the engine keeps a mask per side,
-- which comes to the same thing for one human and works for hotseat too.

--- Has this side seen the tile? Always true with Hidden Map off.
function game.seen(g, side, x, y)
  if g.map.options.hiddenMap == 0 then return true end
  local mask = g.explored and g.explored[side]
  if not mask then return false end
  return mask[y * g.map.width + x] or false
end

--- Uncover the tiles around (x, y). The square is 2 tiles out for a flying
--- stack or one standing on a city, 1 otherwise. FUN_8611_1298.
function game.reveal(g, side, x, y, flying)
  if g.map.options.hiddenMap == 0 then return 0 end
  g.explored = g.explored or {}
  g.explored[side] = g.explored[side] or {}
  local mask = g.explored[side]

  local onCity = g.map.cityTile[y * g.map.width + x] ~= nil
  local r = (flying or onCity) and 2 or 1
  local found = 0
  for ty = y - r, y + r do
    for tx = x - r, x + r do
      if tx >= 0 and ty >= 0 and tx < g.map.width and ty < g.map.height then
        local k = ty * g.map.width + tx
        if not mask[k] then
          mask[k] = true
          found = found + 1
        end
      end
    end
  end
  return found
end

--- Uncover everything a side can already see: its cities and its armies.
function game.revealStart(g, side)
  if g.map.options.hiddenMap == 0 then return end
  for _, c in ipairs(game.sideCities(g, side)) do
    game.reveal(g, side.index, c.x, c.y, false)
  end
  for _, a in ipairs(game.sideArmies(g, side)) do
    if not a.transit then game.reveal(g, side.index, a.x, a.y, false) end
  end
end

------------------------------------------------------------- end of the game

--- Is the game over, or nearly? Returns a table describing the position:
--   { over = bool, winner = side or nil, message = "..." }
-- end_game_check, Ghidra 8065:1aed. Sets g.won and g.surrenderOffered, the
-- two flags the original keeps at .SCN 0x15b and 0x15d.
function game.checkEnd(g)
  local humans, computers, alive = {}, {}, {}
  for _, s in ipairs(g.sides) do
    if s.alive and #game.sideCities(g, s) > 0 then
      alive[#alive + 1] = s
      if s.computer then computers[#computers + 1] = s else humans[#humans + 1] = s end
    end
  end

  local standing = 0
  for _, c in ipairs(g.map.cities) do
    if not c.razed then standing = standing + 1 end
  end

  if #alive == 0 then
    g.over = true
    return { over = true, message = "Alas! No more players are left!" }
  end

  if #humans == 0 and #computers == 1 then
    g.won, g.over = true, true
    computers[1].computer = false          -- so the finished game can be looked at
    return { over = true, winner = computers[1],
             message = ("%s has triumphed!"):format(computers[1].name) }
  end

  if #humans == 1 and #computers == 0 then
    local mine = #game.sideCities(g, humans[1])
    if mine * 2 > standing then
      g.won, g.over = true, true
      return { over = true, winner = humans[1],
               message = ("%s rules the world!"):format(humans[1].name) }
    end
  end

  if #humans == 1 and #computers > 0 then
    local mine = #game.sideCities(g, humans[1])
    local biggest = 0
    for _, s in ipairs(computers) do
      biggest = math.max(biggest, #game.sideCities(g, s))
    end
    if mine * 2 > standing and mine > biggest + standing // 8 then
      g.surrenderOffered = true
      return { over = false, surrender = true,
               message = "Your enemies offer their surrender!" }
    end
  end

  return { over = false }
end

--------------------------------------------------------------------- sites

--- Search whatever the stack is standing on, if anything. Returns the result
--- table from site.search, or nil.
function game.searchHere(g, stack)
  if #stack == 0 then return nil end
  return require("warlords.site").search(g, stack, stack[1].x, stack[1].y)
end

--- A line of prose for a search result, for the interface to show.
function game.describeSearch(r)
  if not r then return nil end
  local name = r.site.name
  if r.kind == "temple" then
    return r.blessed > 0
      and ("%s blesses %d of your armies."):format(name, r.blessed)
      or ("%s has blessed them already."):format(name)
  elseif r.kind == "no hero" then
    return ("%s can only be searched by a hero."):format(name)
  elseif r.kind == "killed" then
    return ("Your hero is slain in %s by a %s!"):format(name, r.monster and r.monster.name or "guardian")
  elseif r.kind == "gold" then
    return ("%s yields %d gold!"):format(name, r.gold)
  elseif r.kind == "item" then
    return ("Your hero finds the %s in %s!"):format(r.item and r.item.name or "treasure", name)
  elseif r.kind == "allies" then
    return ("%d %s join you at %s!"):format(#r.armies, r.type.name, name)
  elseif r.kind == "sage" then
    return ("A sage dwells in %s."):format(name)
  end
  return ("%s holds nothing."):format(name)
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
