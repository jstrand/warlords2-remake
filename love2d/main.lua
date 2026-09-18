-- Warlords II, in the original's own interface.
--
-- The screen is 640x480 and every rect in it comes from the game's own layout
-- files rather than from us: DATA/JOIN.DAT names the dialog's controls and
-- clickable regions, DATA/AREA.DAT places the regions, DATA/BUTTON.DAT the
-- controls, and DATA/FILE.DAT says which .PCK each control is cut from.
-- docs/re/ui.md and docs/formats/screens.md.
--
-- Run from the repository root so that "original/" resolves:
--     love love2d
--     love love2d ISLADIA                       -- pick a scenario
--     love love2d ERYTHEA ../mygame/original    -- point at your own copy

local pal      = require("warlords.pal")
local pck      = require("warlords.pck")
local scn      = require("warlords.scn")
local game     = require("warlords.game")
local move     = require("warlords.move")
local ai       = require("warlords.ai")
local rules    = require("warlords.rules")
local hero     = require("warlords.hero")
local armytype = require("warlords.armytype")
local saveMod  = require("warlords.save")
local screen   = require("warlords.screen")
local uidata   = require("warlords.uidata")
local font     = require("warlords.font")

local W, H = screen.WIDTH, screen.HEIGHT
local TILE = screen.TILE
local ARMY_CELL, ARMY_COLS = 32, 16
local ROAD_STRIDE, ROAD_COLS = 48, 13
local ROAD_KEY, ARMY_KEY = 1, 10

local G = {}

local presentOffer

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

local function say(fmt, ...)
  G.status = select("#", ...) > 0 and fmt:format(...) or fmt
end

--------------------------------------------------------------------- loading

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
  G.palette = palette
  G.dataDir = dataDir
  G.savePath = "warlords-save.lua"

  -- terrain, roads and armies
  G.sheets, G.tileQuads = {}, {}
  for i = 0, 1 do
    local img = pck.toImage(dataDir .. "/TERRAIN0/SCENERY" .. i .. ".PCK", palette)
    G.sheets[i + 1] = img
    G.tileQuads[i + 1] = quadsFor(img, 16, TILE, TILE, TILE, 96)
  end
  G.roadImg = pck.toImage(dataDir .. "/TERRAIN0/ROAD.PCK", palette, ROAD_KEY)
  G.roadQuads = quadsFor(G.roadImg, ROAD_COLS, TILE, TILE, ROAD_STRIDE, 26)
  G.armyImg, G.armyQuads = {}, {}
  for i = 0, 8 do
    local img = pck.toImage(dataDir .. "/TERRAIN0/A" .. i .. ".PCK", palette, ARMY_KEY)
    G.armyImg[i] = img
    G.armyQuads[i] = quadsFor(img, ARMY_COLS, ARMY_CELL, ARMY_CELL, ARMY_CELL, 32)
  end

  -- the original's own chrome and fonts
  G.screen = screen.load(dataDir, palette, uidata.MAIN_SCREEN, 0)
  G.font = font.load(dataDir, "TEXT", palette, 3)
  G.bigFont = font.load(dataDir, "CHANCE17", palette, 3)

  G.mapRect  = screen.region(G.screen, screen.REGION.MAP)
  G.stratRect = screen.region(G.screen, screen.REGION.STRATEGIC)
  G.barRect  = screen.region(G.screen, screen.REGION.BOTTOMBAR)
  G.menuRect = screen.region(G.screen, screen.REGION.MENUBAR)

  G.g = game.new(dataDir, scenario, { seed = os.time() })
  G.player = game.begin(G.g)
  G.selection = nil
  -- centre the 9x9 viewport on the capital
  G.cx = math.max(0, math.min(G.player.capital.x - 4, G.g.map.width - screen.VIEW_COLS))
  G.cy = math.max(0, math.min(G.player.capital.y - 4, G.g.map.height - screen.VIEW_ROWS))

  love.graphics.setBackgroundColor(0, 0, 0)

  if not presentOffer(G.player) then
    say("Click a stack, then click where to go.")
  end
end

--------------------------------------------------------------------- camera

local function clampCamera()
  G.cx = math.max(0, math.min(G.cx, G.g.map.width - screen.VIEW_COLS))
  G.cy = math.max(0, math.min(G.cy, G.g.map.height - screen.VIEW_ROWS))
end

local function centreOn(x, y)
  G.cx = x - math.floor(screen.VIEW_COLS / 2)
  G.cy = y - math.floor(screen.VIEW_ROWS / 2)
  clampCamera()
end

--- Screen point to map tile, or nil when it is outside the viewport.
local function tileAtPoint(sx, sy)
  local r = G.mapRect
  if sx < r.x or sy < r.y or sx >= r.x + r.w or sy >= r.y + r.h then return nil end
  return G.cx + math.floor((sx - r.x) / TILE), G.cy + math.floor((sy - r.y) / TILE)
end

--------------------------------------------------------------------- selection

local function selectableAt(x, y)
  local out = {}
  for _, a in ipairs(game.armiesAt(G.g, x, y)) do
    if a.owner == G.player.index and #out < rules.MAX_STACK then out[#out + 1] = a end
  end
  return out
end

local function select(x, y)
  local stack = selectableAt(x, y)
  if #stack == 0 then
    G.selection = nil
    local city = game.cityAt(G.g, x, y)
    if city then
      say("%s: %s, income %d", city.name,
          city.ownerIndex and G.g.map.sides[city.ownerIndex + 1].name or "neutral",
          city.income)
    end
    return
  end
  G.selection = { x = x, y = y, stack = stack }
  say("%d selected, %d movement", #stack, move.stackMoves(stack))
end

--------------------------------------------------------------------- the turn

local function afterBattle(result)
  if result.captured then
    say("%s is ours!%s", result.captured.name,
        result.loot and result.loot > 0 and (" Looted %d gold."):format(result.loot) or "")
  elseif result.won then
    say("Cleared: %d lost, %d killed.", #result.deadAttackers, #result.deadDefenders)
  else
    say("Beaten back: %d lost, %d killed.", #result.deadAttackers, #result.deadDefenders)
  end
  if G.selection then
    local alive = {}
    for _, a in ipairs(G.selection.stack) do
      if not result.deadByArmy[a] then alive[#alive + 1] = a end
    end
    G.selection = #alive > 0 and { x = alive[1].x, y = alive[1].y, stack = alive } or nil
  end
end

local function moveSelection(x, y)
  local sel = G.selection
  if not sel then return end
  local r = move.moveTo(G.g, sel.stack, x, y)
  if r.stopped == "attack" then
    afterBattle(game.resolveAttack(G.g, sel.stack, r.attack.x, r.attack.y))
  elseif r.stopped == "no route" then
    say("There is no way there.")
  elseif r.steps == 0 then
    say("They cannot move: %s.", r.stopped)
  else
    sel.x, sel.y = sel.stack[1].x, sel.stack[1].y
    say("Moved %d for %d. %d left.", r.steps, r.spent, move.stackMoves(sel.stack))
    local found = game.searchHere(G.g, sel.stack)
    if found then say("%s", game.describeSearch(found)) end
  end
  if G.selection and #G.selection.stack > 0 then
    G.selection.x, G.selection.y = G.selection.stack[1].x, G.selection.stack[1].y
    centreOn(G.selection.x, G.selection.y)
  end
end

function presentOffer(side)
  if not side.heroOffer then return false end
  G.offer = side.heroOffer
  if G.offer.first then
    say("A hero comes to %s and asks no pay. Y accept, N refuse.", G.offer.city.name)
  else
    say("A hero offers to serve for %d gold. Y hire, N refuse.", G.offer.price)
  end
  return true
end

local function endTurn()
  G.selection = nil
  local side = game.endTurn(G.g)
  while side and side.index ~= G.player.index do
    ai.playTurn(G.g, side)
    side = game.endTurn(G.g)
  end
  if not side then
    G.over = true
    say("%s", G.g.log[#G.g.log] or "The game is over.")
    return
  end
  G.player = side
  if not presentOffer(side) then
    say("Turn %d. %d gold, income %d.", G.g.turn, side.gold, side.income or 0)
  end
end

--------------------------------------------------------------------- drawing

local function topArmy(stack)
  local best, bestRank
  for _, a in ipairs(stack) do
    local rank = G.g.map.fightOrder[a.owner or 8][a.type] or 0
    if not bestRank or rank > bestRank then best, bestRank = a, rank end
  end
  return best
end

local function drawMap()
  local r = G.mapRect
  love.graphics.setScissor(r.x, r.y, r.w, r.h)
  for row = 0, screen.VIEW_ROWS - 1 do
    for col = 0, screen.VIEW_COLS - 1 do
      local mx, my = G.cx + col, G.cy + row
      local sx, sy = r.x + col * TILE, r.y + row * TILE
      if mx < G.g.map.width and my < G.g.map.height then
        if game.seen(G.g, G.player, mx, my) then
          local t = scn.tileAt(G.g.map, mx, my)
          local sheet = math.floor(t / 96) + 1
          love.graphics.setColor(1, 1, 1)
          love.graphics.draw(G.sheets[sheet], G.tileQuads[sheet][t % 96], sx, sy)
          local rd = scn.roadAt(G.g.map, mx, my)
          if rd ~= 0 then love.graphics.draw(G.roadImg, G.roadQuads[rd - 1], sx, sy) end

          local stack = game.armiesAt(G.g, mx, my)
          if #stack > 0 then
            local a = topArmy(stack)
            local owner = a.owner or 8
            love.graphics.draw(G.armyImg[owner], G.armyQuads[owner][a.type % 32],
                               sx + 4, sy + 4)
          end
        else
          love.graphics.setColor(0, 0, 0)
          love.graphics.rectangle("fill", sx, sy, TILE, TILE)
        end
      end
    end
  end
  -- the selection box
  if G.selection then
    local col, row = G.selection.x - G.cx, G.selection.y - G.cy
    if col >= 0 and col < screen.VIEW_COLS and row >= 0 and row < screen.VIEW_ROWS then
      love.graphics.setColor(1, 1, 1)
      love.graphics.rectangle("line", r.x + col * TILE + 0.5, r.y + row * TILE + 0.5,
                              TILE - 1, TILE - 1)
    end
  end
  love.graphics.setScissor()
end

--- The strategic map: two pixels per tile, with the viewport box on it.
local function drawStrategic()
  local r = G.stratRect
  love.graphics.setScissor(r.x, r.y, r.w, r.h)
  local terrain = G.g.map.terrain
  for my = 0, G.g.map.height - 1 do
    for mx = 0, G.g.map.width - 1 do
      if game.seen(G.g, G.player, mx, my) then
        local c = G.palette[(terrain and terrain[scn.terrainAt(G.g.map, mx, my)] or 10) + 1]
                  or G.palette[11]
        love.graphics.setColor(c[1], c[2], c[3])
      else
        love.graphics.setColor(0, 0, 0)
      end
      love.graphics.rectangle("fill", r.x + mx * 2, r.y + my * 2, 2, 2)
    end
  end
  for _, city in ipairs(G.g.map.cities) do
    if not city.razed and game.seen(G.g, G.player, city.x, city.y) then
      love.graphics.setColor(1, 1, 1)
      love.graphics.rectangle("fill", r.x + city.x * 2, r.y + city.y * 2, 4, 4)
    end
  end
  love.graphics.setColor(1, 1, 1)
  love.graphics.rectangle("line",
    r.x + G.cx * 2 + 0.5, r.y + G.cy * 2 + 0.5,
    screen.VIEW_COLS * 2, screen.VIEW_ROWS * 2)
  love.graphics.setScissor()
end

local MENUS = { "SSG", "Game", "Order", "Report", "Hero", "View", "History", "Turn" }

local function drawMenuBar()
  love.graphics.setColor(1, 1, 1)
  local x = 8
  for _, name in ipairs(MENUS) do
    local w = math.ceil(G.font.width(name) / 8) * 8
    G.font.draw(name, x, 2)
    x = x + w + 16
  end
end

local function drawBottomBar()
  local r = G.barRect
  love.graphics.setColor(1, 1, 1)
  if G.selection then
    local x = r.x + 8
    for _, a in ipairs(G.selection.stack) do
      local owner = a.owner or 8
      love.graphics.draw(G.armyImg[owner], G.armyQuads[owner][a.type % 32], x, r.y + 2)
      G.font.draw(tostring(a.moves or 0), x + 4, r.y + 38)
      x = x + 40
    end
  else
    local cities = #game.sideCities(G.g, G.player)
    G.font.draw(("Cities %d    %d gp    income %d    upkeep %d"):format(
      cities, G.player.gold, G.player.income or 0, G.player.upkeepTotal or 0),
      r.x + 8, r.y + 6)
  end
  G.font.draw(G.status or "", r.x + 8, r.y + 34)
end

--- Scale the 640x480 screen up by a whole number and centre it.
local function viewTransform()
  local sw, sh = love.graphics.getDimensions()
  local s = math.max(1, math.floor(math.min(sw / W, sh / H)))
  return s, math.floor((sw - W * s) / 2), math.floor((sh - H * s) / 2)
end

function love.draw()
  local s, ox, oy = viewTransform()
  love.graphics.push()
  love.graphics.translate(ox, oy)
  love.graphics.scale(s, s)

  screen.drawBackground(G.screen)
  drawMap()
  drawStrategic()
  screen.drawControls(G.screen)
  drawMenuBar()
  drawBottomBar()

  love.graphics.pop()
end

--------------------------------------------------------------------- input

--- Turn a window point into a point on the 640x480 screen.
local function toScreen(x, y)
  local s, ox, oy = viewTransform()
  return math.floor((x - ox) / s), math.floor((y - oy) / s)
end

function love.mousepressed(mx, my, button)
  if G.over or G.offer then return end
  local x, y = toScreen(mx, my)
  local r = screen.regionAt(G.screen, x, y)
  if not r then return end

  if r.id == screen.REGION.MAP then
    local tx, ty = tileAtPoint(x, y)
    if not tx then return end
    if button == 2 then
      select(tx, ty)
    elseif G.selection then
      moveSelection(tx, ty)
    else
      select(tx, ty)
    end

  elseif r.id == screen.REGION.STRATEGIC then
    centreOn(math.floor((x - r.x) / 2), math.floor((y - r.y) / 2))
  end
end

function love.keypressed(key)
  if key == "escape" then love.event.quit() return end
  if G.over then return end

  if G.offer then
    if key == "y" then
      local h = hero.recruit(G.g, G.player, G.offer)
      say("A hero joins at %s.", G.g.map.cities[h.homeCity + 1].name)
      centreOn(h.x, h.y)
      G.offer, G.player.heroOffer = nil, nil
    elseif key == "n" then
      G.offer, G.player.heroOffer = nil, nil
      say("The hero rides away.")
    end
    return
  end

  if key == "space" then endTurn()
  elseif key == "c" and G.selection then centreOn(G.selection.x, G.selection.y)
  elseif key == "f5" then
    saveMod.write(G.g, G.savePath)
    say("Saved to %s.", G.savePath)
  elseif key == "f9" then
    local ok, loaded = pcall(saveMod.read, G.savePath, G.dataDir)
    if ok then
      G.g, G.selection = loaded, nil
      G.player = loaded.sides[loaded.current]
      centreOn(G.player.capital.x, G.player.capital.y)
      say("Loaded: turn %d, %s to play.", loaded.turn, G.player.name)
    else
      say("Nothing to load.")
    end
  elseif key == "up" or key == "w" then G.cy = G.cy - 1; clampCamera()
  elseif key == "down" or key == "s" then G.cy = G.cy + 1; clampCamera()
  elseif key == "left" or key == "a" then G.cx = G.cx - 1; clampCamera()
  elseif key == "right" or key == "d" then G.cx = G.cx + 1; clampCamera()
  end
end

-- LOVE ignores what main.lua returns; test/ui.lua uses it to inspect state.
return G
