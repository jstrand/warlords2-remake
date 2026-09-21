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
local showBanner, dismissBanner                      -- and these two

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
  -- CITY.PCK is 320x312: the gatehouse behind the start-of-turn banner
  G.cityPic = pck.toImage(dataDir .. "/PICS/CITY.PCK", palette)
  -- MHERO.PCK and FHERO.PCK are 224x170 each, exactly the rect the hero
  -- offer blits them into; the checkbox picks between them.
  G.heroPic = {
    m = pck.toImage(dataDir .. "/PICS/MHERO.PCK", palette),
    f = pck.toImage(dataDir .. "/PICS/FHERO.PCK", palette),
  }
  -- ATRANS2.PCK is 144x246 of map markers on a two-colour mask; the hero is
  -- the white figure at (96, 0). Only colour 15 is his -- 1 is the sheet's
  -- ground and 2 the shading, and the original draws neither.
  G.atrans = pck.toImage(dataDir .. "/TERRAIN0/ATRANS2.PCK", palette, { 1, 2 })
  G.heroMark = love.graphics.newQuad(96, 0, 16, 20, 144, 246)
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
  G.cityMode = 3            -- the dialog opens on production

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

  say("Click a stack, then click where to go.")
  showBanner(G.player)
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

-- The start-of-turn banner (8cc6:0259). Popup 6 of the table at 4125:06a8 is
-- (160, 60) 320x312 -- exactly CITY.PCK, which FILE.DAT group 3 gives as
-- bitmap 28, the id 54f6:0000 loads for this popup. 54f6:0000 then frames it
-- in the side's own colour, and the turn routine writes two centred lines
-- over it at x = 320: the side's name at y = 85 and "Turn %d" at y = 130.
-- 7ecb:0142 then blocks until any input arrives.
local BANNER = { x = 160, y = 60, w = 320, h = 312 }
local BANNER_NAME_Y, BANNER_TURN_Y = 85, 130

function showBanner(side)
  G.banner = { name = side.name, turn = G.g.turn,
               colour = side.colour or 15, edge = side.edge or 0 }
end

--- Close the banner, and only then let the turn's first dialog through. The
--- original's turn routine blocks on 7ecb:0142 until the banner is clicked
--- away and reaches the hero offer afterwards, so the two are never both up.
function dismissBanner()
  G.banner = nil
  presentOffer(G.player)
end

-- The hero offer is popup 2 -- (80, 60) 480x312, no bitmap of its own, so it
-- is MARBLE.PCK cropped -- with dialog 12's controls laid on it. Every number
-- below is read out of auto_ui_hero_emerges (6563:0d5c), which pushes the
-- popup, draws the minimap, the picture and four lines of text, and then puts
-- up the name field, the two checkboxes and the buttons.
--
-- Ghidra drops the arguments to the drawing calls, so the coordinates come
-- from the disassembly: they are DGROUP statics at 4125:1112 onwards, and the
-- control rects in BUTTON.DAT group 9 agree with them exactly.
local HERO_POPUP = { x = 80, y = 60, w = 480, h = 312 }
local HERO_DIALOG = 12                 -- JOIN.DAT: dialog 12 -> button group 9
-- AREA.DAT screen 0 has no regions for this dialog, but screen 6's one region
-- gives the map panel's size: 224x312 is the whole 112x156 map at 2 pixels a
-- tile, the same scale as the strategic map on the main screen.
local HERO_MAP = { x = 80, y = 60, w = 224, h = 312 }
local HERO_PIC = { x = 320, y = 110, w = 224, h = 170 }   -- MHERO/FHERO, exactly
local HERO_TITLE_Y = 63
local HERO_CENTRE = 432                -- the centre line of the right-hand half
local HERO_LINE_Y = { 190, 210, 230, 250 }
-- the labels are drawn right-aligned, ending just short of their box
local HERO_MALE_LABEL, HERO_FEMALE_LABEL = { x = 376, y = 315 }, { x = 480, y = 315 }
local HERO_BOX_W, HERO_BOX_H = 24, 20  -- ABITS: checked at (320,0), clear below
local HERO_CHECKED, HERO_CLEAR = { x = 320, y = 0 }, { x = 320, y = 20 }
-- control ids in group 9
local HERO_OK, HERO_CANCEL, HERO_FIELD = 287, 288, 289
local HERO_MALE, HERO_FEMALE = 290, 291
-- STRING.DAT groups: the title, the four caption lines, the two labels
local HERO_TITLE_GROUP, HERO_LINE_GROUP, HERO_SEX_GROUP = 0x5f, 0x61, 0x62
-- hero_recruit copies the name into a 20-byte slot (2c04:0223, stride 0x14)
local HERO_NAME_MAX = 19

--- The four lines over the picture, as 6563:0d5c assembles them. The first
--- two are empty on turn 1, which is why the free hero's caption sits low.
local function heroLines(offer, female)
  local ui, side = G.screen.ui, G.player
  local first = female and 8 or 0             -- the female wording is +8
  local function s(i) return uidata.text(ui, HERO_LINE_GROUP, first + i) end
  if offer.first then
    return { s(0), s(1), s(2), s(3):format(offer.city.name) }
  end
  return { s(0):format(offer.city.name), s(1):format(offer.price),
           s(2):format(side.gold), s(3) }
end

function presentOffer(side)
  if not side.heroOffer then return false end
  if not G.heroView then G.heroView = screen.dialog(G.screen, HERO_DIALOG) end
  local offer = side.heroOffer
  G.offer = offer
  -- The name and sex were rolled with the offer; the checkboxes only change
  -- the wording and the picture, never the name -- toggling one in the
  -- original leaves a Mystichla standing there in a man's portrait.
  G.offerFemale = offer.female or false
  G.offerName = offer.name or "Hero"
  -- the first hero is free and cannot be turned down, so Cancel is disabled
  G.heroView.state[HERO_CANCEL] = offer.first and uidata.DISABLED or uidata.NORMAL
  G.heroView.state[HERO_OK] = uidata.NORMAL
  return true
end

--- Take the offer: the hero joins under the name and sex now in the dialog.
local function acceptOffer()
  local offer = G.offer
  offer.name, offer.female = G.offerName, G.offerFemale
  local h, allies = hero.recruit(G.g, G.player, offer)
  G.offer, G.player.heroOffer = nil, nil
  centreOn(h.x, h.y)
  if #allies > 0 then
    say("%s joins at %s, with %d %s.", h.name,
        G.g.map.cities[h.homeCity + 1].name, #allies, allies[1].name)
  else
    say("%s joins at %s.", h.name, G.g.map.cities[h.homeCity + 1].name)
  end
end

local function refuseOffer()
  G.offer, G.player.heroOffer = nil, nil
  say("The hero rides away.")
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
  say("Turn %d. %d gold, income %d.", G.g.turn, side.gold, side.income or 0)
  showBanner(side)
end

-- The city dialog is dialog 6 (7204:0000 pushes 6 to the dialog opener). Its
-- four 32x70 slots, ids 197-200, are the city's production choices; they carry
-- no art of their own, so the game draws the army in each.
-- docs/formats/screens.md.
local CITY_DIALOG, CITY_SLOT_FIRST, CITY_SLOTS = 6, 197, 4
-- 192 and 201 are the two variants of Done, 202 is Stop -- the octagonal
-- button beside the production row. All three are cut from CITYBU.PCK.
local CITY_DONE, CITY_DONE_ALT, CITY_STOP = 192, 201, 202

-- The dialog has four modes, chosen by the row of buttons along its foot
-- (193-196), and each shows a different set of controls. Read off screenshots
-- of the running game; docs/formats/screens.md > Dialog 6.
local CITY_MODE_FIRST = 193
local MODE_INFO, MODE_CITY, MODE_PRODUCTION, MODE_VECTOR = 1, 2, 3, 4

-- Controls belonging to each mode, beyond the ones always shown.
local MODE_CONTROLS = {
  [MODE_INFO]       = {},
  [MODE_CITY]       = { 203, 205, 204 },   -- Rename, Raze, Build Prod
  [MODE_PRODUCTION] = { 202 },             -- Stop
  [MODE_VECTOR]     = { 210, 211, 212 },   -- vector, change destination, See All
}
-- Always present: the mode buttons and Done.
local MODE_ALWAYS = { 193, 194, 195, 196, 192 }
-- The production list is shown by two of the modes only.
local LIST_MODES = { [MODE_INFO] = true, [MODE_PRODUCTION] = true }

function openCity(city)
  if not G.cityView then G.cityView = screen.dialog(G.screen, CITY_DIALOG) end
  G.city = city
  G.cityMode = G.cityMode or MODE_PRODUCTION
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

--- Burn the city down. Only ours, and only with a stack standing in it.
local function razeCity()
  local c = G.city
  if c.ownerIndex ~= G.player.index then say("%s is not yours.", c.name) return end
  local stack = selectableAt(c.x, c.y)
  if #stack == 0 then say("Nobody of ours stands in %s.", c.name) return end
  game.raze(G.g, G.player, c, stack)
  say("%s is burned to nothing.", c.name)
  stratDirty()
  closeCity()
end

--- Send what this city builds to the city at this point on the strategic map.
local function vectorTo(mx, my)
  local c = G.city
  if c.ownerIndex ~= G.player.index then say("%s is not yours.", c.name) return end
  local dest = game.cityAt(G.g, mx, my)
  if not dest then say("There is no city there.") return end
  if dest.ownerIndex ~= G.player.index then say("%s is not yours.", dest.name) return end
  if dest == c then
    game.vector(G.g, c, nil)
    say("%s keeps what it builds.", c.name)
  else
    game.vector(G.g, c, dest)
    say("%s sends what it builds to %s.", c.name, dest.name)
  end
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

-- The four configurable buttons are blank in BUTTON.PCK, because their art
-- depends on what is assigned to them: 545c:030a paints each one a second
-- time from MENUBUTT.PCK, at the rect the assigned menu item carries in
-- UDB.DAT. The icon is a whole button, frame and all, so it covers the blank.
local function drawShortcutIcons()
  local ui = G.screen.ui
  local art = G.screen.art_for(uidata.SHORTCUT_BITMAP)
  for i = 0, uidata.SHORTCUT_COUNT - 1 do
    local c = screen.control(G.screen, uidata.SHORTCUT_FIRST + i)
    local item = ui.shortcutItems[ui.shortcuts[i] or -1]
    if c and item and art then
      local src = item.src[G.screen.state[c.id] or uidata.NORMAL]
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(art.image,
        love.graphics.newQuad(src.x, src.y, item.w, item.h, art.w, art.h),
        c.x, c.y)
    elseif c and item then                     -- no art: say what it does
      local short = item.name:sub(1, 6)
      love.graphics.setColor(1, 1, 1)
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

--- Draw only the controls this mode shows. The original does the same: the
--- dialog is one screen whose contents change with the foot buttons.
local function drawCityControls()
  G.cityMode = G.cityMode or MODE_PRODUCTION
  local show = {}
  for _, id in ipairs(MODE_ALWAYS) do show[id] = true end
  for _, id in ipairs(MODE_CONTROLS[G.cityMode] or {}) do show[id] = true end
  love.graphics.setColor(1, 1, 1)
  for _, c in ipairs(G.cityView.dialog.controls) do
    if show[c.id] and c.bitmap ~= 0 and c.w > 0 then
      local art = G.screen.art_for(c.bitmap)
      -- the mode you are in shows its button lit
      local st = (c.id == CITY_MODE_FIRST + G.cityMode - 1)
                 and uidata.ACTIVE or uidata.NORMAL
      local sr = c.src[st]
      if art and sr.x + c.w <= art.w and sr.y + c.h <= art.h then
        love.graphics.draw(art.image,
          love.graphics.newQuad(sr.x, sr.y, c.w, c.h, art.w, art.h), c.x, c.y)
      end
    end
  end
end

local function drawCity()
  G.cityMode = G.cityMode or MODE_PRODUCTION
  local c = G.city
  local R = CITY_RECT

  love.graphics.setColor(1, 1, 1)
  love.graphics.setScissor(R.x, R.y, R.w, R.h)
  love.graphics.draw(G.marble, R.x, R.y)
  love.graphics.setScissor()

  if not G.stratImage then
    G.stratImage = screen.strategicImage(G.screen, G.g, G.player, game.seen)
  end
  love.graphics.setScissor(R.x, R.y, 224, 312)
  love.graphics.draw(G.stratImage, R.x, R.y, 0, 2, 2)
  love.graphics.setColor(1, 1, 1)
  love.graphics.rectangle("line", R.x + c.x * 2 - 1.5, R.y + c.y * 2 - 1.5, 5, 5)
  love.graphics.setScissor()

  love.graphics.setColor(1, 1, 1)
  G.titleFont.draw(c.name, 432 - math.floor(G.titleFont.width(c.name) / 2), R.y + 2)

  local owner = c.ownerIndex or 8
  local building = c.slots and c.producing and c.slots[c.producing]

  if G.cityMode == MODE_PRODUCTION then
    -- The "Current" ring is always grey; only the chosen entry in the list
    -- below takes the owner's colour.
    G.bigFont.draw("Current:", 350, R.y + 53)
    if building then
      love.graphics.draw(G.abits, G.ringQuads[0], 444, R.y + 46)
      love.graphics.draw(G.armyImg[owner], G.armyQuads[owner][building.type % 32],
                         444, R.y + 45)
      G.bigFont.draw(("%dt"):format(c.countdown or building.time), 492, R.y + 53)
    else
      G.bigFont.draw("nothing", 444, R.y + 53)
    end
    if building then
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.bigArmy, 320, R.y + 123)
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

  elseif G.cityMode == MODE_INFO then
    love.graphics.setColor(1, 1, 1)
    local tx, ty = 350, R.y + 40
    for k, line in ipairs({
      ("Income: %d gold"):format(c.income),
      ("Defence: %d"):format(c.defence),
      ("Owner: %s"):format(c.ownerIndex and G.g.map.sides[c.ownerIndex + 1].name
                           or "nobody"),
    }) do
      G.bigFont.draw(line, tx, ty + (k - 1) * (G.bigFont.lineHeight + 4))
    end

  elseif G.cityMode == MODE_VECTOR then
    love.graphics.setColor(1, 1, 1)
    G.bigFont.draw("Current:", 340, R.y + 46)
    local dest = c.vectorTo and G.g.map.cities[c.vectorTo + 1]
    G.bigFont.draw(dest and ("vectored to " .. dest.name) or "kept here",
                   340, R.y + 46 + G.bigFont.lineHeight + 4)
    G.bigFont.draw("click a city on the map to vector there",
                   340, R.y + 46 + (G.bigFont.lineHeight + 4) * 3)

  elseif G.cityMode == MODE_CITY then
    love.graphics.setColor(1, 1, 1)
    local labels = { "Give the city a new name", "Burn it to nothing",
                     "Buy new army types" }
    for k, id in ipairs({ 203, 205, 204 }) do
      local ctl = cityControl(id)
      if ctl then G.bigFont.draw(labels[k], ctl.x + ctl.w + 10, ctl.y + 8) end
    end
  end

  -- the production list: the chosen entry rings in the owner's colour
  if LIST_MODES[G.cityMode] then
    for i = 1, CITY_SLOTS do
      local ctl = cityControl(CITY_SLOT_FIRST + i - 1)
      local slot = c.slots and c.slots[i]
      if ctl then
        love.graphics.setColor(1, 1, 1)
        love.graphics.draw(G.abits,
          G.ringQuads[(slot and c.producing == i) and ringFor(c.ownerIndex) or 0],
          ctl.x, ctl.y + 1)
        if slot then
          love.graphics.draw(G.armyImg[owner], G.armyQuads[owner][slot.type % 32],
                             ctl.x, ctl.y)
        end
      end
    end
  end

  drawCityControls()
end

--- The start-of-turn banner: CITY.PCK framed in the side's colour, with the
--- side's name and the turn number centred over it.
local function drawBanner()
  local b, R = G.banner, BANNER

  local function fill(x, y, w, h)
    love.graphics.rectangle("fill", x, y, w, h)
  end
  local function setPal(i)
    local c = G.palette[i + 1] or G.palette[1]        -- pal.lua is 1-based
    love.graphics.setColor(c[1], c[2], c[3])
  end

  -- The shadow is two black runs a side, down and to the right: 1133:02fe
  -- draws a horizontal run, 1133:0344 a vertical, off the popup rect grown
  -- by one. Taken literally that puts them at x+w and y+h of the grown rect,
  -- one clear of the picture, and the blank pixel between shows through --
  -- the original has none, so they sit against the picture here.
  setPal(0)
  fill(R.x + 1, R.y + R.h, R.w + 2, 2)
  fill(R.x + R.w, R.y + 1, 2, R.h + 2)

  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.cityPic, R.x, R.y)

  -- The frame is painted INTO the picture, over its outer ten pixels, and
  -- the result blitted whole -- which is why the banner is exactly CITY.PCK
  -- and no larger. 54f6:0000 outlines it, lays a 9-pixel band inset by one
  -- on each edge, then outlines again 10 in. The outline takes the side's
  -- edge colour (0xb0, black for all but side 7) and the band the side's own
  -- colour (0xa0). All of it is in the picture's own coordinates.
  local function outline(x, y, w, h)
    fill(R.x + x, R.y + y, w, 1)
    fill(R.x + x, R.y + y + h - 1, w, 1)
    fill(R.x + x, R.y + y, 1, h)
    fill(R.x + x + w - 1, R.y + y, 1, h)
  end
  setPal(b.edge)
  outline(0, 0, R.w, R.h)
  setPal(b.colour)
  fill(R.x + 1, R.y + 1, R.w - 2, 9)                  -- top
  fill(R.x + 1, R.y + 1, 9, R.h - 2)                  -- left
  fill(R.x + 1, R.y + R.h - 10, R.w - 2, 9)           -- bottom
  fill(R.x + R.w - 10, R.y + 1, 9, R.h - 2)           -- right
  setPal(b.edge)
  outline(10, 10, R.w - 20, R.h - 20)

  love.graphics.setColor(1, 1, 1)   -- or the frame's colour tints the glyphs
  local f = G.titleFont
  f.draw(b.name, R.x + math.floor((R.w - f.width(b.name)) / 2), BANNER_NAME_Y)
  local turn = ("Turn %d"):format(b.turn)
  f.draw(turn, R.x + math.floor((R.w - f.width(turn)) / 2), BANNER_TURN_Y)
end

--- The hero offer. Popup 2 has no bitmap of its own, so unlike the banner it
--- is not a picture with a frame painted into it: the marble is blitted and a
--- plain black outline drawn round it. Measured off the original's own
--- screenshot, the outline sits at (x - 1, y) and is w + 2 by h + 2, which
--- puts the contents at (x, y + 1) -- a pixel lower than the popup rect says.
local function drawHeroOffer()
  local R, b = HERO_POPUP, G.offer
  local function setPal(i)
    local c = G.palette[i + 1] or G.palette[1]        -- pal.lua is 1-based
    love.graphics.setColor(c[1], c[2], c[3])
  end

  love.graphics.setColor(0, 0, 0)
  love.graphics.rectangle("line", R.x - 0.5, R.y + 0.5, R.w + 1, R.h + 1)
  -- the shadow: two pixels past the outline, down and to the right
  love.graphics.rectangle("fill", R.x + 1, R.y + R.h + 2, R.w + 2, 2)
  love.graphics.rectangle("fill", R.x + R.w + 1, R.y + 2, 2, R.h + 2)

  -- Everything the popup draws lands a pixel below the coordinate it is given
  -- -- the outline is at (x - 1, y) and the contents start at (x, y + 1), so
  -- the whole inside shifts down by one. Rather than add 1 to twenty numbers,
  -- shift once here and use the original's own coordinates throughout.
  love.graphics.push()
  love.graphics.translate(0, 1)

  -- MARBLE.PCK is 480x360, so popup 2's 480x312 is its top-left corner
  love.graphics.setColor(1, 1, 1)
  love.graphics.setScissor(R.x, R.y + 1, R.w, R.h)
  love.graphics.draw(G.marble, R.x, R.y)

  -- the whole map at the strategic map's own 2 pixels a tile
  if not G.stratImage then
    G.stratImage = screen.strategicImage(G.screen, G.g, G.player, game.seen)
  end
  love.graphics.draw(G.stratImage, HERO_MAP.x, HERO_MAP.y, 0, 2, 2)
  for _, c in ipairs(G.g.map.cities) do
    if not c.razed and game.seen(G.g, G.player, c.x, c.y) then
      love.graphics.rectangle("fill", HERO_MAP.x + c.x * 2, HERO_MAP.y + c.y * 2, 4, 4)
    end
  end
  -- Where the hero would appear. The marker is the white figure near the top
  -- right of ATRANS2.PCK -- a mask sheet, so only its white pixels are drawn
  -- and the two darker ones are keyed out. It lands centred on the city's
  -- four map pixels: with Ussyrus at tile (14, 137) the original puts the
  -- cell's corner at (104, 329), which is that centre less (4, 6).
  love.graphics.draw(G.atrans, G.heroMark,
    HERO_MAP.x + b.city.x * 2 - 4, HERO_MAP.y + b.city.y * 2 - 6)

  -- the portrait, in a one-pixel frame of its own
  love.graphics.draw(G.heroPic[G.offerFemale and "f" or "m"], HERO_PIC.x, HERO_PIC.y)
  love.graphics.setColor(0, 0, 0)
  love.graphics.rectangle("line", HERO_PIC.x - 0.5, HERO_PIC.y - 0.5,
                          HERO_PIC.w + 1, HERO_PIC.h + 1)
  love.graphics.setScissor()

  local ui = G.screen.ui
  local function centred(f, s, y)
    love.graphics.setColor(1, 1, 1)
    f.draw(s, HERO_CENTRE - math.floor(f.width(s) / 2), y)
  end
  centred(G.titleFont, uidata.text(ui, HERO_TITLE_GROUP, 0), HERO_TITLE_Y)
  for i, line in ipairs(heroLines(b, G.offerFemale)) do
    if line ~= "" then centred(G.bigFont, line, HERO_LINE_Y[i]) end
  end

  -- The name field: a black outline two pixels clear of it (the rect at
  -- 4125:1142), then the field itself sunk into the marble -- dark along its
  -- top and left, light along its bottom and right. Nothing fills it; the
  -- marble shows through.
  local field = screen.dialogControl(G.heroView, HERO_FIELD)
  if field then
    local function line(x, y, w, h) love.graphics.rectangle("fill", x, y, w, h) end
    love.graphics.setColor(0, 0, 0)
    love.graphics.rectangle("line", field.x - 1.5, field.y - 1.5,
                            field.w + 3, field.h + 3)
    -- 4 is the palette's dark grey and 2 its light one, a shade either side
    -- of the marble's 3 (the original's DAC renders them 81 / 146 / 113)
    setPal(4)                                      -- the sunken shadow
    line(field.x, field.y, field.w, 1)
    line(field.x, field.y, 1, field.h)
    setPal(2)                                      -- and its highlight
    line(field.x, field.y + field.h - 1, field.w, 1)
    line(field.x + field.w - 1, field.y, 1, field.h)

    love.graphics.setColor(1, 1, 1)
    G.bigFont.draw(G.offerName, field.x + 4,
                   field.y + math.floor((field.h - G.bigFont.lineHeight) / 2))
  end
  local function label(f, at, s)
    love.graphics.setColor(1, 1, 1)
    f.draw(s, at.x - f.width(s), at.y)
  end
  label(G.bigFont, HERO_MALE_LABEL, uidata.text(ui, HERO_SEX_GROUP, 0))
  label(G.bigFont, HERO_FEMALE_LABEL, uidata.text(ui, HERO_SEX_GROUP, 1))
  local function box(id, on)
    local c = screen.dialogControl(G.heroView, id)
    if not c then return end
    local s = on and HERO_CHECKED or HERO_CLEAR
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.abits,
      love.graphics.newQuad(s.x, s.y, HERO_BOX_W, HERO_BOX_H, 480, 40), c.x, c.y)
  end
  box(HERO_MALE, not G.offerFemale)
  box(HERO_FEMALE, G.offerFemale)

  screen.drawDialogControls(G.screen, G.heroView)
  love.graphics.pop()
end

function love.draw()
  screen.drawBackground(G.screen)
  drawMap()
  drawStrategic()
  screen.drawControls(G.screen)
  drawShortcutIcons()
  drawBottomBar()
  if G.city then drawCity() end
  drawMenuBar()
  -- the turn opens with the banner over the offer, and is dismissed first
  if G.offer then drawHeroOffer() end
  if G.banner then drawBanner() end
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
  -- 7ecb:0142 blocks the turn routine until any input arrives: the banner
  -- eats the click that dismisses it rather than passing it on
  if G.banner then dismissBanner() return end
  if G.over then return end

  -- the hero offer is modal: nothing behind it takes a click
  if G.offer then
    local c = screen.dialogControlAt(G.heroView, x, y)
    if not c then return end
    if c.id == HERO_MALE then G.offerFemale = false
    elseif c.id == HERO_FEMALE then G.offerFemale = true
    elseif c.id == HERO_OK then acceptOffer()
    -- the first hero is free, and its Cancel is disabled rather than absent
    elseif c.id == HERO_CANCEL and not G.offer.first then refuseOffer()
    end
    return
  end

  if G.city then
    local c = screen.dialogControlAt(G.cityView, x, y)
    if c and c.id >= CITY_SLOT_FIRST and c.id < CITY_SLOT_FIRST + CITY_SLOTS then
      pickProduction(c.id - CITY_SLOT_FIRST + 1)
    elseif c and (c.id == CITY_DONE or c.id == CITY_DONE_ALT) then
      closeCity()
    elseif c and c.id >= CITY_MODE_FIRST and c.id < CITY_MODE_FIRST + 4 then
      G.cityMode = c.id - CITY_MODE_FIRST + 1
    elseif c and c.id == CITY_STOP and G.cityMode == MODE_PRODUCTION then
      if G.city.ownerIndex == G.player.index then
        game.setProduction(G.g, G.city, nil)
        say("%s builds nothing.", G.city.name)
      end
    elseif c and c.id == 205 and G.cityMode == MODE_CITY then
      razeCity()
    elseif G.cityMode == MODE_VECTOR
       and x >= CITY_RECT.x and x < CITY_RECT.x + 224
       and y >= CITY_RECT.y and y < CITY_RECT.y + 312 then
      vectorTo(math.floor((x - CITY_RECT.x) / 2), math.floor((y - CITY_RECT.y) / 2))
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
  -- any key, escape included, only dismisses the banner
  if G.banner then dismissBanner() return end

  -- The dialog owns the keyboard while it is up -- escape included, or the
  -- game would quit out from under it. The name field is editable, so a
  -- letter types into it rather than running a command.
  if G.offer then
    if key == "return" or key == "kpenter" then acceptOffer()
    elseif key == "backspace" then G.offerName = G.offerName:sub(1, -2)
    elseif key == "escape" and not G.offer.first then refuseOffer()
    end
    return
  end

  if key == "escape" then
    if G.openMenu then G.openMenu = nil return end
    if G.city then closeCity() return end
    love.event.quit() return
  end
  if G.over then return end
  if G.openMenu then G.openMenu = nil end

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

-- Typing into the hero's name field. The original's is a real edit box, and
-- the rolled name is only its suggestion.
function love.textinput(text)
  if not G.offer then return end
  if #G.offerName < HERO_NAME_MAX and text:match("^[%w%s'%-%.]+$") then
    G.offerName = G.offerName .. text
  end
end

-- LOVE ignores what main.lua returns; test/ui.lua uses it to inspect state.
return G
