-- The computer players' shared machinery: their per-side data, the city
-- neighbour table, the stack they have "selected", the flood fill they
-- measure distances with, the battles they simulate, and the walk that moves
-- a stack and fights what it meets.
--
-- A port of WARLORD2.EXE; the addresses are Ghidra's (docs/re/ai.md). The
-- original keeps one global selection (DS:1ede, leader DS:1f02) that every
-- phase fills and moves; here it is a table passed around, `sel`.

local armytype = require("warlords.armytype")
local rules    = require("warlords.rules")
local scn      = require("warlords.scn")

local core = {}

core.NEUTRAL = rules.NEUTRAL or 15

-- city roles, AI data +0x56 + city (docs/re/ai.md > City roles)
core.JUST_TAKEN, core.TAKING_NEUTRAL, core.NEAR_NEUTRAL = 1, 2, 3
core.WEAK, core.BUILDING, core.MEMBER, core.RALLY = 4, 5, 6, 7
core.STOP, core.EXPLORER2, core.EXPLORER, core.NOWHERE = 8, 11, 13, 14

-- an army's standing order, the low nibble of record +14
core.ORDER_NONE, core.ORDER_CITY, core.ORDER_ITEM = 0, 1, 2
core.ORDER_SITE, core.ORDER_ROAM = 3, 4

-- per-city AI flags, +0x11e + city
core.CF_UNSEEN, core.CF_FLIER, core.CF_CLEANED = 0x01, 0x02, 0x04
core.CF_MOVE12, core.CF_MAGIC, core.CF_HERO = 0x08, 0x10, 0x20
core.CF_FLYGROUP, core.CF_MOVE16 = 0x40, 0x80

-- an assault group's flags, entry +0x5a
core.GF_RAZE, core.GF_SACK, core.GF_PILLAGE = 0x01, 0x02, 0x04
core.GF_MOVE16, core.GF_MOVE12, core.GF_FLY = 0x08, 0x10, 0x20

core.MAX_GROUPS = 4                    -- entries in the table; +0x24a says how many are used
core.UNREACHED = 30001                 -- a flood cell never reached (0x7531)

------------------------------------------------------------------ bit tests

-- LuaJIT runs the front end, plain Lua 5.1 the tests: neither has bit
-- operators in common, so flags go through arithmetic.
function core.has(v, bit) return (v or 0) % (bit + bit) >= bit end
function core.set(v, bit) v = v or 0; return core.has(v, bit) and v or v + bit end
function core.clear(v, bit) v = v or 0; return core.has(v, bit) and v - bit or v end

------------------------------------------------------------------ the world

--- map_distance (2012:1199): the straight-line distance, truncated.
function core.dist(x1, y1, x2, y2)
  local dx, dy = x1 - x2, y1 - y2
  return math.floor(math.sqrt(dx * dx + dy * dy))
end

function core.owner(c) return c.ownerIndex or core.NEUTRAL end

--- Is the city still a city (not ruins)? The original tests the terrain
--- under its top-left tile for type 10.
function core.standing(c) return not c.razed end

function core.side(g, index) return g.map.sides[index + 1] end

--- Is a side in the game? .SCN 0x137 + 2*side.
function core.inPlay(g, index)
  local s = g.map.sides[index + 1]
  return s ~= nil and s.inUse and s.alive ~= false
end

function core.isComputer(g, index)
  local s = g.map.sides[index + 1]
  return s ~= nil and s.computer
end

--- The diplomatic state between two sides (bits 0-1 of 0x153b + 8a + b).
function core.state(g, a, b)
  if a == core.NEUTRAL or b == core.NEUTRAL or a == nil or b == nil then return 2 end
  if a == b then return 0 end
  return require("warlords.diplomacy").state(g, a, b)
end

--- A side's proposal to another (bits 2-3).
function core.proposal(g, a, b)
  if a == b then return 0 end
  return require("warlords.diplomacy").proposal(g, a, b)
end

function core.propose(g, a, b, v)
  if a == b then return end
  require("warlords.diplomacy").propose(g, a, b, v)
end

--- The city whose footprint covers (x, y) -- or, as 828e:044a looks, whose
--- top-left is (x, y) or one up and left of it.
function core.cityAt(g, x, y)
  return g.map.cityTile[y * g.map.width + x]
end

--- The site on (x, y) (828e:04b5).
function core.siteAt(g, x, y)
  return g.map.siteAt and g.map.siteAt[y * g.map.width + x]
end

function core.terrain(g, x, y) return scn.terrainAt(g.map, x, y) end

--- Is (x, y) a site tile whose site is still worth a visit: a temple, or a
--- ruin nobody has searched (the tile's "explored" bit 0x40 clear)?
function core.siteOpen(s)
  return s.content == require("warlords.site").TEMPLE or not s.searched
end

--- Has the side seen (x, y), or any tile next to it (623c:16ae)? Always true
--- with Hidden Map off.
function core.explored(g, side, x, y)
  if g.map.options.hiddenMap == 0 then return true end
  local game = require("warlords.game")
  for tx = x - 1, x + 1 do
    for ty = y - 1, y + 1 do
      if tx >= 0 and ty >= 0 and tx < g.map.width and ty < g.map.height
         and game.seen(g, side, tx, ty) then
        return true
      end
    end
  end
  return false
end

function core.flies(g, a) return g.types.byId[a.type].flies end
function core.magical(g, a) return (g.types.byId[a.type].bonus[48] or 0) ~= 0 end
function core.ability(g, a) return g.types.byId[a.type].bonus[52] or 0 end
function core.woods(g, a) return g.types.byId[a.type].woodsMove end
function core.hills(g, a) return g.types.byId[a.type].hillsMove end
function core.isHero(a) return a.type == armytype.HERO end

--- The side's armies, in the order the original walks its records: from the
--- last down to the first.
function core.armies(g, sideIndex)
  local out = {}
  for i = #g.armies, 1, -1 do
    local a = g.armies[i]
    if a.owner == sideIndex then out[#out + 1] = a end
  end
  return out
end

--- Every placed army of a side on (x, y), last record first.
function core.onTile(g, sideIndex, x, y)
  local out = {}
  for i = #g.armies, 1, -1 do
    local a = g.armies[i]
    if a.owner == sideIndex and not a.transit and a.x == x and a.y == y then
      out[#out + 1] = a
    end
  end
  return out
end

--- Is an army still in the game?
function core.alive(g, a)
  for _, b in ipairs(g.armies) do if b == a then return true end end
  return false
end

------------------------------------------------------------- side AI data

--- A fresh block of AI data, as ai_init_side (59bf:084d) leaves it before the
--- level and card fill it in.
function core.newData(g)
  local d = {
    turns = 0,                  -- +0x00, bumped by evaluate
    own = 0, enemy = 0, neutral = 0, unseen = 0,     -- +0x02 .. +0x08
    rebuildLimit = 10,          -- +0x0a
    bold = false,               -- +0x0c bit 0: attacks sides it is not at war with
    heroes = 0,                 -- +0x0e
    rebuildType = 0, rebuildTypeRich = 0,            -- +0x10, +0x12
    bought = 0,                 -- +0x16
    dieHuman = 0, dieLord = 0, dieKnight = 0, dieWarlord = 0,  -- +0x18 .. +0x1e
    sims = 10,                  -- +0x20: battles simulated per estimate
    searchers = 0, explorers = 0,                    -- +0x22, +0x24
    raze = 0, sack = 0, pillage = 0, perCity = 0,    -- +0x26 .. +0x2c
    bonusHuman = 0, bonusWarlord = 0, bonusLord = 0, bonusKnight = 0,
    poor = 0,                   -- +0x36
    solidarity = 0,             -- +0x38
    minStrength = 0, flyCities = 0, strongCities = 0, fastCities = 0,  -- +0x3a .. +0x40
    early = 0,                  -- +0x42
    humanShare = 80,            -- +0x44
    questCity = nil,            -- +0x46
    cautious = 0,               -- +0x48
    questsDone = 0, itemsPassed = 0,                 -- +0x4a, +0x4c
    maxGroups = 1,              -- +0x24a
    roles = {}, held = {}, flags = {}, garrison = {}, keep = {},
    groups = {},
    -- battle statistics, indexed by the other side (ai_record_battle)
    heroesKilled = {}, armiesKilled = {}, battles = {}, lost = {},
    cityBattles = {}, citiesLost = {},
  }
  for i = 0, 7 do
    d.heroesKilled[i], d.armiesKilled[i], d.battles[i] = 0, 0, 0
    d.lost[i], d.cityBattles[i], d.citiesLost[i] = 0, 0, 0
  end
  for i = 1, core.MAX_GROUPS do d.groups[i] = core.emptyGroup() end
  return d
end

--- An empty assault group (5f19:08ba).
function core.emptyGroup()
  return {
    active = 0, target = nil, rally = nil,
    members = {}, cities = {}, dist = {}, bonus = {},
    staged = {}, taken = {}, size = 0, flags = 0,
  }
end

--- The AI data of a side, made on first use.
function core.data(g, side)
  if type(side) == "number" then side = g.map.sides[side + 1] end
  side.ai = side.ai or core.newData(g)
  return side.ai
end

function core.role(d, c) return d.roles[c.index] or 0 end
function core.setRole(d, c, r) d.roles[c.index] = r end
function core.cflag(d, c, bit) return core.has(d.flags[c.index], bit) end
function core.setCflag(d, c, bit) d.flags[c.index] = core.set(d.flags[c.index], bit) end
function core.clearCflag(d, c, bit) d.flags[c.index] = core.clear(d.flags[c.index], bit) end

------------------------------------------------------------ neighbours

--- The shared neighbour table (623c:0398, at game start): each city's six
--- nearest cities by land path, spread over the four directions, with the
--- path length to each. Kept on the game; built on first use.
-- the table depends on the map alone, so it is kept per scenario too
local neighbourCache = {}

function core.neighbours(g, c)
  if not g.aiNeighbours then
    -- keyed on the cities themselves, so a random map never borrows another's
    local parts = { g.map.name or "" }
    for _, o in ipairs(g.map.cities) do parts[#parts + 1] = o.x .. "," .. o.y end
    local key = table.concat(parts, ";")
    g.aiNeighbours = neighbourCache[key] or core.buildNeighbours(g)
    neighbourCache[key] = g.aiNeighbours
  end
  local e = g.aiNeighbours[c.index]
  return e and e.cities or {}, e and e.dist or {}
end

--- For every city: flood by land from it (radius 45, then 60 when that finds
--- nothing) and keep six neighbours. 623c:0749 takes them nearest first, but
--- once three lie on one side (west, east, north or south) of the city,
--- further ones on that side count 50 farther.
function core.buildNeighbours(g)
  local out = {}
  local cities = g.map.cities
  for _, c in ipairs(cities) do
    local function pick(range)
      -- 623c:0398 floods for player 0 with each city lent to it in turn;
      -- -1 is nobody, so every city but the start blocks the way
      local flood = core.flood(g, -1, c.x, c.y, range, { mode = require("warlords.move").LAND })
      local cand = {}
      for _, o in ipairs(cities) do
        if o ~= c then
          local d = core.cityDistance(g, flood, o)
          if d < 100 then cand[#cand + 1] = { city = o, d = d } end
        end
      end
      -- the best fifty, as 623c:0749 keeps them
      table.sort(cand, function(p, q) return p.d < q.d end)
      while #cand > 50 do table.remove(cand) end
      local left, right, up, down = 0, 0, 0, 0
      local picked, dists = {}, {}
      while #picked < 6 do
        local best, bestScore
        for i, e in ipairs(cand) do
          if not e.taken then
            local score = e.d
            if (e.city.x < c.x and left > 2) or (c.x < e.city.x and right > 2)
               or (e.city.y < c.y and up > 2) or (c.y < e.city.y and down > 2) then
              score = score + 50
            end
            if not bestScore or score < bestScore then best, bestScore = i, score end
          end
        end
        if not best then break end
        local e = cand[best]
        e.taken = true
        picked[#picked + 1], dists[#dists + 1] = e.city, e.d
        if e.city.x < c.x then left = left + 1 end
        if c.x < e.city.x then right = right + 1 end
        if e.city.y < c.y then up = up + 1 end
        if c.y < e.city.y then down = down + 1 end
      end
      return picked, dists, #cand
    end
    local picked, dists, found = pick(45)
    if found == 0 then picked, dists = pick(60) end
    out[c.index] = { cities = picked, dist = dists }
  end
  return out
end

------------------------------------------------------------ the selection

--- The movement points a selection shares, and how it moves.
local function refresh(g, sel)
  local move = require("warlords.move")
  local least = 100
  for _, a in ipairs(sel.armies) do least = math.min(least, a.moves or 0) end
  sel.minMoves = #sel.armies > 0 and least or 0
  sel.mode = #sel.armies > 0 and move.stackMode(g, sel.armies) or move.LAND
  sel.hero = nil
  for _, a in ipairs(sel.armies) do if core.isHero(a) and not sel.hero then sel.hero = a end end
  return sel
end

--- Select a list of armies as one stack (623c:14d3). The leader is the first.
function core.select(g, list)
  local sel = { armies = {} }
  for _, a in ipairs(list) do
    if a and core.alive(g, a) and not a.transit then sel.armies[#sel.armies + 1] = a end
  end
  sel.leader = sel.armies[1]
  return refresh(g, sel)
end

--- The rank of an army in its side's fight order -- higher leads.
local function fightRank(g, a)
  local row = g.map.fightOrder[a.owner or 8] or g.map.fightOrder[8]
  return row and row[a.type] or 0
end

--- The lowest group number from 2 up that none of the side's armies uses
--- (1b62:0cde).
function core.newGroup(g, sideIndex)
  local used = {}
  for _, a in ipairs(g.armies) do
    if a.owner == sideIndex and not a.transit and a.group then used[a.group] = true end
  end
  for n = 2, 254 do if not used[n] then return n end end
  return 0
end

--- Select a list and give it an order (623c:0ae7): the leader is the army
--- highest in the fight order; with an order, every army gets it, its
--- destination, a move target (623c:0c7e) and, when there are several, a
--- group of their own; flags 0x80 and 0x20 are cleared and `flags` set.
function core.order(g, list, order, dest, flags)
  local sel = core.select(g, list)
  if #sel.armies == 0 then return nil end
  local best
  for _, a in ipairs(sel.armies) do
    if not best or fightRank(g, a) > fightRank(g, best) then best = a end
  end
  sel.leader = best
  local grp = #sel.armies > 1 and core.newGroup(g, best.owner) or 0
  for _, a in ipairs(sel.armies) do
    if order ~= nil then
      a.aiOrder, a.aiDest = order, dest
      a.group = grp ~= 0 and grp or nil
      core.setTarget(g, a, order, dest)
    end
    a.aiNeutral, a.aiExplore = nil, nil
    if flags then
      if core.has(flags, 0x80) then a.aiNeutral = true end
      if core.has(flags, 0x20) then a.aiExplore = true end
      if core.has(flags, 0x100) then a.aiParty = true end
    end
  end
  return refresh(g, sel)
end

--- Where an order points an army (623c:0c7e): a city's bottom-right tile if
--- it is the side's own, else the corner of it nearest the army; an item's
--- or a site's tile; order 4 leaves the target as it was.
function core.setTarget(g, a, order, dest)
  if order == core.ORDER_ROAM then return end
  local x, y
  if order == core.ORDER_CITY and dest then
    local c = g.map.cities[dest + 1]
    if c then
      x, y = c.x, c.y
      if c.ownerIndex == a.owner then
        x, y = x + 1, y + 1
      else
        if x < a.x then x = x + 1 end
        if y < a.y then y = y + 1 end
      end
    end
  elseif order == core.ORDER_ITEM and dest then
    local it = g.map.items[dest + 1]
    if it and it.x then x, y = it.x, it.y end
  elseif order == core.ORDER_SITE and dest then
    local s = g.map.sites[dest + 1]
    if s then x, y = s.x, s.y end
  end
  a.target = { x = x or a.x, y = y or a.y }
end

--- Forget an army's order (the & 0xf0 / & 1 pair the original writes).
function core.clearOrder(a)
  a.aiOrder, a.aiDest = core.ORDER_NONE, nil
end

--- Select the stack an army moves with (8c07:06eb): every army of the side on
--- its tile sharing its group, or the army alone. They are marked moved.
function core.selectStackOf(g, a)
  local list = {}
  for _, b in ipairs(core.onTile(g, a.owner, a.x, a.y)) do
    if b == a or (a.group and a.group ~= 0 and b.group == a.group) then
      list[#list + 1] = b
      b.aiMoved = true
    end
  end
  local sel = core.select(g, list)
  local best
  for _, b in ipairs(sel.armies) do
    if not best or fightRank(g, b) > fightRank(g, best) then best = b end
  end
  sel.leader = best or a
  return sel
end

--- The side's armies on a tile with at least `minMoves` moves left, up to
--- eight, as a list (623c:12c9).
function core.collect(g, sideIndex, x, y, minMoves)
  local out = {}
  for _, a in ipairs(core.onTile(g, sideIndex, x, y)) do
    if #out >= rules.MAX_STACK then break end
    if not minMoves or minMoves == 0 or (a.moves or 0) >= minMoves then out[#out + 1] = a end
  end
  return out
end

--- The side's armies on a tile in group nibble `grp` with standing order
--- `order`, and a maximum move of at least `minMax` (623c:13b2).
function core.collectOrdered(g, sideIndex, x, y, grp, order, minMax)
  local out = {}
  for _, a in ipairs(core.onTile(g, sideIndex, x, y)) do
    if #out >= rules.MAX_STACK then break end
    if (a.aiGroup or 0) == (grp or 0) and (a.aiOrder or 0) == (order or 0)
       and (not minMax or minMax == 0 or (a.maxMoves or 0) >= minMax) then
      out[#out + 1] = a
    end
  end
  return out
end

------------------------------------------------------------ the flood

--- Flood the map from (x, y) the way the selection moves (1555:1ab9): every
--- tile within `range` tiles either way gets its path cost from the start,
--- plus one. The start is 1, anything unreached UNREACHED.
function core.flood(g, sideIndex, x, y, range, sel)
  local move = require("warlords.move")
  local W, H = g.map.width, g.map.height
  local grid = move.grid(g, sideIndex)
  local mode = sel and sel.mode or move.LAND
  local woods, hills = false, false
  if sel and sel.armies then
    local _, w, h = move.stackMode(g, sel.armies)
    woods, hills = w, h
  end
  local x0, x1 = math.max(0, x - range), math.min(W - 1, x + range)
  local y0, y1 = math.max(0, y - range), math.min(H - 1, y + range)
  local dist, done = {}, {}
  local start = y * W + x
  dist[start] = 0
  local heap, n = { { start, 0 } }, 1
  local function push(k, d)
    n = n + 1
    heap[n] = { k, d }
    local i = n
    while i > 1 do
      local p = math.floor(i / 2)
      if heap[p][2] <= heap[i][2] then break end
      heap[p], heap[i] = heap[i], heap[p]
      i = p
    end
  end
  local function pop()
    local top = heap[1]
    heap[1] = heap[n]; heap[n] = nil; n = n - 1
    local i = 1
    while true do
      local l, r, best = i * 2, i * 2 + 1, i
      if l <= n and heap[l][2] < heap[best][2] then best = l end
      if r <= n and heap[r][2] < heap[best][2] then best = r end
      if best == i then break end
      heap[best], heap[i] = heap[i], heap[best]
      i = best
    end
    return top
  end
  while n > 0 do
    local top = pop()
    local k = top[1]
    if not done[k] then
      done[k] = true
      local d = dist[k]
      local kx, ky = k % W, math.floor(k / W)
      for _, dir in pairs(move.DIRS) do
        local nx, ny = kx + dir[1], ky + dir[2]
        if nx >= x0 and ny >= y0 and nx <= x1 and ny <= y1 then
          local nk = ny * W + nx
          if not done[nk] then
            local c = move.stepCost(grid[k], grid[nk], mode, woods, hills, move.WATER_PENALTY)
            if c and (dist[nk] == nil or d + c < dist[nk]) then
              dist[nk] = d + c
              push(nk, d + c)
            end
          end
        end
      end
    end
  end
  return { dist = dist, W = W, H = H }
end

function core.floodAt(flood, x, y)
  local d = flood.dist[y * flood.W + x]
  return d and d + 1 or core.UNREACHED
end

-- the ring round a city's 2x2 footprint, as 59bf:0a85 walks it: from the
-- top-left, north, then east twice, south three times, west three times and
-- north three times (the directions at DS:07d8, steps at DS:2b16/2b04)
local RING = { 0, 2, 2, 4, 4, 4, 6, 6, 6, 0, 0, 0 }
local DX = { [0] = 0, 1, 1, 1, 0, -1, -1, -1 }
local DY = { [0] = -1, -1, 0, 1, 1, 1, 0, -1 }

--- The flood's distance to a city: the least over the twelve tiles round it,
--- or 100 (1000 with `far`) when none is closer. Also where (59bf:0a85).
function core.cityDistance(g, flood, c, far)
  local best = far and 1000 or 100
  local bx, by
  local x, y = c.x, c.y
  for _, dir in ipairs(RING) do
    local nx, ny = x + DX[dir], y + DY[dir]
    if nx < 0 or ny < 0 or nx >= 112 or ny >= 156 then return best, bx, by end
    x, y = nx, ny
    local d = core.floodAt(flood, x, y)
    if d < best then best, bx, by = d, x, y end
  end
  return best, bx, by
end

------------------------------------------------------------ battles

--- The chance, in percent, that the selection takes (x, y) (623c:15c5): the
--- battle is fought `sims` times without anything changing, and the wins
--- counted.
function core.odds(g, sel, x, y)
  local combat = require("warlords.combat")
  if not sel or #sel.armies == 0 then return 0 end
  local owner = sel.armies[1].owner
  local n = owner and core.data(g, owner).sims or 10
  if n < 1 then n = 1 end
  local attackers, defenders = combat.lines(g, sel.armies, x, y)
  if #defenders == 0 then return 100 end
  local wins = 0
  for _ = 1, n do
    if combat.resolve(g, attackers, defenders, x, y).won then wins = wins + 1 end
  end
  return math.floor(wins * 100 / n)
end

------------------------------------------------------------ the walk

--- Remember the stack's position for anything watching (ai.onWalk).
local function walked(g, sel, r)
  local ai = require("warlords.ai")
  if ai.walked then ai.walked(g, sel.armies, r) end
end

--- A hero picks up whatever lies where it stands (6087:0427): on a city, any
--- item on the city's four tiles; elsewhere what is on its own tile.
function core.pickUp(g, sel)
  if not sel.hero or not core.alive(g, sel.hero) then return end
  local h = sel.hero
  local c = core.cityAt(g, h.x, h.y)
  for _, it in ipairs(g.map.items) do
    if it.status == 1 and it.x and not it.planted then
      local here
      if c then
        here = it.x >= c.x and it.x <= c.x + 1 and it.y >= c.y and it.y <= c.y + 1
      else
        here = it.x == h.x and it.y == h.y
      end
      if here then
        it.status, it.x, it.y = 3, nil, nil
        h.items = h.items or {}
        h.items[#h.items + 1] = it
      end
    end
  end
end

--- Walk the selection towards (tx, ty) (1a8b:0c4f and 1a8b:07f9). Returns
--- the walk's code: 1 no path, 2 stopped short, 3 an enemy army is next,
--- 4 the path is walked, 5 an enemy city is next; with the tile ahead.
local function step(g, sel, tx, ty)
  local move = require("warlords.move")
  local lead = sel.leader
  lead.target = { x = tx, y = ty }
  local path = move.findPath(g, sel.armies, lead.x, lead.y, tx, ty)
  if not path then return 1 end
  if #path == 0 then return 4 end
  local r = move.walk(g, sel.armies, path)
  if r.steps > 0 then walked(g, sel, r) end
  refresh(g, sel)
  core.pickUp(g, sel)
  if r.stopped == "attack" then
    return r.attack.city and 5 or 3, r.attack.x, r.attack.y
  elseif r.stopped == "at peace" then
    local ahead = path[r.steps + 1]
    if ahead and core.cityAt(g, ahead.x, ahead.y) then return 5, ahead.x, ahead.y end
    return 2
  elseif r.stopped == "arrived" then
    return 4
  end
  return 2
end

--- Fight for (x, y) with the selection, for real. Returns the result.
function core.fight(g, sel, x, y)
  local game = require("warlords.game")
  -- decided, shown, and only then taken effect (after_battle, 67cc:0a6b)
  local result = game.decideAttack(g, sel.armies, x, y)
  local ai = require("warlords.ai")
  if ai.onFight then ai.onFight(g, sel.armies, x, y, result) end
  game.applyAttack(g, result)
  -- the dead are gone from the selection
  local keep = {}
  for _, a in ipairs(sel.armies) do if core.alive(g, a) then keep[#keep + 1] = a end end
  sel.armies = keep
  if not core.alive(g, sel.leader) then sel.leader = keep[1] end
  refresh(g, sel)
  return result
end

--- Mark the selection done for the turn (8c07:08fb).
function core.done(sel)
  for _, a in ipairs(sel.armies) do a.done = true end
end

--- move_stack_to (1a8b:0001) for a computer: walk to (tx, ty), fighting what
--- the walk runs into, until the walk stops. `keep` leaves the stack
--- selected and not done. Returns the last walk code.
function core.moveTo(g, sel, tx, ty, keep)
  if not sel or #sel.armies == 0 or tx < 0 or ty < 0
     or tx >= g.map.width or ty >= g.map.height then
    return 0
  end
  local me = sel.leader.owner
  local code
  local again = true
  while again do
    again = false
    if #sel.armies == 0 then break end
    local ax, ay
    code, ax, ay = step(g, sel, tx, ty)
    local lead = sel.leader
    if lead and lead.target and lead.x == lead.target.x and lead.y == lead.target.y then
      code = 4
      lead.target = nil
    end
    if code == 2 then
      core.done(sel)
    elseif code == 5 then
      local c = core.cityAt(g, tx, ty)
      if c and core.standing(c) then
        local dest = lead and lead.aiDest and g.map.cities[lead.aiDest + 1] or c
        local nx, ny
        again, nx, ny = core.attackCity(g, sel, dest, ax, ay)
        if again then tx, ty = nx, ny end
      end
    elseif code == 4 then
      if lead and lead.aiOrder == core.ORDER_SITE and sel.hero then
        local s = core.siteAt(g, sel.hero.x, sel.hero.y)
        if s then require("warlords.ai.moves").searchSite(g, sel, sel.hero, s) end
      end
      for _, a in ipairs(sel.armies) do
        core.clearOrder(a)
        a.aiGroup = 0
      end
    elseif code == 3 then
      core.fight(g, sel, ax, ay)
      if #sel.armies > 0 then again = true end
    end
  end
  if not keep then core.done(sel) end
  return code
end

--- Attack a city the walk has run into (1a8b:0254). A neutral city is
--- simply fought for. Another side's is looked at again first (623c:1771):
--- when a better target turns up, the stack is sent there instead and this
--- returns true with the new target.
function core.attackCity(g, sel, dest, ax, ay)
  local d = core.data(g, sel.leader.owner)
  local me = sel.leader.owner
  local owner = core.owner(dest)
  local city = core.cityAt(g, ax, ay)
  if owner == core.NEUTRAL then
    core.fight(g, sel, ax, ay)
    if city and city.ownerIndex == me then core.setRole(d, city, core.JUST_TAKEN) end
  elseif owner ~= me then
    local pick = dest
    if d.questCity ~= dest.index or not sel.hero then
      pick = core.retarget(g, sel, dest, ax, ay)
    end
    if pick ~= dest then
      for _, a in ipairs(sel.armies) do
        a.aiOrder, a.aiDest = core.ORDER_CITY, pick.index
      end
      return true, pick.x, pick.y
    end
    local was = city and core.owner(city)
    core.fight(g, sel, ax, ay)
    if city and city.ownerIndex == me then
      require("warlords.ai.groups").earlyVengeance(g, me, city, was)
    end
  end
  for _, a in ipairs(sel.armies) do core.clearOrder(a) end
  return false
end

--- Is the city worth attacking with this stack, or is there better nearby
--- (623c:1771)? It is fair game when neutral or at war -- and a human's,
--- one time in four, whatever the state -- and the stack wins more than
--- half its simulated battles. Otherwise the best city within 15 goes.
function core.retarget(g, sel, c, x, y)
  local me = sel.leader.owner
  local odds = core.odds(g, sel, x, y)
  local owner = core.owner(c)
  local fair = owner == core.NEUTRAL or core.state(g, owner, me) == 2
  if owner ~= core.NEUTRAL and not core.isComputer(g, owner) and g.rng:dice(1, 4, -1) == 0 then
    fair = true
  end
  if not fair or odds < 51 then
    local flood = core.flood(g, me, sel.leader.x, sel.leader.y, 15, sel)
    local best = core.bestCity(g, sel, flood, fair)
    return best or c
  end
  return c
end

--- The best city for the selection to go for, by the flood (623c:1885):
--- neutral cities, its own (at distance + 20, worth 15) and cities of sides
--- it is at war with; not the quest city nor one not yet seen. A city it
--- beats more than half the time within this turn's move scores 400 - d +
--- 10 x odds, one it has a fair chance at 100 - d + 5 x odds.
function core.bestCity(g, sel, flood, skipOwn)
  local me = sel.leader.owner
  local d = core.data(g, me)
  local best, bestScore = nil, -1
  local cities = g.map.cities
  for i = #cities, 1, -1 do
    local c = cities[i]
    local owner = core.owner(c)
    if not (skipOwn and owner == me) and core.standing(c)
       and d.questCity ~= c.index and not core.cflag(d, c, core.CF_UNSEEN)
       and (owner == core.NEUTRAL or owner == me or core.state(g, owner, me) == 2) then
      local dist, px = core.cityDistance(g, flood, c)
      if px then
        local odds
        if owner == me then
          odds, dist = 15, dist + 20
        else
          odds = core.odds(g, sel, c.x, c.y)
        end
        local score
        if odds >= 51 and dist < sel.minMoves then
          score = 400 - dist + odds * 10
        elseif odds > 10 then
          score = 100 - dist + odds * 5
        else
          score = -2
        end
        if score > bestScore then best, bestScore = c, score end
      end
    end
  end
  return best
end

return core
