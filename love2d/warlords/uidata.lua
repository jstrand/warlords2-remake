-- The original's screen layout, read from its own data files.
--
-- docs/formats/screens.md. The interface was never compiled into the
-- executable: DATA/JOIN.DAT names, for each dialog, one group of controls in
-- BUTTON.DAT and one set of clickable regions in AREA.DAT. DATA/FILE.DAT
-- group 3 turns a control's bitmap id into a .pck file name.
--
-- Pure Lua: this module never touches love.*, so the layout can be loaded and
-- checked without a window.

local uidata = {}

local function read(path)
  local f = assert(io.open(path, "rb"), "cannot open: " .. path)
  local s = f:read("*a")
  f:close()
  return s
end

local function u16(s, i)                       -- i is 1-based
  local a, b = s:byte(i, i + 1)
  return a + b * 256
end

--------------------------------------------------------------- STRING.DAT form

--- Read a file in STRING.DAT's layout: an index of {offset, count} pairs, a
--- region of per-string offsets, then NUL-terminated text. FILE.DAT uses it
--- too. Returns an array of groups, each an array of strings.
-- docs/formats/string.md.
function uidata.strings(path)
  local s = read(path)
  local nGroups = u16(s, 1) / 4               -- the first offset IS the index size
  local groups, ptr = {}, {}
  for g = 1, nGroups do
    local off, count = u16(s, (g - 1) * 4 + 1), u16(s, (g - 1) * 4 + 3)
    ptr[g] = { off = off, count = count }
  end
  for g = 1, nGroups do
    local out = {}
    for i = 1, ptr[g].count do
      local at = u16(s, ptr[g].off + (i - 1) * 2 + 1)
      local stop = s:find("\0", at + 1, true) or (#s + 1)
      out[i] = s:sub(at + 1, stop - 1)
    end
    groups[g] = out
  end
  return groups
end

--------------------------------------------------------------------- the files

local MAGIC = 0x7d00

--- DATA/JOIN.DAT: 36 entries of {dialog id, BUTTON group, AREA screen}.
function uidata.joins(path)
  local s = read(path)
  local out = {}
  for i = 0, math.floor(#s / 6) - 1 do
    local at = i * 6 + 1
    out[u16(s, at)] = { button = u16(s, at + 2), area = u16(s, at + 4) }
  end
  return out
end

--- DATA/AREA.DAT: screens of clickable regions, 14 bytes each.
function uidata.areas(path)
  local s = read(path)
  local out, at = {}, 1
  while at + 13 <= #s do
    assert(u16(s, at) == MAGIC, "AREA.DAT: lost the record boundary")
    local id, enabled, count = u16(s, at + 2), u16(s, at + 4), u16(s, at + 6)
    local regions = {}
    for i = 1, count do
      local r = at + 14 + (i - 1) * 14
      regions[i] = {
        id = u16(s, r), x = u16(s, r + 4), y = u16(s, r + 6),
        w = u16(s, r + 8), h = u16(s, r + 10),
      }
    end
    out[id] = { id = id, enabled = enabled, regions = regions }
    at = at + 14 + count * 14
  end
  return out
end

-- A control's three source rects are indexed arithmetically by its state, so
-- state n is simply the nth pair. State 1 is the resting look: 0 is the lit
-- one and 2 the disabled one (docs/formats/screens.md).
uidata.ACTIVE, uidata.NORMAL, uidata.DISABLED = 0, 1, 2

--- DATA/BUTTON.DAT: groups of controls, 33 bytes each.
function uidata.buttons(path)
  local s = read(path)
  local out, at = {}, 1
  while at + 32 <= #s do
    assert(u16(s, at) == MAGIC, "BUTTON.DAT: lost the record boundary")
    local id, count = u16(s, at + 2), u16(s, at + 4)
    local controls = {}
    for i = 1, count do
      local c = at + 33 + (i - 1) * 33
      local src = {}
      for st = 0, 2 do
        src[st] = { x = u16(s, c + 19 + st * 4), y = u16(s, c + 21 + st * 4) }
      end
      controls[i] = {
        id = u16(s, c), x = u16(s, c + 6), y = u16(s, c + 8),
        w = s:byte(c + 10), h = s:byte(c + 11),
        src = src, bitmap = u16(s, c + 31),
      }
    end
    out[id] = { id = id, controls = controls }
    at = at + 33 + count * 33
  end
  return out
end

--------------------------------------------------------------- the shortcuts

--- UDB/UDB.DAT: the menu items that may be put on the four configurable
--- buttons. 21 records of 68 bytes:
---
---     +0   u16       unknown flag: 21 on the first record, 29 on the rest
---     +2   u16       menu item id
---     +4   char[50]  NUL-terminated name
---     +54  u16 x, u16 y, u16 w = 64, u16 h = 29   the resting icon
---     +62  u16 x, u16 y, u16 w = 32               the greyed-out icon
---
--- The rects are into MENUBUTT.PCK, which is a 10 x 7 grid of 32 x 29 cells
--- (28 apart: neighbours share a border row). The first rect is 64 wide
--- because it spans a *pair* -- the resting icon and, 32 px to its right, the
--- lit one -- so the three states are at (x, y), (x + 32, y) and the greyed
--- pair. All 21 records together name 63 distinct cells, three each.
---
--- Returns {id -> {name = , src = {[0] = lit, [1] = resting, [2] = greyed},
--- w = , h = }}, with src keyed the way BUTTON.DAT keys its states.
function uidata.shortcutItems(path)
  local s = read(path)
  local out = {}
  local i = 0
  while (i + 1) * 68 <= #s do
    local at = i * 68 + 1
    local id = u16(s, at + 2)
    local stop = s:find("\0", at + 4, true) or (at + 4)
    local name = s:sub(at + 4, stop - 1)
    local x, y = u16(s, at + 54), u16(s, at + 56)
    if #name > 0 then
      out[id] = {
        name = name,
        w = u16(s, at + 66), h = u16(s, at + 60),
        src = {
          [uidata.ACTIVE]   = { x = x + u16(s, at + 66), y = y },
          [uidata.NORMAL]   = { x = x, y = y },
          [uidata.DISABLED] = { x = u16(s, at + 62), y = u16(s, at + 64) },
        },
      }
    end
    i = i + 1
  end
  return out
end

--- The same file as {id -> name}.
function uidata.shortcutNames(path)
  local out = {}
  for id, item in pairs(uidata.shortcutItems(path)) do out[id] = item.name end
  return out
end

--- UDB/UDB.CUR: which menu item sits on each of the four buttons.
-- The shipped default is Search, Move All, Heroes, End Turn.
function uidata.shortcuts(path)
  local s = read(path)
  local out = {}
  for i = 0, math.floor(#s / 2) - 1 do
    out[i] = u16(s, i * 2 + 1)
  end
  return out
end

-- The four configurable buttons, in order. Each runs its assigned menu item
-- through the ordinary command dispatcher: 545c:0072 reads the assignment,
-- 7ae8:0000 turns it into a command code and 17be:0064 runs it -- the same
-- path a key press takes. docs/re/ui.md.
uidata.SHORTCUT_FIRST, uidata.SHORTCUT_COUNT = 179, 4

-- Their art is not the blank in BUTTON.PCK the layout points at: 545c:030a
-- paints each button a second time from bitmap 43 -- MENUBUTT.PCK -- using
-- the rect the assigned item carries in UDB.DAT.
uidata.SHORTCUT_BITMAP = 43

--------------------------------------------------------------------- assembled

--- Load every layout file under `dataDir`. Returns a table with `joins`,
--- `areas`, `buttons` and `bitmaps` (bitmap id -> lower-case .pck name).
function uidata.load(dataDir)
  local d = dataDir .. "/DATA/"
  local files = uidata.strings(d .. "FILE.DAT")
  local bitmaps = {}
  for i, name in ipairs(files[4]) do          -- group 3, 1-indexed here
    bitmaps[i - 1] = name:lower()
  end
  -- The user's own shortcut assignments, if the game has a UDB directory.
  local shortcuts, shortcutItems = {}, {}
  local ok = pcall(function()
    shortcutItems = uidata.shortcutItems(dataDir .. "/UDB/UDB.DAT")
    shortcuts = uidata.shortcuts(dataDir .. "/UDB/UDB.CUR")
  end)
  if not ok then shortcuts, shortcutItems = {}, {} end
  local shortcutNames = {}
  for id, item in pairs(shortcutItems) do shortcutNames[id] = item.name end

  return {
    shortcuts = shortcuts,
    shortcutItems = shortcutItems,
    shortcutNames = shortcutNames,
    joins = uidata.joins(d .. "JOIN.DAT"),
    areas = uidata.areas(d .. "AREA.DAT"),
    buttons = uidata.buttons(d .. "BUTTON.DAT"),
    bitmaps = bitmaps,
    files = files,
    -- DATA/STRING.DAT: the game's whole text corpus, 169 groups. A group is
    -- one dialog or enumeration; get_string(group, i) in the executable is
    -- uidata.text(ui, group, i) here, with the same zero-based numbering.
    text = uidata.strings(d .. "STRING.DAT"),
  }
end

--- One string, numbered as the executable numbers them (both zero-based).
function uidata.text(ui, group, index)
  local g = ui.text[group + 1]
  return g and g[index + 1] or ""
end

--- The controls and regions of one dialog, following JOIN.DAT.
function uidata.dialog(ui, id)
  local join = ui.joins[id]
  if not join then return nil end
  local buttons, area = ui.buttons[join.button], ui.areas[join.area]
  return {
    id = id,
    controls = buttons and buttons.controls or {},
    regions = area and area.regions or {},
    enabled = area and area.enabled or 0,
  }
end

uidata.MAIN_SCREEN = 0                         -- dialog 0: the in-game screen

return uidata
