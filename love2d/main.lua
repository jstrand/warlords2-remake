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
local menuMod  = require("warlords.menu")

local TILE = screen.TILE
local ARMY_CELL, ARMY_COLS = 32, 16
local ROAD_STRIDE, ROAD_COLS = 48, 13
local ROAD_KEY, ARMY_KEY = 1, 10
local RING_W, RING_H = 32, 30      -- one ABITS ring

local G = {}

local presentOffer, stratDirty, openCity, closeCity  -- defined below

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

  -- MARBLE.PCK is 480 wide, exactly the city dialog's width
  G.marble = pck.toImage(dataDir .. "/PICS/MARBLE.PCK", palette)
  -- BIGARMY.PCK is one 128x128 symbol for "this city is producing", not a
  -- picture of the army; ABITS.PCK holds nine 40x40 rings, grey then one per
  -- side, used to ring the chosen production in the owner's colour.
  G.bigArmy = pck.toImage(dataDir .. "/PICS/BIGARMY.PCK", palette)
  -- The rings are 32 x 30 on a 32-pixel stride, nine of them from x = 0; the
  -- rest of the 480 x 40 sheet is other bits, and a 40 x 40 cell drags them in.
  G.abits = pck.toImage(dataDir .. "/PICS/ABITS.PCK", palette, 3)
  G.ringQuads = {}
  for i = 0, 8 do
    G.ringQuads[i] = love.graphics.newQuad(i * RING_W, 0, RING_W, RING_H, 480, 40)
  end

  -- the original's own chrome and fonts
  G.screen = screen.load(dataDir, palette, uidata.MAIN_SCREEN, 0)
  G.font = font.load(dataDir, "TEXT", palette, 3)
  G.bigFont = font.load(dataDir, "CHANCE17", palette, 3)
  G.titleFont = font.load(dataDir, "CHANCE36", palette, 3)

  G.menuLayout = menuMod.layout(G.font, 18, screen.WIDTH)
  G.openMenu = nil

  G.mapRect  = screen.region(G.screen, screen.REGION.MAP)
  G.stratRect = screen.region(G.screen, screen.REGION.STRATEGIC)
  G.barRect  = screen.region(G.screen, screen.REGION.BOTTOMBAR)
  G.menuRect = screen.region(G.screen, screen.REGION.MENUBAR)

  -- A third argument fixes the seed, so a run can be reproduced exactly.
  -- test/ui.lua passes one; without it every game is different.
  local seed = tonumber(arg[3]) or os.time()
  G.seed = seed
  G.g = game.new(dataDir, scenario, { seed = seed })
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
    if city then openCity(city) end
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
    stratDirty()
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
  stratDirty()
  if not presentOffer(side) then
    say("Turn %d. %d gold, income %d.", G.g.turn, side.gold, side.income or 0)
  end
end

-- The city dialog is dialog 6 (7204:0000 pushes 6 to the dialog opener). Its
-- four 32x70 slots, ids 197-200, are the city's production choices; they carry
-- no art of their own, so the game draws the army in each.
-- docs/formats/screens.md.
local CITY_DIALOG, CITY_SLOT_FIRST, CITY_SLOTS = 6, 197, 4
-- 192 and 201 are the two variants of Done, 202 is Stop -- the octagonal
-- button beside the production row. All three are cut from CITYBU.PCK.
local CITY_DONE, CITY_DONE_ALT, CITY_STOP = 192, 201, 202

function openCity(city)
  if not G.cityView then G.cityView = screen.dialog(G.screen, CITY_DIALOG) end
  G.city = city
  say("%s: %s", city.name,
      city.ownerIndex == G.player.index and "choose what to build" or "not yours")
end

function closeCity()
  G.city = nil
end

--- Set the city building slot n (1-based), if it is ours.
local function pickProduction(n)
  local c = G.city
  if not c then return end
  if c.ownerIndex ~= G.player.index then say("%s is not yours.", c.name) return end
  local slot = c.slots and c.slots[n]
  if not slot then say("There is no such slot.") return end
  game.setProduction(G.g, c, n)
  say("%s will build %s: strength %d, %d turns, upkeep %d.",
      c.name, slot.name, slot.strength, slot.time, math.floor(slot.cost / 2))
end

local function loadGame()
  local ok, loaded = pcall(saveMod.read, G.savePath, G.dataDir)
  if not ok then say("Nothing to load.") return end
  G.g, G.selection = loaded, nil
  stratDirty()
  G.player = loaded.sides[loaded.current]
  centreOn(G.player.capital.x, G.player.capital.y)
  say("Loaded: turn %d, %s to play.", loaded.turn, G.player.name)
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

--- The strategic map: MAPCOLOR.DAT through the palette, one pixel per tile,
--- drawn at double size so a 112x156 map fills the 224x312 region exactly.
local function drawStrategic()
  local r = G.stratRect
  if not G.stratImage then
    G.stratImage = screen.strategicImage(G.screen, G.g, G.player, game.seen)
  end
  love.graphics.setScissor(r.x, r.y, r.w, r.h)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.stratImage, r.x, r.y, 0, 2, 2)

  for _, city in ipairs(G.g.map.cities) do
    if not city.razed and game.seen(G.g, G.player, city.x, city.y) then
      love.graphics.setColor(1, 1, 1)
      love.graphics.rectangle("fill", r.x + city.x * 2, r.y + city.y * 2, 4, 4)
    end
  end
  -- the viewport box: 9x9 tiles at 2 pixels each, as the original draws it
  love.graphics.setColor(1, 1, 1)
  love.graphics.rectangle("line",
    r.x + G.cx * 2 + 0.5, r.y + G.cy * 2 + 0.5,
    screen.VIEW_COLS * 2, screen.VIEW_ROWS * 2)
  love.graphics.setScissor()
end

--- The fog only ever opens, so the strategic map is rebuilt on demand.
function stratDirty()
  G.stratImage = nil
end

-- The four configurable buttons carry no icon in BUTTON.PCK; the original
-- draws whatever the assigned command uses. Until that art is found, name
-- them from UDB.DAT so they at least say what they do.
local function drawShortcutLabels()
  local ui = G.screen.ui
  for i = 0, uidata.SHORTCUT_COUNT - 1 do
    local c = screen.control(G.screen, uidata.SHORTCUT_FIRST + i)
    local id = ui.shortcuts[i]
    local name = id and ui.shortcutNames[id]
    if c and name then
      love.graphics.setColor(1, 1, 1)
      local short = name:sub(1, 6)
      G.font.draw(short,
        c.x + math.max(1, math.floor((c.w - G.font.width(short)) / 2)),
        c.y + math.floor((c.h - G.font.lineHeight) / 2))
    end
  end
end

local function drawMenuBar()
  love.graphics.setColor(1, 1, 1)
  for i, m in ipairs(G.menuLayout) do
    if i == G.openMenu then
      love.graphics.setColor(0.35, 0.35, 0.35)
      love.graphics.rectangle("fill", m.x, 0, m.w, 18)
      love.graphics.setColor(1, 1, 1)
    end
    G.font.draw(m.title, m.x + menuMod.PAD, menuMod.BAR_Y)
  end

  local open = G.menuLayout[G.openMenu]
  if not open then return end
  local d = open.drop
  love.graphics.setColor(0.27, 0.27, 0.27)
  love.graphics.rectangle("fill", d.x, d.y, d.w, d.h)
  love.graphics.setColor(0.6, 0.6, 0.6)
  love.graphics.rectangle("line", d.x + 0.5, d.y + 0.5, d.w - 1, d.h - 1)
  for _, r in ipairs(d.rows) do
    if r.label == "-" then
      love.graphics.setColor(0.5, 0.5, 0.5)
      love.graphics.line(r.x + 4, r.y + 2, r.x + r.w - 4, r.y + 2)
    else
      love.graphics.setColor(1, 1, 1)
      G.font.draw(r.label, r.x + menuMod.PAD, r.y + 1)
      if r.key then
        G.font.draw(r.key, r.x + r.w - menuMod.PAD - G.font.width(r.key), r.y + 1)
      end
    end
  end
end

-- The bottom bar's eight army slots and the movement bar under each are real
-- controls, so their rects come from BUTTON.DAT rather than from us: ids
-- 224-231 are the slots and 232-239 the bars. They carry no art of their own
-- (bitmap 0), which is the layout's way of saying the game draws them.
local SLOT_FIRST, BAR_FIRST, SLOT_COUNT = 224, 232, 8

local function drawArmySlots()
  for i = 0, SLOT_COUNT - 1 do
    local a = G.selection.stack[i + 1]
    if not a then break end
    local slot = screen.control(G.screen, SLOT_FIRST + i)
    local bar  = screen.control(G.screen, BAR_FIRST + i)
    if slot then
      local owner = a.owner or 8
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.armyImg[owner], G.armyQuads[owner][a.type % 32],
        slot.x + math.floor((slot.w - ARMY_CELL) / 2), slot.y)
    end
    if bar then
      local text = tostring(a.moves or 0)
      G.font.draw(text, bar.x + math.floor((bar.w - G.font.width(text)) / 2), bar.y)
    end
  end
end

local function drawBottomBar()
  local r = G.barRect
  love.graphics.setColor(1, 1, 1)
  if G.selection and #G.selection.stack > 0 then
    drawArmySlots()
  else
    G.font.draw(("Cities %d    %d gp    income %d    upkeep %d"):format(
      #game.sideCities(G.g, G.player), G.player.gold,
      G.player.income or 0, G.player.upkeepTotal or 0),
      r.x + 8, r.y + 6)
  end
  G.font.draw(G.status or "", r.x + 8, r.y + r.h - G.font.lineHeight - 4)
end

-- The window is exactly 640x480, so there is no transform: screen coordinates
-- and window coordinates are the same. That matters beyond tidiness --
-- love.graphics.setScissor takes window pixels and ignores any transform, so
-- scaling here would clip the map and the strategic map to the wrong place.
--- The city dialog, laid out as the original's: its rect is (80,60) 480x320,
--- with the strategic map filling the left 224x312 -- which is exactly the
--- 112x156 map at two pixels a tile -- and the city panel on the right. The
--- buttons are its own controls, cut from CITYBU.PCK.
local CITY_RECT = { x = 80, y = 60, w = 480, h = 320 }

local function cityControl(id)
  for _, k in ipairs(G.cityView.dialog.controls) do
    if k.id == id then return k end
  end
  return nil
end

-- Ring cell 0 is grey and cells 1-8 are the side colours, so a side's ring is
-- its index plus one. Checked against a screenshot where the second shield's
-- side had the yellow ring, which is cell 2.
local function ringFor(sideIndex)
  if not sideIndex then return 0 end
  return math.max(0, math.min(8, sideIndex + 1))
end

local function drawCity()
  local c = G.city
  local R = CITY_RECT

  love.graphics.setColor(1, 1, 1)
  love.graphics.setScissor(R.x, R.y, R.w, R.h)
  love.graphics.draw(G.marble, R.x, R.y)
  love.graphics.setScissor()

  -- the strategic map fills the dialog's left panel
  if not G.stratImage then
    G.stratImage = screen.strategicImage(G.screen, G.g, G.player, game.seen)
  end
  love.graphics.setScissor(R.x, R.y, 224, 312)
  love.graphics.draw(G.stratImage, R.x, R.y, 0, 2, 2)
  love.graphics.setColor(1, 1, 1)
  love.graphics.rectangle("line", R.x + c.x * 2 - 1.5, R.y + c.y * 2 - 1.5, 5, 5)
  love.graphics.setScissor()

  -- the name, centred over the right panel, in the font the original uses
  love.graphics.setColor(1, 1, 1)
  G.titleFont.draw(c.name, 432 - math.floor(G.titleFont.width(c.name) / 2), R.y + 2)

  local building = c.slots and c.producing and c.slots[c.producing]
  G.bigFont.draw("Current:", 350, R.y + 53)
  if building then
    local owner = c.ownerIndex or 8
    love.graphics.draw(G.abits, G.ringQuads[ringFor(c.ownerIndex)], 444, R.y + 45)
    love.graphics.draw(G.armyImg[owner], G.armyQuads[owner][building.type % 32],
                       448, R.y + 49)
    G.bigFont.draw(("%dt"):format(c.countdown or building.time), 492, R.y + 53)
  else
    G.bigFont.draw("nothing", 444, R.y + 53)
  end

  -- the production choices, each on a ring: grey, or the owner's for the one
  -- being built
  for i = 1, CITY_SLOTS do
    local ctl = cityControl(CITY_SLOT_FIRST + i - 1)
    local slot = c.slots and c.slots[i]
    if ctl and slot then
      local owner = c.ownerIndex or 8
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.abits,
        G.ringQuads[c.producing == i and ringFor(c.ownerIndex) or 0],
        ctl.x, ctl.y + 1)
      love.graphics.draw(G.armyImg[owner], G.armyQuads[owner][slot.type % 32],
                         ctl.x, ctl.y)
    end
  end

  -- BIGARMY is the "producing" symbol, the same picture whatever is built
  if building then
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.bigArmy, 320, R.y + 123)
  end

  -- the chosen type's numbers, where the original lists them
  if building then
    love.graphics.setColor(1, 1, 1)
    local tx, ty = 459, R.y + 126
    G.bigFont.draw(building.name, tx, ty)
    for k, line in ipairs({
      ("Time: %d"):format(building.time),
      ("Cost: %d"):format(building.cost),
      ("Strength: %d"):format(building.strength),
      ("Move: %d"):format(building.move),
    }) do
      G.bigFont.draw(line, tx, ty + 14 + k * (G.bigFont.lineHeight + 4))
    end
  end

  screen.drawDialogControls(G.screen, G.cityView)
end

function love.draw()
  screen.drawBackground(G.screen)
  drawMap()
  drawStrategic()
  screen.drawControls(G.screen)
  drawShortcutLabels()
  drawBottomBar()
  if G.city then drawCity() end
  drawMenuBar()
end

--------------------------------------------------------------------- input

-- A menu item dispatches on its accelerator, so a menu pick and a key press
-- reach the same place -- which is how the original works too, both going
-- through one command dispatcher. docs/re/ui.md > Commands.
local MENU_DOES     -- filled in below, next to the key handler

local function menuPick(key)
  local act = MENU_DOES[key]
  if act then act() else say("%s is not implemented yet.", key) end
end

function love.mousepressed(x, y, button)
  if G.over or G.offer then return end

  if G.city then
    local c = screen.dialogControlAt(G.cityView, x, y)
    if c and c.id >= CITY_SLOT_FIRST and c.id < CITY_SLOT_FIRST + CITY_SLOTS then
      pickProduction(c.id - CITY_SLOT_FIRST + 1)
    elseif c and (c.id == CITY_DONE or c.id == CITY_DONE_ALT) then
      closeCity()
    elseif c and c.id == CITY_STOP then
      if G.city.ownerIndex == G.player.index then
        game.setProduction(G.g, G.city, nil)
        say("%s builds nothing.", G.city.name)
      end
    elseif x < CITY_RECT.x or y < CITY_RECT.y
        or x >= CITY_RECT.x + CITY_RECT.w or y >= CITY_RECT.y + CITY_RECT.h then
      closeCity()          -- a click outside the dialog dismisses it
    end
    return
  end

  -- the menu bar takes precedence over everything beneath it
  local hit = menuMod.titleAt(G.menuLayout, x, y, 18)
  if hit then
    G.openMenu = (G.openMenu == hit) and nil or hit
    return
  end
  if G.openMenu then
    local row = menuMod.rowAt(G.menuLayout, G.openMenu, x, y)
    G.openMenu = nil
    if row and row.key then menuPick(row.key) end
    return
  end

  local c = screen.controlAt(G.screen, x, y)
  if c then
    G.pressed = c.id
    G.screen.state[c.id] = uidata.ACTIVE
    return
  end

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

-- A control's id, minus 100, indexes a 397-entry jump table at 17be:0b0c --
-- that is what turns a click into an action, and several of its entries are
-- the very routines the keyboard table calls, which is how a button and a key
-- are known to do the same thing. docs/re/ui.md > Commands.
--
-- Wired here are only the ones whose meaning is established. The rest press
-- and release but do nothing, rather than being guessed at.
local ACTION = {}

-- The 3x3 pad, ids 320-327, all reach 8611:0723 with the id minus 320, and
-- that routine steps the cursor by one in x, y or both. Laid out on screen
-- the eight ids run clockwise from north, which is what fixes the order.
local PAD_STEP = {
  [0] = { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 },
         { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 },
}
for i = 0, 7 do
  local step = PAD_STEP[i]
  ACTION[320 + i] = function()
    if not G.selection then say("Nothing is selected.") return end
    moveSelection(G.selection.x + step[1], G.selection.y + step[2])
  end
end

-- Ids 179-182 are the four **configurable** buttons, which is why their art in
-- BUTTON.PCK is blank and why they were the hardest to place. 545c:0072 reads
-- the menu item assigned to button n from UDB/UDB.CUR, turns it into a command
-- code and runs it through the same dispatcher a key press uses. The shipped
-- assignment is Search, Move All, Heroes, End Turn.
local SHORTCUT_DOES = {
  ["Search"] = function()
    if not G.selection then say("Nothing is selected.") return end
    local found = game.searchHere(G.g, G.selection.stack)
    say("%s", found and game.describeSearch(found) or "There is nothing here to search.")
  end,
  ["End Turn"] = function() endTurn() end,
}

local function shortcutName(n)
  local id = G.screen.ui.shortcuts[n]
  return id and G.screen.ui.shortcutNames[id] or nil
end

for i = 0, uidata.SHORTCUT_COUNT - 1 do
  ACTION[uidata.SHORTCUT_FIRST + i] = function()
    local name = shortcutName(i)
    if not name then say("That button has nothing assigned.") return end
    local act = SHORTCUT_DOES[name]
    if act then act() else say("%s is not implemented yet.", name) end
  end
end

-- The pad's centre, id 177, shares its handler (8065:0f02) with the Home key.
ACTION[177] = function()
  if G.selection then centreOn(G.selection.x, G.selection.y)
  else centreOn(G.player.capital.x, G.player.capital.y) end
end

function love.mousereleased(x, y, button)
  local id = G.pressed
  if not id then return end
  G.pressed = nil
  G.screen.state[id] = uidata.NORMAL

  local c = screen.controlAt(G.screen, x, y)
  if not c or c.id ~= id then return end          -- released off the button
  local act = ACTION[id]
  if act then act() else say("Button %d is not wired up yet.", id) end
end

-- Each menu accelerator, as far as this engine can honour it. The names are
-- the original's (docs/re/ui.md > The menu); what is missing says so rather
-- than failing quietly.
MENU_DOES = {
  ["alt E"] = function() endTurn() end,
  ["^Q"]    = function() love.event.quit() end,
  ["alt S"] = function()
    saveMod.write(G.g, G.savePath)
    say("Saved to %s.", G.savePath)
  end,
  ["alt L"] = function() loadGame() end,
  ["z"] = function()
    if not G.selection then say("Nothing is selected.") return end
    local found = game.searchHere(G.g, G.selection.stack)
    say("%s", found and game.describeSearch(found) or "There is nothing here to search.")
  end,
  [","] = function()
    if not G.selection then say("Nothing is selected.") return end
    local a = G.selection.stack[1]
    say("%s: strength %d, %d of %d moves.", a.name, a.strength, a.moves or 0, a.maxMoves)
  end,
  ["g"] = function()
    say("Gold %d, income %d, upkeep %d.", G.player.gold,
        G.player.income or 0, G.player.upkeepTotal or 0)
  end,
  ["c"] = function()
    say("You hold %d cities.", #game.sideCities(G.g, G.player))
  end,
  ["s"] = function()
    if not G.selection then say("Nothing is selected.") return end
    local names = {}
    for _, a in ipairs(G.selection.stack) do names[#names + 1] = a.name end
    say("Stack: %s.", table.concat(names, ", "))
  end,
}

function love.keypressed(key)
  if key == "escape" then
    if G.openMenu then G.openMenu = nil return end
    if G.city then closeCity() return end
    love.event.quit() return
  end
  if G.over then return end
  if G.openMenu then G.openMenu = nil end

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
  -- Home shares its handler with the pad's centre button (8065:0f02)
  elseif key == "home" then ACTION[177]()
  elseif key == "c" and G.selection then centreOn(G.selection.x, G.selection.y)
  elseif key == "f5" then
    saveMod.write(G.g, G.savePath)
    say("Saved to %s.", G.savePath)
  elseif key == "f9" then loadGame()
  elseif key == "up" or key == "w" then G.cy = G.cy - 1; clampCamera()
  elseif key == "down" or key == "s" then G.cy = G.cy + 1; clampCamera()
  elseif key == "left" or key == "a" then G.cx = G.cx - 1; clampCamera()
  elseif key == "right" or key == "d" then G.cx = G.cx + 1; clampCamera()
  end
end

-- LOVE ignores what main.lua returns; test/ui.lua uses it to inspect state.
return G
