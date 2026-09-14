-- Warlords II .PAL reader.
-- Plain ASCII, 16 lines of "RR GG BB". Components are PERCENTAGES (0-99),
-- not VGA 0-63. See docs/formats/pck.md.

local pal = {}

function pal.load(path)
  local f = assert(io.open(path, "rb"), "cannot open palette: " .. path)
  local text = f:read("*a")
  f:close()

  local colours = {}
  for r, g, b in text:gmatch("(%d+)%s+(%d+)%s+(%d+)") do
    colours[#colours + 1] = {
      tonumber(r) / 99,
      tonumber(g) / 99,
      tonumber(b) / 99,
    }
  end
  assert(#colours == 16, path .. ": expected 16 entries, got " .. #colours)
  return colours
end

return pal
