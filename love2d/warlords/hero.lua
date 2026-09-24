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

---------------------------------------------------------------------- names
--
-- Each side has its own hundred candidate heroes in
-- TERRAIN<set>/HERONAM<side>.DAT, a line apiece:
--
--     #0 Sir Nick          male
--     #1 Mystichla         female
--
-- load_hero_name (6563:0c67) reads the whole file, rolls dice(1, 100, 0) and
-- walks it for the n'th '#'. The count is not read from the file: 100 is
-- hard-coded, and HERONAM4.DAT ships with 101 lines, so its last hero -- Lady
-- Jorinas -- can never be drawn. Side 0's list is all male, which is why the
-- Sirians never field a heroine.
--
-- The set number is a scenario word (.SCN 0x161); every shipped scenario uses
-- 0, as every other TERRAIN0 path in the engine already assumes.
hero.NAME_ROLL = 100

local function nameFile(g, side)
  return (g.dataDir or "original") .. "/TERRAIN0/HERONAM" .. side.index .. ".DAT"
end

--- The candidate list for a side, as { {name=, female=}, ... }. Cached: the
--- original rereads the file for every hero, which we have no reason to copy.
function hero.names(g, side)
  g.heroNames = g.heroNames or {}
  local got = g.heroNames[side.index]
  if got then return got end

  local list = {}
  local ok, text = pcall(scn.readAll, nameFile(g, side))
  for line in (ok and text or ""):gmatch("[^\r\n]+") do
    local sex, name = line:match("^#(%d)%s+(.+)$")
    if name then list[#list + 1] = { name = name, female = sex == "1" } end
  end
  g.heroNames[side.index] = list
  return list
end

--- Roll the name and sex of a hero offering itself to `side`.
function hero.rollName(g, side)
  local list = hero.names(g, side)
  local n = g.rng:dice(1, hero.NAME_ROLL, 0)
  -- the original would walk off the end of a short file; we stay inside it
  local pick = list[n] or list[#list]
  if not pick then return "Hero", false end
  return pick.name, pick.female
end

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

  -- auto_ui_hero_emerges (6563:0d5c) calls load_hero_name as it opens the
  -- dialog, so the roll happens once an offer exists, whether or not it is
  -- taken up -- and the name is settled before the player ever sees it.
  local function named(o)
    o.name, o.female = hero.rollName(g, side)
    return o
  end

  if g.turn == 1 then
    return named { price = 0, city = side.capital, first = true }
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
  return named { price = price, city = city }
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

--- One ally army, as create_ally_army (Ghidra 6536:12e6) builds it: the type's
--- own strength and move, arriving with its moves already full (+6 and +7 both
--- get the type's move) and with **no upkeep** -- allies are free to keep.
-- All three places allies appear (a hired hero's escort, a ruin, a quest
-- reward) go through that one routine, so they go through this one.
function hero.newAlly(g, type, x, y, owner, homeCity)
  return {
    x = x, y = y, owner = owner, type = type.id, name = type.name,
    strength = type.strength, maxMoves = type.move, moves = type.move,
    upkeep = 0, homeCity = homeCity,
  }
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
    name = offer.name or "Hero", female = offer.female or nil,
    strength = hero.START_STRENGTH,
    -- A hero rides in ready: hero_recruit writes its full move allowance to
    -- both the maximum (+6) and the moves left (+7), so it can act on the turn
    -- it joins. Produced armies are the ones that wait a turn, not these.
    maxMoves = hero.START_MOVES, moves = hero.START_MOVES, upkeep = 0,
    homeCity = city.index, level = 1, experience = 0, items = {},
  }
  if offer.first then
    -- Turn 1: the hero carries its side's standard -- the scenario's own item
    -- record, number = side (hero_recruit), not a new one. Items 0-7 are the
    -- eight standards.
    local std = g.map.items[side.index + 1]
    if std then
      std.status, std.x, std.y = 3, nil, nil
      std.standardOf = side.index
      h.items[#h.items + 1] = std
    end
  end
  g.armies[#g.armies + 1] = h
  local history = require("warlords.history")
  history.deed(g, side, history.EMERGES, city.index, 0, h.name)     -- 7563:04e7

  local allies = {}
  if not offer.first then
    local type, n = hero.allies(g)
    for _ = 1, n do
      local ax, ay = gameMod.freeTileIn(g, city, true)
      local a = hero.newAlly(g, type, ax or hx, ay or hy, side.index, city.index)
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

--------------------------------------------------------------------- items

--- What the hero info dialog lists for a hero (list_carried_items, mode 6,
--- 796c:03d9): the items the hero carries, then the items lying on the tile
--- the hero stands on, each in item order.
function hero.itemsHere(g, h)
  local carried, ground = {}, {}
  for _, it in ipairs(h.items or {}) do carried[#carried + 1] = it end
  for _, it in ipairs(g.map.items) do
    if it.status == 1 and it.x == h.x and it.y == h.y then ground[#ground + 1] = it end
  end
  local function byIndex(a, b) return (a.index or 0) < (b.index or 0) end
  table.sort(carried, byIndex)
  table.sort(ground, byIndex)
  for _, it in ipairs(ground) do carried[#carried + 1] = it end
  return carried
end

--- Put an item down on the hero's tile (7563:0943) -- or lose it there, if
--- the hero is at sea.
function hero.dropItem(g, h, it)
  for i, c in ipairs(h.items or {}) do
    if c == it then table.remove(h.items, i) break end
  end
  if scn.terrainAt(g.map, h.x, h.y) == move.WATER then
    it.status, it.x, it.y = 0, nil, nil
  else
    it.status, it.x, it.y, it.planted = 1, h.x, h.y, nil
  end
end

--- Pick an item up off the hero's tile (7563:08c7). A standard picked up is no
--- longer planted.
function hero.takeItem(g, h, it)
  it.status, it.x, it.y, it.planted = 3, nil, nil, nil
  h.items = h.items or {}
  h.items[#h.items + 1] = it
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
