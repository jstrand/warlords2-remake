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

move.WATER_PENALTY = 10       -- a land stack stepping into open water
move.WATER_PENALTY_TO = 20    -- ... when the whole move is aimed at water
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

  if atSea then return move.LAND, woods, hills, true end
  if anyBoat then return move.BOAT, woods, hills, false end
  if heroFlight or allFly or (allNonHeroFly and anyFly) then
    return move.FLYING, woods, hills, false
  end
  return move.LAND, woods, hills, false
end

--------------------------------------------------------------- the cost grid

--- Is a city a port -- does any tile of its 2x2 footprint touch bridge, water
--- or shore? Only a port city lets a stack change between land and sea.
-- FUN_1555_109d.
local function isPort(g, city)
  for dx = -1, 2 do
    for dy = -1, 2 do
      local x, y = city.x + dx, city.y + dy
      if x >= 0 and y >= 0 and x < g.map.width and y < g.map.height then
        local t = scn.terrainAt(g.map, x, y)
        if t == move.BRIDGE or t == move.WATER or t == move.SHORE then return true end
      end
    end
  end
  return false
end

--- Build the cost grid for one side. Cached on the game state; call
--- move.invalidate(g) when city ownership changes -- or when the side sees
--- more of the map.
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
        byte = byte | move.CROSS_F | move.WATER_F
      elseif terrain == move.WATER then
        byte = byte | move.WATER_F
      elseif terrain == move.FOREST then
        byte = byte | move.FOREST_F
      elseif terrain == move.HILLS then
        byte = byte | move.HILLS_F
      end
      if g.map.crossing[i + 1] then byte = byte | move.CROSS_F end
      grid[i] = byte
    end
  end

  -- cities occupy a 2x2 footprint: passable only to their owner, and a port
  -- city of the moving side is also a crossing
  for _, c in ipairs(g.map.cities) do
    local mine = c.ownerIndex == sideIndex
    local port = mine and isPort(g, c)
    for dx = 0, 1 do
      for dy = 0, 1 do
        local x, y = c.x + dx, c.y + dy
        if x < W and y < H then
          local i = y * W + x
          local byte = grid[i] | move.CITY_F
          if mine then
            if port then byte = byte | move.CROSS_F | move.WATER_F end
          else
            byte = byte - (byte % 8)          -- cost 0: impassable
          end
          grid[i] = byte
        end
      end
    end
  end

  -- with Hidden Map on, a side cannot path through what it has not seen
  if g.map.options.hiddenMap ~= 0 and sideIndex ~= nil then
    local gameMod = require("warlords.game")
    for i = 0, W * H - 1 do
      if not gameMod.seen(g, sideIndex, i % W, i // W) then
        grid[i] = grid[i] - (grid[i] % 8)          -- cost 0: impassable
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

--- The cost of stepping onto tile `to` from tile `from`, or nil if the step
--- cannot be taken. Both are cost-grid bytes. `penalty` is the water entry
--- charge for this move (10, or 20 when the move is aimed at water).
function move.stepCost(fromByte, toByte, mode, woods, hills, penalty)
  local cost = toByte % 8
  local toWater, fromWater = toByte & move.WATER_F, fromByte & move.WATER_F
  local toCross = toByte & move.CROSS_F

  if mode == move.FLYING then
    -- 2 per tile, water and mountains included; 1 where the tile costs 1
    if toWater ~= 0 and toCross == 0 then return 2 end
    if cost == 0 or cost > 2 then return 2 end
    return cost
  end

  if mode == move.BOAT then
    -- a boat keeps to water and cities
    if toWater == 0 and (toByte & move.CITY_F) == 0 then return nil end
    return cost == 0 and nil or cost
  end

  if cost == 0 then return nil end             -- mountains, enemy cities

  -- land: the shoreline may only be crossed at a crossing tile
  if toWater ~= fromWater
     and toCross == 0 and (fromByte & move.CROSS_F) == 0 then
    return nil
  end
  if cost > 2 then
    if woods and (toByte & move.FOREST_F) ~= 0 then cost = 2 end
    if hills and (toByte & move.HILLS_F) ~= 0 then cost = 2 end
  end
  -- stepping out of land (or off a crossing) into open water costs extra
  if (fromWater == 0 or (fromByte & move.CROSS_F) ~= 0)
     and toWater ~= 0 and toCross == 0 then
    cost = cost + penalty
  end
  return cost
end

--- The movement points a stack shares: the lowest of its armies'.
function move.stackMoves(stack)
  local least = math.huge
  for _, a in ipairs(stack) do least = math.min(least, a.moves or 0) end
  return least == math.huge and 0 or least
end

------------------------------------------------------------------ pathfinding

--- Cheapest path from (sx,sy) to (dx,dy) for a stack, as a list of
--- { x, y, cost } steps, or nil if there is no route.
--
-- The original floods the whole map from the destination; this is A* with a
-- Chebyshev heuristic, which is admissible because no step costs less than 1
-- and diagonals are allowed. Same paths, a fraction of the tiles visited.
function move.findPath(g, stack, sx, sy, dx, dy)
  if sx == dx and sy == dy then return {} end
  local W, H = g.map.width, g.map.height
  if dx < 0 or dy < 0 or dx >= W or dy >= H then return nil end

  local side = stack[1] and stack[1].owner
  local mode, woods, hills = move.stackMode(g, stack)
  local grid = move.grid(g, side)

  -- the destination's own terrain decides the water charge for the whole move
  local destTerrain = scn.terrainAt(g.map, dx, dy)
  local penalty = (destTerrain == move.WATER or destTerrain == move.SHORE)
                  and move.WATER_PENALTY_TO or move.WATER_PENALTY

  -- a stack may always path *to* a city, just never through one it does not own
  local goal = dy * W + dx
  local goalByte = grid[goal]
  local restore
  if goalByte % 8 == 0 and (goalByte & move.CITY_F) ~= 0 then
    restore, grid[goal] = goalByte, goalByte | 1
  end

  local dist, prev, done = {}, {}, {}
  local start = sy * W + sx
  dist[start] = 0

  local function heuristic(k)
    local x, y = k % W, k // W
    local ax, ay = x - dx, y - dy
    if ax < 0 then ax = -ax end
    if ay < 0 then ay = -ay end
    return ax > ay and ax or ay
  end

  local heap, n = { { start, heuristic(start) } }, 1
  local function push(k, d)
    n = n + 1
    heap[n] = { k, d }
    local i = n
    while i > 1 do
      local p = i // 2
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
    if k == goal then break end
    if not done[k] then
      done[k] = true
      local d = dist[k]
      local x, y = k % W, k // W
      for _, dir in pairs(move.DIRS) do
        local nx, ny = x + dir[1], y + dir[2]
        if nx >= 0 and ny >= 0 and nx < W and ny < H then
          local nk = ny * W + nx
          if not done[nk] then
            local c = move.stepCost(grid[k], grid[nk], mode, woods, hills, penalty)
            if c and (dist[nk] == nil or d + c < dist[nk]) then
              dist[nk] = d + c
              prev[nk] = { k, c }
              push(nk, d + c + heuristic(nk))
            end
          end
        end
      end
    end
  end

  if restore then grid[goal] = restore end
  if dist[goal] == nil then return nil end

  local path, k = {}, goal
  while k ~= start do
    local p = prev[k]
    table.insert(path, 1, { x = k % W, y = k // W, cost = p[2] })
    k = p[1]
  end
  if #path > move.MAX_PATH then return nil end
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
    if city and city.ownerIndex ~= side then
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
  local water = scn.terrainAt(g.map, dest.x, dest.y) == move.WATER
  for _, a in ipairs(stack) do
    a.x, a.y = dest.x, dest.y
    a.moves = math.max(0, (a.moves or 0) - cost)
    a.atSea = water
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
  if lastOk < #path and result.stopped == "arrived" then result.stopped = "blocked" end
  return result
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
