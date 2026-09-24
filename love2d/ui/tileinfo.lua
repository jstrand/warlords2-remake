-- The right button on the map: what is on a tile (740d:0037, button 2).
--
-- Only on a tile the side has seen, and gone when the button comes up
-- (740d:11cf). The box is 256x75, centred on the tile's centre -- x rounded
-- to the byte grid, then kept within x in [128, 512] and y in [37, 441]
-- (740d:131a) -- with POPUP.PCK's (0, 0) 256x75 behind it (740d:1201). Its
-- two lines of text are centred on the box's middle less 8, at y + 11 and
-- y + 35. What it shows, first that applies:
--
--   a stack the side may see -- its own, any with View Enemies on -- its
--     armies side by side across the middle at y + 16, 24 apart (740d:0bad);
--     an enemy in a tower instead only "Tower / Very hard to conquer!"
--     (group 130)
--   a city (740d:0c73): its name in its owner's colours (your own for a
--     neutral); "Razed!" for ruins; else the owner's 16x16 shields at x + 24
--     and x + 208, ABITS' coin (424, 0) at (x + 48, y + 36) with the income
--     at (x + 80, y + 33) and its castle (424, 11) at (x + 144, y + 36) with
--     the defence at (x + 176, y + 33); a capital adds the side's 32x23
--     BSHIELD at x + 16 and x + 200, y + 10
--   a site (740d:0fd7): its name in your colours, then "Blessings & Quests!"
--     for a temple, "Explored!" or "Unexplored!"
--   a signpost (740d:0a1c): its two lines over POPUP2.PCK instead -- a
--     wooden board, blitted through its mask (the [0x69c8] blit with
--     4125:546d), so the map shows round it: its ground, colour 1, is keyed
--   else the terrain (740d:10e1): its name (group 128) in colour 7 and what
--     it is (group 129) -- a road counts as terrain 0, and a port has lines
--     of its own

local kit    = require("ui.kit")
local game   = require("warlords.game")
local move   = require("warlords.move")
local scn    = require("warlords.scn")

local M = {}

local W, H = 256, 75

--- The box for a tile on the main map (740d:131a).
local function box(tx, ty)
  local G = kit.G
  local cx = (tx - G.cx) * 40 + 36
  local cy = (ty - G.cy) * 40 + 50
  cx = math.floor((cx + 4) / 8) * 8
  cx = math.max(W / 2, math.min(640 - W / 2, cx))
  cy = math.max(math.floor(H / 2), math.min(478 - math.floor(H / 2), cy))
  return cx - W / 2, cy - math.floor(H / 2)
end

local function art(id) return kit.G.screen.art_for(id) end

--- Show what is on (tx, ty); nil when the side has not seen it.
function M.open(tx, ty)
  local G = kit.G
  local g = G.g
  if not game.seen(g, G.player, tx, ty) then return nil end
  local bx, by = box(tx, ty)
  local me = G.player
  local d = {}
  local lines, back, draw = nil, 29, nil
  local f = kit.font(2)

  local armies = game.armiesAt(g, tx, ty)
  local owner = armies[1] and armies[1].owner
  local tower = g.towerAt and g.towerAt[ty * g.map.width + tx]
  local city = game.cityAt(g, tx, ty)
  local site = g.map.siteAt and g.map.siteAt[ty * g.map.width + tx]
  local terrain = scn.terrainAt(g.map, tx, ty)

  local function side(i) return g.map.sides[(i or 8) + 1] end
  local function colours(s)
    s = s or me
    return f.colours(s.colour or 15, s.edge or 0)
  end
  local function twoLines(a, b, fa, fb)
    kit.centred(fa or f.colours(7, 0), a or "", bx + W / 2 - 8, by + 11)
    kit.centred(fb or f, b or "", bx + W / 2 - 8, by + 35)
  end

  if #armies > 0 and (g.map.options.viewEnemies ~= 0 or owner == me.index or tower) then
    if owner ~= me.index and tower and g.map.options.viewEnemies == 0 then
      draw = function() twoLines(kit.text(0x82, 0), kit.text(0x82, 1)) end
    else
      draw = function()
        local n = #armies
        local x = math.floor((bx + W / 2 - n * 12 - 16 + 4) / 8) * 8
        for i, a in ipairs(armies) do kit.army(a.type, a.owner, x + 24 * (i - 1), by + 16, 0) end
      end
    end
  elseif city then
    draw = function()
      local own = city.ownerIndex and side(city.ownerIndex) or me
      kit.centred(colours(own), city.name or "", bx + W / 2 - 8, by + 11)
      if city.razed then
        kit.centred(f, "Razed!", bx + W / 2 - 8, by + 35)
        return
      end
      local sh = G.shieldsImg
      if city.ownerIndex and city.ownerIndex < 8 then
        for _, x in ipairs({ bx + 24, bx + W - 48 }) do
          love.graphics.setColor(1, 1, 1)
          love.graphics.draw(sh, love.graphics.newQuad(city.ownerIndex * 40 + 24, 46, 16, 16, sh:getDimensions()), x, by + 10)
        end
      end
      local aw, ah = G.abits:getDimensions()
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.abits, love.graphics.newQuad(424, 0, 24, 11, aw, ah), bx + 48, by + 36)
      f.draw(("%d"):format(city.income or 0), bx + 80, by + 33)
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.abits, love.graphics.newQuad(424, 11, 24, 11, aw, ah), bx + 144, by + 36)
      f.draw(("%d"):format(city.defence or 0), bx + 176, by + 33)
      local bs = art(38)
      for i, s in ipairs(g.map.sides) do
        if s.capital == city and bs then
          for _, x in ipairs({ bx + 16, bx + W - 56 }) do
            love.graphics.setColor(1, 1, 1)
            love.graphics.draw(bs.image, love.graphics.newQuad((i - 1) * 32, 36, 32, 23, bs.w, bs.h), x, by + 10)
          end
        end
      end
    end
  elseif site then
    draw = function()
      kit.centred(colours(me), site.name or "", bx + W / 2 - 8, by + 11)
      local siteMod = require("warlords.site")
      if site.content == siteMod.TEMPLE then
        kit.centred(f, "Blessings & Quests!", bx + W / 2 - 8, by + 35)
      elseif site.searched then
        kit.centred(f, "Explored!", bx + W / 2 - 8, by + 33)
      else
        kit.centred(f, "Unexplored!", bx + W / 2 - 8, by + 35)
      end
    end
  elseif terrain == move.TOWER and game.signAt(g, tx, ty) then
    local sign = game.signAt(g, tx, ty)
    back = 36
    draw = function() twoLines(sign[1], sign[2], f, f) end
  else
    local road = (scn.roadAt(g.map, tx, ty) or 0) % 32 ~= 0
    local port = g.map.crossing and g.map.crossing[ty * g.map.width + tx + 1]
    if port then
      draw = function() twoLines("Port", "A way for armies to put to sea") end
    else
      local t = road and 0 or terrain
      draw = function() twoLines(kit.text(0x80, t), kit.text(0x81, t)) end
    end
  end

  function d.draw()
    local b = art(back)
    if back == 36 then
      if not G.signBoard then
        local pck = require("warlords.pck")
        local img, w, h = pck.toImage(G.dataDir .. "/PICS/POPUP2.PCK", G.palette, 1)
        G.signBoard = { image = img, w = w, h = h }
      end
      b = G.signBoard
    end
    love.graphics.setColor(1, 1, 1)
    if b then love.graphics.draw(b.image, love.graphics.newQuad(0, 0, W, H, b.w, b.h), bx, by) end
    love.graphics.setColor(1, 1, 1)
    draw()
  end
  function d.mousereleased() kit.pop(d) end
  function d.mousepressed() kit.pop(d) end
  function d.keypressed() kit.pop(d) end

  return kit.push(d)
end

return M
