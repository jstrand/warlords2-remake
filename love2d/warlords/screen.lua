-- The original's 640x480 screen: background, controls and hit regions.
--
-- Everything here is driven by the game's own layout files (uidata.lua) and
-- its own art, so the chrome is the original's rather than a lookalike.
-- docs/re/ui.md and docs/formats/screens.md.

local pck    = require("warlords.pck")
local uidata = require("warlords.uidata")
local layout = require("warlords.layout")

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
    -- the four quarters' colour indices, kept for a bigger screen's ground
    backgroundPx = {},
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
      local img, _, _, px = pck.toImage(path, palette)
      self.background[i] = { image = img, x = (i % 2) * 320, y = math.floor(i / 2) * 240 }
      self.backgroundPx[i] = px
    end
  end
  -- the layout as the data gives it, for relayout to start from every time
  self.original = self.dialog

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

--------------------------------------------------------------------- layout

--- Place the main screen's controls and regions for layout `L`
--- (warlords/layout.lua): each moves with its group, and the map, its frame
--- and the menu bar stretch. The data's own rects are left alone, so this
--- can be done again for another size.
function screen.relayout(self, L)
  local d = self.original
  local out = {}
  for k, v in pairs(d) do out[k] = v end
  out.controls, out.regions = {}, {}
  for i, c in ipairs(d.controls) do
    out.controls[i] = layout.move(L, layout.group(c), c)
  end
  for i, r in ipairs(d.regions) do
    out.regions[i] = layout.region(L, r)
  end
  self.dialog = out
end

--------------------------------------------------------------------- drawing

-- A screen bigger than the original has no art of its own. Its ground is the
-- original's speckled stone, a 244 x 220 tile repeated from the screen's
-- (0, 0) as the original's own border repeats it. The border shows only
-- slices of that tile; stonetile.lua (tools/stone_tile.py) says where on the
-- original's screen each piece of the whole of it comes from. Every panel is
-- the original's own -- marble in a black outline -- copied across whole, and
-- sunk into the stone, as the original sinks it, by a one-pixel bevel: dark
-- above and to the left, light below and to the right. The screen's own edge
-- is the other way round, raised: light along the top and left, dark along
-- the bottom and right.
local STONE = require("warlords.stonetile")
local LIGHT, DARK = 2, 4
local PANELS = {                       -- outline included
  { group = "strat", x = 399, y = 29,  w = 226, h = 314 },
  { group = "panel", x = 399, y = 354, w = 226, h = 116 },
  { group = "bar",   x = 15,  y = 402, w = 362, h = 68 },
}

local function composedArt(self)
  if self.bgImage then return end
  local px = {}
  for i = 0, 3 do
    local q, qx, qy = self.backgroundPx[i], (i % 2) * 320, math.floor(i / 2) * 240
    if q then
      for y = 0, 239 do
        for x = 0, 319 do px[(qy + y) * 640 + qx + x + 1] = q[y * 320 + x + 1] end
      end
    end
  end
  for i = 1, 640 * 480 do px[i] = px[i] or 0 end
  self.bgImage = pck.imageFromPixels(640, 480, px, self.palette)
  local tile = {}
  for _, r in ipairs(STONE) do
    local tx, ty, w, h, sx, sy = r[1], r[2], r[3], r[4], r[5], r[6]
    for y = 0, h - 1 do
      for x = 0, w - 1 do
        tile[(ty + y) * STONE.w + tx + x + 1] = px[(sy + y) * 640 + sx + x + 1]
      end
    end
  end
  self.stoneImg = pck.imageFromPixels(STONE.w, STONE.h, tile, self.palette)
  self.stoneImg:setWrap("repeat", "repeat")
end

--- The bevel round a panel whose outline is the rect x, y, w, h.
local function bevel(self, x, y, w, h)
  local c = self.palette[DARK + 1]
  love.graphics.setColor(c[1], c[2], c[3])
  love.graphics.rectangle("fill", x - 1, y - 1, w + 2, 1)
  love.graphics.rectangle("fill", x - 1, y, 1, h + 1)
  c = self.palette[LIGHT + 1]
  love.graphics.setColor(c[1], c[2], c[3])
  love.graphics.rectangle("fill", x, y + h, w + 1, 1)
  love.graphics.rectangle("fill", x + w, y, 1, h)
end

--- The main screen's ground for layout `L`: the original's own at 640x480,
--- and put together from its pieces on anything bigger.
function screen.drawBackground(self, L)
  love.graphics.setColor(1, 1, 1)
  if not L or L.classic then
    for _, q in pairs(self.background) do
      love.graphics.draw(q.image, q.x, q.y)
    end
    return
  end
  composedArt(self)
  love.graphics.draw(self.stoneImg,
    love.graphics.newQuad(0, 0, L.w, L.h, STONE.w, STONE.h), 0, 0)
  -- the screen's frame
  local c = self.palette[LIGHT + 1]
  love.graphics.setColor(c[1], c[2], c[3])
  love.graphics.rectangle("fill", 0, 0, L.w - 1, 1)
  love.graphics.rectangle("fill", 0, 1, 1, L.h - 2)
  c = self.palette[DARK + 1]
  love.graphics.setColor(c[1], c[2], c[3])
  love.graphics.rectangle("fill", 0, L.h - 1, L.w, 1)
  love.graphics.rectangle("fill", L.w - 1, 0, 1, L.h - 1)
  -- the map's black outline, a pixel out all round
  local m = L.map
  bevel(self, m.x - 1, m.y - 1, m.w + 2, m.h + 2)
  love.graphics.setColor(0, 0, 0)
  love.graphics.rectangle("fill", m.x - 1, m.y - 1, m.w + 2, 1)
  love.graphics.rectangle("fill", m.x - 1, m.y + m.h, m.w + 2, 1)
  love.graphics.rectangle("fill", m.x - 1, m.y - 1, 1, m.h + 2)
  love.graphics.rectangle("fill", m.x + m.w, m.y - 1, 1, m.h + 2)
  for _, p in ipairs(PANELS) do
    local at = layout.move(L, p.group, p)
    bevel(self, at.x, at.y, p.w, p.h)
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(self.bgImage, love.graphics.newQuad(p.x, p.y, p.w, p.h, 640, 480), at.x, at.y)
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

--- The controls of a loaded dialog, drawn from this screen's art.
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
