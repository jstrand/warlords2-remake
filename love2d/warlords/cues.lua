-- What the game plays, and when: the choices behind the music and the
-- advisor's voice, as WARLORD2.EXE makes them. docs/re/sound.md.
--
-- Headless. The file names come from DATA/FILE.DAT (uidata.strings); what
-- is done with them -- synthesis, playback -- is sound.lua's business.
--
-- The original rolls its own dice() for these picks, so with music or speech
-- on it draws from the game's one random stream. The engine's stream is not
-- the original's anyway (rng.lua), and a game should not play out
-- differently because the music is on, so the caller passes a roll of its
-- own: roll(n) -> 1..n.

local armytype = require("warlords.armytype")

local cues = {}

-- The argument of 6dda:0000, the music selector, and who plays each.
cues.TITLE     = 0  -- start screens and setup (7f77:0000, 7bab:0000)
cues.PLAY      = 1  -- a human's turn, once it opens (8cc6:04bd); after a load
cues.COMPUTER  = 2  -- a computer's turn begins (8065:2123); plays once
cues.TRIUMPH   = 3  -- a quest done (4976:1ded), the game won (8065:1fbd)
cues.HERO      = 4  -- a hero offers to join (8cc6:04bd)
cues.TEMPLE    = 5  -- a temple visited (4976:0000)
cues.SAGE      = 6  -- the sage (6536:0aa0)
cues.PROMOTION = 7  -- a hero goes up a level (7563:0672)
cues.MEDAL     = 8  -- an army wins a medal (67cc:2274)
cues.SURRENDER = 9  -- the computers offer to surrender (8065:1f68)
cues.DEFIANCE  = 10 -- the offer turned down (8065:1ecd)
cues.BEGIN     = 11 -- the war begins (7bab:0cfe)

-- FILE.DAT group and index of each cue's song; index -1 is a random entry,
-- as string_lookup rolls dice(1, count, -1) for a negative index.
local SONG = {
  [0] = { 8, 0 }, { 10, -1 }, nil, { 9, -1 }, { 12, 0 }, { 13, -1 }, { 14, -1 },
  { 15, 0 }, { 16, 0 }, { 17, 0 }, { 18, 0 }, { 19, 0 },
}

-- one entry of a FILE.DAT group, numbered as the executable numbers them
local function entry(files, group, index, roll)
  local g = files[group + 1] or {}
  if index < 0 then index = roll(#g) - 1 end
  if index > #g - 1 then index = #g - 1 end
  return g[index + 1]
end

--- The song for a cue: its FILE.DAT name ("INT12.XMI", "STARTUP.XMI") and
--- whether it starts again when it ends -- every cue loops but the computer's.
--- `sides` are the game's sides, for the computer's cue.
function cues.song(files, cue, sides, roll)
  if cue == cues.COMPUTER then
    -- with a human still in play, a computer's turn gets group 11; in a
    -- game of computers alone, 5% group 12, 47% group 11 and 48% group 10
    local human = false
    for _, s in ipairs(sides or {}) do
      if s.alive ~= false and not s.computer then human = true break end
    end
    local group = 11
    if not human then
      local r = roll(100)
      if r < 6 then group = 12 elseif r >= 53 then group = 10 end
    end
    return entry(files, group, -1, roll), false
  end
  local s = SONG[cue]
  if not s then return nil end
  return entry(files, s[1], s[2], roll), true
end

------------------------------------------------------------------ the advisor

-- The advisor's clips are FILE.DAT groups 40-62, each with its subtitle 30
-- groups on (70-88) -- or, for the four set phrases, at index 1 of its own.
cues.GREET, cues.MOMENT, cues.BEGIN_WAR, cues.QUIT = 59, 60, 61, 62

-- 6dda:026f(5), the voice at the start of a human's turn. It keeps a mark
-- a side -- .SCN 0x100 (1 last said winning, 2 losing) and 0x108 (the city
-- count it was said at, rounded down to 5) -- here side.advisor.
local LOSING = { [5] = 40, [10] = 41, [15] = 42, [20] = 43, [25] = 44, [30] = 45 }
local WINNING = { [10] = 48, [15] = 49, [20] = 50, [25] = 51, [30] = 52, [35] = 53 }

--- What the advisor says as `side`'s turn opens: a FILE.DAT group, or nil
--- when he keeps quiet. Updates side.advisor as the original does -- even on
--- turn 1, when he never speaks.
function cues.advisor(g, side, roll)
  local cities, heroes = 0, 0
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex == side.index and not c.razed then cities = cities + 1 end
  end
  if cities > 39 then return nil end
  for _, a in ipairs(g.armies) do
    if a.owner == side.index and a.type == armytype.HERO then heroes = heroes + 1 end
  end
  side.advisor = side.advisor or { dir = 0, mark = 0 }
  local adv = side.advisor
  local group
  if cities < adv.mark then
    -- fallen below the last mark: say how far, by the new one
    local m = math.floor(cities / 5) * 5
    adv.dir, adv.mark = 2, m
    group = LOSING[m + 5] or 46
  elseif adv.mark + 5 <= cities then
    local m = math.floor(cities / 5) * 5
    adv.dir, adv.mark = 1, m
    group = WINNING[m] or 47
  else
    -- otherwise only every seventh turn, and then about the treasury or
    -- the heroes, or one time in five something general
    if g.turn % 7 ~= 0 then return nil end
    if side.gold < 100 then group = 54
    elseif side.gold > 2800 then group = 55
    elseif heroes < 1 then group = 56
    elseif heroes > 4 then group = 57
    elseif roll(5) == 1 then group = 58
    else return nil end
  end
  if g.turn == 1 then return nil end
  return group
end

--- The clip's sample and subtitle names for an advisor group: FILE.DAT
--- entries, one picked at random where a group has several.
function cues.clip(files, group, roll)
  if group >= cues.GREET then
    return entry(files, group, 0, roll), entry(files, group, 1, roll)
  end
  local g = files[group + 1] or {}
  local i = roll(#g)
  local t = files[group + 31] or {}
  return g[i], t[math.min(i, #t)]
end

return cues
