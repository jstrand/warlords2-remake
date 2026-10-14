-- Ruins, temples and sages: what they hold, and what searching them does.
--
-- docs/rules.md > Ruins, temples and sages. The contents are rolled once at
-- game start (`setup_random_sites`, Ghidra 66d4:0000); searching is
-- `site_search`, 6536:0000.

local armytype = require("warlords.armytype")
local rules    = require("warlords.rules")
local scn      = require("warlords.scn")
local move     = require("warlords.move")

local site = {}

-- what a site holds
site.EMPTY, site.TEMPLE, site.ITEM, site.SAGE, site.GOLD, site.ALLIES = 0, 1, 2, 3, 4, 5
site.CONTENT_NAMES = {
  [0] = "empty", "temple", "a magic item", "a sage", "gold", "allies",
}

site.CAPITAL_RANGE = 15        -- "a capital within 15 tiles"
site.RICH_SHARE = 3            -- sites * 3 / 10 are rich
site.BLESSING_TEMPLES = 4      -- only the first four temples can bless

-- The content tables, by band. 3 = sage, 4 = gold, 5 = allies.
site.CONTENT = {
  rich = { 5, 5, 4 },
  far  = { 3, 4, 5, 3, 4 },
  near = { 3, 4, 5 },
}

-- Ally army types, by band (army type ids).
site.ALLY_TYPES = {
  rich = { 25, 23, 27, 19 },   -- Dragons, Wizards, Devils, Archons
  far  = { 24, 20, 26 },       -- Ghosts, Giant Worms, Demons
  near = { 22, 20 },           -- Elementals, Giant Worms
}

--------------------------------------------------------------- setting up

--- Items that may only be hidden in a rich ruin. item_reserved, 66d4:08f4.
function site.itemReserved(item)
  return item.type == rules.ITEM_FLIGHT or item.type == rules.ITEM_DOUBLE_MOVE
      or item.type == rules.ITEM_STANDARD
      or (item.type == rules.ITEM_COMMAND and item.value >= 2)
end

--- Refill item records 8..21 from the scenario's .ITM pool. load_item_pool,
--- 66d4:04ef: the first `reserved` slots take reserved items, the rest
--- ordinary ones.
function site.fillItemPool(g, reserved)
  local pool = g.map.itemPool
  if not pool then return end
  local byIndex, used = {}, {}
  for _, it in ipairs(g.map.items) do byIndex[it.index] = it end

  for idx = 8, 21 do
    local item = byIndex[idx]
    if item then
      local want = idx < 8 + reserved
      local choices = {}
      for i, p in ipairs(pool) do
        if not used[i] and site.itemReserved(p) == want then choices[#choices + 1] = i end
      end
      local pick = g.rng:pick(choices)
      if pick then
        used[pick] = true
        item.name, item.type, item.value = pool[pick].name, pool[pick].type, pool[pick].value
      end
    end
  end
end

--- Mark `sites * 3 / 10` non-temple sites rich. mark_rich_sites, 66d4:091e.
-- Every site's "revealed to" mask starts full; with Quests on the rich ones
-- are cleared, which is what a sage can later point at.
function site.markRich(g)
  local ruins = {}
  for _, s in ipairs(g.map.sites) do
    s.rich, s.revealed = false, 0xff
    if s.type ~= site.TEMPLE then ruins[#ruins + 1] = s end
  end
  local want = math.floor(#g.map.sites * site.RICH_SHARE / 10)
  g.rng:shuffle(ruins)
  for i = 1, math.min(want, #ruins) do
    ruins[i].rich = true
    if g.map.options.quests ~= 0 then ruins[i].revealed = 0 end
  end
end

--- Which content table a site draws from: rich, near (an in-play side's
--- capital less than 15 away by map_distance, 1a8b:0acc) or far.
local function band(g, s, capitals)
  if s.rich then return "rich" end
  for _, c in ipairs(capitals) do
    local dx, dy = s.x - c.x, s.y - c.y
    if math.floor(math.sqrt(dx * dx + dy * dy)) < site.CAPITAL_RANGE then
      return "near"
    end
  end
  return "far"
end

--- Roll what every site holds. setup_random_sites, Ghidra 66d4:0000.
function site.setup(g)
  -- the in-play sides' capitals where they stand (+0x189), city or not
  local capitals = {}
  for _, s in ipairs(g.sides) do
    capitals[#capitals + 1] = { x = s.capX, y = s.capY }
  end

  site.markRich(g)
  local temples = 0
  for _, s in ipairs(g.map.sites) do
    s.content = s.type == site.TEMPLE and site.TEMPLE or site.EMPTY
    if s.content == site.TEMPLE then
      s.templeIndex, temples = temples, temples + 1   -- only the first four bless
    end
    s.item, s.guardian, s.allyType, s.searched = nil, nil, nil, false
    s.band = band(g, s, capitals)
  end

  -- magic items, from the .ITM pool
  local reserved = math.floor(#g.map.sites * 2 / 10)
  site.fillItemPool(g, reserved)

  local bandHi = reserved
  local bandLo = math.min(g.rng:dice(2, 3, 1), bandHi)
  local last = math.min(22, math.floor(#g.map.sites / 3) + g.rng:dice(1, 5, -3) + 8)

  local free = {}
  for _, s in ipairs(g.map.sites) do
    if s.content == site.EMPTY then free[#free + 1] = s end
  end
  g.rng:shuffle(free)

  local byIndex = {}
  for _, it in ipairs(g.map.items) do
    byIndex[it.index] = it
    it.status = 0
  end

  for idx = 8, last - 1 do
    local item = byIndex[idx]
    -- the band between bandLo and bandHi is held back, probably for quests
    if item and not (idx >= 8 + bandLo and idx < 8 + bandHi) then
      local want = site.itemReserved(item)
      for i, s in ipairs(free) do
        if s.rich == want then
          s.content, s.item = site.ITEM, idx
          item.status = 2                        -- hidden in a ruin
          table.remove(free, i)
          break
        end
      end
    end
  end

  -- everything else rolls its contents, and its guardian
  for _, s in ipairs(g.map.sites) do
    if s.content == site.EMPTY then
      s.content = g.rng:pick(site.CONTENT[s.band])
    end
    if s.content == site.TEMPLE then
      s.guardian = nil
    elseif s.content == site.ALLIES then
      -- the allies' type takes the guardian's place (66d4:0000)
      s.allyType = g.rng:pick(site.ALLY_TYPES[s.band])
      s.guardian = nil
    else
      s.guardian = g.rng:dice(1, 9, 0)
    end
  end
  g.map.siteAt = {}
  for _, s in ipairs(g.map.sites) do
    g.map.siteAt[s.y * g.map.width + s.x] = s
  end
end

--------------------------------------------------------------------- searching

local function heroIn(stack)
  for _, a in ipairs(stack) do
    if a.type == armytype.HERO then return a end
  end
  return nil
end

--- Does the hero survive the ruin's guardian (6536:07ad)?
-- 1d100 <= 90 + 5*(hero strength + battle items - monster strength)
--             + 3*(armies on the hero's tile, whoever's, the hero included)
function site.survivesGuardian(g, h, monsterStrength)
  local combat = require("warlords.combat")
  local margin = 90
                 + 5 * ((h.strength or 0) + combat.battleItems(h) - monsterStrength)
                 + 3 * #require("warlords.game").armiesAt(g, h.x, h.y)
  return g.rng:dice(1, 100, 0) <= margin
end

--- The strength of a site's guardian, from the scenario's monster table.
function site.guardianStrength(g, s)
  if not s.guardian then return 0 end
  local m = g.map.monsters[s.guardian]
  return m and m.strength or 0
end

--- Bless a stack at a temple. Each army gains +1 strength (max 9), once per
--- temple per army; only the first four temples can bless.
function site.bless(g, s, stack)
  local heroMod = require("warlords.hero")
  if (s.templeIndex or 0) >= site.BLESSING_TEMPLES then return 0 end
  local bit = s.templeIndex
  local blessed = 0
  for _, a in ipairs(stack) do
    a.blessings = a.blessings or {}
    if not a.blessings[bit] then
      a.blessings[bit] = true
      a.strength = math.min(9, (a.strength or 0) + 1)
      blessed = blessed + 1
      if a.type == armytype.HERO then heroMod.addExperience(g, a, 1) end
    end
  end
  return blessed
end

--- Search the site under a stack (site_search, 6536:0000). Returns a table
--- describing what happened, or nil if there is nothing to search here. The
--- stack's first army needs a move left. With no hero only a temple does
--- anything: it blesses at once. With one, a human player (`human` set)
--- chooses at a temple what to do -- the blessing and the quest are the
--- temple dialog's buttons -- and a found item is left on the ground at the
--- ruin, for the Take button to pick up (6536:01ab); the computer players
--- are blessed and given the item there and then.
function site.search(g, stack, x, y, human)
  local heroMod = require("warlords.hero")
  local history = require("warlords.history")
  local s = g.map.siteAt and g.map.siteAt[y * g.map.width + x]
  if not s or s.searched then return nil end
  if not stack[1] or (stack[1].moves or 0) < 1 then return nil end

  local h = heroIn(stack)

  if s.content == site.TEMPLE then
    if human and h then return { site = s, kind = "temple", hero = h } end
    local blessed = site.bless(g, s, stack)
    return { site = s, kind = "temple", blessed = blessed, hero = h }
  end

  -- only a stack with a hero may search a ruin
  if not h then return { site = s, kind = "no hero" } end
  local side = g.map.sides[h.owner + 1]

  if s.content == site.SAGE then
    heroMod.addExperience(g, h, 3)
    history.deed(g, side, history.FINDS, history.SAGE, 0, h.name)  -- 6536:013c
    s.searched = true
    return { site = s, kind = "sage", hero = h }
  end

  -- ruin_search (6536:01ab)
  heroMod.addExperience(g, h, 3)

  -- the guardian fights once; a guardian beaten is reported with whatever
  -- is found, as the popup tells it ("... and is victorious!"). An allies
  -- site holds its allies' type where the guardian would be, and nothing
  -- guards it.
  local beaten
  if s.content ~= site.ALLIES and s.guardian and s.guardian > 0 then
    local strength = site.guardianStrength(g, s)
    local monster = g.map.monsters[s.guardian]
    if not site.survivesGuardian(g, h, strength) then
      history.deed(g, side, history.KILLED, history.SEARCHING, 0, h.name)  -- 6536:02ef
      history.tally(g, h, nil, false)   -- the side's own losses
      heroMod.dropItems(g, h, h.x, h.y, h.owner)
      for i, a in ipairs(g.armies) do
        if a == h then table.remove(g.armies, i) break end
      end
      return { site = s, kind = "killed", monster = monster, hero = h }
    end
    beaten = monster
    s.guardian = 0
  end

  if s.content == site.ITEM then
    local found
    for _, it in ipairs(g.map.items) do
      if it.index == s.item then found = it end
    end
    if found and human then
      found.status, found.x, found.y = 1, x, y    -- on the ground, to take
    elseif found then
      found.status = 3                            -- carried
      h.items = h.items or {}
      h.items[#h.items + 1] = found
    end
    if found then
      history.deed(g, side, history.FINDS, found.index, 0, h.name)   -- 6536:0571
    end
    s.searched = true
    s.content = site.EMPTY
    return { site = s, kind = "item", item = found, hero = h, guardian = beaten }

  elseif s.content == site.GOLD then
    local gold = s.rich and g.rng:dice(3, 1000, 1000) or g.rng:dice(3, 500, 500)
    side.gold = side.gold + gold
    s.searched = true
    history.deed(g, side, history.FINDS, history.GOLD, 0, h.name)
    return { site = s, kind = "gold", gold = gold, hero = h, guardian = beaten }

  elseif s.content == site.ALLIES then
    s.searched = true
    local type = g.types.byId[s.allyType] or g.types.byId[armytype.SCOUTS]
    local n = g.rng:dice(1, 2, 0)
    if s.rich then n = n + 2 end
    local joined = {}
    for _ = 1, n do
      local a = heroMod.placeAlly(g, type, x, y, side)
      if a then joined[#joined + 1] = a end
    end
    history.deed(g, side, history.FINDS, history.ALLIES, 0, h.name)  -- 6536:07a1
    return { site = s, kind = "allies", armies = joined, count = n, type = type, hero = h,
             guardian = beaten }
  end

  return { site = s, kind = "empty", hero = h, guardian = beaten }
end

-------------------------------------------------------------------- sages

-- What a sage can tell a side, for its hero at (hx, hy) (6536:1610): a rich
-- site the side has not been shown, within 35 tiles as the crow flies -- one
-- entry for gold and one for allies, however many there are, and one for
-- each item by name. An entry is { kind = "gold" | "allies" | "item",
-- item = item, name = text }.
site.SAGE_RANGE = 35

local function sageDistance(s, hx, hy)
  return math.floor(math.sqrt((s.x - hx) ^ 2 + (s.y - hy) ^ 2))   -- 2012:1199
end

--- Has the side been shown the site (its bit in +0x0c)?
function site.shownTo(s, side)
  return math.floor((s.revealed or 0) / 2 ^ side.index) % 2 == 1
end
local shownTo = site.shownTo

local function unshown(g, side, s, hx, hy)
  return s.rich and not s.searched and not shownTo(s, side)
         and sageDistance(s, hx, hy) < site.SAGE_RANGE
end

function site.sageList(g, side, hx, hy)
  local list, gold, allies = {}, false, false
  for _, s in ipairs(g.map.sites) do
    if unshown(g, side, s, hx, hy) then
      if s.content == site.GOLD and not gold then
        gold = true
        list[#list + 1] = { kind = "gold", name = "Gold" }
      elseif s.content == site.ALLIES and not allies then
        allies = true
        list[#list + 1] = { kind = "allies", name = "Allies" }
      elseif s.content == site.ITEM then
        for _, it in ipairs(g.map.items) do
          if it.index == s.item then
            list[#list + 1] = { kind = "item", item = it, name = it.name }
          end
        end
      end
    end
  end
  return list
end

--- Show the side where an entry of sageList lies (6536:0e85): for gold or
--- allies the nearest such site, for an item the site that holds it. The
--- site is marked as shown to the side and the tiles round it uncovered.
function site.sageShow(g, side, entry, hx, hy)
  local found, best
  for _, s in ipairs(g.map.sites) do
    if entry.kind == "item" then
      if s.content == site.ITEM and s.item == entry.item.index then found = s break end
    elseif unshown(g, side, s, hx, hy)
           and s.content == (entry.kind == "gold" and site.GOLD or site.ALLIES) then
      local d = sageDistance(s, hx, hy)
      if not best or d < best then found, best = s, d end
    end
  end
  if not found then return nil end
  if not shownTo(found, side) then found.revealed = (found.revealed or 0) + math.floor(2 ^ side.index) end
  require("warlords.game").reveal(g, side.index, found.x, found.y, false)
  return found
end

--- The sage's gem (6536:0b1a): 3d500 + 500 gold.
function site.sageGem(g, side)
  local n = g.rng:dice(3, 500, 500)
  side.gold = side.gold + n
  return n
end

--- Uncover a patch of the map round the tile pointed at (6536:0cd6): from
--- 9-13 tiles up and left of it, 16-25 tiles wide and high, kept on the
--- map. Returns the patch, x, y, w, h in tiles.
function site.sageMap(g, side, cx, cy)
  local game = require("warlords.game")
  local x0 = math.max(0, cx - g.rng:dice(1, 5, 8))
  local y0 = math.max(0, cy - g.rng:dice(1, 5, 8))
  local w, h = g.rng:dice(1, 10, 15), g.rng:dice(1, 10, 15)
  if x0 + w >= g.map.width then w = g.map.width - x0 - 1 end
  if y0 + h >= g.map.height then h = g.map.height - y0 - 1 end
  for x = x0, x0 + w - 1 do
    for y = y0, y0 + h - 1 do game.reveal(g, side.index, x, y, false) end
  end
  return x0, y0, w, h
end

return site
