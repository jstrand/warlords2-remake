-- The game's music player: Miles' AIL 2.0 AdLib driver (ADLIB.ADV), which
-- MIDPAK.COM loads for WARLORD2.EXE, rebuilt from its disassembly.
--
-- The driver reads XMIDI (xmi.lua) at 120 Hz, gives each note one of nine
-- OPL2 voices and writes the chip's registers from a timbre in the Global
-- Timbre Library, MIDPAK.AD. Its tables -- frequencies, the reset values, the
-- velocity curve -- are read from the player's own ADLIB.ADV. Offsets and
-- routines are in docs/formats/sound.md; the addresses below are ADLIB.ADV
-- file offsets (it loads at offset 0 of its segment).
--
--     local drv = ailfm.new(advBytes, adBytes)
--     drv:play(xmi.parse(bytes), true)     -- true: start again at the end
--     drv:render(out, n)                   -- n samples at opl.RATE
--
-- Headless. Everything here happens between chip writes, so it costs next
-- to nothing beside opl.lua's sample loop.

local opl = require("warlords.opl")
local xmi = require("warlords.xmi")

local floor = math.floor

local ailfm = {}
ailfm.__index = ailfm
ailfm.RATE = opl.RATE

local ADV = {
  freq = 0x101,   -- 192 words: 12 half-tones x 16 fine steps of F-number
  octave = 0x281, -- 96 bytes: the octave of each note from C-2
  halftone = 0x2e1, -- 96 bytes: its half-tone
  reset = 0x340,  -- register values written to 1..0xF5 at start-up
  velocity = 0x52b, -- 16 bytes: velocity / 8 -> sensitivity, 82..127
  programs = 0x2203, -- the program each channel 1..8 starts on
}
ailfm.ADV = ADV

-- operator register offsets of voice v's modulator; the carrier is +3
local OPREG = { [0] = 0, 1, 2, 8, 9, 10, 16, 17, 18 }

local function u16(s, i) local a, b = s:byte(i + 1, i + 2) return a + b * 256 end
local function s16(s, i) local v = u16(s, i) return v >= 0x8000 and v - 0x10000 or v end
local function s8(v) return v >= 0x80 and v - 0x100 or v end

--- The driver's tables, from ADLIB.ADV's bytes.
function ailfm.driverTables(adv)
  local T = { freq = {}, octave = {}, halftone = {}, reset = {}, velocity = {}, programs = {} }
  for i = 0, 191 do T.freq[i] = s16(adv, ADV.freq + i * 2) end
  for i = 0, 95 do
    T.octave[i] = adv:byte(ADV.octave + i + 1)
    T.halftone[i] = adv:byte(ADV.halftone + i + 1)
  end
  for r = 1, 0xf5 do T.reset[r] = adv:byte(ADV.reset + r + 1) end
  for i = 0, 15 do T.velocity[i] = adv:byte(ADV.velocity + i + 1) end
  for ch = 1, 8 do T.programs[ch] = adv:byte(ADV.programs + ch + 1) end
  return T
end

--- The Global Timbre Library (MIDPAK.AD): a directory of 6-byte entries
--- { patch, bank, offset:u32 } ended by bank 0xFF, and at each offset a
--- timbre -- a u16 length then the bytes. Keyed bank * 128 + patch.
function ailfm.timbres(ad)
  local lib, p = {}, 0
  while p + 6 <= #ad do
    local patch, bank = ad:byte(p + 1, p + 2)
    if bank == 0xff then break end
    local off = u16(ad, p + 2) + u16(ad, p + 4) * 65536
    local len = u16(ad, off)
    lib[bank * 128 + patch] = { len = len, ad:byte(off + 3, off + len) }
    p = p + 6
  end
  return lib
end

function ailfm.new(adv, ad)
  local self = setmetatable({}, ailfm)
  self.T = ailfm.driverTables(adv)
  self.lib = ailfm.timbres(ad)
  self.chip = opl.new()
  for r = 1, 0xf5 do self.chip:write(r, self.T.reset[r]) end
  -- channel state, as the driver's start-up leaves it on channels 1..9
  self.chans = {}
  for ch = 0, 15 do
    self.chans[ch] = { vol = 0, expr = 0, mod = 0, sustain = 0, lock = 0, bank = 0,
                       bend = 0, timbre = nil, voices = 0 }
  end
  for ch = 1, 9 do
    local c = self.chans[ch]
    c.vol, c.mod, c.expr, c.sustain, c.bank, c.lock = 127, 0, 127, 0, 0, 0
    if self.T.programs[ch] then self:program(ch, self.T.programs[ch]) end
  end
  self.percussion = {} -- key -> timbre, channel 10's cache
  self.slots = {}
  for i = 0, 15 do self.slots[i] = { active = false, voice = nil, flags = 0 } end
  self.owner = {} -- voice -> channel, or nil when free
  self.nextVoice = 0 -- the round-robin pointer (cs:0cc4)
  self.seq, self.pos, self.tick, self.untilTick = nil, 1, 0, 0
  self.playing, self.loop = false, false
  return self
end

function ailfm:program(ch, p)
  self.chans[ch].timbre = self.lib[self.chans[ch].bank * 128 + p]
end

-- Copy a 14-byte timbre into a slot (1d6c). Bytes, after the length:
-- transpose, then the modulator's 20/40/60/80/E0 registers, C0, and the
-- carrier's 20/40/60/80/E0.
local function loadTimbre(s, t)
  s.keyon = 0x20
  s.fbc = t[7]
  s.conn = t[7] % 2
  s.modKsl, s.modLevel = t[3] - t[3] % 64, 63 - t[3] % 64
  s.carKsl, s.carLevel = t[9] - t[9] % 64, 63 - t[9] % 64
  s.modAvek, s.modMult = t[2] - t[2] % 16, t[2] % 16
  s.carAvek, s.carMult = t[8] - t[8] % 16, t[8] % 16
  s.modAD, s.modSR, s.modWS = t[4], t[5], t[6]
  s.carAD, s.carSR, s.carWS = t[10], t[11], t[12]
  -- which operators follow the volume: the carrier always, the modulator
  -- only when it is heard directly (additive)
  s.scaleMod = s.conn == 1
  s.flags = 0xf9
end

-- hi8(a * b * 2), then one more unless it came to 0: the driver's product
local function scale(a, b)
  local v = floor(a * b * 2 / 256) % 256
  return v == 0 and 0 or v + 1
end

-- Write whatever the slot's flags say has changed (1807).
function ailfm:update(s)
  local v = s.voice
  if not v then return end
  local chip, f, c = self.chip, s.flags, self.chans[s.ch]
  local m, k = OPREG[v], OPREG[v] + 3
  if f >= 0x80 then
    local vib = c.mod >= 64 and 0x40 or 0
    chip:write(0x20 + m, s.modMult + s.modAvek + (s.modAvek % 0x80 >= 0x40 and 0 or vib))
    chip:write(0x20 + k, s.carMult + s.carAvek + (s.carAvek % 0x80 >= 0x40 and 0 or vib))
    f = f - 0x80
  end
  if f >= 0x40 then
    local vol = scale(scale(c.vol, c.expr), s.vel)
    local ml = s.modLevel
    if s.scaleMod then ml = floor(ml * vol / 127) end
    local cl = floor(s.carLevel * vol / 127)
    chip:write(0x40 + m, 63 - ml % 64 + s.modKsl)
    chip:write(0x40 + k, 63 - cl % 64 + s.carKsl)
    f = f - 0x40
  end
  if f >= 0x20 then
    chip:write(0x60 + m, s.modAD) chip:write(0x60 + k, s.carAD)
    chip:write(0x80 + m, s.modSR) chip:write(0x80 + k, s.carSR)
    f = f - 0x20
  end
  if f >= 0x10 then
    chip:write(0xe0 + k, s.carWS) chip:write(0xe0 + m, s.modWS)
    f = f - 0x10
  end
  if f >= 0x08 then
    chip:write(0xc0 + v, s.fbc % 16)
    f = f - 0x08
  end
  if f % 2 == 1 then
    if s.keyon == 0 then
      chip:write(0xb0 + v, s.b0 - s.b0 % 64 + s.b0 % 32) -- key off, pitch kept
    else
      chip:write(0xa0 + v, self:frequency(s, c) % 256)
      s.b0 = floor(self.fw / 256) % 4 + self.block * 4 + s.keyon
      chip:write(0xb0 + v, s.b0)
    end
    f = f - 1
  end
  s.flags = f
end

-- F-number and block for a slot's note (1b61): the note less two octaves,
-- folded into the 96 the tables cover, in sixteenths of a half-tone.
function ailfm:frequency(s, c)
  local n = s.note + s.transpose - 24
  while n < 0 do n = n + 12 end
  while n > 95 do n = n - 12 end
  local bend = floor(c.bend / 32) * 12 -- +-12 half-tones over the range
  local x = floor((n * 256 + bend + 8) / 16)
  while x < 0 do x = x + 192 end
  while x > 0x5ff do x = x - 192 end
  local semi = floor(x / 16)
  local fw = self.T.freq[self.T.halftone[semi] * 16 + x % 16]
  local block = self.T.octave[semi] - 1
  -- the upper half-tones are stored an octave down, flagged negative
  if fw < 0 then block = block + 1 end
  if block < 0 then block, fw = block + 1, floor(fw / 2) end
  if fw < 0 then fw = fw + 0x10000 end
  self.fw, self.block = fw, block
  return fw
end

-- Give a slot a free voice, round-robin from the last one (1740); with none
-- free, let the slots fight over them (1c69).
function ailfm:allocate(s)
  local v = self.nextVoice
  for _ = 1, 9 do
    v = (v + 1) % 9
    self.nextVoice = v
    if not self.owner[v] then
      s.voice = v
      self.owner[v] = s.ch
      self.chans[s.ch].voices = self.chans[s.ch].voices + 1
      s.flags = 0xf9
      self:update(s)
      return
    end
  end
  self:steal()
end

-- A note's claim to a voice is 0x7FFF less the voices its channel already
-- holds (a channel locked by controller 112 claims 0xFFFF). While the best
-- claim of a note without a voice is at least the weakest claim of one
-- with a voice, the weaker note is cut off and its voice handed over.
function ailfm:steal()
  local prio, n = {}, 0
  for i = 0, 15 do
    local s = self.slots[i]
    if s.active then
      n = n + 1
      local c = self.chans[s.ch]
      local p = (c.lock >= 64 and 0xffff or 0x7fff) - c.voices
      prio[i] = p < 0 and 0 or p
    end
  end
  while n > 0 do
    local best, bestP, worst, worstP = nil, 0, nil, 0xffff
    for i = 0, 15 do
      local s = self.slots[i]
      if s.active then
        local p = prio[i]
        if not s.voice then
          if p >= bestP then best, bestP = i, p end
        elseif p <= worstP then
          worst, worstP = i, p
        end
      end
    end
    if bestP < worstP or bestP == 0 or not best or not worst then return end
    local w, b = self.slots[worst], self.slots[best]
    local v = w.voice
    self:release(w)
    w.active = false
    b.voice = v
    self.owner[v] = b.ch
    self.chans[b.ch].voices = self.chans[b.ch].voices + 1
    b.flags = 0xf9
    self:update(b)
    n = n - 1
  end
end

-- Key off and give the voice back (179c).
function ailfm:release(s)
  if not s.voice then return end
  s.keyon = 0
  s.flags = s.flags % 2 == 1 and s.flags or s.flags + 1
  self:update(s)
  local c = self.chans[s.ch]
  c.voices = c.voices - 1
  self.owner[s.voice] = nil
  s.voice = nil
end

function ailfm:noteOn(ch, key, vel)
  if vel == 0 then return self:noteOff(ch, key) end
  local t
  if ch == 9 then
    t = self.percussion[key]
    if t == nil then
      t = self.lib[127 * 128 + key] or false
      self.percussion[key] = t
    end
  else
    t = self.chans[ch].timbre
  end
  if not t or t.len ~= 14 then return end
  local s
  for i = 0, 15 do
    if not self.slots[i].active then s = self.slots[i] break end
  end
  if not s then return end
  s.ch, s.key = ch, key
  -- a drum plays at the pitch its timbre's transpose byte names
  if ch == 9 then s.note, s.transpose = t[1], 0
  else s.note, s.transpose = key, s8(t[1]) end
  s.vel = self.T.velocity[floor(vel / 8)]
  s.active, s.sustained, s.voice = true, false, nil
  loadTimbre(s, t)
  self:allocate(s)
end

function ailfm:noteOff(ch, key)
  local c = self.chans[ch]
  for i = 0, 15 do
    local s = self.slots[i]
    if s.active and s.key == key and s.ch == ch then
      if c.sustain >= 64 then
        s.sustained = true
      else
        self:release(s)
        s.active = false
      end
    end
  end
end

-- flags a controller sets on its channel's sounding notes
local CTRL = { [1] = { "mod", 0x80 }, [7] = { "vol", 0x40 }, [11] = { "expr", 0x40 },
               [10] = { "pan", 0x40 } }

function ailfm:controller(ch, n, v)
  local c = self.chans[ch]
  if n == 114 then c.bank = v
  elseif n == 112 then c.lock = v
  elseif n == 64 then
    c.sustain = v
    if v < 64 then
      for i = 0, 15 do
        local s = self.slots[i]
        if s.active and s.ch == ch and s.sustained then self:noteOff(ch, s.key) end
      end
    end
  elseif CTRL[n] then
    c[CTRL[n][1]] = v
    for i = 0, 15 do
      local s = self.slots[i]
      if s.active and s.ch == ch then
        if floor(s.flags / CTRL[n][2]) % 2 == 0 then s.flags = s.flags + CTRL[n][2] end
        self:update(s)
      end
    end
  end
end

-- One MIDI message (2009). Notes only sound on channels 2-10 (1-9 here).
function ailfm:message(st, a, b)
  local hi, ch = st - st % 16, st % 16
  if hi == 0x90 then
    if ch >= 1 and ch <= 9 then self:noteOn(ch, a, b) end
  elseif hi == 0x80 then
    self:noteOff(ch, a)
  elseif hi == 0xb0 then
    self:controller(ch, a, b)
  elseif hi == 0xc0 then
    self:program(ch, a)
  elseif hi == 0xe0 then
    self.chans[ch].bend = b * 128 + a - 0x2000
    for i = 0, 15 do
      local s = self.slots[i]
      if s.active and s.ch == ch then
        if s.flags % 2 == 0 then s.flags = s.flags + 1 end
        self:update(s)
      end
    end
  end
end

--- Start a parsed sequence (xmi.parse) from its beginning.
function ailfm:play(seq, loop)
  self:stop()
  self.seq, self.pos, self.tick, self.loop, self.playing = seq, 1, 0, loop, seq ~= nil
end

--- Stop the sequence and let every sounding note go.
function ailfm:stop()
  for i = 0, 15 do
    local s = self.slots[i]
    if s.active then self:release(s) s.active = false end
  end
  self.playing = false
end

-- one interval of AIL's 120 Hz timer
function ailfm:interval()
  local q = self.seq
  if not self.playing or not q then return end
  local t = self.tick
  while self.pos <= q.n and q.t[self.pos] <= t do
    local i = self.pos
    self:message(q.st[i], q.a[i], q.b[i])
    self.pos = i + 1
  end
  self.tick = t + 1
  if self.pos > q.n and self.tick > q.length then
    if self.loop then self:play(q, true) else self:stop() end
  end
end

--- Nothing playing and every note faded away.
function ailfm:idle()
  return not self.playing and self.chip:silent()
end

--- Fill out[first..first+n-1] with the next n samples, stepping the
--- sequence as time passes. Keeps the chip running when nothing plays, so
--- released notes fade out.
function ailfm:render(out, n, first)
  first = first or 1
  local i, last = first, first + n - 1
  local per = opl.RATE / xmi.TICK_RATE
  while i <= last do
    if self.untilTick <= 0 then
      self:interval()
      self.untilTick = self.untilTick + per
    end
    local k = math.min(last - i + 1, math.ceil(self.untilTick))
    self.chip:generate(out, k, i)
    i = i + k
    self.untilTick = self.untilTick - k
  end
end

return ailfm
