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
    if not G.aiRun or (not humanLeft() and G.g.noHumansSaid and not kit.top()) then
      if not G.aiRun and G.aiStatus and not G.over then
        fail("computer turn", "the status bar stayed the computer's")
      end
      return
    end
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
  -- the view left in a corner: the next turn opens on the capital (8cc6:0259)
  G.cx, G.cy = 0, 0
  try("end turn " .. i, endTurn)
  if G.g.turn == turn and not G.over then fail("end turn", "alt E did not end the turn") end
  dismissBanner("dismiss the banner on turn " .. i)
  local cap = G.player.capital
  if cap and not G.over and (cap.x < G.cx or cap.x >= G.cx + 9 or cap.y < G.cy or cap.y >= G.cy + 9) then
    fail("turn view", ("turn %d opened at %d,%d, away from the capital at %d,%d"):format(
      G.g.turn, G.cx, G.cy, cap.x, cap.y))
  end
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
      -- held down across a frame: the refresh each frame runs must not
      -- take the pressed look away
      try(("draw with control %d held"):format(c.id), love.draw)
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
      -- a shortcut that opens a dialog (Heroes: Hero > Inspect): close it
      for _ = 1, 3 do
        if require("ui.kit").top() then try(("close what %d opened"):format(c.id), love.keypressed, "escape") end
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

-- The spoils of a city (63fa:0000): Occupy opens the city in Production
-- (63fa:043a); Pillage and Sack show what they took and Raze says the city
-- is in ruins, each over the spoils dialog, which goes when they are closed.
do
  local kitMod = require("ui.kit")
  local cityMod = require("ui.city")
  local uidata = require("warlords.uidata")
  local function clickChoice(id)
    for _, c in ipairs(G.victoryView.dialog.controls) do
      if c.id == id then
        try("choose " .. id, love.mousepressed, c.x + 2, c.y + 2, 1)
        return
      end
    end
    fail("spoils", "no control " .. id)
  end
  local cap = G.player.capital
  G.presentVictory(cap, "Tester")
  try("occupy", love.keypressed, "return")
  local top = kitMod.top()
  if not (top and top.mode == cityMod.PRODUCTION) then
    fail("occupy", "the city did not open in Production")
  end
  for _ = 1, 3 do if kitMod.top() then try("close the city", love.keypressed, "escape") end end
  for _, id in ipairs({ 286, 283 }) do                  -- Sack, Pillage
    G.presentVictory(cap, "Tester")
    if G.victoryView.state[id] == uidata.DISABLED then
      try("occupy instead", love.keypressed, "return")
      for _ = 1, 3 do if kitMod.top() then try("close the city", love.keypressed, "escape") end end
    else
      clickChoice(id)
      if not kitMod.top() then fail("spoils", ("choice %d showed nothing"):format(id)) end
      try("draw the spoils " .. id, love.draw)
      try("close the spoils " .. id, love.keypressed, "return")
      if kitMod.top() then fail("spoils", ("choice %d would not close"):format(id)) end
    end
  end
  local enemy
  for _, c in ipairs(G.g.map.cities) do
    if c.ownerIndex ~= G.player.index and not c.razed then enemy = c break end
  end
  G.presentVictory(enemy, "Tester")
  clickChoice(284)                                      -- Raze
  try("draw the ruins message", love.draw)
  if enemy.razed then fail("raze", "the city fell before the message was read") end
  try("read it", love.keypressed, "return")
  if not enemy.razed then fail("raze", "the city was not razed") end
  try("frame after the spoils", love.draw)
  print("  took a city every way")

  -- a quest's end (quest_check, 4976:1ded): Occupy without the quest's hero
  -- says why the quest failed, and only then opens the city
  local quests = require("warlords.quest")
  local hero
  for _, a in ipairs(G.g.armies) do
    if a.owner == G.player.index and a.type == 28 then hero = a end
  end
  hero = hero or { name = "Tester", x = cap.x, y = cap.y }
  G.selection = nil
  G.player.quest = { type = quests.OCCUPY, hero = hero, target = cap, done = 0 }
  G.presentVictory(cap, "Tester")
  try("occupy the quest's city without its hero", love.keypressed, "return")
  local top = kitMod.top()
  if not top or top.mode ~= nil then fail("quest", "the failure was not said first") end
  try("draw the failure", love.draw)
  try("read it", love.keypressed, "return")
  top = kitMod.top()
  if not (top and top.mode == cityMod.PRODUCTION) then
    fail("quest", "the city did not open after the failure")
  end
  for _ = 1, 3 do if kitMod.top() then try("close the city", love.keypressed, "escape") end end
  if G.player.questNews then fail("quest", "the failure was told twice") end

  -- and whatever else ends a quest is told once the screen is free: each
  -- kind of reward, and a failure
  local q = { hero = hero }
  for _, news in ipairs({
    { quest = q, reward = { kind = "gold", gold = 1500 } },
    { quest = q, reward = { kind = "allies", armies = {}, type = G.g.types.byId[1] } },
    { quest = q, reward = { kind = "item", item = G.g.map.items[1] } },
    { quest = q, reward = { kind = "revealed", site = G.g.map.sites[1] } },
    { quest = q, failed = "the hero is lost", why = 0x20 },
  }) do
    G.player.questNews = news
    try("frame with quest news", love.draw)
    if not kitMod.top() then fail("quest", "news was not shown: " .. (news.failed or news.reward.kind)) end
    try("draw quest news", love.draw)
    try("close quest news", love.keypressed, "return")
    if kitMod.top() then fail("quest", "news would not close") end
  end
  print("  told how quests end")
end

-- A city taken by the player: the map shows the fight as it began until the
-- battle window closes (after_battle, 67cc:0a6b), then the city changes
-- hands and the spoils are asked for
do
  local g = G.g
  local game2, scn = require("warlords.game"), require("warlords.scn")
  local target
  for _, c in ipairs(g.map.cities) do
    if not target and not c.razed and c.ownerIndex ~= G.player.index then target = c end
  end
  if target and not G.over then
    local x, y = target.x - 1, target.y
    -- a garrison of one, for a fight that is sure to be won
    local kept = false
    for i2 = #g.armies, 1, -1 do
      local a = g.armies[i2]
      if a.x and a.x >= target.x and a.x <= target.x + 1 and a.y >= target.y and a.y <= target.y + 1 then
        if kept then table.remove(g.armies, i2) else kept = true end
      end
    end
    local stack = {}
    for k = 1, 8 do
      local a = { owner = G.player.index, type = 11, name = "Giants", strength = 9,
                  maxMoves = 20, moves = 20, upkeep = 1, x = x, y = y }
      g.armies[#g.armies + 1] = a
      stack[k] = a
    end
    G.selection, G.banner, G.offer, G.assault = nil, nil, nil, nil
    for _ = 1, 5 do
      if require("ui.kit").top() then try("close a dialog", love.keypressed, "escape") end
    end
    try("select the attackers", G.selectAt, x, y)
    if G.selection then
      require("warlords.slots").all(G.selection.slots, g)
      G.afterSlotChange()
    end
    local i = target.y * scn.MAP_W + target.x + 1
    local tile, owner = g.map.tiles[i], target.ownerIndex
    -- a click on the city with the sword up (740d:0179)
    G.cx, G.cy = target.x - 4, target.y - 4
    local px = G.mapRect.x + (target.x - G.cx) * 40 + 20
    local py = G.mapRect.y + (target.y - G.cy) * 40 + 20
    -- whatever selecting them opened (the tutorial's notes)
    for _ = 1, 5 do
      if require("ui.kit").top() then try("close a dialog", love.keypressed, "escape") end
    end
    try("attack the city", love.mousepressed, px, py, 1)
    try("release on the city", love.mousereleased, px, py, 1)
    if not G.assault then
      fail("assault", "no battle for a city beside the stack")
    else
      local start = os.clock()
      while G.assault and os.clock() - start < 20 do
        try("frame of an assault", love.draw)
        if G.assault and (g.map.tiles[i] ~= tile or target.ownerIndex ~= owner or stack[1].x ~= x) then
          fail("assault", "the map changed before the battle was seen") break
        end
        try("hurry the assault", love.keypressed, "space")
      end
      if G.assault then fail("assault", "the window never closed") end
      if target.ownerIndex ~= G.player.index then fail("assault", "the city was not taken")
      elseif g.map.tiles[i] == tile then fail("assault", "the castle kept its old colours")
      elseif not G.victory then fail("assault", "no spoils asked for") end
      G.victory = nil
      for _ = 1, 3 do
        if require("ui.kit").top() then try("close the tutorial", love.keypressed, "escape") end
      end
      print("  a city changes hands only once its battle has been seen")
    end
    for i2 = #g.armies, 1, -1 do
      for _, s2 in ipairs(stack) do if g.armies[i2] == s2 then table.remove(g.armies, i2) break end end
    end
    G.selection = nil
  end
end

-- A computer attacking a human (67cc:124a): the battle window opens and
-- closes by itself, and the outcome goes up in the status bar; a fallen
-- army's fire burns out and leaves its place bare (6a35:0000)
do
  local g, ai = G.g, require("warlords.ai")
  local game2 = require("warlords.game")
  local comp, human
  for _, s in ipairs(g.sides) do
    if s.alive and s.computer and not comp then comp = s end
    if s.alive and not s.computer and not human then human = s end
  end
  local att, def
  for _, a in ipairs(g.armies) do
    if comp and a.owner == comp.index and a.x and not att then att = a end
    if human and a.owner == human.index and a.x and not def then def = a end
  end
  if att and def and not G.over then
    G.selection, G.assault, G.banner, G.offer = nil, nil, nil, nil
    human.observe = false
    local result = game2.decideAttack(g, { att }, def.x, def.y)
    local armies = #g.armies
    G.aiSide, G.aiSkip = comp, nil
    G.aiStatus = { side = comp, text = comp.name, progress = 50 }
    G.aiRun = coroutine.create(function() ai.onFight(g, { att }, def.x, def.y, result) end)
    local ok1, what = coroutine.resume(G.aiRun)
    if not ok1 then fail("computer battle", tostring(what)) end
    -- the "being attacked" line waits its five ticks first
    local start = os.clock()
    while what == "pause" and os.clock() - start < 5 do
      ok1, what = coroutine.resume(G.aiRun)
    end
    local a = G.assault
    if not (a and a.computer and a.window) then
      fail("computer battle", "no battle window for an attack on a human")
    else
      while G.assault and os.clock() - start < 20 do
        if #g.armies ~= armies then fail("computer battle", "the dead left the map before the battle was seen") break end
        try("frame of a computer battle", love.draw)
      end
      if #g.armies == armies then fail("computer battle", "the fight never took effect") end
      if G.assault then fail("computer battle", "the window never closed by itself")
      elseif G.aiStatus.text ~= a.outcome then fail("computer battle", "the outcome was not in the status bar") end
      local t = love.timer.getTime()
      for _, fell in ipairs(a.defFell) do
        if t < fell.at + fell.burn then fail("computer battle", "a fire still burned at the end") end
      end
      if #a.defFell + #a.atkFell == 0 then fail("computer battle", "nobody fell") end
    end
    G.aiRun, G.aiSide, G.aiStatus, G.assault = nil, nil, nil, nil
    print("  a computer's battle with a human is shown, and closes itself")
  end
end


-- Order > Move All (1c8c:04c4) walks each stack under orders in turn, the view
-- following, and leaves the last one moved selected
do
  local moveMod = require("warlords.move")
  local sent = 0
  for _, a in ipairs(G.g.armies) do
    if a.owner == G.player.index and not a.transit and sent == 0 then
      -- somewhere a few tiles off that it can get to this turn
      for r = 3, 2, -1 do
        for dx = -r, r do
          local p = sent == 0 and moveMod.preview(G.g, { a }, a.x + dx, a.y + r)
          if p and p.path and #p.path > 0 and (p.reach or 0) > 0 then
            a.target, a.fortified, sent = { x = a.x + dx, y = a.y + r }, false, 1
          end
        end
      end
    end
  end
  G.selection = nil
  try("move all", love.keypressed, "m")
  -- while it walks, a click is lost and a key waits (1a8b:04c8)
  if G.moveAll then
    local sel = G.selection
    try("click during move all", love.mousepressed, 200, 200, 1)
    try("release during move all", love.mousereleased, 200, 200, 1)
    if G.selection ~= sel then fail("move all", "a click during the walks was acted on") end
    try("key during move all", love.keypressed, "a")
    if require("ui.kit").top() then fail("move all", "a key during the walks was acted on") end
    if #G.keyBuffer ~= 1 then fail("move all", "the key was not kept for later") end
  end
  local frames, start = 0, os.clock()
  while G.moveAll and os.clock() - start < 20 do
    try("frame of move all", love.draw)
    frames = frames + 1
  end
  if G.moveAll then fail("move all", "the walks never finished") end
  if sent > 0 then
    if #G.keyBuffer ~= 0 then fail("move all", "the key kept was never read") end
    if not require("ui.kit").top() then fail("move all", "the key kept did nothing once read") end
    for _ = 1, 3 do
      if require("ui.kit").top() then try("close the report", love.keypressed, "escape") end
    end
  end
  local sel = G.selection
  if sent > 0 and sel then
    local x, y = sel.stack[1].x, sel.stack[1].y
    if x < G.cx or x >= G.cx + 9 or y < G.cy or y >= G.cy + 9 then
      fail("move all", "the view did not follow the last stack moved")
    end
  end
  print(("  moved all: %d armies sent, %d frames"):format(sent, frames))
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
    try(what or ("press %d"):format(id), love.mousepressed, c.x + 2, c.y + 2, 1)
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
    try("click the other city on the map", love.mousepressed,
        80 + other.x * 2 + 1, 60 + other.y * 2 + 1, 1)
    try("draw the vector map", love.draw)
    if mine.vectorTo ~= other.index then fail("vector", "the click did not set the vector") end
    press(view, 212, "See All")
    press(view, 216, "See All off")
    try("click the other city again", love.mousepressed,
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

-- Observe (8cc6:0000): a watched computer's walks are shown one by one, the
-- camera on each, and a key runs the rest of the round through
do
  for _, side in ipairs(G.g.sides) do side.observe = true end
  local turn = G.g.turn
  try("end a watched turn", endTurn, true)
  local frames = 0
  -- a computer's turn has no banner; its name and a progress bar that only
  -- grows are in the status bar (8cc6:0000, 5db9:0000)
  local lastSide, lastProgress, grew = nil, 0, false
  while G.aiRun and not (G.walk and G.walk.computer) and frames < 2000 do
    try("watch a frame", love.draw)
    frames = frames + 1
    if G.banner then fail("computer turn", "a banner went up for a computer") break end
    local st = G.aiStatus
    if G.aiRun and not st then fail("computer turn", "no status during a computer's turn") break end
    if st then
      if not st.side.computer then fail("computer turn", "the status is a human's") break end
      if st.side ~= lastSide then lastSide, lastProgress = st.side, 0 end
      if st.progress < lastProgress then fail("computer turn", "the progress bar went back") break end
      if st.progress > lastProgress and lastProgress > 0 then grew = true end
      lastProgress = st.progress
    end
  end
  if frames > 0 and not grew and G.aiRun and not (G.walk and G.walk.computer) then
    fail("computer turn", "the progress bar never moved")
  end
  print(("  computer turns: status bar, progress to %d over %d frames"):format(lastProgress, frames))
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
