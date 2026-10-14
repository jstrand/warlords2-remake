-- Combat: building the two lines, the modifiers, and the fight itself.
--
-- docs/rules.md > Combat. `combat_setup` (Ghidra 6a89:008b) builds the lines,
-- `combat_terrain_class` (6a89:0000) classifies the tile and `combat_resolve`
-- (67cc:08a6) fights. Where the Deluxe manual and the code disagree the code
-- wins; those places are marked.

local armytype = require("warlords.armytype")
local rules    = require("warlords.rules")
local scn      = require("warlords.scn")
local move     = require("warlords.move")

local combat = {}

-- terrain classes, in the order the ARMYTYPE bonus fields are stored
combat.CITY, combat.OPEN, combat.WOODS, combat.HILLS = 0, 1, 2, 3
combat.CLASS_NAMES = { [0] = "city", "open", "woods", "hills" }

-- ARMYTYPE offsets
local BONUS_SELF, BONUS_STACK = 32, 40      -- + 2*class
local SUBTRACT, ABILITY = 50, 52
combat.SIEGE, combat.NEGATE_HERO, combat.NEGATE_OTHER = 1, 2, 3

-- hero strength -> command bonus, indexed 0..9
combat.HERO_TABLE = { [0] = 0, 0, 0, 0, 1, 1, 1, 2, 2, 3 }

combat.HIT_POINTS = 2
combat.MAX_STRENGTH = 15
combat.DIE, combat.DIE_INTENSE = 20, 24
combat.STALEMATE = 10000                    -- throws in one duel before the attacker loses it

--------------------------------------------------------------------- the tile

--- The battle tile's terrain class. combat_terrain_class, Ghidra 6a89:0000.
function combat.terrainClass(g, x, y)
  if g.towerAt and g.towerAt[y * g.map.width + x] then return combat.CITY end
  local t = scn.terrainAt(g.map, x, y)
  if t == move.CITY or t == move.SITE then return combat.CITY end
  if t == move.FOREST then return combat.WOODS end
  if t == move.HILLS or t == move.MOUNTAINS then return combat.HILLS end
  return combat.OPEN
end

--------------------------------------------------------------------- the lines

local function fightOrder(g, ownerIndex)
  -- .SCN 0x60b: 29 bytes per player, row 8 for neutral (FUN_1b62_0024)
  local row = ownerIndex or 8
  return function(a) return g.map.fightOrder[row][a.type] or 0 end
end

--- On water, shore and mountains a side holding a hero sends one flier to
--- the back, keeping the hero's carrier alive longest: the first flier in
--- the order the line was gathered that is not at sea gets 80 on its rank
--- (6a89:008b, before the sort). Returns the army, or nil.
local function heroCarrier(g, line, x, y)
  local t = scn.terrainAt(g.map, x, y)
  if t ~= move.WATER and t ~= move.SHORE and t ~= move.MOUNTAINS then return nil end
  local hasHero = false
  for _, a in ipairs(line) do if a.type == armytype.HERO then hasHero = true end end
  if not hasHero then return nil end
  for _, a in ipairs(line) do
    if not a.atSea and g.types.byId[a.type].flies then return a end
  end
  return nil
end

--- Sort a line by its owner's fight order, lowest first, ties keeping the
--- order it was gathered in (an insertion sort, 6a89:05f1).
local function sortLine(g, line, ownerIndex, carrier)
  local rank = fightOrder(g, ownerIndex)
  local key, order = {}, {}
  for i, a in ipairs(line) do
    key[a] = rank(a) + (a == carrier and 80 or 0)
    order[a] = i
  end
  table.sort(line, function(p, q)
    if key[p] ~= key[q] then return key[p] < key[q] end
    return order[p] < order[q]
  end)
  return line
end

--- Build both battle lines for an attack on (x, y).
-- Defenders are every army on the tile that the attacker does not own -- or on
-- any of the 2x2 tiles when the target is a city -- gathered from the top of
-- the army table down (combat_setup, 6a89:008b).
function combat.lines(g, stack, x, y)
  local attacker = stack[1] and stack[1].owner
  -- only a standing city is fought for as one (its terrain is 10, which
  -- combat_setup and after_battle test); ruins are open ground
  local city = g.map.cityTile[y * g.map.width + x]
  if city and city.razed then city = nil end

  local x0, y0 = city and city.x or x, city and city.y or y
  local x1, y1 = city and city.x + 1 or x, city and city.y + 1 or y

  local defenders, defOwner = {}, nil
  for i = #g.armies, 1, -1 do
    local a = g.armies[i]
    if not a.transit and a.owner ~= attacker and a.x and a.x >= x0 and a.x <= x1
       and a.y >= y0 and a.y <= y1 then
      defenders[#defenders + 1] = a
      if defOwner == nil then defOwner = a.owner end
    end
  end

  local attackers = {}
  for _, a in ipairs(stack) do attackers[#attackers + 1] = a end

  sortLine(g, attackers, attacker, heroCarrier(g, attackers, x, y))
  sortLine(g, defenders, defOwner, heroCarrier(g, defenders, x, y))
  return attackers, defenders, defOwner, city
end

------------------------------------------------------------------- modifiers

local function itemTotal(army, itemType)
  local total = 0
  if army.items then
    for _, it in ipairs(army.items) do
      if it.type == itemType then total = total + (it.value or 0) end
    end
  end
  return total
end

--- Battle items (+n in battle) carried by one army. Heroes only in practice.
function combat.battleItems(army)
  return itemTotal(army, rules.ITEM_BATTLE)
end

--- Command items (+n to command) carried by one army, a standard counting 1
--- (6a89:0d71).
function combat.commandItems(army)
  local n = itemTotal(army, rules.ITEM_COMMAND)
  for _, it in ipairs(army.items or {}) do
    if it.type == rules.ITEM_STANDARD then n = n + 1 end
  end
  return n
end

--- The side's hero bonus: the table lookup on its strongest hero, plus every
--- command item it carries. Standards (type 8) count 1 each.
function combat.heroBonus(g, line)
  local best, command = 0, 0
  for _, a in ipairs(line) do
    if a.type == armytype.HERO then
      local str = (a.strength or 0) + combat.battleItems(a)
      best = math.max(best, math.min(9, str))
      command = command + combat.commandItems(a)
    end
  end
  return combat.HERO_TABLE[best] + command
end

local function lineHas(g, line, ability)
  for _, a in ipairs(line) do
    if g.types.byId[a.type].bonus[ABILITY] == ability then return true end
  end
  return false
end

local function stackBonus(g, line, class)
  local best = 0
  for _, a in ipairs(line) do
    best = math.max(best, g.types.byId[a.type].bonus[BONUS_STACK + 2 * class] or 0)
  end
  return best
end

local function subtract(g, line)
  local least = 0
  for _, a in ipairs(line) do
    least = math.min(least, g.types.byId[a.type].bonus[SUBTRACT] or 0)
  end
  return least
end

--- The city's fortification bonus for the defender, or 0.
function combat.fortify(g, attackers, x, y, class)
  if class ~= combat.CITY then return 0 end
  if lineHas(g, attackers, combat.SIEGE) then return 0 end

  local value
  if g.towerAt and g.towerAt[y * g.map.width + x] then
    value = 1
  elseif scn.terrainAt(g.map, x, y) == move.SITE then
    value = 2
  else
    local city = g.map.cityTile[y * g.map.width + x]
    value = city and city.defence or 0
  end
  local city = g.map.cityTile[y * g.map.width + x]
  if city and city.ownerIndex == nil then value = math.floor(value / 2) end   -- neutral
  return value
end

--- Both sides' modifiers. Returns attackMod, defendMod.
function combat.modifiers(g, attackers, defenders, x, y, class)
  local cap = g.map.combatCap or 5
  local atkHero = lineHas(g, defenders, combat.NEGATE_HERO) and 0
                  or combat.heroBonus(g, attackers)
  local atkStack = lineHas(g, defenders, combat.NEGATE_OTHER) and 0
                   or stackBonus(g, attackers, class)
  local defHero = lineHas(g, attackers, combat.NEGATE_HERO) and 0
                  or combat.heroBonus(g, defenders)
  local defStack = lineHas(g, attackers, combat.NEGATE_OTHER) and 0
                   or stackBonus(g, defenders, class)

  local attackMod = math.min(cap, atkHero + atkStack) + subtract(g, defenders)
  local defendMod = math.min(cap, defHero + defStack
                                  + combat.fortify(g, attackers, x, y, class))
                    + subtract(g, attackers)
  return attackMod, defendMod
end

--- One army's fighting strength.
-- A boat at sea on water or shore is exactly 4, whatever else applies: the
-- manual's "4 or natural strength, whichever is lower" is not what the code does.
function combat.strength(g, army, mod, class, terrain)
  if army.atSea and (terrain == move.WATER or terrain == move.SHORE) then return 4 end
  local s = (army.strength or 0) + mod + combat.battleItems(army)
          + (g.types.byId[army.type].bonus[BONUS_SELF + 2 * class] or 0)
  return math.min(combat.MAX_STRENGTH, s)
end

--- What View > Stack shows beside each army of the group that moves: its
--- strength in a fight on (x, y) (89e0:1b9c). The group's bonus is summed
--- first -- every hero's table value and command items, and for each other
--- army what its stack bonus for the terrain adds over the best so far --
--- and capped at the scenario's limit; then each army is its own strength,
--- battle items for a hero or its terrain bonus for anything else, capped at
--- 9 (a boat at sea is 4), with the group's bonus on top. An army at sea adds
--- nothing to the group. Returns the strengths by army.
function combat.stackStrengths(g, armies, x, y)
  local class = combat.terrainClass(g, x, y)
  local total, best = 0, 0
  for _, a in ipairs(armies) do
    if not a.atSea then
      if a.type == armytype.HERO then
        local s = math.min(9, (a.strength or 0) + combat.battleItems(a))
        total = total + combat.HERO_TABLE[s] + combat.commandItems(a)
      else
        local v = g.types.byId[a.type].bonus[BONUS_STACK + 2 * class] or 0
        if v > best then total, best = total + v - best, v end
      end
    end
  end
  total = math.min(total, g.map.combatCap or 5)
  local out = {}
  for _, a in ipairs(armies) do
    local s
    if a.atSea then s = 4
    elseif a.type == armytype.HERO then s = (a.strength or 0) + combat.battleItems(a)
    else s = (a.strength or 0) + (g.types.byId[a.type].bonus[BONUS_SELF + 2 * class] or 0) end
    out[a] = math.min(9, s) + total
  end
  return out
end

--------------------------------------------------------------------- the fight

--- Fight a battle. Returns a result table:
--   { won = bool, log = { 0 = a defender died, 1 = an attacker died, ... },
--     attackers = {survivors}, defenders = {survivors} }
-- The armies themselves are not removed from the game; the caller applies the
-- outcome (see game.resolveAttack).
function combat.resolve(g, attackers, defenders, x, y)
  local class = combat.terrainClass(g, x, y)
  local terrain = scn.terrainAt(g.map, x, y)
  local attackMod, defendMod = combat.modifiers(g, attackers, defenders, x, y, class)
  local die = g.map.options.intenseCombat ~= 0 and combat.DIE_INTENSE or combat.DIE

  local function prepare(line, mod)
    local out = {}
    for _, a in ipairs(line) do
      out[#out + 1] = {
        army = a,
        strength = math.max(1, combat.strength(g, a, mod, class, terrain)),
        hits = combat.HIT_POINTS - 1,     -- dies when this drops below 0
      }
    end
    return out
  end

  local atk, def = prepare(attackers, attackMod), prepare(defenders, defendMod)
  -- an army at sea on water or shore fights as a boat, strength 4
  -- (DS:029c / DS:02a4); Triumphs counts it a navy
  local boats = {}
  for _, line in ipairs({ attackers, defenders }) do
    for _, a in ipairs(line) do
      if a.atSea and (terrain == move.WATER or terrain == move.SHORE) then boats[a] = true end
    end
  end
  local log, ai, di, throws = {}, 1, 1, 0

  while atk[ai] and def[di] do
    local a, d = atk[ai], def[di]
    local ra, rd = g.rng:dice(1, die, 0), g.rng:dice(1, die, 0)
    local aHits = ra <= a.strength and rd > d.strength
    local dHits = rd <= d.strength and ra > a.strength
    -- past 10,000 throws in one duel the attacker takes the hit; the count
    -- starts again with each army that falls (67cc:08a6)
    if throws > combat.STALEMATE then aHits, dHits = false, true end
    throws = throws + 1

    -- Tutorial: a human player's hero cannot die attacking a neutral defender.
    if dHits and g.map.options.tutorial ~= 0
       and a.army.type == armytype.HERO
       and not (g.map.sides[(a.army.owner or 0) + 1] or {}).computer
       and d.army.owner == nil then
      dHits = false
    end

    if aHits then
      d.hits = d.hits - 1
      if d.hits < 0 then
        d.dead = true
        log[#log + 1] = 0
        di = di + 1
        throws = 0
      end
    elseif dHits then
      a.hits = a.hits - 1
      if a.hits < 0 then
        a.dead = true
        log[#log + 1] = 1
        ai = ai + 1
        throws = 0
      end
    end
  end

  local function survivors(line)
    local out = {}
    for _, e in ipairs(line) do if not e.dead then out[#out + 1] = e.army end end
    return out
  end

  local deadAttackers, deadDefenders = {}, {}
  for _, e in ipairs(atk) do if e.dead then deadAttackers[#deadAttackers + 1] = e.army end end
  for _, e in ipairs(def) do if e.dead then deadDefenders[#deadDefenders + 1] = e.army end end

  local deadByArmy = {}
  for _, a in ipairs(deadAttackers) do deadByArmy[a] = true end
  for _, a in ipairs(deadDefenders) do deadByArmy[a] = true end

  return {
    won = #survivors(def) == 0,
    deadByArmy = deadByArmy,
    log = log,
    attackMod = attackMod, defendMod = defendMod, class = class, boats = boats,
    defStrength0 = defenders[1] and combat.strength(g, defenders[1], defendMod, class, terrain) or 0,
    attackers = survivors(atk), defenders = survivors(def),
    deadAttackers = deadAttackers, deadDefenders = deadDefenders,
  }
end

--- The Military Advisor's verdict: 19 simulated battles, the answer is
--- wins / 2 into STRING.DAT group 126. military_advisor, Ghidra 67cc:1f19.
combat.ADVICE = {
  [0] = "complete and utter suicide!",
  "sheerest folly! Thou shouldst not attack!",
  "a foolish decision!",
  "a brave choice! I leave it to thee!",
  "difficult but not impossible to win!",
  "very evenly matched!",
  "a hard-fought victory! But we shall win!",
  "a comfortable victory!",
  "an easy victory! We cannot lose!",
  "as simple as butchering sleeping cattle!",
}
combat.ADVICE_BATTLES = 19

function combat.advise(g, attackers, defenders, x, y)
  local wins = 0
  for _ = 1, combat.ADVICE_BATTLES do
    if combat.resolve(g, attackers, defenders, x, y).won then wins = wins + 1 end
  end
  return combat.ADVICE[math.floor(wins / 2)], wins
end

return combat
