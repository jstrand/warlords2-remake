-- The original ran at 640x480 in 16 colours (docs/re/ui.md), and the whole
-- interface is laid out for exactly that. The window is left to main.lua,
-- which opens it full screen in the display's own biggest mode, so a pixel
-- is a real pixel of the panel (display.openWindow); display.lua and
-- warlords/layout.lua then fit the interface to it.
function love.conf(t)
  t.window = nil
  t.identity = "warlords2"   -- the save directory, used only in the browser (web.lua)
  t.console = false
end
