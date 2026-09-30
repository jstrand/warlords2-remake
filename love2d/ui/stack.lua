-- View > Stack (89e0:0c9c): the selected stack laid out at length, to be
-- grouped the way the bar under the map groups it.
--
-- It works on a copy of the bar's slots (89e0:0d30) -- the same groups, the
-- same group that moves, the same marks -- and its clicks are the bar's:
-- an army (336-343, 89e0:157d) joins the group that moves or leaves it, a
-- mark (344-351, 89e0:166f) makes its group the one, Group (334) puts the
-- lot together and Ungroup (335) breaks it up. OK (332, default) keeps it
-- (89e0:000a), Cancel (333, cancel) throws it away. Group and Ungroup are
-- greyed for a stack of one, and the rows past the stack.
--
-- Popup 0, (80, 60) 480x320, drawn by 89e0:0e85: the column heads,
-- STACK.PCK's (0, 0) 400x23, at (80, 62) with "(max +%d)" -- the scenario's
-- combat cap -- at (480, 66); and eight rows 30 apart from (112, 90):
--
--   the mark, ABITS.PCK's cross (448, 0) or tick (448, 16), at (80, y + 5)
--   the army on a ring of the colour (side + group) mod 8 + 2, its shadow
--     when it is not in the group that moves
--   a non-hero's medals from (x + 32, y + 8), two to a column
--   name at x + 48, strength at x + 168, and for the group that moves its
--     strength in a fight here (89e0:1b9c) as "(%d)" at x + 184; moves at
--     x + 232, all at y + 5
--   how it moves at (x + 256, y + 8), as Army Bonus shows it -- a hero with a
--     flying item flies, a boat at sea shows the boat -- and the bonus at
--     (x + 304, y + 5)
--   and an empty grey ring for every row past the stack

local kit      = require("ui.kit")
local armybonus = require("ui.armybonus")
local armytype = require("warlords.armytype")
local combat   = require("warlords.combat")
local rules    = require("warlords.rules")
local slotsMod = require("warlords.slots")
local uidata   = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 320 }      -- popup 0
local DIALOG, OK, CANCEL, GROUP, UNGROUP, ARMY, MARK = 18, 332, 333, 334, 335, 336, 344
local ROWS = 8
local MEDALS = { { 424, 22 }, { 432, 22 }, { 440, 22 }, { 456, 32 } }   -- 4125:2fa6

local function copy(s)
  local c = { n = s.n, side = s.side, army = {}, group = {}, inGroup = {}, mark = {} }
  for i = 1, s.n do
    c.army[i], c.group[i], c.inGroup[i], c.mark[i] = s.army[i], s.group[i], s.inGroup[i], s.mark[i]
  end
  c.active = s.active
  return c
end

local function flies(a, t)
  if t.flies then return true end
  if a.type ~= armytype.HERO then return false end
  for _, it in ipairs(a.items or {}) do
    if it.type == rules.ITEM_FLIGHT then return true end
  end
  return false
end

function M.open()
  local G = kit.G
  if not G.selection then return nil end
  local side = G.player
  local d = { view = kit.view(DIALOG), s = copy(G.selection.slots) }

  local function refresh()
    local st, n = d.view.state, d.s.n
    st[OK], st[CANCEL] = uidata.NORMAL, uidata.NORMAL
    st[GROUP] = n > 1 and uidata.NORMAL or uidata.DISABLED
    st[UNGROUP] = st[GROUP]
    for i = 0, ROWS - 1 do
      st[ARMY + i] = i < n and uidata.NORMAL or uidata.DISABLED
      st[MARK + i] = st[ARMY + i]
    end
  end

  function d.draw()
    local s = d.s
    kit.popup(R)
    local head = G.screen.art_for(46)
    love.graphics.setColor(1, 1, 1)
    if head then
      love.graphics.draw(head.image, love.graphics.newQuad(0, 0, 400, 23, head.w, head.h), 80, 62)
    end
    local f = kit.font(2)
    f.draw(("(max +%d)"):format(G.g.map.combatCap or 5), 480, 66)      -- 4125:320d
    local aw, ah = G.abits:getDimensions()
    local function abits(sx, sy, w, h, x, y)
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(G.abits, love.graphics.newQuad(sx, sy, w, h, aw, ah), x, y)
    end
    local moving = {}
    for i = 1, s.n do if s.inGroup[i] then moving[#moving + 1] = s.army[i] end end
    local a1 = s.army[1]
    local fight = a1 and combat.stackStrengths(G.g, moving, a1.x, a1.y) or {}

    for i = 1, ROWS do
      local x, y = 112, 60 + 30 * i
      local a = s.army[i]
      if i <= s.n and a then
        local t = G.g.types.byId[a.type]
        if s.mark[i] then abits(448, s.mark[i] == slotsMod.TICK and 16 or 0, 32, 16, x - 32, y + 5) end
        kit.army(a.type, side.index, x, y, (side.index + s.group[i]) % 8 + 2, not s.inGroup[i])
        if a.type ~= armytype.HERO and (t.bonus[48] or 0) == 0 then
          for m = 0, math.min(#MEDALS, a.medals or 0) - 1 do
            abits(MEDALS[m + 1][1], MEDALS[m + 1][2], 8, 8,
                  x + 32 + math.floor(m / 2) * 8, y + 8 + (m % 2) * 9)
          end
        end
        love.graphics.setColor(1, 1, 1)
        f.draw(a.type == armytype.HERO and (a.name or "") or (t.name or ""), x + 48, y + 5)
        local str = a.strength or 0
        if a.type == armytype.HERO then str = str + combat.battleItems(a) end
        f.draw(("%d"):format(math.min(9, str)), x + 168, y + 5)
        if s.inGroup[i] and fight[a] then f.draw(("(%d)"):format(fight[a]), x + 184, y + 5) end
        f.draw(("%d"):format(a.moves or 0), x + 232, y + 5)
        local src
        if flies(a, t) then src = { 184, 30 }
        elseif a.atSea then src = { 424, 30 }
        elseif t.woodsMove and t.hillsMove then src = { 216, 30 }
        elseif t.woodsMove then src = { 248, 30 }
        elseif t.hillsMove then src = { 152, 30 } end
        if src then abits(src[1], src[2], 32, 10, x + 256, y + 8) end
        love.graphics.setColor(1, 1, 1)
        f.draw(armybonus.bonusText(t, a), x + 304, y + 5)
      else
        kit.army(nil, side.index, x, y, 1)
      end
    end
    local hidden = {}
    for i = 0, ROWS - 1 do hidden[ARMY + i], hidden[MARK + i] = true, true end
    kit.drawControls(d.view, hidden)
  end

  local function keep()
    kit.pop(d)
    G.selection.slots = d.s
    if G.afterSlotChange then G.afterSlotChange() end
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c or d.view.state[c.id] == uidata.DISABLED then return end
    local id = c.id
    if id == OK then return keep()
    elseif id == CANCEL then return kit.pop(d)
    elseif id == GROUP then slotsMod.all(d.s, G.g)
    elseif id == UNGROUP then slotsMod.single(d.s, G.g)
    elseif id >= ARMY and id < ARMY + ROWS then slotsMod.toggle(d.s, G.g, id - ARMY + 1)
    elseif id >= MARK and id < MARK + ROWS then slotsMod.pickGroup(d.s, G.g, id - MARK + 1)
    end
    refresh()
  end

  -- the right button on a row (sub-ids 37-44, 89e0:1747): its army, or for a
  -- row past the stack "Select Army"
  function d.info(sub, sx, sy)
    local infobox = require("ui.infobox")
    local n = sub - 37
    if n < d.s.n then infobox.army(sx, sy, d.s.army[n + 1])
    else infobox.lines(sx, sy, "Select Army", "Select armies when present") end
    return true
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" then keep()
    elseif key == "escape" then kit.pop(d) end
  end

  refresh()
  return kit.push(d)
end

return M
