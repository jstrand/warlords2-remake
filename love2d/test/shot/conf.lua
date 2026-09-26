-- The original's 640x480 window, which main.lua lays out exactly as the
-- original did (warlords/layout.lua); see love2d/conf.lua.
--
--     W2_SIZE=980x615   a window that size instead, for the bigger layout
--     W2_FULLSCREEN=1   the whole screen in its real pixels, as the game runs
function love.conf(t)
  t.window.title = "Warlords II (screenshot)"
  local w, h = (os.getenv("W2_SIZE") or ""):match("^(%d+)x(%d+)$")
  t.window.width = tonumber(w) or 640
  t.window.height = tonumber(h) or 480
  t.window.resizable = false
  if os.getenv("W2_FULLSCREEN") then
    t.window.fullscreen = true
    t.window.fullscreentype = "desktop"
    t.window.highdpi = true
  end
  t.console = false
end
