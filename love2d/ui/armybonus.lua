-- View > Army Bonus (89e0:1e3b): every army type, in the side's fight order,
-- with its strength, moves, way of moving and bonus.
--
-- Popup 0, (80, 60) 480x320, drawn by 89e0:1fd2: "Army Bonus" (group 164)
-- in font 1 centred on (320, 62); a box at (124, 146) 434x186 raised with a
-- (4, 2) bevel and outlined in black a pixel in; the column heads, STACK.PCK's
-- (188, 0) 216x22, at (256, 125); and six rows, 30 apart from y = 148:
--
--   the army on a ring of the side's colour at (128, y)
--   its name at (168, y + 5), strength at (280, y + 5), moves at (328, y + 5)
--   how it moves, from ABITS.PCK at (352, y + 8): (184, 30) flies, (216, 30)
--     woods and hills, (248, 30) woods, (152, 30) hills -- the table at
--     4125:45c4, filled from ARMYTYPE.DAT's +54/+56/+58 (7563:1b20)
--   its bonus (89e0:1a07, group 163) at (400, y + 6)
--
-- Dialog 34: Done (490, default and cancel), 491/492 a row up and down and
-- 493/494 six, greyed at the ends. Of the 29 places only the first 27 are
-- shown -- the list stops at 21 + 6.

local kit      = require("ui.kit")
local armytype = require("warlords.armytype")
local uidata   = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 320 }      -- popup 0
local DIALOG, DONE, UP, DOWN, PAGE_UP, PAGE_DOWN = 34, 490, 491, 492, 493, 494
local ROWS, PLACES = 6, 27
local S = 0xa3

-- 89e0:1a07: the first of the type's bonuses, in this order, as group 163
-- words it. A bonus field is ARMYTYPE.DAT's offset.
local function bonusText(t)
  local b = t.bonus
  local function say(i, v) return kit.text(S, i):format(v or 0) end
  if b[52] == 2 then return say(3) end
  if b[52] == 3 then return say(4) end
  if (b[48] or 0) ~= 0 then return say(5, b[42]) end
  if (b[46] or 0) ~= 0 and (b[44] or 0) ~= 0 and (b[42] or 0) ~= 0 and (b[40] or 0) ~= 0 then
    return say(16, b[40])
  end
  if b[52] == 1 then return say(6) end
  local order = { { 50, 7 }, { 46, 8 }, { 44, 9 }, { 42, 10 }, { 40, 11 },
                  { 38, 12 }, { 36, 13 }, { 34, 14 }, { 32, 15 } }
  for _, o in ipairs(order) do
    if (b[o[1]] or 0) ~= 0 then return say(o[2], b[o[1]]) end
  end
  return say(0)
end

function M.open()
  local G = kit.G
  local side = G.player
  local row = G.g.map.fightOrder[side.index]
  local byRank = {}
  for t = 0, 28 do if row[t] then byRank[row[t]] = t end end
  local d = { view = kit.view(DIALOG), top = 0 }

  local function refresh()
    local st = d.view.state
    st[DONE] = uidata.NORMAL
    local up = d.top > 0 and uidata.NORMAL or uidata.DISABLED
    local down = d.top + ROWS < PLACES and uidata.NORMAL or uidata.DISABLED
    st[UP], st[PAGE_UP], st[DOWN], st[PAGE_DOWN] = up, up, down, down
  end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(0xa4, 0), 320, 62)
    kit.bevel(124, 146, 434, 186, 4, 2)
    kit.setPal(0)
    kit.outline(125, 147, 432, 184)
    local stack = G.screen.art_for(46)
    if stack then
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(stack.image, love.graphics.newQuad(188, 0, 216, 22, stack.w, stack.h), 256, 125)
    end
    local abits = G.abits
    local aw, ah = abits:getDimensions()
    local f = kit.font(2)
    for i = 0, ROWS - 1 do
      local tid = byRank[d.top + i]
      local t = tid and G.g.types.byId[tid]
      if t then
        local y = 148 + 30 * i
        kit.army(tid, side.index, 128, y, side.index + 2)
        love.graphics.setColor(1, 1, 1)
        f.draw(t.name or "", 168, y + 5)
        f.draw(("%d"):format(t.strength or 0), 280, y + 5)
        f.draw(("%d"):format(t.move or 0), 328, y + 5)
        local src
        if t.flies then src = 184
        elseif t.woodsMove and t.hillsMove then src = 216
        elseif t.woodsMove then src = 248
        elseif t.hillsMove then src = 152 end
        if src then
          love.graphics.setColor(1, 1, 1)
          love.graphics.draw(abits, love.graphics.newQuad(src, 30, 32, 10, aw, ah), 352, y + 8)
        end
        love.graphics.setColor(1, 1, 1)
        f.draw(bonusText(t), 400, y + 6)
      end
    end
    kit.drawControls(d.view)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == DONE then return kit.pop(d)
    elseif c.id == UP then d.top = d.top - 1
    elseif c.id == DOWN then d.top = d.top + 1
    elseif c.id == PAGE_UP then d.top = math.max(0, d.top - ROWS)
    elseif c.id == PAGE_DOWN then d.top = math.min(PLACES - ROWS, d.top + ROWS)
    end
    refresh()
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  refresh()
  return kit.push(d)
end

M.bonusText = bonusText
return M
