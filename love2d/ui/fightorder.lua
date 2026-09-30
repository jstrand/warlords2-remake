-- Order > Fight Order (6a89:0de1): the order the side's armies fight in.
--
-- Popup 11, (80, 60) 480x350, drawn by 6a89:0e4a:
--
--   "Fighting Order" (group 127) in font 1, centred on (320, 64), between two
--     of the side's big shields at (88, 64) and (512, 64)
--   "Order of combat for %s" centred on (320, 104), and the three lines of
--     help centred on 320 at y = 348, 368 and 388, in font 2
--   the 27 places, four to a row: the army type whose rank it is on a ring --
--     the side's colour for the one chosen, grey for the rest -- at
--     (88 + 120 (i % 4), 128 + 31 (i / 4)), and "%d." at (128 + 120 (i % 4),
--     134 + 31 (i / 4))
--
-- Dialog 26: OK (425, default), Cancel (426, cancel), Default (427, the
-- neutral row back), 428/429 to move the chosen army a place earlier or
-- later (greyed at the ends, and with nothing chosen), and 430-456 the
-- places. A click on a place chooses it; on the chosen one, lets it go; on
-- another, swaps the two (6a89:1379). The side's row is edited as it goes,
-- and Cancel puts back the copy taken on opening (6a89:111c).

local kit    = require("ui.kit")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 350 }      -- popup 11
local DIALOG = 26
local OK, CANCEL, DEFAULT, EARLIER, LATER, PLACE = 425, 426, 427, 428, 429, 430
local PLACES = 27
local S = 0x7f
local NEUTRAL = 8

function M.open()
  local G = kit.G
  local side = G.player
  local row = G.g.map.fightOrder[side.index]
  local backup = {}
  for t, r in pairs(row) do backup[t] = r end
  local d = { view = kit.view(DIALOG), chosen = -1 }

  local function typeAt(rank)
    for t = 0, 28 do if row[t] == rank then return t end end
  end
  local function swap(a, b)
    local ta, tb = typeAt(a), typeAt(b)
    if ta then row[ta] = b end
    if tb then row[tb] = a end
  end

  local function refresh()
    local st = d.view.state
    st[OK], st[CANCEL], st[DEFAULT] = uidata.NORMAL, uidata.NORMAL, uidata.NORMAL
    local c = d.chosen
    st[EARLIER] = (c > 0) and uidata.NORMAL or uidata.DISABLED
    st[LATER] = (c >= 0 and c < PLACES - 1) and uidata.NORMAL or uidata.DISABLED
    for i = 0, PLACES - 1 do st[PLACE + i] = uidata.NORMAL end
  end

  local function close(keep)
    if not keep then for t, r in pairs(backup) do row[t] = r end end
    kit.pop(d)
  end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(S, 0), 320, 64)
    kit.shield(side.index, 88, 64)
    kit.shield(side.index, 512, 64)
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    kit.centred(f, kit.text(S, 1):format(side.name or ""), 320, 104)
    kit.centred(f, kit.text(S, 2), 320, 348)
    kit.centred(f, kit.text(S, 3), 320, 368)
    kit.centred(f, kit.text(S, 4), 320, 388)
    for i = 0, PLACES - 1 do
      local x, y = 120 * (i % 4), 31 * math.floor(i / 4)
      local t = typeAt(i)
      if t then
        kit.army(t, side.index, 88 + x, 128 + y, i == d.chosen and side.index + 2 or 1)
      end
      love.graphics.setColor(1, 1, 1)
      f.draw(("%d."):format(i + 1), 128 + x, 134 + y)       -- 4125:0d5c
    end
    local hidden = {}
    for i = 0, PLACES - 1 do hidden[PLACE + i] = true end
    kit.drawControls(d.view, hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    local id = c.id
    if id == OK then return close(true)
    elseif id == CANCEL then return close(false)
    elseif id == DEFAULT then
      for t, r in pairs(G.g.map.fightOrder[NEUTRAL]) do row[t] = r end
      d.chosen = -1
    elseif id == EARLIER and d.chosen > 0 then
      swap(d.chosen, d.chosen - 1)
      d.chosen = d.chosen - 1
    elseif id == LATER and d.chosen >= 0 and d.chosen < PLACES - 1 then
      swap(d.chosen, d.chosen + 1)
      d.chosen = d.chosen + 1
    elseif id >= PLACE and id < PLACE + PLACES then
      local i = id - PLACE
      if d.chosen == i then d.chosen = -1
      elseif d.chosen < 0 then d.chosen = i
      else swap(d.chosen, i) d.chosen = -1 end
    end
    refresh()
  end

  -- the right button on a place (sub-ids 45-71, 6a89:1475): the type that
  -- fights there, as ARMYTYPE.DAT has it
  function d.info(sub, sx, sy)
    local t = typeAt(sub - 45)
    local rec = t and G.g.types.byId[t]
    if not rec then return false end
    require("ui.infobox").armyType(sx, sy, t, rec, side.index)
    return true
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" then close(true)
    elseif key == "escape" then close(false) end
  end

  refresh()
  return kit.push(d)
end

return M
