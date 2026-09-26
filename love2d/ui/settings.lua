-- Game > Settings (64d2:0000(1)) -- the same screen that sets the sides up
-- for a new game (64d2:0000(0)).
--
-- Popup 4, (120, 50) 400x360, dialog 9, drawn by 64d2:0137. Heads in the
-- side's colours: "Name" ending at x = 248, "Human", "Enhanced" and
-- "Observe" centred on 280, 396 and 468, all at y = 60. A row a side, 30
-- apart from y = 90:
--
--   its name in its colours, ending at x = 248
--   a side not in play: "Deceased!" at x = 280 -- nothing for one whose name
--     is "Not used"
--   else ABITS' box (320, 0) ticked or (320, 20) clear at (256, y) for Human,
--     then "Human" or its level -- Knight, Lord, Warlord -- at (280, y); the
--     box at (400, y) for Enhanced; for a computer side, the box at (448, y)
--     for Observe
--
-- and under them the boxes for Music (152, 340), Effects (152, 370) and
-- Speech (256, 340), their words 24 to the right. 252-259 turn a side human
-- or computer, 260-267 Enhanced, 268-275 Observe (64d2:04cc, 0508, 053f);
-- 276-278 the sounds, Music and Effects greyed with no sound card; OK (251).
-- A sound box is turned over by 64d2:0576, which writes DATA/OPTIONS.SND
-- straight back (sound.lua).

local kit    = require("ui.kit")
local uidata = require("warlords.uidata")
local sound  = require("sound")

local M = {}

local R = { x = 120, y = 50, w = 400, h = 360 }     -- popup 4
local DIALOG, OK = 9, 251
local HUMAN, ENHANCED, OBSERVE, SOUND = 252, 260, 268, 276
local LEVELS = { [0] = "Knight", "Lord", "Warlord" }            -- 4125:0bf2
local SOUNDS = { { 152, 340, "Music", "music" }, { 152, 370, "Effects", "effects" },
                 { 256, 340, "Speech", "speech" } }

--- `after` runs when OK is pressed.
function M.open(after)
  local G = kit.G
  local g = G.g
  local d = { view = kit.view(DIALOG) }

  local function refresh()
    local st = d.view.state
    st[OK] = uidata.NORMAL
    st[SOUND], st[SOUND + 1], st[SOUND + 2] = uidata.NORMAL, uidata.NORMAL, uidata.NORMAL
    d.hidden = {}
    for i = 0, 7 do
      local s = g.map.sides[i + 1]
      if s and s.inUse then
        st[HUMAN + i], st[ENHANCED + i], st[OBSERVE + i] = uidata.NORMAL, uidata.NORMAL, uidata.NORMAL
      else
        d.hidden[HUMAN + i], d.hidden[ENHANCED + i], d.hidden[OBSERVE + i] = true, true, true
      end
    end
  end

  local function box(on, x, y)
    local aw, ah = G.abits:getDimensions()
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.abits, love.graphics.newQuad(320, on and 0 or 20, 24, 20, aw, ah), x, y)
  end

  function d.draw()
    kit.popup(R)
    local f = kit.font(2)
    local me = G.player
    local head = f.colours(me.colour or 15, me.edge or 0)
    love.graphics.setColor(1, 1, 1)
    kit.right(head, "Name", 248, 60)
    kit.centred(head, "Human", 280, 60)
    kit.centred(head, "Enhanced", 396, 60)
    kit.centred(head, "Observe", 468, 60)
    for i = 0, 7 do
      local s = g.map.sides[i + 1]
      local y = 90 + 30 * i
      if s then
        love.graphics.setColor(1, 1, 1)
        kit.right(f.colours(s.colour or 15, s.edge or 0), s.name or "", 248, y)
        local playing = s.inUse and (s.alive ~= false)
        if not playing then
          if s.name ~= "Not used" then f.draw("Deceased!", 280, y) end
        else
          box(not s.computer, 256, y)
          love.graphics.setColor(1, 1, 1)
          f.draw(s.computer and (LEVELS[s.level or 0] or "") or "Human", 280, y)
          box(s.enhanced, 400, y)
          if s.computer then box(s.observe, 448, y) end
        end
      end
    end
    local on = sound.options()
    for k, sd in ipairs(SOUNDS) do
      box(on[k], sd[1], sd[2])
      love.graphics.setColor(1, 1, 1)
      f.draw(sd[3], sd[1] + 24, sd[2])
    end
    kit.drawControls(d.view, d.hidden)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if not c or d.view.state[c.id] == uidata.DISABLED then return end
    local id = c.id
    if id == OK then
      kit.pop(d)
      if after then after() end
      return
    end
    local i
    if id >= HUMAN and id < HUMAN + 8 then
      i = id - HUMAN
      local s = g.map.sides[i + 1]
      s.computer = not s.computer
    elseif id >= ENHANCED and id < ENHANCED + 8 then
      local s = g.map.sides[id - ENHANCED + 1]
      s.enhanced = not s.enhanced
    elseif id >= OBSERVE and id < OBSERVE + 8 then
      local s = g.map.sides[id - OBSERVE + 1]
      if s.computer then s.observe = not s.observe end
    elseif id >= SOUND and id < SOUND + 3 then
      sound.toggle(SOUNDS[id - SOUND + 1][4])
    end
    refresh()
  end

  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then
      kit.pop(d)
      if after then after() end
    end
  end

  refresh()
  return kit.push(d)
end

return M
