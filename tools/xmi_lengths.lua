-- Each song's length in seconds, as sound.lua works it out: the tick after
-- its last event, at XMIDI's 120 a second. For tools/web_assets.py.
--
--     luajit tools/xmi_lengths.lua original/SOUND/SINT0.XMI ...
package.path = "love2d/?.lua;" .. package.path
local xmi = require("warlords.xmi")
for _, n in ipairs(arg) do
  local f = assert(io.open(n, "rb"))
  local s = f:read("*a")
  f:close()
  local seq = xmi.parse(s)
  print(n .. "\t" .. (seq and (seq.length + 1) / xmi.TICK_RATE or 0))
end
