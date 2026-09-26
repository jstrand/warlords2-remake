-- A Yamaha YM3812 (OPL2), the FM chip on the AdLib and the Sound Blaster.
--
-- The game's music is played by Miles' AIL AdLib driver (ADLIB.ADV, see
-- ailfm.lua) writing registers to this chip, so this is the last link to
-- the sound the original makes. It follows Nuked-OPL3's model of the chip --
-- the log-sine and exponent ROMs, the envelope generator's rate counter,
-- the tremolo and vibrato counters -- cut down to OPL2: nine two-operator
-- channels, four waveforms, no rhythm mode (the driver never turns it on).
--
-- Headless and allocation-free in the sample loop. LOVE runs LuaJIT, whose
-- `bit` library does the masking; plain Lua 5.3+ (the test runner) gets the
-- same functions built from its native operators.
--
--     local chip = opl.new()
--     chip:write(0x20, 0x01) ...
--     chip:generate(out, n)      -- out[1..n] = signed samples, about +-32767
--
-- It runs at the chip's own rate, opl.RATE: a 3.579545 MHz crystal / 72.

local bit = rawget(_G, "bit")
if not bit then
  -- Lua 5.3+: the operators exist but would not even parse under LuaJIT
  bit = load([[return {
    band = function(a, b) return a & b end,
    bor = function(a, b) return a | b end,
    bxor = function(a, b) return a ~ b end,
    rshift = function(a, n) return (a & 0xffffffff) >> n end,
    arshift = function(a, n) return a >> n end,
    lshift = function(a, n) return (a << n) & 0xffffffff end,
  }]])()
end
local band, bor, bxor, rshift, lshift = bit.band, bit.bor, bit.bxor, bit.rshift, bit.lshift
local floor = math.floor

local opl = {}
opl.__index = opl
opl.RATE = 3579545 / 72 -- 49715.9 Hz

-- The chip's ROMs, as the die shows them: a quarter sine in log2 form,
-- 256 steps to the octave, and the 2^x table that undoes it.
local LOGSIN, EXP = {}, {}
for i = 0, 255 do
  LOGSIN[i] = floor(-math.log(math.sin((i + 0.5) * math.pi / 512)) / math.log(2) * 256 + 0.5)
  EXP[i] = floor(2 ^ ((255 - i) / 256) * 1024 + 0.5)
end

-- frequency multiplier, doubled (the chip's 0.5 is 1)
local MT = { [0] = 1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30 }
local KSLROM = { [0] = 0, 32, 40, 45, 48, 51, 53, 55, 56, 58, 59, 60, 61, 62, 63, 64 }
local KSLSHIFT = { [0] = 8, 1, 2, 0 }
local INCSTEP = { [0] = { [0] = 0, 0, 0, 0 }, { [0] = 1, 0, 0, 0 },
                  { [0] = 1, 0, 1, 0 }, { [0] = 1, 1, 1, 0 } }

-- operator register offsets -> operator number, and each channel's pair
local SLOT_OF_REG = {}
for i, r in ipairs({ 0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 16, 17, 18, 19, 20, 21 }) do
  SLOT_OF_REG[r] = i - 1
end
local CH_MOD = { [0] = 0, 1, 2, 6, 7, 8, 12, 13, 14 }

local ATTACK, DECAY, SUSTAIN, RELEASE = 0, 1, 2, 3

-- the exponent: level 0 is loudest, every 256 halves it
local function calcExp(level)
  if level > 0x1fff then level = 0x1fff end
  return rshift(EXP[band(level, 0xff)] * 2, rshift(level, 8))
end

-- the four OPL2 waveforms: sine, half sine, absolute sine, quarter pulses
local function wave(wf, phase, env)
  phase = band(phase, 0x3ff)
  local lv
  if wf == 0 then
    if band(phase, 0x100) ~= 0 then lv = LOGSIN[bxor(band(phase, 0xff), 0xff)]
    else lv = LOGSIN[band(phase, 0xff)] end
    local v = calcExp(lv + env * 8)
    if band(phase, 0x200) ~= 0 then return -v - 1 end
    return v
  elseif wf == 1 then
    if band(phase, 0x200) ~= 0 then lv = 0x1000
    elseif band(phase, 0x100) ~= 0 then lv = LOGSIN[bxor(band(phase, 0xff), 0xff)]
    else lv = LOGSIN[band(phase, 0xff)] end
  elseif wf == 2 then
    if band(phase, 0x100) ~= 0 then lv = LOGSIN[bxor(band(phase, 0xff), 0xff)]
    else lv = LOGSIN[band(phase, 0xff)] end
  else
    if band(phase, 0x100) ~= 0 then lv = 0x1000
    else lv = LOGSIN[band(phase, 0xff)] end
  end
  return calcExp(lv + env * 8)
end

local function newSlot()
  return {
    -- registers
    am = 0, vib = 0, egt = 0, ksr = 0, mult = 0, ksl = 0, tl = 0,
    ar = 0, dr = 0, sl = 0, rr = 0, wf = 0,
    -- state
    key = false, gen = RELEASE, rout = 0x1ff, out = 0, prout = 0,
    phase = 0, phaseOut = 0, reset = false, egOut = 0x1ff,
  }
end

function opl.new()
  local self = setmetatable({}, opl)
  self.slots = {}
  for i = 0, 17 do self.slots[i] = newSlot() end
  self.ch = {}
  for c = 0, 8 do
    self.ch[c] = { fnum = 0, block = 0, fb = 0, con = 0, ksv = 0, ksl = 0,
                   mod = self.slots[CH_MOD[c]], car = self.slots[CH_MOD[c] + 3] }
  end
  self.wse, self.dam, self.dvb = false, false, false
  self.timer, self.egTimer, self.egState, self.egAdd, self.egTimerLo = 0, 0, 0, 0, 0
  self.tremPos, self.trem, self.vibPos = 0, 0, 0
  return self
end

local function updateKsl(c)
  local ksl = KSLROM[rshift(c.fnum, 6)] * 4 - (8 - c.block) * 32
  c.ksl = ksl < 0 and 0 or ksl
  c.ksv = c.block * 2 + band(rshift(c.fnum, 9), 1)
end

function opl:write(reg, v)
  local hi = band(reg, 0xe0)
  if reg == 0x01 then
    self.wse = band(v, 0x20) ~= 0
  elseif reg == 0xbd then
    self.dam, self.dvb = band(v, 0x80) ~= 0, band(v, 0x40) ~= 0
  elseif hi >= 0x20 and hi <= 0x80 or hi == 0xe0 then
    local n = SLOT_OF_REG[band(reg, 0x1f)]
    if not n then return end
    local s = self.slots[n]
    if hi == 0x20 then
      s.am, s.vib = rshift(v, 7), band(rshift(v, 6), 1)
      s.egt, s.ksr, s.mult = band(rshift(v, 5), 1), band(rshift(v, 4), 1), band(v, 15)
    elseif hi == 0x40 then
      s.ksl, s.tl = rshift(v, 6), band(v, 0x3f)
    elseif hi == 0x60 then
      s.ar, s.dr = rshift(v, 4), band(v, 15)
    elseif hi == 0x80 then
      s.sl, s.rr = rshift(v, 4), band(v, 15)
      if s.sl == 15 then s.sl = 31 end
    else
      s.wf = self.wse and band(v, 3) or 0
    end
  elseif reg >= 0xa0 and reg <= 0xa8 then
    local c = self.ch[reg - 0xa0]
    c.fnum = bor(band(c.fnum, 0x300), v)
    updateKsl(c)
  elseif reg >= 0xb0 and reg <= 0xb8 then
    local c = self.ch[reg - 0xb0]
    c.fnum = bor(band(c.fnum, 0xff), band(v, 3) * 256)
    c.block = band(rshift(v, 2), 7)
    updateKsl(c)
    local on = band(v, 0x20) ~= 0
    c.mod.key, c.car.key = on, on
  elseif reg >= 0xc0 and reg <= 0xc8 then
    local c = self.ch[reg - 0xc0]
    c.fb, c.con = band(rshift(v, 1), 7), band(v, 1)
  end
end

-- One step of an operator's envelope generator (Nuked's OPL3_EnvelopeCalc).
local function envelope(self, s, c)
  local eo = s.rout + s.tl * 4 + rshift(c.ksl, KSLSHIFT[s.ksl])
  if s.am ~= 0 then eo = eo + self.trem end
  s.egOut = eo > 0x1ff and 0x1ff or eo

  local reset, rate = false, 0
  if s.key and s.gen == RELEASE then
    reset, rate = true, s.ar
  elseif s.gen == ATTACK then rate = s.ar
  elseif s.gen == DECAY then rate = s.dr
  elseif s.gen == SUSTAIN then rate = s.egt == 0 and s.rr or 0
  else rate = s.rr end
  s.reset = reset

  local shift = 0
  local rateHi = 0
  if rate ~= 0 then
    local ks = rshift(c.ksv, s.ksr == 1 and 0 or 2)
    local r = ks + rate * 4
    rateHi = rshift(r, 2)
    local rateLo = band(r, 3)
    if rateHi > 15 then rateHi = 15 end
    if rateHi < 12 then
      if self.egState == 1 then
        local es = rateHi + self.egAdd
        if es == 12 then shift = 1
        elseif es == 13 then shift = band(rshift(rateLo, 1), 1)
        elseif es == 14 then shift = band(rateLo, 1) end
      end
    else
      shift = band(rateHi, 3) + INCSTEP[rateLo][self.egTimerLo]
      if band(shift, 4) ~= 0 then shift = 3 end
      if shift == 0 then shift = self.egState end
    end
  end

  local rout, inc = s.rout, 0
  if reset and rateHi == 15 then rout = 0 end
  local off = band(s.rout, 0x1f8) == 0x1f8
  if s.gen ~= ATTACK and not reset and off then rout = 0x1ff end
  local gen = s.gen
  if gen == ATTACK then
    if s.rout == 0 then s.gen = DECAY
    elseif s.key and shift > 0 and rateHi ~= 15 then
      inc = floor((-s.rout - 1) / 2 ^ (4 - shift))
    end
  elseif gen == DECAY then
    if rshift(s.rout, 4) == s.sl then s.gen = SUSTAIN
    elseif not off and not reset and shift > 0 then inc = lshift(1, shift - 1) end
  else
    if not off and not reset and shift > 0 then inc = lshift(1, shift - 1) end
  end
  s.rout = band(rout + inc, 0x1ff)
  if reset then s.gen = ATTACK end
  if not s.key then s.gen = RELEASE end
end

-- the phase generator, with the chip's vibrato
local function phaseGen(self, s, c)
  local f = c.fnum
  if s.vib ~= 0 then
    local range = band(rshift(f, 7), 7)
    local vp = self.vibPos
    if band(vp, 3) == 0 then range = 0
    elseif band(vp, 1) ~= 0 then range = rshift(range, 1) end
    if not self.dvb then range = rshift(range, 1) end
    if band(vp, 4) ~= 0 then range = -range end
    f = f + range
  end
  local base = rshift(lshift(f, c.block), 1)
  s.phaseOut = rshift(s.phase, 9)
  if s.reset then s.phase = 0 end
  s.phase = band(s.phase + rshift(base * MT[s.mult], 1), 0x7ffff)
end

--- True once no operator can make a sound: every key is up and every
--- envelope has run out. Generating then gives nothing but zeros.
function opl:silent()
  for i = 0, 17 do
    local s = self.slots[i]
    if s.key or s.gen ~= RELEASE or s.rout < 0x1f8 then return false end
  end
  return true
end

--- Fill out[first..first+n-1] with the next n samples.
function opl:generate(out, n, first)
  first = first or 1
  local chans = self.ch
  for i = first, first + n - 1 do
    local acc = 0
    for ci = 0, 8 do
      local c = chans[ci]
      local m, k = c.mod, c.car
      -- skip a channel both of whose envelopes are silent
      if m.key or k.key or m.rout < 0x1f8 or k.rout < 0x1f8 or m.gen ~= RELEASE or k.gen ~= RELEASE then
        -- modulator, with feedback from its last two outputs
        local fbmod = 0
        if c.fb ~= 0 then fbmod = floor((m.prout + m.out) / 2 ^ (9 - c.fb)) end
        m.prout = m.out
        envelope(self, m, c)
        phaseGen(self, m, c)
        m.out = wave(m.wf, m.phaseOut + fbmod, m.egOut)
        -- carrier, frequency-modulated by it (FM) or beside it (AM)
        k.prout = k.out
        envelope(self, k, c)
        phaseGen(self, k, c)
        if c.con == 0 then
          k.out = wave(k.wf, k.phaseOut + m.out, k.egOut)
          acc = acc + k.out
        else
          k.out = wave(k.wf, k.phaseOut, k.egOut)
          acc = acc + m.out + k.out
        end
      end
    end
    if acc > 32767 then acc = 32767 elseif acc < -32768 then acc = -32768 end
    out[i] = acc

    -- the chip's counters: tremolo every 64 samples, vibrato every 1024
    local t = self.timer
    if band(t, 0x3f) == 0x3f then self.tremPos = (self.tremPos + 1) % 210 end
    local tp = self.tremPos
    if tp >= 105 then tp = 210 - tp end
    self.trem = rshift(tp, self.dam and 2 or 4)
    if band(t, 0x3ff) == 0x3ff then self.vibPos = band(self.vibPos + 1, 7) end
    self.timer = band(t + 1, 0xffff)
    -- the envelope clock: runs at half the sample rate, and each step's
    -- trailing zeros pick which slow rates advance
    if self.egState == 1 then
      local et, sh = self.egTimer, 0
      while sh < 13 and band(rshift(et, sh), 1) == 0 do sh = sh + 1 end
      self.egAdd = sh > 12 and 0 or sh + 1
      self.egTimerLo = band(et, 3)
      self.egTimer = band(et + 1, 0xfffffff)
    end
    self.egState = 1 - self.egState
  end
end

return opl
