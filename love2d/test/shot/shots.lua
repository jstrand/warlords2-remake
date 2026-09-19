-- An example script for test/shot: the start-of-turn banner, and the screen
-- behind it once a key has dismissed it.
--
--     W2_SCRIPT=love2d/test/shot/shots.lua love love2d/test/shot

return {
  { "banner" },
  { "map", function() love.keypressed("return") end },
}
