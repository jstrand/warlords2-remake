-- The start screens: the menu the game opens on, choosing a scenario, and
-- setting its sides and options up (7f77:0000, 7f77:0725, 7bab:0000).
--
-- **The start menu** (7f77:0000, dialog 1): STARTUP0-3.PCK, the four
-- quarters of the screen, with New Scenario (100), Load Game (101), Random
-- Map (102) and Begin (103). On the right the scenario's own
-- PICS\SCENARIO.PCK -- its (0, 0) 264x225 at (328, 200) -- under a colour-3
-- bar at (336, 166) 248x28 with its name centred on (460, 172) (7f77:02bf,
-- 0332). The scenario starts as Erythea (4125:2a6e).
--
-- **Random Map** (7f77:05f5) puts "A Random World" on the bar and, in place of
-- the picture (7f77:0332), its settings on colour 3: four sliders -- Water,
-- Hills, Cities, Forest, 106-109, STARTBU.PCK at (384, 216) 30 apart -- each
-- with a "?" (110-113) that leaves it to chance, the terrain set (104) and
-- "Cities can produce allies" (105). Begin then has the advisor say "One
-- moment..." and makes the world (random_map_setup, 7bab:10e8) on
-- BSCROLL.PCK with a bar (4bed:01ff). New Scenario's choice ends it.
--
-- **New Scenario** (7f77:058d, 0725): popup 19, NEWSCEN.PCK, dialog 29. The
-- scenarios of SCENARIO.DAT -- 84-byte records: name at +0, directory at
-- +20, description at +28, and at +76 its cities, ruins and players -- in a
-- black box at (112, 153) 136x140, seven rows 20 apart, the chosen one in
-- colour 15 and the rest in 5 (7f77:0d45); on the crystal ball, in font 2
-- colour 5, the name centred on (416, 101), the description on (416, 174),
-- the cities ending at x = 368 and the ruins from x = 472 at y = 234, and the
-- players centred on (416, 254). 476-482 choose a row, 470-473 scroll, OK
-- (474) takes it, Cancel (475).
--
-- **Begin** (7bab:0000) sets the sides up on the main screen's own frame
-- (8065:0a10), dialog 3: in the map's panel a box a side, (24, 40) and
-- (208, 40) 160x70, 90 apart down two columns (4125:23c8), framed in the
-- side's colour and edge colour, its name on a colour-3 tab at (48, 32 +
-- 90 i) in its colours, its face from SETUPBU.PCK at (40, 56 + 90 i), its
-- button at (96, 52 + 90 i): Human, Knight, Lord, Warlord or Off -- and its
-- Character box at (88, 79 + 90 i) (7bab:0634). A click on the button
-- (125-132) goes round those five (7bab:0a4e). In the right-hand panel Begin
-- (141), Main Menu (142), "I am the Greatest" (143: every side a Warlord) or
-- "No! I really am Normal" (144), Beginner / Intermediate / Advanced
-- (145-147, lit when the options are theirs) and Edit Options (148); under
-- the map the difficulty rating (group 6) centred on (196, 426) (7bab:0bab),
-- with Recall Options (157) to its left and Random Characters (158) to its
-- right. The options are DATA\OPTIONS.DAT's, the ones the last game began
-- with, not the scenario's own (7bab:223b).
--
-- **Edit Options** (7bab:12a2): popup 4, dialog 4. "Game Options" (group 8),
-- "Affecting Difficulty" and "Not Affecting Difficulty" (groups 9 and 10)
-- over rules at y = 111 and 250; the ten options of group 4 two to a row --
-- names from x = 128 and 320, 30 apart from y = 131 and 270, their values
-- (Average/Strong/Active, On, Off) in colour 7 128 to the right. 159-168
-- change one, 169-171 are the presets, OK (172).
--
-- **Setup Side** (7bab:16ea), from a side's Character box (133-140) or its
-- face (149-156): popup 4, dialog 27 -- the side's name to retype, and for a
-- computer the characters of its level's deck to choose from, with the
-- chosen one's description (7bab:180b). OK (457), Cancel (458).

local kit    = require("ui.kit")
local pck    = require("warlords.pck")
local scn    = require("warlords.scn")
local uidata = require("warlords.uidata")
local aicard = require("warlords.aicard")
local randommap = require("warlords.randommap")
local input  = require("ui.input")

local M = {}

-- the ten options, in the order of group 4 and the table at 4125:23b4
local OPTION_KEYS = {
  [0] = "neutralCities", "diplomacy", "quests", "hiddenMap", "viewEnemies",
  "viewProduction", "intenseCombat", "quickStart", "militaryAdvisor", "randomTurns",
}
local PRESETS = {                                    -- 4125:2378
  [0] = { [0] = 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
  { [0] = 1, 1, 1, 0, 0, 0, 0, 0, 1, 0 },
  { [0] = 2, 1, 1, 1, 0, 1, 0, 0, 1, 0 },
}

-- The options being set up (4125:23b4): one table for the session, the
-- Beginner preset to begin with.
local OPTIONS = {}
for i = 0, 9 do OPTIONS[i] = PRESETS[0][i] end

-- The random world's settings, kept for the session as the original keeps
-- them (4125:28c8-28d8): on, the terrain set, allies, and each slider's
-- place and whether it is set (else "?", left to chance).
local world = { on = false, terrainSet = 0, allies = false, sliders = { 3, 3, 2, 3 }, set = { true, true, true, true } }

local function u16(s, i) return s:byte(i + 1) + s:byte(i + 2) * 256 end
local function cstr(s, a, n) return (s:sub(a + 1, a + n):match("^[^%z]*")) end

--- 7bab:223b: put back the options the last game began with, ten u16s in
--- DATA\OPTIONS.DAT -- the setup screen as it opens, and Recall Options.
--- With no file the table is left as it is.
local function recallOptions(dataDir)
  local f = io.open(dataDir .. "/DATA/OPTIONS.DAT", "rb")
  if not f then return end
  local s = f:read("*a") or ""
  f:close()
  for i = 0, 9 do
    if 2 * i + 2 <= #s then OPTIONS[i] = u16(s, 2 * i) end
  end
end

--- 7bab:2289: keep them for next time, as Begin starts the game.
local function keepOptions(dataDir)
  local f = io.open(dataDir .. "/DATA/OPTIONS.DAT", "wb")
  if not f then return end
  for i = 0, 9 do f:write(string.char(OPTIONS[i] % 256, math.floor(OPTIONS[i] / 256) % 256)) end
  f:close()
end

--- SCENARIO.DAT's records.
function M.scenarios(dataDir)
  local f = io.open(dataDir .. "/DATA/SCENARIO.DAT", "rb")
  if not f then return {} end
  local s = f:read("*a")
  f:close()
  local out = {}
  for i = 0, math.floor(#s / 84) - 1 do
    local o = 84 * i
    out[#out + 1] = {
      name = cstr(s, o, 20), dir = cstr(s, o + 20, 8), text = cstr(s, o + 28, 30),
      cities = u16(s, o + 76), ruins = u16(s, o + 78), players = u16(s, o + 80),
    }
  end
  return out
end

local drawWorld, makeWorld

local function image(G, path, key)
  G.startArt = G.startArt or {}
  if G.startArt[path] == nil then
    local ok, img = pcall(pck.toImage, path, G.palette, key)
    G.startArt[path] = ok and img or false
  end
  return G.startArt[path] or nil
end

------------------------------------------------------------------ the setup

--- The state the setup screens change: the options and every side's
--- player, read from the scenario as the loader leaves them.
local function newSetup(G, sc)
  local map = scn.load(G.dataDir .. "/" .. sc.dir:upper(), sc.dir:upper())
  -- 7bab:0000: the tutorial plays with the Beginner options, any other
  -- scenario with the last game's
  if (map.options.tutorial or 0) ~= 0 then
    for i = 0, 9 do OPTIONS[i] = PRESETS[0][i] end
  else
    recallOptions(G.dataDir)
  end
  local st = { sc = sc, map = map, options = OPTIONS, sides = {}, greatest = false }
  for _, s in ipairs(map.sides) do
    st.sides[s.index] = { inUse = s.inUse, name = s.name, colour = s.colour, edge = s.edge,
                          computer = s.computer, level = s.computer and (s.level or 0) or 0,
                          card = s.card or 0 }
  end
  return st
end

local function presetMatches(st, p)
  for i = 0, 5 do if st.options[i] ~= PRESETS[p][i] then return false end end
  return true
end

--- 7bab:0bab: the options' weight and the computers' strength, as a percent.
local function rating(st)
  local o = st.options
  local cx = math.min(20, o[0] * 4 + o[1] * 4 + o[2] * 3 + o[3] * 4 - o[4] + o[5])
  local sum, n = 0, 0
  for i = 0, 7 do
    local s = st.sides[i]
    if s and s.inUse and s.computer and s.level ~= 3 then sum, n = sum + s.level + 1, n + 1 end
  end
  local v = n == 0 and 80 or math.floor(sum * 80 / (n * 3))
  if v >= 78 then v = 80 end
  return v + cx
end

local function openOptions(G, st, after)
  local R = { x = 120, y = 50, w = 400, h = 360 }   -- popup 4
  local d = { view = kit.view(4) }
  local function refresh()
    local s = d.view.state
    for i = 0, 9 do s[159 + i] = uidata.NORMAL end
    for p = 0, 2 do s[169 + p] = presetMatches(st, p) and uidata.ACTIVE or uidata.NORMAL end
    s[172] = uidata.NORMAL
  end
  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(8, 0), 320, 55)
    local f = kit.font(2)
    for k, y in ipairs({ 111, 250 }) do
      kit.setPal(15) love.graphics.rectangle("fill", 160, y, 320, 1)
      kit.setPal(0) love.graphics.rectangle("fill", 161, y + 1, 320, 1)
      local t = kit.text(8 + k, 0)
      local w = f.width(t)
      kit.setPal(3) love.graphics.rectangle("fill", 320 - math.floor(w / 2) - 4, y, w + 8, 2)
      love.graphics.setColor(1, 1, 1)
      kit.centred(f, t, 320, y - 7)
    end
    for i = 0, 9 do
      local x, y
      if i < 6 then x, y = (i % 2 == 0) and 128 or 320, 131 + 30 * math.floor(i / 2)
      else x, y = (i % 2 == 0) and 128 or 320, 270 + 30 * math.floor((i - 6) / 2) end
      love.graphics.setColor(1, 1, 1)
      f.draw(kit.text(4, i), x, y)
      local v = st.options[i]
      local word
      if i == 0 then word = kit.text(5, v)
      else word = (v ~= 0) and "On" or "Off" end
      f.colours(7, 0).draw(word, x + 128, y)
    end
    kit.drawControls(d.view)
  end
  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    local id = c.id
    if id == 172 then kit.pop(d) return after() end
    if id >= 159 and id <= 168 then
      local i = id - 159
      if i == 0 then st.options[0] = (st.options[0] + 1) % 3
      else st.options[i] = st.options[i] ~= 0 and 0 or 1 end
    elseif id >= 169 and id <= 171 then
      for k = 0, 5 do st.options[k] = PRESETS[id - 169][k] end
    end
    refresh()
  end
  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then kit.pop(d) after() end
  end
  refresh()
  return kit.push(d)
end

local RECTS = {}                                     -- 4125:23c8
for i = 0, 7 do RECTS[i] = { x = i < 4 and 24 or 208, y = 40 + 90 * (i % 4), w = 160, h = 70 } end
-- SETUPBU.PCK's faces (4125:2408): Knight, Lord, Warlord, Off, the two
-- humans, a side not in the scenario, then a Knight, Lord and Warlord
-- playing a character other than the Standard one
local FACES = { [0] = { 0, 0 }, { 0, 40 }, { 0, 80 }, { 0, 120 }, { 0, 160 }, { 0, 200 }, { 440, 40 },
                { 424, 100 }, { 424, 140 }, { 424, 180 } }
local LEVEL_BUTTON = { [0] = 120, 200, 280, 360, 40 }  -- 4125:24b8: Knight .. Off, Human
-- the Character box (4125:24ec): ticked with a character chosen, else empty,
-- 24 x 20 at (x + 64, y + 39), its label at (x + 88, y + 41) (4125:24fc, 251c)
local CHECK = { [0] = { 440, 0 }, { 440, 20 } }

--- A side's face (7bab:0634), an index into FACES.
local function faceOf(s, i)
  if not s.inUse then return 6 end
  if not s.computer then return (i % 2 == 1) and 5 or 4 end
  if (s.card or 0) ~= 0 and s.level < 3 then return s.level + 7 end
  return s.level
end

--- How many computers are in play: Random Characters needs one (7bab:0416).
local function computers(st)
  local n = 0
  for i = 0, 7 do
    local s = st.sides[i]
    if s and s.inUse and s.computer and s.level ~= 3 then n = n + 1 end
  end
  return n
end

-- Setup Side's controls (dialog 27)
local SIDE_OK, SIDE_CANCEL, SIDE_NAME = 457, 458, 459
local SIDE_UP, SIDE_DOWN, SIDE_PAGE_UP, SIDE_PAGE_DOWN, SIDE_ROW = 460, 461, 462, 463, 464
local NAME_BOX = { x = 229, y = 138, w = 188, h = 22 }
-- a Knight's characters are listed in colour 5, a Lord's 7, a Warlord's 9
local LEVEL_INK = { [0] = 5, 7, 9 }

--- Setup Side (7bab:16ea, drawn by 7bab:180b): popup 4, dialog 27. Side
--- `i`'s name, retyped in the field (15 characters, 128 pixels, 7bab:1f8a);
--- for a computer its level's characters, five rows at a time, and the
--- chosen one's description from its .DSC. A human has no character: "N/A".
--- OK keeps what was done, Cancel puts the name and character back.
local function openSide(G, st, i, after)
  local R = { x = 120, y = 50, w = 400, h = 360 }   -- popup 4
  local s = st.sides[i]
  local d = { view = kit.view(27), editing = nil }
  local saved = { name = s.name, card = s.card }
  -- Off has no deck: its letter is past the end of "KLW"
  local deck, n = {}, 0
  if s.computer and s.level < 3 then deck, n = aicard.deck(G.dataDir, s.level) end
  -- the five rows' cards, -1 for none; the chosen one on the last row when
  -- it is past the first five
  local rows = {}
  for r = 0, 4 do rows[r] = r < n and r or -1 end
  if (s.card or 0) > 4 then for r = 0, 4 do rows[r] = rows[r] + s.card - 4 end end
  local desc, descCard = nil, -1

  -- 7bab:1ae4
  local function refresh()
    local v = d.view.state
    for r = 0, 4 do v[SIDE_ROW + r] = rows[r] < 0 and uidata.DISABLED or uidata.NORMAL end
    v[SIDE_OK], v[SIDE_CANCEL], v[SIDE_NAME] = uidata.NORMAL, uidata.NORMAL, uidata.NORMAL
    local up = s.computer and rows[0] >= 1
    local down = s.computer and rows[4] > 0 and rows[4] < n - 1
    v[SIDE_UP] = up and uidata.NORMAL or uidata.DISABLED
    v[SIDE_PAGE_UP] = v[SIDE_UP]
    v[SIDE_DOWN] = down and uidata.NORMAL or uidata.DISABLED
    v[SIDE_PAGE_DOWN] = v[SIDE_DOWN]
  end

  -- an empty name is not taken: the setup screen knows a side by its name
  local function keepName()
    if d.editing.text ~= "" then s.name = d.editing.text end
    d.editing = nil
  end

  local function close(ok)
    if not ok then s.name, s.card = saved.name, saved.card end   -- 7bab:1fd7
    kit.pop(d)
    after()
  end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), "Setup Side", 320, 55)
    local c, e = s.colour or 15, s.edge or 0
    local f = kit.font(2)
    -- two boxes, each outlined in the side's edge colour and twice more in
    -- its colour, a pixel further up and left each time
    for _, b in ipairs({ { 107, 67 }, { 194, 165 } }) do
      local y, h = b[1], b[2]
      kit.setPal(e)
      kit.outline(138, y, 368, h)
      kit.setPal(c)
      kit.outline(137, y - 1, 368, h)
      kit.outline(136, y - 2, 368, h)
    end
    for _, t in ipairs({ { "Side Name", 96 }, { "Leader", 181 } }) do
      kit.setPal(3)
      love.graphics.rectangle("fill", 160, t[2], f.width(t[1]), 17)
      love.graphics.setColor(1, 1, 1)
      f.colours(c, e).draw(t[1], 160, t[2])
    end
    kit.shield(i, 144, 122)
    kit.shield(i, 456, 122)
    kit.shield(i, 144, 202)
    love.graphics.setColor(1, 1, 1)
    kit.centred(f, "Retype the name of this side", 320, 116)
    kit.field(NAME_BOX.x, NAME_BOX.y, NAME_BOX.w, NAME_BOX.h,
              d.editing and d.editing.text or s.name, f)
    if d.editing then d.editing.drawCursor(NAME_BOX.x, NAME_BOX.y) end
    kit.setPal(0)
    love.graphics.rectangle("fill", 196, 218, 72, 1)
    love.graphics.rectangle("fill", 384, 218, 96, 1)
    love.graphics.setColor(1, 1, 1)
    f.draw("Name", 196, 203)
    f.draw("Description", 384, 203)
    -- the list (7bab:1be6): a sunk box, the chosen character in white
    kit.setPal(3)
    love.graphics.rectangle("fill", 189, 249, 186, 100)
    kit.bevel(188, 248, 188, 102, 4, 2)
    love.graphics.setColor(1, 1, 1)
    if not s.computer then
      f.draw("N/A", 196, 224)                      -- 7bab:1d0f
      f.draw("N/A", 384, 224)
    else
      local ink = f.colours(LEVEL_INK[s.level] or 15, 0)
      for r = 0, 4 do
        if rows[r] < 0 then break end
        local g = rows[r] == s.card and f or ink
        g.draw(deck[rows[r]] or "", 196, 252 + 19 * r)
      end
      -- the name, and the .DSC's next six lines (7bab:20dc)
      ink.draw(deck[s.card or 0] or "", 192, 224)
      if descCard ~= s.card then
        descCard = s.card
        desc = nil
        if s.level < 3 then
          local _, lines = aicard.describe(G.dataDir, s.level, s.card)
          desc = lines
        end
      end
      for k = 1, 6 do
        local line = desc and desc[k]
        if line and line ~= "" then ink.draw(line, 384, 224 + 20 * (k - 1)) end
      end
    end
    kit.drawControls(d.view)
  end

  function d.mousepressed(x, y)
    if d.editing then keepName() end
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    local id = c.id
    if id == SIDE_OK then return close(true) end                  -- 7bab:2024
    if id == SIDE_CANCEL then return close(false) end
    if id == SIDE_NAME then
      d.editing = input.editor(15, 128)
    elseif id == SIDE_UP or id == SIDE_DOWN then
      -- 7bab:1ed1: a row up or down, a row run off the deck left empty
      local step = id == SIDE_UP and -1 or 1
      for r = 0, 4 do
        rows[r] = rows[r] + step
        if rows[r] < 0 or rows[r] >= n then rows[r] = -1 end
      end
    elseif id == SIDE_PAGE_UP or id == SIDE_PAGE_DOWN then
      -- 7bab:1f23: five rows, or as many as there are
      local step
      if id == SIDE_PAGE_UP then step = -math.min(rows[0], 5)
      else step = math.min(n - rows[4] - 1, 5) end
      rows[0] = rows[0] + step
      for r = 1, 4 do rows[r] = rows[r - 1] + 1 end
    elseif id >= SIDE_ROW and id < SIDE_ROW + 5 then
      local card = rows[id - SIDE_ROW]                             -- 7bab:1e8a
      if card >= 0 then s.card = card end
    end
    refresh()
  end

  function d.keypressed(key)
    if d.editing then
      local r = d.editing.key(key)
      if r == "keep" then keepName() elseif r == "undo" then d.editing = nil end
      return
    end
    if key == "return" or key == "kpenter" then close(true)
    elseif key == "escape" then close(false) end
  end

  function d.textinput(t) if d.editing then d.editing.input(t) end end

  refresh()
  return kit.push(d)
end

local function openSetup(G, st, begin, back)
  -- the menu bar stays live over it, as over the start menu (7bab:0034)
  local d = { view = kit.view(3), menuBar = true }
  -- a screen, not a dialog: 7bab:0000 clears 4125:166a, so Begin has no ring
  d.view.screen = true
  local art = G.screen.art_for(31)                   -- SETUPBU.PCK

  local function refresh()
    local s = d.view.state
    for i = 0, 7 do s[125 + i] = uidata.NORMAL end
    for p = 0, 2 do s[145 + p] = presetMatches(st, p) and uidata.ACTIVE or uidata.NORMAL end
    s[141], s[142], s[148] = uidata.NORMAL, uidata.NORMAL, uidata.NORMAL
    s[143], s[144] = uidata.NORMAL, uidata.NORMAL
    s[157] = uidata.NORMAL
    s[158] = computers(st) > 0 and uidata.NORMAL or uidata.DISABLED
    d.hidden = { [st.greatest and 143 or 144] = true }
  end

  local function blit(sx, sy, w, h, x, y)
    if not art then return end
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(art.image, love.graphics.newQuad(sx, sy, w, h, art.w, art.h), x, y)
  end

  function d.draw()
    require("warlords.screen").drawBackground(G.screen)
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.marble, love.graphics.newQuad(0, 0, 360, 360, G.marble:getDimensions()), 16, 30)
    love.graphics.draw(G.marble, love.graphics.newQuad(0, 0, 224, 312, G.marble:getDimensions()), 400, 30)
    love.graphics.draw(G.marble, love.graphics.newQuad(0, 60, 360, 66, G.marble:getDimensions()), 16, 403)
    love.graphics.draw(G.marble, love.graphics.newQuad(0, 0, 224, 114, G.marble:getDimensions()), 400, 355)
    local f = kit.font(2)
    for i = 0, 7 do
      local s = st.sides[i]
      local R = RECTS[i]
      if s and s.name ~= "" then
        local c, e = s.colour or 15, s.edge or 0
        kit.setPal(c)
        kit.outline(R.x, R.y, R.w, R.h)
        love.graphics.rectangle("fill", R.x - 1, R.y - 1, R.w + 2, 1)
        love.graphics.rectangle("fill", R.x - 1, R.y - 1, 1, R.h + 2)
        love.graphics.rectangle("fill", R.x + 1, R.y + R.h - 2, R.w - 2, 1)
        love.graphics.rectangle("fill", R.x + R.w - 2, R.y + 1, 1, R.h - 2)
        kit.setPal(e)
        love.graphics.rectangle("fill", R.x + 1, R.y + 1, R.w - 2, 1)
        love.graphics.rectangle("fill", R.x + 1, R.y + 1, 1, R.h - 2)
        love.graphics.rectangle("fill", R.x - 1, R.y + R.h, R.w + 2, 1)
        love.graphics.rectangle("fill", R.x + R.w, R.y - 1, 1, R.h + 2)
        local nx, ny = R.x + 24, R.y - 8
        local w = f.width(s.name)
        kit.setPal(3)
        love.graphics.rectangle("fill", nx - 4, ny, w + 8, 20)
        love.graphics.setColor(1, 1, 1)
        f.colours(c, e).draw(s.name, nx, ny)
        -- the face (7bab:0634): Off, a human (two faces, turn about), or the
        -- computer's level, another face for a character; the button the
        -- same, Off and Human mapped in
        local face = faceOf(s, i)
        local fr = FACES[face]
        blit(fr[1], fr[2], 40, 40, R.x + 16, R.y + 16)
        local lv = face >= 7 and face - 7 or face
        local btn = (lv == 6 or lv == 3) and 3 or (lv >= 4 and 4 or lv)
        blit(LEVEL_BUTTON[btn], 0, 80, 25, R.x + 72, R.y + 12)
        local ck = CHECK[(s.card or 0) ~= 0 and 0 or 1]
        blit(ck[1], ck[2], 24, 20, R.x + 64, R.y + 39)
        love.graphics.setColor(1, 1, 1)
        f.colours(c, e).draw("Character", R.x + 88, R.y + 41)   -- 4125:258b
      end
    end
    love.graphics.setColor(1, 1, 1)
    kit.centred(f, kit.text(7, 0), 512, 48)
    kit.centred(f, kit.text(7, 1), 512, 206)
    kit.centred(f, kit.text(6, 0):format(rating(st)), 196, 426)
    kit.drawControls(d.view, d.hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if not c then return end
    local id = c.id
    if id >= 125 and id <= 132 then
      -- 7bab:0a4e: Human, Knight, Lord, Warlord, Off, and round again
      local s = st.sides[id - 125]
      if s and s.inUse then
        if not s.computer then s.computer, s.level = true, 0
        elseif s.level == 3 then s.computer, s.level = false, 0
        else s.level = s.level + 1 end
        s.card = 0                    -- the level's Standard character
      end
    elseif id == 141 then
      local playing = 0
      for i = 0, 7 do
        local s = st.sides[i]
        if s and s.inUse and not (s.computer and s.level == 3) then playing = playing + 1 end
      end
      if playing < 1 then return end
      keepOptions(G.dataDir)                              -- 7bab:0cfe
      kit.pop(d)
      return begin(st)
    elseif id == 142 then kit.pop(d) return back()
    elseif id == 143 then
      st.greatest = true
      for i = 0, 7 do
        local s = st.sides[i]
        if s and s.inUse then
          -- a side that was not a Warlord gets the Standard Warlord card
          if not (s.computer and s.level == 2) then s.card = 0 end
          s.computer, s.level = true, 2
        end
      end
    elseif id == 144 then st.greatest = false
    elseif id >= 145 and id <= 147 then
      for k = 0, 5 do st.options[k] = PRESETS[id - 145][k] end
    elseif id == 148 then
      return openOptions(G, st, refresh)
    elseif (id >= 133 and id <= 140) or (id >= 149 and id <= 156) then
      -- 7bab:16ea: the Character box or the face; a side not in the
      -- scenario has neither
      local i = id >= 149 and id - 149 or id - 133
      local s = st.sides[i]
      if s and s.inUse then return openSide(G, st, i, refresh) end
    elseif id == 157 then
      recallOptions(G.dataDir)                            -- 7bab:2229
    elseif id == 158 then
      -- 7bab:2051: each computer in play gets 1d(n - 1) of its level's n
      -- characters -- any but the Standard one
      local random = love.math and love.math.random or math.random
      for i = 0, 7 do
        local s = st.sides[i]
        if s and s.inUse and s.computer and s.level ~= 3 then
          local _, n = aicard.deck(G.dataDir, s.level)
          s.card = n > 1 and random(n - 1) or 0
        end
      end
    end
    refresh()
  end

  function d.keypressed(key) end

  refresh()
  return kit.push(d)
end

------------------------------------------------------------ the scenario list

local function openChooser(G, list, current, after)
  local R = { x = 80, y = 45, w = 480, h = 360 }    -- popup 19
  local d = { view = kit.view(29), top = 0, cur = current - 1 }
  while d.cur >= 7 do d.cur, d.top = d.cur - 1, d.top + 1 end
  local function refresh()
    local s = d.view.state
    for _, c in ipairs(d.view.dialog.controls) do s[c.id] = uidata.NORMAL end
    local up = d.top > 0 and uidata.NORMAL or uidata.DISABLED
    local down = d.top + 7 < #list and uidata.NORMAL or uidata.DISABLED
    s[470], s[472], s[471], s[473] = up, up, down, down
  end
  function d.draw()
    kit.popupFrame(R)
    local pic = image(G, G.dataDir .. "/PICS/NEWSCEN.PCK")
    love.graphics.setColor(1, 1, 1)
    if pic then love.graphics.draw(pic, R.x, R.y) end
    kit.setPal(0)
    love.graphics.rectangle("fill", 112, 153, 136, 140)
    love.graphics.setColor(1, 1, 1)
    local f = kit.font(2)
    for i = 0, 6 do
      local e = list[d.top + i + 1]
      if e then f.colours(i == d.cur and 15 or 5, 0).draw(e.name, 112, 153 + 20 * i) end
    end
    local e = list[d.top + d.cur + 1]
    if e then
      local g5 = f.colours(5, 0)
      kit.centred(g5, e.name, 416, 101)
      kit.centred(g5, e.text, 416, 174)
      kit.right(g5, ("%d"):format(e.cities), 368, 234)
      g5.draw(("%d"):format(e.ruins), 472, 234)
      kit.centred(g5, ("%d"):format(e.players), 416, 254)
    end
    kit.drawControls(d.view, { [476] = true, [477] = true, [478] = true, [479] = true,
                               [480] = true, [481] = true, [482] = true })
  end
  local function take(ok)
    kit.pop(d)
    after(ok and (d.top + d.cur + 1) or nil)
  end
  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c or d.view.state[c.id] == uidata.DISABLED then return end
    local id = c.id
    if id == 474 then return take(true)
    elseif id == 475 then return take(false)
    elseif id >= 476 and id <= 482 then
      if list[d.top + id - 476 + 1] then d.cur = id - 476 end
    elseif id == 470 or id == 472 then
      d.top = math.max(0, d.top - (id == 472 and 7 or 1))
    elseif id == 471 or id == 473 then
      d.top = math.min(math.max(0, #list - 7), d.top + (id == 473 and 7 or 1))
    end
    refresh()
  end
  function d.keypressed(key)
    if key == "return" or key == "kpenter" then take(true)
    elseif key == "escape" then take(false) end
  end
  refresh()
  return kit.push(d)
end

------------------------------------------------------------ the random world

local SLIDER_X, SLIDER_W = 384, 120                  -- 4125:29ac

--- 7f77:0332: the random world's settings, in place of the picture.
drawWorld = function(G)
  kit.setPal(3)
  love.graphics.rectangle("fill", 328, 200, 264, 225)  -- 4125:2930
  local f = kit.font(2)
  local art = G.screen.art_for(1)                     -- STARTBU.PCK
  for i = 0, 3 do
    local y = 215 + 30 * i
    love.graphics.setColor(1, 1, 1)
    kit.right(f, kit.text(2, i), 376, y)
    -- the slider at its place (4125:29cc), or bare for "?" (4125:2a04)
    local set = world.set[i + 1]
    local sy = set and 80 + 20 * world.sliders[i + 1] or 220
    if art then
      love.graphics.draw(art.image, love.graphics.newQuad(496, sy, SLIDER_W, 20, art.w, art.h), SLIDER_X, y + 1)
    end
    local shows = set and randommap.SLIDER_FORMATS[i]:format(randommap.SLIDER_SHOWS[i][world.sliders[i + 1]])
                  or "(?)"
    f.draw(shows, 504, y)
  end
  f.draw(randommap.terrainSetName(G.dataDir, world.terrainSet), 336, 340)
  f.draw(kit.text(3, world.allies and 1 or 0), 336, 365)
end

--- random_map_setup (7bab:10e8): make the world, showing its progress as
--- 4bed:01ff does on popup 23 -- the scroll with group 136's lines, the bar
--- RMAPBAR.PCK at (232, 257) growing a tenth at a time, the percentage over
--- it -- then hand it on. A "?" slider is rolled, 1d7-1.
makeWorld = function(G, after)
  local R = { x = 160, y = 55, w = 336, h = 347 }    -- popup 23
  if not G.bscroll then G.bscroll = pck.toImage(G.dataDir .. "/PICS/BSCROLL.PCK", G.palette, 10) end
  local bar = image(G, G.dataDir .. "/PICS/RMAPBAR.PCK", 10)
  local sliders = {}
  for i = 1, 4 do sliders[i] = world.set[i] and world.sliders[i] or randommap.RANDOM_SLIDER end
  local co = coroutine.create(randommap.generate)
  local opts = { dataDir = G.dataDir, rng = require("warlords.rng").new(os.time() % 1000000007),
                 sliders = sliders, allies = world.allies, terrainSet = world.terrainSet }
  local d = { pct = 0 }
  function d.update()
    -- a step of the generator a frame, so the bar moves as it works
    local ok, v = coroutine.resume(co, opts)
    if not ok then error(v, 0) end
    if coroutine.status(co) ~= "dead" then d.pct = v return end
    randommap.install(G.dataDir, v)
    kit.pop(d)
    after()
  end
  function d.draw()
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.bscroll, love.graphics.newQuad(0, 0, R.w, R.h, G.bscroll:getDimensions()), R.x, R.y)
    local f = kit.font(2).colours(0, 7)
    for i, k in ipairs({ 2, 3 }) do kit.centred(f, kit.text(0x88, k), 328, 181 + 20 * (i - 1)) end
    -- 4bed:01b5: a black frame (216d:01fd), the scroll showing through it
    kit.setPal(0)
    love.graphics.rectangle("line", 232.5, 255.5, 191, 24)
    love.graphics.setColor(1, 1, 1)
    local w = math.floor((d.pct + 10) / 10) * 16 + 16
    if bar then love.graphics.draw(bar, love.graphics.newQuad(0, 0, w, 21, bar:getDimensions()), 232, 257) end
    kit.centred(f, ("%d%%"):format(d.pct), 328, 237)
  end
  function d.mousepressed() end
  function d.keypressed() end
  return kit.push(d)
end

------------------------------------------------------------ the start menu

--- Open the start menu. `start(scenarioDir, options, sides, extra)` begins a
--- game (`extra.greatest`: I am the Greatest);
--- `loaded(g)` takes a saved one.
function M.open(start, loaded)
  local G = kit.G
  local list = M.scenarios(G.dataDir)
  local d = { view = kit.view(1), cur = 1, menuBar = true }
  -- a screen, not a dialog: 7f77:0000 clears 4125:166a, so Begin has no ring
  d.view.screen = true
  for i, e in ipairs(list) do if e.dir == "Erythea" then d.cur = i end end   -- 4125:2a6e

  -- back from a random world, the menu is still on one (7f77:0000)
  if G.scenario == randommap.DIR then world.on = true end

  -- 7f77:011e
  local function refresh()
    local s = d.view.state
    s[100], s[101], s[103] = uidata.NORMAL, uidata.NORMAL, uidata.NORMAL
    s[102] = world.on and uidata.DISABLED or uidata.NORMAL
    d.hidden = {}
    if not world.on then
      for id = 104, 113 do d.hidden[id] = true end
      return
    end
    s[104], s[105] = uidata.NORMAL, uidata.NORMAL
    for i = 0, 3 do
      s[106 + i] = uidata.NORMAL
      s[110 + i] = world.set[i + 1] and uidata.NORMAL or uidata.ACTIVE   -- lit while "?"
    end
  end

  function d.draw()
    love.graphics.setColor(1, 1, 1)
    for q = 0, 3 do
      local img = image(G, G.dataDir .. "/PICS/STARTUP" .. q .. ".PCK")
      if img then love.graphics.draw(img, (q % 2) * 320, math.floor(q / 2) * 240) end
    end
    local sc = list[d.cur]
    if world.on then
      drawWorld(G)
    elseif sc then
      local pic = image(G, G.dataDir .. "/" .. sc.dir:upper() .. "/PICS/SCENARIO.PCK")
      if pic then
        love.graphics.setColor(1, 1, 1)
        love.graphics.draw(pic, love.graphics.newQuad(0, 0, 264, 225, pic:getDimensions()), 328, 200)
      end
    end
    if world.on or sc then
      -- 7f77:02bf: the scenario's name, or "A Random World"
      kit.setPal(3)
      love.graphics.rectangle("fill", 336, 166, 248, 28)
      love.graphics.setColor(1, 1, 1)
      kit.centred(kit.font(2), world.on and kit.text(0, 0) or sc.name, 460, 172)
    end
    kit.drawControls(d.view, d.hidden)
  end

  local function reopen() refresh() kit.push(d) end

  local function setUp(sc)
    local st = newSetup(G, sc)
    openSetup(G, st, function(s)
      local options, sides = {}, {}
      for i = 0, 9 do options[OPTION_KEYS[i]] = s.options[i] end
      for i = 0, 7 do
        local e = s.sides[i]
        if e then sides[i] = { computer = e.computer, level = e.level, card = e.card,
                               name = e.name, off = e.computer and e.level == 3 } end
      end
      start(s.sc.dir:upper(), options, sides, { greatest = s.greatest })
    end, reopen)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if not c or d.view.state[c.id] == uidata.DISABLED then return end
    local id = c.id
    if id == 100 then
      -- the chooser goes over the menu, which stays underneath; a scenario
      -- chosen ends the random world (7f77:067c)
      openChooser(G, list, d.cur, function(i)
        if i then d.cur, world.on = i, false end
        refresh()
      end)
    elseif id == 101 then
      require("ui.savegame").load(function(g)
        kit.pop(d)
        loaded(g)
      end)
    elseif id == 102 then
      world.on = true                                     -- 7f77:05f5
    elseif id == 104 then
      world.terrainSet = (world.terrainSet + 1) % randommap.terrainSets(G.dataDir)   -- 7f77:063a
    elseif id == 105 then
      world.allies = not world.allies                     -- 7f77:0661
    elseif id >= 106 and id <= 109 then
      -- 7f77:0512: a "?" slider is set again where it was; a set one moves
      -- to where it was clicked
      local i = id - 106 + 1
      if not world.set[i] then world.set[i] = true
      else
        local v = math.floor((x - SLIDER_X) * 7 / SLIDER_W)
        world.sliders[i] = math.max(0, math.min(6, v))
      end
    elseif id >= 110 and id <= 113 then
      world.set[id - 110 + 1] = false                    -- 7f77:0571
    elseif id == 103 and world.on then
      -- 7f77:060f: "One moment...", the world, then the sides as for any scenario
      require("ui.advisor").say(require("warlords.cues").MOMENT, function()
        makeWorld(G, function()
          kit.pop(d)
          setUp({ name = kit.text(0, 0), dir = randommap.DIR })
        end)
      end)
    elseif id == 103 and list[d.cur] then
      kit.pop(d)
      setUp(list[d.cur])
    end
    refresh()
  end

  function d.keypressed(key) end

  refresh()
  return kit.push(d)
end

return M
