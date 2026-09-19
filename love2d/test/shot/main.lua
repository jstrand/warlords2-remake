-- A LOVE project that runs the real front end and writes PNGs of it.
--
-- test/ui.lua proves the front end *runs*; it asserts nothing about what is
-- drawn, and its setColor is a no-op, so every fault that is purely one of
-- appearance is invisible to it. This is the other half: it plays the game
-- for real, on a real canvas, and saves what came out so it can be compared
-- against a screenshot of the original.
--
--     love love2d/test/shot                       # one shot of the screen
--     W2_OUT=/tmp/shots W2_SCRIPT=shots.lua \
--       love love2d/test/shot ERYTHEA original 12345
--
-- Run it from the repository root, like the game, so that `original/`
-- resolves. The arguments are the game's own: scenario, data directory, seed
-- -- and passing a seed is what makes two runs comparable.
--
--     W2_OUT     where the PNGs go (default: ./shots)
--     W2_SCRIPT  a lua file returning { {name, fn}, ... }. Each entry gets a
--                frame: fn runs, the frame is drawn, and the result is saved
--                as name.png. fn may be nil to just capture.
--
-- It drives the game only through love.keypressed and love.mousepressed, the
-- way a player does -- main.lua keeps its state in a local, and reaching past
-- the handlers would be testing something the player cannot do anyway.

local OUT    = os.getenv("W2_OUT") or "shots"
local SCRIPT = os.getenv("W2_SCRIPT")

-- we live in love2d/test/shot; the game is two directories up
local ROOT = love.filesystem.getSource() .. "/../.."
package.path = ROOT .. "/?.lua;" .. package.path
assert(loadfile(ROOT .. "/main.lua"))()

local steps = SCRIPT and assert(loadfile(SCRIPT))() or { { "screen" } }

local i, settle = 1, 2

function love.update()
  -- give the first frames away, so anything the game defers to a draw has
  -- happened before the script starts
  if settle > 0 then settle = settle - 1 return end
  if i > #steps then love.event.quit() return end

  local step = steps[i]
  i = i + 1
  if step[2] then step[2]() end
  -- the callback runs at the end of this frame, after it has been presented
  love.graphics.captureScreenshot(function(img)
    local path = OUT .. "/" .. step[1] .. ".png"
    local f = assert(io.open(path, "wb"), "cannot write " .. path)
    f:write(img:encode("png"):getString())
    f:close()
    print("wrote " .. path)
  end)
end
