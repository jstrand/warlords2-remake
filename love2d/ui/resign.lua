-- Order > Resign (7721:150d).
--
-- Popup 1, (160, 90) 320x200: "Resign!" (group 165) in font 1 centred on
-- (320, 92), and three lines of font 2 centred on 320 at y = 140, 160 and
-- 180. Dialog 33, three buttons one above the other: 487 resigns
-- graciously, 488 resigns, 489 thinks better of it (default and cancel
-- both).
--
-- Either way the side's cities burn and its armies go (7721:1608). The
-- gracious way is asked three times first, each a message box of its own
-- (8065:10fb), and is told it will not be graciously (8065:1160); the other
-- is told afterwards that everything has burned.

local kit      = require("ui.kit")
local searchUi = require("ui.search")
local game     = require("warlords.game")
local uidata   = require("warlords.uidata")

local M = {}

local R = { x = 160, y = 90, w = 320, h = 200 }     -- popup 1
local DIALOG, GRACIOUS, RESIGN, CANCEL = 33, 487, 488, 489
local S = 0xa5

function M.open()
  local G = kit.G
  local d = { view = kit.view(DIALOG) }
  for _, id in ipairs({ GRACIOUS, RESIGN, CANCEL }) do d.view.state[id] = uidata.NORMAL end
  local t = function(i) return kit.text(S, i) end

  local function burn(after)
    game.resign(G.g, G.player)
    G.selection = nil
    if G.stratDirty then G.stratDirty() end
    if after then after() end
  end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), t(0), 320, 92)
    for i = 1, 3 do kit.centred(kit.font(2), t(i), 320, 120 + 20 * i) end
    kit.drawControls(d.view)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == CANCEL then kit.pop(d)
    elseif c.id == GRACIOUS then
      kit.pop(d)
      searchUi.say(t(6), function()
        searchUi.say(t(7), function()
          searchUi.say(t(8), function()
            searchUi.message(t(9), t(10), function() burn() end)
          end)
        end)
      end)
    elseif c.id == RESIGN then
      kit.pop(d)
      burn(function() searchUi.message(t(4), t(5)) end)
    end
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  return kit.push(d)
end

return M
