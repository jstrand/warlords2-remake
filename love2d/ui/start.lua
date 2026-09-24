-- The start screens: the menu the game opens on, choosing a scenario, and
-- setting its sides and options up (7f77:0000, 7f77:0725, 7bab:0000).
--
-- **The start menu** (7f77:0000, dialog 1): STARTUP0-3.PCK, the four
-- quarters of the screen, with New Scenario (100), Load Game (101), Random
-- Map (102) and Begin (103). On the right the scenario's own
-- PICS\SCENARIO.PCK -- its (0, 0) 264x225 at (328, 200) -- under a colour-3
-- bar at (336, 166) 248x28 with its name centred on (460, 172) (7f77:02bf,
-- 0332). The scenario starts as Erythea (4125:2a6e). The remake has no random
-- map generator, and greys Random Map.
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
-- 90 i) in its colours, its face from SETUPBU.PCK at (40, 56 + 90 i) and its
-- button at (96, 52 + 90 i): Human, Knight, Lord, Warlord or Off
-- (7bab:0634). A click on the button (125-132) goes round those five
-- (7bab:0a4e). In the right-hand panel Begin (141), Main Menu (142), "I am
-- the Greatest" (143: every side a Warlord) or "No! I really am Normal"
-- (144), Beginner / Intermediate / Advanced (145-147, lit when the options
-- are theirs) and Edit Options (148); under the map the difficulty rating
-- (group 6) centred on (196, 426) (7bab:0bab).
--
-- **Edit Options** (7bab:12a2): popup 4, dialog 4. "Game Options" (group 8),
-- "Affecting Difficulty" and "Not Affecting Difficulty" (groups 9 and 10)
-- over rules at y = 111 and 250; the ten options of group 4 two to a row --
-- names from x = 128 and 320, 30 apart from y = 131 and 270, their values
-- (Average/Strong/Active, On, Off) in colour 7 128 to the right. 159-168
-- change one, 169-171 are the presets, OK (172).
--
-- Left out: the Character boxes and Random Characters, which pick the
-- computer players' personalities (dialog 27, 7bab:16ea, 2051), and Recall
-- Options (7bab:2229); the remake's computer players have no personalities.

local kit    = require("ui.kit")
local pck    = require("warlords.pck")
local scn    = require("warlords.scn")
local uidata = require("warlords.uidata")

local M = {}

-- the ten options, in the order of group 4 and the table at 4125:23b4
local OPTION_KEYS = {
  [0] = "neutralCities", "diplomacy", "quests", "hiddenMap", "viewEnemies",
  "viewProduction", "intenseCombat", "quickStart", "militaryAdvisor", "randomTurns",
}
local PRESETS = {                                    -- 4125:2378
  [0] = { [0] = 0, 0, 0, 0, 1, 0 },
  { [0] = 1, 1, 1, 0, 0, 0 },
  { [0] = 2, 1, 1, 1, 0, 1 },
}

local function u16(s, i) return s:byte(i + 1) + s:byte(i + 2) * 256 end
local function cstr(s, a, n) return (s:sub(a + 1, a + n):match("^[^%z]*")) end

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
  local st = { sc = sc, map = map, options = {}, sides = {}, greatest = false }
  for i = 0, 9 do st.options[i] = map.options[OPTION_KEYS[i]] or 0 end
  for _, s in ipairs(map.sides) do
    st.sides[s.index] = { inUse = s.inUse, name = s.name, colour = s.colour, edge = s.edge,
                          computer = s.computer, level = s.computer and (s.level or 0) or 0 }
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
local FACES = { [0] = { 0, 0 }, { 0, 40 }, { 0, 80 }, { 0, 120 }, { 0, 160 }, { 0, 200 }, { 440, 40 } }
local LEVEL_BUTTON = { [0] = 120, 200, 280, 360, 40 }  -- 4125:24b8: Knight .. Off, Human

local function openSetup(G, st, begin, back)
  -- the menu bar stays live over it, as over the start menu (7bab:0034)
  local d = { view = kit.view(3), menuBar = true }
  local art = G.screen.art_for(31)                   -- SETUPBU.PCK

  local function refresh()
    local s = d.view.state
    for i = 0, 7 do s[125 + i] = uidata.NORMAL end
    for p = 0, 2 do s[145 + p] = presetMatches(st, p) and uidata.ACTIVE or uidata.NORMAL end
    s[141], s[142], s[148] = uidata.NORMAL, uidata.NORMAL, uidata.NORMAL
    s[143], s[144] = uidata.NORMAL, uidata.NORMAL
    d.hidden = { [st.greatest and 143 or 144] = true, [157] = true, [158] = true }
    for i = 0, 7 do d.hidden[133 + i], d.hidden[149 + i] = true, true end
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
        -- computer's level; the button the same, Off and Human mapped in
        local face
        if not s.inUse then face = 6
        elseif not s.computer then face = (i % 2 == 1) and 5 or 4
        else face = s.level end
        local fr = FACES[face]
        blit(fr[1], fr[2], 40, 40, R.x + 16, R.y + 16)
        local btn = (face == 6 or face == 3) and 3 or (face >= 4 and 4 or face)
        blit(LEVEL_BUTTON[btn], 0, 80, 25, R.x + 72, R.y + 12)
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
      end
    elseif id == 141 then
      local playing = 0
      for i = 0, 7 do
        local s = st.sides[i]
        if s and s.inUse and not (s.computer and s.level == 3) then playing = playing + 1 end
      end
      if playing < 1 then return end
      kit.pop(d)
      return begin(st)
    elseif id == 142 then kit.pop(d) return back()
    elseif id == 143 then
      st.greatest = true
      for i = 0, 7 do
        local s = st.sides[i]
        if s and s.inUse then s.computer, s.level = true, 2 end
      end
    elseif id == 144 then st.greatest = false
    elseif id >= 145 and id <= 147 then
      for k = 0, 5 do st.options[k] = PRESETS[id - 145][k] end
    elseif id == 148 then
      return openOptions(G, st, refresh)
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

------------------------------------------------------------ the start menu

--- Open the start menu. `start(scenarioDir, options, sides)` begins a game;
--- `loaded(g)` takes a saved one.
function M.open(start, loaded)
  local G = kit.G
  local list = M.scenarios(G.dataDir)
  local d = { view = kit.view(1), cur = 1, menuBar = true }
  for i, e in ipairs(list) do if e.dir == "Erythea" then d.cur = i end end   -- 4125:2a6e

  local function refresh()
    local s = d.view.state
    s[100], s[101], s[103] = uidata.NORMAL, uidata.NORMAL, uidata.NORMAL
    s[102] = uidata.DISABLED
    d.hidden = {}
    for id = 104, 113 do d.hidden[id] = true end
  end

  function d.draw()
    love.graphics.setColor(1, 1, 1)
    for q = 0, 3 do
      local img = image(G, G.dataDir .. "/PICS/STARTUP" .. q .. ".PCK")
      if img then love.graphics.draw(img, (q % 2) * 320, math.floor(q / 2) * 240) end
    end
    local sc = list[d.cur]
    if sc then
      local pic = image(G, G.dataDir .. "/" .. sc.dir:upper() .. "/PICS/SCENARIO.PCK")
      if pic then
        love.graphics.setColor(1, 1, 1)
        love.graphics.draw(pic, love.graphics.newQuad(0, 0, 264, 225, pic:getDimensions()), 328, 200)
      end
      kit.setPal(3)
      love.graphics.rectangle("fill", 336, 166, 248, 28)
      love.graphics.setColor(1, 1, 1)
      kit.centred(kit.font(2), sc.name, 460, 172)
    end
    kit.drawControls(d.view, d.hidden)
  end

  local function reopen() refresh() kit.push(d) end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if not c or d.view.state[c.id] == uidata.DISABLED then return end
    local id = c.id
    if id == 100 then
      -- the chooser goes over the menu, which stays underneath
      openChooser(G, list, d.cur, function(i)
        if i then d.cur = i end
      end)
    elseif id == 101 then
      require("ui.savegame").load(function(g)
        kit.pop(d)
        loaded(g)
      end)
    elseif id == 103 and list[d.cur] then
      kit.pop(d)
      local st = newSetup(G, list[d.cur])
      openSetup(G, st, function(s)
        local options, sides = {}, {}
        for i = 0, 9 do options[OPTION_KEYS[i]] = s.options[i] end
        for i = 0, 7 do
          local e = s.sides[i]
          if e then sides[i] = { computer = e.computer, level = e.level,
                                 off = e.computer and e.level == 3 } end
        end
        start(s.sc.dir:upper(), options, sides)
      end, reopen)
    end
  end

  function d.keypressed(key) end

  refresh()
  return kit.push(d)
end

return M
