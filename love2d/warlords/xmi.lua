-- XMIDI, the music format of Miles' Audio Interface Library (SOUND/*.XMI).
--
-- An IFF file: FORM XDIR (how many sequences), then CAT XMID holding one
-- FORM XMID per sequence, each with a TIMB chunk (the timbres it uses) and
-- EVNT, the events. EVNT is MIDI with two differences:
--
--   - time is counted in intervals of AIL's 120 Hz timer, and a delay is a
--     run of bytes below 0x80 that are simply added up;
--   - a note-on carries its own duration, a MIDI variable-length number
--     after the velocity, and there are no note-offs.
--
-- Tempo meta events are left in by the converter but mean nothing: the
-- delays already have the tempo baked in.
--
-- parse() flattens the first sequence into time order, turning each note's
-- duration into a note-off. At a tick where notes end and others begin the
-- ends come first, as AIL's timer handler retires expired notes before it
-- reads on in the stream -- which matters when voices are short.

local xmi = {}

xmi.TICK_RATE = 120

local function u32be(s, i)
  local a, b, c, d = s:byte(i, i + 3)
  return ((a * 256 + b) * 256 + c) * 256 + d
end

-- the EVNT chunk of the first sequence
local function findEvents(s)
  local i = 1
  local function walk(from, to)
    local p = from
    while p + 8 <= to do
      local id, len = s:sub(p, p + 3), u32be(s, p + 4)
      if id == "FORM" or id == "CAT " then
        local at, size = walk(p + 12, p + 8 + len)
        if at then return at, size end
      elseif id == "EVNT" then
        return p + 8, len
      end
      p = p + 8 + len + len % 2
    end
  end
  return walk(i, #s + 1)
end

--- Parse an XMI file's bytes. Returns { length = ticks, n = count,
--- t = {...}, st = {...}, a = {...}, b = {...} }: parallel arrays of each
--- event's tick, MIDI status byte and data bytes. Note-offs are status
--- 0x80 | channel. Meta events are dropped, except the end of the track.
function xmi.parse(s)
  local start, len = findEvents(s)
  if not start then return nil, "no EVNT chunk" end
  local stop = start + len
  local raw = {} -- { tick, order, status, a, b }
  local p, t, order = start, 0, 0

  local function varlen()
    local v = 0
    repeat
      local c = s:byte(p)
      p = p + 1
      v = v * 128 + c % 128
    until c < 128
    return v
  end

  local finish = 0
  while p < stop do
    local c = s:byte(p)
    if c < 0x80 then
      t = t + c
      p = p + 1
    else
      p = p + 1
      local hi = c - c % 16
      if c == 0xff then
        local kind = s:byte(p)
        p = p + 1
        local n = varlen()
        p = p + n
        if kind == 0x2f then break end
      elseif c == 0xf0 or c == 0xf7 then
        p = p + varlen()
      elseif hi == 0x90 then
        local key, vel = s:byte(p, p + 1)
        p = p + 2
        local dur = varlen()
        order = order + 1
        raw[#raw + 1] = { t, order, c, key, vel }
        -- the note's end sorts before anything else at its tick
        raw[#raw + 1] = { t + dur, -1, 0x80 + c % 16, key, 0 }
        if t + dur > finish then finish = t + dur end
      elseif hi == 0xc0 or hi == 0xd0 then
        order = order + 1
        raw[#raw + 1] = { t, order, c, s:byte(p), 0 }
        p = p + 1
      else
        order = order + 1
        raw[#raw + 1] = { t, order, c, s:byte(p, p + 1) }
        p = p + 2
      end
    end
  end
  if t > finish then finish = t end

  -- a stable sort: by tick, note-offs first, then the file's own order
  for i, e in ipairs(raw) do e[6] = i end
  table.sort(raw, function(x, y)
    if x[1] ~= y[1] then return x[1] < y[1] end
    if (x[2] < 0) ~= (y[2] < 0) then return x[2] < 0 end
    return x[6] < y[6]
  end)
  local seq = { length = finish, n = #raw, t = {}, st = {}, a = {}, b = {} }
  for i, e in ipairs(raw) do
    seq.t[i], seq.st[i], seq.a[i], seq.b[i] = e[1], e[3], e[4], e[5] or 0
  end
  return seq
end

return xmi
