-- The computer players' diplomacy (ai_phase_diplomacy, 558d:0000): each
-- turn the side sets every proposal afresh. docs/re/ai.md > Diplomacy.

local core = require("warlords.ai.core")

local diplomacy = {}

--- May the side go for this city (558d:0851)? A neutral city, or a side it
--- is at war with, always. A side it is at peace with only when it is at
--- war with nobody else -- and a side in between -- only when its card is
--- bold (+0x0c bit 0).
function diplomacy.canAttack(g, side, c)
  local owner = core.owner(c)
  if owner == core.NEUTRAL then return true end
  local d = core.data(g, side)
  local wars, last = 0, nil
  for s = 7, 0, -1 do
    if core.inPlay(g, s) and core.state(g, side.index, s) == 2 then wars, last = wars + 1, s end
  end
  local st = core.state(g, side.index, owner)
  if st == 2 then return true end
  if st == 0 and wars ~= 0 and (wars ~= 1 or last ~= owner) then return false end
  if st > 2 then return false end
  return d.bold and true or false
end

--- Solidarity with a side (558d:0a6e): with Diplomacy on, a card with
--- solidarity, and the side a computer, it spares `other` when `other` is at
--- war with a human whose own solidarity mark is set. Only a side that was
--- once a computer and is now a human carries that mark, so in play this
--- is nearly always false -- as it is in the original.
function diplomacy.spared(g, side, other)
  local d = core.data(g, side)
  if g.map.options.diplomacy == 0 or d.solidarity == 0 or not side.computer then return false end
  for h = 7, 0, -1 do
    local hs = g.map.sides[h + 1]
    if core.inPlay(g, h) and not hs.computer and core.state(g, other, h) == 2
       and (hs.aiSolidarity or 0) ~= 0 then
      return true
    end
  end
  return false
end

--- Claim one more city as the side's own ground (558d:0917): the nearest to
--- its capital, within 40, that is claimed by nobody -- or by a side out of
--- the game.
local function claim(g, side)
  local cap = side.capital
  if not cap then return end
  local best, bestD
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    local cl = c.claim or core.NEUTRAL
    if core.standing(c) and cl ~= side.index and (cl == core.NEUTRAL or not core.inPlay(g, cl)) then
      local dd = core.dist(c.x, c.y, cap.x, cap.y)
      if dd < 40 and (not bestD or dd < bestD) then best, bestD = c, dd end
    end
  end
  if best then best.claim = side.index end
end

--- diplomacy (ai_phase_diplomacy, 558d:0000).
function diplomacy.phase(g, side)
  local d = core.data(g, side)
  local me = side.index
  local human = not side.computer
  claim(g, side)
  if g.map.options.diplomacy == 0 then return end
  local groups = require("warlords.ai.groups")
  local enemy = groups.pickEnemy(g, side)

  local swapped, total, seen, theyWar, theyPeace = {}, {}, {}, {}, {}
  for b = 7, 0, -1 do
    core.propose(g, me, b, 0)
    swapped[b], total[b], seen[b], theyWar[b], theyPeace[b] = 0, 0, 0, 0, 0
    local p = core.proposal(g, b, me)
    if p == 2 then theyWar[b] = 1 elseif p == 0 then theyPeace[b] = 1 end
  end
  local tiles = 0
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if core.standing(c) then tiles = tiles + 1 end
    local o = c.ownerIndex
    if o ~= nil then
      total[o] = total[o] + 1
      if not core.cflag(d, c, core.CF_UNSEEN) then seen[o] = seen[o] + 1 end
      local cl = c.claim or core.NEUTRAL
      if o == me then
        if cl ~= core.NEUTRAL then swapped[cl] = swapped[cl] + 1 end
      elseif cl == me then
        swapped[o] = swapped[o] + 1
      end
    end
  end
  -- the grudge tests; each has a war branch that can never run (docs/re/ai.md)
  for b = 7, 0, -1 do
    if b ~= me and core.inPlay(g, b) then
      local grudge = (theyPeace[b] - theyWar[b]) * 2
      if swapped[b] >= grudge + 4 then core.propose(g, me, b, 1) end
    end
  end
  for b = 7, 0, -1 do
    if b ~= me and core.inPlay(g, b) then
      local grudge = (theyPeace[b] - theyWar[b]) * 2
      local threat = d.lost[b] + d.citiesLost[b] * 4 + d.heroesKilled[b] * 2
      if threat >= grudge + 5 then
        core.propose(g, me, b, math.max(1, core.proposal(g, me, b)))
      end
    end
  end
  if enemy then core.propose(g, me, enemy, 2) end
  for gi = core.MAX_GROUPS, 1, -1 do
    local grp = d.groups[gi]
    if grp.active ~= 0 and grp.target then core.propose(g, me, grp.target, 2) end
  end
  -- whoever holds too much of the world: 50% of the cities for a computer,
  -- the card's share for a human
  local leader, most = nil, 0
  for s = 7, 0, -1 do
    if core.inPlay(g, s) then
      local share = math.floor(total[s] * 100 / math.max(1, tiles))
      local limit = core.isComputer(g, s) and 50 or d.humanShare
      if most < total[s] and limit < share then leader, most = s, total[s] end
    end
  end
  if leader and leader ~= me then
    for s = 7, 0, -1 do
      if core.inPlay(g, s) then
        core.propose(g, me, s, (s == leader or s == enemy) and 2 or 0)
      end
    end
  end
  if not human and d.solidarity ~= 0 then
    for s = 7, 0, -1 do
      if core.inPlay(g, s) and core.isComputer(g, s) and s ~= me
         and core.proposal(g, me, s) ~= 0 and diplomacy.spared(g, side, s) then
        core.propose(g, me, s, 0)
      end
    end
    local atWarWithHuman = false
    for s = 7, 0, -1 do
      if core.inPlay(g, s) and not core.isComputer(g, s) and core.state(g, me, s) == 2 then
        atWarWithHuman = true
      end
    end
    if atWarWithHuman then
      for s = 7, 0, -1 do
        if core.inPlay(g, s) and core.isComputer(g, s)
           and (g.map.sides[s + 1].aiSolidarity or 0) ~= 0 then
          core.propose(g, me, s, 0)
        end
      end
    end
  end
  if not human and g.greatest then
    local fights = groups.fightingHumans(g)
    for s = 7, 0, -1 do
      if core.inPlay(g, s) and core.isComputer(g, s) and (fights[me] or fights[s]) then
        core.propose(g, me, s, 0)
      end
    end
  end
  for b = 7, 0, -1 do
    if seen[b] == 0 then core.propose(g, me, b, 0) end
  end
  for gi = core.MAX_GROUPS, 1, -1 do
    local grp = d.groups[gi]
    if grp.active ~= 0 and grp.target and core.proposal(g, me, grp.target) == 0 then
      groups.cancel(g, side, gi)
    end
  end
end

return diplomacy
