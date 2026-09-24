-- SSG > About Warlords II, and the "?" key (7721:0084).
--
-- Popup 23, (160, 55) 336x347, BSCROLL.PCK from its own (0, 0) with the
-- title painted in -- blitted through its mask, and so with no frame
-- (54f6:0000 masks popups 13 and 23 and frames only the others). 8065:1471 writes up to six lines of group 139 centred on
-- x = 328 at y = 161, 181, 201, 221, 241, 261, black edged in colour 7, an
-- empty line leaving its place blank: "", "Version 1.02", "", and three
-- lines of DOS memory in Kb. The remake has no DOS memory to report and
-- leaves those three out. Any key or click puts it away (8065:10fb).

local kit = require("ui.kit")

local M = {}

local R = { x = 160, y = 55, w = 336, h = 347 }     -- popup 23

function M.open()
  local G = kit.G
  local d = {}
  local lines = { kit.text(0x8b, 1), "Version 1.02", kit.text(0x8b, 3) }   -- 4125:1292
  if not G.bscroll then
    G.bscroll = require("warlords.pck").toImage(G.dataDir .. "/PICS/BSCROLL.PCK", G.palette, 10)
  end
  function d.draw()
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.bscroll, love.graphics.newQuad(0, 0, R.w, R.h, G.bscroll:getDimensions()), R.x, R.y)
    local f = kit.font(2).colours(0, 7)
    for i, line in ipairs(lines) do
      if line ~= "" then kit.centred(f, line, 328, 141 + 20 * i) end
    end
  end
  function d.mousepressed() kit.pop(d) end
  function d.keypressed() kit.pop(d) end
  return kit.push(d)
end

return M
