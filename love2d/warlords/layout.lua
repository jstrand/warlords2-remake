-- Where the main screen's pieces go on a screen of any size.
--
-- Not the original's: it only ever had 640x480. This keeps every piece of
-- that screen at its own size and moves it as a whole, tied to an edge of the
-- bigger screen, and gives all the room left over to the map:
--
--   the menu bar          across the top, as wide as the screen
--   the map and its frame from (16, 30), growing right and down
--   the bottom bar        under the map, at the bottom left, still 360 wide
--   the strategic map     at the top right
--   the control panel     at the bottom right
--
-- The popups are drawn in a 640x480 frame of their own, centred on the
-- screen, so every dialog keeps the coordinates the original gives it.
--
-- At 640x480 every offset is zero and every rect is the original's own,
-- which is what keeps that screen exactly as it was.

local layout = {}

layout.W, layout.H = 640, 480

-- The original's rects that this moves or stretches: AREA.DAT's regions on
-- the main screen, and the two rects the pointer tests (4125:2aa8, :2ab8).
local MAP      = { x = 16, y = 30, w = 360, h = 360 }     -- region 2
local PANEL    = { x = 0, y = 17, w = 392, h = 386 }      -- region 13, the map's frame
local STRAT    = { x = 400, y = 30, w = 224, h = 312 }    -- region 1
local BAR      = { x = 16, y = 408, w = 360, h = 56 }     -- region 9

--- The layout of a screen `w` x `h` UI pixels. Anything smaller than the
--- original is given the original's size.
function layout.compute(w, h)
  w = math.max(layout.W, math.floor(w))
  h = math.max(layout.H, math.floor(h))
  local ex, ey = w - layout.W, h - layout.H
  local L = {
    w = w, h = h, ex = ex, ey = ey,
    classic = ex == 0 and ey == 0,
    -- how far each fixed group moves
    offset = {
      strat = { x = ex, y = 0 },
      panel = { x = ex, y = ey },
      bar   = { x = 0,  y = ey },
      map   = { x = 0,  y = 0 },
    },
    -- the popups' 640x480 frame, centred
    dialog = { x = math.floor(ex / 2), y = math.floor(ey / 2) },
  }
  L.map   = { x = MAP.x, y = MAP.y, w = MAP.w + ex, h = MAP.h + ey }
  L.panel = { x = PANEL.x, y = PANEL.y, w = PANEL.w + ex, h = PANEL.h + ey }
  L.strat = layout.move(L, "strat", STRAT)
  L.bar   = layout.move(L, "bar", BAR)
  return L
end

--- Which group a rect of the original screen belongs to. Everything right of
--- the map's frame is the strategic map above and the control panel below;
--- under the map is the bottom bar.
function layout.group(r)
  if r.x >= 392 then return r.y < 348 and "strat" or "panel" end
  if r.y >= 396 then return "bar" end
  return "map"
end

--- A copy of the original rect `r`, moved with group `group`.
function layout.move(L, group, r)
  local o = L.offset[group]
  local out = {}
  for k, v in pairs(r) do out[k] = v end
  out.x, out.y = r.x + o.x, r.y + o.y
  return out
end

--- One of the main screen's regions, placed: the map, its frame and the menu
--- bar stretch, the rest move with their group.
function layout.region(L, r)
  local out = layout.move(L, layout.group(r), r)
  if r.id == 2 then
    out.x, out.y, out.w, out.h = L.map.x, L.map.y, L.map.w, L.map.h
  elseif r.id == 13 then
    out.x, out.y, out.w, out.h = L.panel.x, L.panel.y, L.panel.w, L.panel.h
  elseif r.id == 3 then
    out.x, out.y, out.w = r.x, r.y, L.w
  end
  return out
end

return layout
