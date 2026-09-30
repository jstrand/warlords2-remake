-- The game's music, effects and advisor voice. docs/re/sound.md.
--
-- Three switches, as Game > Settings has them and DATA/OPTIONS.SND keeps
-- them -- three characters, '1' or '0', for Music, Effects and Speech:
--
--   Music    the XMIDI songs, played the way the Sound Blaster FM version
--            plays them: the S-prefixed files through ADLIB.ADV's logic into
--            an emulated OPL2 (ailfm.lua), on a thread (musicthread.lua)
--   Effects  the .8SN samples
--   Speech   the advisor, whose clips are .8SN samples too
--
-- The samples are DIGPAK's: one plays at a time, and one asked for while
-- another is sounding waits for it to end (255e:065e calls 255e:0736 first),
-- so they queue here. Unsigned 8-bit mono at 11000 Hz -- the rate
-- 255e:04e9 and 255e:065e hand the driver.
--
-- Everything is optional: with no audio device, or the files missing, the
-- calls do nothing.
--
-- The music can also be the game's other two arrangements (not the
-- original's choice to make in play: INSTALL.EXE's driver decided it) -- the
-- MT-32's M files and the Sound Canvas's R files, recorded through emulators
-- of those modules to pre-rendered-sound/<M|R><song>.ogg. Tick 0 of the song
-- is the recording's start, and a recording runs on past the song's end with
-- its reverb dying away. The choice is kept with the remake's other
-- settings (prefs.lua).

local cues = require("warlords.cues")
local prefs = require("prefs")
local web = require("web")
local xmi = require("warlords.xmi")

local sound = {}

sound.SAMPLE_RATE = 11000

-- the synthesizers, and each one's letter before the song's file name
-- (255e:032f: S for FM, M for MT32MPU.ADV, R for SC32MPU.ADV)
sound.SYNTHS = { "fm", "mt32", "sc55" }
local PREFIX = { fm = "S", mt32 = "M", sc55 = "R" }

local S = {
  on = { music = false, effects = false, speech = false },
  samples = {},   -- file name -> SoundData, or false when missing
  queue = {},     -- samples waiting their turn
  playing = nil,  -- the Source sounding now
  cue = nil,      -- the music cue asked for last
  song = nil,     -- { name, loop } of the song started last
  songId = 0,
  songPlaying = false,
  synth = "fm",
  rec = nil,      -- the recording playing: { data, src, loop, at }
  tails = {},     -- recordings let run on to their end under a new start
}

local function read(path)
  local f = io.open(path, "rb")
  if not f then return nil end
  local s = f:read("*a")
  f:close()
  return s
end

local function roll(n)
  local random = love.math and love.math.random or math.random
  return random(math.max(1, n))
end

-- "SOUND\\army.8SN" -> <data>/SOUND/ARMY.8SN
local function path(name)
  local rel = name:gsub("\\+", "/")
  local dir, file = rel:match("^(.-)([^/]*)$")
  return S.dataDir .. "/" .. dir .. file:upper()
end

--- Start up: the settings from DATA/OPTIONS.SND, the music thread.
--- `files` is DATA/FILE.DAT as uidata.strings reads it; `recordings` the
--- folder of pre-rendered songs, if there is one.
function sound.init(dataDir, files, recordings)
  S.dataDir, S.files, S.recDir = dataDir, files, recordings
  S.ok = love.audio ~= nil and love.sound ~= nil
  local opts = read(dataDir .. "/DATA/OPTIONS.SND") or "111"
  S.on.music = opts:sub(1, 1) == "1"
  S.on.effects = opts:sub(2, 2) == "1"
  S.on.speech = opts:sub(3, 3) ~= "0"
  -- the browser's build has no threads (web.lua), and so no FM
  S.fm = love.thread ~= nil and not web.on
  if S.ok and S.fm then
    local adv, ad = read(dataDir .. "/ADLIB.ADV"), read(dataDir .. "/MIDPAK.AD")
    if adv and ad then
      S.thread = love.thread.newThread("musicthread.lua")
      S.thread:start()
      S.inbox = love.thread.getChannel("music")
      S.outbox = love.thread.getChannel("music-out")
      S.inbox:push({ "init", adv, ad })
    end
  end
  local synth = prefs.get("music")
  if PREFIX[synth] and sound.synthAvailable(synth) then
    S.synth = synth
  elseif not sound.synthAvailable(S.synth) then
    for _, s in ipairs(sound.SYNTHS) do
      if sound.synthAvailable(s) then S.synth = s break end
    end
  end
end

--- The switches, as Settings shows them: { music, effects, speech }.
function sound.options()
  return { S.on.music, S.on.effects, S.on.speech }
end

-- 64d2:0576 writes the three characters back after every change
local function saveOptions()
  local f = io.open(S.dataDir .. "/DATA/OPTIONS.SND", "wb")
  if not f then return end
  f:write((S.on.music and "1" or "0") .. (S.on.effects and "1" or "0") .. (S.on.speech and "1" or "0"))
  f:close()
end

--- Turn one switch over (64d2:0576). Music turned on starts the play music;
--- turned off it stops at once.
function sound.toggle(which)
  S.on[which] = not S.on[which]
  if which == "music" then
    if S.on.music then
      S.cue = nil
      sound.music(cues.PLAY)
    else
      sound.stopMusic()
    end
  end
  saveOptions()
end

------------------------------------------------------------------ the music

-- "INT12.XMI" -> <recordings>/RINT12.ogg
local function recordingPath(synth, name)
  return S.recDir and (S.recDir .. "/" .. PREFIX[synth] .. name:upper():gsub("%.XMI$", "") .. ".ogg")
end

local function stopRecordings()
  if S.rec then S.rec.src:stop() S.rec = nil end
  for _, t in ipairs(S.tails) do t:stop() end
  S.tails = {}
end

local function playRecording(data)
  local src = love.audio.newSource(data, "stream")
  src:play()
  return src
end

-- Start a song, `loop`ing or not, on the synthesizer chosen. False when it
-- cannot play: the recording, or the FM driver, missing.
local function startSong(name, loop)
  if S.synth ~= "fm" and S.ok then
    -- the recording, and the song's own length from the file it was made
    -- of: ailfm:interval starts a loop again one tick after it
    local bytes = read(recordingPath(S.synth, name) or "")
    local seq = bytes and xmi.parse(read(S.dataDir .. "/SOUND/" .. PREFIX[S.synth] .. name:upper()) or "")
    if seq then
      if S.inbox then S.inbox:push({ "stop" }) end
      stopRecordings()
      local data = love.filesystem.newFileData(bytes, name .. ".ogg")
      S.rec = { data = data, src = playRecording(data), loop = loop,
                at = (seq.length + 1) / xmi.TICK_RATE }
      S.songId = S.songId + 1
      S.song, S.songPlaying = { name, loop }, true
      return true
    end
  end
  -- the FM version's files carry an S: SINT12.XMI (255e:032f)
  local bytes = S.inbox and read(S.dataDir .. "/SOUND/S" .. name:upper())
  if not bytes then return false end
  stopRecordings()
  S.songId = S.songId + 1
  S.song, S.songPlaying = { name, loop }, true
  S.inbox:push({ "play", bytes, loop, S.songId })
  return true
end

--- Play a cue (warlords/cues.lua) -- unless it is the one already playing,
--- which goes on undisturbed (6dda:0000). `sides` feed the computer's cue.
function sound.music(cue, sides)
  if not S.on.music then return end
  if cue == S.cue and S.songPlaying then return end
  local name, loop = cues.song(S.files, cue, sides, roll)
  if name and startSong(name, loop) then S.cue = cue end
end

function sound.stopMusic()
  S.cue, S.song, S.songPlaying = nil, nil, false
  if S.inbox then S.inbox:push({ "stop" }) end
  stopRecordings()
end

--- The synthesizer the music is played on: "fm", "mt32" or "sc55".
function sound.synth() return S.synth end

--- Can it be? FM wants ADLIB.ADV, the others their recordings.
function sound.synthAvailable(synth)
  if synth == "fm" then return S.fm ~= false end
  local f = PREFIX[synth] and io.open(recordingPath(synth, "STARTUP.XMI") or "", "rb")
  if f then f:close() end
  return f ~= nil
end

--- Play the music on another synthesizer from now on; the song playing
--- starts again on it.
function sound.setSynth(synth)
  if not PREFIX[synth] or synth == S.synth then return end
  S.synth = synth
  prefs.set("music", synth)
  if S.on.music and S.song and S.songPlaying then startSong(S.song[1], S.song[2]) end
end

------------------------------------------------------------------ samples

local function sampleData(name)
  local sd = S.samples[name]
  if sd ~= nil then return sd or nil end
  local bytes = read(path(name))
  if not bytes or #bytes == 0 then S.samples[name] = false return nil end
  sd = love.sound.newSoundData(#bytes, sound.SAMPLE_RATE, 8, 1)
  for i = 1, #bytes do sd:setSample(i - 1, (bytes:byte(i) - 128) / 128) end
  S.samples[name] = sd
  return sd
end

-- queue a sample behind whatever is sounding; `done` runs once it has ended
local function enqueue(name, done)
  local sd = S.ok and sampleData(name)
  if not sd then
    if done then done() end
    return 0
  end
  S.queue[#S.queue + 1] = { sd = sd, done = done }
  sound.update()
  return sd:getDuration()
end

-- FILE.DAT's sample names, by what the game calls them for
local EFFECT = {
  army = { 64, 0 }, army2 = { 64, 5 }, ding = { 64, 1 }, chord = { 64, 2 },
  dramatic = { 37, 0 }, orch = { 37, 1 }, war = { 38, 0 }, splash = { 39, 0 },
  turn = { 63, 0 },
}

--- Play one of the effects -- army, army2, ding, chord, dramatic, orch, war,
--- splash, turn -- if Effects is on. Returns its length in seconds (0 when
--- nothing plays), for the callers that the original holds until it ends.
function sound.effect(what)
  if not S.on.effects or not S.files then return 0 end
  local e = EFFECT[what]
  local g = e and S.files[e[1] + 1]
  local name = g and g[e[2] + 1]
  if not name then return 0 end
  return enqueue(name)
end

--- Say an advisor clip -- a FILE.DAT group from cues.lua -- if Speech is on.
--- Returns its length, or nil when he stays silent; `done` runs when the
--- voice has ended (at once if it could not play).
function sound.speak(group, done)
  if not S.on.speech or not S.files then return nil end
  local name = cues.clip(S.files, group, roll)
  if not name or not (S.ok and sampleData(name)) then return nil end
  return enqueue(name, done)
end

function sound.speechOn() return S.on.speech end

--- True while a sample is sounding or waiting.
function sound.busy()
  return S.playing ~= nil or #S.queue > 0
end

--- Call every frame: start the next sample when the last has ended, and
--- hear from the music thread.
function sound.update()
  if S.playing and not S.playing.src:isPlaying() then
    local done = S.playing.done
    S.playing = nil
    if done then done() end
  end
  if not S.playing and #S.queue > 0 then
    local q = table.remove(S.queue, 1)
    q.src = love.audio.newSource(q.sd)
    q.src:play()
    S.playing = q
  end
  if S.outbox then
    local m = S.outbox:pop()
    while m do
      if m[1] == "ended" and m[2] == S.songId then S.songPlaying = false end
      m = S.outbox:pop()
    end
  end
  -- a recording past its song's end: a loop starts over while the old one's
  -- tail runs out beneath it, as the module's reverb would have rung on
  local r = S.rec
  if r and (r.src:tell() >= r.at or not r.src:isPlaying()) then
    if r.loop then
      S.tails[#S.tails + 1] = r.src
      r.src = playRecording(r.data)
    else
      S.tails[#S.tails + 1] = r.src
      S.rec, S.songPlaying = nil, false
    end
  end
  for i = #S.tails, 1, -1 do
    if not S.tails[i]:isPlaying() then table.remove(S.tails, i) end
  end
end

return sound
