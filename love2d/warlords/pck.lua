-- Warlords II .PCK image decoder.
--
--   u16 version (1), u16 width, u16 height, then four compressed bitplanes.
--
-- Each plane is LZ77 with a signed big-endian offset:
--   cmd < 0x80   literal run of (cmd + 1) bytes
--   cmd >= 0x80  match, 3 bytes: [cmd][mid][len]
--                distance = 65536 - ((cmd << 8) | mid)   -- 1..32768
--                length   = len + 1                      -- 1..256
-- Copies are byte-at-a-time so they may overlap. Reads before the start of the
-- plane yield 0. The window resets at every plane boundary.
--
-- Plane p supplies bit p of the 4bpp colour index; within a byte the MSB is the
-- leftmost pixel. See docs/formats/pck.md.

local pck = {}

-- bit k (MSB first) of every byte value, precomputed
local BITS = {}
for v = 0, 255 do
  local t = {}
  for k = 1, 8 do
    t[k] = math.floor(v / 2 ^ (8 - k)) % 2 == 1
  end
  BITS[v] = t
end

local function u16(s, i)
  return s:byte(i) + s:byte(i + 1) * 256
end

local function decodePlane(s, pos, planeSize)
  local out, n = {}, 0
  while n < planeSize do
    local cmd = s:byte(pos)
    if cmd < 0x80 then
      local count = cmd + 1
      for i = 1, count do
        out[n + i] = s:byte(pos + i)
      end
      n = n + count
      pos = pos + 1 + count
    else
      local dist = 65536 - (cmd * 256 + s:byte(pos + 1))
      local count = s:byte(pos + 2) + 1
      pos = pos + 3
      for _ = 1, count do
        local src = n - dist + 1
        n = n + 1
        out[n] = (src >= 1) and out[src] or 0
      end
    end
  end
  assert(n == planeSize, "plane overran")
  return out, pos
end

-- Returns width, height, and a flat 1-indexed array of palette indices (0-15).
function pck.decode(path)
  local f = assert(io.open(path, "rb"), "cannot open image: " .. path)
  local s = f:read("*a")
  f:close()

  assert(u16(s, 1) == 1, path .. ": unexpected version")
  local w, h = u16(s, 3), u16(s, 5)
  local rowbytes = w / 8
  local planeSize = rowbytes * h

  local pos = 7
  local px = {}
  for i = 1, w * h do px[i] = 0 end

  for p = 0, 3 do
    local plane
    plane, pos = decodePlane(s, pos, planeSize)
    local bitval = 2 ^ p
    for y = 0, h - 1 do
      local base, row = y * rowbytes, y * w
      for xb = 0, rowbytes - 1 do
        local v = plane[base + xb + 1]
        if v ~= 0 then
          local bits = BITS[v]
          local o = row + xb * 8
          for k = 1, 8 do
            if bits[k] then
              px[o + k] = px[o + k] + bitval
            end
          end
        end
      end
    end
  end
  return w, h, px
end

--- An image from decoded indices, with each index passed through `map`
--- (index -> index) if one is given. `keys` is a set of the sheet's own
--- indices, before mapping, to leave transparent. This is how a font is drawn
--- in colours other than its own: 78a8:0839 remaps the sheet's glyph and
--- outline colours.
function pck.imageFromPixels(w, h, px, palette, map, keys)
  keys = keys or {}
  local bytes = {}
  for i = 1, w * h do
    local idx = px[i]
    local transparent = keys[idx]
    if map and map[idx] then idx = map[idx] end
    if transparent then
      bytes[i] = string.char(0, 0, 0, 0)
    else
      local c = palette[idx + 1]
      bytes[i] = string.char(
        math.floor(c[1] * 255), math.floor(c[2] * 255), math.floor(c[3] * 255), 255)
    end
  end
  local data = love.image.newImageData(w, h, "rgba8", table.concat(bytes))
  local img = love.graphics.newImage(data)
  img:setFilter("nearest", "nearest")
  return img
end

-- Decode straight to a LOVE image. keyIndex (optional) becomes transparent.
--- Decode a .PCK into an image. `keyIndex` is the palette index to make
--- transparent, or a table of them -- ATRANS2.PCK's map markers are white on
--- a two-colour mask and need both dropped.
function pck.toImage(path, palette, keyIndex)
  local w, h, px = pck.decode(path)
  local keys = {}
  if type(keyIndex) == "table" then
    for _, k in ipairs(keyIndex) do keys[k] = true end
  elseif keyIndex then
    keys[keyIndex] = true
  end
  local bytes = {}
  for i = 1, w * h do
    local idx = px[i]
    local c = palette[idx + 1]
    if keys[idx] then
      bytes[i] = string.char(0, 0, 0, 0)
    else
      bytes[i] = string.char(
        math.floor(c[1] * 255), math.floor(c[2] * 255), math.floor(c[3] * 255), 255)
    end
  end
  -- newImageData(w, h, format, data) takes a raw byte string in LOVE 11
  local data = love.image.newImageData(w, h, "rgba8", table.concat(bytes))
  local img = love.graphics.newImage(data)
  img:setFilter("nearest", "nearest")
  return img, w, h, px
end

return pck
