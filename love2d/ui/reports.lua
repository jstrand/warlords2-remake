-- The Report menu: Army, City, Gold, Production and Winning.
--
-- One dialog, dialog 7 over popup 2 -- (80, 60) 480x312 -- with the five
-- reports as tabs along y = 103 (controls 218-222, the one showing lit) and
-- Done (223). 6ef3:0000 opens it on the report asked for, and
-- auto_ui_reports_menu (6ef3:0030) draws it:
--
--   the strategic map on the left -- with a banner on every stack outside a
--     city for the Army report (834b:158d), and See All's vectors for the
--     Production report (834b:1817)
--   the report's title (group 73) in font 1, centred on (432, 62)
--   what it measures (group 74), centred on (432, 149)
--   the body: a bar per side still in the game, or for Production the list
--     of what was built this turn
--   the summary (groups 75-79), centred on (432, 322) in the side's colours
--
-- The bars (6ef3:05ff) start at x = 312, one side a row at y = 200 + 14 i,
-- and fill 240 pixels at the top of the scale. Each is tiled 8 pixels at a
-- time from SHIELDS.PCK -- the side's 8x8 at (320 + 8 (i % 4), 40 + 8 (i / 4))
-- -- and sunk with a (0, 1) bevel. Above them the scale: a colour-1 axis at
-- y = 196 from 312 with ticks at 312, 372, 432, 492, 552 (the long ones 6 high,
-- the short 4), shadowed in black a pixel up and left, and "0", half and the
-- top of the scale at (308, 172), (432, 172) and (556, 172).
--
-- The Production list (6f8c:086e) shows five of this turn's armies at a time:
-- its number at (312, 176 + 30 i), the army on a grey ring at
-- (328, 170 + 30 i), and at (368, 176 + 30 i) where it went -- the city's
-- name, "%s ..." for one sent away, "... %s" for one that arrived. 206-209
-- scroll it a row or five rows up and down (6f8c:0abd-0b91).

local kit    = require("ui.kit")
local report = require("warlords.report")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 312 }      -- popup 2
local MAP = { x = 80, y = 60 }
local DIALOG = 7
local TAB_FIRST, DONE = 218, 223
local UP, DOWN, PAGE_UP, PAGE_DOWN = 206, 207, 208, 209
local S_TITLE, S_WHAT = 0x49, 0x4a
local S_SUMMARY = { [0] = 0x4b, 0x4c, 0x4d, 0x4e, 0x4f }
local ROWS = 5

--- The summary line (6ef3:0030's switch).
local function summary(n, f)
  local r = f.result
  if n == report.ARMY or n == report.CITY then
    return r == 1 and kit.text(S_SUMMARY[n], 0) or kit.text(S_SUMMARY[n], 1):format(r)
  elseif n == report.GOLD then
    return kit.text(S_SUMMARY[n], 0):format(r)
  elseif n == report.PRODUCTION then
    return kit.text(S_SUMMARY[n], r == 1 and 1 or 0):format(r)
  end
  return kit.text(S_SUMMARY[n], r)
end

--- A banner for every stack the side can see outside a city (834b:158d):
--- ATRANS2.PCK's 16x10 at (owner * 16, 164), at the tile's (2x - 1, 2y - 1),
--- one a tile -- the first army found there decides whose.
local function drawArmyBanners()
  local G = kit.G
  local game = require("warlords.game")
  local iw, ih = G.atransShields:getDimensions()
  local done = {}
  love.graphics.setScissor(MAP.x, MAP.y, 224, 312)
  love.graphics.setColor(1, 1, 1)
  for _, a in ipairs(G.g.armies) do
    if not a.transit and a.x then
      local k = a.y * 1000 + a.x
      if not done[k] and game.seen(G.g, G.player, a.x, a.y)
         and not game.cityAt(G.g, a.x, a.y) then
        done[k] = true
        local side = a.owner or 8
        love.graphics.draw(G.atransShields, love.graphics.newQuad(side * 16, 164, 16, 10, iw, ih),
                           MAP.x + math.max(0, a.x * 2 - 1), MAP.y + math.max(0, a.y * 2 - 1))
      end
    end
  end
  love.graphics.setScissor()
end

local function drawBars(f)
  local G = kit.G
  local sw, sh = G.shieldsImg:getDimensions()
  for i = 0, 7 do
    if not f.out[i] then
      local y = 200 + 14 * i
      local w = 1
      if f.max > 0 then w = math.floor(math.max(0, f.value[i]) * 240 / f.max) end
      w = math.max(1, w)
      love.graphics.setColor(1, 1, 1)
      local sx, sy = 320 + 8 * (i % 4), 40 + 8 * math.floor(i / 4)
      local x = 0
      while x < w do
        local piece = math.min(8, w - x)
        love.graphics.draw(G.shieldsImg, love.graphics.newQuad(sx, sy, piece, 8, sw, sh),
                           312 + x, y)
        x = x + 8
      end
      kit.bevel(311, y - 1, w + 2, 10, 0, 1)
    end
  end

  -- the scale: the axis in colour 1, then again in black a pixel up and left
  local function axis(colour, d)
    kit.setPal(colour)
    love.graphics.rectangle("fill", 312 - d, 196 - d, 240, 1)
    for k, tx in ipairs({ 312, 372, 432, 492, 552 }) do
      local long = (k % 2 == 1)
      love.graphics.rectangle("fill", tx - d, (long and 190 or 192) - d, 1, long and 6 or 4)
    end
  end
  axis(1, 0)
  axis(0, 1)
  local font = kit.font(2)
  love.graphics.setColor(1, 1, 1)
  kit.right(font, ("%d"):format(f.max), 556, 172)
  kit.centred(font, ("%d"):format(math.floor(f.max / 2)), 432, 172)
  kit.centred(font, "0", 308, 172)
end

local function drawProduction(d)
  local G = kit.G
  local list = G.player.produced or {}
  local font = kit.font(2)
  for i = 0, ROWS - 1 do
    local e = list[d.top + i + 1]
    if not e then break end
    local y = 176 + 30 * i
    love.graphics.setColor(1, 1, 1)
    font.draw(("%d"):format(d.top + i + 1), 312, y)
    kit.army(e.type, G.player.index, 328, 170 + 30 * i, 1)
    local city = e.city and G.g.map.cities[e.city + 1]
    local name = e.standard and "Standard" or (city and city.name or "")
    local text
    if e.kind == "sent" then text = ("%s ..."):format(name)
    elseif e.kind == "arrived" then text = ("... %s"):format(name)
    else text = name end
    love.graphics.setColor(1, 1, 1)
    font.draw(text, 368, y)
  end
end

local function refresh(d)
  local st = d.view.state
  for i = 0, 4 do st[TAB_FIRST + i] = (i == d.n) and uidata.ACTIVE or uidata.NORMAL end
  st[DONE] = uidata.NORMAL
  d.hidden = {}
  if d.n == report.PRODUCTION then
    local count = #(kit.G.player.produced or {})
    local up = d.top > 0
    local down = d.top + ROWS < count
    st[UP] = up and uidata.NORMAL or uidata.DISABLED
    st[PAGE_UP] = st[UP]
    st[DOWN] = down and uidata.NORMAL or uidata.DISABLED
    st[PAGE_DOWN] = st[DOWN]
  else
    for _, id in ipairs({ UP, DOWN, PAGE_UP, PAGE_DOWN }) do d.hidden[id] = true end
  end
end

--- Open the reports on report `n` (report.ARMY ... report.WINNING).
function M.open(n)
  local G = kit.G
  local d = { view = kit.view(DIALOG), n = n, top = 0 }

  function d.show(k)
    d.n, d.top = k, 0
    refresh(d)
  end

  function d.close() kit.pop(d) end

  function d.scroll(by)
    local count = #(G.player.produced or {})
    d.top = math.max(0, math.min(math.max(0, count - ROWS), d.top + by))
    refresh(d)
  end

  function d.draw()
    local f = report.figures(G.g, G.player, d.n)
    kit.popup(R)
    if d.n == report.PRODUCTION then
      G.drawVectorMap(MAP.x, MAP.y, nil, nil, true)
    else
      G.drawStrategicMap(MAP.x, MAP.y)
      if d.n == report.ARMY then drawArmyBanners() end
    end
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(S_TITLE, d.n), 432, 62)
    kit.centred(kit.font(2), kit.text(S_WHAT, d.n), 432, 149)
    if d.n == report.PRODUCTION then drawProduction(d) else drawBars(f) end
    local side = G.player
    kit.centred(kit.font(2).colours(side.colour or 15, side.edge or 0), summary(d.n, f), 432, 322)
    kit.drawControls(d.view, d.hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if not c then return end
    if c.id == DONE then d.close()
    elseif c.id >= TAB_FIRST and c.id < TAB_FIRST + 5 then d.show(c.id - TAB_FIRST)
    elseif c.id == UP then d.scroll(-1)
    elseif c.id == DOWN then d.scroll(1)
    elseif c.id == PAGE_UP then d.scroll(-ROWS)
    elseif c.id == PAGE_DOWN then d.scroll(ROWS)
    end
  end

  -- Done heads both the default and the cancel lists
  function d.keypressed(key)
    if key == "escape" or key == "return" or key == "kpenter" then d.close() end
  end

  refresh(d)
  return kit.push(d)
end

return M
