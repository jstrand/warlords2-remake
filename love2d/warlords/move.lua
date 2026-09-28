-- Movement: the cost grid, stack modes, pathfinding and walking a path.
--
-- This follows WARLORD2.EXE rather than reinventing: the original builds a
-- one-byte-per-tile cost grid for the moving side (`path_build_cost_grid`,
-- Ghidra 1555:0d9e), runs a wavefront over it (`path_wavefront`, 1555:0373)
-- and walks the resulting path (`walk_path`, 1a8b:07f9). See docs/rules.md >
-- Movement and > Moving a stack.

local armytype = require("warlords.armytype")
local rules    = require("warlords.rules")
local scn      = require("warlords.scn")

local move = {}

-- terrain type ids, as the scenario's own tile -> type table numbers them
move.ROAD, move.BRIDGE, move.WATER, move.SHORE = 0, 1, 2, 3
move.FOREST, move.HILLS, move.MOUNTAINS, move.PLAIN = 4, 5, 6, 7
move.MARSH, move.TOWER, move.CITY, move.SITE = 8, 9, 10, 11

move.TERRAIN_NAMES = {
  [0] = "road", "bridge", "water", "shore", "forest", "hills",
  "mountains", "plain", "marsh", "tower", "city", "site",
}

-- DS:1274 in WARLORD2.EXE. 0 = impassable.
move.COST = { [0] = 1, 1, 1, 2, 4, 6, 0, 2, 5, 2, 1, 2 }

-- cost-grid flags
move.WATER_F, move.CROSS_F = 0x08, 0x10
move.HILLS_F, move.FOREST_F, move.CITY_F = 0x20, 0x40, 0x80

-- stack modes, as the original numbers them
move.BOAT, move.LAND, move.FLYING = 0, 1, 2

-- The cost grid packs a cost and four flags into one number, exactly as the
-- original packs them into one byte. LOVE runs LuaJIT, which has no bitwise
-- operators, so the two accessors below do it with arithmetic: every flag is a
-- distinct power of two above the 3-bit cost.
local function has(byte, flag)
  return byte % (flag + flag) >= flag
end

local function with(byte, flag)
  return has(byte, flag) and byte or byte + flag
end

move.has = has

-- The water charge only steers the route: the wavefront adds it where a land
-- stack comes out of open water, but the walk never spends it.
move.WATER_PENALTY = 10       -- a land stack's route leaving open water
move.WATER_PENALTY_TO = 20    -- ... when the whole move is aimed at water
move.PAST_SHORE = 0x80        -- a step after going to sea or ashore (1555:19e4)
move.MIN_MOVE_LEFT = 2        -- the walk stops below this
move.MAX_PATH = 200           -- a path is at most 200 compass steps

-- compass directions, 0 = north, clockwise (the order the game stores paths in)
move.DIRS = {
  [0] = { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 },
  { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 },
}

--------------------------------------------------------------- stack modes

--- How a stack moves, and what bonuses it carries.
-- Returns mode, woodsBonus, hillsBonus, atSea. docs/rules.md > How a stack moves.
function move.stackMode(g, stack)
  local atSea, anyBoat = false, false
  local allFly, anyFly, allNonHeroFly, heroFlight = true, false, true, false
  local woods, hills = false, false

  for _, a in ipairs(stack) do
    local t = g.types.byId[a.type]
    if a.atSea then atSea = true end
    if t.boat then anyBoat = true end
    if t.flies and not a.atSea then anyFly = true end
    if not t.flies or a.atSea then
      allFly = false
      if a.type ~= armytype.HERO then allNonHeroFly = false end
    end
    if a.type == armytype.HERO and a.items then
      for _, it in ipairs(a.items) do
        if it.type == rules.ITEM_FLIGHT then heroFlight = true end
      end
    end
    if t.woodsMove then woods = true end
    if t.hillsMove then hills = true end
  end

  -- at sea the bonus flags are the sea flag alone: no woods or hills bonus
  if atSea then return move.LAND, false, false, true end
  if anyBoat then return move.BOAT, woods, hills, false end
  if heroFlight or allFly or (allNonHeroFly and anyFly) then
    return move.FLYING, woods, hills, false
  end
  return move.LAND, woods, hills, false
end

--------------------------------------------------------------- the cost grid

-- 1555:109d walks round a city from its top left tile, a step at a time in
-- these directions (4125:01f8): the twelve tiles that border it
local PORT_TOUR = { 0, 2, 2, 4, 4, 4, 6, 6, 6, 0, 0, 0 }

--- Is a city a port -- does any tile bordering its 2x2 footprint hold a
--- bridge, water or shore? Only a port city lets a stack change between land
--- and sea. 1555:109d, which gives up -- not a port -- the moment its walk
--- round the city would leave the map.
function move.isPort(g, city)
  local x, y = city.x, city.y
  for _, d in ipairs(PORT_TOUR) do
    x, y = x + move.DIRS[d][1], y + move.DIRS[d][2]
    if x < 0 or y < 0 or x >= g.map.width or y >= g.map.height then return false end
    local t = scn.terrainAt(g.map, x, y)
    if t == move.BRIDGE or t == move.WATER or t == move.SHORE then return true end
  end
  return false
end

--- Build the cost grid for one side (path_build_cost_grid, 1555:0d9e).
--- Cached on the game state; call move.invalidate(g) when city ownership
--- changes. What a side has not seen is not in it: the search itself shuts
--- that out (path_prepare_grid, 1555:08bf; see findPath).
function move.grid(g, sideIndex)
  g.grids = g.grids or {}
  local cached = g.grids[sideIndex or -1]
  if cached then return cached end

  local W, H = g.map.width, g.map.height
  local grid = {}
  for y = 0, H - 1 do
    for x = 0, W - 1 do
      local i = y * W + x
      local terrain = scn.terrainAt(g.map, x, y)
      local byte = scn.roadAt(g.map, x, y) % 0x20 ~= 0 and 1 or move.COST[terrain]
      if terrain == move.BRIDGE then
        byte = with(with(byte, move.CROSS_F), move.WATER_F)
      elseif terrain == move.WATER or terrain == move.SHORE then
        byte = with(byte, move.WATER_F)
      elseif terrain == move.FOREST then
        byte = with(byte, move.FOREST_F)
      elseif terrain == move.HILLS then
        byte = with(byte, move.HILLS_F)
      end
      if g.map.crossing[i + 1] then byte = with(byte, move.CROSS_F) end
      grid[i] = byte
    end
  end

  -- cities occupy a 2x2 footprint: passable only to their owner, and a port
  -- city of the moving side is also a crossing. A razed city's tiles are
  -- ruins, not city ground, and so are no city at all; but the ruins of a
  -- port are a crossing for every side (the second loop of 1555:0d9e).
  for _, c in ipairs(g.map.cities) do
    local mine = c.ownerIndex == sideIndex and not c.razed
    local port = (mine or c.razed) and move.isPort(g, c)
    for dx = 0, 1 do
      for dy = 0, 1 do
        local x, y = c.x + dx, c.y + dy
        if x < W and y < H then
          local i = y * W + x
          local byte = grid[i]
          if c.razed then
            if port then byte = with(with(byte, move.CROSS_F), move.WATER_F) end
          else
            byte = with(byte, move.CITY_F)
            if mine then
              if port then byte = with(with(byte, move.CROSS_F), move.WATER_F) end
            else
              byte = byte - (byte % 8)          -- cost 0: impassable
            end
          end
          grid[i] = byte
        end
      end
    end
  end

  g.grids[sideIndex or -1] = grid
  return grid
end

--- Drop the cached grids. Call after a city changes hands.
function move.invalidate(g)
  g.grids = nil
end

--------------------------------------------------------------------- costs

--- The wavefront's weight for spreading from tile `from` to tile `to`, or nil
--- if it cannot spread there (1555:0373). Both are cost-grid bytes. The
--- original floods out from the destination, so `from` is the tile nearer
--- it; `penalty` is the water charge (10, or 20 when the move is aimed at
--- water), added where a land stack's walk would come out of open water.
--- This picks the route only: move.walkCost is what the walk spends.
function move.stepCost(fromByte, toByte, mode, woods, hills, penalty)
  local cost = toByte % 8
  local toWater, fromWater = has(toByte, move.WATER_F), has(fromByte, move.WATER_F)
  local toCross = has(toByte, move.CROSS_F)

  if mode == move.FLYING then
    -- never into a city that is not its own (1555:08bf shuts every city of
    -- cost 0 out of a flier's search)
    if cost == 0 and has(toByte, move.CITY_F) then return nil end
    -- 2 per tile, water and mountains included; 1 where the tile costs 1
    if toWater and not toCross then return 2 end
    if cost == 0 or cost > 2 then return 2 end
    return cost
  end

  if mode == move.BOAT then
    -- a boat keeps to water and cities
    if not toWater and not has(toByte, move.CITY_F) then return nil end
    return cost == 0 and nil or cost
  end

  if cost == 0 then return nil end             -- mountains, enemy cities

  -- land: the shoreline may only be crossed at a crossing tile
  if toWater ~= fromWater and not toCross and not has(fromByte, move.CROSS_F) then
    return nil
  end
  if cost > 2 then
    if woods and has(toByte, move.FOREST_F) then cost = 2 end
    if hills and has(toByte, move.HILLS_F) then cost = 2 end
  end
  -- the flood reaching open water from land (or a crossing) costs extra
  if (not fromWater or has(fromByte, move.CROSS_F)) and toWater and not toCross then
    cost = cost + penalty
  end
  return cost
end

--- The moves a walk spends stepping onto a tile (1555:19e4): the tile's own
--- cost, 2 for a flier where that is more, and the woods and hills bonuses on
--- land. No water charge.
function move.walkCost(byte, mode, woods, hills)
  local cost = byte % 8
  if mode == move.FLYING then
    if has(byte, move.WATER_F) and not has(byte, move.CROSS_F) then return 2 end
    if cost == 0 or cost > 2 then return 2 end
    return cost
  end
  if mode == move.LAND and cost > 2 then
    if woods and has(byte, move.FOREST_F) then cost = 2 end
    if hills and has(byte, move.HILLS_F) then cost = 2 end
  end
  return cost
end

--- Does a land stack stepping onto this tile go to sea (or, `atSea`, come
--- ashore)? Water from land, or land from the sea, where the tile is no
--- crossing. The walk ends there: every step after it costs PAST_SHORE.
function move.crossesShore(byte, atSea)
  if has(byte, move.CROSS_F) then return false end
  return has(byte, move.WATER_F) ~= (atSea and true or false)
end

--- The movement points a stack shares: the lowest of its armies'.
function move.stackMoves(stack)
  local least = math.huge
  for _, a in ipairs(stack) do least = math.min(least, a.moves or 0) end
  return least == math.huge and 0 or least
end

------ The route a stack would take to (x, y) and how much of it it can walk
--- now: every tile of the path in order, and how many of them are within
--- this turn's movement. That is what the map draws as rings -- plain while
--- the stack can still get there this turn, crossed once it cannot
--- (8611:2ef5, docs/re/ui.md > Walking).
function move.preview(g, stack, x, y)
  if #stack == 0 then return nil end
  local path = move.findPath(g, stack, stack[1].x, stack[1].y, x, y)
  if not path or #path == 0 then return nil end
  local left, reach = move.stackMoves(stack), 0
  for i, step in ipairs(path) do
    if left < move.MIN_MOVE_LEFT or step.cost > left then break end
    left = left - step.cost
    reach = i
  end
  return { path = path, reach = reach }
end

--------------------------------------------------------------- pathfinding

-- The search, as 1555:000a runs it. It works on one number a tile
-- (path_prepare_grid, 1555:08bf): UNREACHED until the flood gets there,
-- SHUT where the stack may never go, and then the distance from the
-- destination -- negative while the tile still has to spread it further,
-- positive once it has.
local UNREACHED, SHUT = 30000, 30001
move.SHUT = SHUT

-- Where the wavefront spreads from a tile, by where the tile lies: inside,
-- or along one of the map's edges or corners (4125:00e2, 00f6, 0162)
local NB_FIRST = { [0] = 0, 9, 13, 19, 23, 29, 35, 39, 45, 49 }
local NB_DX = { [0] = -1, 0, 1, 1, 1, 0, -1, -1, -10, 1, 1, 0, -10, 1, 1, 0, -1, -1,
  -10, 0, -1, -1, -10, 0, 1, 1, 1, 0, -10, -1, 0, 0, -1, -1, -10, 0, 1, 1, -10,
  -1, 0, 1, 1, -1, -10, -1, 0, -1, -10, 0, 1, 0, -1, -10 }
local NB_DY = { [0] = -1, -1, -1, 0, 1, 1, 1, 0, -10, 0, 1, 1, -10, 0, 1, 1, 1, 0,
  -10, 1, 1, 0, -10, -1, -1, 0, 1, 1, -10, -1, -1, 1, 1, 0, -10, -1, -1, 0, -10,
  -1, -1, -1, 0, 0, -10, -1, -1, 0, -10, -1, 0, 1, 0, -10 }

-- the order path_trace tries the neighbours in, turning from straight at the
-- destination (4125:0212)
local TRACE_ORDER = { 0, 7, 1, 6, 2, 5, 3, 4 }

--- The compass direction from one tile towards another (1a8b:0ae4): by
--- the signs alone, so anything up and to the right is north-east.
function move.direction(x1, y1, x2, y2)
  if x1 == x2 and y1 == y2 then return nil end
  if x1 == x2 then return y2 < y1 and 0 or 4 end
  if y1 == y2 then return x2 < x1 and 6 or 2 end
  if x1 <= x2 then return y2 < y1 and 1 or 3 end
  return y2 < y1 and 7 or 5
end

--- map_distance (1a8b:0acc): the straight-line distance, rounded down.
function move.distance(x1, y1, x2, y2)
  local ax, ay = x1 - x2, y1 - y2
  return math.floor(math.sqrt(ax * ax + ay * ay))
end

local function absval(v) return v < 0 and -v or v end

-- path_prepare_grid (1555:08bf): nothing reached, and what the stack can
-- never enter shut -- a land stack's impassable ground, a boat's dry land, a
-- flier's cities that are not its own. With Hidden Map on and a human
-- moving, ground it has not seen is shut too: all but the destination for a
-- land stack, all but the destination's own row and column for a boat or a
-- flier, as the original tests them.
function move.prepare(g, grid, sideIndex, mode, dx, dy)
  local W, H = g.map.width, g.map.height
  local side = sideIndex ~= nil and g.map.sides[sideIndex + 1]
  local fog = g.map.options.hiddenMap ~= 0 and side and not side.computer
  local seen = fog and require("warlords.game").seen
  local dist = {}
  for y = 0, H - 1 do
    for x = 0, W - 1 do
      local k = y * W + x
      local b = grid[k]
      local hidden = fog and not seen(g, sideIndex, x, y)
      local shut
      if mode == move.LAND then
        shut = b % 8 == 0 or (hidden and not (x == dx and y == dy))
      elseif mode == move.BOAT then
        shut = not has(b, move.WATER_F) or (hidden and x ~= dx and y ~= dy)
      else
        shut = (b % 8 == 0 and has(b, move.CITY_F)) or (hidden and x ~= dx and y ~= dy)
      end
      dist[k] = shut and SHUT or UNREACHED
    end
  end
  return dist
end

-- path_wavefront (1555:0373). Not a best-first search: it sweeps squares
-- ever wider round the destination, each tile still to spread passing its
-- distance on to its neighbours -- and taking back any it learns a shorter
-- way to -- and stops once the sweep has passed the start. Only tiles in
-- the rectangle round start and destination, `margin` wider on every side,
-- spread: 6 on the first pass, 50 on the second, which carries on from the
-- square the first had reached. 1 when the start was reached.
local function wavefront(q, pass)
  local W, H = q.W, q.H
  local sx, sy, dx, dy = q.sx, q.sy, q.dx, q.dy
  local dist, grid, mode = q.dist, q.grid, q.mode
  local margin = pass == 0 and 6 or 50
  if pass == 0 then q.ring = 0 end
  local lox, loy = math.min(sx, dx), math.min(sy, dy)
  local hix, hiy = math.max(sx, dx), math.max(sy, dy)
  local bx0 = lox < margin and 0 or lox - margin
  local by0 = loy < margin and 0 or loy - margin
  local bx1 = hix + margin < W and hix + margin or W - 1
  local by1 = hiy + margin < H and hiy + margin or H - 1
  local LAND, FLYING, BOAT = move.LAND, move.FLYING, move.BOAT
  local WATER, CROSS, CITY = move.WATER_F, move.CROSS_F, move.CITY_F
  local found, going = 1, true
  while going do
    local any = false
    local r = q.ring
    -- the square at this distance, as far as it lies in the rectangle
    local x0, x1 = math.max(dx - r, 0, bx0), math.min(dx + r, W - 1, bx1)
    local y0, y1 = math.max(dy - r, 0, by0), math.min(dy + r, H - 1, by1)
    for x = x0, x1 do
      for y = y0, y1 do
        local k = y * W + x
        local v = dist[k]
        if v < 1 then
          any = true
          local cur = grid[k]
          local nd = -v
          dist[k] = nd
          local curWater, curCross = has(cur, WATER), has(cur, CROSS)
          local class = 0
          if y == 0 then class = x == 0 and 1 or (x == W - 1 and 3 or 2)
          elseif y == H - 1 then class = x == 0 and 6 or (x == W - 1 and 8 or 7)
          elseif x == 0 then class = 4
          elseif x == W - 1 then class = 5 end
          local i = NB_FIRST[class]
          while NB_DX[i] ~= -10 do
            local nx, ny = x + NB_DX[i], y + NB_DY[i]
            i = i + 1
            local nk = ny * W + nx
            local nv = dist[nk]
            if nv ~= SHUT then
              local nb = grid[nk]
              local c = nb % 8
              local ok = true
              local nWater, nCross = has(nb, WATER), has(nb, CROSS)
              if nx == sx and ny == sy then
                -- stepping off the start costs 1, and never over the shoreline
                if mode == LAND and not curCross and not nCross and nWater ~= curWater then
                  ok = false
                end
                c = 1
              elseif mode == FLYING then
                if (not nWater or nCross) and c ~= 0 and c < 3 then
                  -- as dear as the tile, where that is 1 or 2
                else
                  c = 2
                end
              elseif mode == BOAT then
                if not nWater and not has(nb, CITY) then
                  dist[nk] = SHUT
                  ok = false
                end
              else
                if not curCross and not nCross and nWater ~= curWater then
                  ok = false
                else
                  if c > 2 and ((q.hills and has(nb, move.HILLS_F))
                                or (q.woods and has(nb, move.FOREST_F))) then
                    c = 2
                  end
                  -- the walk coming out of open water onto land or a crossing
                  if (not curWater or curCross) and nWater and not nCross then
                    c = c + q.penalty
                  end
                end
              end
              if ok and nd + c < absval(nv) then dist[nk] = v - c end
            end
          end
          if x == sx and y == sy then going = false end
        end
      end
    end
    q.ring = r + 1
    if not any then going, found = false, 0 end
  end
  return found
end

-- path_trace (1555:117b): from the start, each step to whichever neighbour
-- the flood put nearest the destination -- strictly nearer than here -- the
-- first found in TRACE_ORDER winning a tie; a land stack does not step over
-- the shoreline except at a crossing. At most 198 steps; where it can go no
-- further the path simply ends there.
local function trace(q)
  local W, H = q.W, q.H
  local dist, grid = q.dist, q.grid
  local x, y = q.sx, q.sy
  local tx, ty = q.dx, q.dy
  local d = absval(dist[y * W + x])
  local steps = {}
  while not (x == tx and y == ty) do
    local cur = grid[y * W + x]
    local land = q.mode == move.LAND
    local curWater = land and has(cur, move.WATER_F)
    local curCross = land and has(cur, move.CROSS_F)
    local toward = move.direction(x, y, tx, ty)
    local bx, by, bdir
    for _, turn in ipairs(TRACE_ORDER) do
      local dir = (toward + turn) % 8
      local nx, ny = x + move.DIRS[dir][1], y + move.DIRS[dir][2]
      if nx >= 0 and ny >= 0 and nx < W and ny < H then
        local v = dist[ny * W + nx]
        if v ~= UNREACHED and v ~= SHUT then
          local nb = grid[ny * W + nx]
          local blocked = land and not curCross and not has(nb, move.CROSS_F)
                          and has(nb, move.WATER_F) ~= curWater
          if not blocked and absval(v) < d then
            bx, by, bdir, d = nx, ny, dir, absval(v)
          end
        end
      end
    end
    if bx and #steps < 198 then
      x, y = bx, by
      steps[#steps + 1] = { x = x, y = y, dir = bdir }
    else
      tx, ty = x, y
    end
  end
  return steps
end

-- path_single_step (1555:020e): a destination one tile away (diagonals
-- included, map_distance being 1) is simply stepped to, unless a land or
-- boat stack would cross between land and water, or the ground there is
-- impassable. Its terrain alone decides, whoever holds it.
local function singleStep(g, mode, sx, sy, dx, dy)
  local function wet(t) return t == move.WATER or t == move.SHORE end
  local from, to = scn.terrainAt(g.map, sx, sy), scn.terrainAt(g.map, dx, dy)
  if mode ~= move.FLYING then
    if wet(from) ~= wet(to) then return false end
    if move.COST[to] == 0 then return false end
  end
  return true
end

--- The path from (sx,sy) to (dx,dy) for a stack, as a list of { x, y, cost }
--- steps, or nil if there is no route: 1555:000a, step for step. The
--- original also keeps its last paths to hand them back unasked for
--- (1555:1508); with nothing changed in between they are the same path, so
--- that is not copied.
function move.findPath(g, stack, sx, sy, dx, dy)
  if sx == dx and sy == dy then return {} end
  local W, H = g.map.width, g.map.height
  if dx < 0 or dy < 0 or dx >= W or dy >= H then return nil end

  local side = stack[1] and stack[1].owner
  local mode, woods, hills, atSea = move.stackMode(g, stack)
  local grid = move.grid(g, side)

  local path, restore = nil, {}
  if move.distance(dx, dy, sx, sy) == 1 and singleStep(g, mode, sx, sy, dx, dy) then
    path = { { x = dx, y = dy } }
  else
    -- a stack may always path *to* a city, just never through one it does
    -- not own: its footprint opens at the city cost for this search, and a
    -- port is a crossing, so a stack at sea can attack it (path_mark_cities,
    -- 1555:0bde)
    local goal = dy * W + dx
    local goalByte = grid[goal]
    local city = g.map.cityTile[goal]
    if city and goalByte % 8 == 0 and has(goalByte, move.CITY_F) then
      local port = move.isPort(g, city)
      for ox = 0, 1 do
        for oy = 0, 1 do
          local x, y = city.x + ox, city.y + oy
          if x < W and y < H then
            local k = y * W + x
            restore[k] = grid[k]
            local byte = grid[k] - grid[k] % 8 + move.COST[move.CITY]
            if port then byte = with(with(byte, move.CROSS_F), move.WATER_F) end
            grid[k] = byte
          end
        end
      end
    end

    -- the water charge, set once for the move: 20 when it is aimed at water
    local penalty = 0
    if mode == move.LAND then
      local t = scn.terrainAt(g.map, dx, dy)
      penalty = (t == move.WATER or t == move.SHORE) and move.WATER_PENALTY_TO
                or move.WATER_PENALTY
    end
    local q = { W = W, H = H, sx = sx, sy = sy, dx = dx, dy = dy, grid = grid,
                mode = mode, woods = woods, hills = hills, penalty = penalty,
                dist = move.prepare(g, grid, side, mode, dx, dy) }
    local found = 0
    if q.dist[goal] ~= SHUT or has(grid[goal], move.CITY_F) then
      q.dist[goal] = -1
      for pass = 0, 1 do
        if found == 0 then found = wavefront(q, pass) end
      end
    end
    if found == 1 then path = trace(q) end
  end

  -- what the walk spends on each step (path_step_costs, 1555:18be): a land
  -- stack's move ends where it goes to sea or comes ashore
  local ashore = false
  for _, step in ipairs(path or {}) do
    local byte = grid[step.y * W + step.x]
    if ashore then
      step.cost = move.PAST_SHORE
    else
      step.cost = move.walkCost(byte, mode, woods, hills)
      ashore = mode == move.LAND and move.crossesShore(byte, atSea)
    end
    step.dir = nil
  end
  for k, byte in pairs(restore) do grid[k] = byte end
  if path and #path == 0 then path = nil end
  return path
end

--------------------------------------------------------------- walking a path

local armiesAt   -- resolved lazily: game.lua requires this module back
local function stackAt(g, x, y)
  if not armiesAt then armiesAt = require("warlords.game").armiesAt end
  return armiesAt(g, x, y)
end

--- Walk `stack` along `path`, stopping where the rules say to stop.
-- Returns { steps, spent, stopped, attack }. `stopped` is one of "arrived",
-- "out of moves", "blocked" or "attack"; on "attack" the caller resolves the
-- fight and may walk again. docs/rules.md > Moving a stack.
function move.walk(g, stack, path)
  local side = stack[1] and stack[1].owner
  local left = move.stackMoves(stack)
  local result = { steps = 0, spent = 0, stopped = "arrived" }
  local lastOk, cumulative, costTo = 0, 0, {}

  for i, step in ipairs(path) do
    if left < move.MIN_MOVE_LEFT or step.cost > left then
      result.stopped = "out of moves"
      break
    end

    local city = g.map.cityTile[step.y * g.map.width + step.x]
    local here = stackAt(g, step.x, step.y)
    local diplomacy = require("warlords.diplomacy")
    if city and not city.razed and city.ownerIndex ~= side then
      if not diplomacy.mayAttack(g, side, city.ownerIndex) then
        result.stopped = "at peace"
        break
      end
      result.stopped = "attack"
      result.attack = { x = step.x, y = step.y, city = city }
      break
    end
    if here[1] and here[1].owner ~= side then
      -- a tile held by a side we are at peace with simply blocks
      if not diplomacy.mayAttack(g, side, here[1].owner) then
        result.stopped = "at peace"
        break
      end
      result.stopped = "attack"
      result.attack = { x = step.x, y = step.y }
      break
    end

    left = left - step.cost
    cumulative = cumulative + step.cost
    -- the stack may only *stop* where it fits; other steps are passed over
    if #here + #stack <= rules.MAX_STACK then
      lastOk, costTo[i] = i, cumulative
    end
  end

  if lastOk == 0 then
    if result.stopped == "arrived" then result.stopped = "blocked" end
    return result
  end

  local cost = costTo[lastOk]
  local dest = path[lastOk]
  local mode, _, _, wasAtSea = move.stackMode(g, stack)
  for _, a in ipairs(stack) do
    a.x, a.y = dest.x, dest.y
    a.moves = math.max(0, (a.moves or 0) - cost)
  end
  -- going to sea or coming ashore uses up the move of every army that did
  -- (1a8b:0d1a), and the walk ends there whatever it met (1a8b:0c4f)
  local change = move.settleSea(g, stack, dest.x, dest.y, mode, wasAtSea)
  if change then
    for _, a in ipairs(stack) do
      if (a.atSea and true or false) == (change == "to sea") then a.moves = 0 end
    end
    result.stopped, result.attack = "out of moves", nil
  end

  -- walking uncovers the map as it goes
  if g.map.options.hiddenMap ~= 0 and side ~= nil then
    local gameMod = require("warlords.game")
    local flying = move.stackMode(g, stack) == move.FLYING
    local found = 0
    for _, step in ipairs(path) do
      if step == dest then break end
      found = found + gameMod.reveal(g, side, step.x, step.y, flying)
    end
    found = found + gameMod.reveal(g, side, dest.x, dest.y, flying)
    if found > 0 then move.invalidate(g) end
  end

  result.steps, result.spent = lastOk, cost
  -- the tiles actually walked, in order, so the map can play the walk back
  result.walked = {}
  for i = 1, lastOk do result.walked[i] = { x = path[i].x, y = path[i].y } end
  if lastOk < #path and result.stopped == "arrived" then result.stopped = "blocked" end
  return result
end

--- Going to sea and coming ashore, at the end of a walk (1a8b:04c8). Only a
--- stack moving as a land stack changes: flying and boat moves leave it as
--- it was. A land stack ending on water or a shore puts to sea -- every army
--- in it that cannot fly, and a hero too unless a flier goes with it and
--- nothing else walks. One at sea ending on land (not water, shore or a
--- bridge) comes ashore, every army. Returns "to sea" or "ashore" when that
--- happened, nil otherwise.
function move.settleSea(g, stack, x, y, mode, wasAtSea)
  if mode ~= move.LAND then return nil end
  local t = scn.terrainAt(g.map, x, y)
  if not wasAtSea then
    if t ~= move.WATER and t ~= move.SHORE then return nil end
    local flier, walker = false, false
    for _, a in ipairs(stack) do
      if g.types.byId[a.type].flies then flier = true
      elseif a.type ~= armytype.HERO then walker = true end
    end
    if walker then flier = false end
    local change
    for _, a in ipairs(stack) do
      if not g.types.byId[a.type].flies and (a.type ~= armytype.HERO or not flier) then
        a.atSea = true
        change = "to sea"
      end
    end
    return change
  elseif t ~= move.WATER and t ~= move.SHORE and t ~= move.BRIDGE then
    for _, a in ipairs(stack) do a.atSea = false end
    return "ashore"
  end
  return nil
end

--- Move a stack towards a tile: pathfind, then walk.
-- move_stack_to, Ghidra 1a8b:0001.
function move.moveTo(g, stack, x, y)
  if #stack == 0 then return { steps = 0, spent = 0, stopped = "blocked" } end
  local path = move.findPath(g, stack, stack[1].x, stack[1].y, x, y)
  if not path then return { steps = 0, spent = 0, stopped = "no route" } end
  if #path == 0 then return { steps = 0, spent = 0, stopped = "arrived" } end
  return move.walk(g, stack, path)
end

return move
