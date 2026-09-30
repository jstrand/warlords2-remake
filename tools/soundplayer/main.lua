-- A jukebox for the game's sounds: one button per song and per sample.
--
--     love tools/soundplayer              -- from the repository root
--     love tools/soundplayer /path/to/data
--
-- The songs are the S set (SINT*.XMI, SSTARTUP.XMI), played as the Sound
-- Blaster FM version plays them, through love2d/warlords/ailfm.lua on a
-- thread. The M (MT-32) and R (Sound Canvas) sets need those synthesizers
-- and are left out. The samples are the .8SN files: unsigned 8-bit mono at
-- 11000 Hz (docs/formats/sound.md); an advisor line shows its subtitle.

local SAMPLES = {
  "ARMY", "ARMY2", "CHORD", "DING", "DRAMATIC", "ORCH", "SPLASH", "TURN", "WAR",
  "VBEGIN", "VGOLD00", "VGOLD01", "VGOLD01A", "VGREET0", "VHERO00", "VHERO01",
  "VLOSE05", "VLOSE10", "VLOSE15", "VLOSE20", "VLOSE25", "VLOSE35",
  "VMESS00", "VMESS01", "VMESS02", "VMESS03", "VMOMENT", "VQUIT",
  "VWIN05", "VWIN05A", "VWIN10", "VWIN15", "VWIN20", "VWIN25", "VWIN30", "VWIN35",
}
local SUBTITLE = { VHERO01 = "VHERO1" }  -- the one .TXT not named after its sample

-- the music thread: { "play", xmiBytes, id } / { "stop" } in, { "ended", id } out
local THREAD = [[
local libPath, adv, ad = ...
package.path = libPath .. ";" .. package.path
require("love.sound")
require("love.audio")
require("love.timer")
local ailfm = require("warlords.ailfm")
local xmi = require("warlords.xmi")
local inbox = love.thread.getChannel("jukebox")
local outbox = love.thread.getChannel("jukebox-out")
local RATE, CHUNK, BUFFERS, GAIN = math.floor(ailfm.RATE + 0.5), 1024, 6, 2 / 32768
local drv = ailfm.new(adv, ad)
local src = love.audio.newQueueableSource(RATE, 16, 1, BUFFERS)
local buf, current = {}, nil
while true do
  local m = drv:idle() and not src:isPlaying() and inbox:demand() or inbox:pop()
  while m do
    if m[1] == "play" then
      drv:play(xmi.parse(m[2]), false)
      current = m[3]
    elseif m[1] == "stop" then
      drv:stop()
      current = nil
    end
    m = inbox:pop()
  end
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
  love.timer.sleep(0.01)
end
]]

local dataDir
local sections = {}   -- { title, buttons = { { label, kind, file } } }
local song = nil      -- the button whose song is playing
local songId = 0
local sample = nil    -- { button, src }
local subtitle = nil
local inbox, outbox
local buttons = {}    -- laid out: { b, x, y, w, h }
local status = ""

local BW, BH, GAP, MARGIN = 104, 26, 6, 16

local function read(path)
  local f = io.open(path, "rb")
  if not f then return nil end
  local s = f:read("*a")
  f:close()
  return s
end

local function exists(path)
  local f = io.open(path, "rb")
  if f then f:close() end
  return f ~= nil
end

function love.load(args)
  love.window.setMode(960, 640, { resizable = true })
  love.window.setTitle("Warlords II sounds")
  dataDir = args[1] or "original"

  local music = { title = "Music (FM)", buttons = {} }
  if exists(dataDir .. "/SOUND/SSTARTUP.XMI") then
    music.buttons[#music.buttons + 1] = { label = "SSTARTUP", kind = "song", file = "SSTARTUP" }
  end
  for i = 0, 23 do
    local name = "SINT" .. i
    if exists(dataDir .. "/SOUND/" .. name .. ".XMI") then
      music.buttons[#music.buttons + 1] = { label = name, kind = "song", file = name }
    end
  end
  local effects = { title = "Effects", buttons = {} }
  local voices = { title = "Advisor", buttons = {} }
  for _, name in ipairs(SAMPLES) do
    if exists(dataDir .. "/SOUND/" .. name .. ".8SN") then
      local group = name:sub(1, 1) == "V" and voices or effects
      group.buttons[#group.buttons + 1] = { label = name, kind = "sample", file = name }
    end
  end
  sections = { music, effects, voices }

  local adv, ad = read(dataDir .. "/ADLIB.ADV"), read(dataDir .. "/MIDPAK.AD")
  if adv and ad then
    local libPath = love.filesystem.getSource() .. "/../../love2d/?.lua"
    inbox = love.thread.getChannel("jukebox")
    outbox = love.thread.getChannel("jukebox-out")
    love.thread.newThread(THREAD):start(libPath, adv, ad)
  else
    music.buttons = {}
    status = "no ADLIB.ADV / MIDPAK.AD in " .. dataDir .. ": no music"
  end
  if #music.buttons + #effects.buttons + #voices.buttons == 0 then
    status = "no sounds found in " .. dataDir .. "/SOUND -- run from the repository root, or pass the data dir"
  end
end

local function stopSong()
  if song and inbox then inbox:push({ "stop" }) end
  song = nil
end

local function stopSample()
  if sample then sample.src:stop() end
  sample, subtitle = nil, nil
end

local function playSong(b)
  stopSong()
  local bytes = read(dataDir .. "/SOUND/" .. b.file .. ".XMI")
  if not bytes then return end
  songId = songId + 1
  inbox:push({ "play", bytes, songId })
  song = b
end

local function playSample(b)
  stopSample()
  local bytes = read(dataDir .. "/SOUND/" .. b.file .. ".8SN")
  if not bytes or #bytes == 0 then return end
  local sd = love.sound.newSoundData(#bytes, 11000, 8, 1)
  for i = 1, #bytes do sd:setSample(i - 1, (bytes:byte(i) - 128) / 128) end
  local src = love.audio.newSource(sd)
  src:play()
  sample = { button = b, src = src }
  local txt = read(dataDir .. "/SOUND/" .. (SUBTITLE[b.file] or b.file) .. ".TXT")
  subtitle = txt and txt:gsub("~.*$", ""):gsub("|", "\n") or nil
end

function love.update()
  if sample and not sample.src:isPlaying() then sample, subtitle = nil, nil end
  if outbox then
    local m = outbox:pop()
    while m do
      if m[1] == "ended" and m[2] == songId then song = nil end
      m = outbox:pop()
    end
  end
end

function love.draw()
  love.graphics.clear(0.12, 0.12, 0.14)
  local font = love.graphics.getFont()
  local w = love.graphics.getWidth()
  local x, y = MARGIN, MARGIN
  buttons = {}

  love.graphics.setColor(1, 1, 1)
  love.graphics.print("Click to play, click again to stop. Space stops everything.", x, y)
  y = y + 28
  for _, s in ipairs(sections) do
    if #s.buttons > 0 then
      love.graphics.setColor(0.9, 0.8, 0.5)
      love.graphics.print(s.title, MARGIN, y)
      y = y + 22
      x = MARGIN
      for _, b in ipairs(s.buttons) do
        if x + BW > w - MARGIN and x > MARGIN then x, y = MARGIN, y + BH + GAP end
        local on = b == song or (sample and sample.button == b)
        love.graphics.setColor(on and { 0.25, 0.55, 0.3 } or { 0.25, 0.27, 0.33 })
        love.graphics.rectangle("fill", x, y, BW, BH, 4, 4)
        love.graphics.setColor(1, 1, 1)
        love.graphics.print(b.label, x + math.floor((BW - font:getWidth(b.label)) / 2),
          y + math.floor((BH - font:getHeight()) / 2))
        buttons[#buttons + 1] = { b = b, x = x, y = y }
        x = x + BW + GAP
      end
      y = y + BH + 20
    end
  end

  love.graphics.setColor(0.8, 0.8, 0.8)
  if subtitle then love.graphics.printf(subtitle, MARGIN, y, w - 2 * MARGIN, "center") end
  if status ~= "" then love.graphics.print(status, MARGIN, love.graphics.getHeight() - 24) end
end

function love.mousepressed(mx, my, button)
  if button ~= 1 then return end
  for _, p in ipairs(buttons) do
    if mx >= p.x and mx < p.x + BW and my >= p.y and my < p.y + BH then
      local b = p.b
      if b.kind == "song" then
        if b == song then stopSong() else playSong(b) end
      else
        if sample and sample.button == b then stopSample() else playSample(b) end
      end
      return
    end
  end
end

function love.keypressed(key)
  if key == "space" then stopSong() stopSample()
  elseif key == "escape" then love.event.quit() end
end
