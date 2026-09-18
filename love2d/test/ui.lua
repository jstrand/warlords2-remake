-- Drive main.lua without LOVE.
--
--     lua love2d/test/ui.lua            -- from the repository root
--     luajit love2d/test/ui.lua         -- what LOVE actually runs
--
-- LOVE cannot be run headlessly, so this stubs just enough of its API to load
-- the front end and exercise every path a player takes: draw a frame, select
-- a stack, walk it, attack, set production, end the turn, save and load. It
-- catches the runtime mistakes a syntax check cannot -- a nil field, a bad
-- argument order, a call that only happens on the third keypress.
--
-- It asserts nothing about what is *drawn*. It only proves the code runs.

package.path = "love2d/?.lua;" .. package.path

local DATA = arg[1] or "original"
local SCENARIO = arg[2] or "TUTORIA"
-- A fixed seed, so a run is reproducible: without one the harness plays a
-- different game every time and its runtime swings wildly.
local SEED = arg[3] or "20250918"
local failures = 0

local function fail(what, err)
  failures = failures + 1
  print(("  FAIL  %s: %s"):format(what, err))
end

local function try(what, fn, ...)
  local ok, err = pcall(fn, ...)
  if not ok then fail(what, tostring(err)) end
  return ok
end

--------------------------------------------------------------- the LOVE stub

local drawCalls = 0

local function stubImage(w, h)
  return {
    getDimensions = function() return w or 640, h or 240 end,
    getWidth = function() return w or 640 end,
    getHeight = function() return h or 240 end,
    setFilter = function() end,
    setWrap = function() end,
  }
end

love = {
  graphics = {
    newQuad = function(x, y, w, h) return { x = x, y = y, w = w, h = h } end,
    newImage = function() return stubImage() end,
    newFont = function() return { getWidth = function() return 10 end,
                                  getHeight = function() return 14 end } end,
    setFont = function() end,
    setColor = function() end,
    setBackgroundColor = function() end,
    getDimensions = function() return 1024, 768 end,
    getWidth = function() return 1024 end,
    getHeight = function() return 768 end,
    draw = function() drawCalls = drawCalls + 1 end,
    rectangle = function() end,
    line = function() end,
    print = function() end,
    printf = function() end,
    push = function() end,
    pop = function() end,
    translate = function() end,
    scale = function() end,
    origin = function() end,
    setScissor = function() end,
    getScissor = function() end,
  },
  image = {
    newImageData = function(w, h) return stubImage(w, h) end,
  },
  event = { quit = function() end },
}

--------------------------------------------------------------------- running

print("loading the front end")
local chunk = assert(loadfile("love2d/main.lua"))
local G = chunk()          -- main.lua hands back its view state, for tests

print("  scenario: " .. SCENARIO .. ", seed " .. SEED)
if not try("love.load", love.load, { SCENARIO, DATA, SEED }) then
  print(("\n%d failed"):format(failures))
  os.exit(1)
end

try("first frame", love.draw)
print(("  drew %d sprites"):format(drawCalls))

-- Turn 1 always offers a free hero (docs/rules.md > When a hero offers to
-- join), and the offer has to reach the player at load: game.begin runs the
-- first turn's start, so nothing else will ever ask about it. This went
-- unnoticed once because the engine set the offer and the front end dropped it.
local function heroesOf(g, side)
  local n = 0
  for _, a in ipairs(g.armies) do
    if a.type == 28 and a.owner == side.index then n = n + 1 end
  end
  return n
end

if not G then
  fail("turn-1 hero", "main.lua did not return its state")
elseif not G.offer then
  fail("turn-1 hero", "no hero was offered on turn 1")
else
  local before = heroesOf(G.g, G.player)
  try("accept the turn-1 hero", love.keypressed, "y")
  local after = heroesOf(G.g, G.player)
  if after ~= before + 1 then
    fail("turn-1 hero", ("accepting gave %d heroes, wanted %d"):format(after, before + 1))
  else
    print("  the free turn-1 hero arrived")
  end
end

-- the turn sequence, several times over: this runs every computer player too
for i = 1, 3 do
  try("end turn " .. i, love.keypressed, "space")
end
try("frame after the turns", love.draw)

-- a hero may have been offered on turn 1
try("accept a hero", love.keypressed, "y")
try("refuse a hero", love.keypressed, "n")

-- selection and movement, by clicking around the middle of the view
for _, button in ipairs({ 1, 2 }) do
  for x = 40, 600, 120 do
    for y = 40, 400, 120 do
      try(("click %d at %d,%d"):format(button, x, y), love.mousepressed, x, y, button)
    end
  end
end
try("frame after clicking", love.draw)

-- press and release every control of the main screen, on and off the button,
-- so the state handling runs for all of them
if G and G.screen then
  local uidata = require("warlords.uidata")
  local screenMod = require("warlords.screen")
  for _, c in ipairs(G.screen.dialog.controls) do
    if c.w > 0 and c.h > 0 then
      local cx, cy = c.x + math.floor(c.w / 2), c.y + math.floor(c.h / 2)
      -- Several controls share a rect: 183/184/185 are three variants of one
      -- button, as are 240/241. Only the one hit testing finds can light up.
      local hit = screenMod.controlAt(G.screen, cx, cy)
      try(("press control %d"):format(c.id), love.mousepressed, cx, cy, 1)
      if hit and G.screen.state[hit.id] ~= uidata.ACTIVE then
        fail("control press", ("control %d did not light up"):format(hit.id))
      end
      try(("release control %d"):format(c.id), love.mousereleased, cx, cy, 1)
      if hit and G.screen.state[hit.id] ~= uidata.NORMAL then
        fail("control release", ("control %d stayed lit"):format(hit.id))
      end
    end
  end
  -- released away from the button: must still reset, and must not act
  local c = G.screen.dialog.controls[2]
  try("press then leave", love.mousepressed, c.x + 1, c.y + 1, 1)
  try("release elsewhere", love.mousereleased, 5, 470, 1)
  if G.screen.state[c.id] ~= uidata.NORMAL then
    fail("control release", "a button left pressed after releasing off it")
  end
  print("  exercised every control on the main screen")
end

-- the rest of the keys
for _, key in ipairs({ "p", "p", "c", "home", "up", "down", "left", "right",
                       "w", "a", "s", "d", "f5", "f9", "space" }) do
  try("key " .. key, love.keypressed, key)
end
try("frame after the keys", love.draw)

-- open every menu and pick every item in it
if G and G.menuLayout then
  local menuMod = require("warlords.menu")
  local picked = 0
  for i, m in ipairs(G.menuLayout) do
    try(("open menu %s"):format(m.title), love.mousepressed, m.x + 2, 4, 1)
    if G.openMenu ~= i then
      fail("menu", ("clicking %s did not open it"):format(m.title))
    end
    for _, r in ipairs(m.drop.rows) do
      if r.label ~= "-" then
        -- reopen, then pick the row
        G.openMenu = i
        try(("pick %s > %s"):format(m.title, r.label),
            love.mousepressed, r.x + 2, r.y + 1, 1)
        if G.openMenu ~= nil then
          fail("menu", ("picking %s left the menu open"):format(r.label))
        end
        picked = picked + 1
      end
    end
    G.openMenu = nil
  end
  print(("  opened %d menus and picked %d items"):format(#G.menuLayout, picked))
end

-- the city dialog: open one of our own cities, pick each slot, close again
if G and G.g then
  local game = require("warlords.game")
  local scr  = require("warlords.screen")
  local mine = game.sideCities(G.g, G.player)[1]
  if not mine then
    fail("city dialog", "the player holds no city to open")
  else
    -- put the city in view, then click its tile
    G.cx = math.max(0, math.min(mine.x - 4, G.g.map.width - 9))
    G.cy = math.max(0, math.min(mine.y - 4, G.g.map.height - 9))
    local r = scr.region(G.screen, scr.REGION.MAP)
    local sx = r.x + (mine.x - G.cx) * scr.TILE + 2
    local sy = r.y + (mine.y - G.cy) * scr.TILE + 2
    -- a city tile usually holds our garrison, so open it directly instead
    try("open the city dialog", function() G.city = nil end)
    local ok2 = pcall(function()
      love.mousepressed(sx, sy, 1)
    end)
    if not ok2 then fail("city dialog", "clicking the city tile errored") end

    -- drive it regardless of whether the click landed on armies
    G.city = mine
    G.cityView = G.cityView or scr.dialog(G.screen, 6)
    try("draw the city dialog", love.draw)
    for i = 1, 4 do
      local c
      for _, k in ipairs(G.cityView.dialog.controls) do
        if k.id == 196 + i then c = k end
      end
      if c then
        G.city = mine
        try(("pick slot %d"):format(i), love.mousepressed,
            c.x + 2, c.y + 2, 1)
      end
    end
    G.city = mine
    try("close with escape", love.keypressed, "escape")
    if G.city ~= nil then fail("city dialog", "escape did not close it") end
    print("  drove the city dialog")
  end
end

-- clicking the status bar must be ignored, not crash
try("click the status bar", love.mousepressed, 100, 750, 1)

-- make sure no dialog is left open: while one is, clicks are swallowed, the
-- player never moves and the game below never reaches an end
G.city, G.openMenu, G.offer = nil, nil, nil

-- play on until the game ends, so the end-of-game path runs too
-- 150 rounds is enough to reach the end in the small scenarios, and to run
-- the turn machinery hard in the large ones. Stop as soon as it is over:
-- every further turn is a full round of computer players for nothing.
for _ = 1, 150 do
  try("long game", love.keypressed, "space")
  if G.over then break end
end
try("final frame", love.draw)

os.remove("warlords-save.lua")

if failures == 0 then
  print("\nthe front end ran clean")
  os.exit(0)
end
print(("\n%d failed"):format(failures))
os.exit(1)
