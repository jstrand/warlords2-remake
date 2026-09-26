-- The music, synthesised on a thread of its own (sound.lua starts it).
--
-- ailfm.lua plays the sequence into its OPL2 at the chip's own rate and the
-- samples go out through a queueable source a few buffers ahead. Commands
-- come in on the "music" channel:
--
--   { "init", advBytes, adBytes }    ADLIB.ADV and MIDPAK.AD
--   { "play", xmiBytes, loop, id }   start a song
--   { "stop" }                       let every note go
--   { "volume", v }
--
-- and { "ended", id } goes back on "music-out" when a song that does not loop
-- runs out. When nothing sounds the thread stops rendering and just waits.

require("love.sound")
require("love.audio")
require("love.timer")
local ailfm = require("warlords.ailfm")
local xmi = require("warlords.xmi")

local inbox = love.thread.getChannel("music")
local outbox = love.thread.getChannel("music-out")

local RATE = math.floor(ailfm.RATE + 0.5)
local CHUNK, BUFFERS = 1024, 6
-- The chip's nine voices leave a lot of headroom: a song peaks around a
-- fifth of full scale, some 10 dB under the samples. On the card the balance
-- was the mixer's, which is not measured here; twice as loud brings the two
-- close.
local GAIN = 2 / 32768
local drv, src, current
local buf = {}

local function handle(m)
  local cmd = m[1]
  if cmd == "init" then
    drv = ailfm.new(m[2], m[3])
    src = love.audio.newQueueableSource(RATE, 16, 1, BUFFERS)
  elseif not drv then
    return
  elseif cmd == "play" then
    local seq = xmi.parse(m[2])
    current = m[4]
    drv:play(seq, m[3])
  elseif cmd == "stop" then
    drv:stop()
    current = nil
  elseif cmd == "volume" then
    src:setVolume(m[2])
  elseif cmd == "quit" then
    return true
  end
end

while true do
  -- idle: block until there is something to do
  local m
  if not drv or (drv:idle() and (not src or not src:isPlaying())) then
    m = inbox:demand()
  else
    m = inbox:pop()
  end
  while m do
    if handle(m) then return end
    m = inbox:pop()
  end
  if drv then
    local wasPlaying = drv.playing
    while src:getFreeBufferCount() > 0 and not drv:idle() do
      drv:render(buf, CHUNK)
      local sd = love.sound.newSoundData(CHUNK, RATE, 16, 1)
      for i = 1, CHUNK do
        local v = buf[i] * GAIN
        sd:setSample(i - 1, v > 1 and 1 or v < -1 and -1 or v)
      end
      src:queue(sd)
    end
    if wasPlaying and not drv.playing and current then
      outbox:push({ "ended", current })
      current = nil
    end
    if not src:isPlaying() and src:getFreeBufferCount() < BUFFERS then src:play() end
  end
  love.timer.sleep(0.01)
end
