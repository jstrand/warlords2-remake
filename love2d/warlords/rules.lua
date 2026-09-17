-- The rules of Warlords II, as read out of WARLORD2.EXE.
--
-- Every number here has a citation in docs/rules.md; keep the two in step. The
-- functions are pure: they take state and dice, and return values. Turn order
-- and mutation live in game.lua.

local armytype = require("warlords.armytype")

local rules = {}

--- Deliberate faults in the original.
--
-- The remake reproduces the original's bugs by default, so a game can be
-- compared against DOSBox move for move. Set a field to false to play the
-- game as it was evidently meant to work. Each one is documented where it is
-- used and in docs/rules.md.
rules.bugs = {
  -- Post-battle hero experience checks the *attacker's* type array when
  -- deciding whether a surviving defender was a hero (docs/rules.md > Combat).
  heroExperienceReadsAttackerTypes = true,
  -- The AI's diplomacy phase can never escalate to war through its own
  -- grudge and threat tests: both "declare war" branches sit inside the
  -- failing half of the enclosing test (docs/re/ai.md > Diplomacy).
  aiWarEscalationUnreachable = true,
  -- ai_pick_enemy's "enemy cities next to mine" term accumulates into the
  -- side's own slot, which is zeroed before the pick (docs/re/ai.md).
  aiAdjacencyTermSelfScored = true,
}

rules.NEUTRAL = 15          -- the owner byte used for neutral cities
rules.MAX_STACK = 8         -- armies on one tile
rules.MAX_MOVE = 99         -- movement points are capped here at turn start
rules.MOVE_CARRY = 2        -- unused moves carried into the next turn, at most
rules.SEA_MOVES = 20        -- an army at sea gets this instead of its maximum
rules.SEA_MIN_UPKEEP = 4
rules.MAX_HERO_XP = 60

-- Production purposes: weights on (time, strength, move).
-- docs/rules.md > Choosing the best type for a purpose.
rules.PURPOSE = {
  [1] = { time = 10, str = 4,  move = 1 },   -- quick and cheap
  [2] = { time = 10, str = 10, move = 1 },   -- balanced
  [3] = { time = 5,  str = 10, move = 1 },   -- strongest
  [4] = { time = 5,  str = 10, move = 1 },   -- flying types only
  [5] = { time = 5,  str = 10, move = 1 },
  [6] = { time = 10, str = 1,  move = 10 },  -- fastest
}

-- Garrison level (0-3) -> production purpose. DS:0d60.
rules.GARRISON_PURPOSE = { [0] = 1, [1] = 6, [2] = 2, [3] = 3 }

-- Item effects (docs/rules.md > Item types).
rules.ITEM_BATTLE, rules.ITEM_COMMAND = 1, 2
rules.ITEM_FLIGHT, rules.ITEM_DOUBLE_MOVE, rules.ITEM_GOLD_PER_CITY = 5, 6, 7
rules.ITEM_STANDARD = 8

--- A city's defence: 1 below three production types, otherwise 2.
-- city_compute_defence, Ghidra 7087:0930.
function rules.cityDefence(nTypes)
  return nTypes >= 3 and 2 or 1
end

--- Build a city's production slots the way setup_city_production does:
--- ARMYTYPE stats, a random nudge per stat, then sorted by purchase price.
-- The .SCN file's own per-slot values are ignored. docs/rules.md > Production.
function rules.citySlots(produceIds, types, rng)
  local slots = {}
  for _, id in ipairs(produceIds) do
    local a = types.byId[id]
    local strength, time, cost, move = a.strength, a.time, a.cost, a.move

    if rng:chance(10) then
      if rng:chance(60) then strength = math.min(9, strength + 1)
      else strength = math.max(1, strength - 1) end
    end
    if rng:chance(20) then
      local r = rng:dice(1, 100, 0)
      move = move + (r < 10 and 4 or r < 60 and 2 or r < 95 and -2 or -4)
      move = math.max(2, move)
    end
    move = math.max(6, move)
    if rng:chance(10) then
      cost = cost + (rng:chance(60) and -(cost // 4) or (cost // 4))
    end
    if rng:chance(10) then
      time = rng:chance(60) and math.max(1, time - 1) or time + 1
    end

    slots[#slots + 1] = {
      type = id, name = a.name, strength = strength,
      time = time, cost = cost, move = move, price = math.abs(a.price),
    }
  end
  table.sort(slots, function(p, q) return p.price < q.price end)
  return slots
end

--- The slot a city would build for a purpose, or nil.
-- best_production_for, Ghidra 623c:103d. Slots are scanned last to first with
-- a strict >, so ties go to the later slot.
function rules.bestSlot(slots, purpose, types, sideBonus)
  local w = rules.PURPOSE[purpose]
  local best, bestScore = nil, 0
  for i = #slots, 1, -1 do
    local slot = slots[i]
    local a = types.byId[slot.type]
    if purpose ~= 4 or a.flies then
      local str = math.min(9, slot.strength + (sideBonus and 2 or 0))
      if a.siege then str = str + 2 end
      local time = slot.time + ((str < 3 and purpose ~= 6) and 1 or 0)
      local score = (10 - math.min(10, time)) * w.time
                  + str * w.str
                  + (slot.move * w.move) // 2
      if score > bestScore then best, bestScore = slot, score end
    end
  end
  return best
end

--- The garrison level a city starts with. `neutralCities` is the option value.
-- docs/rules.md > Starting garrisons. nil means "no garrison": a placeholder
-- Scouts army of strength 1 stands in the city instead.
function rules.garrisonLevel(owned, neutralCities, rng)
  if owned then return 3 end
  if neutralCities <= 0 then return nil end
  return math.min(3, rng:dice(1, 4, 0) + neutralCities - 2)
end

--- A new army's stats, from the city slot that built it.
-- city_produce_army, Ghidra 6f8c:0dd7.
function rules.armyFromSlot(slot, enhanced)
  local strength = slot.strength
  if enhanced then strength = math.min(9, strength + 2) end
  return {
    type = slot.type, name = slot.name,
    strength = strength,
    maxMoves = slot.move,
    upkeep = slot.cost // 2,
  }
end

return rules
