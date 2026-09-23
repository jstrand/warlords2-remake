-- View > Items (66d4:0c21): the scenario's fourteen magic items.
--
-- Popup 4, (120, 50) 400x360, drawn by 66d4:0c50: "Items" (group 166) in
-- font 1 centred on (320, 52), "Items in this scenario" (group 167) in font
-- 2 centred on (320, 382), and a row for each item from record 8 on -- the
-- sides' standards left out -- sorted by kind and then by value, 20 apart
-- from y = 90: its name in colour 7 ending at x = 304, and what it does from
-- x = 336 (group 167, by kind). Dialog 35: Done (495, default and cancel) and
-- Help (496), which shows HELP\HITEM.GFX (FILE.DAT group 0x45).

local kit    = require("ui.kit")
local help   = require("ui.help")
local rules  = require("warlords.rules")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 120, y = 50, w = 400, h = 360 }     -- popup 4
local DIALOG, DONE, HELP = 35, 495, 496
local FIRST, LAST = 8, 21                           -- records 8-21
local WHAT = {
  [rules.ITEM_BATTLE] = 1, [rules.ITEM_COMMAND] = 2, [rules.ITEM_FLIGHT] = 3,
  [rules.ITEM_DOUBLE_MOVE] = 4, [rules.ITEM_GOLD_PER_CITY] = 5,
}

function M.open()
  local G = kit.G
  local d = { view = kit.view(DIALOG) }
  d.view.state[DONE], d.view.state[HELP] = uidata.NORMAL, uidata.NORMAL

  -- an insertion sort, by kind then value: equal ones keep their order
  local list = {}
  for _, it in ipairs(G.g.map.items) do
    if it.index >= FIRST and it.index <= LAST then list[#list + 1] = it end
  end
  for i = 2, #list do
    local j = i
    while j > 1 and ((list[j].type or 0) < (list[j - 1].type or 0)
          or ((list[j].type or 0) == (list[j - 1].type or 0)
              and (list[j].value or 0) < (list[j - 1].value or 0))) do
      list[j], list[j - 1] = list[j - 1], list[j]
      j = j - 1
    end
  end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(0xa6, 0), 320, 52)
    kit.centred(kit.font(2), kit.text(0xa7, 0), 320, 382)
    local f = kit.font(2).colours(7, 0)
    -- the kind's line index carries over from the row before when an item is
    -- of no kind it knows, as 66d4:0c50's does
    local what = 0
    for i, it in ipairs(list) do
      local y = 70 + 20 * i
      what = WHAT[it.type] or what
      kit.right(f, it.name or "", 304, y)
      f.draw(kit.text(0xa7, what):format(it.value or 0), 336, y)
    end
    kit.drawControls(d.view)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == DONE then kit.pop(d)
    elseif c.id == HELP then help.open("HELP\\HITEM.GFX") end
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  return kit.push(d)
end

return M
