-- Diplomacy: the state between every pair of sides, and the proposals that
-- change it.
--
-- docs/rules.md > Diplomacy. The matrix is a byte per ordered pair at .SCN
-- 0x153b + 8*side + other: bits 0-1 the current state, bits 2-3 this side's
-- proposal. Proposals are applied at the start of the proposing side's turn
-- (`diplomacy_apply`, Ghidra 484e:0db3).

local diplomacy = {}

diplomacy.PEACE, diplomacy.INTERMEDIATE, diplomacy.WAR = 0, 1, 2
diplomacy.STATE_NAMES = { [0] = "peace", "uneasy", "war" }

-- STRING.DAT group 106, best first. Ratings are relative: the sides in play
-- are sorted by score, lowest first, and each rank takes a title.
diplomacy.TITLES = {
  "Statesman", "Diplomat", "Pragmatist", "Politician",
  "Deceiver", "Scoundrel", "Turncoat", "Running Dog",
}

-- which titles are used, by how many sides are in play (docs/rules.md)
diplomacy.RATING_RANKS = {
  [1] = { 1 },
  [2] = { 1, 8 },
  [3] = { 1, 4, 8 },
  [4] = { 1, 2, 6, 8 },
  [5] = { 1, 2, 4, 6, 8 },
  [6] = { 1, 2, 4, 6, 7, 8 },
  [7] = { 1, 2, 4, 5, 6, 7, 8 },
  [8] = { 1, 2, 3, 4, 5, 6, 7, 8 },
}

--------------------------------------------------------------------- state

local function key(a, b) return a * 8 + b end

--- Set every pair: war if the Diplomacy option is off, peace if it is on,
--- and every proposal to match. diplomacy_init, Ghidra 484e:11bd.
function diplomacy.init(g)
  local start = g.map.options.diplomacy ~= 0 and diplomacy.PEACE or diplomacy.WAR
  g.diplomacy = { state = {}, proposal = {} }
  for a = 0, 7 do
    for b = 0, 7 do
      local v = a == b and diplomacy.PEACE or start
      g.diplomacy.state[key(a, b)] = v
      g.diplomacy.proposal[key(a, b)] = v
    end
  end
end

--- The state between two sides. Either order gives the same answer in a
--- consistent game; the matrix keeps both halves.
function diplomacy.state(g, a, b)
  if not g.diplomacy or a == nil or b == nil then return diplomacy.WAR end
  return g.diplomacy.state[key(a, b)] or diplomacy.WAR
end

function diplomacy.atWar(g, a, b)
  return diplomacy.state(g, a, b) ~= diplomacy.PEACE
end

--- May `a` attack `b`? A side at peace may not; the game refuses with
--- STRING.DAT group 140, "Milord! Thou art attacking without first having
--- declared war".
function diplomacy.mayAttack(g, a, b)
  if a == nil or b == nil then return true end        -- neutrals are fair game
  return diplomacy.state(g, a, b) ~= diplomacy.PEACE
end

--- Propose a state to another side. The proposal stands until changed; it
--- is acted on at the start of each of this side's turns.
function diplomacy.propose(g, a, b, state)
  if not g.diplomacy or a == b then return end
  g.diplomacy.proposal[key(a, b)] = state
end

--- The state a side proposes to another -- the current state when nothing
--- was ever proposed.
function diplomacy.proposal(g, a, b)
  if not g.diplomacy then return nil end
  local p = g.diplomacy.proposal[key(a, b)]
  if p == nil then p = diplomacy.state(g, a, b) end
  return p
end

--------------------------------------------------------------- applying them

--- Apply one side's proposals. Returns a list of messages.
--
-- A proposal more hostile than the state takes effect at once, for both
-- sides, and the other side's proposal is raised to the new state if it was
-- below it. One less hostile only lands when the other side's proposal is no
-- more hostile. Proposals are not used up. diplomacy_apply, Ghidra
-- 484e:0db3.
function diplomacy.apply(g, side)
  if not g.diplomacy then return {} end
  local messages = {}
  local a = side.index

  for b = 0, 7 do
    local other = g.map.sides[b + 1]
    if b ~= a and other then
      local now = diplomacy.state(g, a, b)
      local want = diplomacy.proposal(g, a, b)
      if want ~= now then
        if want > now then                            -- escalation: at once
          g.diplomacy.state[key(a, b)] = want
          g.diplomacy.state[key(b, a)] = want
          if diplomacy.proposal(g, b, a) < want then
            g.diplomacy.proposal[key(b, a)] = want
          end
          if want == diplomacy.WAR then
            local history = require("warlords.history")
            history.deed(g, side, history.WAR, a, b, "")          -- 484e:0f15
            messages[#messages + 1] = ("War declared with %s!"):format(other.name)
          end
        elseif diplomacy.proposal(g, b, a) <= want then -- de-escalation: mutual
          g.diplomacy.state[key(a, b)] = want
          g.diplomacy.state[key(b, a)] = want
          if want == diplomacy.PEACE then
            local history = require("warlords.history")
            history.deed(g, side, history.PEACE, a, b, "")         -- 484e:1027
            messages[#messages + 1] = ("Peace negotiated with %s!"):format(other.name)
          end
        end
      end
    end
  end
  return messages
end

--- At the end of a side's turn, its peace overtures count against it
--- (diplomacy_score_update, 484e:1063, from the end of ai_turn and of a
--- human's turn): every proposal less hostile than both the state and the
--- other side's proposal adds to its diplomatic score.
function diplomacy.scoreUpdate(g, side)
  if not g.diplomacy then return end
  local a = side.index
  for b = 0, 7 do
    if b ~= a and g.map.sides[b + 1] then
      local p, st = diplomacy.proposal(g, a, b), diplomacy.state(g, a, b)
      if p < st and p < diplomacy.proposal(g, b, a) then
        diplomacy.addScore(g, side, st, p)
      end
    end
  end
end

--- What a peaceful move adds to the **diplomatic score** (484e:1063):
--   peace from uneasy, or uneasy from war -> 1d2+1
--   peace from war                        -> 1d10+10
--
-- This is the *same* score that pillage, sack and raze raise. It is not a
-- virtue: the rating sorts it lowest-first, so a side that neither commits
-- atrocities nor sues for peace is the Statesman, and one that does either
-- often ends up the Running Dog.
function diplomacy.addScore(g, side, from, to)
  local gain
  if from == diplomacy.WAR and to == diplomacy.PEACE then
    gain = g.rng:dice(1, 10, 10)
  elseif to < from then
    gain = g.rng:dice(1, 2, 1)
  end
  if gain then
    require("warlords.game").addDiploScore(g, side, gain)
  end
  return gain
end

--------------------------------------------------------------------- ratings

--- Every side's diplomatic title. Ratings are *relative*: sides are sorted by
--- score, lowest (best behaved) first, and each rank takes a title.
-- diplomatic_rating, Ghidra 484e:0aed.
function diplomacy.ratings(g)
  local order = {}
  for i, s in ipairs(g.sides) do
    order[#order + 1] = { side = s, score = s.diploScore or 0, seq = i }
  end
  table.sort(order, function(p, q)
    if p.score ~= q.score then return p.score < q.score end
    return p.seq < q.seq                              -- ties keep side order
  end)

  local ranks = diplomacy.RATING_RANKS[#order] or diplomacy.RATING_RANKS[8]
  local out = {}
  for i, entry in ipairs(order) do
    out[entry.side.index] = diplomacy.TITLES[ranks[i] or #diplomacy.TITLES]
  end
  return out
end

return diplomacy
