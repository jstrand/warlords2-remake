-- The original's 640x480 screen: background, controls and hit regions.
--
-- Everything here is driven by the game's own layout files (uidata.lua) and
-- its own art, so the chrome is the original's rather than a lookalike.
-- docs/re/ui.md and docs/formats/screens.md.

local pck    = require("warlords.pck")
local uidata = require("warlords.uidata")

local screen = {}

screen.WIDTH, screen.HEIGHT = 640, 480

-- The main screen's regions, by the id AREA.DAT gives them.
screen.REGION = {
  STRATEGIC = 1,     -- (400,30) 224x312
  MAP       = 2,     -- (16,30)  360x360, 9x9 tiles of 40
  MENUBAR   = 3,     -- (0,0)    640x18
  BOTTOMBAR = 9,     -- (16,408) 360x56
  MAPPANEL  = 13,
}

screen.TILE = 40
screen.VIEW_COLS, screen.VIEW_ROWS = 9, 9

--------------------------------------------------------------------- loading

--- Find a .pck by name, wherever it lives: the UI art is in PICS/ and the
--- terrain-specific art in TERRAIN<n>/. The loader picks the directory from a
--- flag byte we do not have, so try the places the shipped files actually are.
local function findArt(dataDir, name, terrain)
  local tries = {
    ("%s/PICS/%s"):format(dataDir, name:upper()),
    ("%s/TERRAIN%d/%s"):format(dataDir, terrain or 0, name:upper()),
    ("%s/%s"):format(dataDir, name:upper()),
  }
  for _, p in ipairs(tries) do
    local f = io.open(p, "rb")
    if f then f:close() return p end
  end
  return nil
end

--- Load the layout and every bitmap the given dialog needs.
function screen.load(dataDir, palette, dialogId, terrain)
  local ui = uidata.load(dataDir)
  local self = {
    ui = ui,
    dialog = uidata.dialog(ui, dialogId or uidata.MAIN_SCREEN),
    art = {},
    dataDir = dataDir,
    palette = palette,
  }

  function self.art_for(bitmapId)
    if self.art[bitmapId] then return self.art[bitmapId] end
    local name = ui.bitmaps[bitmapId]
    if not name then return nil end
    local path = findArt(dataDir, name, terrain)
    if not path then return nil end
    -- Colour 3 is the sheets' background; keep it, the controls are opaque.
    local img, w, h = pck.toImage(path, palette)
    self.art[bitmapId] = { image = img, w = w, h = h }
    return self.art[bitmapId]
  end

  -- The background is four 320x240 quadrants of one 640x480 image.
  self.background = {}
  for i = 0, 3 do
    local path = findArt(dataDir, ("screen%d.pck"):format(i), terrain)
    if path then
      local img = pck.toImage(path, palette)
      self.background[i] = { image = img, x = (i % 2) * 320, y = math.floor(i / 2) * 240 }
    end
  end

  -- TERRAIN<n>/MAPCOLOR.DAT is the strategic map's own colour table: one
  -- palette index per terrain tile id. Rendering the map through it gives the
  -- green-islands-on-blue of the original's overview exactly.
  self.mapColour = {}
  do
    local p = ("%s/TERRAIN%d/MAPCOLOR.DAT"):format(dataDir, terrain or 0)
    local f = io.open(p, "rb")
    if f then
      local s = f:read("*a")
      f:close()
      for i = 1, #s do self.mapColour[i - 1] = s:byte(i) end
    end
  end

  -- Controls, with a live state each. State 1 is the resting look.
  self.state = {}
  for _, c in ipairs(self.dialog.controls) do
    self.state[c.id] = uidata.NORMAL
  end

  -- Quads are cut lazily, one per control per state.
  local quads = {}
  function self.quad(c, st)
    local key = c.id * 4 + st
    if quads[key] then return quads[key] end
    local art = self.art_for(c.bitmap)
    if not art then return nil end
    local s = c.src[st]
    if s.x + c.w > art.w or s.y + c.h > art.h then return nil end
    quads[key] = love.graphics.newQuad(s.x, s.y, c.w, c.h, art.w, art.h)
    return quads[key]
  end

  return self
end

--------------------------------------------------------------------- drawing

function screen.drawBackground(self)
  love.graphics.setColor(1, 1, 1)
  for _, q in pairs(self.background) do
    love.graphics.draw(q.image, q.x, q.y)
  end
end

-- Some controls share a rect exactly -- 183/184/185 are three variants of one
-- button, 240/241 two of another. The original enables whichever it wants and
-- shows only that; until we know which, draw the first, so what is drawn is
-- also what hit testing finds.
local function isCovered(self, c, index)
  for i = 1, index - 1 do
    local o = self.dialog.controls[i]
    if o.x == c.x and o.y == c.y and o.w == c.w and o.h == c.h then return true end
  end
  return false
end

function screen.drawControls(self)
  love.graphics.setColor(1, 1, 1)
  for i, c in ipairs(self.dialog.controls) do
    if c.bitmap ~= 0 and c.w > 0 and c.h > 0 and not isCovered(self, c, i) then
      local st = self.state[c.id] or uidata.NORMAL
      local q = self.quad(c, st)
      local art = self.art_for(c.bitmap)
      if q and art then
        love.graphics.draw(art.image, q, c.x, c.y)
      end
    end
  end
end

--------------------------------------------------------------- strategic map

--- Build the strategic map as one image, a pixel per tile. It is drawn at
--- twice the size, which is how the original's 112x156 map fills the
--- 224x312 region exactly. Rebuild it only when the fog changes -- per-tile
--- rectangles every frame would be 17472 draw calls.
function screen.strategicImage(self, g, side, seen)
  local scnMod = require("warlords.scn")
  local w, h = g.map.width, g.map.height
  local bytes = {}
  for y = 0, h - 1 do
    for x = 0, w - 1 do
      local c
      if seen and not seen(g, side, x, y) then
        c = { 0, 0, 0 }
      else
        local idx = self.mapColour[scnMod.tileAt(g.map, x, y)] or 0
        c = self.palette[idx + 1] or { 0, 0, 0 }
      end
      bytes[#bytes + 1] = string.char(
        math.floor(c[1] * 255), math.floor(c[2] * 255), math.floor(c[3] * 255), 255)
    end
  end
  local data = love.image.newImageData(w, h, "rgba8", table.concat(bytes))
  local img = love.graphics.newImage(data)
  img:setFilter("nearest", "nearest")
  return img
end

--------------------------------------------------------------------- regions

--- The region under a point, topmost first, as the original hit-tests.
function screen.regionAt(self, x, y)
  local rs = self.dialog.regions
  for i = #rs, 1, -1 do
    local r = rs[i]
    if r.w > 0 and r.h > 0
       and x >= r.x and x < r.x + r.w and y >= r.y and y < r.y + r.h then
      return r
    end
  end
  return nil
end

function screen.region(self, id)
  for _, r in ipairs(self.dialog.regions) do
    if r.id == id then return r end
  end
  return nil
end

--- One control by id, or nil.
function screen.control(self, id)
  for _, c in ipairs(self.dialog.controls) do
    if c.id == id then return c end
  end
  return nil
end

--- The control under a point, or nil.
function screen.controlAt(self, x, y)
  for _, c in ipairs(self.dialog.controls) do
    if c.w > 0 and c.h > 0
       and x >= c.x and x < c.x + c.w and y >= c.y and y < c.y + c.h then
      return c
    end
  end
  return nil
end

return screen
