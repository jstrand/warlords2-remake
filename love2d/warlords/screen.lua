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

function screen.drawControls(self)
  love.graphics.setColor(1, 1, 1)
  for _, c in ipairs(self.dialog.controls) do
    if c.bitmap ~= 0 and c.w > 0 and c.h > 0 then
      local st = self.state[c.id] or uidata.NORMAL
      local q = self.quad(c, st)
      local art = self.art_for(c.bitmap)
      if q and art then
        love.graphics.draw(art.image, q, c.x, c.y)
      end
    end
  end
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
