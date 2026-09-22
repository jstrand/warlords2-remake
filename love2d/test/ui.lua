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
  -- the assault plays itself out on the clock, so it needs one
  timer = { getTime = os.clock },
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

-- Every human turn opens with the banner (8cc6:0259), which blocks on any
-- input. It has to be dismissed before anything else reaches the game, so
-- the harness clears it the way a player would.
-- Dismissing it is also what lets the turn's first dialog through, so nothing
-- may be waiting underneath while the banner is still up.
local function dismissBanner(what)
  if G.banner and G.offer then
    fail("banner", "a hero was offered before the banner was dismissed")
  end
  if G.banner then try(what or "dismiss the banner", love.keypressed, "return") end
  if G.banner then fail("banner", "a key did not dismiss it") end
end

if not G.banner then
  fail("banner", "no banner at the start of the first turn")
else
  if G.banner.name ~= G.player.name then
    fail("banner", ("banner says %q, player is %q"):format(G.banner.name, G.player.name))
  end
  if G.banner.turn ~= G.g.turn then fail("banner", "banner shows the wrong turn") end
  if G.banner.colour == nil then fail("banner", "the side has no colour to frame it") end
  try("draw the banner", love.draw)
  -- a click dismisses it too, and must not reach the map underneath
  try("click the banner away", love.mousepressed, 320, 200, 1)
  if G.banner ~= nil then fail("banner", "a click did not dismiss it") end
  print("  the turn banner opened and closed")
end
dismissBanner()

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
  -- The offer is a modal dialog (popup 2 with dialog 12's controls), so it
  -- has to be driven the way a player drives it: type into the name field,
  -- tick a box, press OK.
  if not G.offer.name or G.offer.name == "" then
    fail("turn-1 hero", "the offer carries no name")
  end
  try("draw the hero dialog", love.draw)
  local wasFemale = G.offerFemale
  try("tick the other box", love.mousepressed, wasFemale and 390 or 494, 320, 1)
  if G.offerFemale == wasFemale then
    fail("turn-1 hero", "the checkbox did not change the hero's sex")
  end
  try("draw the other portrait", love.draw)
  try("type into the name", love.textinput, "x")
  if not G.offerName:find("x$") then fail("turn-1 hero", "the name field does not type") end
  try("rub it out again", love.keypressed, "backspace")

  -- the first hero is free, so Cancel must be refused
  try("press the disabled Cancel", love.mousepressed, 350, 350, 1)
  if not G.offer then fail("turn-1 hero", "Cancel closed an offer that cannot be refused") end

  local before = heroesOf(G.g, G.player)
  try("press OK", love.mousepressed, 510, 350, 1)
  local after = heroesOf(G.g, G.player)
  if after ~= before + 1 then
    fail("turn-1 hero", ("accepting gave %d heroes, wanted %d"):format(after, before + 1))
  else
    print("  the free turn-1 hero arrived")
  end
end

--- Later offers can be refused; clear whichever dialog is up.
local function dismissOffer()
  if G.offer then
    if G.offer.first then try("accept a free hero", love.keypressed, "return")
    else try("refuse a hero", love.keypressed, "escape") end
  end
  if G.offer then fail("hero offer", "the dialog would not close") end
end

-- the turn sequence, several times over: this runs every computer player too
for i = 1, 3 do
  try("end turn " .. i, love.keypressed, "space")
  dismissBanner("dismiss the banner on turn " .. i)
  dismissOffer()
end
try("frame after the turns", love.draw)

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
      -- one of these controls ends the turn, which raises the banner; it
      -- would otherwise eat the next control's press
      dismissBanner()
      dismissOffer()
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
  dismissBanner()
  dismissOffer()
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
    -- every mode of the dialog, with its own controls
    for mode = 1, 4 do
      G.city = mine
      local btn
      for _, k in ipairs(G.cityView.dialog.controls) do
        if k.id == 192 + mode then btn = k end
      end
      if btn then
        try(("city mode %d"):format(mode), love.mousepressed, btn.x + 2, btn.y + 2, 1)
        if G.cityMode ~= mode then
          fail("city dialog", ("button %d did not select mode %d"):format(btn.id, mode))
        end
        try(("draw city mode %d"):format(mode), love.draw)
      end
    end
    G.cityMode = 3

    -- Stop, then Done: both are real controls of dialog 6
    for _, id in ipairs({ 202, 192 }) do
      G.city = mine
      local c
      for _, k in ipairs(G.cityView.dialog.controls) do
        if k.id == id then c = k end
      end
      if c then
        try(("city button %d"):format(id), love.mousepressed, c.x + 2, c.y + 2, 1)
      end
    end
    if G.city ~= nil then fail("city dialog", "Done did not close it") end

    -- a click outside the dialog dismisses it; one inside must not
    G.city = mine
    try("click inside the panel", love.mousepressed, 300, 350, 1)
    if G.city == nil then fail("city dialog", "a click inside closed it") end
    try("click outside the panel", love.mousepressed, 10, 460, 1)
    if G.city ~= nil then fail("city dialog", "a click outside did not close it") end

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
G.city, G.openMenu, G.offer, G.banner = nil, nil, nil, nil

-- play on until the game ends, so the end-of-game path runs too
-- 150 rounds is enough to reach the end in the small scenarios, and to run
-- the turn machinery hard in the large ones. Stop as soon as it is over:
-- every further turn is a full round of computer players for nothing.
for _ = 1, 150 do
  try("long game", love.keypressed, "space")
  dismissBanner()
  dismissOffer()
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
