-- Report > Diplomacy (484e:0000): the Diplomatic Report and, behind its
-- Action button, the Diplomatic Action screen. Only with the Diplomacy
-- option on.
--
-- Both are popup 11, (80, 60) 480x350.
--
-- The **report** (484e:0039, dialog 21): "Diplomatic Report" (group 107) in
-- font 1 centred on (320, 64); a grid in colour 1 -- a header row from
-- y = 110 and column from x = 88, then a 32x29 cell for every pair from
-- (120, 139) -- with each side in play's 16x16 shield along the top at
-- (128 + 32 i, 117) and down the side at (96, 146 + 29 i); in each cell what
-- DIPLOM.PCK shows for the state from that column's side to that row's --
-- (384, 0) war, (320, 0) peace, nothing for uneasy -- and (384, 29) on the
-- diagonal. Beside it a box outlined in black, (392, 110) 160x262, with
-- "Diplomatic Rating" (group 109) centred on (472, 116) and the sides best
-- first, each its shield at (400, 146 + 29 i) and title (group 106) at
-- (432, 146 + 29 i). Done (368) and Action (369).
--
-- The **action** screen (484e:0382, dialog 22): "Diplomatic Action" (group
-- 108) centred on (320, 64), the side's big shield at (96, 111) and name at
-- (144, 122), the row labels (group 110) at x = 104; then a 40-wide column
-- for every other side from x = 272: its big shield at y = 111, the state
-- between you at y = 151 and its proposal to you at 191 -- DIPLOM.PCK
-- (80 + 120 state, 45), a blank box when the proposal is the state or the
-- side is not playing -- and your three proposals to it at 245, 285 and 325:
-- peace (0, 45), uneasy (120, 45), war (240, 45), each lit 40 to the right
-- when it is the one chosen. The buttons under them (372-378, 380-386,
-- 388-394) set it (484e:0a69). OK (370) closes; Report (371) goes back.

local kit       = require("ui.kit")
local diplomacy = require("warlords.diplomacy")
local uidata    = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 350 }      -- popup 11
local REPORT, DONE, ACTION = 21, 368, 369
local ACT, OK, BACK = 22, 370, 371
local OFFER = { [0] = 372, 380, 388 }               -- a row of 7 each

local function playing(g, i)
  local s = g.map.sides[i + 1]
  return s and s.inUse
end

local function diplom(sx, sy, w, h, x, y)
  local art = kit.G.screen.art_for(47)
  if not art then return end
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(art.image, love.graphics.newQuad(sx, sy, w, h, art.w, art.h), x, y)
end

local function smallShield(side, x, y)
  local G = kit.G
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.shieldsImg,
    love.graphics.newQuad(side * 40 + 24, 46, 16, 16, G.shieldsImg:getDimensions()), x, y)
end

local function blank(x, y)
  kit.setPal(0)
  kit.outline(x, y, 40, 40)
  kit.setPal(2)
  love.graphics.rectangle("fill", x + 1, y + 1, 38, 38)
end

--- The proposal from a to b, which reads as the state when there is none.
local function proposal(g, a, b)
  local p = diplomacy.proposal(g, a, b)
  if p == nil then p = diplomacy.state(g, a, b) end
  return p
end

local openAction

local function openReport()
  local G = kit.G
  local g = G.g
  local d = { view = kit.view(REPORT) }
  d.view.state[DONE], d.view.state[ACTION] = uidata.NORMAL, uidata.NORMAL
  local titles = diplomacy.ratings(g)
  local order = {}
  for _, s in ipairs(g.sides) do order[#order + 1] = s end
  table.sort(order, function(p, q)
    local sp, sq = p.diploScore or 0, q.diploScore or 0
    if sp ~= sq then return sp < sq end
    return p.index < q.index
  end)

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(0x6b, 0), 320, 64)
    kit.setPal(1)
    love.graphics.rectangle("fill", 88, 139, 1, 232)
    for i = 0, 8 do love.graphics.rectangle("fill", 120 + 32 * i, 110, 1, 261) end
    love.graphics.rectangle("fill", 120, 110, 256, 1)
    for i = 0, 8 do love.graphics.rectangle("fill", 88, 139 + 29 * i, 288, 1) end
    for i = 0, 7 do
      if playing(g, i) then
        smallShield(i, 128 + 32 * i, 117)
        smallShield(i, 96, 146 + 29 * i)
      end
    end
    for col = 0, 7 do
      for row = 0, 7 do
        local x, y = 120 + 32 * col, 139 + 29 * row
        if col == row then diplom(384, 29, 32, 29, x, y)
        elseif playing(g, col) and playing(g, row) then
          local st = diplomacy.state(g, col, row)
          if st == diplomacy.WAR then diplom(384, 0, 32, 29, x, y)
          elseif st == diplomacy.PEACE then diplom(320, 0, 32, 29, x, y) end
        end
      end
    end
    kit.setPal(0)
    kit.outline(392, 110, 160, 262)
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    kit.centred(f, kit.text(0x6d, 0), 472, 116)
    for i, s in ipairs(order) do
      local y = 146 + 29 * (i - 1)
      smallShield(s.index, 400, y)
      love.graphics.setColor(1, 1, 1)
      f.draw(titles[s.index] or "", 432, y)
    end
    kit.drawControls(d.view)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == DONE then kit.pop(d)
    elseif c.id == ACTION then kit.pop(d) openAction() end
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  return kit.push(d)
end

openAction = function()
  local G = kit.G
  local g = G.g
  local me = G.player.index
  local d = { view = kit.view(ACT) }
  for _, c in ipairs(d.view.dialog.controls) do d.view.state[c.id] = uidata.NORMAL end
  local others = {}
  for i = 0, 7 do if i ~= me then others[#others + 1] = i end end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(0x6c, 0), 320, 64)
    kit.shield(me, 96, 111)
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    f.draw(G.player.name or "", 144, 122)
    for i, y in ipairs({ 165, 205, 251, 271, 291, 311, 331 }) do
      f.draw(kit.text(0x6e, i - 1), 104, y)
    end
    for col, other in ipairs(others) do
      local x = 272 + 40 * (col - 1)
      kit.shield(other, x, 111)
      if not playing(g, other) then
        for _, y in ipairs({ 151, 191, 245, 285, 325 }) do blank(x, y) end
      else
        local st = diplomacy.state(g, other, me)
        diplom(80 + 120 * st, 45, 40, 40, x, 151)
        local theirs = proposal(g, other, me)
        if theirs == st then blank(x, 191) else diplom(80 + 120 * theirs, 45, 40, 40, x, 191) end
        local mine = proposal(g, me, other)
        for k = 0, 2 do
          diplom(120 * k + (mine == k and 40 or 0), 45, 40, 40, x, 245 + 40 * k)
        end
      end
    end
    local hidden = {}
    for k = 0, 2 do for i = 0, 6 do hidden[OFFER[k] + i] = true end end
    kit.drawControls(d.view, hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == OK then return kit.pop(d)
    elseif c.id == BACK then kit.pop(d) return openReport() end
    for k = 0, 2 do
      local i = c.id - OFFER[k]
      if i >= 0 and i < 7 and others[i + 1] then
        diplomacy.propose(g, me, others[i + 1], k)
      end
    end
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  return kit.push(d)
end

--- Report > Diplomacy: nothing with the option off.
function M.open()
  if kit.G.g.map.options.diplomacy == 0 then return nil end
  return openReport()
end

--- The control panel's diplomacy button (183-185, 484e:0346): straight to
--- the Diplomatic Action screen, and nothing with the option off.
function M.action()
  if kit.G.g.map.options.diplomacy == 0 then return nil end
  return openAction()
end

--- Which of the three faces the button wears (8065:0174), from what the
--- other sides propose to `me` against where things stand with them
--- (diplomacy_flag_pending, 484e:0cc7): 183 when nobody proposes a change,
--- 185 when every change proposed is friendlier, 184 when any is more
--- hostile. Nil with the option off, when 183 is greyed.
function M.buttonFor(g, me)
  if g.map.options.diplomacy == 0 then return nil end
  local friendlier, hostile = false, false
  for s = 0, 7 do
    if s ~= me then
      local st, p = diplomacy.state(g, s, me), proposal(g, s, me)
      if p ~= st then
        if p < st then friendlier = true else hostile = true end
      end
    end
  end
  if not (friendlier or hostile) then return 183 end
  return hostile and 184 or 185
end

return M
