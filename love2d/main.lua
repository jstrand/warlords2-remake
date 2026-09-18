-- Warlords II, playable slice.
--
-- Reads the ORIGINAL data files at runtime and plays a real game on top of the
-- rules core in warlords/: move stacks, take cities, set production, end the
-- turn and let the computer players answer.
--
-- Run from the repository root so that "original/" resolves:
--     love love2d
--     love love2d ISLADIA                      -- pick a scenario
--     love love2d ERYTHEA ../mygame/original    -- point at your own copy

local pal    = require("warlords.pal")
local pck    = require("warlords.pck")
local scn    = require("warlords.scn")
local game   = require("warlords.game")
local move   = require("warlords.move")
local ai     = require("warlords.ai")
local rules  = require("warlords.rules")
local hero   = require("warlords.hero")
local armytype = require("warlords.armytype")

local TILE = scn.TILE
local ARMY_CELL, ARMY_COLS = 32, 16   -- A<n>.PCK is a 16x2 grid of 32x32 sprites
local ROAD_STRIDE, ROAD_COLS = 48, 13 -- ROAD.PCK: 40px art on a 48px stride
local ROAD_KEY, ARMY_KEY = 1, 10      -- colour-key indices
local BAR = 76                        -- status bar height

local G = {}          -- view state; the game itself lives in G.g

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
  G.sheets, G.tileQuads = {}, {}
  for i = 0, 1 do
    local img = pck.toImage(dataDir .. "/TERRAIN0/SCENERY" .. i .. ".PCK", palette)
    G.sheets[i + 1] = img
    G.tileQuads[i + 1] = quadsFor(img, 16, TILE, TILE, TILE, 96)
  end

  G.roadImg = pck.toImage(dataDir .. "/TERRAIN0/ROAD.PCK", palette, ROAD_KEY)
  G.roadQuads = quadsFor(G.roadImg, ROAD_COLS, TILE, TILE, ROAD_STRIDE, 26)

  -- one army sheet per side colour, A8 for neutral
  G.armyImg, G.armyQuads = {}, {}
  for i = 0, 8 do
    local img = pck.toImage(dataDir .. "/TERRAIN0/A" .. i .. ".PCK", palette, ARMY_KEY)
    G.armyImg[i] = img
    G.armyQuads[i] = quadsFor(img, ARMY_COLS, ARMY_CELL, ARMY_CELL, ARMY_CELL, 32)
  end

  G.g = game.new(dataDir, scenario, { seed = os.time() })
  G.player = game.begin(G.g)
  G.selection = nil
  G.cx, G.cy = G.player.capital.x - 6, G.player.capital.y - 5
  say("Click a stack to select it, then click where to go. Space ends the turn.")

  love.graphics.setBackgroundColor(0, 0, 0)
  G.font = love.graphics.newFont(13)
  G.bold = love.graphics.newFont(15)
  love.graphics.setFont(G.font)
end

------------------------------------------------------------------ the camera

local function viewSize()
  local sw, sh = love.graphics.getDimensions()
  return math.ceil(sw / TILE), math.ceil((sh - BAR) / TILE)
end

local function clampCamera()
  local cols, rows = viewSize()
  G.cx = math.max(0, math.min(G.cx, G.g.map.width - cols))
  G.cy = math.max(0, math.min(G.cy, G.g.map.height - rows))
end

local function centreOn(x, y)
  local cols, rows = viewSize()
  G.cx, G.cy = x - math.floor(cols / 2), y - math.floor(rows / 2)
  clampCamera()
end

------------------------------------------------------------------- selection

--- The player's armies on a tile that still have movement, up to a full stack.
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
      say("%s: %s, income %d, defence %d", city.name,
          city.ownerIndex and G.g.map.sides[city.ownerIndex + 1].name or "neutral",
          city.income, city.defence)
    end
    return
  end
  G.selection = { x = x, y = y, stack = stack }
  local names = {}
  for _, a in ipairs(stack) do names[#names + 1] = a.name end
  say("%d selected (%s), %d movement", #stack,
      table.concat(names, ", "), move.stackMoves(stack))
end

------------------------------------------------------------------ the player

local function afterBattle(result, x, y)
  if result.captured then
    say("%s is ours!%s", result.captured.name,
        result.loot and result.loot > 0 and (" Looted %d gold."):format(result.loot) or "")
    G.captured = result.captured
  elseif result.won then
    say("The tile is cleared: %d lost, %d killed.",
        #result.deadAttackers, #result.deadDefenders)
  else
    say("Beaten back: %d lost, %d killed.", #result.deadAttackers, #result.deadDefenders)
  end
  -- a selection can be wiped out entirely
  if G.selection then
    local alive = {}
    for _, a in ipairs(G.selection.stack) do
      if not result.deadByArmy[a] then alive[#alive + 1] = a end
    end
    G.selection = #alive > 0
      and { x = alive[1].x, y = alive[1].y, stack = alive } or nil
  end
end

local function moveSelection(x, y)
  local sel = G.selection
  if not sel then return end
  local r = move.moveTo(G.g, sel.stack, x, y)
  if r.stopped == "attack" then
    local result = game.resolveAttack(G.g, sel.stack, r.attack.x, r.attack.y)
    afterBattle(result, r.attack.x, r.attack.y)
  elseif r.stopped == "no route" then
    say("There is no way there.")
  elseif r.steps == 0 then
    say("They cannot move: %s.", r.stopped)
  else
    sel.x, sel.y = sel.stack[1].x, sel.stack[1].y
    say("Moved %d tiles for %d. %d movement left.", r.steps, r.spent,
        move.stackMoves(sel.stack))
    -- standing on a ruin or temple searches it
    local found = game.searchHere(G.g, sel.stack)
    if found then
      say("%s", game.describeSearch(found))
      if found.kind == "killed" then
        local alive = {}
        for _, a in ipairs(sel.stack) do
          if a.type ~= armytype.HERO then alive[#alive + 1] = a end
        end
        G.selection = #alive > 0 and { x = alive[1].x, y = alive[1].y, stack = alive } or nil
      end
    end
  end
  if G.selection and #G.selection.stack > 0 then
    G.selection.x, G.selection.y = G.selection.stack[1].x, G.selection.stack[1].y
    centreOn(G.selection.x, G.selection.y)
  end
end

local function endTurn()
  G.selection, G.captured = nil, nil
  local side = game.endTurn(G.g)
  -- let every computer player answer before handing control back
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
  if side.heroOffer then
    G.offer = side.heroOffer
    say("A hero offers to serve for %d gold. Press Y to hire, N to refuse.",
        G.offer.price)
  else
    say("Turn %d. %d gold, income %d, upkeep %d.", G.g.turn, side.gold,
        side.income or 0, side.upkeepTotal or 0)
  end
end

--------------------------------------------------------------------- drawing

local function drawTile(mx, my, sx, sy)
  local t = scn.tileAt(G.g.map, mx, my)
  local sheet = math.floor(t / 96) + 1
  love.graphics.draw(G.sheets[sheet], G.tileQuads[sheet][t % 96], sx, sy)
  local r = scn.roadAt(G.g.map, mx, my)
  if r ~= 0 then
    love.graphics.draw(G.roadImg, G.roadQuads[r - 1], sx, sy)
  end
end

--- The army shown for a tile: the one highest in its owner's fight order.
local function topArmy(stack)
  local best, bestRank
  for _, a in ipairs(stack) do
    local rank = G.g.map.fightOrder[a.owner or 8][a.type] or 0
    if not bestRank or rank > bestRank then best, bestRank = a, rank end
  end
  return best
end

function love.draw()
  local cols, rows = viewSize()
  local sw, sh = love.graphics.getDimensions()

  for ty = 0, rows do
    local my = G.cy + ty
    if my >= 0 and my < G.g.map.height then
      for tx = 0, cols do
        local mx = G.cx + tx
        if mx >= 0 and mx < G.g.map.width then
          drawTile(mx, my, tx * TILE, ty * TILE)
        end
      end
    end
  end

  -- armies, one sprite per occupied tile
  local drawn = {}
  for _, a in ipairs(G.g.armies) do
    if not a.transit then
      local k = a.y * G.g.map.width + a.x
      if not drawn[k] then
        drawn[k] = true
        local tx, ty = a.x - G.cx, a.y - G.cy
        if tx >= 0 and ty >= 0 and tx <= cols and ty <= rows then
          local stack = game.armiesAt(G.g, a.x, a.y)
          local top = topArmy(stack)
          local sheet = top.owner or 8
          love.graphics.draw(G.armyImg[sheet], G.armyQuads[sheet][top.type],
                             tx * TILE + (TILE - ARMY_CELL) / 2,
                             ty * TILE + (TILE - ARMY_CELL) / 2)
          if #stack > 1 then
            love.graphics.setColor(0, 0, 0, 0.6)
            love.graphics.rectangle("fill", tx * TILE + TILE - 14, ty * TILE + TILE - 14, 14, 14)
            love.graphics.setColor(1, 1, 1)
            love.graphics.print(#stack, tx * TILE + TILE - 11, ty * TILE + TILE - 15)
          end
        end
      end
    end
  end

  -- the selection
  if G.selection then
    love.graphics.setColor(1, 1, 0.3)
    love.graphics.rectangle("line", (G.selection.x - G.cx) * TILE,
                            (G.selection.y - G.cy) * TILE, TILE, TILE)
    love.graphics.setColor(1, 1, 1)
  end

  -- status bar
  love.graphics.setColor(0, 0, 0, 0.82)
  love.graphics.rectangle("fill", 0, sh - BAR, sw, BAR)
  love.graphics.setColor(1, 1, 1)
  love.graphics.setFont(G.bold)
  love.graphics.print(("%s — turn %d — %s")
    :format(G.g.map.name, G.g.turn, G.player.name), 10, sh - BAR + 8)
  love.graphics.setFont(G.font)
  love.graphics.print(("%d gold   income %d   upkeep %d   %d cities   %d armies")
    :format(G.player.gold, G.player.income or 0, G.player.upkeepTotal or 0,
            #game.sideCities(G.g, G.player), #game.sideArmies(G.g, G.player)),
    10, sh - BAR + 30)
  love.graphics.print(G.status or "", 10, sh - BAR + 50)
  if G.g.side and G.g.side.quest then
    love.graphics.setColor(0.85, 0.8, 0.4)
    love.graphics.print("quest: " .. require("warlords.quest").describe(G.g.side.quest),
                        360, sh - BAR + 30)
    love.graphics.setColor(1, 1, 1)
  end

  love.graphics.setColor(0.7, 0.7, 0.7)
  love.graphics.printf("space: end turn   p: production   c: centre   esc: quit",
                       sw - 430, sh - BAR + 50, 420, "right")
  love.graphics.setColor(1, 1, 1)
end

--------------------------------------------------------------------- input

function love.mousepressed(px, py, button)
  local sh = love.graphics.getHeight()
  if py > sh - BAR then return end
  local x, y = G.cx + math.floor(px / TILE), G.cy + math.floor(py / TILE)
  if x < 0 or y < 0 or x >= G.g.map.width or y >= G.g.map.height then return end

  if button == 1 then
    if G.selection and (x ~= G.selection.x or y ~= G.selection.y) then
      moveSelection(x, y)
    else
      select(x, y)
    end
  elseif button == 2 then
    select(x, y)
  end
end

--- Cycle what the selected tile's city builds.
local function cycleProduction()
  local sel = G.selection
  local x, y = sel and sel.x or nil, sel and sel.y or nil
  local city = x and game.cityAt(G.g, x, y) or G.captured
  if not city or city.ownerIndex ~= G.player.index then
    say("Select one of your cities first.")
    return
  end
  if #city.slots == 0 then
    say("%s can build nothing.", city.name)
    return
  end
  local next = ((city.producing or 0) % #city.slots) + 1
  game.setProduction(G.g, city, next)
  local slot = city.slots[next]
  say("%s builds %s: strength %d, move %d, %d turns, upkeep %d.",
      city.name, slot.name, slot.strength, slot.move, slot.time, slot.cost // 2)
end

function love.keypressed(key)
  if key == "escape" then love.event.quit() return end
  if G.over then return end

  if G.offer then
    if key == "y" then
      local h = hero.recruit(G.g, G.player, G.offer)
      say("%s joins at %s.", h.title or "A hero",
          G.g.map.cities[h.homeCity + 1].name)
      centreOn(h.x, h.y)
      G.offer = nil
    elseif key == "n" then
      G.offer = nil
      say("The hero rides away.")
    end
    return
  end

  if key == "space" then endTurn()
  elseif key == "p" then cycleProduction()
  elseif key == "c" then
    if G.selection then centreOn(G.selection.x, G.selection.y)
    else centreOn(G.player.capital.x, G.player.capital.y) end
  elseif key == "up" or key == "w" then G.cy = G.cy - 3; clampCamera()
  elseif key == "down" or key == "s" then G.cy = G.cy + 3; clampCamera()
  elseif key == "left" or key == "a" then G.cx = G.cx - 3; clampCamera()
  elseif key == "right" or key == "d" then G.cx = G.cx + 3; clampCamera()
  end
end
