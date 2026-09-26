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
-- The screen, in pixels: the original's 640x480 unless a fourth argument
-- asks for another ("980x615"), which lays the main screen out bigger
-- (warlords/layout.lua) and moves every dialog into a centred frame.
local SCREEN_W, SCREEN_H = 640, 480
if arg[4] then
  SCREEN_W, SCREEN_H = arg[4]:match("^(%d+)x(%d+)$")
  SCREEN_W, SCREEN_H = tonumber(SCREEN_W), tonumber(SCREEN_H)
end
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
    getDimensions = function() return SCREEN_W, SCREEN_H end,
    getWidth = function() return SCREEN_W end,
    getHeight = function() return SCREEN_H end,
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
  -- keys the harness holds down, for the accelerators (alt E ends the turn)
  keyboard = { isDown = function(...)
    for _, k in ipairs({ ... }) do if HELD and HELD[k] then return true end end
    return false
  end },
  -- the assault plays itself out on the clock, so it needs one
  timer = { getTime = os.clock },
}

--------------------------------------------------------------------- running

print("loading the front end")
local chunk = assert(loadfile("love2d/main.lua"))
local G = chunk()          -- main.lua hands back its view state, for tests

print("  scenario: " .. SCENARIO .. ", seed " .. SEED)
if arg[4] then print(("  screen: %dx%d"):format(SCREEN_W, SCREEN_H)) end

--- A click in a dialog, at the dialog's own 640x480 coordinates: on a bigger
--- screen the dialogs sit in a centred frame (warlords/layout.lua).
local function dialogClick(x, y, button)
  local d = G.layout and G.layout.dialog or { x = 0, y = 0 }
  return love.mousepressed(x + d.x, y + d.y, button)
end
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
--- Let the computer's round, if one is being played, run through: a key
--- skips its moves, and any message on the way is put away.
function humanLeft()
  local game = require("warlords.game")
  for _, s in ipairs(G.g.sides) do
    if s.alive and not s.computer and #game.sideCities(G.g, s) > 0 then return true end
  end
  return false
end

function finishComputer()
  local kit = require("ui.kit")
  for _ = 1, 20000 do
    -- with no human left the computers fight on, as long as they like
    if not G.aiRun or (not humanLeft() and G.g.noHumansSaid and not kit.top()) then return end
    if kit.top() then love.keypressed("return")
    elseif not G.aiSkip then love.keypressed("x") end
    love.draw()
  end
  if G.aiRun then error("the computer's round never came back") end
end

local function dismissBanner(what)
  finishComputer()
  if G.banner and G.offer then
    fail("banner", "a hero was offered before the banner was dismissed")
  end
  if G.banner then try(what or "dismiss the banner", love.keypressed, "return") end
  if G.banner then fail("banner", "a key did not dismiss it") end
  -- in the tutorial its pages come next, once each: put them away
  local kit = require("ui.kit")
  for _ = 1, 5 do
    if not kit.top() then break end
    try("put a tutorial page away", love.keypressed, "space")
  end
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
  try("tick the other box", dialogClick, wasFemale and 390 or 494, 320, 1)
  if G.offerFemale == wasFemale then
    fail("turn-1 hero", "the checkbox did not change the hero's sex")
  end
  try("draw the other portrait", love.draw)
  try("type into the name", love.textinput, "x")
  if not G.offerName:find("x$") then fail("turn-1 hero", "the name field does not type") end
  try("rub it out again", love.keypressed, "backspace")

  -- the first hero is free, so Cancel must be refused
  try("press the disabled Cancel", dialogClick, 350, 350, 1)
  if not G.offer then fail("turn-1 hero", "Cancel closed an offer that cannot be refused") end

  local before = heroesOf(G.g, G.player)
  try("press OK", dialogClick, 510, 350, 1)
  local after = heroesOf(G.g, G.player)
  if after ~= before + 1 then
    fail("turn-1 hero", ("accepting gave %d heroes, wanted %d"):format(after, before + 1))
  else
    print("  the free turn-1 hero arrived")
  end
  -- on turn 1 the capital opens in Production next (8cc6:04bd)
  local kit = require("ui.kit")
  local found
  for _, d in ipairs(G.modals) do
    if d.city == G.player.capital and d.mode == 2 then found = true end
  end
  if not found then
    fail("turn-1 hero", "the capital's dialog did not open in Production after the hero")
  else
    print("  the capital opened in Production")
  end
  -- put the tutorial's pages and the dialog away
  for _ = 1, 6 do
    if not kit.top() then break end
    try("close a dialog", love.keypressed, "escape")
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

--- Game > End Turn, as a player types it: alt E. The computer then plays
--- across frames; unless `watch` is asked for, a key runs its moves through
--- at once, and the frames are drawn until the round comes back. Any message
--- on the way (the last human gone) is put away.
function endTurn(watch)
  HELD = { lalt = true }
  love.keypressed("e")
  HELD = nil
  if not watch then finishComputer() end
end

-- the turn sequence, several times over: this runs every computer player too
for i = 1, 3 do
  local turn = G.g.turn
  try("end turn " .. i, endTurn)
  if G.g.turn == turn and not G.over then fail("end turn", "alt E did not end the turn") end
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

-- The Grp button: 240 and 241 share its rect, and whichever is live is the
-- one a click reaches -- group the stack, then break it up again.
do
  local slotsMod = require("warlords.slots")
  G.city, G.openMenu, G.offer, G.banner = nil, nil, nil, nil
  for i = #G.modals, 1, -1 do G.modals[i] = nil end
  local tile
  for _, a in ipairs(G.g.armies) do
    if a.owner == G.player.index and a.x then
      local n = 0
      for _, b in ipairs(G.g.armies) do
        if b.owner == a.owner and b.x == a.x and b.y == a.y then n = n + 1 end
      end
      if n >= 2 then tile = a break end
    end
  end
  local grp
  for _, c in ipairs(G.screen.dialog.controls) do if c.id == 240 then grp = c end end
  if tile and grp then
    try("select a stack for Grp", function() G.selectAt(tile.x, tile.y) end)
    for i = #G.modals, 1, -1 do G.modals[i] = nil end
    local function click()
      try("draw", love.draw)                   -- refreshes the buttons' states
      try("press Grp", love.mousepressed, grp.x + 2, grp.y + 2, 1)
      try("release Grp", love.mousereleased, grp.x + 2, grp.y + 2, 1)
    end
    if slotsMod.grouped(G.selection.slots) then click() end
    if slotsMod.grouped(G.selection.slots) then fail("Grp", "could not ungroup to start") end
    click()
    if not slotsMod.grouped(G.selection.slots) then fail("Grp", "the click did not group the stack") end
    click()
    if slotsMod.grouped(G.selection.slots) then
      fail("Grp", "the click did not ungroup the stack")
    else
      print("  the Grp button groups and ungroups")
    end
    -- a single army's slot (89e0:0963) drops it out of the group and back
    local slot2
    for _, c in ipairs(G.screen.dialog.controls) do if c.id == 225 then slot2 = c end end
    local function clickSlot()
      try("draw", love.draw)
      try("press slot 2", love.mousepressed, slot2.x + 2, slot2.y + 2, 1)
      try("release slot 2", love.mousereleased, slot2.x + 2, slot2.y + 2, 1)
    end
    click()
    clickSlot()
    if slotsMod.grouped(G.selection.slots) then
      fail("slots", "clicking an army did not take it out of the group")
    end
    clickSlot()
    if not slotsMod.grouped(G.selection.slots) then
      fail("slots", "clicking it again did not put it back")
    else
      print("  an army's slot takes it out of the group and back")
    end
    G.selection = nil
  else
    print("  (no stack of two to try Grp on)")
  end
end

-- The hand drags the map (8065:0e04): 80 pixels down and right moves the
-- view's centre two tiles up and left.
do
  G.selection, G.drag = nil, nil
  for i = #G.modals, 1, -1 do G.modals[i] = nil end
  G.cx, G.cy = 20, 20
  local x, y
  for tx = 0, 8 do
    for ty = 0, 8 do
      local px, py = G.mapRect.x + tx * 40 + 20, G.mapRect.y + ty * 40 + 20
      if not x and G.pointerKind(px, py) == 4 then x, y = px, py end
    end
  end
  if x then
    try("press the hand", love.mousepressed, x, y, 1)
    try("drag", love.mousemoved, x + 80, y + 80, 80, 80)
    try("let go", love.mousereleased, x + 80, y + 80, 1)
    if G.cx ~= 18 or G.cy ~= 18 then
      fail("drag", ("the view went to %d,%d, not 18,18"):format(G.cx, G.cy))
    elseif G.drag then
      fail("drag", "the drag outlived the button")
    else
      print("  the hand drags the map")
    end
  end
end

-- 18a9:0896: a city is only attacked from beside it. Far off, the pointer
-- over it is the tower and a click opens it; next to it, the sword.
do
  local game = require("warlords.game")
  local mine
  for _, a in ipairs(G.g.armies) do
    if a.owner == G.player.index and a.x then mine = a break end
  end
  local target
  for _, c in ipairs(G.g.map.cities) do
    if c.ownerIndex ~= G.player.index and not c.razed then target = c break end
  end
  G.city, G.openMenu, G.offer, G.banner = nil, nil, nil, nil
  for i = #G.modals, 1, -1 do G.modals[i] = nil end
  if mine and target then
    local ox, oy = mine.x, mine.y
    local function pointAt(tx, ty)
      G.cx, G.cy = tx - 4, ty - 4
      return G.mapRect.x + 4 * 40 + 20, G.mapRect.y + 4 * 40 + 20
    end
    try("select a stack", function() G.selectAt(mine.x, mine.y) end)
    for i = #G.modals, 1, -1 do G.modals[i] = nil end   -- the tutorial's page
    -- put the stack far from the city, then right beside it
    local lead = G.selection and G.selection.stack[1] or mine
    ox, oy = lead.x, lead.y
    local function place(x, y) for _, a in ipairs(G.selection.stack) do a.x, a.y = x, y end end
    place(target.x + 6, target.y)
    local px, py = pointAt(target.x, target.y)
    local far = G.pointerKind(px, py)
    if far == 8 or far == 10 then fail("pointer", "the sword showed over a far city") end
    place(target.x - 1, target.y)
    local near = G.pointerKind(px, py)
    if near ~= 8 and near ~= 10 then
      fail("pointer", ("beside a city the pointer was %s, not the sword"):format(tostring(near)))
    end
    place(ox, oy)
    G.selection = nil
    for i = #G.modals, 1, -1 do G.modals[i] = nil end
    print("  the sword shows only beside a city")
  end
end

-- press and release every control of the main screen, on and off the button,
-- so the state handling runs for all of them
if G and G.screen then
  local uidata = require("warlords.uidata")
  local screenMod = require("warlords.screen")

  -- 8065:0174's rules: with nothing selected only the cycle's "next" is
  -- live, and picking a stack up brings the rest of the row with it
  local function stateOf(id) return G.screen.state[id] end
  G.selection = nil
  try("refresh with nothing selected", love.draw)
  if stateOf(176) ~= uidata.DISABLED then
    fail("button state", "fortify was live with nothing selected")
  end
  if stateOf(174) == uidata.DISABLED then
    fail("button state", "next army was greyed while armies remain")
  end
  if stateOf(173) ~= uidata.DISABLED then
    fail("button state", "walk on was live without a route")
  end
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
      -- a greyed-out button must not light up and must not act; a live one
      -- must do both
      local off = hit and G.screen.state[hit.id] == uidata.DISABLED
      try(("press control %d"):format(c.id), love.mousepressed, cx, cy, 1)
      if hit and not off and G.screen.state[hit.id] ~= uidata.ACTIVE then
        fail("control press", ("control %d did not light up"):format(hit.id))
      end
      if off and G.screen.state[hit.id] ~= uidata.DISABLED then
        fail("control press", ("disabled control %d lit up"):format(hit.id))
      end
      try(("release control %d"):format(c.id), love.mousereleased, cx, cy, 1)
      if hit and not off and G.screen.state[hit.id] == uidata.ACTIVE then
        fail("control release", ("control %d stayed lit"):format(hit.id))
      end
    end
  end
  -- released away from the button: must still reset, and must not act
  local c
  for _, k in ipairs(G.screen.dialog.controls) do
    if k.w > 0 and G.screen.state[k.id] ~= uidata.DISABLED then c = k break end
  end
  if c then
    try("press then leave", love.mousepressed, c.x + 1, c.y + 1, 1)
    try("release elsewhere", love.mousereleased, 5, 470, 1)
    if G.screen.state[c.id] == uidata.ACTIVE then
      fail("control release", "a button left pressed after releasing off it")
    end
  end
  print("  exercised every control on the main screen")
end

-- the rest of the keys: the original's own (17be:0064, 17be:0444), and the
-- letters that open a dialog -- which is closed again with its Done
local kitMod = require("ui.kit")
for _, key in ipairs({ "p", "c", "b", "v", "home", "up", "down", "left", "right",
                       "tab", "backspace", "delete", "end", "return", "escape",
                       "8", "6", "2", "4", "kp5", "space", "m", "f5", "f9" }) do
  try("key " .. key, love.keypressed, key)
  for _ = 1, 3 do
    if kitMod.top() then try("close what " .. key .. " opened", love.keypressed, "escape") end
  end
  if kitMod.top() then fail("keys", key .. " left a dialog open") end
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
        -- an item that opens a dialog: draw it, then close it again
        if kitMod.top() then try("draw " .. r.label, love.draw) end
        for _ = 1, 3 do
          if kitMod.top() then try("close " .. r.label, love.keypressed, "escape") end
        end
        picked = picked + 1
      end
    end
    G.openMenu = nil
  end
  print(("  opened %d menus and picked %d items"):format(#G.menuLayout, picked))
end

-- the city dialog: open one of our own cities and drive every mode, the
-- dialogs it opens, and the ways out
if G and G.g then
  local game = require("warlords.game")
  local kit  = require("ui.kit")
  local mine = game.sideCities(G.g, G.player)[1]

  local function control(view, id)
    for _, k in ipairs(view.dialog.controls) do if k.id == id then return k end end
  end
  local function press(view, id, what)
    local c = control(view, id)
    if not c then fail("city dialog", ("no control %d"):format(id)) return end
    try(what or ("press %d"):format(id), dialogClick, c.x + 2, c.y + 2, 1)
    try("draw after " .. (what or id), love.draw)
  end

  if not mine then
    fail("city dialog", "the player holds no city to open")
  else
    try("open the city dialog", G.openCity, mine)
    if G.city ~= mine then fail("city dialog", "it did not open") end
    if G.cityMode ~= 2 then fail("city dialog", "our own city should open on Production") end
    try("draw the city dialog", love.draw)
    local view = G.cityView

    for id = 197, 200 do press(view, id, ("pick slot %d"):format(id - 196)) end
    for mode = 0, 3 do
      press(view, 193 + mode, ("city mode %d"):format(mode))
      if G.cityMode ~= mode then
        fail("city dialog", ("button %d did not select mode %d"):format(193 + mode, mode))
      end
    end

    -- City mode: rename, and back out of razing
    press(view, 194, "City mode")
    press(view, 203, "Rename")
    local ask = kit.top()
    if ask == nil or ask.close ~= nil then fail("rename", "no text entry came up") end
    press(ask.view, 191, "click the name field")
    try("type a name", love.textinput, "Newburg")
    try("end the edit", love.keypressed, "return")
    press(ask.view, 189, "OK")
    if mine.name ~= "Newburg" then fail("rename", ("the city is called %q"):format(mine.name)) end
    press(view, 205, "Raze")
    ask = kit.top()
    press(ask.view, 190, "Cancel the raze")
    if mine.razed then fail("raze", "Cancel razed the city") end
    if G.cityMode ~= 1 then fail("raze", "Cancel did not go back to City mode") end

    -- Build Prod: buy whatever is for sale, then Done
    G.player.gold = 5000
    press(view, 204, "Build Prod")
    local b = kit.top()
    if not b or not b.types then fail("build production", "the screen did not open") end
    if b then
      local before = #mine.slots
      for n = 1, #b.types do
        if b.view.state[400 + n] ~= 2 then press(b.view, 400 + n, "buy a type") break end
      end
      if #mine.slots == before and before < 4 then fail("build production", "nothing was bought") end
      press(b.view, 396, "Done")
      if G.cityMode ~= 1 then fail("build production", "Done did not return to City mode") end
    end

    -- Vector mode: send this city's armies to another of ours, with the
    -- send button and then a click on the map; then See All; then a plain
    -- click on the other city moves the dialog there
    local other
    for _, c in ipairs(G.g.map.cities) do
      if c ~= mine and not c.razed then other = c break end
    end
    local wasOwner = other.ownerIndex
    other.ownerIndex = G.player.index
    press(view, 195, "Production mode")
    press(view, 197, "build something to vector")
    press(view, 196, "Vector mode")
    press(view, 210, "send the armies elsewhere")
    try("click the other city on the map", dialogClick,
        80 + other.x * 2 + 1, 60 + other.y * 2 + 1, 1)
    try("draw the vector map", love.draw)
    if mine.vectorTo ~= other.index then fail("vector", "the click did not set the vector") end
    press(view, 212, "See All")
    press(view, 216, "See All off")
    try("click the other city again", dialogClick,
        80 + other.x * 2 + 1, 60 + other.y * 2 + 1, 1)
    if G.city ~= other then fail("vector", "a plain click did not move the dialog") end
    try("back to our city", function() love.keypressed("escape") G.openCity(mine) end)
    mine.vectorTo = nil
    other.ownerIndex = wasOwner

    -- Stop in Production, then Done
    press(view, 195, "Production mode")
    press(view, 202, "Stop")
    if mine.producing ~= nil then fail("city dialog", "Stop did not stop production") end
    press(view, 201, "Done")
    if G.city ~= nil then fail("city dialog", "Done did not close it") end

    -- a click outside does nothing: the original's dialog is modal
    G.openCity(mine)
    try("click outside the panel", love.mousepressed, 10, 460, 1)
    if G.city == nil then fail("city dialog", "a click outside closed it") end
    try("close with escape", love.keypressed, "escape")
    if G.city ~= nil then fail("city dialog", "escape did not close it") end

    -- someone else's city opens on Info, with the other modes greyed
    for _, c in ipairs(G.g.map.cities) do
      if c.ownerIndex ~= G.player.index then
        G.openCity(c)
        if G.cityMode ~= 0 then fail("city dialog", "a foreign city should open on Info") end
        press(view, 195, "Production on a foreign city")
        if G.cityMode ~= 0 then fail("city dialog", "a foreign city let Production in") end
        try("close it", love.keypressed, "escape")
        break
      end
    end
    print("  drove the city dialog")
  end
end

-- clicking the status bar must be ignored, not crash
try("click the status bar", love.mousepressed, 100, 750, 1)

-- make sure no dialog is left open: while one is, clicks are swallowed, the
-- player never moves and the game below never reaches an end
finishComputer()
G.city, G.openMenu, G.offer, G.banner = nil, nil, nil, nil
for i = #G.modals, 1, -1 do G.modals[i] = nil end

-- The map's zoom (not the original's): at every zoom a click on the middle of
-- a stack's tile picks that stack up, and the view stays on the map.
do
  for i = #G.modals, 1, -1 do G.modals[i] = nil end
  G.selection, G.openMenu, G.route = nil, nil, nil
  local mine
  for _, a in ipairs(G.g.armies) do
    if a.owner == G.player.index and a.x and not a.transit then mine = a break end
  end
  local was, zooms = G.zoom, 0
  for z = 1, 5 do
    G.setZoom(z)
    if mine and G.zoom == z then
      zooms = zooms + 1
      G.centreOn(mine.x, mine.y)
      local x, y = G.mapToUI(mine.x + 0.5, mine.y + 0.5)
      try(("click a stack at zoom %d"):format(z), love.mousepressed, math.floor(x), math.floor(y), 1)
      local sel = G.selection
      if not sel or sel.x ~= mine.x or sel.y ~= mine.y then
        fail("zoom", ("at zoom %d the click missed the stack at %d,%d"):format(z, mine.x, mine.y))
      end
      local vw, vh = G.viewSize()
      if G.cx < 0 or G.cy < 0 or G.cx + vw > G.g.map.width + 1e-6
         or G.cy + vh > G.g.map.height + 1e-6 then
        fail("zoom", ("at zoom %d the view left the map"):format(z))
      end
      try(("draw at zoom %d"):format(z), love.draw)
      for i = #G.modals, 1, -1 do G.modals[i] = nil end   -- the tutorial's page
      G.selection = nil
    end
  end
  G.setZoom(was)
  print(("  clicked a stack at %d zooms"):format(zooms))
end

-- Observe (8cc6:0000): a watched computer's walks are shown one by one, the
-- camera on each, and a key runs the rest of the round through
do
  for _, side in ipairs(G.g.sides) do side.observe = true end
  local turn = G.g.turn
  try("end a watched turn", endTurn, true)
  local frames = 0
  while G.aiRun and not (G.walk and G.walk.computer) and frames < 2000 do
    try("watch a frame", love.draw)
    frames = frames + 1
  end
  local watched = G.walk ~= nil and G.walk.computer
  -- holding Shift between the computer's turns opens Settings (5db9:045e)
  if G.aiRun then
    local kit = require("ui.kit")
    HELD = { lshift = true }
    for _ = 1, 200 do
      if kit.top() or not G.aiRun then break end
      if G.walk and G.walk.computer then
        G.walk = nil
        try("cut a walk short", G.resumeComputer)
      end
      try("draw with Shift held", love.draw)
    end
    HELD = nil
    if G.aiRun and not kit.top() then fail("observe", "Shift did not open Settings")
    elseif kit.top() then
      print("  Shift during the computer's turns opens Settings")
      try("close Settings", love.keypressed, "return")
    end
  end
  try("skip the rest", finishComputer)
  if G.aiRun then fail("observe", "a key did not run the round through") end
  if G.g.turn == turn and not G.over then fail("observe", "the round never came back") end
  if watched then print("  a computer's walk was shown")
  else print("  (no computer walk in sight to show)") end
  dismissBanner()
  dismissOffer()
end

-- play on until the game ends, so the end-of-game path runs too
-- 150 rounds is enough to reach the end in the small scenarios, and to run
-- the turn machinery hard in the large ones. Stop as soon as it is over:
-- every further turn is a full round of computer players for nothing.
for _ = 1, 150 do
  try("long game", endTurn)
  dismissBanner()
  dismissOffer()
  if G.over or not humanLeft() then break end
end
try("final frame", love.draw)
print(("  the game reached turn %d%s"):format(G.g.turn,
  G.over and ", and ended" or (not humanLeft() and ", the war going on without a human" or "")))
G.aiRun = nil

-- the start screens keep the menu bar, with only Quit and Load game live
-- (7f77:0200)
do
  local kit = require("ui.kit")
  for i = #G.modals, 1, -1 do G.modals[i] = nil end
  G.over = nil
  if try("open the start screens", openStart) then
    try("draw the start screen", love.draw)
    try("open the Game menu", love.mousepressed, 60, 4, 1)
    if G.openMenu ~= 2 then fail("start menu bar", "the Game menu did not open") end
    try("draw it open", love.draw)
    if G.menuEnabled("alt S") then fail("start menu bar", "Save game was live") end
    if not G.menuEnabled("ui scale 1") then fail("start menu bar", "the interface's scale was greyed") end
    if G.menuEnabled("map zoom 1") then fail("start menu bar", "the map's zoom was live") end
    if not G.menuEnabled("^Q") then fail("start menu bar", "Quit was greyed") end
    try("close it", love.keypressed, "escape")
    if G.openMenu then fail("start menu bar", "escape left the menu open") end
    if not kit.top() then fail("start menu bar", "escape closed the start screen") end
    print("  the start screens keep the menu bar")
  end
end

os.remove("warlords-save.lua")

if failures == 0 then
  print("\nthe front end ran clean")
  os.exit(0)
end
print(("\n%d failed"):format(failures))
os.exit(1)
