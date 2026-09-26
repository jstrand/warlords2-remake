-- The advisor: a horned helmet that speaks (6dda:026f). docs/re/sound.md.
--
-- He waits for any sample still sounding (255e:0736), then saves the screen
-- under (152, 15) 352x442 and draws VOICE.PCK there through its mask,
-- colour 10 -- FILE.DAT group 3's bitmap 50. While his clip plays he
-- blinks: a count starts at dice(1, 30, 10), goes up a BIOS tick at a time,
-- and past 40 it restarts at 0 and the eyes -- VOICEBIT.PCK, three 160x47
-- frames down the sheet, open, half shut and shut -- are put over his at
-- (232, 261): half, shut, half, open, two ticks each. When the clip ends
-- the screen comes back and the game goes on. Nothing cuts him short.
--
-- With Speech off he never appears. (With speech on but no digitised sound
-- the original writes the clip's .TXT under him instead, a line at a time;
-- the engine always has sound, so that path is not taken here.)

local kit   = require("ui.kit")
local pck   = require("warlords.pck")
local sound = require("sound")

local M = {}

local AT = { x = 152, y = 15 }
local EYES = { x = 232, y = 261, w = 160, h = 47 }
local BLINK = { 1, 2, 1, 0 }            -- VOICEBIT rows, 47 apart
local TICK = 1 / 18.2
local VOICE_KEY = 10

local function clock()
  return love.timer and love.timer.getTime and love.timer.getTime() or 0
end

local function images(G)
  if not G.voicePic then
    G.voicePic = pck.toImage(G.dataDir .. "/PICS/VOICE.PCK", G.palette, VOICE_KEY)
    G.voiceBits = pck.toImage(G.dataDir .. "/PICS/VOICEBIT.PCK", G.palette)
  end
  return G.voicePic, G.voiceBits
end

--- Have him say a clip -- a FILE.DAT group, as warlords/cues.lua names
--- them -- and run `after` once he has finished. With Speech off, or no
--- clip to play, `after` runs straight away.
function M.say(group, after)
  after = after or function() end
  if not group or not sound.speechOn() then return after() end
  local G = kit.G
  local d = { waiting = true }

  local function finish()
    kit.pop(d)
    after()
  end

  local function start()
    d.waiting = false
    local len = sound.speak(group, finish)
    if not len then
      kit.pop(d)
      return after()
    end
    d.started = clock()
    -- 6dda:0944: the first blink after dice(1, 30, 10) .. 40 ticks
    d.count = love.math and love.math.random(11, 40) or math.random(11, 40)
    d.nextTick = d.started + TICK
    d.blink = nil
  end

  function d.update()
    if d.waiting then
      if not sound.busy() then start() end
      return
    end
    local t = clock()
    while d.nextTick and t >= d.nextTick do
      d.nextTick = d.nextTick + TICK
      if d.blink then
        d.blink.ticks = d.blink.ticks + 1
        if d.blink.ticks >= 2 then
          d.blink.ticks, d.blink.frame = 0, d.blink.frame + 1
          if d.blink.frame > #BLINK then d.blink = nil end
        end
      else
        d.count = d.count + 1
        if d.count > 40 then d.count, d.blink = 0, { frame = 1, ticks = 0 } end
      end
    end
  end

  function d.draw()
    if d.waiting then return end
    local pic, bits = images(G)
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(pic, AT.x, AT.y)
    if d.blink then
      local bw, bh = bits:getDimensions()
      local row = BLINK[d.blink.frame] or 0
      love.graphics.draw(bits, love.graphics.newQuad(0, row * EYES.h, EYES.w, EYES.h, bw, bh),
                         EYES.x, EYES.y)
    end
  end

  -- he holds the game while he speaks: input goes nowhere
  function d.mousepressed() end
  function d.keypressed() end

  kit.push(d)
  if not sound.busy() then start() end
  return d
end

return M
