-- What the right button shows off the map: a box that stays up while the
-- button is held, and goes when it comes up (740d:11cf).
--
-- Two boards, both blitted through the mask 4125:546d so that colour 1 lets
-- what is behind show through:
--
--   POPUP.PCK (bitmap 29), 256x75: two lines, as the map's tile box has them
--     (740d:1201, 740d:1158) -- the first in colour 7, the second in white,
--     both font 2 centred on the box's middle less 8, at y + 11 and y + 35.
--     740d:131a centres it on the pointer, x rounded to the byte grid, kept
--     within x in [128, 512] and y in [37, 441].
--   POPUP3.PCK (bitmap 40), 240x128: an army (ui_army_info, 740d:0626) or an
--     army type (ui_army_type_info, 740d:032a). 740d:1486 centres it the
--     same way, kept within x in [120, 520] and y in [64, 414]. On it, in
--     font 2 white edged in black: the name centred on (x + 112, y + 8), the
--     army on the grey ring at (x + 96, y + 31) with a non-hero's medals
--     round it, four numbers -- right-aligned ending at x + 104 and from
--     x + 120, at y + 66 and y + 86 -- and at (x + 96, y + 103) ABITS' icon
--     for how the type moves: flying, woods and hills, woods, or hills.
--
-- A right-button press on a control looks the control up in
-- HELP\WARLORD2.HLP (54bd:0000): the first record with its id, and when the
-- record's sub-id is 0 its title and description are the two lines. A
-- sub-id otherwise says what the control stands for (54bd:00df), and the
-- dialog it is on answers through `d.info(sub, sx, sy)`:
--
--   1-4    the city dialog's production slots: the type in that slot
--   5-24   Build Production's list: the nth type for sale
--   33-36  the four configurable buttons: "- User-Defined Button -" and
--          the name of the menu item on it
--   37-44  View > Stack's rows: the army in that row
--   45-71  Fight Order's places: the type fighting in that place
--
-- Controls 224-241, the bottom bar's, carry sub-id 65535 and show nothing:
-- the bar's region answers for them (89e0:0ad9).

local kit    = require("ui.kit")

local M = {}

local LINES = { bitmap = 29, file = "POPUP.PCK",  w = 256, h = 75 }
local ARMY  = { bitmap = 40, file = "POPUP3.PCK", w = 240, h = 128 }

-- ABITS' 32 x 10 icons for how a type moves (740d:0626), and the medals'
-- 8 x 8s (4125:2fa6) at the four places round the army
local MOVE_ICON = { fly = { 184, 30 }, both = { 216, 30 }, woods = { 248, 30 }, hills = { 152, 30 } }
local MEDALS = { { 424, 22 }, { 432, 22 }, { 440, 22 }, { 456, 32 } }
local MEDAL_AT = { { 128, 36 }, { 88, 36 }, { 128, 45 }, { 88, 45 } }

--- A board, loaded once and keyed on colour 1.
function M.board(which)
  local G = kit.G
  G.tileBoards = G.tileBoards or {}
  if not G.tileBoards[which.bitmap] then
    local pck = require("warlords.pck")
    local img, w, h = pck.toImage(G.dataDir .. "/PICS/" .. which.file, G.palette, 1)
    G.tileBoards[which.bitmap] = { image = img, w = w, h = h }
  end
  return G.tileBoards[which.bitmap]
end
M.LINES, M.ARMY, M.POPUP2 = LINES, ARMY, { bitmap = 36, file = "POPUP2.PCK", w = 256, h = 75 }

--- Where a board goes for a point on the screen (740d:131a, 740d:1486):
--- centred on it, x on the byte grid, kept on the screen with two rows to
--- spare at the foot; given back in the dialogs' frame, where it is drawn.
function M.place(which, sx, sy)
  local L = kit.G.layout
  local hw, hh = math.floor(which.w / 2), math.floor(which.h / 2)
  local cx = math.floor((math.floor(sx) + 4) / 8) * 8
  cx = math.max(hw, math.min(L.w - hw, cx))
  local cy = math.max(hh, math.min(L.h - 2 - hh, math.floor(sy)))
  return cx - hw - L.dialog.x, cy - hh - L.dialog.y
end

--- Put a box up. `draw(x, y)` fills it in at the board's corner.
local function show(which, sx, sy, draw, what)
  local bx, by = M.place(which, sx, sy)
  local d = { infobox = what }          -- what it shows, for the tests
  function d.draw()
    local b = M.board(which)
    love.graphics.setColor(1, 1, 1)
    if b then
      love.graphics.draw(b.image, love.graphics.newQuad(0, 0, which.w, which.h, b.w, b.h), bx, by)
    end
    love.graphics.setColor(1, 1, 1)
    draw(bx, by)
  end
  function d.mousereleased() kit.pop(d) end
  function d.mousepressed() kit.pop(d) end
  function d.keypressed() kit.pop(d) end
  return kit.push(d)
end

--- Two lines (740d:1201 and 740d:1158): a title in colour 7, then white.
function M.lines(sx, sy, title, text)
  local f = kit.font(2)
  return show(LINES, sx, sy, function(x, y)
    kit.centred(f.colours(7, 0), title or "", x + LINES.w / 2 - 8, y + 11)
    kit.centred(f, text or "", x + LINES.w / 2 - 8, y + 35)
  end, { title = title, text = text })
end

--- The name, the army and how its type moves, which both army boxes share.
local function armyBoard(x, y, name, typeId, side, medals)
  local G = kit.G
  local f = kit.font(2)
  kit.centred(f, name or "", x + 112, y + 8)
  kit.army(typeId, side, x + 96, y + 31, 1)
  local aw, ah = G.abits:getDimensions()
  local function abits(src, w, h, ax, ay)
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.abits, love.graphics.newQuad(src[1], src[2], w, h, aw, ah), ax, ay)
  end
  for m = 1, math.min(4, medals or 0) do
    abits(MEDALS[m], 8, 8, x + MEDAL_AT[m][1], y + MEDAL_AT[m][2])
  end
  local t = G.g.types.byId[typeId]
  local src
  if t and t.flies then src = MOVE_ICON.fly
  elseif t and t.woodsMove and t.hillsMove then src = MOVE_ICON.both
  elseif t and t.woodsMove then src = MOVE_ICON.woods
  elseif t and t.hillsMove then src = MOVE_ICON.hills end
  if src then abits(src, 32, 10, x + 96, y + 103) end
end

local function numbers(x, y, a, b, c, d)
  local f = kit.font(2)
  love.graphics.setColor(1, 1, 1)
  kit.right(f, a, x + 104, y + 66)
  f.draw(b, x + 120, y + 66)
  kit.right(f, c, x + 104, y + 86)
  f.draw(d, x + 120, y + 86)
end

--- An army (ui_army_info, 740d:0626): a hero by its name, anything else by
--- its type's; its strength, its full movement, what is left of it this
--- turn, and its upkeep.
function M.army(sx, sy, a)
  local G = kit.G
  local armytype = require("warlords.armytype")
  local t = G.g.types.byId[a.type] or {}
  local hero = a.type == armytype.HERO
  return show(ARMY, sx, sy, function(x, y)
    armyBoard(x, y, hero and a.name or t.name, a.type, G.player.index,
              not hero and a.medals or 0)
    numbers(x, y, ("Strength: %d"):format(a.strength or 0),     -- 4125:10a8
                  ("Movement: %d"):format(a.maxMoves or 0),     -- 4125:10b5
                  ("Remain: %d"):format(a.moves or 0),          -- 4125:10c2
                  ("Upkeep: %d"):format(a.upkeep or 0))         -- 4125:10cd
  end, { army = a })
end

--- An army type (ui_army_type_info, 740d:032a): what one of it would be --
--- strength and movement, the turns it takes to build and what it costs.
--- `s` has the four numbers, a city's production slot or an ARMYTYPE
--- record; `side` is whose colours it is drawn in.
function M.armyType(sx, sy, typeId, s, side)
  local G = kit.G
  local t = G.g.types.byId[typeId] or {}
  return show(ARMY, sx, sy, function(x, y)
    armyBoard(x, y, t.name, typeId, side or G.player.index, 0)
    numbers(x, y, ("Strength: %d"):format(s.strength or 0),     -- 4125:107c
                  ("Movement: %d"):format(s.move or 0),         -- 4125:1089
                  ("Time: %d"):format(s.time or 0),             -- 4125:1096
                  ("Cost: %d"):format(s.cost or 0))             -- 4125:109f
  end, { type = typeId })
end

--------------------------------------------------------------------- the help file

--- HELP\WARLORD2.HLP (docs/formats/hlp.md): a u16 count, then 104-byte
--- records -- u16 id, u16 sub-id, the title and the description, 50 bytes
--- each. Only the first record with an id counts, as 54bd:0000 reads it.
local function helpFile()
  local G = kit.G
  if G.helpRecords then return G.helpRecords end
  local out = {}
  local f = io.open(G.dataDir .. "/HELP/WARLORD2.HLP", "rb")
  if f then
    local s = f:read("*a")
    f:close()
    local function u16(at) return s:byte(at) + s:byte(at + 1) * 256 end
    local function str(at)
      local raw = s:sub(at, at + 49)
      return raw:sub(1, (raw:find("\0", 1, true) or 51) - 1)
    end
    for i = 0, u16(1) - 1 do
      local at = 3 + i * 104
      if at + 103 > #s then break end
      local id = u16(at)
      if not out[id] then
        out[id] = { sub = u16(at + 2), title = str(at + 4), text = str(at + 54) }
      end
    end
  end
  G.helpRecords = out
  return out
end
M.helpFile = helpFile

--- The help for control `id`, at a point on the screen, asking `d` -- the
--- dialog it is on, or nil for the main screen -- about the controls that
--- stand for something. True when a box went up.
function M.control(sx, sy, id, d)
  local rec = helpFile()[id]
  if not rec then return false end
  if rec.sub == 0 then
    M.lines(sx, sy, rec.title, rec.text)
    return true
  end
  local sub = rec.sub
  if sub >= 33 and sub <= 36 then
    -- the configurable buttons (545c:03f8): the menu item each one runs
    local ui = kit.G.screen.ui
    local item = ui.shortcuts and ui.shortcuts[sub - 33]
    local name = item and ui.shortcutNames and ui.shortcutNames[item] or ""
    M.lines(sx, sy, "- User-Defined Button -", name)            -- 4125:05e6
    return true
  end
  if d and d.info then return d.info(sub, sx, sy) and true or false end
  return false
end

--- The control on a dialog's view under a point of its frame, disabled ones
--- included -- a greyed button still says what it is (18a9:0756) -- but not
--- those the dialog has taken off its face.
function M.controlAt(view, x, y, hidden)
  local found
  for _, c in ipairs(view.dialog.controls) do
    if c.w > 0 and c.h > 0 and not (hidden and hidden[c.id])
       and x >= c.x and x < c.x + c.w and y >= c.y and y < c.y + c.h then
      found = c                    -- 18a9:0756 walks back to front
    end
  end
  return found
end

return M
