-- The same 640x480 window main.lua is laid out for; see love2d/conf.lua.
function love.conf(t)
  t.window.title = "Warlords II (screenshot)"
  t.window.width = 640
  t.window.height = 480
  t.window.resizable = false
  t.console = false
end
