-- What a pillage or a sack took (auto_ui_pillage_sack_raze, 63fa:0508),
-- shown over the spoils dialog and closed, the two together, by any key or
-- click (8065:10fb).
--
-- Popup 12, (176, 60) 288x300, marble. In font 1, "Pillage!" or "Sack!"
-- (group 68, 0 or 1) centred on (320, 64); then in font 2, centred on 320:
--   y 115  "The city of %s is pillaged" / "... sacked"   group 69
--   y 135  "for %d gold!"                                 group 70
--   y 165  "Ability to produce %d unit(s) has been lost"  group 71
--   y 185  "and only %d unit(s) remain"                   group 72
-- A box sunk twice into the marble -- (184, 220) 272x130 bevelled (4, 2)
-- and (185, 221) 270x128 (2, 4) -- headed "Destroyed" at (192, 230) and
-- "Gold" at (368, 230) (group 68, 2 and 3), and three rows 30 apart: the
-- army lost on a grey ring at (208, 250 + 30i), its name at (248, 256 + 30i)
-- and its worth, "%d gp", at (368, 256 + 30i); an empty ring where none was.

local kit = require("ui.kit")

local M = {}

local R = { x = 176, y = 60, w = 288, h = 300 }     -- popup 12
local TITLE, LINE, GOLD, LOST, LEFT = 0x44, 0x45, 0x46, 0x47, 0x48

--- `sacked` false for a pillage. `lost` is game.pillage's or game.sack's
--- list; `left` the production types the city still has. `after` runs when
--- it is closed.
function M.open(sacked, city, gold, lost, left, after)
  local G = kit.G
  local d = {}
  local which = sacked and 1 or 0
  local function close() kit.pop(d) if after then after() end end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(TITLE, which), 320, 64)
    local f = kit.font(2)
    kit.centred(f, kit.text(LINE, which):format(city.name), 320, 115)
    kit.centred(f, kit.text(GOLD, 0):format(gold), 320, 135)
    kit.centred(f, kit.text(LOST, #lost == 1 and 0 or 1):format(#lost), 320, 165)
    kit.centred(f, kit.text(LEFT, left == 1 and 0 or 1):format(left), 320, 185)
    kit.bevel(184, 220, 272, 130, 4, 2)
    kit.bevel(185, 221, 270, 128, 2, 4)
    love.graphics.setColor(1, 1, 1)
    f.draw(kit.text(TITLE, 2), 192, 230)
    f.draw(kit.text(TITLE, 3), 368, 230)
    for i = 0, 2 do
      local l = lost[i + 1]
      local y = 250 + 30 * i
      kit.army(l and l.type, G.player.index, 208, y, 1)
      if l then
        love.graphics.setColor(1, 1, 1)
        local t = G.g.types.byId[l.type]
        f.draw(t and t.name or "", 248, y + 6)
        f.draw(("%d gp"):format(l.gold), 368, y + 6)
      end
    end
  end
  function d.mousepressed() close() end
  function d.keypressed() close() end
  return kit.push(d)
end

return M
