-- The Lua side of the FM comparison: luajit cpp/test/fmrender.lua FILE SECS
package.path = "love2d/?.lua;" .. package.path
local ailfm = require("warlords.ailfm")
local xmi = require("warlords.xmi")
local function read(p) local f = assert(io.open(p, "rb")) local s = f:read("*a") f:close() return s end
local drv = ailfm.new(read("original/ADLIB.ADV"), read("original/MIDPAK.AD"))
drv:play(xmi.parse(read(arg[1])), false)
local N, buf = 49716, {}
for s = 0, tonumber(arg[2]) - 1 do
  drv:render(buf, N)
  local sum, peak = 0, 0
  for i = 1, N do
    sum = (sum * 31 + buf[i] + 65536) % 1000000007
    if math.abs(buf[i]) > peak then peak = math.abs(buf[i]) end
  end
  print(s, string.format("%d", sum), peak)
end
