-- Warlords II scenario data: .SCN, .MAP, .RD
-- See docs/formats/scenario.md and docs/formats/map.md.

local scn = {}

scn.MAP_W, scn.MAP_H = 112, 156
scn.TILE = 40                -- terrain tiles are 40x40
scn.TILE_MASK = 0x7FFF       -- bit 15 of a map entry is a flag

local SIDE_NAMES, SIDE_STRIDE = 0, 20
local SIDE_RECS, SIDE_REC_STRIDE = 387, 20
local CITIES_COUNT, CITIES, CITY_STRIDE = 5499, 5501, 65

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

function scn.load(dir, name)
  local base = dir .. "/" .. name
  local s = readAll(base .. ".SCN")
  assert(#s == 12001, "unexpected .SCN size: " .. #s)

  local sides = {}
  for i = 0, 7 do
    local o = SIDE_RECS + SIDE_REC_STRIDE * i
    sides[i + 1] = {
      index = i,
      name = cstr(s, SIDE_NAMES + SIDE_STRIDE * i, SIDE_STRIDE),
      gold = u16(s, o + 2),
      capX = u16(s, o + 6),
      capY = u16(s, o + 8),
    }
  end

  local cities, byPos = {}, {}
  for i = 0, u16(s, CITIES_COUNT) - 1 do
    local o = CITIES + CITY_STRIDE * i
    local produces = {}
    for k = 0, 3 do
      local t = s:byte(o + 22 + k + 1)
      if t ~= 255 then produces[#produces + 1] = t end
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

  -- Ownership is derived: at scenario start a side owns only its capital.
  for _, sd in ipairs(sides) do
    local c = byPos[sd.capY * scn.MAP_W + sd.capX]
    sd.inUse = c ~= nil
    sd.capital = c
    if c then c.owner = sd end
  end

  -- terrain grid + road overlay
  local m = readAll(base .. ".MAP")
  local tiles = {}
  for i = 0, scn.MAP_W * scn.MAP_H - 1 do
    tiles[i + 1] = u16(m, i * 2) % 0x8000
  end
  local roads = readAll(base .. ".RD")

  return {
    name = name, sides = sides, cities = cities, cityAt = byPos,
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

return scn
