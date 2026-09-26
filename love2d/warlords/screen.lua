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

  -- TERRAIN<n>/MAPCOLOR.DAT is the strategic map's own colour table, read by
  -- 834b:2a11 as eleven tables, 1200 bytes in all:
  --
  --   4 x 256  a colour per tile id, one table for each of the four pixels a
  --            tile covers: top left, top right, bottom left, bottom right
  --   3 x 32   sixteen u16 colours each, the first of which is used (it is
  --            the identity; the others remap a few colours)
  --   4 x 20   the same four pixels for a tile with a road on it, by road
  --
  -- So a tile is not one flat colour: forest, hills and marsh come out
  -- speckled, and roads are drawn in.
  self.mapColour = { tile = {}, road = {}, remap = {} }
  do
    local p = ("%s/TERRAIN%d/MAPCOLOR.DAT"):format(dataDir, terrain or 0)
    local f = io.open(p, "rb")
    if f then
      local d = f:read("*a")
      f:close()
      for q = 0, 3 do
        local t = {}
        for i = 0, 255 do t[i] = d:byte(q * 256 + i + 1) end
        self.mapColour.tile[q] = t
        local r = {}
        for i = 0, 19 do r[i] = d:byte(1120 + q * 20 + i + 1) end
        self.mapColour.road[q] = r
      end
      for i = 0, 15 do self.mapColour.remap[i] = d:byte(1024 + i * 2 + 1) end
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
-- button, 240/241 two of another (Grp: group, ungroup). The original enables
-- whichever applies and greys the rest, and that live one is what shows and
-- what a click reaches; with none live, the first stands for them all.
local function sameRect(a, b)
  return a.x == b.x and a.y == b.y and a.w == b.w and a.h == b.h
end

local function isCovered(self, c, index)
  local live = (self.state[c.id] or uidata.NORMAL) ~= uidata.DISABLED
  for i, o in ipairs(self.dialog.controls) do
    if i ~= index and sameRect(o, c) then
      local oLive = (self.state[o.id] or uidata.NORMAL) ~= uidata.DISABLED
      if oLive and (not live or i < index) then return true end
      if not oLive and not live and i < index then return true end
    end
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

--- Another dialog's layout, sharing this screen's art and bitmap table.
--- The main screen stays loaded underneath, as it does in the original.
function screen.dialog(self, id)
  local d = uidata.dialog(self.ui, id)
  if not d then return nil end
  local state = {}
  for _, c in ipairs(d.controls) do state[c.id] = uidata.NORMAL end
  return { dialog = d, state = state, id = id }
end

-- The default buttons: the controls Enter presses, the first live one of
-- them in whatever dialog is up (DS:155c, read by 17be:0064).
screen.DEFAULT_IDS = {}
for _, id in ipairs({ 123, 103, 141, 174, 189, 192, 201, 223, 287, 242, 285, 251,
                      292, 294, 330, 331, 332, 172, 356, 359, 368, 370, 396, 421,
                      424, 425, 457, 469, 282, 485, 484, 483, 474, 489, 490, 495 }) do
  screen.DEFAULT_IDS[id] = true
end

--- The ring round a default button (1a0a:0005): two rounded rects in colour
--- 0, the inner one a pixel clear of the control, drawn as the original's own
--- twelve runs apiece -- 2133:02fe across, 2133:0344 down.
local function defaultRing(self, c)
  local p = self.palette and self.palette[1] or { 0, 0, 0 }
  love.graphics.setColor(p[1], p[2], p[3])
  local function across(x, y, n) love.graphics.rectangle("fill", x, y, n, 1) end
  local function down(x, y, n) love.graphics.rectangle("fill", x, y, 1, n) end
  local X, Y, W, H = c.x - 2, c.y - 2, c.w + 3, c.h + 3
  for _ = 1, 2 do
    across(X + 1, Y, W - 2);        down(X + W - 1, Y, 2)
    across(X + W - 1, Y + 1, 2);    down(X + W, Y + 1, H - 2)
    across(X + W - 1, Y + H - 1, 2); down(X + W - 1, Y + H - 1, 2)
    across(X + 1, Y + H, W - 2);    down(X + 1, Y + H - 1, 2)
    across(X, Y + H - 1, 2);        down(X, Y + 1, H - 2)
    across(X, Y + 1, 2);            down(X + 1, Y, 2)
    X, Y, W, H = X - 1, Y - 1, W + 2, H + 2
  end
  love.graphics.setColor(1, 1, 1)
end

--- The controls of a loaded dialog, drawn from this screen's art, each
--- default button with its ring -- which the original only draws while a
--- dialog is up (4125:166a): never on the main screen, nor on a view marked
--- `screen`, one the original shows with the flag clear (the start menu and
--- the sides screen).
function screen.drawDialogControls(self, view)
  love.graphics.setColor(1, 1, 1)
  for _, c in ipairs(view.dialog.controls) do
    if c.bitmap ~= 0 and c.w > 0 and c.h > 0 then
      local art = self.art_for(c.bitmap)
      local s = c.src[view.state[c.id] or uidata.NORMAL]
      if art and s.x + c.w <= art.w and s.y + c.h <= art.h then
        love.graphics.draw(art.image,
          love.graphics.newQuad(s.x, s.y, c.w, c.h, art.w, art.h), c.x, c.y)
      end
    end
    if screen.DEFAULT_IDS[c.id] and not view.screen and c.w > 0 and c.h > 0 then
      defaultRing(self, c)
    end
  end
end

--- One control of a loaded dialog, by id.
function screen.dialogControl(view, id)
  for _, c in ipairs(view.dialog.controls) do
    if c.id == id then return c end
  end
  return nil
end

function screen.dialogControlAt(view, x, y)
  for _, c in ipairs(view.dialog.controls) do
    if c.w > 0 and c.h > 0
       and x >= c.x and x < c.x + c.w and y >= c.y and y < c.y + c.h then
      return c
    end
  end
  return nil
end

--------------------------------------------------------------- strategic map

--- Build the strategic map as one 224x312 image, two pixels a tile each way,
--- the way 834b:2785 paints STRAT.PCK: each of a tile's four pixels takes its
--- own colour from MAPCOLOR.DAT, from the road tables when the tile has a
--- road. Tile ids 0x50-0x5f get a random grey, 2-4, per pixel (dice(1, 3, 1));
--- those are drawn from a generator of their own here so that painting the
--- map never moves the game's dice. Tiles `side` has not seen are black.
--- Rebuilt only when the fog changes -- per-pixel rectangles every frame
--- would be 70000 draw calls.
function screen.strategicImage(self, g, side, seen)
  local scnMod = require("warlords.scn")
  local w, h = g.map.width, g.map.height
  local mc = self.mapColour
  local rows = {}
  local seed = 12345
  local function grey()
    seed = (seed * 1103515245 + 12345) % 2147483648
    return 2 + math.floor(seed / 65536) % 3
  end
  local function rgba(i)
    local c = self.palette[(mc.remap[i] or i) + 1] or { 0, 0, 0 }
    return string.char(math.floor(c[1] * 255), math.floor(c[2] * 255),
                       math.floor(c[3] * 255), 255)
  end
  local black = string.char(0, 0, 0, 255)
  for y = 0, h - 1 do
    local top, bottom = {}, {}
    for x = 0, w - 1 do
      local px
      if seen and not seen(g, side, x, y) then
        px = { black, black, black, black }
      else
        local road = scnMod.roadAt(g.map, x, y) % 32
        local id = road ~= 0 and road - 1 or (scnMod.tileAt(g.map, x, y) % 256)
        px = {}
        if road == 0 and id >= 0x50 and id <= 0x5f then
          for q = 1, 4 do px[q] = rgba(grey()) end
        else
          local t = road ~= 0 and mc.road or mc.tile
          for q = 0, 3 do px[q + 1] = rgba(t[q] and t[q][id] or 0) end
        end
      end
      top[#top + 1] = px[1]; top[#top + 1] = px[2]
      bottom[#bottom + 1] = px[3]; bottom[#bottom + 1] = px[4]
    end
    rows[#rows + 1] = table.concat(top)
    rows[#rows + 1] = table.concat(bottom)
  end
  local data = love.image.newImageData(w * 2, h * 2, "rgba8", table.concat(rows))
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
  for i, c in ipairs(self.dialog.controls) do
    if c.w > 0 and c.h > 0 and not isCovered(self, c, i)
       and x >= c.x and x < c.x + c.w and y >= c.y and y < c.y + c.h then
      return c
    end
  end
  return nil
end

return screen
