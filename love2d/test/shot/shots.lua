-- An example script for test/shot: the three things a turn opens with, in
-- the order the player meets them.
--
--     W2_SCRIPT=love2d/test/shot/shots.lua love love2d/test/shot

return {
  { "banner" },                                              -- whose turn it is
  { "hero",   function() love.keypressed("return") end },    -- turn 1's free hero
  { "map",    function() love.mousepressed(510, 350, 1) end },  -- OK, and play
}
