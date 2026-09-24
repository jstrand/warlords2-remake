-- History > City, Events, Gold, Winners (6d51:0000 with 0-3).
--
-- Plays back what warlords/history.lua recorded, a round at a time. With
-- nothing on record yet (turn 1) it says so in a message box (group 93).
-- Popup 10, (32, 60) 576x312, dialog 19, drawn by 6d51:0096:
--
--   the strategic map at (32, 60) with the city shields as they stood on the
--     turn shown (834b:12d3)
--   the title (group 91) in font 1 centred on (432, 64), and the four tabs
--     (352-355), the one showing lit
--   City, Gold, Winners (6d51:034a): a graph at (300, 150) 292x140 of every
--     side's cities, gold or score, one step a turn from the bottom-left
--     corner in the side's colour, scaled to the largest on record (at least
--     10, 500, 100); white axes with black shadows a pixel up and left, and
--     ticks; the top value and "0" right-aligned at x = 292, "0", "Turns" and
--     the last turn under the axis; the turn shown as a colour-13 line from
--     y = 148 to 296; and the line of group 92 centred on (432, 318) -- your
--     own figure, or who led on Winners
--   Events (6d51:16ba): that round's deeds, 17 apart from y = 149, each its
--     side's shield (ATRANS2's turn-strip 16x14) at x = 264 and the words at
--     x = 280 -- a war, peace or treachery adding the other side's shield
--     after them on the byte grid -- and a timeline under them (6d51:18da):
--     a box at (272, 319) 320x17 bevelled twice, filled from (275, 322) in
--     your colour up to the turn and your edge colour past it
--   "Turn %d" centred on (312, 350)
--
-- The arrows (357/358) step a turn, greyed at the ends; a click on the graph
-- (region 10) or the timeline (region 11) picks the turn under it (6d51:0908).
-- Done (356).

local kit     = require("ui.kit")
local history = require("warlords.history")
local uidata  = require("warlords.uidata")

local M = {}

local R = { x = 32, y = 60, w = 576, h = 312 }      -- popup 10
local DIALOG, TAB, DONE, PREV, NEXT = 19, 352, 356, 357, 358
local GX, GY, GW, GH = 300, 150, 292, 140            -- 4125:0dc6
local TL = { x = 275, y = 322, w = 314, h = 11 }     -- 4125:0dd6
M.CITY, M.EVENTS, M.GOLD, M.WINNERS = 0, 1, 2, 3
local FLOOR = { [0] = 10, [2] = 500, [3] = 100 }

local function values(r, mode)
  if mode == M.CITY then return r.cities
  elseif mode == M.GOLD then return r.gold end
  return r.score
end

local function hline(x, y, n) love.graphics.rectangle("fill", x, y, n, 1) end
local function vline(x, y, n) love.graphics.rectangle("fill", x, y, 1, n) end

local line = kit.line

function M.open(mode)
  local G = kit.G
  local recs = G.g.history or {}
  local n = math.min(#recs, history.LAST_TURN)
  if n < 1 then
    return require("ui.search").message(kit.text(0x5d, 0), kit.text(0x5d, 1))
  end
  local d = { view = kit.view(DIALOG), mode = mode, turn = n }
  local side = G.player

  local function refresh()
    local st = d.view.state
    for i = 0, 3 do st[TAB + i] = (i == d.mode) and uidata.ACTIVE or uidata.NORMAL end
    st[PREV] = d.turn == 1 and uidata.DISABLED or uidata.NORMAL
    st[NEXT] = d.turn == n and uidata.DISABLED or uidata.NORMAL
    st[DONE] = uidata.NORMAL
  end

  local function tx(t) return GX + math.floor(t * GW / n) end

  local function drawGraph()
    local top = FLOOR[d.mode]
    for t = 1, n do
      for s = 0, 7 do top = math.max(top, values(recs[t], d.mode)[s] or 0) end
    end
    local function axes(c, o)
      kit.setPal(c)
      vline(GX - o, GY - o, GH)
      hline(GX - o, GY + GH - 1 - o, GW)
      for _, y in ipairs({ GY, GY + math.floor(GH / 2), GY + GH - 1 }) do hline(GX - 4 - o, y - o, 4) end
      for _, x in ipairs({ GX, GX + math.floor(GW / 2), GX + GW - 1 }) do vline(x - o, GY + GH - 1 - o, 4) end
    end
    axes(0, 1)
    axes(15, 0)
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    kit.right(f, ("%d"):format(top), GX - 8, GY - 6)
    kit.right(f, "0", GX - 8, GY + GH - 8)
    kit.centred(f, "0", GX, GY + GH + 8)
    kit.right(f, ("%d"):format(n), GX + GW + 8, GY + GH + 8)
    kit.centred(f, "Turns", GX + math.floor(GW / 2), GY + GH + 8)
    local px, py = {}, {}
    for s = 0, 7 do px[s], py[s] = GX + 1, GY + GH - 2 end
    for t = 1, n do
      local x = tx(t)
      local v = values(recs[t], d.mode)
      for s = 0, 7 do
        local y = GY + GH - 1 - math.floor((v[s] or 0) / top * GH)
        local sd = G.g.map.sides[s + 1]
        line(sd and sd.colour or 15, px[s], py[s], x, y)
        px[s], py[s] = x, y
      end
    end
    local x = tx(d.turn)
    line(13, x, GY - 2, x, GY + GH + 6)
    -- the line under it (group 92)
    local r = recs[d.turn]
    local text
    if d.mode == M.WINNERS then
      local best, lead = 0, 0
      for s = 0, 7 do if (r.score[s] or 0) > best then best, lead = r.score[s], s end end
      local sd = G.g.map.sides[lead + 1]
      text = kit.text(0x5c, 3):format(d.turn, sd and sd.name or "")
    else
      text = kit.text(0x5c, d.mode):format(d.turn, values(r, d.mode)[side.index] or 0)
    end
    love.graphics.setColor(1, 1, 1)
    kit.centred(f, text, 432, 318)
  end

  local function shield(s, x, y)
    local w, h = G.atransShields:getDimensions()
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.atransShields,
      love.graphics.newQuad(112 + 16 * math.floor(s / 4), 94 + 14 * (s % 4), 16, 14, w, h), x, y)
  end

  local function eventText(e)
    local function t(i, ...) return kit.text(0x5e, i):format(...) end
    local sides, cities = G.g.map.sides, G.g.map.cities
    local function sideName(i) local s = sides[i + 1] return s and s.name or "" end
    local function cityName(i) local c = cities[i + 1] return c and c.name or "" end
    local ty, v1, v2 = e.type, e.v1, e.v2
    if ty == history.EMERGES then return t(0, e.name, cityName(v1))
    elseif ty == history.KILLED then
      if v1 == history.IN_BATTLE then return t(1, e.name)
      elseif v1 == history.SEARCHING then return t(2, e.name) end
      return t(3, e.name, cityName(v1))
    elseif ty == history.QUEST_DONE then return t(4, e.name)
    elseif ty == history.QUEST_GIVEN then return t(5, e.name)
    elseif ty == history.VANQUISHED then return t(6, sideName(v1))
    elseif ty == history.WON then
      if v1 == history.BY_NAME then return t(7, e.name, cityName(v2)) end
      return t(8, sideName(v1), cityName(v2))
    elseif ty == history.FINDS then
      local what
      if v1 == history.ALLIES then what = kit.text(0x5e, 13)
      elseif v1 == history.SAGE then what = kit.text(0x5e, 14)
      elseif v1 == history.GOLD then what = kit.text(0x5e, 15)
      else
        for _, it in ipairs(G.g.map.items) do if it.index == v1 then what = it.name end end
      end
      return t(9, e.name, what or "")
    elseif ty == history.VICTORIOUS then return t(10, sideName(v1))
    elseif ty == history.TREACHERY then return t(16, sideName(v1))
    elseif ty == history.WAR then return t(11, sideName(v1))
    elseif ty == history.PEACE then return t(12, sideName(v1)) end
    return ""
  end

  local function drawEvents()
    local f = kit.font(2)
    for i, e in ipairs(recs[d.turn].events or {}) do
      local y = 149 + 17 * (i - 1)
      shield(e.side, 264, y + 1)
      local text = eventText(e)
      love.graphics.setColor(1, 1, 1)
      f.draw(text, 280, y)
      if e.type == history.WAR or e.type == history.PEACE or e.type == history.TREACHERY then
        shield(e.v2, 280 + math.floor(f.width(text) / 8) * 8 + 16, y + 1)
      end
    end
    -- the timeline
    kit.bevel(272, 319, 320, 17, 4, 2)
    kit.bevel(273, 320, 318, 15, 4, 2)
    kit.setPal(0)
    kit.outline(274, 321, 316, 13)
    local w = math.min(TL.w, math.floor(d.turn * TL.w / n))
    kit.setPal(side.colour or 15)
    love.graphics.rectangle("fill", TL.x, TL.y, w, TL.h)
    if w < TL.w - 8 then
      kit.setPal(side.edge or 0)
      love.graphics.rectangle("fill", TL.x + w, TL.y, TL.w - w, TL.h)
    end
  end

  function d.draw()
    kit.popup(R)
    G.drawStrategicMap(R.x, R.y, nil, false, recs[d.turn].owners)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(0x5b, d.mode), 432, 64)
    if d.mode == M.EVENTS then drawEvents() else drawGraph() end
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(2), ("Turn %d"):format(d.turn), 312, 350)
    kit.drawControls(d.view)
  end

  local function pick(x, x0, w)
    local t = math.floor((x - x0) / w * n) + 1
    d.turn = math.max(1, math.min(n, t))
  end

  function d.mousepressed(x, y)
    if d.mode ~= M.EVENTS and x >= GX and x < GX + GW and y >= GY and y < GY + 160 then
      pick(x, GX, GW) return refresh()
    elseif d.mode == M.EVENTS and x >= TL.x and x < TL.x + TL.w and y >= TL.y and y < TL.y + TL.h then
      pick(x, TL.x, TL.w) return refresh()
    end
    local c = kit.controlAt(d.view, x, y)
    if not c or d.view.state[c.id] == uidata.DISABLED then return end
    if c.id == DONE then return kit.pop(d)
    elseif c.id == PREV then d.turn = d.turn - 1
    elseif c.id == NEXT then d.turn = d.turn + 1
    elseif c.id >= TAB and c.id < TAB + 4 then d.mode = c.id - TAB end
    refresh()
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  refresh()
  return kit.push(d)
end


--------------------------------------------------------------------- triumphs

-- History > Triumphs (6d51:09eb): what the side has done to each other side,
-- or lost itself. Popup 0, (80, 60) 480x320, dialog 20 (6d51:0a21):
-- "Triumphs" (group 80) centred on (320, 64); eight tabs from (128, 109),
-- 48 apart -- BUTTON.PCK's (400, 0) 48x40, (400, 40) for the one showing,
-- with the side's BSHIELD.PCK 32x36 at (side * 32, 0) on it, 8 in and 2
-- down (360-367); then five rows 35 apart from y = 168, each a kind of army
-- (types 4, 25, 28, 5 and 29 in that side's colours) on a grey ring at
-- x = 104 and, when the count is not 0, its line (groups 81-85 for your own
-- losses, 86-90 for the others', singular or plural) at (144, y + 9).
-- Done (359).

local TRI = { x = 80, y = 60, w = 480, h = 320 }    -- popup 0
local TRI_DIALOG, TRI_DONE, TRI_TAB = 20, 359, 360
local KINDS = { [0] = 4, 25, 28, 5, 29 }

function M.triumphs()
  local G = kit.G
  local me = G.player.index
  local d = { view = kit.view(TRI_DIALOG), opp = me }

  local function refresh()
    for i = 0, 7 do d.view.state[TRI_TAB + i] = (i == d.opp) and uidata.ACTIVE or uidata.NORMAL end
    d.view.state[TRI_DONE] = uidata.NORMAL
  end

  function d.draw()
    kit.popup(TRI)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(0x50, 0), 320, 64)
    local button, bshield = G.screen.art_for(4), G.screen.art_for(38)
    for i = 0, 7 do
      local x = 128 + 48 * i
      love.graphics.setColor(1, 1, 1)
      if button then
        love.graphics.draw(button.image,
          love.graphics.newQuad(400, i == d.opp and 40 or 0, 48, 40, button.w, button.h), x, 109)
      end
      if bshield then
        love.graphics.draw(bshield.image,
          love.graphics.newQuad(i * 32, 0, 32, 36, bshield.w, bshield.h), x + 8, 111)
      end
    end
    local f = kit.font(2)
    local base = (d.opp == me) and 0x51 or 0x56
    for k = 0, 4 do
      local y = 168 + 35 * k
      kit.army(KINDS[k], d.opp, 104, y, 1)
      local n = history.triumph(G.g, me, d.opp, k)
      if n > 0 then
        love.graphics.setColor(1, 1, 1)
        f.draw(kit.text(base + k, n == 1 and 0 or 1):format(n), 144, y + 9)
      end
    end
    local hidden = {}
    for i = 0, 7 do hidden[TRI_TAB + i] = true end
    kit.drawControls(d.view, hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == TRI_DONE then return kit.pop(d) end
    if c.id >= TRI_TAB and c.id < TRI_TAB + 8 then d.opp = c.id - TRI_TAB refresh() end
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  refresh()
  return kit.push(d)
end

return M
