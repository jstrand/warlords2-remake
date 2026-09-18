-- The original's proportional fonts: TEXT, CHANCE17 and CHANCE36.
--
-- A .FNT is a .PCK image -- a sheet of glyphs -- and the .FIN beside it holds
-- the metrics. docs/formats/font.md.
--
-- Glyphs are packed left to right into rows `lineHeight` tall, each taking a
-- slot rounded up to a multiple of 8 (the art is 1-bit planar, so glyphs start
-- on byte boundaries). A glyph that will not fit in the rest of a row starts
-- the next one.

local pck = require("warlords.pck")

local font = {}

-- The sheet holds no glyph for the space; its advance is not in the file.
-- A quarter of the line height matches the shipped screens closely enough to
-- be indistinguishable, but it is a guess -- see docs/formats/font.md.
local function spaceAdvance(lineHeight)
  return math.floor(lineHeight / 4 + 0.5)
end

local function readMetrics(path)
  local f = assert(io.open(path, "rb"), "cannot open font metrics: " .. path)
  local s = f:read("*a")
  f:close()

  local m = {
    count      = s:byte(1),
    first      = s:byte(2),
    sheetWidth = s:byte(3) * 256 + s:byte(4),   -- big-endian, unlike everything else
    lineHeight = s:byte(5),
    baseline   = s:byte(6),
  }
  -- Widths start at +12 and describe char `first + 1` onwards: the space has
  -- no glyph in the sheet at all.
  m.width = {}
  for i = 0, m.count - 1 do
    m.width[m.first + 1 + i] = s:byte(13 + i)
  end
  m.width[m.first] = spaceAdvance(m.lineHeight)
  return m
end

--- Load a font. `name` is "TEXT", "CHANCE17" or "CHANCE36"; both files sit in
--- the data directory's root. Returns a table with `draw`, `width` and
--- `lineHeight`.
function font.load(dataDir, name, palette, keyIndex)
  local m = readMetrics(dataDir .. "/" .. name .. ".FIN")
  local img, w, h = pck.toImage(dataDir .. "/" .. name .. ".FNT", palette, keyIndex)

  -- Lay the glyphs out exactly as the sheet was packed.
  local quad, glyphW = {}, {}
  local x, row = 0, 0
  for i = 0, m.count - 2 do                    -- the last entry has no glyph
    local c = m.first + 1 + i
    local gw = m.width[c]
    local slot = math.ceil(gw / 8) * 8
    if x + slot > m.sheetWidth then x, row = 0, row + 1 end
    if gw > 0 then
      quad[c] = love.graphics.newQuad(x, row * m.lineHeight, gw, m.lineHeight, w, h)
      glyphW[c] = gw
    end
    x = x + slot
  end

  local self = {
    lineHeight = m.lineHeight,
    baseline = m.baseline,
    image = img,
    metrics = m,
  }

  function self.width(text)
    local n = 0
    for i = 1, #text do
      n = n + (m.width[text:byte(i)] or 0)
    end
    return n
  end

  function self.draw(text, px, py)
    local cx = px
    for i = 1, #text do
      local c = text:byte(i)
      if quad[c] then
        love.graphics.draw(img, quad[c], cx, py)
      end
      cx = cx + (m.width[c] or 0)
    end
    return cx - px
  end

  --- Draw centred in a rect, the way the original centres a control's text.
  function self.drawCentred(text, rx, ry, rw, rh)
    self.draw(text,
      rx + math.floor((rw - self.width(text)) / 2),
      ry + math.floor((rh - m.lineHeight) / 2) + 1)
  end

  return self
end

return font
