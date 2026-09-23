-- Order > Signpost (540d:01a4): rewrite the sign the selected stack stands on.
--
-- Only on a tile of terrain 9 (a tower or a signpost) that has a sign in
-- CURRENT.SGN. Popup 1, (160, 90) 320x200, drawn by 540d:02d7:
--
--   "A Signpost!" in font 1, centred on (320, 94)
--   "Type the new message for" / "this signpost!" in font 2, centred on 320
--     at y = 140 and 160
--   the sign's two lines in fields (7ecb:0058) at (200, 190) and (200, 215),
--     240x22
--
-- over dialog 24: Done (421, default and cancel) and the two fields' hit
-- areas (422, 423). Clicking a field types a new line into it, up to 29
-- characters and 216 pixels (540d:03dd, 0432); the file is written back when
-- the dialog closes.

local kit    = require("ui.kit")
local input  = require("ui.input")
local game   = require("warlords.game")
local move   = require("warlords.move")
local scn    = require("warlords.scn")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 160, y = 90, w = 320, h = 200 }     -- popup 1
local DIALOG, DONE, LINE1, LINE2 = 24, 421, 422, 423
local FIELDS = { [1] = { x = 200, y = 190 }, [2] = { x = 200, y = 215 } }
local FIELD_W, FIELD_H = 240, 22
local MAX_CHARS, MAX_WIDTH = 30, 216

--- Open on the sign under `stack`, or do nothing if there is none.
function M.open(stack)
  local G = kit.G
  local a = stack and stack[1]
  if not a or scn.terrainAt(G.g.map, a.x, a.y) ~= move.TOWER then return nil end
  local sign = game.signAt(G.g, a.x, a.y)
  if not sign then return nil end

  local d = { view = kit.view(DIALOG) }
  for _, id in ipairs({ DONE, LINE1, LINE2 }) do d.view.state[id] = uidata.NORMAL end

  local function keep()
    if d.editing then sign[d.line], d.editing = d.editing.text, nil end
  end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), "A Signpost!", 320, 94)          -- 4125:03eb
    kit.centred(kit.font(2), "Type the new message for", 320, 140)
    kit.centred(kit.font(2), "this signpost!", 320, 160)
    for i, p in ipairs(FIELDS) do
      local text = (d.editing and d.line == i) and d.editing.text or sign[i]
      kit.field(p.x, p.y, FIELD_W, FIELD_H, text, kit.font(2))
      if d.editing and d.line == i then d.editing.drawCursor(p.x, p.y) end
    end
    kit.drawControls(d.view, { [LINE1] = true, [LINE2] = true })
  end

  function d.mousepressed(x, y)
    keep()
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == DONE then kit.pop(d)
    elseif c.id == LINE1 or c.id == LINE2 then
      d.line, d.editing = c.id - LINE1 + 1, input.editor(MAX_CHARS, MAX_WIDTH)
    end
  end

  function d.keypressed(key)
    if d.editing then
      local r = d.editing.key(key)
      if r == "keep" then keep() elseif r == "undo" then d.editing = nil end
      return
    end
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  function d.textinput(t)
    if d.editing then d.editing.input(t) end
  end

  return kit.push(d)
end

return M
