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

--- Which content table a site draws from.
local function band(g, s, capitals)
  if s.rich then return "rich" end
  for _, c in ipairs(capitals) do
    if math.max(math.abs(s.x - c.x), math.abs(s.y - c.y)) < site.CAPITAL_RANGE then
      return "near"
    end
  end
  return "far"
end

--- Roll what every site holds. setup_random_sites, Ghidra 66d4:0000.
function site.setup(g)
  local capitals = {}
  for _, s in ipairs(g.sides) do
    if s.capital then capitals[#capitals + 1] = s.capital end
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
      s.allyType = g.rng:pick(site.ALLY_TYPES[s.band])
      s.guardian = g.rng:dice(1, 9, 0)
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

--- Does the hero survive the ruin's guardian?
-- 1d100 <= 90 + 5*(hero strength + battle items - monster strength)
--             + 3*(armies on the hero's tile, the hero included)
function site.survivesGuardian(g, h, stack, monsterStrength)
  local combat = require("warlords.combat")
  local margin = 90
                 + 5 * ((h.strength or 0) + combat.battleItems(h) - monsterStrength)
                 + 3 * #stack
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

--- Search the site a stack is standing on. Returns a table describing what
--- happened, or nil if there is nothing to search here.
-- site_search, Ghidra 6536:0000.
--- Search the site under a stack (site_search, 6536:0000). A human player
--- (`human` set) chooses at a temple what to do -- the blessing and the
--- quest are the temple dialog's buttons -- and a found item is left on the
--- ground at the ruin, for the Take button to pick up (6536:01ab); the
--- computer players are blessed and given the item there and then.
function site.search(g, stack, x, y, human)
  local gameMod = require("warlords.game")
  local heroMod = require("warlords.hero")
  local s = g.map.siteAt and g.map.siteAt[y * g.map.width + x]
  if not s or s.searched then return nil end

  local h = heroIn(stack)

  if s.content == site.TEMPLE then
    if human then return { site = s, kind = "temple", hero = h } end
    local blessed = site.bless(g, s, stack)
    local out = { site = s, kind = "temple", blessed = blessed }
    -- a stack with a hero may also take a quest here
    if h and g.map.options.quests ~= 0 then
      local side = g.map.sides[h.owner + 1]
      out.quest = require("warlords.quest").assign(g, side, h)
    end
    return out
  end

  -- only a stack with a hero may search a ruin
  if not h then return { site = s, kind = "no hero" } end
  s.searched = true

  if s.content == site.SAGE then
    heroMod.addExperience(g, h, 3)
    return { site = s, kind = "sage", hero = h }
  end

  heroMod.addExperience(g, h, 3)

  -- the guardian fights once; a guardian beaten is reported with whatever
  -- is found, as the popup tells it ("... and is victorious!")
  local beaten
  if s.guardian and s.guardian > 0 then
    local strength = site.guardianStrength(g, s)
    beaten = g.map.monsters[s.guardian]
    if not site.survivesGuardian(g, h, stack, strength) then
      heroMod.dropItems(g, h, x, y)
      for i, a in ipairs(g.armies) do
        if a == h then table.remove(g.armies, i) break end
      end
      return { site = s, kind = "killed", monster = g.map.monsters[s.guardian], hero = h }
    end
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
    local side = g.map.sides[h.owner + 1]
    local q = require("warlords.quest").event(g, side, "item", { hero = h })
    return { site = s, kind = "item", item = found, hero = h, quest = q, guardian = beaten }

  elseif s.content == site.GOLD then
    local gold = s.rich and g.rng:dice(3, 1000, 1000) or g.rng:dice(3, 500, 500)
    local side = g.map.sides[h.owner + 1]
    side.gold = side.gold + gold
    return { site = s, kind = "gold", gold = gold, hero = h, guardian = beaten }

  elseif s.content == site.ALLIES then
    local type = g.types.byId[s.allyType] or g.types.byId[armytype.SCOUTS]
    local n = s.rich and g.rng:dice(1, 2, 2) or g.rng:dice(1, 2, 0)
    local joined = {}
    for _ = 1, n do
      local ax, ay = x, y
      if #gameMod.armiesAt(g, ax, ay) >= rules.MAX_STACK then
        -- placed within one tile when the site is full
        for dx = -1, 1 do
          for dy = -1, 1 do
            local nx, ny = x + dx, y + dy
            if nx >= 0 and ny >= 0 and nx < g.map.width and ny < g.map.height
               and #gameMod.armiesAt(g, nx, ny) < rules.MAX_STACK
               and move.COST[scn.terrainAt(g.map, nx, ny)] ~= 0 then
              ax, ay = nx, ny
            end
          end
        end
      end
      if #gameMod.armiesAt(g, ax, ay) < rules.MAX_STACK then
        local a = require("warlords.hero").newAlly(
          g, type, ax, ay, h.owner, h.homeCity)
        g.armies[#g.armies + 1] = a
        joined[#joined + 1] = a
      end
    end
    return { site = s, kind = "allies", armies = joined, type = type, hero = h,
             guardian = beaten }
  end

  return { site = s, kind = "empty", hero = h, guardian = beaten }
end

return site
