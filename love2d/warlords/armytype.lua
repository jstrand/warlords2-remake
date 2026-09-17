-- TERRAIN0/ARMYTYPE.DAT -- the 29 army types.
-- See docs/formats/armytype.md. Records are keyed by their type id (+0), not
-- by position in the file: the file is in display order.

local armytype = {}

local COUNT, STRIDE = 29, 62
local N_BONUS = 15

-- bonus field offsets worth naming (docs/formats/armytype.md, docs/rules.md)
armytype.SIEGE       = 52   -- value 1 = siege ability (+2 strength vs cities)
armytype.FLIES       = 54
armytype.WOODS_MOVE  = 56
armytype.HILLS_MOVE  = 58
armytype.BOAT        = 60

armytype.NAVY = 5           -- removed from every city's production at game start
armytype.HERO = 28
armytype.SCOUTS = 11        -- placeholder garrison when Neutral Cities is off

local function u16(s, off)
  return s:byte(off + 1) + s:byte(off + 2) * 256
end

local function i16(s, off)
  local v = u16(s, off)
  return v >= 0x8000 and v - 0x10000 or v
end

local function cstr(s, off, len)
  local raw = s:sub(off + 1, off + len)
  local z = raw:find("\0", 1, true)
  return z and raw:sub(1, z - 1) or raw
end

--- Load ARMYTYPE.DAT. Returns { byId = {[id]=rec}, list = {rec,...} } with the
--- list in file (display) order.
function armytype.load(path)
  local f = assert(io.open(path, "rb"), "cannot open: " .. path)
  local s = f:read("*a")
  f:close()
  assert(#s == COUNT * STRIDE, ("unexpected %s size: %d"):format(path, #s))

  local byId, list = {}, {}
  for i = 0, COUNT - 1 do
    local o = i * STRIDE
    local bonus = {}
    for b = 0, N_BONUS - 1 do bonus[32 + b * 2] = u16(s, o + 32 + b * 2) end
    local a = {
      id = u16(s, o),               -- also the sprite index
      name = cstr(s, o + 2, 16),
      strength = u16(s, o + 22),
      time = u16(s, o + 24),
      cost = u16(s, o + 26),        -- per-turn upkeep basis
      move = u16(s, o + 28),
      price = i16(s, o + 30),       -- negative: can never be bought
      bonus = bonus,
    }
    a.flies = bonus[armytype.FLIES] ~= 0
    a.siege = bonus[armytype.SIEGE] == 1
    a.boat = bonus[armytype.BOAT] ~= 0
    a.woodsMove = bonus[armytype.WOODS_MOVE] ~= 0
    a.hillsMove = bonus[armytype.HILLS_MOVE] ~= 0
    byId[a.id] = a
    list[#list + 1] = a
  end
  return { byId = byId, list = list }
end

return armytype
