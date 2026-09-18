-- A computer player.
--
-- The phase order, the city roles, the production purposes and the garrison
-- sizes are the original's (docs/re/ai.md). The *movement* decisions are not:
-- the original's assault groups and target scoring are documented but depend
-- on per-side statistics a remake has no reason to keep, so `ai.phaseAttack`
-- and `ai.phaseOrders` below are ours, and are marked as such. Everything that
-- claims a Ghidra address is decoded.

local armytype = require("warlords.armytype")
local combat   = require("warlords.combat")
local move     = require("warlords.move")
local rules    = require("warlords.rules")

local ai = {}

-- city roles (docs/re/ai.md). 9, 10 and 12 are never assigned by the original
-- either -- its production switch has dead rows for them.
ai.JUST_TAKEN, ai.TAKING_NEUTRAL, ai.NEAR_NEUTRAL = 1, 2, 3
ai.WEAK_GARRISON, ai.BUILDING_UP, ai.GROUP_MEMBER = 4, 5, 6
ai.GROUP_TARGET, ai.STOP, ai.EXPLORER = 7, 8, 13

-- role -> production purpose. ai_production, Ghidra 5db9:06d4.
ai.PURPOSE_FOR_ROLE = {
  [2] = 4, [3] = nil,     -- role 3 is decided by the Neutral Cities option
  [4] = 2, [5] = 3, [6] = 3, [7] = 3, [8] = false,
  [11] = 4, [13] = 4,
}

ai.MIN_GOLD_TO_BUILD = 40      -- below this, and with income under upkeep, stop
ai.REBUILD_GOLD = 500          -- what "rebuilding" wants in hand

--------------------------------------------------------------------- helpers

local function cityTiles(city)
  local out = {}
  for dx = 0, 1 do for dy = 0, 1 do out[#out + 1] = { city.x + dx, city.y + dy } end end
  return out
end

local function distance(ax, ay, bx, by)
  return math.max(math.abs(ax - bx), math.abs(ay - by))
end

--- The cities next to a city, as the original's neighbour table would give
--- them: the six nearest, by map distance (5db9:0656 keeps a precomputed list).
local function neighbours(g, city, n)
  local out = {}
  for _, c in ipairs(g.map.cities) do
    if c ~= city then
      out[#out + 1] = { city = c, d = distance(city.x, city.y, c.x, c.y) }
    end
  end
  table.sort(out, function(p, q) return p.d < q.d end)
  local picked = {}
  for i = 1, math.min(n or 6, #out) do picked[#picked + 1] = out[i].city end
  return picked
end

--------------------------------------------------------------------- phases

--- evaluate: recount, resolve role 1, and keep the role bytes tidy.
-- ai_phase_evaluate, Ghidra 59bf:0000.
function ai.phaseEvaluate(g, side)
  local gameMod = require("warlords.game")
  side.ai = side.ai or { roles = {} }
  local roles = side.ai.roles

  local own, enemy, neutral = 0, 0, 0
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex == side.index then
      own = own + 1
      if roles[c.index] == nil then roles[c.index] = ai.JUST_TAKEN end
    else
      roles[c.index] = nil
      if c.ownerIndex == nil then neutral = neutral + 1 else enemy = enemy + 1 end
    end
  end
  side.ai.own, side.ai.enemy, side.ai.neutral = own, enemy, neutral

  -- role 1 resolves by whether a neutral city is among the six neighbours
  for _, c in ipairs(gameMod.sideCities(g, side)) do
    if roles[c.index] == ai.JUST_TAKEN then
      local near = false
      for _, n in ipairs(neighbours(g, c)) do
        if n.ownerIndex == nil then near = true end
      end
      roles[c.index] = near and ai.NEAR_NEUTRAL or ai.BUILDING_UP
    end
  end
end

--- The garrison a city wants: 8 at war with a neighbour's owner, else 4 for
--- two or more foreign neighbours, 3 for one, 2 for none.
-- ai_wanted_garrison, Ghidra 5ca7:0a3d.
function ai.wantedGarrison(g, side, city)
  local foreign, atWar = 0, false
  for _, n in ipairs(neighbours(g, city)) do
    if n.ownerIndex ~= nil and n.ownerIndex ~= side.index then
      foreign = foreign + 1
      atWar = true              -- without diplomacy every other side is an enemy
    end
  end
  if atWar then return 8 end
  if foreign >= 2 then return 4 end
  if foreign == 1 then return 3 end
  return 2
end

--- neutral: a city with a neutral neighbour hunts it, one without settles
--- down. ai_neutral_targets, Ghidra 57ea:00b5.
function ai.phaseNeutral(g, side)
  local gameMod = require("warlords.game")
  local roles = side.ai.roles
  for _, c in ipairs(gameMod.sideCities(g, side)) do
    local near = false
    for _, n in ipairs(neighbours(g, c)) do
      if n.ownerIndex == nil then near = true end
    end
    local role = roles[c.index]
    if near then
      if role ~= ai.TAKING_NEUTRAL then roles[c.index] = ai.NEAR_NEUTRAL end
    elseif role == ai.NEAR_NEUTRAL or role == ai.TAKING_NEUTRAL then
      roles[c.index] = ai.WEAK_GARRISON
    end
  end
end

--- Set each city's role from its garrison. ai_city_garrison_check, 5ca7:023f
--- decides the 4 / 5 / 8 split; **which** cities are allowed to stop is ours:
--- the original relies on its assault groups to keep roles moving, so a
--- straight port of the check leaves every city on role 8 and the side never
--- builds anything again.
function ai.phaseGarrisons(g, side)
  local gameMod = require("warlords.game")
  local roles = side.ai.roles
  for _, c in ipairs(gameMod.sideCities(g, side)) do
    local here = #gameMod.armiesAt(g, c.x, c.y)
    local want = ai.wantedGarrison(g, side, c)
    if here >= rules.MAX_STACK then
      roles[c.index] = ai.STOP             -- nothing more fits on the tile
    elseif here < 2 then
      roles[c.index] = ai.WEAK_GARRISON
    elseif here < want then
      roles[c.index] = ai.BUILDING_UP
    elseif ai.hasTargetNear(g, side, c) then
      roles[c.index] = ai.BUILDING_UP      -- there is still somewhere to go
    else
      roles[c.index] = ai.STOP
    end
  end
end

--- Is there a city this side does not own within striking distance?
function ai.hasTargetNear(g, side, city)
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex ~= side.index and not c.razed
       and distance(city.x, city.y, c.x, c.y) <= 25 then
      return true
    end
  end
  return false
end

--- Choose what every city builds. ai_production, Ghidra 5db9:06d4.
function ai.phaseProduction(g, side)
  local gameMod = require("warlords.game")
  local cities = gameMod.sideCities(g, side)
  for i = #cities, 1, -1 do
    local c = cities[i]
    -- stop the whole phase when the side is broke and running at a loss
    if side.gold < ai.MIN_GOLD_TO_BUILD
       and (side.income or 0) < (side.upkeepTotal or 0) then
      return
    end
    if not c.producing then
      local role = side.ai.roles[c.index] or ai.BUILDING_UP
      local purpose = ai.PURPOSE_FOR_ROLE[role]
      if role == ai.NEAR_NEUTRAL then
        purpose = g.map.options.neutralCities ~= 0 and 2 or 1
      elseif purpose == nil then
        purpose = 3
      end
      if purpose then
        local slot = rules.bestSlot(c.slots, purpose, g.types, side.enhanced)
        -- purpose 4 wants a flier and may find none
        if not slot and purpose == 4 then
          slot = rules.bestSlot(c.slots, 3, g.types, side.enhanced)
        end
        if slot then
          for n, s in ipairs(c.slots) do
            if s == slot then gameMod.setProduction(g, c, n) end
          end
        end
      end
    end
  end
end

--------------------------------------------------------- movement (ours, not
--------------------------------------------------------- the original's)

--- What a city is worth attacking from here, or nil if it is too far.
local function targetScore(g, side, from, city)
  local d = distance(from.x, from.y, city.x, city.y)
  if d > 25 then return nil end
  local defenders = 0
  for _, t in ipairs(cityTiles(city)) do
    defenders = defenders + #require("warlords.game").armiesAt(g, t[1], t[2])
  end
  local score = 100 - d * 3 - defenders * 8
  if city.ownerIndex == nil then score = score + 12 end   -- neutrals are cheaper
  return score
end

--- Give idle armies a standing order to march on a city. The original keeps
--- the order in the army record (+14/+15) and its assault groups decide the
--- target; the scoring here is ours.
function ai.phaseOrders(g, side)
  local gameMod = require("warlords.game")
  for _, c in ipairs(gameMod.sideCities(g, side)) do
    local here = gameMod.armiesAt(g, c.x, c.y)
    local spare = {}
    for i = 3, #here do                              -- always leave two behind
      if not here[i].order then spare[#spare + 1] = here[i] end
    end
    if #spare > 0 then
      local best, bestScore
      for _, target in ipairs(g.map.cities) do
        if target.ownerIndex ~= side.index and not target.razed then
          local s = targetScore(g, side, c, target)
          if s and (not bestScore or s > bestScore) then best, bestScore = target, s end
        end
      end
      if best then
        -- send them as one stack, up to the stack limit
        for i = 1, math.min(#spare, rules.MAX_STACK) do
          spare[i].order = { target = best.index }
        end
      end
    end
  end
end

--- March every army that has an order, and attack when it arrives.
function ai.phaseMove(g, side)
  local gameMod = require("warlords.game")

  -- group the ordered armies by tile and target, so a stack moves together
  local groups = {}
  for _, a in ipairs(gameMod.sideArmies(g, side)) do
    if a.order and not a.transit and (a.moves or 0) > 0 then
      local k = ("%d,%d,%d"):format(a.x, a.y, a.order.target)
      groups[k] = groups[k] or {}
      local group = groups[k]
      if #group < rules.MAX_STACK then group[#group + 1] = a end
    end
  end

  for _, stack in pairs(groups) do
    local city = g.map.cities[stack[1].order.target + 1]
    if not city or city.ownerIndex == side.index or city.razed then
      for _, a in ipairs(stack) do a.order = nil end     -- the order is stale
    else
      ai.sendAt(g, side, stack, city)
    end
  end
end

--- Walk a stack at a city, attacking if it reaches it. One path per turn: the
--- walk stops when the stack runs out of movement anyway.
function ai.sendAt(g, side, stack, city)
  local gameMod = require("warlords.game")
  if #stack == 0 or (stack[1].moves or 0) <= 0 then return end

  local path = move.findPath(g, stack, stack[1].x, stack[1].y, city.x, city.y)
  if not path then
    for _, a in ipairs(stack) do a.order = nil end     -- nowhere to go
    return
  end
  if #path == 0 then return end

  local r = move.walk(g, stack, path)
  if r.stopped == "attack" then
    local result = gameMod.resolveAttack(g, stack, r.attack.x, r.attack.y)
    if result.won then
      for _, a in ipairs(stack) do
        if not result.deadByArmy[a] then a.order = nil end
      end
    end
  end
end

--------------------------------------------------------------- diplomacy

ai.LEADER_SHARE = 50           -- % of all cities that makes a side the target

--- Set this side's proposals. The original clears them all first and then
--- works through its grudge, threat, leader and assault tests
--- (docs/re/ai.md > Diplomacy); the two escalate-to-war branches in its own
--- grudge and threat tests are unreachable, so war only ever comes from the
--- last three. This keeps those three.
function ai.phaseDiplomacy(g, side)
  local gameMod = require("warlords.game")
  local diplomacy = require("warlords.diplomacy")
  if g.map.options.diplomacy == 0 then return end     -- everyone is already at war

  local total = #g.map.cities
  local leader, leaderCities = nil, 0
  for _, s in ipairs(g.sides) do
    if s.alive then
      local n = #gameMod.sideCities(g, s)
      if n > leaderCities then leader, leaderCities = s, n end
    end
  end

  -- whoever we have armies marching on
  local marchingOn = {}
  for _, a in ipairs(gameMod.sideArmies(g, side)) do
    if a.order then
      local city = g.map.cities[a.order.target + 1]
      if city and city.ownerIndex then marchingOn[city.ownerIndex] = true end
    end
  end

  for _, other in ipairs(g.sides) do
    if other.alive and other.index ~= side.index then
      local wantWar = marchingOn[other.index]
      if leader == other and leaderCities * 100 // math.max(1, total) > ai.LEADER_SHARE then
        wantWar = true
      end
      diplomacy.propose(g, side.index, other.index,
                        wantWar and diplomacy.WAR or diplomacy.PEACE)
    end
  end
end

---------------------------------------------------------------- hero errands

--- Send each hero, with the stack it stands in, at the nearest unsearched
--- ruin. The scoring is the original's (ai_send_hero_party, Ghidra 5ad0:11a6):
--- 215 - d inside 15 tiles, 90 - d inside 40, nothing further.
function ai.phaseHeroes(g, side)
  local gameMod = require("warlords.game")
  local siteMod = require("warlords.site")
  local taken = {}

  for _, a in ipairs(gameMod.sideArmies(g, side)) do
    if a.type == armytype.HERO and not a.transit and (a.moves or 0) > 0 then
      -- a temple is worth a visit when the side has no quest in hand
      local wantTemple = g.map.options.quests ~= 0 and side.quest == nil
      local best, bestScore
      for _, s in ipairs(g.map.sites) do
        local worth = s.content ~= siteMod.TEMPLE or wantTemple
        if not s.searched and not taken[s] and worth then
          local d = distance(a.x, a.y, s.x, s.y)
          local score = d < 15 and (215 - d) or (d < 40 and (90 - d) or nil)
          if score and (not bestScore or score > bestScore) then best, bestScore = s, score end
        end
      end
      if best then
        taken[best] = true
        -- the hero takes one companion, as the original does
        local stack = { a }
        for _, mate in ipairs(gameMod.armiesAt(g, a.x, a.y)) do
          if mate ~= a and mate.owner == side.index and #stack < 2 then
            stack[#stack + 1] = mate
          end
        end
        local path = move.findPath(g, stack, a.x, a.y, best.x, best.y)
        if path and #path > 0 then
          move.walk(g, stack, path)
          local found = gameMod.searchHere(g, stack)
          if found and found.kind == "killed" then break end   -- the hero is gone
        end
        taken[best] = true
      end
    end
  end
end

---------------------------------------------------------------- exploring

--- Walk spare stacks towards the edge of what the side has seen.
--
-- Ours, not the original's: it has dedicated explore and search phases and
-- marks whole cities as explorers (roles 11 and 13), which this does not
-- reproduce. It shows: with *Hidden Map* on the computer players expand much
-- more slowly than they do with it off, because they spend too long feeling
-- their way around. Worth replacing with the real phases before anyone plays
-- a fog game seriously.
function ai.phaseExplore(g, side)
  if g.map.options.hiddenMap == 0 then return end
  local gameMod = require("warlords.game")

  -- the frontier: seen tiles that touch something unseen
  local frontier = {}
  local mask = (g.explored or {})[side.index] or {}
  for k in pairs(mask) do
    local x, y = k % g.map.width, k // g.map.width
    local edge = false
    for dx = -1, 1 do
      for dy = -1, 1 do
        local nx, ny = x + dx, y + dy
        if nx >= 0 and ny >= 0 and nx < g.map.width and ny < g.map.height
           and not gameMod.seen(g, side.index, nx, ny) then
          edge = true
        end
      end
    end
    if edge then frontier[#frontier + 1] = { x = x, y = y } end
  end
  if #frontier == 0 then return end

  -- who can go: anything already in the field without an order, plus the
  -- surplus in each city beyond the two that hold it
  local scouts = {}
  local inCity = {}
  for _, c in ipairs(gameMod.sideCities(g, side)) do
    local here = gameMod.armiesAt(g, c.x, c.y)
    for i, a in ipairs(here) do
      inCity[a] = true
      -- the attack phase has first call on the third army; only the fourth
      -- and beyond are spare enough to go wandering
      if i >= 4 and not a.order and (a.moves or 0) > 0 then scouts[#scouts + 1] = a end
    end
  end
  for _, a in ipairs(gameMod.sideArmies(g, side)) do
    if not inCity[a] and not a.transit and not a.order and (a.moves or 0) > 0 then
      scouts[#scouts + 1] = a
    end
  end

  for _, scout in ipairs(scouts) do
    -- head for the *far* edge of what we know, so a scout covers ground
    -- instead of shuffling one tile at a time; keep trying, because a
    -- frontier tile may be across water
    local candidates = {}
    for _, f in ipairs(frontier) do
      local d = distance(scout.x, scout.y, f.x, f.y)
      if d > 0 then candidates[#candidates + 1] = { f = f, d = d } end
    end
    table.sort(candidates, function(p, q) return p.d > q.d end)
    for i = 1, math.min(#candidates, 8) do
      local f = candidates[i].f
      local path = move.findPath(g, { scout }, scout.x, scout.y, f.x, f.y)
      if path and #path > 0 then
        move.walk(g, { scout }, path)
        break
      end
    end
  end
end

--------------------------------------------------------------------- the turn

--- Play one computer turn. The phase order is the original's, minus the
--- phases a remake has no use for (docs/re/ai.md > Turn pipeline).
function ai.playTurn(g, side)
  local heroMod = require("warlords.hero")

  -- the computer always hires an offered hero it can afford (5db9:0919)
  if side.heroOffer and side.gold >= (side.heroOffer.price or 0) then
    heroMod.recruit(g, side, side.heroOffer)
    side.heroOffer = nil
  end

  ai.phaseDiplomacy(g, side)
  ai.phaseEvaluate(g, side)
  ai.phaseHeroes(g, side)
  ai.phaseOrders(g, side)
  ai.phaseMove(g, side)
  ai.phaseExplore(g, side)
  ai.phaseNeutral(g, side)
  ai.phaseGarrisons(g, side)
  ai.phaseProduction(g, side)
end

return ai
