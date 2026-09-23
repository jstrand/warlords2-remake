-- The game's list chooser (796c:0000): pick one of a list of names.
--
-- Popup 3, (96, 50) 200x200, with dialog 2. The title in font 2 centred on
-- (196, 52); a colour-3 box at (106, 70) 180x110 sunk with a (4, 2) bevel;
-- five names from (112, 74), 20 apart, the chosen one in colour 15 and the
-- rest in colour 2 (796c:00f3). Controls 114-118 choose a row, 121/122
-- scroll a row up and down and 119/120 five rows, all four hidden when the
-- list fits (796c:0301); OK (123, the default) hands the chosen entry to the
-- caller, Cancel (124, the cancel) hands it nil (796c:01ab, 01d4).

local kit    = require("ui.kit")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 96, y = 50, w = 200, h = 200 }       -- popup 3
local BOX = { x = 106, y = 70, w = 180, h = 110 }    -- 4125:15a6
local DIALOG = 2
local ROW, PAGE_UP, PAGE_DOWN, UP, DOWN, OK, CANCEL = 114, 119, 120, 121, 122, 123, 124
local ROWS = 5

--- Open the chooser on `list` (each entry has a `name`) with entry `start`
--- (1-based) chosen; `after(entry or nil)` runs when it closes.
function M.open(title, list, start, after)
  local d = { view = kit.view(DIALOG), top = 0, cur = (start or 1) - 1 }
  local n = #list
  while d.cur >= ROWS do d.cur, d.top = d.cur - 1, d.top + 1 end

  local function refresh()
    local st = d.view.state
    for i = 0, ROWS - 1 do st[ROW + i] = uidata.NORMAL end
    st[OK], st[CANCEL] = uidata.NORMAL, uidata.NORMAL
    d.hidden = {}
    if n > ROWS then
      local up = d.top > 0 and uidata.NORMAL or uidata.DISABLED
      local down = d.top + ROWS - 1 < n - 1 and uidata.NORMAL or uidata.DISABLED
      st[PAGE_UP], st[UP], st[PAGE_DOWN], st[DOWN] = up, up, down, down
    else
      for _, id in ipairs({ PAGE_UP, PAGE_DOWN, UP, DOWN }) do d.hidden[id] = true end
    end
  end

  local function close(entry)
    kit.pop(d)
    if after then after(entry) end
  end

  function d.draw()
    kit.popup(R)
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    kit.centred(f, title, 196, 52)
    kit.setPal(3)
    love.graphics.rectangle("fill", BOX.x, BOX.y, BOX.w, BOX.h)
    kit.bevel(BOX.x, BOX.y, BOX.w, BOX.h, 4, 2)
    for i = 0, ROWS - 1 do
      local e = list[d.top + i + 1]
      if e then
        f.colours(i == d.cur and 15 or 2, 0).draw(e.name or "", 112, 74 + 20 * i)
      end
    end
    kit.drawControls(d.view, d.hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if not c then return end
    local id = c.id
    if id >= ROW and id < ROW + ROWS then
      if list[d.top + id - ROW + 1] then d.cur = id - ROW end
    elseif id == UP then
      d.top, d.cur = d.top - 1, math.min(d.cur + 1, ROWS - 1)
    elseif id == DOWN then
      d.top, d.cur = d.top + 1, math.max(d.cur - 1, 0)
    elseif id == PAGE_UP then
      d.top = d.top - math.min(ROWS, d.top)
    elseif id == PAGE_DOWN then
      d.top = d.top + math.min(ROWS, n - (d.top + ROWS - 1) - 1)
    elseif id == OK then return close(list[d.top + d.cur + 1])
    elseif id == CANCEL then return close(nil)
    end
    refresh()
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" then close(list[d.top + d.cur + 1])
    elseif key == "escape" then close(nil) end
  end

  refresh()
  return kit.push(d)
end

return M
