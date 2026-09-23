-- View > Ruins (`.`): the city dialog's fifth mode, on a ruin or temple.
--
-- 17be's inline case runs 7204:0000 with mode 4 at the cursor, which takes
-- the nearest site shown to the side -- and seen, with a hidden map -- as the
-- crow flies (828e:06fd). Dialog 6 over popup 2, with the mode buttons
-- (193-196) hidden: only Done (192) is left. The map on the left is
-- 834b:05e4's, every site shown to the side marked and this one boxed; the
-- right half is auto_ui_city_info's case 4 (7204:0cdf):
--
--   the site's name in font 1, centred on (432, 62)
--   SPECBITS.PCK's 96x63 at (328, 104) -- the temple for a temple, else one
--     of five ruins by the site's number mod 5 -- in a black frame a pixel
--     out and a (4, 2) bevel round that
--   "Type: ..." (group 113, by what it holds) at (432, 112) and
--     "Explored: ..." (group 114) at (432, 136)
--   a colour-2 box at (312, 180) 240x48, outlined in black, holding the map
--     markers' legend: the four ATRANS2.PCK marks at (320, 188), (432, 188),
--     (320, 208), (432, 208), their words (group 115) 16 right and 3 up
--   the site's three lines of .SPC description at (310, 259), 20 apart

local kit    = require("ui.kit")
local game   = require("warlords.game")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 312 }      -- popup 2
local MAP = { x = 80, y = 60 }
local DIALOG, DONE = 6, 192
local PICTURES = { [0] = { 0, 0 }, { 96, 0 }, { 192, 0 }, { 0, 63 }, { 96, 63 }, { 192, 63 } }
local LEGEND = { { 112, 0 }, { 112, 10 }, { 112, 20 }, { 128, 0 } }     -- 4125:0f28
local LEGEND_AT = { { 320, 188 }, { 432, 188 }, { 320, 208 }, { 432, 208 } }

--- The site nearest (x, y) that the side may look at, or nil.
function M.nearest(x, y)
  local G = kit.G
  local best, bestD
  for _, s in ipairs(G.g.map.sites) do
    local shown = math.floor((s.revealed or 0) / 2 ^ G.player.index) % 2 == 1
    if shown and game.seen(G.g, G.player, s.x, s.y) then
      local d = math.floor(math.sqrt((s.x - x) ^ 2 + (s.y - y) ^ 2))
      if not bestD or d < bestD then best, bestD = s, d end
    end
  end
  return best
end

function M.open(s)
  local G = kit.G
  if not s then return nil end
  local site = require("warlords.site")
  local d = { view = kit.view(DIALOG) }
  d.view.state[DONE] = uidata.NORMAL
  local shown = { [DONE] = true }
  d.hidden = {}
  for _, c in ipairs(d.view.dialog.controls) do
    if not shown[c.id] then d.hidden[c.id] = true end
  end

  function d.draw()
    kit.popup(R)
    G.drawStrategicMap(MAP.x, MAP.y)
    G.drawSiteMarkers(MAP.x, MAP.y, s)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), s.name or "", 432, 62)
    kit.bevel(326, 102, 100, 67, 4, 2)
    kit.setPal(0)
    kit.outline(327, 103, 98, 65)
    local pic = PICTURES[s.content == site.TEMPLE and 0 or (s.index % 5) + 1]
    local art = G.screen.art_for(6)
    if art then
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(art.image, love.graphics.newQuad(pic[1], pic[2], 96, 63, art.w, art.h), 328, 104)
    end
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    f.draw(kit.text(0x71, s.content or 0), 432, 112)
    f.draw(kit.text(0x72, s.searched and 1 or 0), 432, 136)
    kit.setPal(2)
    love.graphics.rectangle("fill", 312, 180, 240, 48)
    kit.setPal(0)
    kit.outline(312, 180, 240, 48)
    local w, h = G.atransShields:getDimensions()
    for i, src in ipairs(LEGEND) do
      local at = LEGEND_AT[i]
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.atransShields, love.graphics.newQuad(src[1], src[2], 16, 10, w, h), at[1], at[2])
      f.draw(kit.text(0x73, i - 1), at[1] + 16, at[2] - 3)
    end
    local lines = G.g.map.siteText[s.index] or {}
    for i = 1, 3 do f.draw(lines[i] or "", 310, 239 + 20 * i) end
    kit.drawControls(d.view, d.hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if c and c.id == DONE then kit.pop(d) end
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  return kit.push(d)
end

return M
