-- The original ran at 640x480 in 16 colours (docs/re/ui.md), and the whole
-- interface is laid out for exactly that. The game opens full screen, in the
-- display's real pixels (highdpi), and display.lua and warlords/layout.lua
-- fit the interface to it; the 1280x960 is the window's size if full screen
-- is turned off.
function love.conf(t)
  t.window.title = "Warlords II"
  t.window.width = 1280
  t.window.height = 960
  t.window.fullscreen = true
  t.window.fullscreentype = "desktop"
  t.window.resizable = false
  -- real pixels on a Retina screen, so the scaling is exact (display.lua)
  t.window.highdpi = true
  t.console = false
end
