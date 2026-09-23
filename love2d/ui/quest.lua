-- Report > Quest (auto_ui_no_quest, 4976:0167; the text, 4976:0320).
--
-- Popup 2 with dialog 16 -- Done (330). "Quest" in font 1 centred on
-- (432, 62), and the strategic map on the left with no city shields. With no
-- quest -- or a quest whose hero is gone -- one of four lines of group 20,
-- chosen at random, centred on (432, 134). With one, SCROLL.PCK at (304, 95)
-- and on it, in font 2 black on yellow, "%s's Quest" centred on (432, 145), a
-- black rule at (360, 165) 160 long, and the quest's own lines (groups
-- 21-27) centred on x = 432 at the y each type puts them.
--
-- The map shows where to go: an orange line (colour 8) from the hero to the
-- target, both at (2x - 2, 2y - 2), and at the target a small orange shield
-- in black (828e:08cd) inside an orange box (828e:099e) -- or, for a quest to
-- slay a kind of army or a side's armies, a banner on every such stack
-- (834b:158d). The hero's figure is drawn over it all.

local kit      = require("ui.kit")
local armytype = require("warlords.armytype")
local game     = require("warlords.game")
local quest    = require("warlords.quest")
local uidata   = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 312 }      -- popup 2
local MAP = { x = 80, y = 60 }
local DIALOG, DONE = 16, 330
local COMPASS = { [0] = "north", "northeast", "east", "southeast",
                  "south", "southwest", "west", "northwest" }     -- 4125:2b7c

--- 828e:0b51: the compass point from (x1, y1) to (x2, y2).
local function direction(x1, y1, x2, y2)
  if x1 == x2 then return y1 < y2 and 4 or 0 end
  if y1 == y2 then return x1 < x2 and 2 or 6 end
  if x2 < x1 and y2 < y1 then return 7 end
  if x2 < x1 and y1 < y2 then return 5 end
  if x1 < x2 and y2 < y1 then return 1 end
  if x1 < x2 and y1 < y2 then return 3 end
  return 0
end

--- Where an item is: its carrier, the ruin it lies in, or the ground.
local function itemPlace(g, it)
  if it.status == 3 then
    for _, a in ipairs(g.armies) do
      for _, c in ipairs(a.items or {}) do if c == it and a.x then return a.x, a.y end end
    end
  elseif it.status == 2 then
    for _, s in ipairs(g.map.sites) do if s.item == it.index then return s.x, s.y end end
  end
  return it.x, it.y
end

--- The quest's lines, as (y, text) pairs, and where its target is.
local function questText(g, side, q)
  local t = function(grp, i) return kit.text(grp, i) end
  local h, lines, tx, ty = q.hero, {}, nil, nil
  local function add(y, s) lines[#lines + 1] = { y, s } end
  local function fabled(c)
    return g.map.options.hiddenMap ~= 0 and not game.seen(g, side, c.x, c.y)
  end
  if q.type == quest.SLAY_HERO then
    local foe = q.target
    add(175, t(0x15, 0)); add(195, t(0x15, 1))
    add(215, t(0x15, 2):format(g.map.sides[foe.owner + 1].name))
    add(235, foe.name or "")
    add(265, t(0x15, 3))
    tx, ty = foe.x, foe.y
    add(285, COMPASS[direction(h.x, h.y, tx, ty)])
  elseif q.type == quest.RETRIEVE_ITEM then
    add(175, t(0x16, 0)); add(195, t(0x16, 1)); add(215, q.target.name or "")
    add(245, t(0x16, 2))
    tx, ty = itemPlace(g, q.target)
    if tx then add(265, COMPASS[direction(h.x, h.y, tx, ty)]) end
  elseif q.type == quest.SLAY_TYPE then
    add(175, t(0x17, 0)); add(195, t(0x17, 1)); add(215, t(0x17, 2))
    add(235, q.target.name or "")
  elseif q.type == quest.SLAUGHTER then
    add(175, t(0x18, 0)); add(195, t(0x18, 1):format(q.required or 0))
    add(215, q.target.name or "")
    add(245, t(0x18, 2)); add(265, t(0x18, 3):format(q.done or 0))
  elseif q.type == quest.OCCUPY or q.type == quest.RAZE then
    local grp = q.type == quest.OCCUPY and 0x19 or 0x1a
    local c = q.target
    add(175, t(grp, 0))
    add(195, t(grp, fabled(c) and 2 or 1):format(c.name))
    add(215, t(grp, 3)); add(235, t(grp, 4)); add(265, t(grp, 5))
    tx, ty = c.x, c.y
    add(285, COMPASS[direction(h.x, h.y, tx, ty)])
  elseif q.type == quest.PILLAGE_GOLD then
    add(175, t(0x1b, 0)); add(195, t(0x1b, 1):format(q.required or 0))
    add(215, t(0x1b, 2)); add(235, t(0x1b, 3)); add(265, t(0x1b, 4))
    add(285, t(0x1b, 5):format(q.done or 0))
  end
  return lines, tx, ty
end

function M.open()
  local G = kit.G
  local d = { view = kit.view(DIALOG) }
  d.view.state[DONE] = uidata.NORMAL
  local q = G.player.quest
  local alive = false
  if q and q.hero then
    for _, a in ipairs(G.g.armies) do if a == q.hero and a.x then alive = true end end
  end
  if not alive then q = nil end
  d.noQuest = kit.text(0x14, G.g.rng:dice(1, 4, -1))

  local function mapMarks(tx, ty)
    local c8 = G.palette[9]
    local function pal(i) local c = G.palette[i + 1] love.graphics.setColor(c[1], c[2], c[3]) end
    love.graphics.setScissor(MAP.x, MAP.y, 224, 312)
    if q.type == quest.SLAY_TYPE or q.type == quest.SLAUGHTER then
      local iw, ih = G.atransShields:getDimensions()
      local done = {}
      for _, a in ipairs(G.g.armies) do
        local match = (q.type == quest.SLAY_TYPE and a.type == q.target.id)
                      or (q.type == quest.SLAUGHTER and a.owner == q.target.index)
        if match and a.x and a.owner ~= G.player.index and not done[a.x + a.y * 1000]
           and game.seen(G.g, G.player, a.x, a.y) then
          done[a.x + a.y * 1000] = true
          love.graphics.setColor(1, 1, 1)
          love.graphics.draw(G.atransShields,
            love.graphics.newQuad((a.owner or 8) * 16, 164, 16, 10, iw, ih),
            MAP.x + math.max(0, a.x * 2 - 1), MAP.y + math.max(0, a.y * 2 - 1))
        end
      end
    elseif tx then
      local h = q.hero
      pal(8)
      local function pix(x, y) love.graphics.rectangle("fill", MAP.x + x, MAP.y + y, 1, 1) end
      -- the line, Bresenham between the two points
      local x0, y0, x1, y1 = h.x * 2 - 2, h.y * 2 - 2, tx * 2 - 2, ty * 2 - 2
      local dx, dy = math.abs(x1 - x0), -math.abs(y1 - y0)
      local sx, sy = x0 < x1 and 1 or -1, y0 < y1 and 1 or -1
      local err = dx + dy
      while true do
        pix(x0, y0)
        if x0 == x1 and y0 == y1 then break end
        local e2 = 2 * err
        if e2 >= dy then err = err + dy x0 = x0 + sx end
        if e2 <= dx then err = err + dx y0 = y0 + sy end
      end
      -- the small shield at the target, and the box round it
      local bx, by = MAP.x + tx * 2, MAP.y + ty * 2 - 1
      pal(8)
      love.graphics.rectangle("fill", bx, by, 4, 5)
      pal(0)
      love.graphics.rectangle("fill", bx - 1, by, 6, 1)
      love.graphics.rectangle("fill", bx, ty * 2 + 4 + MAP.y, 4, 1)
      love.graphics.rectangle("fill", bx - 1, by, 1, 5)
      love.graphics.rectangle("fill", bx + 4, by, 1, 5)
      pal(8)
      kit.outline(MAP.x + tx * 2 - 2, MAP.y + ty * 2 - 2, 8, 8)
    end
    love.graphics.setScissor()
  end

  function d.draw()
    kit.popup(R)
    G.drawStrategicMap(MAP.x, MAP.y, nil, true)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), "Quest", 432, 62)                 -- 4125:00b4
    if not q then
      kit.centred(kit.font(2), d.noQuest, 432, 134)
    else
      love.graphics.draw(G.scrollPic, 304, 95)
      local f = kit.font(2).colours(0, 7)
      kit.centred(f, ("%s's Quest"):format(q.hero.name or ""), 432, 145)
      kit.setPal(0)
      love.graphics.rectangle("fill", 360, 165, 160, 1)
      local lines, tx, ty = questText(G.g, G.player, q)
      for _, l in ipairs(lines) do kit.centred(f, l[2], 432, l[1]) end
      mapMarks(tx, ty)
      G.drawHeroFigure(MAP.x, MAP.y, q.hero.x, q.hero.y)
    end
    kit.drawControls(d.view)
  end

  function d.close() kit.pop(d) end
  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if c and c.id == DONE then d.close() end
  end
  function d.keypressed(key)
    if key == "escape" or key == "return" or key == "kpenter" then d.close() end
  end

  return kit.push(d)
end

return M
