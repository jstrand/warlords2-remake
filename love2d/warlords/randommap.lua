-- The random map generator, "A Random World": random_map_setup (7bab:10e8)
-- and random_map_generate (4bed:011c), phase for phase. docs/re/random_map.md.
--
-- A port of the web port's web/src/warlords/randommap.js, line for line: the
-- engines' dice are the same, so a seed makes the same world in both.
--
-- RANDOM\RANDOM.DAT is both the generator's parameters and its memory: the
-- original reads the whole 0xa560 bytes into one buffer and works in it, the
-- 112x156 grid of terrain types at +0x6120 included. This does the same, so
-- the offsets below are RANDOM.DAT's own, and the few reads the original makes
-- off the edge of the grid land where they did. Writes off the grid are
-- dropped; the original's would corrupt its own tables.
--
-- The result is a scenario like any other -- RANDOM.SCN (Erythea's, rewritten),
-- .MAP, .RD, .SGN, .CTY and .SPC -- which scn.load reads as it reads the
-- shipped ones, out of scn's store of files in memory (scn.install): the
-- shipped RANDOM folder holds the last world the original made, and is left
-- alone. The dice are the engine's (rng.lua), so a map is not the one the
-- original would make from the same seed, but every rule is its rule.

local armytype = require("warlords.armytype")
local move     = require("warlords.move")
local scn      = require("warlords.scn")

local M = {}

M.DIR = "RANDOM"
M.FILES = { "SCN", "MAP", "RD", "SGN", "CTY", "SPC" }   -- what it makes
local W, H = 112, 156

-- terrain type ids, as the grid and the scenario's tile table number them
local ROAD, BRIDGE, WATER, SHORE, FOREST, HILLS = 0, 1, 2, 3, 4, 5
local MOUNTAINS, PLAIN, MARSH, TOWER, CITY, SITE = 6, 7, 8, 9, 10, 11

-- RANDOM.DAT: the parameters (the sliders add to these)
local P_CITIES, P_EDGE_POINTS = 0x2a, 0x2c
local P_MOUNTAINS, P_HILLS, P_WIDE_RIVERS, P_RIVERS = 0x34, 0x36, 0x38, 0x3a
local P_FOREST, P_PASSES, P_EROSION = 0x3c, 0x3e, 0x40
local SLIDER_HILLS, SLIDER_WATER, SLIDER_FOREST, SLIDER_CITIES = 0x44, 0x52, 0x60, 0x6e
local CORNERS, EDGES, DIAGONALS = 0x7c, 0x8c, 0xac
local NB = 0xbc          -- the 8 neighbours, S first and anticlockwise
local NB_ROAD = 0xdc     -- the 8 again, N first and clockwise: road shapes
-- RANDOM.DAT: the generator's working space and its tables
local EDGE_COUNT, EDGE_PTS, EDGE_START, EDGE_END = 0xfc, 0x104, 0x244, 0x254
local SHAPE = 0x264      -- neighbour mask -> tile variant, -1 for none
local ROAD_SHAPE = 0x364 -- neighbour mask -> road id - 1, -1 for none
local TILES = 0x464      -- per terrain type, 16 variants x 2 checkerboard tiles
local SIDE_NAMES, SITE_FORMATS = 0x764, 0xed2
local SIGN_CITY, SIGN_LEAGUES, SIGNS = 0x1888, 0x189c, 0x18b0
local SIDE_CLASS, REGION_CLASS, PRODUCTION = 0x1d62, 0x1d72, 0x1d82
local FOREST_DONE, FOREST_WANTED = 0x1f52, 0x1f54
local NODES, NODE_COUNT, GRID = 0x5dd6, 0x611e, 0x6120
local DAT_SIZE = 0xa560

-- .SCN offsets (docs/formats/scenario.md)
local S_SIDE_REC, S_RANDOM, S_TERRAIN_SET = 387, 0x120, 0x161
local S_TERRAIN, S_SITE_COUNT, S_SITES, SITE_STRIDE = 0x710, 0x80f, 0x811, 31
local S_CITY_COUNT, S_CITIES, CITY_STRIDE = 5499, 5501, 65
local N_SITES = 40

-- directions the signposts give (4125:2b7c)
local COMPASS = { [0] = "north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest" }
-- the twelve tiles round a city's 2x2 footprint, where its roads start (DS:0382)
local ROUND_CITY = { [0] = { -1, -1 }, { 0, -1 }, { 1, -1 }, { 2, -1 }, { 2, 0 }, { 2, 1 }, { 2, 2 },
  { 1, 2 }, { 0, 2 }, { -1, 2 }, { -1, 1 }, { -1, 0 } }
-- the road builder's terrain costs, pseudo-player 14 (DS:01e0)
local ROAD_COST = { [0] = 1, 1, 3, 3, 4, 5, 7, 2, 5, 2, 1, 7 }
-- what the start menu shows beside each slider (4125:28e0, formats 4125:2918)
M.SLIDER_SHOWS = { [0] = { [0] = 5, 7, 9, 11, 13, 15, 17 }, { [0] = 5, 7, 9, 11, 13, 15, 17 },
  { [0] = 70, 75, 80, 85, 90, 95, 100 }, { [0] = 9, 11, 13, 15, 17, 19, 21 } }
M.SLIDER_FORMATS = { [0] = "(%d%%)", "(%d%%)", "(%d)", "(%d%%)" }
M.RANDOM_SLIDER = 7      -- a slider of 7 is rolled: 1d7-1

local FOOTPRINT = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }   -- DS:02c4, 02cc
local SAFETY = 200000    -- a loop the original might never leave

local function abs(v) return v < 0 and -v or v end
--- C's division, towards zero.
local function idiv(a, b)
  local q = a / b
  return q >= 0 and math.floor(q) or math.ceil(q)
end
--- C's remainder, with the sign of the dividend.
local function cmod(a, b) return a - b * idiv(a, b) end
local function max(a, b) return a > b and a or b end
local function min(a, b) return a < b and a or b end

--- Chebyshev distance (4c49:0fed).
local function reach(x1, y1, x2, y2) return max(abs(x1 - x2), abs(y1 - y2)) end

--- map_distance (1a8b:0acc): the straight line, rounded down.
local function distance(x1, y1, x2, y2)
  local dx, dy = x1 - x2, y1 - y2
  return math.floor(math.sqrt(dx * dx + dy * dy))
end

--- 4c49:0dd0: the step from one tile towards another, one of 8 compass
--- directions by the slope, cut at tan 22.5 and tan 67.5 (4125:0292).
local function step(x1, y1, x2, y2)
  if x2 == x1 then
    if y2 == y1 then return 0, 0 end
    return 0, y2 < y1 and -1 or 1
  end
  if y2 == y1 then return x2 < x1 and -1 or 1, 0 end
  local s = (y2 - y1) / (x2 - x1)
  if x2 < x1 then
    if s >= 2.414 then return 0, -1 end
    if s >= 0.414 then return -1, -1 end
    if s >= -0.414 then return -1, 0 end
    if s >= -2.414 then return -1, 1 end
    return 0, 1
  end
  if s >= 2.414 then return 0, 1 end
  if s >= 0.414 then return 1, 1 end
  if s >= -0.414 then return 1, 0 end
  if s >= -2.414 then return 1, -1 end
  return 0, -1
end

--- The direction from one tile to another for a signpost (828e:0b51).
local function compass(x1, y1, x2, y2)
  if x1 == x2 then return y1 < y2 and 4 or 0 end
  if y1 == y2 then return x1 < x2 and 2 or 6 end
  if x2 < x1 and y2 < y1 then return 7 end
  if x2 < x1 and y1 < y2 then return 5 end
  if x1 < x2 and y2 < y1 then return 1 end
  return 3
end

--- The checkerboard: 1 where x + y is even (C's (n)/2 != (n-1)/2).
local function parity(x, y)
  local n = x + y
  return (n == 0 or idiv(n, 2) ~= idiv(n - 1, 2)) and 1 or 0
end

local function clamp(p)                  -- 4bed:0470
  if p.x < 1 then p.x = 0 end
  if p.y < 1 then p.y = 0 end
  if p.x > W - 1 then p.x = W - 1 end
  if p.y > H - 1 then p.y = H - 1 end
end

local function i8(b) return b > 127 and b - 256 or b end
local BIT = { [0] = 1, 2, 4, 8, 16, 32, 64, 128 }

-- the low and high bytes of a 16-bit word, without bit operators
local function lo(v) return v % 256 end
local function hi(v) return v - v % 256 end

--- Bytes of a string as a 0-based table.
local function bytes(s)
  local t = {}
  for i = 1, #s do t[i - 1] = s:byte(i) end
  return t
end

--- A 0-based byte table, `n` long, as a string.
local function str(t, n)
  local parts, chunk = {}, {}
  for i = 0, n - 1 do
    chunk[#chunk + 1] = string.char(t[i] or 0)
    if #chunk == 4096 then parts[#parts + 1] = table.concat(chunk) chunk = {} end
  end
  parts[#parts + 1] = table.concat(chunk)
  return table.concat(parts)
end

local function cstr(t, o, n)
  local out = {}
  for i = o, o + n - 1 do
    local b = t[i]
    if b == nil or b == 0 then break end
    out[#out + 1] = string.char(b)
  end
  return table.concat(out)
end

local function getI16(t, o)
  local v = t[o] + t[o + 1] * 256
  return v >= 0x8000 and v - 0x10000 or v
end
local function setI16(t, o, v)
  v = v % 65536
  t[o] = v % 256
  t[o + 1] = (v - v % 256) / 256
end

local G = {}
G.__index = G

function M.newGenerator(dat, template, types, rng, opts)
  local self = setmetatable({}, G)
  self.m = {}
  for i = 0, DAT_SIZE - 1 do self.m[i] = dat[i] or 0 end
  self.scn = {}
  for i = 0, template.n - 1 do self.scn[i] = template[i] end
  self.scnSize = template.n
  self.types = types
  self.rng = rng
  self.allies = opts.allies and true or false
  self.tiles, self.rd = {}, {}
  for i = 0, W * H - 1 do self.tiles[i], self.rd[i] = 0, 0 end
  self.cty, self.spc = {}, {}
  self.siteCell = -1            -- 4125:02d4, where the sites' round begins
  return self
end

-- --- memory ----------------------------------------------------------------

function G:r16(o) return getI16(self.m, o) end
function G:w16(o, v) setI16(self.m, o, v) end
function G:str(o, n) return cstr(self.m, o, n) end
function G:s16(o) return getI16(self.scn, o) end
function G:ws16(o, v) setI16(self.scn, o, v) end
function G:wstr(o, s)                    -- strcpy into the scenario image
  for i = 1, #s do self.scn[o + i - 1] = s:byte(i) end
  self.scn[o + #s] = 0
end

--- The terrain grid. Off the grid a read gets whatever RANDOM.DAT holds
--- there, as the original's did.
function G:at(x, y)
  local o = GRID + y * W + x
  if o >= 0 and o < DAT_SIZE then return self.m[o] end
  return 0
end
function G:put(x, y, t)
  local i = y * W + x
  if i >= 0 and i < W * H then self.m[GRID + i] = t end
end
function G:nx(i) return self:r16(NB + 4 * i) end
function G:ny(i) return self:r16(NB + 4 * i + 2) end
--- The index of a step among the 8 neighbours, or -1.
function G:nbIndex(dx, dy)
  local k = -1
  for i = 0, 7 do if self:nx(i) == dx and self:ny(i) == dy then k = i end end
  return k
end

-- the tile map: the low byte is the tile, bit 15 a crossing
function G:tile(x, y)
  if x >= 0 and y >= 0 and x < W and y < H then return lo(self.tiles[y * W + x]) end
  return 0
end
function G:setTile(x, y, t)
  if x < 0 or y < 0 or x >= W or y >= H then return end
  local i = y * W + x
  self.tiles[i] = hi(self.tiles[i]) + t % 256
end
function G:crossingAt(i) return self.tiles[i] >= 0x8000 end
function G:setCrossing(i) if self.tiles[i] < 0x8000 then self.tiles[i] = self.tiles[i] + 0x8000 end end
function G:clearCrossing(i) if self.tiles[i] >= 0x8000 then self.tiles[i] = self.tiles[i] - 0x8000 end end
function G:road(x, y)
  if x >= 0 and y >= 0 and x < W and y < H then return self.rd[y * W + x] % 32 end
  return 0
end
function G:setRoad(x, y, r)
  if x < 0 or y < 0 or x >= W or y >= H then return end
  local i = y * W + x
  self.rd[i] = self.rd[i] - self.rd[i] % 32 + r % 32
end
function G:clearRoad(i) self.rd[i] = self.rd[i] - self.rd[i] % 32 end
function G:terrainOf(t) return self.scn[S_TERRAIN + t] end
--- Variant `v` of terrain type `t` from RANDOM.DAT's tile table, the
--- checkerboard picking one of its pair.
function G:tileFor(t, v, x, y) return lo(self:r16(TILES + 64 * t + 4 * v + 2 * parity(x, y)) % 65536) end

-- the cities, as the scenario holds them
function G:cityCount() return self:s16(S_CITY_COUNT) end
function G:setCityCount(n) self:ws16(S_CITY_COUNT, n) end
function G:crec(i) return S_CITIES + CITY_STRIDE * i end
function G:cx(i) return self:s16(self:crec(i)) end
function G:cy(i) return self:s16(self:crec(i) + 2) end

--- dice(n, sides, bonus) (6ecb:02bf): no sides gives the bonus, and the
--- clamp makes fewer than none give n + bonus.
function G:dice(n, sides, bonus)
  bonus = bonus or 0
  if sides == 0 then return bonus end
  if sides < 0 then return n + bonus end
  return self.rng:dice(n, sides, bonus)
end

--- terrain_near (4c49:11f5): is type `t` on any of the 8 tiles round.
function G:near(x, y, t)
  for i = 0, 7 do
    local ax, ay = x + self:nx(i), y + self:ny(i)
    if ax >= 0 and ay >= 0 and ax < W and ay < H and self:at(ax, ay) == t then return true end
  end
  return false
end

--- 4c49:117d: plain on any of the 8 tiles round.
function G:nearPlain(x, y) return self:near(x, y, PLAIN) end

--- 4bed:04ae: push a point by (1d3-2) x 1dn each way; with `toEdge`, 30%
--- of the time it then goes to the nearest edge. Clamped to the map.
function G:jitter(p, n, toEdge)
  local s1 = self:dice(1, 3, -2)
  p.x = p.x + s1 * self:dice(1, n, 0)
  local s2 = self:dice(1, 3, -2)
  p.y = p.y + s2 * self:dice(1, n, 0)
  if self:dice(1, 100, 0) < 30 and toEdge then
    local l, t, r, b = p.x, p.y, W - 1 - p.x, H - 1 - p.y
    if l <= t and l <= r and l <= b then p.x = 0 end
    if t <= l and t <= r and t <= b then p.y = 0 end
    if r <= t and r <= l and r <= b then p.x = W - 1 end
    if b <= t and b <= l and b <= r then p.y = H - 1 end
  end
  clamp(p)
end

-- --- 4c49: the coastline ----------------------------------------------------

function G:pt(o) return { x = self:r16(o), y = self:r16(o + 2) } end
function G:setPt(o, p) self:w16(o, p.x) self:w16(o + 2, p.y) end

--- 4c49:0087: where edge `i`'s stretch of coast begins and ends -- the
--- map's corners drawn in 10 along the diagonal, then pushed about.
function G:coastEnds(i)
  local j = (i + 1) % 4
  if self:r16(P_EDGE_POINTS + 2 * i) < 3 then
    self:setPt(EDGE_START + 4 * i, self:pt(EDGES + 8 * i))
    self:setPt(EDGE_END + 4 * i, self:pt(EDGES + 8 * i + 4))
    return
  end
  local a = self:pt(EDGES + 8 * i)
  a.x = a.x - 10 * self:r16(DIAGONALS + 4 * i)
  a.y = a.y - 10 * self:r16(DIAGONALS + 4 * i + 2)
  self:jitter(a, 16, true)
  self:put(a.x, a.y, PLAIN)
  self:setPt(EDGE_START + 4 * i, a)
  local b = self:pt(EDGES + 8 * i + 4)
  b.x = b.x - 10 * self:r16(DIAGONALS + 4 * j)
  b.y = b.y - 10 * self:r16(DIAGONALS + 4 * j + 2)
  self:jitter(b, 16, true)
  self:put(b.x, b.y, PLAIN)
  self:setPt(EDGE_END + 4 * i, b)
end

--- 4c49:0206: swap an edge's end with the next one's start where that
--- keeps the outline turning round the middle.
function G:coastOrder()
  for i = 0, 3 do
    local j = (i + 1) % 4
    local e, s = self:pt(EDGE_END + 4 * i), self:pt(EDGE_START + 4 * j)
    if (56 - e.x) / (78 - e.y) < (56 - s.x) / (78 - s.y) then
      self:setPt(EDGE_END + 4 * i, s)
      self:setPt(EDGE_START + 4 * j, e)
    end
  end
end

local function onEdge(p) return p.x == 0 or p.y == 0 or p.x == W - 1 or p.y == H - 1 end

--- 4c49:02f9: the points along edge `i`, each a step on from the last
--- towards the end and pushed about by up to half a step.
function G:coastPoints(i)
  local base, want = EDGE_PTS + 0x50 * i, self:r16(P_EDGE_POINTS + 2 * i)
  local s, e = self:pt(EDGE_START + 4 * i), self:pt(EDGE_END + 4 * i)
  self:setPt(base, s)
  local n = 1
  if want < 3 or (onEdge(s) and onEdge(e)) then
    self:setPt(base + 4, e)
    self:w16(EDGE_COUNT + 2 * i, 2)
    return
  end
  local len = idiv(reach(e.x, e.y, s.x, s.y), want - 1)
  local k = 0
  while true do
    local p = self:pt(base + 4 * k)
    local dx, dy = step(p.x, p.y, e.x, e.y)
    local q = { x = p.x + dx * len, y = p.y + dy * len }
    self:jitter(q, idiv(len, 2), false)
    k = k + 1
    self:put(q.x, q.y, PLAIN)
    self:setPt(base + 4 * k, q)
    n = n + 1
    if reach(e.x, e.y, q.x, q.y) < len or n >= want - 1 then break end
  end
  self:setPt(base + 4 * (k + 1), e)
  self:w16(EDGE_COUNT + 2 * i, n + 1)
end

--- 4c49:104a: turn a step by -1 to +3 eighths.
function G:turn(dx, dy)
  local k = self:nbIndex(dx, dy)
  local i = cmod(self:dice(1, 5, (k < 0 and 8 or k) - 2), 8)
  return self:nx(i), self:ny(i)
end

--- 4c49:0a0e: a wandering line of plain from a to b.
function G:coastLine(a, b)
  local cur = { x = a.x, y = a.y }
  local fx, fy = step(a.x, a.y, b.x, b.y)
  for _ = 1, SAFETY do
    local dx, dy = step(cur.x, cur.y, b.x, b.y)
    local r = reach(cur.x, cur.y, b.x, b.y) < 3 and 1 or self:dice(1, 3, 0)
    if r == 2 then dx, dy = fx, fy
    elseif r == 3 then dx, dy = self:turn(dx, dy) fx, fy = dx, dy end
    cur.x, cur.y = cur.x + dx, cur.y + dy
    clamp(cur)
    self:put(cur.x, cur.y, PLAIN)
    if cur.x == b.x and cur.y == b.y then return end
  end
end

--- 4c49:1016: which edge a point lies on, -1 for none.
local function edgeOf(p)
  if p.x == 0 then return 3 end
  if p.y == 0 then return 0 end
  if p.x == W - 1 then return 1 end
  if p.y == H - 1 then return 2 end
  return -1
end

--- A straight run of plain from a to b, one step each.
function G:straight(a, b)
  local dx, dy = step(a.x, a.y, b.x, b.y)
  local cur = { x = a.x, y = a.y }
  local n = 0
  while n < SAFETY and not (cur.x == b.x and cur.y == b.y) do
    cur.x, cur.y = cur.x + dx, cur.y + dy
    clamp(cur)
    self:put(cur.x, cur.y, PLAIN)
    n = n + 1
  end
end

--- 4c49:0b0e: two points on the map's edge are joined along it, round a
--- corner if need be; across the map they wander as any other.
function G:coastAlongEdge(a, b)
  local ea, eb = edgeOf(a), edgeOf(b)
  if ea == eb then return self:straight(a, b) end
  local apart = abs(ea - eb)
  if apart == 2 then return self:coastLine(a, b) end
  local corner = self:pt(CORNERS + 4 * (apart == 3 and 0 or max(ea, eb)))
  self:straight(a, corner)
  self:straight(b, corner)
end

function G:coastSegment(a, b)
  if onEdge(a) and onEdge(b) then self:coastAlongEdge(a, b)
  else self:coastLine(a, b) end
end

--- 4c49:05c7: join every point of the outline to the next.
function G:coastJoin()
  for i = 0, 3 do
    local base, n = EDGE_PTS + 0x50 * i, self:r16(EDGE_COUNT + 2 * i)
    local k = 0
    while k < n - 1 do
      self:coastSegment(self:pt(base + 4 * k), self:pt(base + 4 * (k + 1)))
      k = k + 1
    end
    local j = (i + 1) % 4
    self:coastSegment(self:pt(base + 4 * k), self:pt(EDGE_PTS + 0x50 * j))
  end
end

--- 4c49:10bd: may the sea flood into this tile?
function G:floods(x, y)
  local t = self:at(x, y)
  if t == SHORE or t == BRIDGE then return false end
  local inside = x ~= 0 and y ~= 0 and x ~= W - 1 and y ~= H - 1
  if not inside and t == 0 then return true end
  return self:at(x + 1, y) == WATER or self:at(x - 1, y) == WATER
    or self:at(x, y + 1) == WATER or self:at(x, y - 1) == WATER
end

--- 4c49:0867: the sea floods in from the edges up to the outline; what it
--- does not reach is land.
function G:coastFill()
  local m = self.m
  for i = 0, W * H - 1 do m[GRID + i] = m[GRID + i] == PLAIN and SHORE or 0 end
  local function flood(x, y) if self:floods(x, y) then self:put(x, y, WATER) end end
  for _ = 1, 2 do
    for x = 0, W - 1 do for y = 0, H - 1 do flood(x, y) end end
    for y = 0, H - 1 do for x = 0, W - 1 do flood(x, y) end end
    for x = W - 1, 0, -1 do for y = H - 1, 0, -1 do flood(x, y) end end
    for y = H - 1, 0, -1 do for x = W - 1, 0, -1 do flood(x, y) end end
  end
  for i = 0, W * H - 1 do m[GRID + i] = m[GRID + i] == WATER and WATER or PLAIN end
end

--- 4c49:0ce7: water beside land is shore.
function G:shores()
  for x = 0, W - 1 do
    for y = 0, H - 1 do
      if self:at(x, y) == WATER then
        if self:nearPlain(x, y) then self:put(x, y, SHORE) end
        if self:near(x, y, HILLS) then self:put(x, y, SHORE) end
        if self:near(x, y, MOUNTAINS) then self:put(x, y, SHORE) end
      end
    end
  end
end

--- 4c49:0000: the land.
function G:coast()
  for i = 0, 3 do self:coastEnds(i) end
  self:coastOrder()
  for i = 0, 3 do self:coastPoints(i) end
  self:coastJoin()
  self:coastFill()
  self:shores()
  if self:dice(1, 100, 0) < 50 then
    -- 4c49:0daa: 0-2 channels straight across, west to east
    local n = self:dice(1, 3, -1)
    for _ = 1, n do self:river(true, false, true) end
    self:waterTidy()
  end
  self:shores()
end

-- --- 4d71: mountains and hills ------------------------------------------------

function G:node(i) return NODES + 14 * i end

--- 4d71:0495 / 03af: seeds on random plain tiles, each a node.
function G:seeds(n, t, kind)
  for _ = 1, n do
    local x, y = 0, 0
    for _ = 1, SAFETY do
      x = self:dice(1, W, -1)
      y = self:dice(1, H, -1)
      if self:at(x, y) == PLAIN then break end
    end
    self:put(x, y, t)
    local c = self:r16(NODE_COUNT)
    local o = self:node(c)
    self:w16(o, x) self:w16(o + 2, y) self:w16(o + 4, kind) self:w16(o + 6, 0)
    self:w16(NODE_COUNT, c + 1)
  end
end

--- 4d71:057b: each node is linked to its nearest others -- a hill to 0-1,
--- a mountain to 0-3.
function G:link()
  local count = self:r16(NODE_COUNT)
  if count < 2 then return end
  for i = 0, count - 1 do
    local o = self:node(i)
    local n = self:r16(o + 4) == 1 and self:dice(1, 2, -1) or self:dice(1, 4, -1)
    if n >= count - 1 then n = count - 1 end
    self:w16(o + 6, n)
    for l = 0, n - 1 do
      local best, bestJ = 10000, -1
      for j = 0, count - 1 do
        if j ~= i then
          local k = 0
          while k < l and self:r16(o + 8 + 2 * k) ~= j do k = k + 1 end
          if k == l then
            local p = self:node(j)
            local d = reach(self:r16(p), self:r16(p + 2), self:r16(o), self:r16(o + 2))
            if d < best then best, bestJ = d, j end
          end
        end
      end
      self:w16(o + 8 + 2 * l, bestJ)
    end
  end
end

--- 4d71:09a7: a ridge from a to b, wandering as the coast does; `width` 2
--- adds a tile to either side, 3 one two out. Only plain and hills give
--- way to it.
function G:ridge(a, b, width, t)
  local cur = { x = a.x, y = a.y }
  local fx, fy = step(a.x, a.y, b.x, b.y)
  local k = 0
  local function lay(x, y)
    local v = self:at(x, y)
    if v == PLAIN or v == HILLS then self:put(x, y, t) end
  end
  for _ = 1, SAFETY do
    local dx, dy = step(cur.x, cur.y, b.x, b.y)
    local r = reach(cur.x, cur.y, b.x, b.y) < 3 and 1 or self:dice(1, 3, 0)
    if r == 2 then dx, dy = fx, fy
    elseif r == 3 then dx, dy = self:turn(dx, dy) fx, fy = dx, dy end
    cur.x, cur.y = cur.x + dx, cur.y + dy
    local ki = self:nbIndex(dx, dy)
    if ki >= 0 then k = ki end
    local l, rr = (k + 2) % 8, (k + 6) % 8
    local p1 = { x = cur.x + self:nx(l), y = cur.y + self:ny(l) }
    local p2 = { x = cur.x + self:nx(rr), y = cur.y + self:ny(rr) }
    local p3 = { x = cur.x + 2 * self:nx(l), y = cur.y + 2 * self:ny(l) }
    local p4 = { x = cur.x + 2 * self:nx(rr), y = cur.y + 2 * self:ny(rr) }
    clamp(cur)
    lay(cur.x, cur.y)
    local done = cur.x == b.x and cur.y == b.y
    if width == 2 then
      clamp(p1)
      lay(p1.x, p1.y)
      lay(p2.x, p2.y)             -- the original clamps only the one
    end
    if width == 3 then
      clamp(p3)
      lay(p3.x, p3.y)
      lay(p4.x, p4.y)
    end
    if done then return end
  end
end

--- 4d71:0884 / 08f3: a node with no links gets a short ridge of its own,
--- up and to the left.
function G:blob(p, t)
  local q = { x = p.x + self:dice(1, 5, -10), y = 0 }
  q.y = p.y + self:dice(1, 5, -10)
  clamp(q)
  self:ridge(p, q, self:dice(1, 2, 1), t)
end

--- 4d71:06d2: the ridges along the links.
function G:ridges()
  local count = self:r16(NODE_COUNT)
  if count < 2 then return end
  for i = 0, count - 1 do
    local o = self:node(i)
    local p = { x = self:r16(o), y = self:r16(o + 2) }
    local hill, links = self:r16(o + 4) == 1, self:r16(o + 6)
    if links == 0 then
      self:blob(p, hill and HILLS or MOUNTAINS)
    else
      for l = 0, links - 1 do
        local q = self:node(self:r16(o + 8 + 2 * l))
        local to = { x = self:r16(q), y = self:r16(q + 2) }
        if hill and self:r16(q + 4) == 1 then self:ridge(p, to, 3, HILLS)
        else self:ridge(p, to, self:dice(1, 2, 1), MOUNTAINS) end
      end
    end
  end
end

--- 4d71:002e: plain hemmed in by mountains (or hills) on all four sides
--- joins them, mountains on the shore come down, and a diagonal run of
--- three mountains is thickened on one side.
function G:mountainTidy()
  for x = 0, W - 1 do
    for y = 0, H - 1 do
      if x ~= 0 and y ~= 0 and x ~= W - 1 and y ~= H - 1 and self:at(x, y) == PLAIN then
        for _, t in ipairs({ MOUNTAINS, HILLS }) do
          if self:at(x, y + 1) == t and self:at(x + 1, y) == t
             and self:at(x - 1, y) == t and self:at(x, y - 1) == t then self:put(x, y, t) end
        end
      end
      if self:at(x, y) == MOUNTAINS and self:near(x, y, SHORE) then self:put(x, y, PLAIN) end
    end
  end
  for x = 1, W - 2 do
    for y = 1, H - 2 do
      if self:at(x, y) == MOUNTAINS then
        local function m(dx, dy) return self:at(x + dx, y + dy) == MOUNTAINS end
        if m(1, 1) and m(-1, -1) and not m(1, -1) and not m(-1, 1) then
          if self:dice(1, 10, 0) > 5 then self:put(x - 1, y + 1, MOUNTAINS)
          else self:put(x + 1, y - 1, MOUNTAINS) end
        elseif m(-1, 1) and m(1, -1) and not m(-1, -1) and not m(1, 1) then
          if self:dice(1, 10, 0) > 5 then self:put(x + 1, y + 1, MOUNTAINS)
          else self:put(x - 1, y - 1, MOUNTAINS) end
        end
      end
    end
  end
end

--- 4d71:033a: mountains are ringed with hills.
function G:foothills()
  for x = 0, W - 1 do
    for y = 0, H - 1 do
      local t = self:at(x, y)
      if t ~= MOUNTAINS and t ~= HILLS and self:near(x, y, MOUNTAINS) then self:put(x, y, HILLS) end
    end
  end
end

--- 4d71:0000.
function G:highlands()
  self:w16(NODE_COUNT, 0)
  self:seeds(self:r16(P_MOUNTAINS), MOUNTAINS, 2)
  self:seeds(self:r16(P_HILLS), HILLS, 1)
  self:link()
  self:ridges()
  self:mountainTidy()
  self:foothills()
end

-- --- 4f5f: erosion and passes --------------------------------------------------

--- 4f5f:0044: a walk between two random tiles; erosion wears its
--- mountains to hills, a pass cuts mountains and hills to plain, five tiles
--- across.
function G:wear(pass)
  local a = { x = self:dice(1, W, -1), y = 0 }
  a.y = self:dice(1, H, -1)
  local b = { x = self:dice(1, W, -1), y = 0 }
  b.y = self:dice(1, H, -1)
  local cur = { x = a.x, y = a.y }
  while not (cur.x == b.x and cur.y == b.y) do
    local dx, dy = step(cur.x, cur.y, b.x, b.y)
    local k = self:nbIndex(dx, dy)
    local l, r = (k + 1) % 8, (k + 7) % 8
    local t = self:at(cur.x, cur.y)
    local hit
    if pass then hit = t == MOUNTAINS or t == HILLS else hit = t == MOUNTAINS end
    if hit then
      local to = pass and PLAIN or HILLS
      for _, o in ipairs({ { 0, 0 }, { self:nx(l), self:ny(l) }, { self:nx(r), self:ny(r) },
                           { 2 * self:nx(l), 2 * self:ny(l) }, { 2 * self:nx(r), 2 * self:ny(r) } }) do
        local p = { x = cur.x + o[1], y = cur.y + o[2] }
        clamp(p)
        self:put(p.x, p.y, to)
      end
    end
    cur.x, cur.y = cur.x + dx, cur.y + dy
  end
end

--- Hills with three mountains on their four sides become mountain
--- (4f5f:04d6); plain with three hills, hills (05ae).
function G:surrounded(t, by)
  for x = 1, W - 2 do
    for y = 1, H - 2 do
      if self:at(x, y) == t then
        local n = 0
        if self:at(x, y + 1) == by then n = n + 1 end
        if self:at(x, y - 1) == by then n = n + 1 end
        if self:at(x + 1, y) == by then n = n + 1 end
        if self:at(x - 1, y) == by then n = n + 1 end
        if n > 2 then self:put(x, y, by) end
      end
    end
  end
end

--- 4f5f:0000.
function G:erosion()
  local i = 0
  while i < self:r16(P_EROSION) do self:wear(false) i = i + 1 end
  i = 0
  while i < self:r16(P_PASSES) do self:wear(true) i = i + 1 end
  self:surrounded(HILLS, MOUNTAINS)
  self:surrounded(PLAIN, HILLS)
  self:foothills()
end

-- --- 4eb7: rivers ---------------------------------------------------------------

--- 4eb7:005d: a river. Without `channel` it runs from a random hill or
--- mountain to a random water or shore tile, 200 tries each; a channel
--- runs from the west edge to the east. Each step lays water three tiles
--- across (`wide`: five, a channel seven), turns aside a quarter of the
--- time each way, and with `stops` the river ends on meeting water the
--- second time.
function G:river(wide, stops, channel)
  local a, b
  if channel then
    a = { x = 0, y = self:dice(1, H, -1) }
    b = { x = W - 1, y = self:dice(1, H, -1) }
  else
    a, b = { x = 0, y = 0 }, { x = 0, y = 0 }
    for _ = 1, 200 do
      a = { x = self:dice(1, W, -1), y = 0 }
      a.y = self:dice(1, H, -1)
      local t = self:at(a.x, a.y)
      if t == HILLS or t == MOUNTAINS then break end
    end
    for _ = 1, 200 do
      b = { x = self:dice(1, W, -1), y = 0 }
      b.y = self:dice(1, H, -1)
      local t = self:at(b.x, b.y)
      if t == WATER or t == SHORE then break end
    end
  end
  local cur = { x = a.x, y = a.y }
  local met = 0
  local done = cur.x == b.x and cur.y == b.y
  local n = 0
  while not done and n < SAFETY do
    n = n + 1
    local dx, dy = step(cur.x, cur.y, b.x, b.y)
    local k = self:nbIndex(dx, dy)
    local l, r = (k + 1) % 8, (k + 7) % 8
    local aside1, aside2 = (k + 2) % 8, (k + 6) % 8
    if stops then
      -- nb(-1) reads the word before the table, as the original's did
      local ahead = self:at(cur.x + self:nx(k), cur.y + self:ny(k))
      if ahead == WATER and met == 1 then done = true end
      if ahead == WATER and met == 0 then met = met + 1 end
    end
    local function lay(ox, oy)
      local p = { x = cur.x + ox, y = cur.y + oy }
      clamp(p)
      self:put(p.x, p.y, WATER)
    end
    lay(0, 0)
    lay(self:nx(l), self:ny(l))
    lay(self:nx(r), self:ny(r))
    if wide then
      lay(2 * self:nx(l), 2 * self:ny(l))
      lay(2 * self:nx(r), 2 * self:ny(r))
      if channel then
        lay(3 * self:nx(l), 3 * self:ny(l))
        lay(3 * self:nx(r), 3 * self:ny(r))
      end
    end
    cur.x, cur.y = cur.x + dx, cur.y + dy
    if cur.x == b.x and cur.y == b.y then done = true end
    local roll = self:dice(1, 4, 0)
    if roll == 2 then cur.x, cur.y = cur.x + self:nx(aside1), cur.y + self:ny(aside1)
    elseif roll == 3 then cur.x, cur.y = cur.x + self:nx(aside2), cur.y + self:ny(aside2) end
  end
end

local function wet(t) return t == SHORE or t == WATER end

--- 4eb7:060c: land with water on three of its four sides goes under, shore
--- out of sight of land becomes open water, and a diagonal of water is
--- given a shore tile beside it.
function G:waterTidy()
  local function land(t) return t == PLAIN or t == HILLS or t == MOUNTAINS end
  for x = 0, W - 1 do
    for y = 0, H - 1 do
      if land(self:at(x, y)) then
        local n = 0
        if wet(self:at(x, y + 1)) then n = n + 1 end
        if wet(self:at(x, y - 1)) then n = n + 1 end
        if wet(self:at(x + 1, y)) then n = n + 1 end
        if wet(self:at(x - 1, y)) then n = n + 1 end
        if n > 2 then self:put(x, y, WATER) end
      end
    end
  end
  for x = 0, W - 1 do
    for y = 0, H - 1 do
      if self:at(x, y) == SHORE and not self:near(x, y, PLAIN) and not self:near(x, y, HILLS)
         and not self:near(x, y, MOUNTAINS) then self:put(x, y, WATER) end
    end
  end
  local function dry(t) return t == HILLS or t == FOREST or t == PLAIN end
  for x = 1, W - 2 do
    for y = 1, H - 2 do
      if wet(self:at(x, y)) then
        local function at(dx, dy) return self:at(x + dx, y + dy) end
        if wet(at(1, 1)) and wet(at(-1, -1)) and dry(at(1, -1)) and dry(at(-1, 1)) then
          if self:dice(1, 10, 0) > 4 then self:put(x - 1, y + 1, SHORE)
          else self:put(x + 1, y - 1, SHORE) end
        elseif wet(at(-1, 1)) and wet(at(1, -1)) and dry(at(-1, -1)) and dry(at(1, 1)) then
          if self:dice(1, 10, 0) > 4 then self:put(x + 1, y + 1, SHORE)
          else self:put(x - 1, y - 1, SHORE) end
        end
      end
    end
  end
end

--- 4eb7:0000.
function G:rivers()
  local i = 0
  while i < self:r16(P_RIVERS) do self:river(false, true, false) i = i + 1 end
  i = 0
  while i < self:r16(P_WIDE_RIVERS) do self:river(true, true, false) i = i + 1 end
  self:waterTidy()
  self:shores()
  self:mountainTidy()
  self:foothills()
  self:waterTidy()
  self:shores()
end

-- --- 4e47: forests ----------------------------------------------------------------

function G:addForest() self:w16(FOREST_DONE, self:r16(FOREST_DONE) + 1) end

--- 4e47:00b5: a wood round a random plain tile: eight arms, and off each
--- step of an arm two side shoots that shorten as it goes.
function G:wood()
  local x, y = 0, 0
  for _ = 1, SAFETY do
    x = self:dice(1, W, -1)
    y = self:dice(1, H, -1)
    if self:at(x, y) == PLAIN then break end
  end
  local arms = {}
  local span, base
  if self:dice(1, 100, 0) < 65 then
    for i = 0, 7 do arms[i] = self:dice(1, 8, 2) end
    span, base = 4, 2
  else
    for i = 0, 7 do arms[i] = self:dice(1, 10, 5) end
    span, base = 6, 4
  end
  local function grow(p)
    clamp(p)
    if self:at(p.x, p.y) == PLAIN then
      self:put(p.x, p.y, FOREST)
      self:addForest()
    end
  end
  self:put(x, y, FOREST)
  self:addForest()
  for i = 0, 7 do
    local cur = { x = x, y = y }
    local k = 0
    for _ = 1, arms[i] do
      cur.x, cur.y = cur.x + self:nx(i), cur.y + self:ny(i)
      grow(cur)
      local l, r = (i + 2) % 8, (i + 6) % 8
      k = k + 1
      local nl = self:dice(1, span - idiv(k, 2), base - idiv(k, 4))
      local nr = self:dice(1, span - idiv(k, 2), base - idiv(k, 3))
      local p = { x = cur.x, y = cur.y }
      for _ = 1, nl do p.x, p.y = p.x + self:nx(l), p.y + self:ny(l) grow(p) end
      local q = { x = cur.x, y = cur.y }
      for _ = 1, nr do q.x, q.y = q.x + self:nx(r), q.y + self:ny(r) grow(q) end
    end
  end
end

--- 4e47:03d9: plain with forest on three sides is forest, and a diagonal
--- of forest is given a tile beside it.
function G:forestTidy()
  for x = 0, W - 1 do
    for y = 0, H - 1 do
      if self:at(x, y) == PLAIN then
        local n = 0
        if self:at(x, y + 1) == FOREST then n = n + 1 end
        if self:at(x, y - 1) == FOREST then n = n + 1 end
        if self:at(x + 1, y) == FOREST then n = n + 1 end
        if self:at(x - 1, y) == FOREST then n = n + 1 end
        if n > 2 then
          self:put(x, y, FOREST)
          self:addForest()
        end
      end
    end
  end
  local function open(t) return t == HILLS or t == SHORE or t == PLAIN end
  for x = 1, W - 2 do
    for y = 1, H - 2 do
      if self:at(x, y) == FOREST then
        local function at(dx, dy) return self:at(x + dx, y + dy) end
        if at(1, 1) == FOREST and at(-1, -1) == FOREST and open(at(1, -1)) and open(at(-1, 1)) then
          if self:dice(1, 10, 0) < 5 then self:put(x + 1, y - 1, FOREST)
          else self:put(x - 1, y + 1, FOREST) end
        elseif at(-1, 1) == FOREST and at(1, -1) == FOREST and open(at(-1, -1)) and open(at(1, 1)) then
          if self:dice(1, 10, 0) < 5 then self:put(x - 1, y - 1, FOREST)
          else self:put(x + 1, y + 1, FOREST) end
        end
      end
    end
  end
end

--- 4e47:0000: woods until they cover land / 100 x the Forest parameter.
function G:forests()
  local land = 0
  for i = 0, W * H - 1 do
    local t = self.m[GRID + i]
    if t == PLAIN or t == HILLS or t == MOUNTAINS then land = land + 1 end
  end
  self:w16(FOREST_WANTED, idiv(land, 100) * self:r16(P_FOREST))
  self:w16(FOREST_DONE, 0)
  local n = 0
  while n < 10000 and self:r16(FOREST_DONE) < self:r16(FOREST_WANTED) do
    self:wood()
    self:forestTidy()
    n = n + 1
  end
end

-- --- 4fc9: marshes ------------------------------------------------------------------

--- 4fc9:0029: 1d5+3 tiles of marsh scattered round (x, y).
function G:marshAround(x, y)
  local n = self:dice(1, 5, 3)
  for _ = 1, n do
    local r1 = self:dice(1, 3, 0)
    local p = { x = x + r1 * self:nx(self:dice(1, 8, -1)), y = 0 }
    local r2 = self:dice(1, 3, 0)
    p.y = y + r2 * self:ny(self:dice(1, 8, -1))
    clamp(p)
    if self:at(p.x, p.y) == PLAIN then self:put(p.x, p.y, MARSH) end
  end
end

--- 4fc9:010a: a marsh, near the shore if one of four tries finds it.
function G:marsh()
  local x, y, tries = 0, 0, 0
  for _ = 1, SAFETY do
    x = self:dice(1, 102, 5)
    y = self:dice(1, 146, 5)
    if self:at(x, y) == PLAIN then
      if tries >= 4 then break end
      tries = tries + 1
      if self:near(x, y, SHORE) or self:near(x + 1, y, SHORE)
         or self:near(x, y + 1, SHORE) or self:near(x + 1, y + 1, SHORE) then break end
    end
  end
  self:put(x, y, MARSH)
  self:marshAround(x, y)
  self:marshAround(x + 1, y + 1)
  self:marshAround(x - 1, y - 1)
  self:marshAround(x - 1, y + 1)
  self:marshAround(x + 1, y - 1)
end

--- 4f5f:06a0: 1d3 marshes.
function G:marshes()
  local n = self:dice(1, 3, 0)
  for _ = 1, n do self:marsh() end
end

-- --- 513d: cities and sites -------------------------------------------------------

--- 513d:0331: a city's 2x2 footprint, clear of water, hills and other
--- cities by a tile; on the coast if one of four tries finds it.
function G:placeCity()
  local x, y, tries = 0, 0, 0
  for _ = 1, SAFETY do
    x = self:dice(1, 102, 5)
    y = self:dice(1, 146, 5)
    local ok = true
    for _, f in ipairs(FOOTPRINT) do
      local dx, dy = f[1], f[2]
      local t = self:at(x + dx, y + dy)
      if t == SHORE or t == WATER or t == CITY or t == MOUNTAINS or t == HILLS then ok = false end
      if self:near(x + dx, y + dy, CITY) or self:near(x + dx + 1, y + dy + 1, CITY)
         or self:near(x + dx - 1, y + dy - 1, CITY) then ok = false end
    end
    if ok then
      if tries >= 4 then break end
      tries = tries + 1
      if self:near(x, y, SHORE) or self:near(x + 1, y, SHORE)
         or self:near(x, y + 1, SHORE) or self:near(x + 1, y + 1, SHORE) then break end
    end
  end
  local c = self:cityCount()
  self:ws16(self:crec(c), x)
  self:ws16(self:crec(c) + 2, y)
  self:setCityCount(c + 1)
  for _, f in ipairs(FOOTPRINT) do self:put(x + f[1], y + f[2], CITY) end
end

--- 513d:0000.
function G:cities()
  self:setCityCount(0)
  local i = self:r16(0xa84)
  while i < self:r16(P_CITIES) do self:placeCity() i = i + 1 end
end

--- 513d:0a77: somewhere for a site. The map is taken a sixteenth at a time,
--- round in turn: 50 tries there for plain clear of cities and of other
--- sites by three, then 50 anywhere, then 50 for anything not a city.
function G:siteSpot()
  if self.siteCell == -1 then self.siteCell = self:dice(1, 16, -1) end
  local cx, cy = self.siteCell % 4, idiv(self.siteCell, 4)
  local x, y, found = 0, 0, false
  local i = 0
  while i < 50 and not found do
    x = self:dice(1, 20, idiv(cx * W, 4) + 3)
    y = self:dice(1, 31, idiv(cy * H, 4) + 3)
    found = self:at(x, y) == PLAIN and not self:near(x, y, CITY) and not self:near(x, y, SITE)
      and not self:near(x - 3, y, SITE) and not self:near(x + 3, y, SITE)
      and not self:near(x, y - 3, SITE) and not self:near(x, y + 3, SITE)
    i = i + 1
  end
  i = 0
  while i < 50 and not found do
    x = self:dice(1, 102, 5)
    y = self:dice(1, 146, 5)
    found = self:at(x, y) == PLAIN and not self:near(x, y, CITY) and not self:near(x, y, SITE)
    i = i + 1
  end
  if not found then
    x = self:dice(1, 102, 5)
    y = self:dice(1, 146, 5)
    i = 0
    while i < 50 and not found do
      x = self:dice(1, 102, 5)
      y = self:dice(1, 146, 5)
      found = self:at(x, y) ~= CITY and self:at(x, y) ~= SITE
      i = i + 1
    end
  end
  self.siteCell = (self.siteCell + 1) % 16
  return x, y
end

--- 513d:003a: forty sites, keeping the template's temples and ruins, with
--- names and descriptions from RANDOM.DAT's word lists.
function G:sites()
  self:ws16(S_SITE_COUNT, N_SITES)
  for i = 0, N_SITES - 1 do
    self:ws16(S_SITES + SITE_STRIDE * i + 2, -1)
    self:ws16(S_SITES + SITE_STRIDE * i, -1)
  end
  local tenth = 0
  for i = 0, N_SITES - 1 do
    local now = idiv(i * 10, N_SITES)
    if now ~= tenth then tenth = now coroutine.yield(70 + now) end
    local x, y = self:siteSpot()
    local o = S_SITES + SITE_STRIDE * i
    self:ws16(o, x)
    self:ws16(o + 2, y)
    self:put(x, y, SITE)
    local name, text
    if self.scn[o + 24] == 1 then
      name = ("%s Temple"):format(self:str(0x1000 + 10 * self:dice(1, 10, -1), 10))
      text = "#%03d|The %s can bless your|armies or give you|quests|\n"
    else
      local f = self:dice(1, self:r16(SITE_FORMATS), -1)
      local ending = self:str(0xf9c + 10 * self:dice(1, 10, -1), 10)
      local word = self:str(0xf38 + 10 * self:dice(1, 10, -1), 10) .. ending
      name = self:str(SITE_FORMATS + 2 + 20 * f, 20):format(word)
      text = "#%03d|%s is|inhabited by monsters and|full of treasure!|\n"
    end
    self:wstr(o + 4, name)
    self.spc[#self.spc + 1] = text:format(i, name)
  end
end

-- --- 4fef: terrain to tiles -----------------------------------------------------

--- 4fef:005c: every terrain type its first tile, a city its castle and the
--- first eight sites their own.
function G:toTiles()
  for y = 0, H - 1 do
    for x = 0, W - 1 do
      local t = self:at(x, y)
      if t == CITY and self:at(x, y - 1) ~= CITY and self:at(x - 1, y) ~= CITY and x > 0 and y > 0 then
        local c = lo(self:r16(TILES + 64 * CITY) % 65536)
        self:setTile(x, y, c) self:setTile(x + 1, y, c + 1)
        self:setTile(x, y + 1, c + 16) self:setTile(x + 1, y + 1, c + 17)
      elseif t == SITE then
        self:setTile(x, y, self:tileFor(SITE, 0, x, y))
      elseif t ~= CITY then
        self:setTile(x, y, self:tileFor(t, 0, x, y))
      end
    end
  end
  for i = 0, 7 do
    local o = S_SITES + SITE_STRIDE * i
    self:setTile(self:s16(o), self:s16(o + 2), lo(self:r16(TILES + 64 * SITE + 4) % 65536))
  end
end

--- The 8 neighbours as a mask (bit i for neighbour i), counting the map's
--- edge in: 4fef:0c9d (water, shore, bridge), 0d4f (forest), 0dcb
--- (mountains), 0e47 (hills or mountains).
function G:mask(x, y, test)
  local m = 0
  for i = 0, 7 do
    local ax, ay = x + self:nx(i), y + self:ny(i)
    local off = ax < 0 or ay < 0 or ax >= W or ay >= H
    if off or test(self:at(ax, ay)) then m = m + BIT[i] end
  end
  return m
end

function G:shape(m) return i8(self.m[SHAPE + m]) end

--- 4fef:0464: the tiles that join each kind of ground to its neighbours. A
--- shape the tile set has no tile for gives way: water to marsh, forest
--- and hills often to plain, mountains to hills.
function G:shapes()
  local function each(t, f)
    for y = 0, H - 1 do for x = 0, W - 1 do if self:at(x, y) == t then f(x, y) end end end
  end
  for y = 0, H - 1 do
    for x = 0, W - 1 do
      local t = self:at(x, y)
      if t == WATER or t == SHORE then
        local v = self:shape(self:mask(x, y, function(n) return n == WATER or n == SHORE or n == BRIDGE end))
        if v < 0 then
          self:put(x, y, MARSH)
          self:setTile(x, y, self:tileFor(MARSH, 0, x, y))
          self:marshAround(x, y)
          self:clearCrossing(y * W + x)
        else
          self:put(x, y, WATER)
          self:setTile(x, y, self:tileFor(WATER, v, x, y))
        end
      end
    end
  end
  each(FOREST, function(x, y)
    local v = self:shape(self:mask(x, y, function(n) return n == FOREST end))
    if v >= 0 then
      self:setTile(x, y, self:tileFor(FOREST, v, x, y))
      self:put(x, y, FOREST)
    elseif self:dice(1, 10, 0) < 6 then
      self:setTile(x, y, self:tileFor(PLAIN, 0, x, y))
      self:put(x, y, PLAIN)
    else
      self:setTile(x, y, self:tileFor(FOREST, 13, x, y))
    end
  end)
  each(MOUNTAINS, function(x, y)
    local v = self:shape(self:mask(x, y, function(n) return n == MOUNTAINS end))
    if v >= 0 then
      self:setTile(x, y, self:tileFor(MOUNTAINS, v, x, y))
      self:put(x, y, MOUNTAINS)
    else
      self:setTile(x, y, self:tileFor(HILLS, 0, x, y))
      self:put(x, y, HILLS)
    end
  end)
  each(HILLS, function(x, y)
    local v = self:shape(self:mask(x, y, function(n) return n == MOUNTAINS or n == HILLS end))
    if v >= 0 then
      self:setTile(x, y, self:tileFor(HILLS, v, x, y))
      self:put(x, y, HILLS)
    elseif self:dice(1, 10, 0) < 6 then
      self:setTile(x, y, self:tileFor(PLAIN, 0, x, y))
      self:put(x, y, PLAIN)
    else
      self:put(x, y, HILLS)
      self:setTile(x, y, self:tileFor(HILLS, 13, x, y))
    end
  end)
  each(MARSH, function(x, y)
    -- mostly the plain marsh; three in ten one of four others
    local o = self:dice(1, 10, 0) < 7 and 2 * parity(x, y) or 4 * self:dice(1, 4, 0)
    self:setTile(x, y, lo(self:r16(TILES + 64 * MARSH + o) % 65536))
    self:put(x, y, MARSH)
  end)
end

-- --- 5311: bridges, crossings and roads ------------------------------------------

--- 5311:0a7f: ground a bridge may land on.
function G:firm(x, y)
  local t = self:at(x, y)
  return t == PLAIN or t == FOREST or t == HILLS
end

--- The nearest city, by map distance, to each listed spot (shared by
--- 5311:03c3 and 08f6; a spot's figure is worked out once).
function G:nearestCities(list, n)
  if list.cityDist[0] ~= -1 then return end
  for i = n - 1, 0, -1 do
    if list.x[i] ~= -1 then
      local best = 10000
      for c = self:cityCount() - 1, 0, -1 do
        best = min(best, distance(list.x[i], list.y[i], self:cx(c), self:cy(c)))
      end
      list.cityDist[i] = best
    end
  end
end

--- 5311:08f6 (bridges) and 03c3 (crossings): the best spot still free --
--- a die roll, plus more the nearer a city is. Bridges keep 10 apart.
function G:pickSpot(list, n, apart)
  self:nearestCities(list, n)
  for i = n - 1, 0, -1 do
    if list.chosen[i] == 0 and list.x[i] ~= -1 then
      local best = 10000
      for j = n - 1, 0, -1 do
        if list.chosen[j] ~= 0 then best = min(best, distance(list.x[i], list.y[i], list.x[j], list.y[j])) end
      end
      list.chosenDist[i] = best
    end
  end
  local bestScore, pick = -1, -1
  for i = n - 1, 0, -1 do
    if list.chosen[i] == 0 and list.x[i] ~= -1 and not (apart and not (list.chosenDist[i] > 9)) then
      local roll = self:dice(1, 15, 1)
      local near = list.cityDist[i] < 31 and 30 - list.cityDist[i] or 0
      if bestScore < near + roll then bestScore, pick = near + roll, i end
    end
  end
  return pick
end

--- 5311:05b4: bridges over two-tile rivers. As the original has it, only
--- a river running north and south (tiles 0x26 and 0x28) is ever bridged.
function G:bridges(list)
  local n = 0
  for y = 1, H - 3 do
    for x = 1, W - 3 do
      local t = self:tile(x, y)
      local across = (t == 0x26 or t == 0x16) and (self:tile(x + 1, y) == 0x28 or self:tile(x + 1, y) == 0x18)
      local along = (t == 0x21 or t == 0x11) and (self:tile(x, y + 1) == 0x24 or self:tile(x, y + 1) == 0x14)
      if across or along then
        local ew = t == 0x26 and 1 or 0
        local ok = ew == 1 and ((self:firm(x - 1, y) and self:firm(x + 2, y))
          or (self:firm(x, y - 1) and self:firm(x, y + 2)))
        if ok and n < 50 then
          list.x[n], list.y[n], list.ew[n] = x, y, ew
          n = n + 1
        end
      end
    end
  end
  if n == 0 then return end
  while true do
    local i = self:pickSpot(list, n, true)
    if i == -1 then break end
    list.chosen[i] = 1
    local x, y = list.x[i], list.y[i]
    if list.ew[i] == 0 then
      self:setTile(x, y, 0x84) self:setTile(x, y + 1, 0x94)
      self:put(x, y + 1, BRIDGE) self:put(x, y, BRIDGE)
      self:setRoad(x, y + 2, 1) self:setRoad(x, y - 1, 1)
    else
      self:setTile(x, y, 0x85) self:setTile(x + 1, y, 0x86)
      self:put(x + 1, y, BRIDGE) self:put(x, y, BRIDGE)
      self:setRoad(x + 2, y, 1) self:setRoad(x - 1, y, 1)
    end
  end
end

local SEAWARD = { [0x20] = { -1, 0 }, [0x23] = { -1, 0 }, [0x21] = { 0, -1 },
                  [0x22] = { 1, 0 }, [0x25] = { 1, 0 }, [0x24] = { 0, 1 } }

--- 5311:01f6: up to ten crossings -- shore that looks out on three tiles of
--- open water, at least 15 from the others.
function G:crossings(list)
  for y = 1, H - 3 do
    for x = 1, W - 3 do
      if self:dice(1, 2, -1) == 0 then
        local d = SEAWARD[self:tile(x, y)]
        if d then
          local open = true
          for k = 1, 3 do if self:at(x + k * d[1], y + k * d[2]) ~= WATER then open = false end end
          if open then
            local near = 10000
            for i = 49, 0, -1 do
              if list.x[i] ~= -1 then near = min(near, distance(x, y, list.x[i], list.y[i])) end
            end
            if near > 14 then
              for i = 0, 49 do
                if list.x[i] == -1 then list.x[i], list.y[i] = x, y break end
              end
            end
          end
        end
      end
    end
  end
  list.cityDist[0] = -1
  local n = 0
  while n < 10 do
    local i = self:pickSpot(list, 50, false)
    if i == -1 then break end
    list.chosen[i] = 1
    self:setCrossing(list.y[i] * W + list.x[i])
    n = n + 1
  end
end

--- 5311:0e22: a pair of cities to join -- a random one not yet served, and
--- of the unserved 30 to 70 away, the one that rolls highest.
function G:roadPair(served)
  while true do
    local a = -1
    for _ = 80, 1, -1 do
      a = self:dice(1, self:cityCount(), -1)
      if (served[a] or 0) == 0 then break end
      a = -1
    end
    if a == -1 then return nil end
    if a < 100 then served[a] = 1 end
    local b, best = -1, -1
    for c = self:cityCount() - 1, 0, -1 do
      if (served[c] or 0) == 0 then
        local d = distance(self:cx(a), self:cy(a), self:cx(c), self:cy(c))
        if d > 29 and d < 71 then
          local roll = self:dice(1, 1000, 0)
          if best < roll then best, b = roll, c end
        end
      end
    end
    if b ~= -1 then
      if b < 100 then served[b] = 1 end
      return a, b
    end
  end
end

--- 5311:0ad0: a tile round a city's footprint to start a road from.
function G:roadEnd(c)
  for _ = 8, 1, -1 do
    local r = ROUND_CITY[self:dice(1, 12, -1)]
    local x, y = self:cx(c) + r[1], self:cy(c) + r[2]
    local t = self:at(x, y)
    if t == PLAIN or t == FOREST or t == HILLS then return { x, y } end
  end
  return nil
end

--- 5311:0c1c: lay a road along the path the game's own pathfinder finds,
--- as pseudo-player 14 -- not over water, shore or bridges -- and every six
--- steps count the cities within 10 as served.
function G:layRoad(served, from, to)
  local ground = {
    terrain = function(x, y) return self:terrainOf(self:tile(x, y)) end,
    road = function(x, y) return self:road(x, y) ~= 0 end,
    crossing = function(x, y) return self:crossingAt(y * W + x) end,
  }
  local route = move.roadRoute(ground, ROAD_COST, W, H, from[1], from[2], to[1], to[2])
  if not route then return end
  local x, y, since = from[1], from[2], 0
  for i = 1, min(#route, 200) do
    since = since + 1
    if x == to[1] and y == to[2] then break end
    x, y = route[i].x, route[i].y
    local t = self:at(x, y)
    if t ~= SHORE and t ~= WATER and t ~= BRIDGE then self:setRoad(x, y, 1) end
    if since > 5 then
      since = 0
      for c = self:cityCount() - 1, 0, -1 do
        if distance(x, y, self:cx(c), self:cy(c)) < 10 and c < 100 then served[c] = 1 end
      end
    end
  end
end

--- 5311:0000: tiles, then bridges, crossings and roads.
function G:roads()
  coroutine.yield(81)
  self:toTiles()
  self:shapes()
  self:shapes()
  for i = 0, W * H - 1 do self:clearCrossing(i) self:clearRoad(i) end
  coroutine.yield(82)
  local list = { x = {}, y = {}, ew = {}, cityDist = {}, chosen = {}, chosenDist = {} }
  for i = 0, 49 do
    list.x[i], list.y[i], list.ew[i], list.cityDist[i], list.chosenDist[i] = -1, -1, -1, -1, -1
    list.chosen[i] = 0
  end
  self:bridges(list)
  for i = 49, 0, -1 do
    if list.chosen[i] == 0 then
      list.x[i], list.y[i], list.ew[i], list.cityDist[i], list.chosenDist[i] = -1, -1, -1, -1, -1
    end
  end
  self:crossings(list)
  coroutine.yield(83)
  local served = {}
  for i = 0, 99 do served[i] = 0 end
  local shown = 4
  while true do
    local pa, pb = self:roadPair(served)
    if not pa then break end
    if shown < 10 and self:dice(1, 3, -1) == 0 then
      coroutine.yield(80 + shown)
      shown = shown + 1
    end
    local a = self:roadEnd(pa)
    local b = a and self:roadEnd(pb)
    if a and b and not (a[1] == b[1] and a[2] == b[2]) then self:layRoad(served, a, b) end
  end
  for i = 0, W * H - 1 do if self.m[GRID + i] == SITE then self:clearRoad(i) end end
end

--- 4fef:0a4b: each road its shape from the roads and bridges round it; a
--- shape there is no piece for leaves plain or hills. Then a dozen or two
--- straight pieces take their other look.
function G:roadShapes()
  for y = 0, H - 1 do
    for x = 0, W - 1 do
      if self:road(x, y) ~= 0 then
        local m = 0
        for i = 0, 7 do
          local ax, ay = x + self:r16(NB_ROAD + 4 * i), y + self:r16(NB_ROAD + 4 * i + 2)
          if ax >= 0 and ay >= 0 and ax < W and ay < H
             and (self:road(ax, ay) ~= 0 or self:terrainOf(self:tile(ax, ay)) == BRIDGE) then
            m = m + BIT[i]
          end
        end
        local v = i8(self.m[ROAD_SHAPE + m])
        if v >= 0 then self:setRoad(x, y, v + 1)
        elseif self:dice(1, 10, 0) < 6 then
          self:setTile(x, y, self:tileFor(PLAIN, 0, x, y))
          self:put(x, y, PLAIN)
        else
          self:setTile(x, y, self:tileFor(HILLS, 13, x, y))
        end
      end
    end
  end
  for _, ft in ipairs({ { 2, 17 }, { 1, 16 } }) do
    local n = self:dice(1, 10, 10)
    for _ = 1, n do
      for _ = 1, 10000 do
        local x = self:dice(1, W, -1)
        local y = self:dice(1, H, -1)
        if self:road(x, y) == ft[1] then self:setRoad(x, y, ft[2]) break end
      end
    end
  end
end

-- --- 513d / 5132: the sides and the cities' details -------------------------

--- 513d:05e3: the cities, read back off the tile map in rows.
function G:cityList()
  local n = 0
  for y = 0, H - 1 do
    for x = 0, W - 1 do
      if self:tile(x, y) == 0x60 then
        local o = self:crec(n)
        self:ws16(o, x) self:ws16(o + 2, y)
        self.scn[o + 21] = 15
        self.scn[o + 20] = self:dice(1, 3, 2)
        n = n + 1
      end
    end
  end
  self:setCityCount(n)
end

--- 513d:08f0: a random city in one of the map's eight regions, four
--- across and two down; after 202 tries, wherever the last one was.
function G:cityIn(region)
  local x0, y0 = (region % 4) * 28, idiv(region, 4) * 78
  local c = 0
  for _ = 1, 202 do
    c = self:dice(1, self:cityCount(), -1)
    local x, y = self:cx(c), self:cy(c)
    if x >= x0 and x < x0 + 28 and y >= y0 and y < y0 + 78 then break end
  end
  return c
end

local function sideRec(s) return S_SIDE_REC + 20 * s end

--- 513d:0689: each side a capital in a region of its own, 12 from the
--- edge and 20 from the others both ways; ten rounds of 100 tries a side,
--- the last round taking what it gets. A side's armies favour its region.
function G:capitals()
  local round = 1
  while true do
    local used = {}
    for s = 0, 7 do self:ws16(sideRec(s) + 6, -100) self:ws16(sideRec(s) + 8, -100) end
    local again = false
    for s = 0, 7 do
      local region
      repeat region = self:dice(1, 8, -1) until not used[region]
      used[region] = true
      local c, ok, tries = 0, false, 0
      while not ok and tries < 100 do
        c = self:cityIn(region)
        local x, y = self:cx(c), self:cy(c)
        if not (x < 12 or x > 100 or y < 12 or y > 144) then
          ok = true
          for o = 0, 7 do
            if abs(x - self:s16(sideRec(o) + 6)) < 20 or abs(y - self:s16(sideRec(o) + 8)) < 20 then
              ok = false
              break
            end
          end
        end
        tries = tries + 1
      end
      -- a find on the hundredth try counts as none, as the original has it
      if tries > 99 and round < 10 then again = true break end
      self:w16(REGION_CLASS + 2 * region, self:r16(SIDE_CLASS + 2 * s))
      self:ws16(sideRec(s) + 6, self:cx(c))
      self:ws16(sideRec(s) + 8, self:cy(c))
    end
    if not again then break end
    round = round + 1
  end
  for s = 0, 7 do
    local x, y = self:s16(sideRec(s) + 6), self:s16(sideRec(s) + 8)
    for c = 0, self:cityCount() - 1 do
      if self:cx(c) == x and self:cy(c) == y then self.scn[self:crec(c) + 21] = s break end
    end
    -- 513d:09b3: the castle in the side's colours
    local t = s < 6 and 0x62 + 2 * s or 0x80 + 2 * (s - 6)
    self:setTile(x, y, t) self:setTile(x + 1, y, t + 1)
    self:setTile(x, y + 1, t + 16) self:setTile(x + 1, y + 1, t + 17)
  end
end

--- 5132:0014 / 006b: each side a name from five, and 3d50+20 gold.
function G:sides()
  for s = 0, 7 do
    local name = self:str(SIDE_NAMES + 100 * s + 20 * self:dice(1, 5, -1), 20)
    for i = 0, 19 do self.scn[20 * s + i] = 0 end
    self:wstr(20 * s, name)
  end
  for s = 0, 7 do self:ws16(sideRec(s) + 2, self:dice(3, 50, 20)) end
end

--- What touches a city's footprint (513d:0ce0, into 4125:02d6-02de).
function G:surroundings(c)
  local x, y = self:cx(c), self:cy(c)
  local function by(t)
    return self:near(x, y, t) or self:near(x + 1, y, t) or self:near(x, y + 1, t) or self:near(x + 1, y + 1, t)
  end
  return { shore = by(SHORE), forest = by(FOREST), road = by(ROAD), hills = by(HILLS), marsh = by(MARSH) }
end

--- random_city_value (513d:1104): a capital 9; else 1d4-1, +4 by the
--- shore, +2 by a road, -1 by forest, -2 by marsh, within 0..9.
function G:value(c, f)
  if self.scn[self:crec(c) + 21] ~= 15 then return 9 end
  local v = self:dice(1, 4, -1)
  if f.shore then v = v + 4 end
  if f.road then v = v + 2 end
  if f.forest then v = v - 1 end
  if f.marsh then v = v - 2 end
  if v > 8 then v = 9 end
  if v < 1 then v = 0 end
  return v
end

--- 513d:1468: a name of two or three syllables, ending most often in a
--- word for what lies round it.
function G:cityName(c, f)
  local three = self:dice(1, 10, 0) < 7
  local name = self:str(0xa86 + 10 * self:dice(1, 20, -1), 10)
  if three then name = name .. self:str(0xb4e + 10 * self:dice(1, 20, -1), 10) end
  local ending = nil
  if self:dice(1, 10, 0) < 7 then
    if f.marsh then ending = 0xda6
    elseif f.forest then ending = 0xcde
    elseif f.shore then ending = 0xd42
    elseif f.hills then ending = 0xe0a
    elseif self:dice(1, 10, 0) < 5 then ending = 0xe6e end
  end
  if ending then name = name .. self:str(ending + 10 * self:dice(1, 10, -1), 10)
  else name = name .. self:str(0xc16 + 10 * self:dice(1, 20, -1), 10) end
  local o = self:crec(c) + 4
  for i = 0, 15 do self.scn[o + i] = 0 end
  name = name:sub(1, 15)
  self:wstr(o, name)
  return name
end

--- 513d:1171: the city's name, income -- value x 2 + 1d8 + 14 -- and its
--- three lines of description, grander as the city is richer.
function G:cityText(c, v, f)
  local name = self:cityName(c, f)
  self:ws16(self:crec(c) + 42, v * 2 + self:dice(1, 8, 0) + 14)
  local function grade() return min(9, max(0, self:dice(1, 3, v - 2))) end
  local g1 = grade()
  local g2 = grade()
  local function w(o, i) return self:str(o + 16 * i, 16) end
  local a, b, cc
  if (not f.forest and not f.marsh and not f.hills) or self:dice(1, 100, 0) > 79 then
    if self:dice(1, 100, 0) < 50 then
      a = w(0x16a8, self:dice(1, 10, -1))
      b = w(0x1748, self:dice(1, 10, -1))
      cc = w(0x17e8, self:dice(1, 10, -1))
    else
      a = w(0x14c8, self:dice(1, 10, -1))
      b = w(0x1568, self:dice(1, 10, -1))
      cc = w(0x1608, self:dice(1, 10, -1))
    end
  else
    local k
    if f.marsh then k = self:dice(1, 2, 7)
    elseif f.hills then k = self:dice(1, 4, 3)
    else k = self:dice(1, 4, -1) end
    a = w(0x12e8, self:dice(1, 10, -1))
    b = w(0x1388, self:dice(1, 10, -1))
    cc = w(0x1428, k)
  end
  local kind = w(0x1248, g2)
  local adj = self:str(0x1068 + 0x30 * g1 + 16 * self:dice(1, 3, -1), 16)
  self.cty[#self.cty + 1] = ("#%03d|%s is a %s|%s, %s|%s %s|\n"):format(c, name, adj, kind, a, b, cc)
end

--- The army types' flags the generator asks after (build_army_move_flags,
--- 6715:0000): ARMYTYPE +54 flies, +48 magical -- an ally.
function G:flies(t) local a = self.types.byId[t] return a ~= nil and (a.bonus[54] or 0) ~= 0 end
function G:magical(t) local a = self.types.byId[t] return a ~= nil and (a.bonus[48] or 0) ~= 0 end

function G:setSlot(c, k, e)
  local o, a = self:crec(c), self.types.byId[e] or {}
  self.scn[o + 22 + k] = e % 256
  self.scn[o + 26 + k] = (a.time or 0) % 256
  self.scn[o + 30 + k] = (a.strength or 0) % 256
  self.scn[o + 34 + k] = (a.move or 0) % 256
  self.scn[o + 38 + k] = (a.cost or 0) % 256
end

--- 513d:161d: what a city can make. value / 2 + 1d4 - 1 slots, one more
--- each by forest and hills, at most 4, from RANDOM.DAT's list in order:
--- each type rolls its chance, and must suit the city's region, or its
--- forest, hills or shore, or be at home anywhere. A rich city sometimes
--- adds a flier. Allies only where the option lets cities make them.
function G:production(c, v, f)
  local x, y = self:cx(c), self:cy(c)
  local region = idiv(x * 4, W) + idiv(y * 2, H) * 4
  local slots = idiv(v, 2)
  slots = slots + self:dice(1, 4, -1)
  if f.forest then slots = slots + 1 end
  if f.hills then slots = slots + 1 end
  if slots > 3 then slots = 4 end
  if slots < 1 then slots = 0 end
  local function rec(e) return PRODUCTION + 16 * e end
  local function suits(e)
    local terr = self:r16(rec(e) + 6)
    return terr == 7 or (terr == 4 and f.forest) or (terr == 5 and f.hills)
      or self:r16(rec(e) + 4) == self:r16(REGION_CLASS + 2 * region)
  end
  local k = 0
  local e = 0
  while e < 29 and k < slots do
    local t = self:r16(rec(e))
    if not (not self.allies and self:magical(t))
       and not (self:dice(1, 10, -1) >= self:r16(rec(e) + 2)) then
      local ok = suits(e) or (self:r16(rec(e) + 6) == 3 and f.shore)
      if t == 5 and not f.shore then ok = false end
      if ok then self:setSlot(c, k, t) k = k + 1 end
    end
    e = e + 1
  end
  if k == 0 then self:setSlot(c, k, self:r16(rec(0))) k = k + 1 end
  if v > 6 and self:dice(1, 10, -1) < 5 and k < 4 then
    local flier = false
    for j = 0, k - 1 do if self:flies(self.scn[self:crec(c) + 22 + j]) then flier = true end end
    if not flier and k < slots then
      for e2 = 0, 28 do
        local t = self:r16(rec(e2))
        if self:flies(t) and not self:magical(t) and suits(e2) then
          self:setSlot(c, k, t)
          k = k + 1
          break
        end
      end
    end
  end
  while k < 4 do self.scn[self:crec(c) + 22 + k] = 255 k = k + 1 end
end

--- random_magical_type (6563:1a9b).
function G:magicalType()
  local n = 0
  for t = 0, 27 do if self:magical(t) then n = n + 1 end end
  local pick = self:dice(1, n, -1)
  local i, t = 0, 0
  while t < 28 do
    if self:magical(t) then
      if i == pick then break end
      i = i + 1
    end
    t = t + 1
  end
  if t > 27 or not self:magical(t) then return 25 end
  return t
end

--- random_add_production (513d:1b3f): with allies on, 2d3 cities may also
--- make one.
function G:allyProduction()
  local n = self:dice(2, 3, 0)
  local tries = 0
  while n > 0 and tries < 100 do
    local c = self:dice(1, self:cityCount(), -1)
    local o = self:crec(c)
    local k = 0
    for j = 0, 3 do if self.scn[o + 22 + j] < 128 then k = k + 1 end end
    if k < 4 then self:setSlot(c, k, self:magicalType()) n = n - 1 end
    tries = tries + 1
  end
end

--- 513d:1c1d: a capital adds a strong army it cannot make yet -- strength
--- 5 or more, not a navy or an ally -- in its last empty slot.
function G:capitalArmies()
  for s = 0, 7 do
    local x, y = self:s16(sideRec(s) + 6), self:s16(sideRec(s) + 8)
    local c = -1
    local i = 0
    while i < self:cityCount() and c < 0 do
      if self:cx(i) == x and self:cy(i) == y then c = i end
      i = i + 1
    end
    if c >= 0 then
      local o = self:crec(c)
      -- the highest empty slot, else the last
      local k = 3
      while k >= 0 and self.scn[o + 22 + k] < 128 do k = k - 1 end
      if k < 0 then k = 3 end
      local draws, pick = 0, -1
      while draws <= 39 do
        local t = self:dice(1, 28, -1)
        if not (t == 5 or self:magical(t)) then
          draws = draws + 1
          local has = false
          for j = 0, 3 do if self.scn[o + 22 + j] == t then has = true end end
          local a = self.types.byId[t]
          if not has and a and (a.strength or 0) >= 5 then pick = t break end
        end
      end
      -- one found on the fortieth draw is dropped, as the original has it
      if pick >= 0 and draws < 40 then self.scn[o + 22 + k] = pick end
    end
  end
end

--- random_map_cities (513d:0ce0).
function G:cityDetails()
  for c = 0, self:cityCount() - 1 do
    local f = self:surroundings(c)
    local v = self:value(c, f)
    self:cityText(c, v, f)
    self:production(c, v, f)
  end
  if self.allies then self:allyProduction() end
  self:capitalArmies()
end

--- auto_file_random_sgn (4fef:113d): 41-70 signposts on open plain off
--- the roads, most pointing the way to the nearest city, a few with
--- RANDOM.DAT's own words on them.
function G:signs()
  local n = self:dice(1, 30, 40)
  local out = {}
  for i = 0, 2 + 104 * n - 1 do out[i] = 0 end
  setI16(out, 0, n)
  local function put(o, s)
    for i = 1, min(#s, 49) do out[o + i - 1] = s:byte(i) end
  end
  local fixed = 0
  for i = 0, n - 1 do
    local x, y = 0, 0
    for _ = 1, SAFETY do
      x = self:dice(1, W, -1)
      y = self:dice(1, H, -1)
      if self:terrainOf(self:tile(x, y)) == PLAIN and not self:near(x, y, CITY)
         and not self:near(x, y, TOWER) and self:road(x, y) == 0 then break end
    end
    self:setTile(x, y, 0)
    local o = 2 + 104 * i
    setI16(out, o, x) setI16(out, o + 2, y)
    if self:dice(1, 10, 0) < 4 and fixed < self:r16(SIGNS) then
      put(o + 4, self:str(SIGNS + 2 + 60 * fixed, 30))
      put(o + 54, self:str(SIGNS + 2 + 60 * fixed + 30, 30))
      fixed = fixed + 1
    else
      local c, best = -1, 1000
      for j = 0, self:cityCount() - 1 do
        local d = distance(x, y, self:cx(j), self:cy(j))
        if d < best then best, c = d, j end
      end
      if c >= 0 then
        local cx, cy = self:cx(c), self:cy(c)
        put(o + 4, self:str(SIGN_CITY, 20):format(cstr(self.scn, self:crec(c) + 4, 16)))
        put(o + 54, self:str(SIGN_LEAGUES, 20):format(distance(x, y, cx, cy) * 2,
          COMPASS[compass(x, y, cx, cy)]))
      end
    end
  end
  return str(out, 2 + 104 * n)
end

-- --- the whole -----------------------------------------------------------------

--- random_map_generate (4bed:011c), yielding as the original's progress
--- bar moves; returns the scenario's files.
function G:run()
  coroutine.yield(0)
  -- 4bed:036b: the map starts as sea
  for i = 0, W * H - 1 do self.m[GRID + i] = WATER self:clearRoad(i) end
  self:coast()
  coroutine.yield(10)
  self:highlands()
  coroutine.yield(20)
  self:erosion()
  coroutine.yield(30)
  self:rivers()
  coroutine.yield(40)
  self:forests()
  coroutine.yield(50)
  self:marshes()
  coroutine.yield(60)
  self:cities()
  coroutine.yield(70)
  self:sites()
  coroutine.yield(80)
  self:roads()
  coroutine.yield(90)
  -- 4fef:0014
  self:roadShapes()
  coroutine.yield(92)
  self:cityList()
  self:capitals()
  self:sides()
  coroutine.yield(94)
  self:cityDetails()
  coroutine.yield(96)
  local sgn = self:signs()
  coroutine.yield(98)
  self.scn[S_RANDOM], self.scn[S_RANDOM + 1] = 1, 0
  local map, rd = {}, {}
  for i = 0, W * H - 1 do
    local v = self.tiles[i]
    map[2 * i] = v % 256
    map[2 * i + 1] = (v - v % 256) / 256
    rd[i] = self.rd[i] % 32
  end
  coroutine.yield(100)
  return {
    SCN = str(self.scn, self.scnSize), MAP = str(map, 2 * W * H), RD = str(rd, W * H),
    SGN = sgn, CTY = table.concat(self.cty), SPC = table.concat(self.spc),
  }
end

--- The sliders as random_map_setup (7bab:10e8) takes them: Water, Hills,
--- Cities, Forest, each 0-6, 7 rolled as 1d7-1.
function M.settle(sliders, rng)
  local out = {}
  for i = 1, 4 do
    local v = sliders[i]
    out[i] = v == M.RANDOM_SLIDER and rng:dice(1, 7, -1) or v
  end
  return out
end

--- random_map_load_params (4bed:0000): RANDOM.DAT with the sliders added.
function M.params(dataDir, sliders)
  local s = scn.readMaybe(dataDir .. "/RANDOM/RANDOM.DAT")
  if not s then error("Random data not found") end
  local m = bytes(s)
  local function add(o, t, i) setI16(m, o, getI16(m, o) + getI16(m, t + 2 * i)) end
  local water, hills, cities, forest = sliders[1], sliders[2], sliders[3], sliders[4]
  add(P_CITIES, SLIDER_CITIES, cities)
  add(P_FOREST, SLIDER_FOREST, forest)
  add(P_HILLS, SLIDER_HILLS, hills)
  add(P_MOUNTAINS, SLIDER_HILLS, hills)
  add(P_WIDE_RIVERS, SLIDER_WATER, water)
  add(P_RIVERS, SLIDER_WATER, water)
  return m
end

--- Make a random world. A coroutine body: it yields the progress, 0-100, as
--- the original's bar shows it, and returns the files, keyed by extension.
---
--- opts: { dataDir, rng, sliders = { water, hills, cities, forest } (0-7),
---         allies, terrainSet }
function M.generate(opts)
  local sliders = M.settle(opts.sliders or { 3, 3, 2, 3 }, opts.rng)
  local dat = M.params(opts.dataDir, sliders)
  -- auto_file_x_scn (4fef:1001): the scenario of the chosen terrain set,
  -- Erythea for the only one there is, is the template
  local t = scn.readMaybe(opts.dataDir .. "/ERYTHEA/ERYTHEA.SCN")
  if not t then error("cannot open: ERYTHEA.SCN") end
  local template = bytes(t)
  template.n = #t
  local types = armytype.load(opts.dataDir .. "/TERRAIN0/ARMYTYPE.DAT")
  local gen = M.newGenerator(dat, template, types, opts.rng, opts)
  gen.scn[S_TERRAIN_SET] = opts.terrainSet or 0
  return gen:run()
end

--- Run a generation through to its end.
function M.generateNow(opts)
  local co = coroutine.create(M.generate)
  local ok, v = coroutine.resume(co, opts)
  while true do
    if not ok then error(v, 0) end
    if coroutine.status(co) == "dead" then return v end
    ok, v = coroutine.resume(co)
  end
end

--- Put a random world's files where scn.load will find them.
function M.install(dataDir, files)
  for ext, s in pairs(files) do
    scn.install(("%s/%s/%s.%s"):format(dataDir, M.DIR, M.DIR, ext), s)
  end
end

--- The installed world's files, keyed by extension, for a save to carry.
function M.files(dataDir)
  local out = {}
  for _, ext in ipairs(M.FILES) do
    out[ext] = scn.readMaybe(("%s/%s/%s.%s"):format(dataDir, M.DIR, M.DIR, ext))
  end
  return out
end

--- The terrain set's name, as the start menu shows it (DATA\TERRAIN.DAT).
function M.terrainSetName(dataDir, set)
  local s = scn.readMaybe(dataDir .. "/DATA/TERRAIN.DAT")
  if not s then return "" end
  local n = s:byte(1) + s:byte(2) * 256
  if set >= n then return "" end
  local o = 2 + 52 * set + 2
  return (s:sub(o + 1, o + 50):match("^[^%z]*"))
end

--- How many terrain sets there are.
function M.terrainSets(dataDir)
  local s = scn.readMaybe(dataDir .. "/DATA/TERRAIN.DAT")
  if not s then return 1 end
  return max(1, s:byte(1) + s:byte(2) * 256)
end

return M
