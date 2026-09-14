-- Warlords II: the smallest possible start.
--
-- Draws a scenario map at 100% scale (40px tiles) from the ORIGINAL data files,
-- puts one army on the player's capital, and lets you make exactly one move.
-- After that it does nothing, on purpose.
--
-- Run from the repository root so that "original/" resolves:
--     love love2d
--     love love2d ISLADIA          -- pick a scenario
--     love love2d ERYTHEA ../mygame/original   -- point at your own copy

local pal      = require("warlords.pal")
local pck      = require("warlords.pck")
local scn      = require("warlords.scn")
local terrain  = require("warlords.terrain")

local TILE = scn.TILE
local ARMY_CELL, ARMY_COLS = 32, 16   -- A0.PCK is a 16x2 grid of 32x32 sprites
local ROAD_STRIDE, ROAD_COLS = 48, 13 -- ROAD.PCK: 40px art on a 48px stride
local ROAD_KEY, ARMY_KEY = 1, 10      -- colour-key indices

local G = {}   -- game state

local function quadsFor(img, cols, cellW, cellH, stride, count)
  local qs = {}
  for i = 0, count - 1 do
    qs[i] = love.graphics.newQuad(
      (i % cols) * (stride or cellW), math.floor(i / cols) * cellH,
      cellW, cellH, img:getDimensions())
  end
  return qs
end

local function exists(path)
  local f = io.open(path, "rb")
  if f then f:close() return true end
  return false
end

function love.load(arg)
  arg = arg or {}
  local scenario = arg[1] or "ERYTHEA"
  local dataDir  = arg[2] or "original"

  if not exists(dataDir .. "/TERRAIN0/WAR2.PAL") then
    error(("cannot find the game data at %q.\n"):format(dataDir)
      .. "Run from the repository root (love love2d), or pass the path:\n"
      .. "  love love2d ERYTHEA /path/to/warlords2")
  end
  if not exists(dataDir .. "/" .. scenario .. "/" .. scenario .. ".SCN") then
    error(("no scenario %q in %q"):format(scenario, dataDir))
  end

  local palette = pal.load(dataDir .. "/TERRAIN0/WAR2.PAL")

  -- terrain sheets: two 640x240 images, each a 16x6 grid of 40x40 tiles
  G.sheets, G.sheetPx = {}, {}
  for i = 0, 1 do
    local img, w, h, px = pck.toImage(dataDir .. "/TERRAIN0/SCENERY" .. i .. ".PCK", palette)
    G.sheets[i + 1] = img
    G.sheetPx[i + 1] = { w = w, h = h, px = px }
    G.tileQuads = G.tileQuads or {}
    G.tileQuads[i + 1] = quadsFor(img, 16, TILE, TILE, TILE, 96)
  end

  G.roadImg = pck.toImage(dataDir .. "/TERRAIN0/ROAD.PCK", palette, ROAD_KEY)
  G.roadQuads = quadsFor(G.roadImg, ROAD_COLS, TILE, TILE, ROAD_STRIDE, 26)

  G.armyImg = pck.toImage(dataDir .. "/TERRAIN0/A0.PCK", palette, ARMY_KEY)
  G.armyQuads = quadsFor(G.armyImg, ARMY_COLS, ARMY_CELL, ARMY_CELL, ARMY_CELL, 32)

  G.map = scn.load(dataDir .. "/" .. scenario, scenario)
  G.terrain = terrain.classify(G.sheetPx)

  -- the player is the first side that is actually in use
  for _, s in ipairs(G.map.sides) do
    if s.inUse then G.player = s break end
  end
  local cap = G.player.capital

  -- one army, standing on the capital, of the first type that city produces
  G.army = { x = cap.x, y = cap.y, type = cap.produces[1] or 1 }
  G.moved = false
  G.status = "Arrow keys or WASD: move one tile.   Esc: quit"

  love.graphics.setBackgroundColor(0, 0, 0)
  G.font = love.graphics.newFont(14)
  love.graphics.setFont(G.font)
end

-- top-left tile of the view, centred on the army and clamped to the map
local function camera()
  local sw, sh = love.graphics.getDimensions()
  local cols, rows = math.ceil(sw / TILE), math.ceil((sh - 40) / TILE)
  local cx = G.army.x - math.floor(cols / 2)
  local cy = G.army.y - math.floor(rows / 2)
  cx = math.max(0, math.min(cx, G.map.width - cols))
  cy = math.max(0, math.min(cy, G.map.height - rows))
  return cx, cy, cols, rows
end

function love.draw()
  local cx, cy, cols, rows = camera()

  for ty = 0, rows do
    local my = cy + ty
    if my >= 0 and my < G.map.height then
      for tx = 0, cols do
        local mx = cx + tx
        if mx >= 0 and mx < G.map.width then
          local t = scn.tileAt(G.map, mx, my)
          local sheet = math.floor(t / 96) + 1
          love.graphics.draw(G.sheets[sheet], G.tileQuads[sheet][t % 96],
                             tx * TILE, ty * TILE)
          local r = scn.roadAt(G.map, mx, my)
          if r ~= 0 then
            -- road piece id N draws ROAD.PCK cell N-1
            love.graphics.draw(G.roadImg, G.roadQuads[r - 1], tx * TILE, ty * TILE)
          end
        end
      end
    end
  end

  -- the army, centred in its tile
  local ax = (G.army.x - cx) * TILE + (TILE - ARMY_CELL) / 2
  local ay = (G.army.y - cy) * TILE + (TILE - ARMY_CELL) / 2
  love.graphics.draw(G.armyImg, G.armyQuads[G.army.type], ax, ay)
  love.graphics.setColor(1, 1, 1)
  love.graphics.rectangle("line", (G.army.x - cx) * TILE, (G.army.y - cy) * TILE, TILE, TILE)

  -- status bar
  local sw, sh = love.graphics.getDimensions()
  love.graphics.setColor(0, 0, 0, 0.75)
  love.graphics.rectangle("fill", 0, sh - 40, sw, 40)
  love.graphics.setColor(1, 1, 1)
  local cap = G.player.capital
  love.graphics.print(string.format(
    "%s   %s (%d gold)   capital %s, income %d, defence %d   army at (%d,%d) on %s   |  %s",
    G.map.name, G.player.name, G.player.gold, cap.name, cap.income, cap.defence,
    G.army.x, G.army.y, G.terrain[scn.tileAt(G.map, G.army.x, G.army.y)], G.status),
    8, sh - 27)
end

local DIRS = {
  up = { 0, -1 }, w = { 0, -1 },
  down = { 0, 1 }, s = { 0, 1 },
  left = { -1, 0 }, a = { -1, 0 },
  right = { 1, 0 }, d = { 1, 0 },
}

function love.keypressed(key)
  if key == "escape" then love.event.quit() return end
  if G.moved then return end          -- one move, then nothing. On purpose.

  local dir = DIRS[key]
  if not dir then return end

  local nx, ny = G.army.x + dir[1], G.army.y + dir[2]
  if nx < 0 or ny < 0 or nx >= G.map.width or ny >= G.map.height then
    G.status = "Off the edge of the map."
    return
  end

  local class = G.terrain[scn.tileAt(G.map, nx, ny)]
  if not terrain.passable(class) then
    G.status = "A land army cannot enter " .. class .. "."
    return
  end
  if G.map.cityAt[ny * G.map.width + nx] then
    G.status = "Cities must be attacked, not moved into."
    return
  end

  G.army.x, G.army.y = nx, ny
  G.moved = true
  G.status = "Move complete. That is all this build does."
end
