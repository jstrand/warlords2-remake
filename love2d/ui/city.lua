-- The city dialog: dialog 6 over popup 2, (80, 60) 480x312.
--
-- One frame, four modes. The row of buttons along the foot (193-196) switches
-- between them, and 7204:0000 is what every route in goes through, with the
-- mode as its argument:
--
--   0  Info        any city; what anyone can see of it
--   1  City        Rename, Raze, Build Prod
--   2  Production  what it builds
--   3  Vector      where what it builds goes
--
-- Modes 1-3 are only for a city of the side's own: 7204:03a9 greys their
-- buttons otherwise. Clicking one of your own cities on the map opens it in
-- Production, any other in Info (740d:0037).
--
-- The left half is the strategic map at two pixels a tile. The right half is
-- auto_ui_city_info (7204:06de), one case per mode, and every position below
-- is out of its data segment -- the table at 4125:0ea0 onward. Text is font 2,
-- CHANCE17, except the city's name at the top, in font 1 and centred on
-- x = 432 at y = 62 in every mode.

local kit    = require("ui.kit")
local input  = require("ui.input")
local buy    = require("ui.buyprod")
local game   = require("warlords.game")
local scn    = require("warlords.scn")
local uidata = require("warlords.uidata")

local M = {}

M.INFO, M.CITY, M.PRODUCTION, M.VECTOR = 0, 1, 2, 3

local R = { x = 80, y = 60, w = 480, h = 312 }     -- popup 2
local MAP = { x = 80, y = 60, w = 224, h = 312 }   -- area screen 3, region 6
local DIALOG = 6
local DONE, DONE_PROD, STOP = 192, 201, 202
local MODE_FIRST = 193
local SLOT_FIRST = 197                              -- 197-200, Production only
local RENAME, BUILD, RAZE = 203, 204, 205
-- Vector mode's three buttons, each with a lit twin at the same rect:
-- 210/214 send this city's armies somewhere new, 211/215 send the armies of
-- every city vectoring here somewhere new, 212/216 See All.
local V_SEND, V_MOVE, V_ALL = 210, 211, 212
local V_SEND_LIT, V_MOVE_LIT, V_ALL_LIT = 214, 215, 216

-- STRING.DAT groups
local S_STATS = 0x74       -- Income, Defence, and the three kinds of owner
local S_CURRENT = 0x75     -- "Current:", ", then to", "Standard!", "Nowhere!"
local S_RENAME, S_RAZE, S_BUILD = 0x77, 0x78, 0x79
local S_VECTOR = 0x9a      -- "Current:", "%dt", "Next turn:", "Turn after:"
local S_HELP1, S_HELP2 = 0x9b, 0x9c    -- what 210 and 211 do, idle and waiting

-- Rename and Raze put their words in the executable, not in STRING.DAT
-- (4125:1003-1025 and 4125:0a6c-0a9d).
local RENAME_TITLE = "Rename City"
local RENAME_LINES = { "Type the new name for", "this city" }
local RAZE_TITLE = "Raze City"
local RAZE_LINES = { "Are you sure that you", "want to", "raze %s?", "You won't be popular!" }

local SHIELD_L, SHIELD_R = { 312, 102 }, { 512, 102 }      -- 4125:0ea0, :0ea4
local STAT_AT = { { 356, 106 }, { 356, 126 }, { 356, 150 } } -- :0ea8-:0eb0
local TEXT_AT = { { 310, 259 }, { 310, 279 }, { 310, 299 } } -- :0ec8-:0ed0
local INFO_SLOTS = { { 320, 190 }, { 376, 190 }, { 432, 190 }, { 488, 190 } } -- :0f18
local PROD_SLOTS = { { 312, 142 }, { 360, 142 }, { 408, 142 }, { 456, 142 } } -- :0f78
local CITY_TEXT = { { 376, 183 }, { 376, 203 }, { 376, 231 },
                    { 376, 251 }, { 376, 279 }, { 376, 299 } }                  -- :0fa8

--------------------------------------------------------------------- helpers

local function owned(d)
  local G = kit.G
  return d.city.ownerIndex == G.player.index and #game.sideCities(G.g, G.player) > 0
end

--- The side whose capital this is, or nil. Its small shield marks the city.
local function capitalOf(city)
  for _, s in ipairs(kit.G.g.map.sides) do
    if s.capital == city then return s.index end
  end
  return nil
end

--- The small shield (8611:0bf7, size 4): BSHIELD.PCK's bottom row, 32x23
--- from (side * 32, 36).
local function smallShield(side, x, y)
  local G = kit.G
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.shieldImg,
    love.graphics.newQuad(side * 32, 36, 32, 23, G.shieldImg:getDimensions()), x, y)
end

--- The three lines every mode but Production opens with (the case at
--- 7204:0727 and the one at :1769 share them): income, defence and owner. A
--- razed city has neither income nor defence, and says so.
local function stats(city)
  local f = kit.font(2)
  love.graphics.setColor(1, 1, 1)
  local razed = city.razed
  f.draw(kit.text(S_STATS, 0):format(razed and 0 or city.income), STAT_AT[1][1], STAT_AT[1][2])
  f.draw(kit.text(S_STATS, 1):format(razed and 0 or city.defence), STAT_AT[2][1], STAT_AT[2][2])
  local owner
  if razed then owner = kit.text(S_STATS, 2)
  elseif city.ownerIndex == nil then owner = kit.text(S_STATS, 3)
  else owner = kit.text(S_STATS, 4):format(kit.G.g.map.sides[city.ownerIndex + 1].name) end
  f.draw(owner, STAT_AT[3][1], STAT_AT[3][2])
end

-- The owner's shield either side of the name; a razed city has no owner, so
-- it gets the neutral one (city_make_ruins clears the owner byte).
local function shields(city)
  local side = city.ownerIndex or 8
  kit.shield(side, SHIELD_L[1], SHIELD_L[2])
  kit.shield(side, SHIELD_R[1], SHIELD_R[2])
end

--- The city's own four map tiles, as 8611:0860 draws a terrain tile.
local function tile(mx, my, x, y)
  local G = kit.G
  local t = scn.tileAt(G.g.map, mx, my)
  local sheet = math.floor(t / 96) + 1
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.sheets[sheet], G.tileQuads[sheet][t % 96], x, y)
end

--------------------------------------------------------------------- modes

-- 0: Info. The production list, on grey rings -- unless the city is razed, or
-- View Production is on and the city is someone else's, when a picture of it
-- stands in instead: CITYBACK.PCK in a sunken frame, with the city's own four
-- map tiles over it (7204:0a66).
local function drawInfo(d)
  local G, city = kit.G, d.city
  shields(city)
  stats(city)
  local owner = city.ownerIndex or 8
  if not city.razed and (G.g.map.options.viewProduction == 0
                         or city.ownerIndex == G.player.index) then
    for i, at in ipairs(INFO_SLOTS) do
      local slot = city.slots[i]
      kit.army(slot and slot.type, owner, at[1], at[2], 1)
    end
  else
    kit.setPal(0)
    kit.outline(311, 169, 242, 88)
    kit.bevel(310, 168, 244, 90, 4, 2)
    love.graphics.setColor(1, 1, 1)
    if G.cityBack then
      love.graphics.draw(G.cityBack,
        love.graphics.newQuad(0, 0, 240, 86, G.cityBack:getDimensions()), 312, 170)
    end
    tile(city.x, city.y, 392, 176)
    tile(city.x, city.y + 1, 392, 216)
    tile(city.x + 1, city.y, 432, 176)
    tile(city.x + 1, city.y + 1, 432, 216)
  end
  local cap = capitalOf(city)
  if cap then smallShield(cap, 408, 170) end
  local lines = G.g.map.cityText[city.index] or {}
  local f = kit.font(2)
  love.graphics.setColor(1, 1, 1)
  for i, at in ipairs(TEXT_AT) do
    if lines[i] then f.draw(lines[i], at[1], at[2]) end
  end
end

-- 1: City. The three buttons, each with two lines of text beside it.
local function drawCityMode(d)
  shields(d.city)
  stats(d.city)
  local f = kit.font(2)
  love.graphics.setColor(1, 1, 1)
  local k = 1
  for _, group in ipairs({ S_RENAME, S_RAZE, S_BUILD }) do
    for i = 0, 1 do
      f.draw(kit.text(group, i), CITY_TEXT[k][1], CITY_TEXT[k][2])
      k = k + 1
    end
  end
end

--- What the city is building and how long it has to go, as the Production
--- and Vector modes both write it: "%dt", or "-" when it builds nothing.
local function countdown(city)
  if city.producing then return ("%dt"):format(city.countdown or 0) end
  return "-"
end

-- 2: Production (7204:0ef8). "Current:" with the army being built and its
-- countdown -- and, when it is vectored, where to -- then the list, the entry
-- chosen ringed in the side's colour, and the chosen type's numbers beside
-- BIGARMY.PCK (54e0:0000).
local function drawProduction(d)
  local G, city = kit.G, d.city
  local f = kit.font(2)
  local owner = city.ownerIndex or 8
  local cap = capitalOf(city)
  if cap then smallShield(cap, 312, 110) end
  love.graphics.setColor(1, 1, 1)
  kit.right(f, kit.text(S_CURRENT, 0), 408, 110)
  local building = city.producing and city.slots[city.producing]
  kit.army(building and building.type, owner, 416, 104, 1)
  local text = countdown(city)
  love.graphics.setColor(1, 1, 1)
  if city.vectorTo then
    -- a city by name; the standard as "Standard!", or "Nowhere!" once it is
    -- no longer planted (7204:0ef8)
    local where
    if city.vectorTo == game.STANDARD then
      where = kit.text(S_CURRENT, game.standardAt(G.g, G.player) and 2 or 3)
    else
      local dest = G.g.map.cities[city.vectorTo + 1]
      where = dest and dest.name or kit.text(S_CURRENT, 3)
    end
    f.draw(text .. kit.text(S_CURRENT, 1), 456, 102)
    f.draw(where, 456, 122)
  else
    f.draw(text, 456, 110)
  end

  local chosen
  for i, at in ipairs(PROD_SLOTS) do
    local slot = city.slots[i]
    local mine = slot and slot.type == d.chosen
    if mine then chosen = slot end
    kit.army(slot and slot.type, owner, at[1], at[2], mine and G.player.index + 2 or 1)
  end

  if chosen then
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.bigArmy, 320, 182)
    local x = 320 + 136                                -- 4125:060a
    f.draw(chosen.name, x, 182)
    f.draw(("Time: %d"):format(chosen.time), x, 212)
    f.draw(("Cost: %d"):format(chosen.cost), x, 232)
    f.draw(("Strength: %d"):format(chosen.strength), x, 252)
    f.draw(("Move: %d"):format(chosen.move), x, 272)
  end
end

-- 3: Vector (7204:12b7). What is being built and its countdown, and the two
-- armies of it already on the road -- the one out this turn at (432, 103),
-- the one arriving next turn at (472, 103) -- then two rows of what is on
-- its way here from other cities, next turn and the turn after, four at
-- most in each. Below them the two buttons each say what they do
-- (7087:0638), and say it differently while a click on the map is awaited.
local function drawVector(d)
  local G, city = kit.G, d.city
  local f = kit.font(2)
  local owner = city.ownerIndex or 8
  love.graphics.setColor(1, 1, 1)
  kit.right(f, kit.text(S_VECTOR, 0), 360, 109)
  local building = city.producing and city.slots[city.producing]
  kit.army(building and building.type, owner, 368, 103, 1)
  love.graphics.setColor(1, 1, 1)
  f.draw(city.producing and kit.text(S_VECTOR, 1):format(city.countdown or 0) or "-", 408, 109)

  local out, onRoad, next, after = nil, nil, {}, {}
  for _, a in ipairs(G.g.armies) do
    if a.transit and a.owner == G.player.index then
      if a.homeCity == city.index and a.transit.dest == city.vectorTo then
        if a.transit.turns >= 2 then out = a else onRoad = a end
      end
      if a.transit.dest == city.index and a.homeCity ~= city.index then
        local row = a.transit.turns >= 2 and after or next
        if #row < 4 then row[#row + 1] = a end
      end
    end
  end
  kit.army(out and out.type, owner, 432, 103, 1)
  kit.army(onRoad and onRoad.type, owner, 472, 103, 1)

  love.graphics.setColor(1, 1, 1)
  kit.right(f, kit.text(S_VECTOR, 2), 392, 155)
  kit.right(f, kit.text(S_VECTOR, 3), 392, 188)
  for i = 1, 4 do
    local x = 400 + (i - 1) * 40
    kit.army(next[i] and next[i].type, G.player.index, x, 149, 1)
    kit.army(after[i] and after[i].type, G.player.index, x, 182, 1)
  end

  love.graphics.setColor(1, 1, 1)
  local a0 = d.sub == 1 and 2 or 0
  f.draw(kit.text(S_HELP1, a0), 368, 221)
  f.draw(kit.text(S_HELP1, a0 + 1), 368, 241)
  local b0 = d.sub == 2 and 2 or 0
  f.draw(kit.text(S_HELP2, b0), 368, 272)
  f.draw(kit.text(S_HELP2, b0 + 1), 368, 292)
end

local DRAW = {
  [M.INFO] = drawInfo, [M.CITY] = drawCityMode,
  [M.PRODUCTION] = drawProduction, [M.VECTOR] = drawVector,
}

--------------------------------------------------------------------- states

--- Which controls this mode shows, and in what state (7204:03a9). A control
--- the routine does not repaint is simply not there: the panel under it has
--- been drawn over.
local function refresh(d)
  local st = d.view.state
  local mine = owned(d)
  for i = 0, 3 do
    local id = MODE_FIRST + i
    if i == d.mode then st[id] = uidata.ACTIVE
    elseif i > 0 and not mine then st[id] = uidata.DISABLED
    else st[id] = uidata.NORMAL end
  end

  local shown = { [193] = true, [194] = true, [195] = true, [196] = true }
  if d.mode == M.PRODUCTION then
    for i = 0, 3 do shown[SLOT_FIRST + i] = true; st[SLOT_FIRST + i] = uidata.NORMAL end
    shown[DONE_PROD], st[DONE_PROD] = true, uidata.NORMAL
    shown[STOP] = true
    st[STOP] = d.city.producing and uidata.NORMAL or uidata.DISABLED
  else
    shown[DONE], st[DONE] = true, uidata.NORMAL
  end
  if d.mode == M.CITY then
    for _, id in ipairs({ RENAME, RAZE, BUILD }) do shown[id], st[id] = true, uidata.NORMAL end
  end
  if d.mode == M.VECTOR then
    local G = kit.G
    local all = G.vectorSeeAll and V_ALL_LIT or V_ALL
    shown[all], st[all] = true, uidata.NORMAL
    local send = d.sub == 1 and V_SEND_LIT or V_SEND
    shown[send] = true
    st[send] = (d.sub == 1 or d.city.producing) and uidata.NORMAL or uidata.DISABLED
    local move = d.sub == 2 and V_MOVE_LIT or V_MOVE
    shown[move] = true
    st[move] = (d.sub == 2 or #game.vectoredTo(G.g, d.city) > 0)
               and uidata.NORMAL or uidata.DISABLED
  end

  d.hidden = {}
  for _, c in ipairs(d.view.dialog.controls) do
    if not shown[c.id] then d.hidden[c.id] = true end
  end
end

--------------------------------------------------------------------- the dialog

--- Open the dialog on a city. `mode` defaults the way a click on the map
--- does: Production for a city of your own, Info for any other.
function M.open(city, mode)
  local G = kit.G
  if not G.cityView then G.cityView = kit.view(DIALOG) end
  local d = { city = city, view = G.cityView }
  G.city = city

  function d.setMode(m)
    if m ~= M.INFO and city.ownerIndex ~= G.player.index then m = M.INFO end
    -- 7204:0000: going into Production chooses what the city builds now
    if m == M.PRODUCTION and d.mode ~= M.PRODUCTION then
      local b = city.producing and city.slots[city.producing]
      d.chosen = b and b.type or nil
    end
    d.mode = m
    d.sub = 0
    G.cityMode = m
    refresh(d)
  end

  function d.close()
    kit.pop(d)
    G.city = nil
    require("ui.tutorial").show("select")         -- 7204:0321
  end

  --- Choose what the city builds: slot n of the list (7087:00c8). The
  --- countdown starts again even if it was already building that.
  function d.pick(n)
    local slot = city.slots[n]
    if slot then d.chosen = slot.type end
    if not d.chosen then return end
    for i, s in ipairs(city.slots) do
      if s.type == d.chosen then game.setProduction(G.g, city, i) end
    end
    refresh(d)
  end

  function d.stop()
    game.setProduction(G.g, city, nil)
    d.chosen = nil
    refresh(d)
  end

  function d.rename()
    input.open({
      title = RENAME_TITLE, lines = RENAME_LINES, text = city.name,
      maxChars = 15, maxWidth = 128,                     -- 7204:2013
      ok = function(name) game.renameCity(G.g, city, name) d.setMode(M.CITY) end,
      cancelled = function() d.setMode(M.CITY) end,
    })
  end

  function d.raze()
    local lines = {}
    for i, l in ipairs(RAZE_LINES) do lines[i] = (i == 3) and l:format(city.name) or l end
    input.open({
      title = RAZE_TITLE, lines = lines, confirm = true,
      ok = function()
        game.raze(G.g, G.player, city, nil, true)
        if G.stratDirty then G.stratDirty() end
        d.setMode(M.INFO)
      end,
      cancelled = function() d.setMode(M.CITY) end,
    })
  end

  function d.build()
    buy.open(city, function() d.setMode(M.CITY) end)
  end

  function d.draw()
    kit.popup(R)
    if d.mode == M.VECTOR and G.drawVectorMap then
      local filter
      if d.sub == 1 then filter = -1
      elseif d.sub == 2 then filter = #game.vectoredTo(G.g, city) end
      G.drawVectorMap(MAP.x, MAP.y, city, filter, G.vectorSeeAll and d.sub == 0)
    elseif G.drawStrategicPanel then
      G.drawStrategicPanel(MAP.x, MAP.y, city)
    end
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), city.name, 432, 62)
    DRAW[d.mode](d)
    kit.drawControls(d.view, d.hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if c then
      if c.id == DONE or c.id == DONE_PROD then d.close()
      elseif c.id >= MODE_FIRST and c.id < MODE_FIRST + 4 then d.setMode(c.id - MODE_FIRST)
      elseif c.id >= SLOT_FIRST and c.id < SLOT_FIRST + 4 then d.pick(c.id - SLOT_FIRST + 1)
      elseif c.id == STOP then d.stop()
      elseif c.id == RENAME then d.rename()
      elseif c.id == RAZE then d.raze()
      elseif c.id == BUILD then d.build()
      -- 7087:050e: each of the first two toggles its waiting state; See All
      -- toggles for good, and is remembered from city to city
      elseif c.id == V_SEND or c.id == V_SEND_LIT then
        d.sub = (d.sub == 1) and 0 or 1
        refresh(d)
      elseif c.id == V_MOVE or c.id == V_MOVE_LIT then
        d.sub = (d.sub == 2) and 0 or 2
        refresh(d)
      elseif c.id == V_ALL or c.id == V_ALL_LIT then
        G.vectorSeeAll = not G.vectorSeeAll
        refresh(d)
      end
      return
    end
    if d.mode == M.VECTOR and x >= MAP.x and x < MAP.x + MAP.w
       and y >= MAP.y and y < MAP.y + MAP.h then
      d.mapClick(math.floor((x - MAP.x) / 2), math.floor((y - MAP.y) / 2))
    end
  end

  --- A click on the map in Vector mode (7087:072e, 7087:028b). It means the
  --- side's city nearest the click. With Shift held, or while waiting to
  --- send this city's armies somewhere, that city becomes the destination --
  --- if this city is building, and the destination takes fewer than four,
  --- and clicking this city itself lifts the vector. While waiting to move
  --- the armies coming here, every city sending them is redirected to it, if
  --- it could take them all. Otherwise -- and afterwards, unless it was a
  --- send -- the dialog moves to the city clicked.
  function d.mapClick(mx, my)
    local target = game.nearestCity(G.g, mx, my, G.player)
    if not target then return end
    -- the planted standard, if it is nearer the click than the city is
    -- (828e:0651)
    local sx, sy = game.standardAt(G.g, G.player)
    local toStandard = sx and math.max(math.abs(sx - mx), math.abs(sy - my))
                       < math.max(math.abs(target.x - mx), math.abs(target.y - my))
    local shift = love.keyboard and love.keyboard.isDown
                  and love.keyboard.isDown("lshift", "rshift")
    local sub = d.sub
    if shift or sub == 1 then
      if city.producing then
        if toStandard then game.vectorToStandard(G.g, city, G.player)
        else game.vector(G.g, city, target) end
      end
      if shift then d.sub = 0 refresh(d) return end
    elseif sub == 2 then
      local incoming = game.vectoredTo(G.g, city)
      local there = game.vectoredTo(G.g, target)
      local ok = #incoming > 0 and #incoming + #there <= game.MAX_VECTORED_TO
      for _, c in ipairs(incoming) do if c == target then ok = false end end
      if ok then
        for _, c in ipairs(incoming) do c.vectorTo = target.index end
      end
    end
    d.sub = 0
    if sub ~= 1 and target ~= city then
      d.close()
      M.open(target, M.VECTOR)
      return
    end
    refresh(d)
  end

  -- Done heads both the cancel and the default lists (4125:1514, :155c)
  function d.keypressed(key)
    if key == "escape" or key == "return" or key == "kpenter" then d.close() end
  end

  kit.push(d)
  if mode == nil then
    mode = city.ownerIndex == G.player.index and M.PRODUCTION or M.INFO
  end
  d.mode = nil
  d.setMode(mode)
  -- the tutorial on production (7204:025d): the first time, and again
  -- once the side holds two cities
  local moments = { "prod" }
  if #game.sideCities(G.g, G.player) >= 2 then moments[2] = "prod2" end
  require("ui.tutorial").chain(moments)
  return d
end

return M
