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
combat.STALEMATE = 10000                    -- throws before the defender wins

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

--- Sort a line by its owner's fight order, lowest first, ties keeping stack
--- order.
local function sortLine(g, line, ownerIndex)
  local rank = fightOrder(g, ownerIndex)
  local order = {}
  for i, a in ipairs(line) do order[a] = i end
  table.sort(line, function(p, q)
    local rp, rq = rank(p), rank(q)
    if rp ~= rq then return rp < rq end
    return order[p] < order[q]
  end)
  return line
end

--- On water, shore and mountain tiles a side holding a hero sends one
--- qualifying army to the back, keeping the hero's carrier alive longest.
local function protectHeroCarrier(g, line, x, y)
  local t = scn.terrainAt(g.map, x, y)
  if t ~= move.WATER and t ~= move.SHORE and t ~= move.MOUNTAINS then return end
  local hasHero = false
  for _, a in ipairs(line) do if a.type == armytype.HERO then hasHero = true end end
  if not hasHero then return end
  for i, a in ipairs(line) do
    if g.types.byId[a.type].flies then
      table.remove(line, i)
      line[#line + 1] = a
      return
    end
  end
end

--- Build both battle lines for an attack on (x, y).
-- Defenders are every army on the tile that the attacker does not own -- or on
-- any of the 2x2 tiles when the target is a city.
function combat.lines(g, stack, x, y)
  local gameMod = require("warlords.game")
  local attacker = stack[1] and stack[1].owner
  local city = g.map.cityTile[y * g.map.width + x]

  local tiles = { { x, y } }
  if city then
    tiles = {}
    for dx = 0, 1 do
      for dy = 0, 1 do tiles[#tiles + 1] = { city.x + dx, city.y + dy } end
    end
  end

  local defenders, defOwner = {}, nil
  for _, t in ipairs(tiles) do
    for _, a in ipairs(gameMod.armiesAt(g, t[1], t[2])) do
      if a.owner ~= attacker then
        defenders[#defenders + 1] = a
        if defOwner == nil then defOwner = a.owner end
      end
    end
  end

  local attackers = {}
  for _, a in ipairs(stack) do attackers[#attackers + 1] = a end

  sortLine(g, attackers, attacker)
  sortLine(g, defenders, defOwner)
  protectHeroCarrier(g, attackers, x, y)
  protectHeroCarrier(g, defenders, x, y)
  return attackers, defenders, defOwner, city
end

------------------------------------------------------------------- modifiers

local function itemTotal(army, itemType)
  local total = 0
  if army.items then
    for _, it in ipairs(army.items) do
      if it.type == itemType then total = total + math.max(1, it.value) end
    end
  end
  return total
end

--- Battle items (+n in battle) carried by one army. Heroes only in practice.
function combat.battleItems(army)
  return itemTotal(army, rules.ITEM_BATTLE)
end

--- The side's hero bonus: the table lookup on its strongest hero, plus every
--- command item it carries. Standards (type 8) count 1 each.
function combat.heroBonus(g, line)
  local best, command = 0, 0
  for _, a in ipairs(line) do
    if a.type == armytype.HERO then
      local str = (a.strength or 0) + combat.battleItems(a)
      best = math.max(best, math.min(9, str))
      command = command + itemTotal(a, rules.ITEM_COMMAND)
      if a.items then
        for _, it in ipairs(a.items) do
          if it.type == rules.ITEM_STANDARD then command = command + 1 end
        end
      end
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
  if city and city.ownerIndex == nil then value = value // 2 end   -- neutral
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
  local log, ai, di, throws = {}, 1, 1, 0

  while atk[ai] and def[di] do
    local a, d = atk[ai], def[di]
    local ra, rd = g.rng:dice(1, die, 0), g.rng:dice(1, die, 0)
    local aHits = ra <= a.strength and rd > d.strength
    local dHits = rd <= d.strength and ra > a.strength
    throws = throws + 1
    if throws > combat.STALEMATE then aHits, dHits = false, true end

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
      end
    elseif dHits then
      a.hits = a.hits - 1
      if a.hits < 0 then
        a.dead = true
        log[#log + 1] = 1
        ai = ai + 1
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
    attackMod = attackMod, defendMod = defendMod, class = class,
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
  return combat.ADVICE[wins // 2], wins
end

return combat
