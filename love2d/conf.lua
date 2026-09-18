-- The original ran at 640x480 in 16 colours (docs/re/ui.md), and the whole
-- interface is laid out for exactly that. The window is scaled by a whole
-- number at draw time, so the pixels stay square and sharp.
function love.conf(t)
  t.window.title = "Warlords II"
  t.window.width = 640 * 2
  t.window.height = 480 * 2
  t.window.resizable = true
  t.console = false
end
