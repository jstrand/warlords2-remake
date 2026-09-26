-- The original ran at 640x480 in 16 colours (docs/re/ui.md), and the whole
-- interface is laid out for exactly that. The game opens full screen, and
-- main.lua draws the 640x480 frame into a canvas, scales it up by a whole
-- number with nearest filtering so every original pixel stays a crisp block,
-- and centres it with black around. The game itself only ever sees 640x480
-- coordinates. The 1280x960 is the window size if full screen is turned off.
function love.conf(t)
  t.window.title = "Warlords II"
  t.window.width = 1280
  t.window.height = 960
  t.window.fullscreen = true
  t.window.fullscreentype = "desktop"
  t.window.resizable = false
  -- real pixels on a Retina screen, so the scaling is exact (main.lua)
  t.window.highdpi = true
  t.console = false
end
