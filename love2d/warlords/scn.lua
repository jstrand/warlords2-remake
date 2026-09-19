-- Warlords II scenario data: .SCN, .MAP, .RD, .ITM
-- See docs/formats/scenario.md, docs/formats/map.md and docs/formats/itm.md.
--
-- The .SCN file is loaded verbatim into one segment by the original, so every
-- offset here is also a live memory address in WARLORD2.EXE; the Ghidra
-- addresses quoted in docs/rules.md are these numbers plus 0x2c04:0.

local scn = {}

scn.MAP_W, scn.MAP_H = 112, 156
scn.TILE = 40                -- terrain tiles are 40x40
scn.TILE_MASK = 0x7FFF       -- bit 15 of a map entry is a flag

local SIDE_NAMES, SIDE_STRIDE = 0, 20
-- The eight 20-byte side names end exactly at 0xa0, where two colour tables
-- follow, one palette index a side. 0xa0 is the side's own colour -- white,
-- yellow, orange, red, green, blue, cyan, black -- and 0xb0 the colour it
-- outlines things in, black for everyone but side 7, who outlines in red.
-- 54f6:0000 uses both to frame the turn banner.
local SIDE_COLOURS, SIDE_EDGES = 0xa0, 0xb0
local SIDE_RECS, SIDE_REC_STRIDE = 387, 20
local LEVELS, CONTROLLERS, ENHANCED = 0xc0, 0xd0, 0xf0
local MONSTER_STRENGTH = 0x1007
local FIGHT_ORDER, FIGHT_ROWS, FIGHT_TYPES = 0x60b, 9, 29
local COMBAT_CAP, DIPLO_SCORE = 0x112, 0x10e3
local TERRAIN_TABLE, TERRAIN_COUNT = 0x710, 255
local SITES_COUNT, SITES, SITE_STRIDE = 0x80f, 0x811, 31
local ITEMS, ITEM_STRIDE, N_ITEMS = 3305, 29, 22
local MONSTERS, MONSTER_STRIDE, N_MONSTERS = 3943, 16, 10
local CITIES_COUNT, CITIES, CITY_STRIDE = 5499, 5501, 65

-- Option words. They are NOT in menu order; see docs/rules.md > Game setup.
local OPTIONS = {
  neutralCities = 0x11a, diplomacy = 0x11c, quests = 0x11e,
  randomTurns = 0x122, hiddenMap = 0x124, intenseCombat = 0x126,
  quickStart = 0x128, viewEnemies = 0x12a, militaryAdvisor = 0x12c,
  tutorial = 0x12e, viewProduction = 0x132,
}

local function u16(s, off)   -- off is 0-based, as in the docs
  return s:byte(off + 1) + s:byte(off + 2) * 256
end

local function cstr(s, off, len)
  local raw = s:sub(off + 1, off + len)
  local z = raw:find("\0", 1, true)
  return z and raw:sub(1, z - 1) or raw
end

local function readAll(path)
  local f = assert(io.open(path, "rb"), "cannot open: " .. path)
  local s = f:read("*a")
  f:close()
  return s
end
scn.readAll = readAll

local function fileExists(path)
  local f = io.open(path, "rb")
  if f then f:close() return true end
  return false
end

--- The scenario's .ITM magic item pool (docs/formats/itm.md).
function scn.loadItemPool(path)
  if not fileExists(path) then return nil end
  local lines = {}
  for line in readAll(path):gmatch("([^\r\n]+)") do lines[#lines + 1] = line end
  local count = tonumber(lines[1])
  local pool = {}
  for i = 2, math.min(#lines, count + 1) do
    local line = lines[i]
    pool[#pool + 1] = {
      name = (line:sub(1, 20):gsub("_", " "):gsub("%s+$", "")),
      type = tonumber(line:sub(22, 22)),
      value = tonumber(line:sub(24, 24)),
    }
  end
  return pool
end

function scn.load(dir, name)
  local base = dir .. "/" .. name
  local s = readAll(base .. ".SCN")
  assert(#s == 12001, "unexpected .SCN size: " .. #s)

  local options = {}
  for key, off in pairs(OPTIONS) do options[key] = u16(s, off) end

  local sides = {}
  for i = 0, 7 do
    local o = SIDE_RECS + SIDE_REC_STRIDE * i
    sides[i + 1] = {
      index = i,
      name = cstr(s, SIDE_NAMES + SIDE_STRIDE * i, SIDE_STRIDE),
      colour = u16(s, SIDE_COLOURS + 2 * i),
      edge = u16(s, SIDE_EDGES + 2 * i),
      gold = u16(s, o + 2),
      capX = u16(s, o + 6),
      capY = u16(s, o + 8),
      -- 0 = human, 1 = computer; level 0-2; Enhanced gives +2 strength
      computer = u16(s, CONTROLLERS + 2 * i) ~= 0,
      level = u16(s, LEVELS + 2 * i),
      enhanced = u16(s, ENHANCED + 2 * i) ~= 0,
      diploScore = u16(s, DIPLO_SCORE + 2 * i),

    }
  end

  local cities, byPos = {}, {}
  for i = 0, u16(s, CITIES_COUNT) - 1 do
    local o = CITIES + CITY_STRIDE * i
    local produces = {}
    for k = 0, 3 do
      local t = s:byte(o + 22 + k + 1)
      -- the game removes Navy (type 5) from every city at start
      if t ~= 255 and t ~= 5 then produces[#produces + 1] = t end
    end
    local c = {
      index = i,
      x = u16(s, o), y = u16(s, o + 2),
      name = cstr(s, o + 4, 16),
      income = s:byte(o + 42 + 1),
      produces = produces,
      -- derived, not stored: see docs/rules.md
      defence = (#produces >= 3) and 2 or 1,
    }
    cities[#cities + 1] = c
    byPos[c.y * scn.MAP_W + c.x] = c
  end

  -- A city covers a 2x2 footprint on the map (path_build_cost_grid marks all
  -- four tiles); byPos keys the top-left, cityTile every tile of it.
  local cityTile = {}
  for _, c in ipairs(cities) do
    for dx = 0, 1 do
      for dy = 0, 1 do
        cityTile[(c.y + dy) * scn.MAP_W + (c.x + dx)] = c
      end
    end
  end

  -- Ownership is derived: at scenario start a side owns only its capital.
  for _, sd in ipairs(sides) do
    local c = byPos[sd.capY * scn.MAP_W + sd.capX]
    sd.inUse = c ~= nil
    sd.capital = c
    if c then c.owner = sd end
  end

  -- fight order: 29 bytes per player, row 8 for neutral (FUN_1b62_0024)
  local fightOrder = {}
  for row = 0, FIGHT_ROWS - 1 do
    local r = {}
    for t = 0, FIGHT_TYPES - 1 do
      r[t] = s:byte(FIGHT_ORDER + row * FIGHT_TYPES + t + 1)
    end
    fightOrder[row] = r
  end

  local sites = {}
  for i = 0, u16(s, SITES_COUNT) - 1 do
    local o = SITES + SITE_STRIDE * i
    sites[#sites + 1] = {
      index = i,
      x = u16(s, o), y = u16(s, o + 2),
      name = cstr(s, o + 4, 20),
      type = u16(s, o + 24),          -- 1 = temple, 2 = ruin
    }
  end

  local items = {}
  for i = 0, N_ITEMS - 1 do
    local o = ITEMS + ITEM_STRIDE * i
    local nm = cstr(s, o, 20)
    if nm ~= "" then
      items[#items + 1] = {
        index = i, name = nm, type = s:byte(o + 21), value = s:byte(o + 22),
      }
    end
  end

  local monsters = {}
  for i = 0, N_MONSTERS - 1 do
    local nm = cstr(s, MONSTERS + MONSTER_STRIDE * i, 12)
    if nm ~= "" then
      -- keyed by slot: a site's guardian byte indexes this directly
      monsters[i] = { index = i, name = nm, strength = u16(s, MONSTER_STRENGTH + 2 * i) }
    end
  end

  -- tile index -> terrain type id, the same table WARLORD2.EXE reads
  local terrainType = {}
  for i = 0, TERRAIN_COUNT - 1 do terrainType[i] = s:byte(TERRAIN_TABLE + i + 1) end

  -- terrain grid + road overlay. Bit 15 of a map word marks a **crossing**:
  -- the tile where a stack may change between land and water (path_build_cost_grid,
  -- Ghidra 1555:0d9e).
  local m = readAll(base .. ".MAP")
  local tiles, crossing = {}, {}
  for i = 0, scn.MAP_W * scn.MAP_H - 1 do
    local w = u16(m, i * 2)
    tiles[i + 1] = w % 0x8000
    crossing[i + 1] = w >= 0x8000
  end
  local roads = readAll(base .. ".RD")

  return {
    name = name, sides = sides, cities = cities, cityAt = byPos, cityTile = cityTile,
    sites = sites, items = items, monsters = monsters, crossing = crossing,
    itemPool = scn.loadItemPool(base .. ".ITM"),
    options = options, terrainType = terrainType, fightOrder = fightOrder,
    combatCap = u16(s, COMBAT_CAP),
    tiles = tiles, roads = roads,
    width = scn.MAP_W, height = scn.MAP_H,
  }
end

function scn.tileAt(g, x, y)
  return g.tiles[y * scn.MAP_W + x + 1]
end

function scn.roadAt(g, x, y)
  return g.roads:byte(y * scn.MAP_W + x + 1)
end

--- Is this tile a crossing (map word bit 15)?
function scn.isCrossing(g, x, y)
  return g.crossing[y * scn.MAP_W + x + 1]
end

--- The terrain type id (0-11) of a map tile, via the scenario's own table.
function scn.terrainAt(g, x, y)
  return g.terrainType[scn.tileAt(g, x, y) % 256]
end

-------------------------------------------------------------- city castles
--
-- A city's castle is drawn from the map itself: the .MAP file ships every
-- city as tile 96, and the game *rewrites* those four tiles whenever the
-- city changes hands. SCENERY1 holds the castles as 80x80 blocks -- two
-- tiles across and two rows down -- so a block whose top-left cell is `t`
-- covers t, t+1, t+16, t+17 (16 cells to a sheet row).
--
-- set_city_tiles (6bd8:0000) picks the block for an owner:
--
--     if owner == 15 then owner = -1 end          -- neutral
--     if owner < 6 then base, off = 0x62, owner * 2
--     else              base, off = 0x80, (owner - 6) * 2 end
--
-- so neutral lands on 0x60 = 96, sides 0..5 run 98..108 along the top row,
-- and sides 6 and 7 restart at 128 on the second -- the gap at 110 is the
-- mountain sprite sitting between them.
--
-- city_make_ruins (649c:016b) has no such split: the razed row is eight
-- contiguous blocks from 0xa0, and it reads the owner *before* clearing it,
-- so ruins keep the colours of whoever held the city.

function scn.cityTileBase(city)
  if city.razed then
    return 0xa0 + 2 * (city.razedBy or 0)
  end
  local o = city.ownerIndex
  if not o then return 96 end          -- the original passes owner -1
  if o < 6 then return 0x62 + 2 * o end
  return 0x80 + 2 * (o - 6)
end

--- Stamp a city's 2x2 castle onto the map in its owner's colours.
function scn.setCityTiles(map, city)
  local t = scn.cityTileBase(city)
  local i = city.y * scn.MAP_W + city.x + 1
  map.tiles[i] = t
  map.tiles[i + 1] = t + 1
  map.tiles[i + scn.MAP_W] = t + 16
  map.tiles[i + scn.MAP_W + 1] = t + 17
end

--- Restamp every city. Cheap enough to run after a load rather than track.
function scn.refreshCityTiles(map)
  for _, c in ipairs(map.cities) do scn.setCityTiles(map, c) end
end

return scn
