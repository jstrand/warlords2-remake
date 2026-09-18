-- Heroes: offers, recruitment, experience, promotion and death.
--
-- docs/rules.md > Heroes. This is where the first of the original's bugs
-- shows up: see hero.battleExperience and rules.bugs.

local armytype = require("warlords.armytype")
local rules    = require("warlords.rules")
local scn      = require("warlords.scn")
local move     = require("warlords.move")

local hero = {}

hero.MAX_IN_GAME = 40          -- heroes in the whole game
hero.MAX_PER_SIDE = 5          -- 6 once the side owns this many cities
hero.MAX_PER_BIG_SIDE, hero.BIG_SIDE_CITIES = 6, 40
hero.START_STRENGTH, hero.START_MOVES = 5, 14
hero.MAX_STRENGTH = 9
hero.PROMOTION_MOVES = 2

-- experience needed for each promotion, and what the levels are called
hero.LEVELS = { "Hero", "Cavalier", "Champion", "Paladin" }
hero.PROMOTION_AT = { 15, 30, 60 }

--------------------------------------------------------------------- offers

local function countHeroes(g, side)
  local all, mine = 0, 0
  for _, a in ipairs(g.armies) do
    if a.type == armytype.HERO then
      all = all + 1
      if side and a.owner == side.index then mine = mine + 1 end
    end
  end
  return all, mine
end

--- Does a hero offer itself to `side` this turn, and at what price?
-- Returns nil when none does. hero_offer_check, Ghidra 7563:0000.
function hero.offer(g, side)
  local gameMod = require("warlords.game")
  if g.turn == 1 then
    return { price = 0, city = side.capital, first = true }
  end

  local all, mine = countHeroes(g, side)
  if all >= hero.MAX_IN_GAME then return nil end
  local cities = gameMod.sideCities(g, side)
  local limit = #cities >= hero.BIG_SIDE_CITIES and hero.MAX_PER_BIG_SIDE
                                                 or hero.MAX_PER_SIDE
  if mine >= limit then return nil end

  local price = mine == 0 and g.rng:dice(1, 400, 300) or g.rng:dice(1, 600, 1000)
  if price > side.gold then return nil end
  if g.rng:dice(1, 30, 0) >= 7 then return nil end          -- a 20% chance

  local city = g.rng:pick(cities)
  if not city then return nil end
  return { price = price, city = city }
end

--- The allies a hired hero brings: 1-3 of one random magical type.
-- hero_brings_allies (7563:01fc) has a "do allies come?" flag, but it is
-- hard-coded to 1, so they always do.
function hero.allies(g)
  local magical = {}
  for _, a in ipairs(g.types.list) do
    if (a.bonus[48] or 0) ~= 0 then magical[#magical + 1] = a end
  end
  local type = g.rng:pick(magical)
  if not type then
    for _, a in ipairs(g.types.list) do
      if a.name:find("Dragon") then type = a end               -- the fallback
    end
  end
  local roll = g.rng:dice(1, 100, 0)
  local n = roll < 70 and 1 or roll < 95 and 2 or 3
  return type, n
end

--- Hire the offered hero. Returns the hero army and the allies it brought.
-- hero_recruit, Ghidra 7563:031b.
function hero.recruit(g, side, offer)
  local gameMod = require("warlords.game")
  local city = offer.city
  side.gold = math.max(0, side.gold - (offer.price or 0))
  local hx, hy = gameMod.freeTileIn(g, city, true)
  hx, hy = hx or city.x, hy or city.y

  local h = {
    x = hx, y = hy, owner = side.index, type = armytype.HERO,
    name = "Hero", strength = hero.START_STRENGTH,
    maxMoves = hero.START_MOVES, moves = 0, upkeep = 0,
    homeCity = city.index, level = 1, experience = 0, items = {},
  }
  if offer.first then
    -- turn 1: the hero carries its side's standard
    h.items[#h.items + 1] =
      { name = side.name .. " Standard", type = rules.ITEM_STANDARD, value = 1,
        standardOf = side.index }
  end
  g.armies[#g.armies + 1] = h

  local allies = {}
  if not offer.first then
    local type, n = hero.allies(g)
    for _ = 1, n do
      local ax, ay = gameMod.freeTileIn(g, city, true)
      local a = {
        x = ax or hx, y = ay or hy, owner = side.index, type = type.id, name = type.name,
        strength = type.strength, maxMoves = type.move, moves = 0,
        upkeep = type.cost // 2, homeCity = city.index,
      }
      g.armies[#g.armies + 1] = a
      allies[#allies + 1] = a
    end
  end
  return h, allies
end

--------------------------------------------------------------- experience

--- Give a hero experience, capped at 60.
function hero.addExperience(g, h, n)
  h.experience = math.min(rules.MAX_HERO_XP, (h.experience or 0) + n)
end

--- Promote a side's heroes, one step each. hero_check_promotions, 7563:0579.
-- Returns the list of heroes promoted.
function hero.checkPromotions(g, side)
  local promoted = {}
  for _, a in ipairs(g.armies) do
    if a.type == armytype.HERO and a.owner == side.index then
      local level = a.level or 1
      local need = hero.PROMOTION_AT[level]
      if need and (a.experience or 0) >= need then
        a.level = level + 1
        a.strength = math.min(hero.MAX_STRENGTH, a.strength + 1)
        a.maxMoves = a.maxMoves + hero.PROMOTION_MOVES
        a.title = hero.LEVELS[a.level]
        promoted[#promoted + 1] = a
      end
    end
  end
  return promoted
end

--- Award experience after a battle.
--
-- **Original bug.** The game credits a surviving *defending* hero only when
-- `combat_atk_type[i]` -- the **attacker's** army at the same position in the
-- line -- is a hero (67cc:0c8e). So a defending hero usually gains nothing.
-- rules.bugs.heroExperienceReadsAttackerTypes reproduces that; turn it off and
-- the defender's own line is checked, which is plainly what was meant.
function hero.battleExperience(g, attackers, defenders, result, wasCity)
  for i, a in ipairs(attackers) do
    if a.type == armytype.HERO and not result.deadByArmy[a] then
      hero.addExperience(g, a, wasCity and 2 or 1)
    end
  end
  for i, d in ipairs(defenders) do
    if not result.deadByArmy[d] then
      local counts
      if rules.bugs.heroExperienceReadsAttackerTypes then
        local sameSlot = attackers[i]                 -- the wrong line, on purpose
        counts = sameSlot ~= nil and sameSlot.type == armytype.HERO
      else
        counts = true
      end
      if counts and d.type == armytype.HERO then hero.addExperience(g, d, 1) end
    end
  end
end

--------------------------------------------------------------------- death

--- A dead hero drops everything it carried on the tile where it fell -- and
--- loses it all if that tile is water. hero_drop_items, Ghidra 67cc:16cd.
function hero.dropItems(g, h, x, y)
  if not h.items or #h.items == 0 then return {} end
  local drowned = scn.terrainAt(g.map, x, y) == move.WATER
  local dropped = {}
  for _, it in ipairs(h.items) do
    if drowned then
      it.status, it.x, it.y = 0, nil, nil            -- lost for good
    else
      it.status, it.x, it.y = 1, x, y                -- lying on the ground
      dropped[#dropped + 1] = it
    end
    -- cities vectoring to a lost standard stop doing so
    if it.standardOf then
      for _, c in ipairs(g.map.cities) do
        if c.vectorTo == -2 and c.ownerIndex == it.standardOf then c.vectorTo = nil end
      end
    end
  end
  h.items = {}
  return dropped
end

return hero
