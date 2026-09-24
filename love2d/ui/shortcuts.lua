-- Game > Shortcuts (545c:0000): which menu items the four configurable
-- buttons carry.
--
-- Popup 0, (80, 60) 480x320, dialog 14, drawn by 545c:014a. "Menu
-- Shortcuts" in font 1 centred on (320, 63); the 21 items of UDB.DAT -- a
-- u16 count, then 68-byte records: the item's command at +0, its name at
-- +2, and at +52 its button's rect in MENUBUTT.PCK, 64 wide with the lit
-- half on the right -- three to a row from (96, 100), 152 and 30 apart
-- (4125:04ce), each its button and its name 40 to the right and 5 down, lit
-- when it is on the slot being set; "Choose 4 buttons for shortcuts"
-- centred on (320, 320); and the four slots from (96, 342), 40 apart
-- (4125:0522), each its item's button or a blank one, the slot being set
-- outlined in colour 9. 316-319 pick a slot (545c:00e9), 295-315 put an item
-- on it (545c:0053), OK (294) keeps them (545c:0032 writes UDB.CUR).
--
-- The remake keeps the choice for the session rather than writing into the
-- original's files.

local kit    = require("ui.kit")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 320 }      -- popup 0
local DIALOG, OK, ITEM, SLOT = 14, 294, 295, 316
local BLANK = { 256, 84 }                           -- 4125:049e

local function u16(s, i) return s:byte(i + 1) + s:byte(i + 2) * 256 end

local function readItems(dataDir)
  local f = io.open(dataDir .. "/UDB/UDB.DAT", "rb")
  if not f then return {} end
  local s = f:read("*a")
  f:close()
  local out = {}
  for i = 0, u16(s, 0) - 1 do
    local o = 2 + 68 * i
    if o + 68 > #s then break end
    local raw = s:sub(o + 3, o + 52)
    out[i + 1] = {
      id = u16(s, o), name = raw:match("^[^%z]*"),
      x = u16(s, o + 52), y = u16(s, o + 54), w = u16(s, o + 56), h = u16(s, o + 58),
    }
  end
  return out
end

function M.open()
  local G = kit.G
  local ui = G.screen.ui
  local items = readItems(G.dataDir)
  local d = { view = kit.view(DIALOG), slot = 0 }
  local art = G.screen.art_for(uidata.SHORTCUT_BITMAP)

  local function itemIndex(id)
    for i, it in ipairs(items) do if it.id == id then return i end end
  end

  local function refresh()
    local st = d.view.state
    st[OK] = uidata.NORMAL
    for i = 0, 20 do st[ITEM + i] = uidata.NORMAL end
    for i = 0, 3 do st[SLOT + i] = uidata.NORMAL end
  end

  local function button(x, y, sx, sy)
    if not art then return end
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(art.image, love.graphics.newQuad(sx, sy, 32, 29, art.w, art.h), x, y)
  end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), "Menu Shortcuts", 320, 63)          -- 4125:055c
    local f = kit.font(2)
    local current = itemIndex(ui.shortcuts[d.slot])
    for i, it in ipairs(items) do
      local x = 96 + 152 * ((i - 1) % 3)
      local y = 100 + 30 * math.floor((i - 1) / 3)
      button(x, y, it.x + (i == current and it.w / 2 or 0), it.y)
      love.graphics.setColor(1, 1, 1)
      f.draw(it.name, x + 40, y + 5)
    end
    kit.centred(f, "Choose 4 buttons for shortcuts", 320, 320)   -- 4125:056b
    for s = 0, 3 do
      local x = 96 + 40 * s
      local it = items[itemIndex(ui.shortcuts[s]) or 0]
      if it then button(x, 342, it.x, it.y) else button(x, 342, BLANK[1], BLANK[2]) end
      if s == d.slot then
        kit.setPal(9)
        kit.outline(x, 342, 32, 29)
      end
    end
    local hidden = {}
    for i = 0, 20 do hidden[ITEM + i] = true end
    for i = 0, 3 do hidden[SLOT + i] = true end
    kit.drawControls(d.view, hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == OK then return kit.pop(d) end
    if c.id >= SLOT and c.id < SLOT + 4 then d.slot = c.id - SLOT
    elseif c.id >= ITEM and c.id < ITEM + 21 then
      local it = items[c.id - ITEM + 1]
      if it then
        ui.shortcuts[d.slot] = it.id
        ui.shortcutNames[it.id] = ui.shortcutNames[it.id] or it.name
      end
    end
    refresh()
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) end
  end

  refresh()
  return kit.push(d)
end

return M
