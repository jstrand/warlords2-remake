-- Hero > Levels (auto_ui_hero_levels, 7563:1652).
--
-- Popup 0, (80, 60) 480x320, and dialog 28: Done (469) and nothing else.
--
--   "Hero Levels" in font 1, centred on (320, 63)
--   the column heads in font 2, in the side's colours, at y = 105: Hero and
--     Level from x = 128 and 272, Exp, Needs, Str and Move centred on 392,
--     440, 488 and 536 (group 104)
--   a row per hero, 30 apart from y = 128, highest level first and, within
--     a level, last army first: the hero on a ring at (88, y) -- its side's
--     colour at the top level, grey below -- its name at (128, y + 6), its
--     title (groups 99 and 100, by sex) at (272, y + 6), and centred on the
--     columns its experience, what the next level needs (15, 30, 60, or "-"),
--     its strength and its moves
--   empty grey rings down to the sixth row

local kit      = require("ui.kit")
local armytype = require("warlords.armytype")
local uidata   = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 320 }      -- popup 0
local DIALOG, DONE = 28, 469
local S, S_MALE, S_FEMALE = 0x68, 0x63, 0x64
local NEEDS = { 15, 30, 60 }

function M.open()
  local G = kit.G
  local d = { view = kit.view(DIALOG) }
  d.view.state[DONE] = uidata.NORMAL

  local rows = {}
  for level = 4, 1, -1 do
    for i = #G.g.armies, 1, -1 do
      local a = G.g.armies[i]
      if a.type == armytype.HERO and a.owner == G.player.index and (a.level or 1) == level then
        rows[#rows + 1] = a
      end
    end
  end

  function d.draw()
    local side = G.player
    local f = kit.font(2)
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(S, 0), 320, 63)
    local head = f.colours(side.colour or 15, side.edge or 0)
    head.draw(kit.text(S, 1), 128, 105)
    head.draw(kit.text(S, 2), 272, 105)
    for k, x in ipairs({ 392, 440, 488, 536 }) do kit.centred(head, kit.text(S, k + 2), x, 105) end

    for i, h in ipairs(rows) do
      local y = 128 + 30 * (i - 1)
      local level = h.level or 1
      kit.army(armytype.HERO, side.index, 88, y, level == 4 and side.index + 2 or 1)
      love.graphics.setColor(1, 1, 1)
      f.draw(h.name or "", 128, y + 6)
      f.draw(kit.text(h.female and S_FEMALE or S_MALE, level - 1), 272, y + 6)
      kit.centred(f, ("%d"):format(h.experience or 0), 392, y + 6)
      kit.centred(f, NEEDS[level] and ("%d"):format(NEEDS[level]) or "-", 440, y + 6)
      kit.centred(f, ("%d"):format(h.strength or 0), 488, y + 6)
      kit.centred(f, ("%d"):format(h.maxMoves or 0), 536, y + 6)
    end
    for i = #rows + 1, 6 do kit.army(nil, side.index, 88, 128 + 30 * (i - 1), 1) end

    kit.drawControls(d.view)
  end

  function d.close() kit.pop(d) end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if c and c.id == DONE then d.close() end
  end

  -- Done is in both the default and the cancel lists (469)
  function d.keypressed(key)
    if key == "escape" or key == "return" or key == "kpenter" then d.close() end
  end

  return kit.push(d)
end

return M
