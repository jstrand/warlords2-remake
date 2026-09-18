-- The original ran at 640x480 in 16 colours (docs/re/ui.md), and the whole
-- interface is laid out for exactly that. The window is that size, so screen
-- coordinates and window coordinates are the same thing -- which also keeps
-- love.graphics.setScissor honest, since it works in window pixels and would
-- ignore any transform we put under it.
function love.conf(t)
  t.window.title = "Warlords II"
  t.window.width = 640
  t.window.height = 480
  t.window.resizable = false
  t.console = false
end
