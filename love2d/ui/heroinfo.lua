-- Hero > Inspect: the hero info dialog.
--
-- 6c1b:0000 lists the side's heroes -- by army, last first -- and opens dialog
-- 8 over popup 2, (80, 60) 480x312, on the one nearest the cursor. With no
-- hero it does nothing. auto_ui_army_info_labels (6c1b:00f9) draws:
--
--   the strategic map with no city shields, but every hero's figure, the one
--     shown drawn last (6c1b:10ac)
--   a black outline at (308, 206) 248x129 inside a (4, 2) bevel
--   the hero's name in font 1, centred on (432, 62)
--   the hero and the rest of its stack on grey rings along y = 110 from
--     x = 304, 32 apart, eight places
--   right-aligned at x = 384: "In:" or "Near:", "Battle:", "Command:"
--     (y = 145, 165, 185), and at x = 528 "Level:" and "Exp:" (165, 185);
--     the figures 8 to the right of each (group 131)
--   "%d of %d" -- which hero this is -- at (312, 347)
--
-- and auto_ui_army_info_labels_2 (6c1b:0724) the items:
--
--   "Items Being Carried" or "Items on the Ground", centred on (432, 211)
--   a colour-3 box at (367, 236) 184x66 sunk with a (4, 2) bevel, and a raised
--     (2, 4) one at (369, 258) 180x21 round the chosen item
--   the item before, the chosen one and the one after at (376, 238 + 22 i),
--     in colour 10 on the ground, 7 carried (5 if lost)
--   what the chosen item does, centred on (340, 260) in the same colour
--   two boxes from ABITS -- (312, 311) Carried and (400, 311) Ground -- ticked
--     for the kind of item chosen, with their words at (336, 313) in colour 7
--     and (424, 313) in colour 10
--
-- Controls: 242 Done, 243/244 the next and previous hero, 245/246 the item
-- above and below, 247 Drop It (a carried item chosen), 248 Take It (one on
-- the ground), 250/249 the Carried and Ground boxes, which choose the first
-- item of their kind. Done recomputes income and checks the item quest
-- (6c1b:0b2d).

local kit      = require("ui.kit")
local armytype = require("warlords.armytype")
local combat   = require("warlords.combat")
local game     = require("warlords.game")
local hero     = require("warlords.hero")
local rules    = require("warlords.rules")
local quest    = require("warlords.quest")
local uidata   = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 312 }      -- popup 2
local MAP = { x = 80, y = 60 }
local DIALOG = 8
local DONE, NEXT, PREV, UP, DOWN, DROP, TAKE, GROUND, CARRIED =
      242, 243, 244, 245, 246, 247, 248, 249, 250
local S = 0x83                                      -- STRING.DAT group 131
local CHECKED, CLEAR = { 320, 0 }, { 320, 20 }      -- 4125:0ad4

local function itemColour(it)
  if it.status == 1 then return 10 elseif it.status == 0 then return 5 end
  return 7
end

--- What an item does, as the dialog abbreviates it (4125:0d7c-0d9c).
local function effect(it)
  local t, v = it.type, it.value or 0
  if t == rules.ITEM_STANDARD then return "com +1"
  elseif t == rules.ITEM_COMMAND then return ("com +%d"):format(v)
  elseif t == rules.ITEM_BATTLE then return ("bat +%d"):format(v)
  elseif t == rules.ITEM_FLIGHT then return "fly"
  elseif t == rules.ITEM_DOUBLE_MOVE then return "move"
  elseif t == rules.ITEM_GOLD_PER_CITY then return ("gld +%d"):format(v) end
  return ""
end

local function itemSum(h, t)
  local n = 0
  for _, it in ipairs(h.items or {}) do
    if it.type == t then n = n + (it.value or 0) end
    if t == rules.ITEM_COMMAND and it.type == rules.ITEM_STANDARD then n = n + 1 end
  end
  return n
end

--- Open the dialog, or do nothing if the side has no hero.
function M.open()
  local G = kit.G
  local heroes = {}
  for i = #G.g.armies, 1, -1 do
    local a = G.g.armies[i]
    if a.type == armytype.HERO and a.owner == G.player.index and not a.transit and a.x then
      heroes[#heroes + 1] = a
    end
  end
  if #heroes == 0 then return nil end
  local cx, cy = G.cx + 4, G.cy + 4
  local cur, best = 1, nil
  for i, h in ipairs(heroes) do
    local d = math.max(math.abs(h.x - cx), math.abs(h.y - cy))
    if not best or d < best then cur, best = i, d end
  end

  local d = { view = kit.view(DIALOG), cur = cur }

  local function list() return hero.itemsHere(G.g, heroes[d.cur]) end

  --- choose item i of the list (or none)
  local function choose(i)
    local items = list()
    d.item = (i and items[i]) and i or nil
    d.ground = d.item and items[d.item].status == 1 or false
    local st = d.view.state
    st[DONE] = uidata.NORMAL
    st[NEXT] = #heroes > 1 and uidata.NORMAL or uidata.DISABLED
    st[PREV] = st[NEXT]
    st[UP] = (d.item and d.item > 1) and uidata.NORMAL or uidata.DISABLED
    st[DOWN] = (d.item and d.item < #items) and uidata.NORMAL or uidata.DISABLED
    st[DROP] = d.item and uidata.NORMAL or uidata.DISABLED
    st[TAKE] = st[DROP]
    d.hidden = { [d.ground and DROP or TAKE] = true }
  end

  local function showHero(i)
    d.cur = (i - 1) % #heroes + 1
    choose(#list() > 0 and 1 or nil)
  end

  --- the first item of a kind: 3 carried, 1 on the ground (6c1b:0fc2, 1037)
  local function firstOf(status)
    for i, it in ipairs(list()) do
      if it.status == status then choose(i) return end
    end
  end

  function d.close()
    kit.pop(d)
    quest.event(G.g, G.player, "item", {})
  end

  function d.draw()
    local h = heroes[d.cur]
    local f = kit.font(2)
    kit.popup(R)
    G.drawStrategicMap(MAP.x, MAP.y, nil, true)
    for i, o in ipairs(heroes) do
      if i ~= d.cur then G.drawHeroFigure(MAP.x, MAP.y, o.x, o.y) end
    end
    G.drawHeroFigure(MAP.x, MAP.y, h.x, h.y)

    kit.setPal(0)
    kit.outline(308, 206, 248, 129)
    kit.bevel(307, 205, 250, 131, 4, 2)

    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), h.name or "", 432, 62)

    -- the hero, then the rest of the stack
    kit.army(armytype.HERO, G.player.index, 304, 110, 1)
    local k = 1
    for _, a in ipairs(game.armiesAt(G.g, h.x, h.y)) do
      if a ~= h and k < 8 then
        kit.army(a.type, a.owner, 304 + k * 32, 110, 1)
        k = k + 1
      end
    end
    for j = k, 7 do kit.army(nil, G.player.index, 304 + j * 32, 110, 1) end

    local inCity = game.cityAt(G.g, h.x, h.y)
    local near, nearD
    for _, c in ipairs(G.g.map.cities) do
      if game.seen(G.g, G.player, c.x, c.y) then
        local dd = math.max(math.abs(c.x - h.x), math.abs(c.y - h.y))
        if not nearD or dd < nearD then near, nearD = c, dd end
      end
    end
    local battle = itemSum(h, rules.ITEM_BATTLE)
    local command = (combat.HERO_TABLE[math.min(9, (h.strength or 0) + battle)] or 0)
                    + itemSum(h, rules.ITEM_COMMAND)
    love.graphics.setColor(1, 1, 1)
    kit.right(f, kit.text(S, inCity and 1 or 0), 384, 145)
    kit.right(f, kit.text(S, 2), 384, 165)
    kit.right(f, kit.text(S, 3), 384, 185)
    kit.right(f, kit.text(S, 4), 528, 165)
    kit.right(f, kit.text(S, 5), 528, 185)
    f.draw(near and near.name or "", 392, 145)
    f.draw(("+%d"):format(battle), 392, 165)
    f.draw(("+%d"):format(command), 392, 185)
    f.draw(("%d"):format(h.level or 1), 536, 165)
    f.draw(("%d"):format(h.experience or 0), 536, 185)
    f.draw(kit.text(S, 6):format(d.cur, #heroes), 312, 347)

    -- the items
    local items = list()
    kit.setPal(3)
    love.graphics.rectangle("fill", 367, 236, 184, 66)
    kit.bevel(367, 236, 184, 66, 4, 2)
    kit.bevel(369, 258, 180, 21, 2, 4)
    love.graphics.setColor(1, 1, 1)
    kit.centred(f, kit.text(S, d.ground and 7 or 8), 432, 211)
    if d.item then
      for row = 0, 2 do
        local it = items[d.item - 1 + row]
        if it then f.colours(itemColour(it), 0).draw(it.name or "", 376, 238 + 22 * row) end
      end
      local it = items[d.item]
      kit.centred(f.colours(itemColour(it), 0), effect(it), 340, 260)
    end
    local function box(src, x)
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.abits, love.graphics.newQuad(src[1], src[2], 24, 20, 480, 40), x, 311)
    end
    box(d.ground and CLEAR or CHECKED, 312)
    box(d.ground and CHECKED or CLEAR, 400)
    f.colours(7, 0).draw("Carried", 336, 313)
    f.colours(10, 0).draw("Ground", 424, 313)

    kit.drawControls(d.view, d.hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if not c then return end
    local items = list()
    if c.id == DONE then d.close()
    elseif c.id == NEXT then showHero(d.cur + 1)
    elseif c.id == PREV then showHero(d.cur - 1)
    elseif c.id == UP then choose(d.item - 1)
    elseif c.id == DOWN then choose(d.item + 1)
    elseif c.id == DROP and d.item then
      hero.dropItem(G.g, heroes[d.cur], items[d.item])
      choose(math.min(d.item, #list()))
    elseif c.id == TAKE and d.item then
      hero.takeItem(G.g, heroes[d.cur], items[d.item])
      choose(math.min(d.item, #list()))
    elseif c.id == GROUND then firstOf(1)
    elseif c.id == CARRIED then firstOf(3)
    end
  end

  -- Done is in both the default and the cancel lists (242)
  function d.keypressed(key)
    if key == "escape" or key == "return" or key == "kpenter" then d.close() end
  end

  showHero(cur)
  return kit.push(d)
end

return M
