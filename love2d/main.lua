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
local diplomacy = require("warlords.diplomacy")
local hero     = require("warlords.hero")
local armytype = require("warlords.armytype")
local saveMod  = require("warlords.save")
local screen   = require("warlords.screen")
local uidata   = require("warlords.uidata")
local font     = require("warlords.font")
local menuMod  = require("warlords.menu")
local slotsMod = require("warlords.slots")
local kit      = require("ui.kit")
local cityUi   = require("ui.city")
local reportsUi = require("ui.reports")
local heroInfo  = require("ui.heroinfo")
local levelsUi  = require("ui.levels")
local searchUi  = require("ui.search")
local questUi   = require("ui.quest")

local TILE = screen.TILE
-- An army sheet is 16 cells across on a 32-pixel stride; its rows are 30
-- apart and 29 tall, which is what 8611:08be reads out of it -- not the 32
-- the cell width suggests.
local ARMY_CELL, ARMY_COLS, ARMY_ROW, ARMY_H = 32, 16, 30, 29
local ROAD_STRIDE, ROAD_COLS = 48, 13
local ROAD_KEY, ARMY_KEY, SHADOW_KEY = 1, 10, 15
local SCROLL_KEY = 10                  -- SCROLL.PCK stands on green, as the armies do
-- WAR.PCK sits on colour 1 and BSHIELD.PCK on the green it is drawn over
local WAR_KEY, SHIELD_KEY = 1, 12
local RING_W, RING_H = 32, 30      -- one ABITS ring
local CURS_RATE = 18.2 / 4          -- CURS.PCK frames a second (177b:011f)

local G = {}

local presentOffer, stratDirty, openCity, closeCity  -- defined below
local drawStack
local viewCity                                      -- and this one
local reslot                                        -- and this one
local startAssault, pressAssault, presentVictory, takeCity   -- the assault
local refreshControls                               -- and the button states
local showBanner, dismissBanner                      -- and these two

local function quadsFor(img, cols, cellW, cellH, stride, count, rowStride)
  local qs = {}
  for i = 0, count - 1 do
    qs[i] = love.graphics.newQuad(
      (i % cols) * (stride or cellW), math.floor(i / cols) * (rowStride or cellH),
      cellW, cellH, img:getDimensions())
  end
  return qs
end

--- Wall time, for the things that play out on their own: the walk and the
--- battle window. The test harness has a clock of its own.
local function now()
  return (love.timer and love.timer.getTime and love.timer.getTime()) or 0
end

local function exists(path)
  local f = io.open(path, "rb")
  if f then f:close() return true end
  return false
end

-- Where the game's running commentary collects. Nothing draws it: the
-- original has no message line in the bottom bar, and says these things
-- through popups and the advisor instead.
local function say(fmt, ...)
  G.status = select("#", ...) > 0 and fmt:format(...) or fmt
end

--------------------------------------------------------------------- loading

function love.load(arg)
  arg = arg or {}
  -- with no scenario named the game opens on its start screens
  G.starting = arg[1] == nil
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
  -- A0-A8 are the sides' armies and ASHADOW the same sheet drawn as a ghost,
  -- which is how an army that is not moving with the group is shown.
  G.armyImg, G.armyQuads = {}, {}
  for i = 0, 8 do
    local img = pck.toImage(dataDir .. "/TERRAIN0/A" .. i .. ".PCK", palette, ARMY_KEY)
    G.armyImg[i] = img
    G.armyQuads[i] = quadsFor(img, ARMY_COLS, ARMY_CELL, ARMY_H, ARMY_CELL, 32, ARMY_ROW)
  end
  -- the shadow sheet keys on white, not on the sides' key colour: it is two
  -- colours of ghost on a white ground
  G.shadowImg = pck.toImage(dataDir .. "/TERRAIN0/ASHADOW.PCK", palette, SHADOW_KEY)

  -- MARBLE.PCK is 480 wide, exactly the city dialog's width
  G.marble = pck.toImage(dataDir .. "/PICS/MARBLE.PCK", palette)
  -- BIGARMY.PCK is one 128x128 symbol for "this city is producing", not a
  -- picture of the army; ABITS.PCK holds nine 40x40 rings, grey then one per
  -- side, used to ring the chosen production in the owner's colour.
  G.bigArmy = pck.toImage(dataDir .. "/PICS/BIGARMY.PCK", palette)
  -- CITY.PCK is 320x312: the gatehouse behind the start-of-turn banner
  G.cityPic = pck.toImage(dataDir .. "/PICS/CITY.PCK", palette)
  -- The assault: WAR.PCK's fire cloud over the map, BSHIELD.PCK's big shields
  -- beside the two battle lines, and VICTORY.PCK behind the spoils dialog.
  G.warPic = pck.toImage(dataDir .. "/PICS/WAR.PCK", palette, WAR_KEY)
  G.shieldImg = pck.toImage(dataDir .. "/TERRAIN0/BSHIELD.PCK", palette, SHIELD_KEY)
  G.victoryPic = pck.toImage(dataDir .. "/PICS/VICTORY.PCK", palette)
  -- MHERO.PCK and FHERO.PCK are 224x170 each, exactly the rect the hero
  -- offer blits them into; the checkbox picks between them.
  G.heroPic = {
    m = pck.toImage(dataDir .. "/PICS/MHERO.PCK", palette),
    f = pck.toImage(dataDir .. "/PICS/FHERO.PCK", palette),
  }
  -- ATRANS2.PCK is 144x246 of map markers on a mask colour, 1: the side
  -- shields for the turn strip and the strategic map, and the hero's figure.
  -- Drawn through a mask on colour 1 alone: colour 2 is part of the art
  -- (1997:027e, from 8cc6:0952).
  G.atransShields = pck.toImage(dataDir .. "/TERRAIN0/ATRANS2.PCK", palette, 1)
  G.heroMark = love.graphics.newQuad(96, 0, 16, 15, 144, 246)     -- 4125:2cb6
  G.bagQuad = love.graphics.newQuad(64, 0, 32, 29, 144, 246)
  G.blastQuad = love.graphics.newQuad(32, 0, 32, 29, 144, 246)   -- 6a35:0000
  -- HIDDEN.PCK: the hidden map's edges, fourteen 40 x 40 cells 48 apart in
  -- two rows 41 apart; colour 1 is where the map shows through
  G.fogImg = pck.toImage(dataDir .. "/PICS/HIDDEN.PCK", palette, 1)
  G.fogQuads = {}
  for c = 0, 13 do
    G.fogQuads[c] = love.graphics.newQuad((c % 7) * 48, math.floor(c / 7) * 41, 40, 40,
                                          G.fogImg:getDimensions())
  end
  -- CURS.PCK: the marching box round the selected stack (177b:01a1). Frame
  -- k is the 40 x 40 cell at ((k % 4) * 64, (k / 4) * 40), drawn at the
  -- tile's corner: 0-3 box a lone army in 30 x 30 at (10, 10), 4-7 a group
  -- in the whole tile. Keyed on 0; on the original's screen the sheet's 1
  -- shows white and its 4 black (checked frame by frame).
  do
    local w, h, px = pck.decode(dataDir .. "/PICS/CURS.PCK")
    G.cursImg = pck.imageFromPixels(w, h, px, palette, { [1] = 15, [4] = 0 }, { [0] = true })
  end
  G.cursQuads = {}
  for k = 0, 7 do
    G.cursQuads[k] = love.graphics.newQuad((k % 4) * 64, math.floor(k / 4) * 40, 40, 40,
                                           G.cursImg:getDimensions())
  end
  -- The rings are 32 x 30 on a 32-pixel stride, nine of them from x = 0; the
  -- rest of the 480 x 40 sheet is other bits, and a 40 x 40 cell drags them in.
  -- Drawn opaque, as 1997:0129 blits every bitmap it is given: a cell's own
  -- grey ground replaces whatever was under it. Checked pixel for pixel
  -- against the status bar of the original running.
  G.abits = pck.toImage(dataDir .. "/PICS/ABITS.PCK", palette)
  G.ringQuads = {}
  for i = 0, 8 do
    G.ringQuads[i] = love.graphics.newQuad(i * RING_W, 0, RING_W, RING_H, 480, 40)
  end

  -- STAND.PCK: the twelve mouse pointers, drawn by the game itself (22bf)
  G.pointerImg = pck.toImage(dataDir .. "/STAND.PCK", palette, 10)
  G.pointerQuads = {}
  for i = 0, 11 do
    G.pointerQuads[i] = love.graphics.newQuad(i * 16, 0, 16, 16, G.pointerImg:getDimensions())
  end
  if love.mouse and love.mouse.setVisible then love.mouse.setVisible(false) end

  -- the original's own chrome and fonts
  G.screen = screen.load(dataDir, palette, uidata.MAIN_SCREEN, 0)
  G.font = font.load(dataDir, "TEXT", palette, 3)
  G.bigFont = font.load(dataDir, "CHANCE17", palette, 3)
  G.titleFont = font.load(dataDir, "CHANCE36", palette, 3)

  G.menuLayout = menuMod.layout(G.font, menuMod.BAR_H, screen.WIDTH)
  G.openMenu = nil
  -- SHIELDS.PCK: every side's shield, 40x40 in a frame, for the dialogs that
  -- show whose something is; CITYBACK.PCK the ground a city's picture sits on
  G.shieldsImg = pck.toImage(dataDir .. "/TERRAIN0/SHIELDS.PCK", palette)
  G.cityBack = pck.toImage(dataDir .. "/TERRAIN0/CITYBACK.PCK", palette)
  -- the pictures of Hero > Search: a ruin, and a temple
  G.searchPic = pck.toImage(dataDir .. "/PICS/SEARCH.PCK", palette)
  G.templePic = pck.toImage(dataDir .. "/PICS/TEMPLE.PCK", palette)
  -- the quest's parchment, blitted through a mask (1997:027e)
  G.scrollPic = pck.toImage(dataDir .. "/PICS/SCROLL.PCK", palette, SCROLL_KEY)
  kit.init(G)
  -- the music, effects and advisor (sound.lua), switched as DATA/OPTIONS.SND
  -- says; FILE.DAT names every song and sample
  G.audio = require("sound")
  G.audio.init(dataDir, G.screen.ui.files)

  G.mapRect  = screen.region(G.screen, screen.REGION.MAP)
  G.stratRect = screen.region(G.screen, screen.REGION.STRATEGIC)
  G.barRect  = screen.region(G.screen, screen.REGION.BOTTOMBAR)
  G.menuRect = screen.region(G.screen, screen.REGION.MENUBAR)

  -- A third argument fixes the seed, so a run can be reproduced exactly.
  -- test/ui.lua passes one; without it every game is different.
  local seed = tonumber(arg[3]) or os.time()
  G.scenario = scenario
  love.graphics.setBackgroundColor(0, 0, 0)
  if G.starting then
    openStart()
    -- 7f77:0000 greets the player the first time round: "Greetings, Warlord!"
    require("ui.advisor").say(require("warlords.cues").GREET)
    return
  end
  newGame(seed)
  beginGame()
  say("Click a stack, then click where to go.")
end

--- The start screens (7f77:0000): a new game set up there, or a saved one.
function openStart()
  G.starting = true
  G.audio.music(require("warlords.cues").TITLE)
  require("ui.start").open(function(dir, options, sides, extra)
    -- 7bab:0cfe: the war begins to its own music, and the advisor says so
    -- before anything is set up
    local cues = require("warlords.cues")
    G.audio.music(cues.BEGIN)
    require("ui.advisor").say(cues.BEGIN_WAR, function()
      G.starting = false
      G.scenario = dir
      G.seed = os.time()
      G.g = game.new(G.dataDir, dir, { seed = G.seed, options = options, sides = sides,
                                        greatest = extra and extra.greatest })
      G.selection, G.over, G.stratImage = nil, nil, nil
      beginGame()
      if G.player.computer and G.playComputer then G.playComputer() end
    end)
  end, function(g)
    G.starting = false
    G.takeLoaded(g)
  end)
end

--- A fresh game of the scenario. Until the new-game screens set the sides
--- up, the first side is the player's and the rest are the computer's;
--- Settings can change that.
function newGame(seed)
  G.seed = seed
  G.g = game.new(G.dataDir, G.scenario, { seed = seed })
  for i, s in ipairs(G.g.sides) do s.computer = i > 1 end
  G.selection, G.over = nil, nil
  G.stratImage = nil
end

--- Start its first turn, and centre the view on the capital.
function beginGame()
  G.player = game.begin(G.g)
  G.cursor = G.player.capital and
             { x = G.player.capital.x, y = G.player.capital.y } or { x = 0, y = 0 }
  G.selection = nil
  -- centre the 9x9 viewport on the capital
  G.cx = math.max(0, math.min(G.player.capital.x - 4, G.g.map.width - screen.VIEW_COLS))
  G.cy = math.max(0, math.min(G.player.capital.y - 4, G.g.map.height - screen.VIEW_ROWS))
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

--------------------------------------------------------------------- walking

-- A stack walks one tile at a time, and the map shows where it is going.
--
-- The destination is kept in the army record itself -- the move target at
-- +0x12/+0x14 -- so it outlives the walk that could not finish: the stack
-- goes as far as its movement allows and the rest of the route stays on
-- screen. Order > Move All (1c8c:04c4) takes every stack that still has one
-- and walks it on.
--
-- 1c8c:0963 turns the pathfinder's directions into a list of tiles, and
-- 8611:2ef5 marks them into the on-screen tile cache: a ring on every tile of
-- the route but the last, plain while the stack can still reach it this turn
-- (index < 4125:2eaa, the steps it can afford) and crossed once it cannot.
-- The last tile takes a ghost of the leading army instead. Both come out of
-- ASHADOW.PCK, which is also where the bar's shadowed armies come from.
--
-- Walking, 1a8b:04c8 steps the stack one tile per pass, re-centres the view
-- on it (8611:0565 -> 8611:0629) and waits a couple of ticks, and bumps
-- 4125:2ea8 so the route drawn behind it shortens as it goes.
-- docs/re/ui.md > Walking.

local WALK = {
  ring     = { 496, 31 },                      -- ASHADOW.PCK, 16 x 14
  crossed  = { 496, 48 },
  ringW    = 16, ringH = 14,
  ringAt   = { 16, 13 },                       -- within the tile
  ghostAt  = { 4, 4 },                         -- as our own armies sit
  stepTime = 0.1,                              -- one tile of the walk
}

--- Work out the route the selection is showing, if it has anywhere to be.
local function refreshRoute()
  G.route = nil
  local sel = G.selection
  if not sel or #sel.stack == 0 then return end
  local target = sel.stack[1].target
  if not target then return end
  if sel.stack[1].x == target.x and sel.stack[1].y == target.y then
    for _, a in ipairs(sel.stack) do a.target = nil end
    return
  end
  G.route = move.preview(G.g, sel.stack, target.x, target.y)
end

--- Give the selection somewhere to be, or take it away again.
local function orderTo(stack, x, y)
  for _, a in ipairs(stack) do
    a.target = (x and { x = x, y = y }) or nil
  end
end

--- Play the walk back a tile at a time, centring on the stack as it goes.
--- The armies are already where they finished -- this only decides where they
--- are drawn until it catches up.
local function startWalk(armies, tiles)
  if #tiles == 0 then return end
  G.walk = { armies = {}, tiles = tiles, i = 1, at = now(), top = armies[1] }
  for _, a in ipairs(armies) do G.walk.armies[a] = true end
  centreOn(tiles[1].x, tiles[1].y)
end

local function advanceWalk()
  local w = G.walk
  if not w then return end
  local t = now()
  while w.i < #w.tiles and t - w.at >= WALK.stepTime do
    w.i = w.i + 1
    w.at = w.at + WALK.stepTime
    centreOn(w.tiles[w.i].x, w.tiles[w.i].y)
  end
  if w.i >= #w.tiles and t - w.at >= WALK.stepTime then
    G.walk = nil
    refreshRoute()
    -- a computer's walk shown, the computer goes on
    if w.computer and G.resumeComputer then G.resumeComputer() end
  end
end

--- Where the walking stack is drawn while the walk plays out.
local function walkingAt()
  local w = G.walk
  return w and w.tiles[w.i] or nil
end

--- The route, as rings on the map: 8611:2ef5's two marks and the ghost it
--- puts on the far end.
local function drawRoute()
  local r, route = G.mapRect, G.route
  if not route then return end
  local ghost = G.selection and G.selection.stack[1]
  -- while the walk plays out, the route is drawn from where it has got to,
  -- which is what 4125:2ea8 does as 1a8b:04c8 bumps it step by step
  local first = G.walk and G.walk.i or 1
  for i = first, #route.path do
    local step = route.path[i]
    local col, row = step.x - G.cx, step.y - G.cy
    if col >= 0 and col < screen.VIEW_COLS and row >= 0 and row < screen.VIEW_ROWS then
      local sx, sy = r.x + col * TILE, r.y + row * TILE
      love.graphics.setColor(1, 1, 1)
      if i == #route.path then
        if ghost then
          love.graphics.draw(G.shadowImg, G.armyQuads[8][ghost.type % 32],
                             sx + WALK.ghostAt[1], sy + WALK.ghostAt[2])
        end
      else
        local src = (i <= route.reach) and WALK.ring or WALK.crossed
        love.graphics.draw(G.shadowImg,
          love.graphics.newQuad(src[1], src[2], WALK.ringW, WALK.ringH, 512, 64),
          sx + WALK.ringAt[1], sy + WALK.ringAt[2])
      end
    end
  end
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

--- Pick up whatever the slots now say moves, and remember the grouping in
--- the armies themselves. Everything else in the front end goes on reading
--- `stack`, so it acts on the group and nothing else.
local function syncSelection()
  local sel = G.selection
  if not sel then return end
  slotsMod.commit(sel.slots, G.g)
  sel.stack = slotsMod.selected(sel.slots)
  -- Being in the group that moves puts an army back in the cycle and marks it
  -- offered for this pass, which is what 89e0:000a and 8c07:06eb do between
  -- them. It is the group, not the tile: an army left out of it keeps
  -- whatever it was.
  for _, a in ipairs(sel.stack) do a.fortified, a.offered = nil, true end
end

--- Put the slots back over a new list of armies -- what is left after a
--- battle, or whoever stands on the tile the group has walked to -- keeping
--- the group that was moving. The original does the same: 89e0:0d30 runs
--- again and keeps the selection when more than one army was in it.
function reslot(armies)
  local sel = G.selection
  if not sel then return end
  local s = armies and #armies > 0 and slotsMod.keep(sel.slots, G.g, armies)
  if not s then G.selection = nil return end
  sel.slots = s
  sel.x, sel.y = s.army[1].x, s.army[1].y
  syncSelection()
end

--- Pick up what is on a tile. `pick` names the army to take, for when the
--- cycle hands one over rather than the player clicking; without it the tile
--- offers up whichever army it shows.
local function select(x, y, pick)
  local stack = selectableAt(x, y)
  if #stack == 0 then
    G.selection = nil
    local city = game.cityAt(G.g, x, y)
    if city then openCity(city) end
    return
  end
  -- Clicking a tile selects one army, not the stack: the bar is where the
  -- group that moves is put together. docs/re/ui.md > The army slots.
  local s = slotsMod.build(G.g, stack, G.player.index,
                           slotsMod.clicked(G.g, stack, G.player.index, pick))
  G.selection = { x = x, y = y, slots = s }
  syncSelection()
  refreshRoute()
  say("%d of %d, %d movement",
      #G.selection.stack, s.n, slotsMod.moves(s))
  -- the tutorial on picking a stack up (1b62:051d-062a): moving; fighting,
  -- next to an enemy city; searching, a hero on a site
  local moments = { "move" }
  for dx = -1, 1 do
    for dy = -1, 1 do
      local c = game.cityAt(G.g, x + dx, y + dy)
      if c and c.ownerIndex ~= G.player.index and not c.razed then moments[2] = "fight" end
    end
  end
  local lead = G.selection.stack[1]
  if lead and lead.type == armytype.HERO and G.g.map.siteAt
     and G.g.map.siteAt[y * G.g.map.width + x] then
    moments[#moments + 1] = "search"
  end
  require("ui.tutorial").chain(moments)
end

------------------------------------------------------------- the army cycle

-- The five buttons above the pad are the turn's rhythm: they walk you through
-- your armies one stack at a time until none is left to move.
--
-- Three bits of the army's flags word at +0xc decide who is still to be
-- offered (8c07:040e):
--
--   0x0001  in the cycle. Selecting a stack sets it (89e0:000a); **fortify**
--           clears it, and nothing at the turn's start puts it back -- which
--           is what makes fortifying outlast the turn.
--   0x0040  done for this turn. **Quit army** sets it (8c07:08fb) and the
--           start of the side's turn clears it (8c07:0113).
--   0x0200  already offered this pass. Selecting a stack sets it, and when
--           the cycle runs out of unoffered armies it clears them all and
--           starts round again.
--
-- The next army is the **nearest** of the eligible ones to where the cycle
-- last stopped, which begins each turn at the side's capital. An army exactly
-- there counts as far away instead of near, so "next" never hands you back
-- the stack you are already standing on. docs/re/ui.md > The army cycle.
local function cycleReset()
  G.cursor = G.player.capital and
             { x = G.player.capital.x, y = G.player.capital.y } or { x = 0, y = 0 }
end

--- Manhattan distance to where the cycle last stopped, with 0 pushed to the
--- back of the queue (8c07:040e's 0x2328).
local function cycleRank(a)
  local d = math.abs(a.x - G.cursor.x) + math.abs(a.y - G.cursor.y)
  return d == 0 and 9000 or d
end

--- The stack the cycle offers next, or nil when every army is done.
local function nextArmy()
  if not G.cursor then cycleReset() end
  local fresh, used
  for _, a in ipairs(G.g.armies) do
    if a.owner == G.player.index and not a.transit and not a.fortified and not a.done then
      if a.offered then
        if not used or cycleRank(a) < cycleRank(used) then used = a end
      else
        if not fresh or cycleRank(a) < cycleRank(fresh) then fresh = a end
      end
    end
  end
  if not fresh then
    -- round again: everything left has been offered once already
    if not used then return nil end
    for _, a in ipairs(G.g.armies) do
      if a.owner == G.player.index then a.offered = nil end
    end
    fresh = used
  end
  G.cursor = { x = fresh.x, y = fresh.y }
  return fresh
end

--- Offer the next stack, centring on it. Nothing left to offer ends the turn's
--- business rather than leaving a stale selection up.
-- for the shot harness and tests: select what stands on a tile
G.selectAt = function(x, y) select(x, y) end

local function selectNext()
  local a = nextArmy()
  if not a then
    G.selection, G.route = nil, nil
    say("Every army has moved.")
    G.audio.effect("chord")                     -- 8c07:019b: nothing left
    return
  end
  G.audio.effect("ding")
  select(a.x, a.y, a)
  centreOn(a.x, a.y)
end

--- Whatever is selected is out of the cycle for the rest of this turn, and on
--- to the next stack (control 175, 8c07:0393).
local function quitArmy()
  if not G.selection then selectNext() return end
  for _, a in ipairs(G.selection.stack) do a.done = true end
  G.route = nil
  selectNext()
end

--- Dig in: out of the cycle until it is picked up again, this turn and every
--- turn after (control 176, 8c07:03a6).
local function fortify()
  if not G.selection then selectNext() return end
  local n = #G.selection.stack
  for _, a in ipairs(G.selection.stack) do
    a.fortified, a.target = true, nil
  end
  say("%d armies dig in.", n)
  G.route = nil
  selectNext()
end

--- Put the selection down (control 178, 8065:0f26 -> 1b62:08b3).
local function deselect()
  G.selection, G.route, G.walk = nil, nil, nil
end

--------------------------------------------------------------------- the turn

local function afterBattle(result)
  require("ui.tutorial").show("fresult")
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
    for i = 1, G.selection.slots.n do
      local a = G.selection.slots.army[i]
      if not result.deadByArmy[a] then alive[#alive + 1] = a end
    end
    reslot(alive)
  end
end

--- Walk the selection towards (x, y). Only `attack` -- a click with the
--- sword up (740d:0179), or a step by hand (1c8c:041f) -- turns running into
--- an enemy into an assault: a walk (1c8c:01fd) just stops beside it.
local function moveSelection(x, y, attack)
  local sel = G.selection
  if not sel then return end
  local from = { x = sel.stack[1].x, y = sel.stack[1].y }
  -- the destination is remembered, so a walk that runs out of movement can be
  -- taken up again -- by Move All, or by hand next turn
  orderTo(sel.stack, x, y)
  G.route = move.preview(G.g, sel.stack, x, y)
  local r = move.moveTo(G.g, sel.stack, x, y)
  if r.stopped == "attack" and not (attack and r.steps == 0) then r.stopped = "blocked" end
  if r.stopped == "attack" then
    -- The stack has walked as far as the tile beside the target; the assault
    -- is fought from there. The fight is resolved first and then shown.
    local result = game.resolveAttack(G.g, sel.stack, r.attack.x, r.attack.y)
    startAssault(r.attack.x, r.attack.y, result)
    afterBattle(result)
  elseif r.stopped == "no route" then
    say("There is no way there.")
    G.audio.effect("chord")                     -- 1a8b:0c4f, 1c8c:0007
  elseif r.steps == 0 then
    say("They cannot move: %s.", r.stopped)
  else
    local walked = sel.stack
    sel.x, sel.y = sel.stack[1].x, sel.stack[1].y
    reslot(selectableAt(sel.x, sel.y))
    stratDirty()
    say("Moved %d for %d. %d left.", r.steps, r.spent, move.stackMoves(sel.stack))
    startWalk(walked, r.walked or {})
  end
  if G.selection and #G.selection.stack > 0 then
    G.selection.x, G.selection.y = G.selection.stack[1].x, G.selection.stack[1].y
    if not G.walk then centreOn(G.selection.x, G.selection.y) end
  end
  if not G.walk then refreshRoute() end
end

------------------------------------------------------------------ the pointer

-- 18a9:0896 decides the mouse pointer from what is under it and what is
-- selected, and a click on the map does whatever that pointer promises
-- (740d:0037's jump table on the same number) -- so the pointer is the whole
-- rule for what a click does. STAND.PCK (FILE.DAT group 0x18) holds the
-- twelve, 16x16 along one row on colour key 10 (79de:0117 -> 22bf:000a);
-- 22bf:036f takes cell k from (k * 16, 0), and its hotspot is the same
-- distance across and down, from the table at 4125:045c.
local PTR = { ARROW = 0, VIEW = 1, BOAT = 2, CITY = 3, HAND = 4, SELECT = 5,
              WALK = 6, SITE = 7, ATTACK = 8, ADVISE = 9, PEACE = 10, ALT = 11 }
PTR.HOT = { [0] = 0, 6, 8, 6, 8, 8, 8, 8, 0, 8, 8, 0 }
PTR.STRAT = { 400, 30, 224, 312 }         -- 4125:2aa8, the strategic map
PTR.PANEL = { 0, 17, 392, 386 }           -- 4125:2ab8, the map's frame
PTR.OK = { [move.ROAD] = true, [move.BRIDGE] = true,
           [move.WATER] = true, [move.SHORE] = true, [move.CITY] = true }

local function inRect(r, x, y)
  return x >= r[1] and y >= r[2] and x < r[1] + r[3] and y < r[2] + r[4]
end

local function held(a, b)
  return love.keyboard and love.keyboard.isDown and love.keyboard.isDown(a, b)
end

local function pointerKind(x, y)
  if G.starting or G.over or G.banner or G.offer or G.assault or G.victory or G.aiRun
     or G.openMenu or kit.top() or not G.player or G.player.computer then
    return PTR.ARROW
  end
  if G.drag then return PTR.HAND end             -- 4125:1176, while it drags
  local alt, ctrl = held("lalt", "ralt"), held("lctrl", "rctrl")
  if inRect(PTR.STRAT, x, y) then return alt and PTR.ALT or PTR.VIEW end
  local tx, ty = tileAtPoint(x, y)
  if not tx then return inRect(PTR.PANEL, x, y) and PTR.HAND or PTR.ARROW end

  local g, me = G.g, G.player.index
  if tx < 0 or ty < 0 or tx >= g.map.width or ty >= g.map.height then return PTR.HAND end
  local here = game.armiesAt(g, tx, ty)
  local city = game.cityAt(g, tx, ty)
  local armies = here[1] ~= nil
  -- the tile's owner nibble: whoever stands there, else the city's, else none
  local owner = armies and here[1].owner or (city and city.ownerIndex) or rules.NEUTRAL
  -- a razed city is ruins (7204:0000 still opens on it)
  local t = city and (city.razed and move.SITE or move.CITY) or scn.terrainAt(g.map, tx, ty)

  local sel = G.selection and #G.selection.stack > 0 and G.selection.stack
  local dist, mode, under, atSea = 0, nil, move.PLAIN, false
  if sel then
    dist = math.max(math.abs(tx - sel[1].x), math.abs(ty - sel[1].y))
    mode = move.stackMode(g, sel)
    under = scn.terrainAt(g.map, sel[1].x, sel[1].y)
    atSea = sel[1].atSea and true or false
  end
  -- a tile out of sight, or one the stack cannot enter at all, is the hand
  local cost = (sel and mode ~= move.FLYING) and (move.COST[t] or 0) or 1
  if not game.seen(g, G.player, tx, ty) or cost <= 0 then return PTR.HAND end

  local function walk()
    if mode ~= move.FLYING and (t == move.WATER or t == move.SHORE) then return PTR.BOAT end
    return PTR.WALK
  end
  local function look()
    if t == move.CITY then return PTR.CITY end
    if t == move.SITE then return PTR.SITE end
    return PTR.HAND
  end
  local enemyNear = sel and dist <= 1 and owner ~= me and (t == move.CITY or armies)

  if held("lshift", "rshift") then
    if enemyNear and g.map.options.militaryAdvisor ~= 0 then return PTR.ADVISE end
    return look()
  end
  if armies and owner == me and not ctrl then
    if alt then return PTR.ALT end
    if not sel or dist < 1 then return PTR.SELECT end
    return walk()
  end
  -- Next to an enemy the sword -- or the heart, for a side at peace. A land
  -- stack at sea fights only onto water, a shore, a bridge or a city, and
  -- one ashore reaches an enemy on the shore only from the same.
  if enemyNear
     and not (atSea and mode == move.LAND and under == move.SHORE and not PTR.OK[t])
     and not (not atSea and mode == move.LAND and t == move.SHORE and not PTR.OK[under]) then
    if g.map.options.diplomacy == 0 or owner == rules.NEUTRAL then return PTR.ATTACK end
    local st = diplomacy.state(g, me, owner)
    if t == move.CITY then return st == diplomacy.WAR and PTR.ATTACK or PTR.PEACE end
    return st ~= diplomacy.PEACE and PTR.ATTACK or PTR.PEACE
  end
  if sel and (owner == me or (owner == rules.NEUTRAL and t ~= move.CITY)) then
    if alt then return PTR.ALT end
    if dist >= 1 and ctrl and armies then return PTR.SELECT end
    return walk()
  end
  return look()
end
G.pointerKind = pointerKind

local function drawPointer()
  if not (G.pointerImg and love.mouse and love.mouse.getPosition) then return end
  local mx, my = love.mouse.getPosition()
  local k = pointerKind(mx, my)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.pointerImg, G.pointerQuads[k], mx - PTR.HOT[k], my - PTR.HOT[k])
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
  G.audio.effect("turn")                         -- the fanfare, TURN.8SN
end

--- Close the banner, and only then let the turn's first dialog through. The
--- original's turn routine blocks on 7ecb:0142 until the banner is clicked
--- away and reaches the hero offer afterwards, so the two are never both up.
function dismissBanner()
  G.banner = nil
  -- the advisor has his say (6dda:026f(5)) -- with Speech off the original
  -- skips him before he so much as updates his marks -- and then 8cc6:04bd
  -- puts on the hero's music or the turn's
  local cues = require("warlords.cues")
  local says = G.audio.speechOn() and cues.advisor(G.g, G.player, function(n) return math.random(n) end)
  require("ui.advisor").say(says, function()
    G.audio.music(G.player.heroOffer and cues.HERO or cues.PLAY)
    -- the tutorial's pages for the second turn and the hero offer come first
    local tutorial = require("ui.tutorial")
    local moments = {}
    if G.g.turn == 2 then moments[#moments + 1] = "turn2" end
    if G.player.heroOffer then moments[#moments + 1] = "hero" end
    tutorial.chain(moments, function()
      if not presentOffer(G.player) then afterOffer() end
    end)
  end)
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
  afterOffer()
end

--- The turn's opening, once any hero offer is settled (8cc6:04bd, the
--- stage after 7563:0d5c): on turn 1 the capital's dialog opens in
--- Production, for the first thing to build to be chosen.
function afterOffer()
  if G.g.turn == 1 and G.player.capital and not G.player.computer then
    cityUi.open(G.player.capital, cityUi.PRODUCTION)
  end
end

local function refuseOffer()
  G.offer, G.player.heroOffer = nil, nil
  afterOffer()
  say("The hero rides away.")
end

-- The computer's turns. They run in a coroutine, so the screen keeps being
-- drawn and the keyboard read while they are played: it gives a frame back
-- after every side's turn, and stops after each walk worth showing for it to
-- play out on the map. A turn is shown when its side is observed or no human
-- is left (8cc6:0000 -> 2c04:0159); a walk only if the player has seen any
-- of it. Holding Shift or Alt between turns opens Settings (5db9:045e), where
-- a side can be handed back to a human or its watching turned off; any other
-- key or a click runs the rest of the round through unwatched.
local finishRound, resumeComputer, stepComputer, playComputers
do
local function humansLeft()
  for _, s in ipairs(G.g.sides) do
    if s.alive and not s.computer and #game.sideCities(G.g, s) > 0 then return true end
  end
  return false
end

local function shown(side)
  return side and (side.observe or not humansLeft())
end

ai.onWalk = function(g, stack, r)
  local co = G.aiRun
  if not co or coroutine.running() ~= co or G.aiSkip or not shown(G.aiSide) then return end
  local seen = not humansLeft()
  for _, t in ipairs(r.walked or {}) do
    if seen then break end
    if game.seen(g, G.player, t.x, t.y) then seen = true end
  end
  if seen then coroutine.yield("walk", stack, r.walked) end
end

function resumeComputer()
  local co = G.aiRun
  if not co then return end
  G.aiWait = nil
  local ok, what, a, b = coroutine.resume(co)
  if not ok then G.aiRun = nil error(what) end
  if coroutine.status(co) == "dead" then
    G.aiRun, G.aiSkip, G.aiSide = nil, nil, nil
    finishRound(what)
    return
  end
  stratDirty()
  if what == "walk" then
    local armies = {}
    for _, army in ipairs(a) do armies[#armies + 1] = army end
    startWalk(armies, b)
    G.walk.computer = true
  elseif what == "nohumans" then
    -- 8065:1c6f: the last human is gone, and the war goes on
    local t = function(i) return uidata.text(G.screen.ui, 0xd, i) end
    searchUi.message(t(0), t(1), function()
      searchUi.message(t(2), t(3), function() G.aiWait = true end)
    end)
  else
    G.aiWait = true                 -- a turn done: go on at the next frame
  end
end
G.resumeComputer = function() resumeComputer() end

--- Called every frame: carry the computer on once nothing is in its way.
function stepComputer()
  if not G.aiRun or not G.aiWait or G.walk or kit.top() then return end
  if held("lshift", "rshift") or held("lalt", "ralt") then
    G.aiSkip = nil
    require("ui.settings").open()
    return
  end
  resumeComputer()
end

--- Play computer sides from `side` on, until a human's turn or the end.
function playComputers(side)
  G.aiRun = coroutine.create(function()
    while side and side.computer do
      G.aiSide = side
      -- 8065:2123: each computer turn opens with a song that plays once
      G.audio.music(require("warlords.cues").COMPUTER, G.g.sides)
      ai.playTurn(G.g, side)
      side = game.endTurn(G.g)
      if G.g.ending and G.g.ending.noHumans then coroutine.yield("nohumans") end
      coroutine.yield("turn")
    end
    return side
  end)
  resumeComputer()
end
end

local function endTurn()
  G.selection = nil
  local side = game.endTurn(G.g)
  -- the computer plays its sides; a human side, one or several, is handed
  -- to whoever sits at the keyboard
  if side and side.computer then return playComputers(side) end
  finishRound(side)
end

--- The round is over: the next human side takes the keyboard.
function finishRound(side)
  local ending = require("ui.ending")
  if not side then
    G.over = true
    say("%s", G.g.log[#G.g.log] or "The game is over.")
    ending.over(G.g.ending)
    return
  end
  G.player = side
  stratDirty()
  cycleReset()
  say("Turn %d. %d gold, income %d.", G.g.turn, side.gold, side.income or 0)
  -- what the round's end found comes first, then the turn's banner
  ending.show(G.g.ending, function() showBanner(side) end)
end
-- a computer side that opens the game plays its turn before anyone's
G.playComputer = function()
  playComputers(G.player)
end

-- The city dialog lives in ui/city.lua; these are the front end's ways in.
function openCity(city)
  cityUi.open(city)
  say("%s", city.name)
end

function closeCity()
  local top = kit.top()
  if top and top.close then top.close() end
end
G.openCity = function(city) openCity(city) end

local function takeLoaded(loaded)
  G.g, G.selection = loaded, nil
  stratDirty()
  G.player = loaded.sides[loaded.current]
  centreOn(G.player.capital.x, G.player.capital.y)
  G.audio.music(require("warlords.cues").PLAY)     -- 7721:02d3
end

G.takeLoaded = function(g) takeLoaded(g) end
G.startAssault = function(x, y, result) startAssault(x, y, result) end

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

-- The figure a stack shows (8611:2985): its top army's, or a boat -- the
-- Navy, type 5 -- when an army in it is at sea.
local NAVY = 5
local function stackFigure(stack)
  for _, a in ipairs(stack) do
    if a.atSea then return NAVY end
  end
  local a = topArmy(stack)
  return a and a.type
end

-- The hidden map (8611:24c2): over each unseen tile, black through its
-- HIDDEN.PCK cell -- cell c at ((c % 7) * 48, (c / 7) * 41), 40 x 40, black
-- where the fog is and colour 1 where the map shows through (152a:017f with
-- the mask 4125:551e). Cell 14, a tile with nothing seen round it, is black.
local function drawFog()
  if G.g.map.options.hiddenMap == 0 then return end
  local r = G.mapRect
  for row = 0, screen.VIEW_ROWS - 1 do
    for col = 0, screen.VIEW_COLS - 1 do
      local mx, my = G.cx + col, G.cy + row
      local cell = mx < G.g.map.width and my < G.g.map.height
                   and game.fogCell(G.g, G.player, mx, my)
      if cell then
        local sx, sy = r.x + col * TILE, r.y + row * TILE
        if cell == game.FOG_BLACK then
          love.graphics.setColor(0, 0, 0)
          love.graphics.rectangle("fill", sx, sy, TILE, TILE)
        else
          love.graphics.setColor(1, 1, 1)
          love.graphics.draw(G.fogImg, G.fogQuads[cell], sx, sy)
        end
      end
    end
  end
  love.graphics.setColor(1, 1, 1)
end

local function drawMap()
  local r = G.mapRect
  -- one item a tile is enough to draw: the last in item order, as 8611:2d7c
  -- walks them from the end
  G.itemsOnTile = {}
  for i = #G.g.map.items, 1, -1 do
    local it = G.g.map.items[i]
    if it.status == 1 and it.x and not G.itemsOnTile[it.x + it.y * 1000] then
      G.itemsOnTile[it.x + it.y * 1000] = it
    end
  end
  love.graphics.setScissor(r.x, r.y, r.w, r.h)
  for row = 0, screen.VIEW_ROWS - 1 do
    for col = 0, screen.VIEW_COLS - 1 do
      local mx, my = G.cx + col, G.cy + row
      local sx, sy = r.x + col * TILE, r.y + row * TILE
      if mx < G.g.map.width and my < G.g.map.height then
        -- every tile is drawn; the hidden map is laid over it afterwards
        do
          local t = scn.tileAt(G.g.map, mx, my)
          local sheet = math.floor(t / 96) + 1
          love.graphics.setColor(1, 1, 1)
          love.graphics.draw(G.sheets[sheet], G.tileQuads[sheet][t % 96], sx, sy)
          local rd = scn.roadAt(G.g.map, mx, my)
          if rd ~= 0 then love.graphics.draw(G.roadImg, G.roadQuads[rd - 1], sx, sy) end
          -- Items on the ground (8611:2d7c, drawn by 8611:1a79 before the
          -- stacks, at the tile's corner): a planted standard as its side's
          -- flag, army cell 29 of the side's sheet; anything else as the bag,
          -- ATRANS2.PCK's (64, 0) 32x29.
          local here = G.itemsOnTile and G.itemsOnTile[mx + my * 1000]
          if here then
            if here.planted and here.standardOf then
              local side = here.standardOf
              love.graphics.draw(G.armyImg[side], G.armyQuads[side][29], sx, sy)
            else
              love.graphics.draw(G.atransShields, G.bagQuad, sx, sy)
            end
          end

          local stack = game.armiesAt(G.g, mx, my)
          -- a stack still walking is drawn where the walk has got to, not
          -- where it has already arrived
          if G.walk then
            local left = {}
            for _, a in ipairs(stack) do
              if not G.walk.armies[a] then left[#left + 1] = a end
            end
            stack = left
          end
          if #stack > 0 then
            local a = topArmy(stack)
            drawStack(a.owner or 8, stackFigure(stack), #stack, sx, sy)
          end
        end
      end
    end
  end
  drawFog()
  drawRoute()

  -- the stack itself, wherever the walk has got to
  local at = walkingAt()
  if at then
    local col, row = at.x - G.cx, at.y - G.cy
    local a = G.walk.top
    if a and col >= 0 and col < screen.VIEW_COLS and row >= 0 and row < screen.VIEW_ROWS then
      local n, walking = 0, {}
      for w in pairs(G.walk.armies) do n = n + 1; walking[#walking + 1] = w end
      drawStack(a.owner or 8, stackFigure(walking), n, r.x + col * TILE, r.y + row * TILE)
    end
  end

  -- the selection box, which follows the walk
  if G.selection then
    local col, row = (at and at.x or G.selection.x) - G.cx,
                     (at and at.y or G.selection.y) - G.cy
    if col >= 0 and col < screen.VIEW_COLS and row >= 0 and row < screen.VIEW_ROWS then
      love.graphics.setColor(1, 1, 1)
      -- 828e:0afd / 0b27: the small box for one army, the large for a group
      -- (4125:2bda); 177b:0161 steps it every fourth BIOS tick
      local base = #G.selection.stack > 1 and 4 or 0
      local frame = base + math.floor(now() * CURS_RATE) % 4
      love.graphics.draw(G.cursImg, G.cursQuads[frame], r.x + col * TILE, r.y + row * TILE)
    end
  end
  love.graphics.setScissor()
end

--- The strategic map as the original paints it anywhere it appears -- the
--- main screen, the city dialog, the hero offer: STRAT.PCK's rendering of the
--- terrain (screen.strategicImage), then every city it can see as an 8x8
--- shield from ATRANS2.PCK, the owner's at (side * 16, 30), at the city's
--- (2x - 1, 2y - 1) (834b:0ed7). The sheet also carries each shield shifted a
--- pixel at a time, for a planar blit that can only start on a byte; drawing
--- the unshifted one where it belongs comes to the same thing. A razed city
--- has no shield. `mark` gets a white box round its shield, one pixel clear.
--- `owners`, if given, is who held each city (by position in the list, 0xff
--- for ruins) -- History draws the shields as they were (834b:12d3).
function G.drawStrategicMap(x, y, mark, noCities, owners)
  if not G.stratImage then
    G.stratImage = screen.strategicImage(G.screen, G.g, G.player, game.seen)
  end
  love.graphics.setScissor(x, y, 224, 312)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.stratImage, x, y)
  local w, h = G.atransShields:getDimensions()
  for i, c in ipairs(noCities and {} or G.g.map.cities) do
    local owner = owners and owners[i]
    local gone = owners and owner == 0xff or (not owners and c.razed)
    if not gone and game.seen(G.g, G.player, c.x, c.y) then
      local side = owners and owner or c.ownerIndex or 8
      love.graphics.draw(G.atransShields, love.graphics.newQuad(side * 16, 30, 8, 8, w, h),
                         x + c.x * 2 - 1, y + c.y * 2 - 1)
    end
  end
  if mark then
    local mx, my = x + mark.x * 2 - 1, y + mark.y * 2 - 1
    local c = G.palette[16]
    love.graphics.setColor(c[1], c[2], c[3])
    love.graphics.rectangle("fill", mx - 1, my - 1, 10, 1)
    love.graphics.rectangle("fill", mx - 1, my + 9, 10, 1)
    love.graphics.rectangle("fill", mx - 1, my - 1, 1, 10)
    love.graphics.rectangle("fill", mx + 9, my - 1, 1, 10)
  end
  love.graphics.setScissor()
end

--- Every site shown to the side, marked on a strategic map drawn at (x, y)
--- (834b:05e4): ATRANS2.PCK's 16x10 -- (112, 20) a temple, (112, 10) a site
--- searched, (128, 0) a rich one, (112, 0) any other -- at 2y - 1 and at
--- 2x - 1 rounded to the nearest multiple of 8, the blit's byte grid. With a
--- hidden map only on a tile the side has seen. The site `highlight` gets a
--- white 12x10 box.
function G.drawSiteMarkers(x, y, highlight)
  local site = require("warlords.site")
  local w, h = G.atransShields:getDimensions()
  love.graphics.setScissor(x, y, 224, 312)
  for _, s in ipairs(G.g.map.sites) do
    local shown = math.floor((s.revealed or 0) / 2 ^ G.player.index) % 2 == 1
    if shown and game.seen(G.g, G.player, s.x, s.y) then
      local src
      if s.content == site.TEMPLE then src = { 112, 20 }
      elseif s.searched then src = { 112, 10 }
      elseif s.rich then src = { 128, 0 }
      else src = { 112, 0 } end
      local mx = math.floor((s.x * 2 - 1 + 4) / 8) * 8
      local my = s.y * 2 - 1
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.atransShields, love.graphics.newQuad(src[1], src[2], 16, 10, w, h),
                         x + mx, y + my)
      if s == highlight then
        kit.setPal(15)
        kit.outline(x + mx, y + my, 12, 10)
      end
    end
  end
  love.graphics.setScissor()
end

local function drawStrategic()
  local r = G.stratRect
  G.drawStrategicMap(r.x, r.y)
  love.graphics.setScissor(r.x, r.y, r.w, r.h)
  -- The view box (8961:0698): the 9x9 viewport at 2 pixels a tile, as a
  -- white square 18 across with sides two pixels thick.
  local c = G.palette[16]
  love.graphics.setColor(c[1], c[2], c[3])
  local bx, by, n = r.x + G.cx * 2, r.y + G.cy * 2, screen.VIEW_COLS * 2
  love.graphics.rectangle("fill", bx, by, n, 2)
  love.graphics.rectangle("fill", bx, by + n - 2, n, 2)
  love.graphics.rectangle("fill", bx, by, 2, n)
  love.graphics.rectangle("fill", bx + n - 2, by, 2, n)
  love.graphics.setScissor()
end

--- The fog only ever opens, so the strategic map is rebuilt on demand.
function stratDirty()
  G.stratImage = nil
end
G.stratDirty = function() stratDirty() end
G.openQuest = function() questUi.open() end

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

-- The menu bar (7ae8:02d8, 2372:132f): white, the titles in TEXT with a black
-- glyph (7ae8:029c: font 0 in colours 0 on 15), each at its rect's x + 2.
-- A title whose menu is open, and the row under the pointer in a dropdown,
-- are XORed with colour 7 (2372:102d through 2012:044f) -- white goes to 8,
-- orange, and black to 7, yellow. A greyed item's glyph is colour 3.
--
-- The dropdown (2372:0803) is white with a black outline down its left and
-- along its bottom and right, and a second line a pixel further out below and
-- to the right for its shadow; there is no line along its top, which sits a
-- pixel under the bar. Its labels are 3 in, a separator is a black line.
local function palColour(i)
  local c = G.palette[i + 1]
  love.graphics.setColor(c[1], c[2], c[3])
end

-- A stack on the map (8611:0335): the top army's figure, 32 x 29, at (8, 7)
-- in the tile; the flag pole, three 40-pixel lines down x + 2, 3 and 4 in
-- colours 14, 13 and 14; and the flag, from the side's own army sheet at
-- (464, y) 48 x 8, bigger the more armies stand there -- y = 29, 38, 47, 56
-- for one to four (4125:2d4e). More than four fly the smallest 8 lower down
-- as well, and the flag for the rest at the top.
do
local FLAG_Y = { 29, 38, 47, 56 }
local flagQuads = {}
function drawStack(owner, armyType, count, sx, sy)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.armyImg[owner], G.armyQuads[owner][armyType % 32], sx + 8, sy + 7)
  palColour(14)
  love.graphics.rectangle("fill", sx + 2, sy, 1, TILE)
  love.graphics.rectangle("fill", sx + 4, sy, 1, TILE)
  palColour(13)
  love.graphics.rectangle("fill", sx + 3, sy, 1, TILE)
  love.graphics.setColor(1, 1, 1)
  local function flag(k, y)
    if not flagQuads[k] then
      flagQuads[k] = love.graphics.newQuad(464, FLAG_Y[k], 48, 8, G.armyImg[owner]:getDimensions())
    end
    love.graphics.draw(G.armyImg[owner], flagQuads[k], sx, y)
  end
  if count > 4 then flag(1, sy + 8) count = count - 4 end
  flag(math.max(1, math.min(count, 4)), sy)
end
end

-- Turn and the sides still in it, at the right of the bar (8cc6:0952): the
-- strip (437, 0) 202x17 is filled white, "Turn %d" is right-aligned in font 2,
-- black, and a 16x14 shield from ATRANS2.PCK stands for each side in play,
-- 16 apart and ending at x = 624; the side whose turn it is sits on a black
-- box.
local function drawTurnStrip()
  palColour(15)
  love.graphics.rectangle("fill", 437, 0, 202, 17)
  local alive = {}
  for _, side in ipairs(G.g.sides) do
    if side.alive then alive[#alive + 1] = side end
  end
  local n = #alive
  local turn = ("Turn %d"):format(G.g.turn)
  local f = G.bigFont.colours(0, 15)
  f.draw(turn, (8 - n) * 16 + 508 - f.width(turn), 0)
  for k, side in ipairs(alive) do
    local x = (8 - n + k - 1) * 16 + 512
    if side == G.player then
      palColour(0)
      love.graphics.rectangle("fill", x - 3, 1, 17, 15)
    end
    local s = side.index
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.atransShields,
      love.graphics.newQuad(112 + math.floor(s / 4) * 16, 94 + (s % 4) * 14, 16, 14,
                            G.atransShields:getDimensions()), x, 2)
  end
end

local function drawMenuBar()
  palColour(15)
  love.graphics.rectangle("fill", 0, 0, 640, menuMod.BAR_H)
  for i, m in ipairs(G.menuLayout) do
    local lit = (i == G.openMenu)
    if lit then
      palColour(8)
      love.graphics.rectangle("fill", m.x, 0, m.w, menuMod.BAR_H)
    end
    love.graphics.setColor(1, 1, 1)
    G.font.colours(lit and 7 or 0, lit and 8 or 15).draw(m.title, m.x + 2, menuMod.BAR_Y)
  end
  if not G.starting then drawTurnStrip() end

  local open = G.menuLayout[G.openMenu]
  if not open then return end
  local d = open.drop
  palColour(15)
  love.graphics.rectangle("fill", d.x, d.y, d.w - 1, d.h - 1)
  palColour(0)
  love.graphics.rectangle("fill", d.x, d.y + d.h - 2, d.w - 2, 1)
  love.graphics.rectangle("fill", d.x + 2, d.y + d.h - 1, d.w - 2, 1)
  love.graphics.rectangle("fill", d.x, d.y, 1, d.h - 2)
  love.graphics.rectangle("fill", d.x + d.w - 2, d.y, 1, d.h - 1)
  love.graphics.rectangle("fill", d.x + d.w - 1, d.y + 1, 1, d.h - 1)

  local mx, my = -1, -1
  if love.mouse and love.mouse.getPosition then mx, my = love.mouse.getPosition() end
  local hover = menuMod.rowAt(G.menuLayout, G.openMenu, mx, my)
  for _, r in ipairs(d.rows) do
    if r.label == "-" then
      palColour(0)
      love.graphics.rectangle("fill", r.x + 1, r.y - 1, d.w - 2, 1)
    else
      local grey = not (G.menuEnabled and G.menuEnabled(r.key or r.act))
      local lit = (r == hover) and not grey
      if lit then
        palColour(8)
        love.graphics.rectangle("fill", r.x + 1, r.y, d.w - 3, r.h)
      end
      local glyph = grey and 3 or (lit and 7 or 0)
      local f = G.font.colours(glyph, lit and 8 or 15)
      love.graphics.setColor(1, 1, 1)   -- the separators' black would tint it
      f.draw(r.label, r.x + 3, r.y)
      if r.key then f.draw(r.key, r.x + d.keyCol, r.y) end
    end
  end
end

-- The bottom bar's eight army slots and the mark under each are real
-- controls, so their hit rects come from BUTTON.DAT: ids 224-231 are the
-- slots and 232-239 the marks, and 240/241 the Grp button (one rect, two
-- controls -- the game shows whichever applies). They carry no art of their
-- own (bitmap 0), which is the layout's way of saying the game draws them.
--
-- What it draws is all in ABITS.PCK and the army sheets, at the rects
-- 89e0:0356 and 8611:08be use. Those are the original's own numbers, read
-- out of its data segment; docs/re/ui.md > The army slots has the table.
local SLOT_FIRST, BAR_FIRST, SLOT_COUNT = 224, 232, 8
local GRP_ALL, GRP_NONE = 240, 241

local SLOT_X, SLOT_Y, SLOT_STEP = 24, 405, 40   -- ring and army
local SLOT_MOVES_X, SLOT_MOVES_Y = 8, 31        -- the moves left, within it
local MARK_Y = 449                              -- the tick or cross
local MARK_W, MARK_H = 32, 16
local MARK_SRC = { [slotsMod.CROSS] = { 448, 0 }, [slotsMod.TICK] = { 448, 16 } }
local DIGIT_SRC, DIGIT_W = { 64, 30 }, 8        -- "0123456789" in ABITS
local GROUP_SRC, MOVE_SRC = { 0, 30, 32, 8 }, { 32, 30, 32, 8 }
local GROUP_AT, MOVE_AT   = { 344, 420 }, { 344, 428 }
local GROUP_MOVES_AT      = { 352, 436 }
local GRP_SRC = { red = { 288, 0 }, green = { 288, 19 } }
-- how the moving group travels (89e0:070d): one 32 x 10 icon at (344, 407)
local MOVE_ICON_AT, MOVE_ICON_W, MOVE_ICON_H = { 344, 407 }, 32, 10
local MOVE_ICON = {
  fly = { 184, 30 }, sea = { 424, 30 },
  both = { 216, 30 }, woods = { 248, 30 }, hills = { 152, 30 },
}
local GRP_AT, GRP_W, GRP_H = { 344, 447 }, 32, 19

local abitsQuad             -- (x, y, w, h) -> a cached quad into ABITS.PCK
do
  local cache = {}
  function abitsQuad(x, y, w, h)
    local key = ("%d,%d,%d,%d"):format(x, y, w, h)
    if not cache[key] then
      cache[key] = love.graphics.newQuad(x, y, w, h, 480, 40)
    end
    return cache[key]
  end
end

local function drawAbits(src, w, h, x, y)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.abits, abitsQuad(src[1], src[2], w, h), x, y)
end

--- A number in ABITS's own 8x8 digits, which is how the bar writes every
--- number it shows. Always two digits: the format is "%02d" throughout.
local function drawDigits(n, x, y)
  local text = ("%02d"):format(math.max(0, math.min(99, n)))
  for i = 1, #text do
    local d = text:byte(i) - 48
    drawAbits({ DIGIT_SRC[1] + d * DIGIT_W, DIGIT_SRC[2] }, DIGIT_W, DIGIT_W,
              x + (i - 1) * DIGIT_W, y)
  end
end

--- The ring a slot's army sits in. Ring 0 is grey and 1-8 are the sides'
--- colours, and a slot takes the colour `group` steps round from the current
--- player's -- so each group in the stack gets a colour of its own, rather
--- than the ring saying who owns the army.
local function groupRing(group)
  if not group then return 0 end
  return (G.player.index + group) % 8 + 1
end

local function drawArmySlots()
  local s = G.selection and G.selection.slots
  for i = 0, SLOT_COUNT - 1 do
    local x = SLOT_X + i * SLOT_STEP
    local a = s and s.army[i + 1]
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.abits, G.ringQuads[a and groupRing(s.group[i + 1]) or 0],
                       x, SLOT_Y)
    if a then
      -- the armies that are not moving with the group are drawn as ghosts
      local img = s.inGroup[i + 1] and G.armyImg[G.player.index] or G.shadowImg
      love.graphics.draw(img, G.armyQuads[G.player.index][a.type % 32], x, SLOT_Y)
      drawDigits(a.moves or 0, x + SLOT_MOVES_X, SLOT_Y + SLOT_MOVES_Y)
      local mark = MARK_SRC[s.mark[i + 1]]
      if mark then drawAbits(mark, MARK_W, MARK_H, x, MARK_Y) end
    end
  end

  -- "Group Move", the group's own movement, and the Grp button: green once
  -- the whole stack moves as one, red while it does not (89e0:0567).
  drawAbits(GROUP_SRC, GROUP_SRC[3], GROUP_SRC[4], GROUP_AT[1], GROUP_AT[2])
  drawAbits(MOVE_SRC, MOVE_SRC[3], MOVE_SRC[4], MOVE_AT[1], MOVE_AT[2])
  drawDigits(s and slotsMod.moves(s) or 0, GROUP_MOVES_AT[1], GROUP_MOVES_AT[2])
  drawAbits(s and slotsMod.grouped(s) and GRP_SRC.green or GRP_SRC.red,
            GRP_W, GRP_H, GRP_AT[1], GRP_AT[2])

  -- over it, the way the moving group travels (89e0:070d): a wing when it
  -- flies (1c8c:07fa, the stack's own rule), a boat when every one of it is
  -- at sea, else its woods and hills bonuses -- or nothing, colour 3
  local moving = {}
  if s then
    for i, a in ipairs(s.army) do
      if s.inGroup[i] then moving[#moving + 1] = a end
    end
  end
  local icon
  if #moving > 0 then
    local sea, woods, hills = true, false, false
    for _, a in ipairs(moving) do
      local t = G.g.types.byId[a.type]
      if t.woodsMove then woods = true end
      if t.hillsMove then hills = true end
      if not a.atSea then sea = false end
    end
    if move.stackMode(G.g, moving) == move.FLYING then icon = MOVE_ICON.fly
    elseif sea then icon = MOVE_ICON.sea
    elseif woods and hills then icon = MOVE_ICON.both
    elseif woods then icon = MOVE_ICON.woods
    elseif hills then icon = MOVE_ICON.hills end
  end
  if icon then
    drawAbits(icon, MOVE_ICON_W, MOVE_ICON_H, MOVE_ICON_AT[1], MOVE_ICON_AT[2])
  else
    palColour(3)
    love.graphics.rectangle("fill", MOVE_ICON_AT[1], MOVE_ICON_AT[2], MOVE_ICON_W, MOVE_ICON_H)
  end
end

-- With nothing selected the bar shows the side's standing instead: cities,
-- treasury, income and upkeep. 89e0:05a3 draws it as four 40 x 20 cells of
-- ABITS.PCK -- the right-hand end of the sheet, past the rings -- each with a
-- number beside it. The rects and both points are the original's own, read
-- out of its data segment at 4125:3072, :3092 and :30a2, and the formats are
-- the "%d" and three "%dgp" that follow them at :3128.
local STATUS_W, STATUS_H = 40, 20
local STATUS = {
  { sx = 344, sy =  0, x =  32, tx =  72, fmt = "%d",
    value = function() return #game.sideCities(G.g, G.player) end },
  { sx = 344, sy = 20, x = 120, tx = 144, fmt = "%dgp",
    value = function() return G.player.gold end },
  { sx = 384, sy =  0, x = 200, tx = 232, fmt = "%dgp",
    value = function() return game.income(G.g, G.player) end },
  { sx = 384, sy = 20, x = 280, tx = 320, fmt = "%dgp",
    value = function() return game.upkeep(G.g, G.player) end },
}
local STATUS_Y = 425                           -- icons and text share a row

local function drawStatus()
  for _, st in ipairs(STATUS) do
    drawAbits({ st.sx, st.sy }, STATUS_W, STATUS_H, st.x, STATUS_Y)
    G.bigFont.draw(st.fmt:format(st.value()), st.tx, STATUS_Y)   -- font 2 (89e0:05a3)
  end
end

-- The bar is not the screen's own art: 8065:0aeb blits MARBLE.PCK over it,
-- the rect at 4125:2a9c -- (16, 403) 360x66 -- from the marble's (0, 60),
-- before either face is drawn. Checked against the original running.
local BAR_GROUND = { x = 16, y = 403, w = 360, h = 66, sx = 0, sy = 60 }

local function drawBottomBar()
  love.graphics.setColor(1, 1, 1)
  local b = BAR_GROUND
  love.graphics.draw(G.marble,
    love.graphics.newQuad(b.sx, b.sy, b.w, b.h, G.marble:getDimensions()), b.x, b.y)
  if G.selection then
    drawArmySlots()
  else
    drawStatus()
  end
end

-- Everything draws into a 640x480 frame with no transform (the scaling up to
-- the window happens once, at the end of main.lua). That matters beyond
-- tidiness -- love.graphics.setScissor takes target pixels and ignores any
-- transform, so scaling here would clip the map and the strategic map to the
-- wrong place.
-- The city dialog's Vector mode paints the map its own way (834b:08df, and
-- 834b:1817 for See All): no owner shields, but a marker on each of the
-- side's cities from ATRANS2.PCK's row at y = 94, 16x10, and lines for the
-- vectors. The markers, by the table at 4125:2c76:
--
--   0 white, filled   a city building something      3 white, empty   idle
--   4 black, filled   the dialog's city, building    6 black, empty   idle
--   1 yellow, filled  a destination, building        5 yellow, empty  idle
--   2 orange          a city vectoring here
--
-- A marker sits at (2x - 2, 2y - 1); a line runs between (2x + 2, 2y + 2) of
-- its two cities, colour 7 to where a city sends its armies and colour 8 from
-- a city sending them here.
local VMARK_X = { [0] = 0, 32, 48, 16, 64, 80, 96 }

local function linePixels(x0, y0, x1, y1)
  -- 2133:00b8 draws a plain Bresenham line, both ends included
  local dx, dy = math.abs(x1 - x0), -math.abs(y1 - y0)
  local sx, sy = x0 < x1 and 1 or -1, y0 < y1 and 1 or -1
  local err = dx + dy
  while true do
    love.graphics.rectangle("fill", x0, y0, 1, 1)
    if x0 == x1 and y0 == y1 then break end
    local e2 = 2 * err
    if e2 >= dy then err = err + dy; x0 = x0 + sx end
    if e2 <= dx then err = err + dx; y0 = y0 + sy end
  end
end

--- Draw the map as the city dialog's Vector mode shows it. `filter` hides the
--- cities that could not take more vectors: -1 when choosing where `city`
--- sends its armies, n when moving the n cities vectoring to it, nil for all.
function G.drawVectorMap(x, y, city, filter, seeAll)
  if not G.stratImage then
    G.stratImage = screen.strategicImage(G.screen, G.g, G.player, game.seen)
  end
  love.graphics.setScissor(x, y, 224, 312)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.stratImage, x, y)
  local iw, ih = G.atransShields:getDimensions()
  local function marker(c, k)
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.atransShields, love.graphics.newQuad(VMARK_X[k], 94, 16, 10, iw, ih),
                       x + c.x * 2 - 2, y + c.y * 2 - 1)
  end
  local function line(a, b, colour)
    local c = G.palette[colour + 1]
    love.graphics.setColor(c[1], c[2], c[3])
    linePixels(x + a.x * 2 + 2, y + a.y * 2 + 2, x + b.x * 2 + 2, y + b.y * 2 + 2)
  end
  local mine = game.sideCities(G.g, G.player)
  local function building(c) return c.producing and 0 or 3 end
  -- The side's planted standard: its flag (ATRANS2.PCK's (96, 15), 16x15,
  -- 834b:1f5f with figure 0), and a line from each city sending armies there
  -- to the standard's (2x, 2y).
  local sx, sy = game.standardAt(G.g, G.player)
  local function toStandard(c, colour)
    local k = G.palette[colour + 1]
    love.graphics.setColor(k[1], k[2], k[3])
    linePixels(x + c.x * 2 + 2, y + c.y * 2 + 2, x + sx * 2, y + sy * 2)
  end

  if seeAll then
    local done = {}
    for _, c in ipairs(mine) do
      if not done[c] then
        done[c] = true
        local incoming = game.vectoredTo(G.g, c)
        local k = building(c)
        if c.vectorTo then k = 2 end
        if #incoming > 0 then k = c.producing and 1 or 5 end
        marker(c, k)
        local dest = c.vectorTo and c.vectorTo >= 0 and G.g.map.cities[c.vectorTo + 1]
        if dest then
          done[dest] = true
          marker(dest, dest.producing and 1 or 5)
          line(dest, c, 7)
        elseif c.vectorTo == game.STANDARD and sx then
          toStandard(c, 8)
        end
        for _, src in ipairs(incoming) do
          done[src] = true
          marker(src, 2)
          line(src, c, 8)
        end
      end
    end
    local c = G.palette[16]
    love.graphics.setColor(c[1], c[2], c[3])
    local bx, by = x + (city and city.x or -10) * 2 - 3, y + (city and city.y or -10) * 2 - 2
    love.graphics.rectangle("fill", bx, by, 11, 1)
    love.graphics.rectangle("fill", bx, by + 11, 11, 1)
    love.graphics.rectangle("fill", bx, by, 1, 11)
    love.graphics.rectangle("fill", bx + 11, by, 1, 11)
  else
    for _, c in ipairs(mine) do
      local n = #game.vectoredTo(G.g, c)
      local full = (filter == -1 and n + 1 > game.MAX_VECTORED_TO)
                   or (filter and filter >= 0 and n + filter > game.MAX_VECTORED_TO)
      if c == city or not full then
        local k = building(c)
        if c == city then k = c.producing and 4 or 6 end
        marker(c, k)
      end
    end
    local dest = city.vectorTo and city.vectorTo >= 0 and G.g.map.cities[city.vectorTo + 1]
    if dest then
      marker(dest, dest.producing and 1 or 5)
      line(city, dest, 7)
    elseif city.vectorTo == game.STANDARD and sx then
      toStandard(city, 7)
    end
    for _, src in ipairs(game.vectoredTo(G.g, city)) do
      marker(src, 2)
      line(src, city, 8)
    end
  end
  if sx then
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.atransShields, love.graphics.newQuad(96, 15, 16, 15, iw, ih),
      x + math.max(0, math.floor((sx * 2 - 2) / 8) * 8), y + math.max(0, sy * 2 - 6))
  end
  love.graphics.setScissor()
end

--- A hero's figure on the strategic map (834b:1f5f): ATRANS2.PCK's (96, 0)
--- 16x15 at the hero's 2y - 6, and at 2x - 2 rounded down to a multiple of 8.
function G.drawHeroFigure(x, y, tx, ty)
  love.graphics.setScissor(x, y, 224, 312)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.atransShields, G.heroMark,
    x + math.max(0, math.floor((tx * 2 - 2) / 8) * 8), y + math.max(0, ty * 2 - 6))
  love.graphics.setScissor()
end

--- The strategic map in a dialog's left-hand panel -- the city dialog's --
--- with the city marked.
function G.drawStrategicPanel(x, y, city)
  G.drawStrategicMap(x, y, city)
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

  -- The popup's own frame goes round the outside -- a black outline a pixel
  -- out and a two-pixel shadow beyond it, as every popup has -- on top of the
  -- frame painted into the picture. Checked against the original running.
  kit.popupFrame(R)

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
  -- 7ecb:00d6 centres on x = 320 as 320 - width / 2, truncating: for an odd
  -- width that is a pixel right of centring the text in the rect
  local f = G.titleFont
  kit.centred(f, b.name, 320, BANNER_NAME_Y)
  kit.centred(f, ("Turn %d"):format(b.turn), 320, BANNER_TURN_Y)
end

--- The hero offer. Popup 2 has no bitmap of its own, so unlike the banner it
--- is not a picture with a frame painted into it: the marble is blitted and
--- the ordinary popup frame drawn round it -- which, checked against the
--- original running at its own 640x480, puts the outline one pixel outside
--- the rect on every side and the contents exactly at the rect.
local function drawHeroOffer()
  local R, b = HERO_POPUP, G.offer
  kit.popupFrame(R)

  -- MARBLE.PCK is 480x360, so popup 2's 480x312 is its top-left corner
  love.graphics.setColor(1, 1, 1)
  love.graphics.setScissor(R.x, R.y, R.w, R.h)
  love.graphics.draw(G.marble, R.x, R.y)

  -- the whole map at the strategic map's own 2 pixels a tile
  love.graphics.setScissor()
  G.drawStrategicMap(HERO_MAP.x, HERO_MAP.y)
  love.graphics.setScissor(R.x, R.y, R.w, R.h)
  love.graphics.setColor(1, 1, 1)
  -- Where the hero would appear (834b:1f5f): ATRANS2.PCK's figure at
  -- (96, 0) 16x15, masked on colour 1 like the shields, at the city's
  -- (2y - 6) and at 2x - 2 rounded down to a multiple of 8 -- the routine
  -- blits on a byte, and this one does not carry shifted copies.
  local fx = math.max(0, math.floor((b.city.x * 2 - 2) / 8) * 8)
  local fy = math.max(0, b.city.y * 2 - 6)
  love.graphics.draw(G.atransShields, G.heroMark, HERO_MAP.x + fx, HERO_MAP.y + fy)

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
    -- font 2 in 15 with a colour-14 outline, dark brown (78a8:06ae(2, 15, 14, 3))
    if line ~= "" then centred(G.bigFont.colours(15, 14), line, HERO_LINE_Y[i]) end
  end

  -- The name field: a black outline two pixels clear of it (the rect at
  -- 4125:1142), then the field itself as 7ecb:0058 draws every field --
  -- filled flat with colour 3 and sunk into the marble.
  local field = screen.dialogControl(G.heroView, HERO_FIELD)
  if field then
    love.graphics.setColor(0, 0, 0)
    kit.outline(field.x - 2, field.y - 2, field.w + 4, field.h + 4)
    kit.field(field.x, field.y, field.w, field.h, G.offerName, G.bigFont)
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
end

------------------------------------------------------------------ the assault

-- A city is not walked into, it is assaulted. `walk_path` stops the moment
-- the next step is a city the mover does not own and hands that tile to
-- `attack_tile` (67cc:0000), so a stack always fights from one of the eight
-- tiles around it -- diagonals included, the path stepping diagonally like
-- any other move.
--
-- What the player sees then, in order:
--
--   67cc:1836  the fire cloud from WAR.PCK over the tile
--   6a35:0160  the battle window: both lines drawn up, each beside its side's
--              shield from BSHIELD.PCK
--   6a35:0094  the fight played back, one army struck off at a time in the
--              order combat_resolve logged; space runs it through
--   6a35:04c5  how it ended, written under the lines
--   63fa:0000  and, if a city fell, what is to be done with it
--
-- The fight is decided before any of it is drawn -- combat_resolve runs first
-- and the window only replays its log -- so nothing here can change the
-- outcome. docs/re/ui.md > The assault.

-- Every number here is the original's own, read out of its data segment:
-- popup 8 and the rect at 4125:0cfa, the row heights at 4125:0d14 and the two
-- rows of places at 4125:0d1e and :0d2e.
local AS = {
  window   = { x = 160, y = 60, w = 320, h = 312 },   -- popup 8
  cloudW   = 128, cloudH = 120,                       -- the rect at 4125:0cfa
  warW     = 240, warH = 180,
  shieldW  = 32, shieldH = 36, shieldSheet = 288, shieldSheetH = 59,
  shieldDef = { 176, 86 }, shieldAtk = { 176, 246 },
  rows     = { 86, 116, 146, 176 },                   -- the defender's rows
  atkRow   = 246,                                     -- the attacker's
  xEven    = { 216, 248, 280, 312, 344, 376, 408, 440 },
  xOdd     = { 232, 264, 296, 328, 360, 392, 424 },
  sea      = { 0, 162, 32, 18 }, seaDrop = 10,        -- WAR.PCK's water
  textY    = 298, textStep = 20,                      -- 4125:4366, stepped 20
  cloudTime = 0.7,                                    -- before the window
  -- One casualty (6a35:0094): the blast goes on, then 7ecb:0000 waits 5, 3
  -- and 5 BIOS ticks; once space is pressed, 2 and 3. 18.2 ticks a second.
  fellTime = 13 / 18.2, fellFast = 5 / 18.2,
  -- STRING.DAT groups: how a fight ends, and what the spoils dialog says
  fled = 141, wonCityHero = 142, wonCity = 143,
  wonHero = 144, won = 145, lost = 146, loot = 147,
  vText = 65, vWho = 66, vWhere = 67,
  -- dialog 11 behind popup 7, which is exactly VICTORY.PCK
  vPopup = { x = 160, y = 90, w = 320, h = 200 },
  vDialog = 11,
  occupy = 285, pillage = 283, sack = 286, raze = 284,
  vTitleY = 93, vLineY = { 140, 160, 180, 200 },
}

--- The plain popup look 54f6:0000 gives a window that carries no side colour:
--- a black outline, and a two-pixel shadow down and to the right.
local function popupFrame(R)
  love.graphics.setColor(0, 0, 0)
  love.graphics.rectangle("line", R.x - 0.5, R.y - 0.5, R.w + 1, R.h + 1)
  love.graphics.rectangle("fill", R.x + 1, R.y + R.h + 1, R.w + 2, 2)
  love.graphics.rectangle("fill", R.x + R.w + 1, R.y + 1, 2, R.h + 2)
end

--- Where slot k of a line of n sits across the window: a full row takes all
--- eight places, a shorter one is centred, and an odd one takes the places
--- between them so that it centres on the same middle (6a35:0160).
local function slotX(n, k)
  if n >= 8 then return AS.xEven[k] end
  local from = 4 - math.floor((n + 1) / 2)
  local t = (n % 2 == 1) and AS.xOdd or AS.xEven
  return t[from + k]
end

--- A whole line laid out: rows of eight from the top, the last row centred.
local function lineSlots(n, rows)
  local out, i, row = {}, 1, 1
  while n - i + 1 >= 8 and rows[row + 1] do
    for k = 1, 8 do out[i + k - 1] = { x = AS.xEven[k], y = rows[row] } end
    i, row = i + 8, row + 1
  end
  local y = rows[row] or rows[#rows]
  for k = 1, n - i + 1 do out[i + k - 1] = { x = slotX(n - i + 1, k), y = y } end
  return out
end

--- The name the spoils dialog calls the victor by: the hero who led the
--- assault if one did, otherwise the best army in the line (attack_tile
--- reads 451b:1efe, the selection's hero, and falls back to the army).
local function victorName(line)
  for _, a in ipairs(line) do
    if a.type == armytype.HERO and a.name then return a.name end
  end
  local best = line[1]
  local rec = best and G.g.types.byId[best.type]
  return rec and rec.name or "Your armies"
end

--- How the fight ended, in the original's own words.
local function outcomeLines(result, city, fled)
  local ui = G.screen.ui
  local out = {}
  if not result.won then
    out[#out + 1] = uidata.text(ui, AS.lost, 0)
    return out
  end
  local hero = nil
  for _, a in ipairs(result.lines.attackers) do
    if a.type == armytype.HERO and a.name then hero = a.name break end
  end
  if city then
    if fled then
      out[#out + 1] = uidata.text(ui, AS.fled, G.g.rng:dice(1, 4, -1))
    end
    if hero then
      out[#out + 1] = uidata.text(ui, AS.wonCityHero, 0):format(hero)
    else
      out[#out + 1] = uidata.text(ui, AS.wonCity, 0)
    end
  elseif hero then
    out[#out + 1] = uidata.text(ui, AS.wonHero, 0):format(hero)
    out[#out + 1] = uidata.text(ui, AS.wonHero, 1)
  else
    out[#out + 1] = uidata.text(ui, AS.won, 0)
  end
  if result.loot and result.loot > 0 then
    out[#out + 1] = uidata.text(ui, AS.loot, 0):format(result.loot)
  end
  return out
end

--- Begin the assault the player has just ordered. The fight is already
--- decided; this only sets up the showing of it.
function startAssault(x, y, result)
  local lines = result.lines or { attackers = {}, defenders = {} }
  local fled = #lines.defenders == 0
  G.assault = {
    x = x, y = y, result = result,
    def = lines.defenders, atk = lines.attackers,
    defSlots = lineSlots(#lines.defenders, AS.rows),
    atkSlots = lineSlots(#lines.attackers, { AS.atkRow }),
    defSide = lines.defenders[1] and lines.defenders[1].owner or 8,
    atkSide = lines.attackers[1] and lines.attackers[1].owner or 8,
    step = 0, defDown = 0, atkDown = 0,
    phase = "cloud", at = now(),
    -- on a human's turn 67cc:1836 plays WAR.8SN under the cloud and holds
    -- it there until the sample ends
    cloudTime = (G.g.side and not G.g.side.computer) and G.audio.effect("war") or 0,
    captured = result.captured,
    victor = victorName(lines.attackers),
    message = outcomeLines(result, lines.city, fled),
  }
end

--- Carry the playback on by the clock. The draw calls it, so the animation
--- needs no update of its own.
local function advanceAssault()
  local a = G.assault
  local t = now()
  if a.phase == "cloud" then
    if t - a.at >= math.max(AS.cloudTime, a.cloudTime) then a.phase, a.at = "battle", t end
    return
  end
  if a.phase ~= "battle" then return end
  -- each casualty's blast goes on at once and is then waited on, so the
  -- first shows the moment the window opens and the last is held too
  local each = a.fast and AS.fellFast or AS.fellTime
  local log = a.result.log or {}
  while a.step < #log and t >= a.at do
    a.step = a.step + 1
    if log[a.step] == 1 then a.atkDown = a.atkDown + 1
    else a.defDown = a.defDown + 1 end
    -- 7dda:0181: a blow for each, ARMY.8SN when an attacker falls and
    -- ARMY2.8SN a defender -- until Space hurries the fight on
    if not a.fast then G.audio.effect(log[a.step] == 1 and "army" or "army2") end
    a.at = a.at + each
  end
  if a.step >= #log and t >= a.at then a.phase = "over" end
end

--- A key or a click: run the playback through, and when it is through, close
--- the window and ask what is to be done with the city.
function pressAssault()
  local a = G.assault
  if not a then return end
  if a.phase ~= "over" then
    a.fast, a.at = true, math.min(a.at, now() + AS.fellFast)   -- hurry the rest
    advanceAssault()
    return
  end
  G.assault = nil
  if a.captured then presentVictory(a.captured, a.victor) end
end

local function drawCloud()
  local a, r = G.assault, G.mapRect
  local sx = r.x + (a.x - G.cx) * TILE + math.floor((TILE - AS.cloudW) / 2)
  local sy = r.y + (a.y - G.cy) * TILE + math.floor((TILE - AS.cloudH) / 2)
  love.graphics.setScissor(r.x, r.y, r.w, r.h)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.warPic,
    love.graphics.newQuad(0, 0, AS.cloudW, AS.cloudH, AS.warW, AS.warH), sx, sy)
  love.graphics.setScissor()
end

--- One line of armies. They are struck off from the front, which is the order
--- combat_setup drew them up in and the order they fall.
local function drawBattleLine(armies, slots, side, down)
  local sheet = G.armyImg[side] or G.armyImg[8]
  local quads = G.armyQuads[side] or G.armyQuads[8]
  for i, army in ipairs(armies) do
    local at = slots[i]
    if at then
      love.graphics.setColor(1, 1, 1)
      if army.atSea then
        love.graphics.draw(G.warPic,
          love.graphics.newQuad(AS.sea[1], AS.sea[2], AS.sea[3], AS.sea[4], AS.warW, AS.warH),
          at.x, at.y + AS.seaDrop)
      end
      love.graphics.draw(sheet, quads[army.type % 32], at.x, at.y)
      -- a fallen army keeps its place, under the blast that took it
      if i <= down then
        love.graphics.draw(G.atransShields, G.blastQuad, at.x, at.y)
      end
    end
  end
end

local function drawBattle()
  local a, R = G.assault, AS.window
  popupFrame(R)
  love.graphics.setColor(1, 1, 1)
  -- popup 8 is 320 x 312 of marble from its origin, not the whole sheet
  love.graphics.draw(G.marble, love.graphics.newQuad(0, 0, R.w, R.h, G.marble:getDimensions()),
                     R.x, R.y)

  local function shield(side, at)
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.shieldImg,
      love.graphics.newQuad((side or 8) * AS.shieldW, 0, AS.shieldW, AS.shieldH,
                            AS.shieldSheet, AS.shieldSheetH), at[1], at[2])
  end
  shield(a.defSide, AS.shieldDef)
  shield(a.atkSide, AS.shieldAtk)

  drawBattleLine(a.def, a.defSlots, a.defSide, a.defDown)
  drawBattleLine(a.atk, a.atkSlots, a.atkSide, a.atkDown)

  if a.phase == "over" then
    local f, y = G.bigFont, AS.textY
    love.graphics.setColor(1, 1, 1)
    for _, line in ipairs(a.message) do
      kit.centred(f, line, 320, y)                  -- 6a35:04c5, on x = 320
      y = y + AS.textStep
    end
  end
end

local function drawAssault()
  advanceAssault()
  if G.assault.phase == "cloud" then drawCloud() else drawBattle() end
end

---------------------------------------------------------- the spoils of a city

-- Dialog 11, behind popup 7 at (160, 90) -- exactly VICTORY.PCK -- and four
-- buttons cut from DBUTTON.PCK. Pillage needs a production type to strip and
-- sack needs two, so 63fa:0000 greys out whichever cannot be had.
function presentVictory(city, victor)
  local ui = G.screen.ui
  if not G.victoryView then G.victoryView = screen.dialog(G.screen, AS.vDialog) end
  G.victory = {
    city = city,
    -- both lines pick at random from their group, as get_string(g, -1) does
    who = uidata.text(ui, AS.vWho, G.g.rng:dice(1, 4, -1)):format(victor),
    where = uidata.text(ui, AS.vWhere, G.g.rng:dice(1, 3, -1)):format(city.name),
  }
  local v = G.victoryView
  v.state[AS.occupy] = uidata.NORMAL
  v.state[AS.raze] = uidata.NORMAL
  v.state[AS.pillage] = #city.slots >= 1 and uidata.NORMAL or uidata.DISABLED
  v.state[AS.sack] = #city.slots >= 2 and uidata.NORMAL or uidata.DISABLED
end

local function drawVictory()
  local R, v, ui = AS.vPopup, G.victory, G.screen.ui
  popupFrame(R)
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.victoryPic, R.x, R.y)

  local function centred(f, text, y) kit.centred(f, text, 320, y) end
  centred(G.titleFont, uidata.text(ui, AS.vText, 0), AS.vTitleY)
  centred(G.bigFont, v.who, AS.vLineY[1])
  centred(G.bigFont, v.where, AS.vLineY[2])
  centred(G.bigFont, uidata.text(ui, AS.vText, 1), AS.vLineY[3])
  centred(G.bigFont, uidata.text(ui, AS.vText, 2), AS.vLineY[4])

  screen.drawDialogControls(G.screen, G.victoryView)
end

--- What the player chose to do with the city they have just taken.
function takeCity(what)
  local v = G.victory
  if not v then return end
  local c, stack = v.city, G.selection and G.selection.stack or {}
  G.victory = nil
  if what == AS.pillage then
    local gold = game.pillage(G.g, G.player, c, stack)
    say("%s is pillaged: %d gold.", c.name, gold)
  elseif what == AS.sack then
    local gold = game.sack(G.g, G.player, c, stack)
    say("%s is sacked: %d gold.", c.name, gold)
  elseif what == AS.raze then
    game.raze(G.g, G.player, c, stack)
    say("%s is burned to the ground.", c.name)
    stratDirty()
  else
    say("%s is ours.", c.name)
  end
end

--- The samples queue behind one another and the advisor blinks by the clock;
--- both are carried on here. Nothing else in the game needs an update: the
--- walk and the battle run off the draw.
function love.update()
  G.audio.update()
  local top = kit.top()
  if top and top.update then top.update() end
end

function love.draw()
  stepComputer()
  if G.starting then
    for _, d in ipairs(G.modals) do d.draw() end
    drawMenuBar()
    drawPointer()
    return
  end
  advanceWalk()
  refreshControls()
  screen.drawBackground(G.screen)
  drawMap()
  drawStrategic()
  -- 8065:0a9d refills the control panel with marble before the controls go
  -- on, the rect at 4125:2a94 -- (400, 355) 224x114 -- from the marble's own
  -- origin, as 8065:0aeb does for the bottom bar
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.marble, love.graphics.newQuad(0, 0, 224, 114, G.marble:getDimensions()),
                     400, 355)
  screen.drawControls(G.screen)
  drawShortcutIcons()
  drawBottomBar()
  -- the dialogs, bottom of the stack first
  for _, d in ipairs(G.modals) do d.draw() end
  drawMenuBar()
  -- the turn opens with the banner over the offer, and is dismissed first
  if G.offer then drawHeroOffer() end
  if G.assault then drawAssault() end
  if G.victory then drawVictory() end
  if G.banner then drawBanner() end
  drawPointer()
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
  -- the computer's moves being shown: a key or a click runs the rest through
  if G.aiRun and not kit.top() then
    -- Shift and Alt are held to reach Settings, not pressed to skip
    if key and key:match("shift$") or key and key:match("alt$") then return end
    G.aiSkip = true
    if G.walk and G.walk.computer then G.walk = nil resumeComputer() end
    return
  end
  -- 7ecb:0142 blocks the turn routine until any input arrives: the banner
  -- eats the click that dismisses it rather than passing it on
  if G.banner then dismissBanner() return end

  -- The assault is modal while it plays: a click runs it through, and the
  -- click that ends it opens the question of what to do with the city.
  if G.assault then pressAssault() return end
  if G.victory then
    local c = screen.dialogControlAt(G.victoryView, x, y)
    if c and G.victoryView.state[c.id] ~= uidata.DISABLED then takeCity(c.id) end
    return
  end

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

  -- On the start screens the menu bar stays live over them (7f77:0200 sets
  -- what it offers there); anywhere else a dialog takes every click.
  local top = kit.top()
  if top and top.menuBar and (G.openMenu or menuMod.titleAt(G.menuLayout, x, y, menuMod.BAR_H)) then
    top = nil
  end
  if top then
    if top.mousepressed then top.mousepressed(x, y, button) end
    return
  end
  if G.over then return end

  -- the menu bar takes precedence over everything beneath it
  local hit = menuMod.titleAt(G.menuLayout, x, y, menuMod.BAR_H)
  if hit then
    G.openMenu = (G.openMenu == hit) and nil or hit
    return
  end
  if G.openMenu then
    local row = menuMod.rowAt(G.menuLayout, G.openMenu, x, y)
    G.openMenu = nil
    local key = row and (row.key or row.act)
    if key and G.menuEnabled(key) then menuPick(key) end
    return
  end

  local c = screen.controlAt(G.screen, x, y)
  if c then
    -- a disabled button does not light up and does not arm
    if G.screen.state[c.id] == uidata.DISABLED then return end
    G.pressed = c.id
    G.screen.state[c.id] = uidata.ACTIVE
    return
  end

  -- The hand drags the map (740d:00e4 -> 8065:0e04): it remembers the tile
  -- the view is centred on, and as the mouse moves the view follows it a
  -- whole tile per 40 pixels until the button comes up.
  if button == 1 and pointerKind(x, y) == PTR.HAND then
    G.drag = { cx = G.cx + math.floor(screen.VIEW_COLS / 2),
               cy = G.cy + math.floor(screen.VIEW_ROWS / 2), dx = 0, dy = 0 }
    return
  end

  local r = screen.regionAt(G.screen, x, y)
  if not r then return end

  if r.id == screen.REGION.MAP then
    local tx, ty = tileAtPoint(x, y)
    if not tx then return end
    -- A left click on a tile of our own picks that stack up (8c07:06eb);
    -- anywhere else it is an order to go there. The right button shows what
    -- is on the tile while it is held (740d:0037).
    if button == 2 then
      require("ui.tileinfo").open(tx, ty)
      return
    end
    -- the left button does what the pointer shows (740d:00ce)
    local k = pointerKind(x, y)
    if k == PTR.WALK or k == PTR.BOAT then
      moveSelection(tx, ty)                    -- 1c8c:01fd
    elseif k == PTR.ATTACK or k == PTR.PEACE then
      moveSelection(tx, ty, true)              -- attack_tile, 67cc:0000
    elseif k == PTR.CITY or k == PTR.SITE then
      local city = game.cityAt(G.g, tx, ty)    -- 7204:0000
      if city then openCity(city) end
    elseif k == PTR.SELECT or k == PTR.ALT then
      select(tx, ty)                           -- 1b62:0405
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

-- The army slots and the marks under them, ids 224-239: 89e0:0963 takes a
-- slot and 89e0:0910 a mark, each with the id minus its row's base. Clicking
-- an army adds it to the group that moves, or drops it out into a group of
-- its own; clicking a mark makes that whole group the one that moves, and
-- nothing else. 240/241 share a rect -- the Grp button -- and are the two
-- directions of the same switch: group the lot, or break it up again.
local function afterSlotChange()
  syncSelection()
  say("%d of %d, %d movement", #G.selection.stack, G.selection.slots.n,
      slotsMod.moves(G.selection.slots))
end
G.afterSlotChange = afterSlotChange

for i = 0, SLOT_COUNT - 1 do
  ACTION[SLOT_FIRST + i] = function()
    if not G.selection then return end
    slotsMod.toggle(G.selection.slots, G.g, i + 1)
    afterSlotChange()
  end
  ACTION[BAR_FIRST + i] = function()
    if not G.selection then return end
    slotsMod.pickGroup(G.selection.slots, G.g, i + 1)
    afterSlotChange()
  end
end

ACTION[GRP_ALL] = function()
  if not G.selection then return end
  local s = G.selection.slots
  if slotsMod.grouped(s) then slotsMod.single(s, G.g)
  else slotsMod.all(s, G.g) end
  afterSlotChange()
end
ACTION[GRP_NONE] = ACTION[GRP_ALL]

-- The five buttons above the pad, ids 173-178. 173 walks the selection on
-- along the route it already has (1c8c:01fd with the army's own move target);
-- the other four are the army cycle -- next, done for this turn, fortify, and
-- put it down -- reaching 8065:0ec9 / 0ed7 / 0ef4 / 0f26, which is the row
-- docs/re/ui.md > The army cycle sets out.
ACTION[173] = function()
  local sel = G.selection
  if not sel or #sel.stack == 0 then say("Nothing is selected.") return end
  local target = sel.stack[1].target
  if not target then say("They have nowhere to be.") return end
  moveSelection(target.x, target.y)
end
ACTION[174] = function() selectNext() end
ACTION[175] = function() quitArmy() end
ACTION[176] = function() fortify() end
ACTION[178] = function() deselect() end

-- The 3x3 pad, ids 320-327, all reach 8611:0723 with the id minus 320, and
-- that routine steps the *cursor* -- the tile the view is centred on -- by
-- one in x, y or both, then recentres (8611:0629). It moves the view, not
-- the stack: the digit keys do that. Laid out on screen the eight ids run
-- clockwise from north, which is what fixes the order.
local PAD_STEP = {
  [0] = { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 },
         { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 },
}
for i = 0, 7 do
  local step = PAD_STEP[i]
  ACTION[320 + i] = function()
    G.cx, G.cy = G.cx + step[1], G.cy + step[2]
    clampCamera()
  end
end

-- Ids 179-182 are the four **configurable** buttons, which is why their art in
-- BUTTON.PCK is blank and why they were the hardest to place. 545c:0072 reads
-- the menu item assigned to button n from UDB/UDB.CUR, turns it into a command
-- code and runs it through the same dispatcher a key press uses. The shipped
-- assignment is Search, Move All, Heroes, End Turn.
--- Order > Move All (1c8c:04c4): every stack that still has somewhere to be
--- walks on as far as it can. The original steps through the armies with a
--- cursor of its own and never looks at one twice, which is what keeps a
--- stack that has run out of movement from holding the loop up.
local function moveAll()
  local seen, moved = {}, 0
  while true do
    local lead
    for _, a in ipairs(G.g.armies) do
      if a.owner == G.player.index and a.target and not a.transit and not seen[a] then
        lead = a break
      end
    end
    if not lead then break end

    local stack = {}
    for _, a in ipairs(game.armiesAt(G.g, lead.x, lead.y)) do
      if a.owner == G.player.index and a.target and not a.transit
         and a.target.x == lead.target.x and a.target.y == lead.target.y then
        stack[#stack + 1] = a
        seen[a] = true
      end
    end
    local target = { x = lead.target.x, y = lead.target.y }
    local r = move.moveTo(G.g, stack, target.x, target.y)
    if r.steps and r.steps > 0 then
      moved = moved + 1
      for _, a in ipairs(stack) do
        if a.x == target.x and a.y == target.y then a.target = nil end
      end
      stratDirty()
    end
  end
  if moved == 0 then say("Nothing is under orders.")
  else say("%d stack%s moved on.", moved, moved == 1 and "" or "s") end
  if G.selection then
    reslot(selectableAt(G.selection.stack[1].x, G.selection.stack[1].y))
  end
  refreshRoute()
end

--- Hero > Search (6536:0000) with the selected stack. The search is decided
--- at once, so the selection is put back over whoever is still standing.
local function search()
  if not G.selection then return end
  local x, y = G.selection.x, G.selection.y
  searchUi.open(G.selection.stack)
  reslot(selectableAt(x, y))
  stratDirty()
end

-- The key of each menu item UDB.DAT lets a button carry, by its menu item
-- id: the button runs the same command the key does, and is greyed when
-- the menu item is (545c:00aa asks 2372:0f9f, the menu's own enable bit).
local SHORTCUT_KEY = {
  [507] = "m", [508] = "q", [510] = "a", [511] = "k", [512] = "g", [513] = "n",
  [514] = "w", [516] = "=", [517] = ",", [518] = "f", [521] = "z", [524] = "b",
  [525] = "c", [526] = "p", [527] = "v", [528] = ".", [529] = "s", [530] = "h",
  [531] = "e", [534] = "l", [535] = "alt E",
}

local function shortcutKey(n)
  return SHORTCUT_KEY[G.screen.ui.shortcuts[n] or -1]
end

for i = 0, uidata.SHORTCUT_COUNT - 1 do
  ACTION[uidata.SHORTCUT_FIRST + i] = function()
    local key = shortcutKey(i)
    if key and G.menuEnabled(key) then MENU_DOES[key]() end
  end
end

-- 186 and 187, the two wide buttons under the cycle's row, are Tab and
-- Backspace's twins. 186 (8065:0f3f) looks at where the stack is going, and
-- back: it centres on the stack's destination unless the view is already
-- there, and on the stack otherwise. 187 (8065:0fe9) forgets the
-- destination, so the route goes from the map.
-- the "?" (8065:104e): the mouse's help page, then the keys'
ACTION[188] = function()
  local help = require("ui.help")
  help.open("HELP\\HMOUSE.GFX", function()
    help.open("HELP\\HKEYS.GFX", nil, help.POPUP4)
  end, help.POPUP4)
end

ACTION[186] = function()
  local sel = G.selection
  if not sel or #sel.stack == 0 then return end
  local t = sel.stack[1].target
  local vx, vy = G.cx + 4, G.cy + 4
  if t and not (vx == math.max(4, math.min(107, t.x)) and vy == math.max(4, math.min(151, t.y)))
     and sel.x >= G.cx and sel.x < G.cx + screen.VIEW_COLS
     and sel.y >= G.cy and sel.y < G.cy + screen.VIEW_ROWS then
    centreOn(t.x, t.y)
  else
    centreOn(sel.x, sel.y)
  end
end
ACTION[187] = function()
  local sel = G.selection
  if not sel or not sel.stack[1] or not sel.stack[1].target then return end
  for _, a in ipairs(sel.stack) do a.target = nil end
  G.route = nil
end

-- The pad's centre, id 177, shares its handler (8065:0f02) with the Home key.
ACTION[177] = function()
  if G.selection then centreOn(G.selection.x, G.selection.y)
  else centreOn(G.player.capital.x, G.player.capital.y) end
end

--- Which buttons are live. 8065:0174 is the original's own refresh -- it runs
--- after every action and sets each control to 1 or 2, normal or greyed --
--- and these are its rules. A control this engine has no handler for is
--- greyed as well: it is not clickable, and saying so plainly beats letting
--- it press and do nothing.
function refreshControls()
  local st = G.screen.state
  local sel = G.selection
  -- Under the start-of-turn banner, and the hero offer that follows it, every
  -- control is still greyed from the turn before: the original's refresh
  -- does not run until the turn's opening is over.
  if G.banner or G.offer then
    for id in pairs(st) do st[id] = uidata.DISABLED end
    return
  end
  -- a live button held down keeps its pressed look until it is let go
  local function set(id, live)
    if st[id] == nil then return end
    if not (live and ACTION[id]) then st[id] = uidata.DISABLED
    elseif id == G.pressed then st[id] = uidata.ACTIVE
    else st[id] = uidata.NORMAL end
  end

  -- 173: there is a route left to walk on along (2ea6 > 2ea8)
  local walkOn = sel ~= nil and G.route ~= nil
                 and #G.route.path > (G.walk and G.walk.i or 0)
  set(173, walkOn)
  -- 174/175: is any army still in the cycle at all (8c07:09e7)
  local more = false
  for _, a in ipairs(G.g.armies) do
    if a.owner == G.player.index and not a.transit
       and not a.fortified and not a.done then more = true break end
  end
  set(174, more)
  set(175, more and sel ~= nil)
  set(176, sel ~= nil)                         -- fortify
  set(177, sel ~= nil)                         -- centre on the selection
  set(178, sel ~= nil)                         -- deselect
  set(186, sel ~= nil)
  set(187, sel ~= nil and sel.stack[1] ~= nil and sel.stack[1].target ~= nil)
  set(188, true)

  -- 224-231 and the marks 232-239: a slot below the stack's size is live,
  -- the rest are not (8065:0174)
  for i = 0, SLOT_COUNT - 1 do
    local live = sel ~= nil and i < sel.slots.n
    set(SLOT_FIRST + i, live)
    set(BAR_FIRST + i, live)
  end
  -- 240/241 share the Grp rect and are the two ways of the same switch
  local grouped = sel and slotsMod.grouped(sel.slots)
  set(240, sel ~= nil and not grouped)
  set(241, sel ~= nil and grouped)

  for i = 0, uidata.SHORTCUT_COUNT - 1 do
    local key = shortcutKey(i)
    set(uidata.SHORTCUT_FIRST + i, key ~= nil and G.menuEnabled(key))
  end
  for i = 0, 7 do set(320 + i, true) end       -- the pad is always live
end

--- The drag's own sums, as 8065:0e04 keeps them: the map goes the way the
--- mouse does, so the view's centre goes the other.
function love.mousemoved(x, y, dx, dy)
  local d = G.drag
  if not d then return end
  d.dx, d.dy = d.dx - dx, d.dy - dy
  local function tiles(v) return v < 0 and -math.floor(-v / TILE) or math.floor(v / TILE) end
  centreOn(d.cx + tiles(d.dx), d.cy + tiles(d.dy))
end

function love.mousereleased(x, y, button)
  if G.drag then
    G.drag = nil
    if not G.pressed then return end
  end
  local top = kit.top()
  if top and top.mousereleased then top.mousereleased(x, y, button) return end
  local id = G.pressed
  if not id then return end
  G.pressed = nil
  G.screen.state[id] = uidata.NORMAL

  local c = screen.controlAt(G.screen, x, y)
  if not c or c.id ~= id then return end          -- released off the button
  local act = ACTION[id]
  if act then act() end
  refreshControls()
end

--- The city nearest the cursor -- the tile the view is centred on -- that
--- the side has seen, and of its own for any mode but Info; then the city
--- dialog on it in that mode.
viewCity = function(mode)
  local cx, cy = G.cx + 4, G.cy + 4
  local best, bestD
  for _, c in ipairs(G.g.map.cities) do
    local ok = game.seen(G.g, G.player, c.x, c.y)
               and (mode == cityUi.INFO or c.ownerIndex == G.player.index)
    if ok then
      local d = math.max(math.abs(c.x - cx), math.abs(c.y - cy))
      if not bestD or d < bestD then best, bestD = c, d end
    end
  end
  if best then cityUi.open(best, mode) end
end

-- Each menu accelerator, as far as this engine can honour it. The names are
-- the original's (docs/re/ui.md > The menu); what is missing says so rather
-- than failing quietly.
MENU_DOES = {
  ["alt E"] = function() endTurn() end,
  ["m"] = function() moveAll() end,
  -- Game > Quit (7721:0000) and New game (7721:019d) ask first (groups 44
  -- and 45); a new game goes back to the start screens (7f77:0000).
  ["^Q"] = function()
    local lines = {}
    for i = 1, 4 do lines[i] = uidata.text(G.screen.ui, 0x2c, i) end
    require("ui.input").open({ title = uidata.text(G.screen.ui, 0x2c, 0), lines = lines,
                               confirm = true, ok = function()
      -- 7721:0072: "Farewell, Warlord! We -shall- meet again!"
      require("ui.advisor").say(require("warlords.cues").QUIT, function() love.event.quit() end)
    end })
  end,
  ["alt N"] = function()
    local lines = {}
    for i = 1, 4 do lines[i] = uidata.text(G.screen.ui, 0x2d, i) end
    require("ui.input").open({ title = uidata.text(G.screen.ui, 0x2d, 0), lines = lines,
      confirm = true, ok = function()
        G.selection, G.banner, G.offer = nil, nil, nil
        openStart()
      end })
  end,
  -- Game > Save game and Load game (7721:093b, 026b): ten slots
  ["alt S"] = function() require("ui.savegame").save() end,
  ["alt L"] = function()
    require("ui.savegame").load(function(g)
      -- from the start screens, a game read back puts them away
      if G.starting then
        for i = #G.modals, 1, -1 do G.modals[i] = nil end
        G.starting = false
      end
      takeLoaded(g)
    end)
  end,
  ["z"] = function() search() end,
  -- View > Cities, Build, Production and Vectoring open the city dialog in
  -- one of its modes on the city nearest the cursor -- any city for Cities,
  -- one of the side's own for the rest (17be:0064's inline cases, through
  -- 7204:0000 and 828e:04fa).
  ["c"] = function() viewCity(cityUi.INFO) end,
  ["b"] = function() viewCity(cityUi.CITY) end,
  ["p"] = function() viewCity(cityUi.PRODUCTION) end,
  ["v"] = function() viewCity(cityUi.VECTOR) end,
  -- Hero > Plant Flag (7563:09f7): the selected stack's hero plants the
  -- side's standard where it stands
  ["f"] = function()
    if not G.selection then return end
    for _, a in ipairs(G.selection.stack) do
      if a.type == armytype.HERO and game.plantFlag(G.g, G.player, a) then
        stratDirty()
        return
      end
    end
  end,
  -- Order > Disband (1b62:06bf): the selected armies, after asking -- the
  -- fourth line says whether a hero is among them
  ["q"] = function()
    if not G.selection or #G.selection.stack == 0 then return end
    local stack = G.selection.stack
    local heroes = false
    for _, a in ipairs(stack) do if a.type == armytype.HERO then heroes = true end end
    require("ui.input").open({
      title = "Disband", confirm = true,                       -- 4125:2bf8
      lines = { "Are you sure you", "want to disband this", "group?",
                heroes and "It contains heroes" or "" },
      ok = function()
        game.disband(G.g, G.player, stack)
        G.selection = nil
        stratDirty()
      end,
    })
  end,
  -- Order > Fight Order (6a89:0de1)
  ["i"] = function() require("ui.fightorder").open() end,
  -- Order > Resign (7721:150d)
  ["r"] = function() require("ui.resign").open() end,
  -- View > Ruins (17be's inline case: 7204:0000 in mode 4 at the cursor)
  ["."] = function()
    local ruin = require("ui.ruin")
    ruin.open(ruin.nearest(G.cx + 4, G.cy + 4))
  end,
  -- View > Stack (89e0:0c9c)
  ["s"] = function() require("ui.stack").open() end,
  -- View > Army Bonus (89e0:1e3b)
  ["o"] = function() require("ui.armybonus").open() end,
  -- View > Items (66d4:0c21)
  ["t"] = function() require("ui.items").open() end,
  -- Order > Signpost (540d:01a4)
  ["x"] = function()
    if G.selection then require("ui.signpost").open(G.selection.stack) end
  end,
  -- History > City, Events, Gold, Winners (6d51:0000(0-3))
  ["h"] = function() require("ui.history").open(0) end,
  ["e"] = function() require("ui.history").open(1) end,
  ["j"] = function() require("ui.history").open(2) end,
  ["y"] = function() require("ui.history").open(3) end,
  -- Game > Shortcuts (545c:0000)
  ["alt U"] = function() require("ui.shortcuts").open() end,
  -- Game > Settings (64d2:0000(1))
  ["alt X"] = function() require("ui.settings").open() end,
  -- SSG > About Warlords II, and "?" (7721:0084)
  ["?"] = function() require("ui.about").open() end,
  -- History > Triumphs (6d51:09eb)
  ["l"] = function() require("ui.history").triumphs() end,
  -- Report > Diplomacy (484e:0000)
  ["d"] = function() require("ui.diplomacy").open() end,
  -- Report > Quest (4976:0167)
  ["="] = function() questUi.open() end,
  -- Hero > Levels (7563:1652)
  ["u"] = function() levelsUi.open() end,
  -- Hero > Inspect (6c1b:0000)
  [","] = function() heroInfo.open() end,
  -- Report > Army, City, Gold, Production, Winning: 6ef3:0000(0-4)
  ["a"] = function() reportsUi.open(0) end,
  ["k"] = function() reportsUi.open(1) end,
  ["g"] = function() reportsUi.open(2) end,
  ["n"] = function() reportsUi.open(3) end,
  ["w"] = function() reportsUi.open(4) end,
}

-- Hero > Inspect is live while the side has a hero (8065:0519: an army of
-- type 0x1c that is its own).
local function sideHasHero()
  for _, a in ipairs(G.g.armies) do
    if a.owner == G.player.index and a.type == armytype.HERO then return true end
  end
  return false
end

-- Hero > Search is live (8065:0519) when the selected army stands on a site
-- not yet searched (tile flag 0x40), and is a hero -- or the site is a
-- temple, which any army may visit.
local function canSearch()
  local lead = G.selection and G.selection.stack[1]
  if not lead then return false end
  local m = G.g.map
  local s = m.siteAt and m.siteAt[lead.y * m.width + lead.x]
  if not s or s.searched then return false end
  return lead.type == armytype.HERO or s.type == require("warlords.site").TEMPLE
end

-- A menu item this engine cannot do yet is greyed, the way the original greys
-- one that is not available: saying so beats a pick that does nothing.
function G.menuEnabled(key)
  if key == nil or MENU_DOES[key] == nil then return false end
  -- 7f77:0200: on the start screens every menu is greyed but Game's Quit,
  -- and Load game while a slot holds one (7721:0e25; Load map likewise, by
  -- 7721:0e42, which the remake does not have)
  if G.starting then
    return key == "^Q" or (key == "alt L" and require("ui.savegame").used() > 0)
  end
  -- 8065:0519, the menu's own refresh after every action, greys what does
  -- not apply at the moment; these are the rules the remake has taken on
  if key == "," then return sideHasHero() end
  if key == "z" then return canSearch() end
  return true
end

function love.keypressed(key)
  -- the computer's moves being shown: a key or a click runs the rest through
  if G.aiRun and not kit.top() then
    -- Shift and Alt are held to reach Settings, not pressed to skip
    if key and key:match("shift$") or key and key:match("alt$") then return end
    G.aiSkip = true
    if G.walk and G.walk.computer then G.walk = nil resumeComputer() end
    return
  end
  -- any key, escape included, only dismisses the banner
  if G.banner then dismissBanner() return end

  -- The assault holds the keyboard the way the original does: space runs the
  -- playback through (6a35:0094 watches for it), and any key ends it.
  if G.assault then pressAssault() return end
  if G.victory then
    -- Occupy heads both the default and the cancel lists
    if key == "return" or key == "kpenter" or key == "escape" then takeCity(AS.occupy) end
    return
  end

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

  local top = kit.top()
  if top then
    -- the start screens' menu takes its accelerators too
    if top.menuBar and love.keyboard and love.keyboard.isDown then
      local accel
      if love.keyboard.isDown("lctrl", "rctrl") and key == "q" then accel = "^Q"
      elseif love.keyboard.isDown("lalt", "ralt") and #key == 1 then accel = "alt " .. key:upper() end
      if accel then
        G.openMenu = nil
        if G.menuEnabled(accel) then MENU_DOES[accel]() end
        return
      end
    end
    if G.openMenu then G.openMenu = nil return end
    if top.keypressed then top.keypressed(key) end
    return
  end

  if G.openMenu then G.openMenu = nil return end
  if G.over then return end

  -- The main screen's keys, as 17be:0064 and 17be:0444 give them. Enter and
  -- Escape are the first live control of the default and cancel lists --
  -- Next Army and Quit Army here -- and a letter is its menu item.
  local alt = love.keyboard and love.keyboard.isDown and love.keyboard.isDown("lalt", "ralt")
  local ctrl = love.keyboard and love.keyboard.isDown and love.keyboard.isDown("lctrl", "rctrl")
  local shift = love.keyboard and love.keyboard.isDown and love.keyboard.isDown("lshift", "rshift")
  local function press(id)
    if G.screen.state[id] ~= uidata.DISABLED and ACTION[id] then ACTION[id]() end
    refreshControls()
  end
  local digit = key:match("^kp(%d)$") or key:match("^(%d)$")

  if key == "return" or key == "kpenter" then press(174)
  elseif key == "escape" then press(175)
  elseif key == "space" then press(240)           -- group the stack (89e0:0a55)
  elseif key == "tab" then press(186)
  elseif key == "backspace" then press(187)
  elseif key == "home" then press(177)
  elseif key == "end" then deselect()             -- 1b62:08b3, as 178 is
  elseif key == "delete" then press(173)          -- walk on along the route
  elseif digit == "5" then
    -- 5 centres on the stack, or on the cursor (8611:0565)
    if G.selection then centreOn(G.selection.x, G.selection.y) end
  elseif digit then
    -- 1-9 step the stack the way the numeric pad lies (1c8c:0197): 8 north,
    -- then clockwise; 1c8c:041f forgets its destination as it goes
    local STEP = { ["8"] = { 0, -1 }, ["9"] = { 1, -1 }, ["6"] = { 1, 0 },
                   ["3"] = { 1, 1 }, ["2"] = { 0, 1 }, ["1"] = { -1, 1 },
                   ["4"] = { -1, 0 }, ["7"] = { -1, -1 } }
    local st = STEP[digit]
    if st and G.selection then
      for _, a in ipairs(G.selection.stack) do a.target = nil end
      moveSelection(G.selection.x + st[1], G.selection.y + st[2])
    end
  -- the arrows step the cursor, as the pad does (8611:0723)
  elseif key == "up" then press(320)
  elseif key == "right" then press(322)
  elseif key == "down" then press(324)
  elseif key == "left" then press(326)
  -- not the original's: quick save and load
  elseif key == "f5" then
    saveMod.write(G.g, G.savePath)
    say("Saved to %s.", G.savePath)
  elseif key == "f9" then loadGame()
  else
    local accel
    if ctrl and key == "q" then accel = "^Q"
    elseif alt and #key == 1 then accel = "alt " .. key:upper()
    elseif shift and key == "/" then accel = "?"
    elseif #key == 1 then accel = key end
    if accel and MENU_DOES[accel] then MENU_DOES[accel]() end
  end
end

-- Typing into the hero's name field. The original's is a real edit box, and
-- the rolled name is only its suggestion.
function love.textinput(text)
  local top = kit.top()
  if top and not G.offer then
    if top.textinput then top.textinput(text) end
    return
  end
  if not G.offer then return end
  if #G.offerName < HERO_NAME_MAX and text:match("^[%w%s'%-%.]+$") then
    G.offerName = G.offerName .. text
  end
end

--------------------------------------------------------------------- scaling

-- Everything above works in the original's 640x480. Any other window (the
-- game starts full screen) gets that frame drawn into a 640x480 canvas, blown
-- up by the largest whole number that fits with nearest filtering, so each
-- pixel becomes a crisp block, and centred with black around it. setScissor
-- works in the canvas's pixels while it is the target, so the clipping stays
-- right. The sums are in the display's real pixels (conf.lua asks for
-- highdpi), so on a Retina screen the blocks are whole device pixels, not
-- whole points the OS then resamples. The mouse comes back to 640x480 before
-- any handler sees it. (A function rather than a do block: main.lua's chunk is at Lua's limit on
-- locals.)
;(function()
  local W, H = 640, 480
  local drawFrame = love.draw
  local canvas, scale, ox, oy, dpi

  -- a window position (LOVE gives it in points) in the game's pixels, held
  -- to the frame
  local function toGame(x, y)
    x = math.floor((x * dpi - ox) / scale)
    y = math.floor((y * dpi - oy) / scale)
    return math.max(0, math.min(W - 1, x)), math.max(0, math.min(H - 1, y))
  end

  local function setUp()
    if canvas ~= nil or not love.graphics.newCanvas then return end
    dpi = love.graphics.getDPIScale()
    local ww, wh = love.graphics.getPixelDimensions()
    if ww == W and wh == H then canvas = false return end
    scale = math.max(1, math.floor(math.min(ww / W, wh / H)))
    ox = math.floor((ww - W * scale) / 2)
    oy = math.floor((wh - H * scale) / 2)
    canvas = love.graphics.newCanvas(W, H)
    canvas:setFilter("nearest", "nearest")
    local getPosition = love.mouse.getPosition
    love.mouse.getPosition = function() return toGame(getPosition()) end
    for _, name in ipairs({ "mousepressed", "mousereleased" }) do
      local handle = love.handlers[name]
      love.handlers[name] = function(x, y, ...)
        x, y = toGame(x, y)
        return handle(x, y, ...)
      end
    end
    local moved = love.handlers.mousemoved
    love.handlers.mousemoved = function(x, y, dx, dy, ...)
      x, y = toGame(x, y)
      return moved(x, y, dx * dpi / scale, dy * dpi / scale, ...)
    end
  end

  function love.draw()
    setUp()
    if not canvas then return drawFrame() end
    love.graphics.push("all")
    love.graphics.setCanvas(canvas)
    love.graphics.clear(love.graphics.getBackgroundColor())
    drawFrame()
    love.graphics.pop()
    love.graphics.clear(0, 0, 0)
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(canvas, ox / dpi, oy / dpi, 0, scale / dpi, scale / dpi)
  end
end)()

-- LOVE ignores what main.lua returns; test/ui.lua uses it to inspect state.
return G
