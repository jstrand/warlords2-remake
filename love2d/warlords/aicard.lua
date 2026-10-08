-- The computer players' character cards: CARDS/K|L|W nnn.CRD and .DSC.
--
-- docs/formats/crd.md. A computer side's level picks the deck (K Knight,
-- L Lord, W Warlord) and .SCN 0x00e0 + 2*side the card; at game start the
-- card overwrites the level's built-in AI settings (59bf:0d7b).

local aicard = {}

aicard.LETTERS = { [0] = "K", "L", "W" }       -- "KLW", 7bab:21c1
aicard.RECORD = 0x67                           -- 103 bytes

local function i16(s, off)
  local v = s:byte(off + 1) + s:byte(off + 2) * 256
  return v >= 0x8000 and v - 0x10000 or v
end

--- The file name of a card: CARDS\%c%03d.%s.
function aicard.path(dataDir, level, number, ext)
  return ("%s/CARDS/%s%03d.%s"):format(dataDir, aicard.LETTERS[level] or "W",
                                       number or 0, ext or "CRD")
end

--- Read a card. Returns the record's fields, named for the AI data they fill,
--- or nil when the file is missing or short.
function aicard.load(dataDir, level, number)
  local f = io.open(aicard.path(dataDir, level, number, "CRD"), "rb")
  if not f then return nil end
  local s = f:read("*a")
  f:close()
  if #s < aicard.RECORD then return nil end
  local fightOrder = {}
  for t = 0, 28 do fightOrder[t] = s:byte(0x0e + t + 1) end
  return {
    groups = i16(s, 0x08),          -- +0x24a
    bold = i16(s, 0x0a),            -- +0x0c bit 0 (558d:0851)
    fightOrder = fightOrder,        -- .SCN 0x60b + 29*side
    cautious = i16(s, 0x2b),        -- +0x48
    rebuildType = i16(s, 0x2d),     -- +0x10
    rebuildTypeRich = i16(s, 0x2f), -- +0x12
    rebuildLimit = i16(s, 0x31),    -- +0x0a
    raze = i16(s, 0x45),            -- +0x26
    sack = i16(s, 0x47),            -- +0x28
    pillage = i16(s, 0x49),         -- +0x2a
    perCity = i16(s, 0x4b),         -- +0x2c
    bonusHuman = i16(s, 0x4d),      -- +0x2e
    bonusKnight = i16(s, 0x4f),     -- +0x34
    bonusLord = i16(s, 0x51),       -- +0x32
    bonusWarlord = i16(s, 0x53),    -- +0x30
    dieHuman = i16(s, 0x55),        -- +0x18 = 1dv
    dieKnight = i16(s, 0x57),       -- +0x1c = 1dv
    dieLord = i16(s, 0x59),         -- +0x1a = 1dv
    dieWarlord = i16(s, 0x5b),      -- +0x1e = 1dv
    poor = i16(s, 0x5f),            -- +0x36
    early = i16(s, 0x61),           -- +0x42
    humanShare = i16(s, 0x63),      -- +0x44 = 10v + 1d10
    solidarity = i16(s, 0x65),      -- +0x38
  }
end

--- A card's name and description, from its .DSC: the first line, then the
--- rest. nil when there is none.
function aicard.describe(dataDir, level, number)
  local f = io.open(aicard.path(dataDir, level, number, "DSC"), "rb")
  if not f then return nil end
  local s = f:read("*a")
  f:close()
  local lines = {}
  for line in (s .. "\n"):gmatch("([^\r\n]*)\r?\n") do lines[#lines + 1] = line end
  local name = table.remove(lines, 1) or ""
  while #lines > 0 and lines[#lines] == "" do table.remove(lines) end
  return name, lines
end

--- A level's deck as the setup screen lists it (list_carried_items mode 7,
--- 796c:03d9): the name of each card from 000 up to the first missing .DSC,
--- at most 30 -- the first 40 bytes of the file, cut at a carriage return.
--- Numbered from 0, as the cards are; the count comes second.
function aicard.deck(dataDir, level)
  local out, n = {}, 0
  while n < 30 do
    local f = io.open(aicard.path(dataDir, level, n, "DSC"), "rb")
    if not f then break end
    local s = f:read(40) or ""
    f:close()
    out[n] = s:match("^[^\r]*")
    n = n + 1
  end
  return out, n
end

--- How many cards a level has on disk, 000 up to the first missing one.
function aicard.count(dataDir, level)
  local n = 0
  while n < 100 do
    local f = io.open(aicard.path(dataDir, level, n, "CRD"), "rb")
    if not f then break end
    f:close()
    n = n + 1
  end
  return n
end

return aicard
