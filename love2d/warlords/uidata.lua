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
  return {
    joins = uidata.joins(d .. "JOIN.DAT"),
    areas = uidata.areas(d .. "AREA.DAT"),
    buttons = uidata.buttons(d .. "BUTTON.DAT"),
    bitmaps = bitmaps,
    files = files,
  }
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
