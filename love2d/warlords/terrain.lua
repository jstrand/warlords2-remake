-- Terrain classification.
--
-- HEURISTIC. The game's own tile-to-terrain table has not been found --
-- FILE.DAT references TERRAIN%d\MAPCOLOR.DAT but no copy ships. So each tile is
-- classified by its dominant palette colour, plus the four tiles whose meaning
-- is pinned by scenario data (every signpost/site/city record lands on one).
-- Good enough to decide "can a land army stand here"; NOT authority for
-- movement costs. See docs/gamestate.md.

local terrain = {}

local SPECIAL = { [0] = "signpost", [10] = "temple", [12] = "ruins", [96] = "city" }

local COLOUR_CLASS = {
  [5] = "water", [6] = "water",
  [10] = "plain", [11] = "plain", [43] = "plain",
  [12] = "forest",
  [1] = "mountain", [2] = "mountain", [3] = "mountain", [4] = "mountain",
}

-- Land armies may not enter these (manual ch.8: water/shore need boats,
-- mountains need fliers).
terrain.BLOCKED = { water = true, mountain = true }

-- sheets: { {w,h,px}, {w,h,px} } from pck.decode for SCENERY0/1
function terrain.classify(sheets)
  local TILE, COLS, PER_SHEET = 40, 16, 96
  local out = {}
  for t = 0, 191 do
    if SPECIAL[t] then
      out[t] = SPECIAL[t]
    else
      local sheet = sheets[math.floor(t / PER_SHEET) + 1]
      local w, px = sheet.w, sheet.px
      local c = t % PER_SHEET
      local sx, sy = (c % COLS) * TILE, math.floor(c / COLS) * TILE
      local hist = {}
      for y = 0, TILE - 1 do
        local row = (sy + y) * w + sx
        for x = 1, TILE do
          local v = px[row + x]
          hist[v] = (hist[v] or 0) + 1
        end
      end
      local best, bestn = 0, -1
      for v, n in pairs(hist) do
        if n > bestn then best, bestn = v, n end
      end
      out[t] = COLOUR_CLASS[best] or "other"
    end
  end
  return out
end

function terrain.passable(class)
  return not terrain.BLOCKED[class]
end

return terrain
