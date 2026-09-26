-- Render one of the game's songs to a WAV file, as the Sound Blaster FM
-- version plays it (love2d/warlords/ailfm.lua), to listen to outside the game.
--
--     luajit tools/xmi2wav.lua SINT12 out.wav [seconds] [data dir]
--
-- Run from the repository root. The name is the file in SOUND/ without .XMI;
-- the song plays once, to its end or for `seconds`. Mono, 16-bit, at the
-- OPL2's own 49716 Hz. LuaJIT renders about four times faster than real time.

package.path = "love2d/?.lua;" .. package.path
local ailfm = require("warlords.ailfm")
local xmi = require("warlords.xmi")

local name, out, secs, data = arg[1], arg[2], tonumber(arg[3] or ""), arg[4] or "original"
if not name or not out then
  io.stderr:write("usage: luajit tools/xmi2wav.lua SINT12 out.wav [seconds] [data dir]\n")
  os.exit(1)
end

local function read(p)
  local f = assert(io.open(p, "rb"), "cannot open " .. p)
  local s = f:read("*a")
  f:close()
  return s
end

local drv = ailfm.new(read(data .. "/ADLIB.ADV"), read(data .. "/MIDPAK.AD"))
local seq = assert(xmi.parse(read(data .. "/SOUND/" .. name:upper() .. ".XMI")))
drv:play(seq, false)

local rate = math.floor(ailfm.RATE + 0.5)
local limit = secs and math.floor(secs * rate) or math.huge
local f = assert(io.open(out, "wb"))
local function le(v, n)
  local b = {}
  for i = 1, n do b[i] = string.char(v % 256) v = math.floor(v / 256) end
  return table.concat(b)
end
f:write(("\0"):rep(44))                   -- the header, once the length is known

local buf, total, chunk = {}, 0, 4096
while total < limit and not drv:idle() do
  local n = math.min(chunk, limit - total)
  drv:render(buf, n)
  local parts = {}
  for i = 1, n do
    local v = buf[i]
    parts[i] = le(v < 0 and v + 65536 or v, 2)
  end
  f:write(table.concat(parts))
  total = total + n
end

f:seek("set", 0)
f:write("RIFF", le(36 + total * 2, 4), "WAVEfmt ", le(16, 4), le(1, 2), le(1, 2),
        le(rate, 4), le(rate * 2, 4), le(2, 2), le(16, 2), "data", le(total * 2, 4))
f:close()
print(("%s: %.1f s"):format(out, total / rate))
