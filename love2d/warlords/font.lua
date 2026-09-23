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
  -- Two tables of `count` bytes, from `first` (78a8:03ea reads them straight
  -- after an 11-byte header). The first, at +11, is how wide each glyph's art
  -- is -- the sheet is packed by it, and the space, which has no glyph, is 0.
  -- The second, at +11 + count, is how far the pen moves on: usually the
  -- same, but a letter that overhangs its neighbour is narrower here, and
  -- the space has its width. CHANCE17's T is 18 wide and moves on 13, which
  -- puts its bar over the "u" of "Turn"; CHANCE36's space is 15.
  m.ink, m.width = {}, {}
  for i = 0, m.count - 1 do
    m.ink[m.first + i] = s:byte(12 + i)
    m.width[m.first + i] = s:byte(12 + m.count + i)
  end
  return m
end

--- Load a font. `name` is "TEXT", "CHANCE17" or "CHANCE36"; both files sit in
--- the data directory's root. Returns a table with `draw`, `width` and
--- `lineHeight`.
function font.load(dataDir, name, palette, keyIndex)
  local m = readMetrics(dataDir .. "/" .. name .. ".FIN")
  local img, w, h, px = pck.toImage(dataDir .. "/" .. name .. ".FNT", palette, keyIndex)

  -- Lay the glyphs out exactly as the sheet was packed.
  local quad, glyphW = {}, {}
  local x, row = 0, 0
  for i = 0, m.count - 2 do                    -- the last entry has no glyph
    local c = m.first + 1 + i
    local gw = m.ink[c]
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

  local function drawWith(image, text, x0, y0)
    local cx = x0
    for i = 1, #text do
      local c = text:byte(i)
      if quad[c] then
        love.graphics.draw(image, quad[c], cx, y0)
      end
      cx = cx + (m.width[c] or 0)
    end
    return cx - x0
  end

  function self.draw(text, x0, y0)
    return drawWith(img, text, x0, y0)
  end

  --- The same font in other colours, the way 78a8:06ae asks for it: the
  --- sheet's glyph colour (15) becomes `glyph` and its outline (0) becomes
  --- `outline`; the ground stays see-through. Each pair is built once.
  local variants = {}
  function self.colours(glyph, outline)
    local k = glyph * 16 + outline
    if not variants[k] then
      local vimg = (glyph == 15 and outline == 0) and img
        or pck.imageFromPixels(w, h, px, palette, { [15] = glyph, [0] = outline },
                               keyIndex and { [keyIndex] = true } or nil)
      local v = {}
      for key, val in pairs(self) do v[key] = val end
      v.draw = function(text, x0, y0) return drawWith(vimg, text, x0, y0) end
      variants[k] = v
    end
    return variants[k]
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
