-- The computer players' assault groups: a rally city where a strike force
-- gathers, up to four member cities building for it, and six enemy cities
-- to take. Picking whom to attack is here too.
-- docs/re/ai.md > Assault groups; addresses are Ghidra's.

local core = require("warlords.ai.core")

local groups = {}

-- roles a rally city may be picked from (DS:0940, DS:094c)
local RALLY_ROLES = { [5] = true, [8] = true, [6] = true, [4] = true, [14] = true }

--- Integer division truncating toward zero, as the original's does.
local function div(a, b)
  local q = a / b
  return q < 0 and math.ceil(q) or math.floor(q)
end

local function own(g, side)
  return require("warlords.ai.cities").own(g, side)
end

local function city(g, i) return i and g.map.cities[i + 1] or nil end

--- Is a city free to serve a new group (5f19:0a7a): not an active group's
--- rally city, and quiet -- role 8 or 5, or with `members` also 6.
function groups.free(g, side, c, members)
  local d = core.data(g, side)
  for gi = d.maxGroups, 1, -1 do
    local grp = d.groups[gi]
    if grp.active ~= 0 and grp.rally == c.index then return false end
  end
  local r = core.role(d, c)
  if r == core.STOP or r == core.BUILDING then return true end
  return members and r == core.MEMBER or false
end

--- Is a city an active group's rally city (5f19:07f8)?
function groups.isRally(g, side, c)
  local d = core.data(g, side)
  for gi = d.maxGroups, 1, -1 do
    local grp = d.groups[gi]
    if grp.active ~= 0 and grp.rally == c.index then return true end
  end
  return false
end

--- Cancel a group (563e:066d): its rally and member cities go back to role 8.
function groups.cancel(g, side, gi)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  local r = city(g, grp.rally)
  if r then core.setRole(d, r, core.STOP) end
  for k = 4, 1, -1 do
    local m = city(g, grp.members[k])
    if m then core.setRole(d, m, core.STOP) end
  end
  d.groups[gi] = core.emptyGroup()
end

------------------------------------------------------------ the spoils

--- The gold sacking a city would bring: every production type but the
--- cheapest, at half its price (city_sack_value).
local function sackValue(g, c)
  local v = 0
  for i = 2, #c.slots do v = v + math.floor(math.abs(g.types.byId[c.slots[i].type].price) / 2) end
  return v
end
groups.sackValue = sackValue

--- Pillage a city (563e:1a2a).
function groups.pillage(g, side, c)
  require("warlords.game").pillage(g, side, c)
  local ai = require("warlords.ai")
  if ai.onSpoils then ai.onSpoils(g, side, c, "pillaged") end
end

--- Sack a city (563e:199c) -- pillage it when there is nothing to sack.
function groups.sack(g, side, c)
  if sackValue(g, c) == 0 then return groups.pillage(g, side, c) end
  require("warlords.game").sack(g, side, c)
  local ai = require("warlords.ai")
  if ai.onSpoils then ai.onSpoils(g, side, c, "sacked") end
end

--- Raze a city (auto_str_is_being_razed, 563e:1864). Not one in the side's
--- heartland -- three of its own cities within 45 -- unless `force`, and a
--- city worth 400 in sacking is sacked instead. After razing, the stack
--- picks its next city (623c:1851). Returns that city, or nil.
function groups.raze(g, side, c, force, sel)
  if not force then
    local nb, nd = core.neighbours(g, c)
    local n = 0
    for j = #nb, 1, -1 do
      if nb[j].ownerIndex == side.index and nd[j] < 45 then n = n + 1 end
    end
    if n > 2 then return nil end
  end
  if sackValue(g, c) >= 400 then
    groups.sack(g, side, c)
    return nil
  end
  local game = require("warlords.game")
  game.raze(g, side, c, sel and sel.armies or {})
  local ai = require("warlords.ai")
  if ai.onSpoils then ai.onSpoils(g, side, c, "razed") end
  if not sel or #sel.armies == 0 then return nil end
  local flood = core.flood(g, side.index, sel.leader.x, sel.leader.y, 15, sel)
  return core.bestCity(g, sel, flood, true)
end

--- Early vengeance (5e97:0000): a card that has it razes or sacks a human's
--- city taken in the first ten turns -- razes it when it is worth less than
--- 200 in sacking.
function groups.earlyVengeance(g, me, c, was)
  local side = g.map.sides[me + 1]
  local d = core.data(g, side)
  if side.computer and was and was ~= core.NEUTRAL and not core.isComputer(g, was)
     and d.early ~= 0 and g.turn < 10 and d.questCity ~= c.index then
    if sackValue(g, c) < 200 then groups.raze(g, side, c, false) else groups.sack(g, side, c) end
    return true
  end
  return false
end

--- What the group will do to the cities it takes (5f19:06a0): raze, sack or
--- pillage, rolled once on 1d1000 against the card's chances.
local function rollSpoils(g, side, grp)
  local d = core.data(g, side)
  local bias = d.own * d.perCity
  if not side.computer then bias = bias + d.bonusHuman
  elseif side.level == 2 then bias = bias + d.bonusWarlord
  elseif side.level == 1 then bias = bias + d.bonusLord
  elseif side.level == 0 then bias = bias + d.bonusKnight end
  grp.flags = 0
  if d.raze ~= 0 and g.rng:dice(1, 1000, 0) < d.raze + bias then
    grp.flags = core.GF_RAZE
    return
  end
  if side.gold < 100 then bias = bias + d.poor end
  if d.sack ~= 0 and g.rng:dice(1, 1000, 0) < d.sack + bias then
    grp.flags = core.GF_SACK
    return
  end
  if d.pillage ~= 0 and g.rng:dice(1, 1000, 0) < d.pillage + bias then
    grp.flags = core.GF_PILLAGE
  end
end

------------------------------------------------------------ preparing

--- Look over what the side's cities can build for the groups (59bf:01b3):
--- the weakest of the eight strongest builds (at most 4), the cities that
--- build fliers, movers of 16 and movers of 12; then each group's wanted
--- size and kind: group 1 plain (8, or 12 with eight fast builders), group
--- 2 movers of 12, group 3 movers of 16, group 4 fliers, when there are four
--- such cities.
function groups.prepare(g, side)
  local d = core.data(g, side)
  local cities = require("warlords.ai.cities")
  local active = 0
  for gi = d.maxGroups, 1, -1 do if d.groups[gi].active ~= 0 then active = active + 1 end end
  d.minStrength, d.flyCities, d.strongCities, d.fastCities = 0, 0, 0, 0
  local top = {}
  for _, c in ipairs(own(g, side)) do
    core.clearCflag(d, c, core.CF_MOVE12)
    core.clearCflag(d, c, core.CF_FLYGROUP)
    core.clearCflag(d, c, core.CF_MOVE16)
    if core.role(d, c) ~= core.RALLY then
      local good, slot = cities.buildsWell(g, side, c)
      if good then
        local s = slot.strength + (g.types.byId[slot.type].siege and 2 or 0)
        top[#top + 1] = s
      end
    end
  end
  table.sort(top, function(p, q) return p > q end)
  local least = 100
  for i = 1, math.min(8, #top) do if top[i] < least then least = top[i] end end
  d.minStrength = math.min(4, least)

  local function strong(c, slot)
    return g.types.byId[slot.type].siege or d.minStrength <= slot.strength
  end

  if active == 4 then
    for _, c in ipairs(own(g, side)) do
      if groups.free(g, side, c, true) and core.cflag(d, c, core.CF_FLIER) then
        local ok = false
        for _, slot in ipairs(c.slots) do
          if g.types.byId[slot.type].flies and slot.strength > 4 then ok = true end
        end
        if ok then
          d.flyCities = d.flyCities + 1
          core.setCflag(d, c, core.CF_FLYGROUP)
        end
      end
    end
  end
  local flyEnough = d.flyCities >= 4
  local left = flyEnough and d.flyCities - 4 or d.flyCities
  if active > 2 then
    for _, c in ipairs(own(g, side)) do
      if groups.free(g, side, c, true) then
        local skip = false
        if core.cflag(d, c, core.CF_FLYGROUP) then
          if left == 0 then skip = true
          else core.clearCflag(d, c, core.CF_FLYGROUP); left = left - 1 end
        end
        if not skip then
          local ok = false
          for _, slot in ipairs(c.slots) do
            if strong(c, slot) and slot.move > 15 then ok = true end
          end
          if ok then
            d.strongCities = d.strongCities + 1
            core.setCflag(d, c, core.CF_MOVE16)
          end
        end
      end
    end
  end
  local manyStrong = d.strongCities > 7
  local fastBuilders = 0
  local strongEnough = d.strongCities >= 4
  left = strongEnough and d.strongCities - 4 or d.strongCities
  if active > 1 then
    for _, c in ipairs(own(g, side)) do
      if groups.free(g, side, c, true) and not core.cflag(d, c, core.CF_FLYGROUP) then
        local skip = false
        if core.cflag(d, c, core.CF_MOVE16) then
          if left == 0 then skip = true
          else core.clearCflag(d, c, core.CF_MOVE16); left = left - 1 end
        end
        if not skip then
          local ok = false
          for _, slot in ipairs(c.slots) do
            if strong(c, slot) and slot.move > 11 then
              fastBuilders = fastBuilders + 1
              if not manyStrong or slot.move > 15 then ok = true end
            end
          end
          if ok then
            d.fastCities = d.fastCities + 1
            core.setCflag(d, c, core.CF_MOVE12)
          end
        end
      end
    end
  end
  local fastEnough = d.fastCities >= 4
  left = fastEnough and d.fastCities - 4 or d.fastCities
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if c.ownerIndex == side.index and not core.cflag(d, c, core.CF_FLYGROUP)
       and not core.cflag(d, c, core.CF_MOVE16) and core.cflag(d, c, core.CF_MOVE12) and left ~= 0 then
      core.clearCflag(d, c, core.CF_MOVE12)
      left = left - 1
    end
  end
  for gi = d.maxGroups, 1, -1 do
    local grp = d.groups[gi]
    if grp.active ~= 0 and gi <= 4 then
      grp.size = 8
      if gi == 1 then
        grp.size = fastBuilders < 8 and 8 or 12
      elseif gi == 2 then
        grp.flags = core.clear(grp.flags, core.GF_MOVE12)
        if fastEnough then grp.flags = core.set(grp.flags, core.GF_MOVE12); grp.size = 12 end
      elseif gi == 3 then
        grp.flags = core.clear(grp.flags, core.GF_MOVE16)
        if strongEnough then grp.flags = core.set(grp.flags, core.GF_MOVE16); grp.size = 16 end
      elseif gi == 4 then
        grp.flags = core.clear(grp.flags, core.GF_FLY)
        if flyEnough then grp.flags = core.set(grp.flags, core.GF_FLY); grp.size = 12 end
      end
    end
  end
end

------------------------------------------------------------ a group's turn

--- Drop staged stacks that are gone or have left the group (563e:1251).
local function cleanStaged(g, side, gi)
  local grp = core.data(g, side).groups[gi]
  for s = 1, 4 do
    local a = grp.staged[s]
    if a and not (core.alive(g, a) and a.owner == side.index and a.aiGroup == gi) then
      grp.staged[s] = nil
    end
  end
end

--- Re-check the plan (563e:041c): targets that changed hands are dropped,
--- and the rally city's neighbours held by the target side and seen are
--- added -- in an empty place, or over the farthest if nearer. Returns the
--- number of targets.
local function checkPlan(g, side, gi)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  local n = 0
  for k = 1, 6 do
    local t = city(g, grp.cities[k])
    if t then
      if d.questCity == t.index or core.owner(t) ~= grp.target then
        grp.cities[k] = nil
      else
        n = n + 1
      end
    end
  end
  local rally = city(g, grp.rally)
  if not rally then return n end
  local nb, nd = core.neighbours(g, rally)
  for j = #nb, 1, -1 do
    local m = nb[j]
    if core.owner(m) == grp.target and not core.cflag(d, m, core.CF_UNSEEN) then
      local have = false
      for k = 1, 6 do if grp.cities[k] == m.index then have = true end end
      if not have then
        n = n + 1
        local slot, worst
        for k = 6, 1, -1 do
          if grp.cities[k] == nil then slot, worst = k, -1 break end
          if (worst or 0) < (grp.dist[k] or 0) then slot, worst = k, grp.dist[k] end
        end
        if slot and (worst == -1 or nd[j] < worst) then
          grp.cities[slot], grp.dist[slot] = m.index, nd[j]
        end
      end
    end
  end
  return n
end

--- With one target left, add its neighbours of the target side, and move
--- the rally to one of the side's own cities next to it (563e:1579).
local function adjustRally(g, side, gi)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  local only, count = nil, 0
  for k = 1, 6 do if grp.cities[k] then only, count = grp.cities[k], count + 1 end end
  if count ~= 1 then return end
  local nb = core.neighbours(g, city(g, only))
  local next, rallyNear
  for j = 1, #nb do
    local m = nb[j]
    if not core.cflag(d, m, core.CF_UNSEEN) then
      if core.owner(m) == grp.target then
        for k = 1, 6 do
          if grp.cities[k] == nil then grp.cities[k] = m.index break end
        end
      elseif m.ownerIndex == side.index then
        if m.index == grp.rally then rallyNear = true end
        if not groups.isRally(g, side, m) and not next then next = m end
      end
    end
  end
  if not rallyNear and next then grp.rally = next.index end
end

--- Move the rally to the side's nearest neighbouring city that is no other
--- group's rally (563e:02ed); cancel the group when there is none.
local function relocate(g, side, gi)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  local r = city(g, grp.rally)
  local nb = core.neighbours(g, r)
  local best, bestD = nil, 1000
  for j = #nb, 1, -1 do
    local m = nb[j]
    if m.ownerIndex == side.index and not groups.isRally(g, side, m) then
      local dd = core.dist(m.x, m.y, r.x, r.y)
      if dd < bestD then best, bestD = m, dd end
    end
  end
  if not best then
    groups.cancel(g, side, gi)
    return false
  end
  grp.rally = best.index
  return true
end

--- A city the group has taken (563e:134c): look it over, drop it from the
--- targets and keep it among the group's cities; the rally moves to it when
--- it lies nearer the enemy. With no enemy city about either, the group is
--- done.
local function takeCity(g, side, gi, c)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  if grp.rally == c.index then return false end
  require("warlords.ai.cities").garrison(g, side, c, true)
  for k = 1, 6 do
    if grp.cities[k] == c.index then grp.cities[k] = nil break end
  end
  local function near(x)
    local nb, nd = core.neighbours(g, x)
    local n = 0
    for j = 1, #nb do
      if core.owner(nb[j]) == grp.target then
        if nd[j] < 10 then n = n + 1 end
        if nd[j] < 20 then n = n + 1 end
        if nd[j] < 30 then n = n + 1 end
        if nd[j] < 50 then n = n + 1 end
      end
    end
    return n
  end
  local here = near(c)
  local rally = city(g, grp.rally)
  local there = rally and near(rally) or 0
  local pick = (there < here) and c or rally
  if pick then
    local have = false
    for k = 1, 6 do if grp.taken[k] == pick.index then have = true end end
    if not have then
      for k = 1, 6 do
        if grp.taken[k] == nil then grp.taken[k] = pick.index break end
      end
    end
  end
  if here == 0 and there == 0 then
    groups.cancel(g, side, gi)
    return false
  end
  grp.rally = pick.index
  core.setRole(d, c, core.RALLY)
  groups.followUp(g, side, gi)
  return true
end

--- Send the group's stack at a city and deal with what it takes
-- (563e:0f1b): a stack with no way there is disbanded; a city taken is
-- razed, sacked or pillaged as the group rolled -- not the side's own
-- capital, and razing only with two targets left -- and kept. A razed city
-- sends the stack on to the next. Returns 1 when something was taken, 3
-- otherwise.
function groups.march(g, side, gi, list, c)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  local lead = list[1]
  if lead and core.alive(g, lead) and not core.standing(c) then
    local best, bestD = nil, 1000
    for k = 1, 6 do
      local t = city(g, grp.cities[k])
      if t then
        local dd = core.dist(lead.x, lead.y, t.x, t.y)
        if dd < bestD and core.standing(t) then best, bestD = t, dd end
      end
    end
    if best then c = best end
  end
  while true do
    local n = 0
    for k = 1, 6 do if grp.cities[k] then n = n + 1 end end
    local keep = {}
    for _, a in ipairs(list) do
      if core.alive(g, a) and a.owner == side.index then keep[#keep + 1] = a end
    end
    list = keep
    local was = core.owner(c)
    local sel = core.order(g, list, core.ORDER_CITY, c.index, 0)
    if not sel then cleanStaged(g, side, gi) return 3 end
    local r = core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y)
    if r == 1 then
      require("warlords.game").disband(g, side, sel.armies)
      cleanStaged(g, side, gi)
      return 3
    end
    cleanStaged(g, side, gi)
    if c.ownerIndex ~= side.index then return 3 end
    if was == side.index then return 3 end
    local capital = side.capital == c
    if not capital and n >= 2 and core.has(grp.flags, core.GF_RAZE) then
      local nextCity = groups.raze(g, side, c, false, sel)
      if not nextCity then return 1 end
      c = nextCity
      list = sel.armies
    else
      if not capital and core.has(grp.flags, core.GF_SACK) then
        groups.sack(g, side, c)
      elseif not capital and core.has(grp.flags, core.GF_PILLAGE) then
        groups.pillage(g, side, c)
      end
      takeCity(g, side, gi, c)
      return 1
    end
  end
end

--- Look for a stack of the target side worth hitting near a staged stack
-- (563e:0c32): on open ground it can reach this turn, seen, with three
-- armies and 11 strength or a hero, which it beats more than 75% of the
-- time. The stack attacks it, then keeps its order. Returns 2 if it went.
local function hitArmies(g, side, gi, list, target, range, flood, sel)
  local lead = list[1]
  if not lead or lead.atSea then return 0 end
  local game = require("warlords.game")
  local move = require("warlords.move")
  local bx, by, bestOdds = nil, nil, 0
  for x = lead.x - range, lead.x + range - 1 do
    for y = lead.y - range, lead.y + range - 1 do
      if x >= 0 and y >= 0 and x < g.map.width and y < g.map.height then
        local here = game.armiesAt(g, x, y)
        local t = core.terrain(g, x, y)
        if here[1] and here[1].owner == target and t ~= move.CITY
           and t ~= move.SHORE and t ~= move.WATER
           and core.floodAt(flood, x, y) <= sel.minMoves - 1
           and game.seen(g, side.index, x, y) then
          local count, strength, heroes = #here, 0, 0
          for _, a in ipairs(here) do
            strength = strength + (a.strength or 0)
            if core.isHero(a) then heroes = heroes + 1 end
          end
          local o = core.odds(g, sel, x, y)
          if o > 75 and ((count > 2 and strength > 10) or heroes ~= 0) and bestOdds < o then
            bx, by, bestOdds = x, y, o
          end
        end
      end
    end
  end
  if not bx then return 0 end
  local dest = lead.aiOrder == core.ORDER_CITY and lead.aiDest or nil
  local s = core.order(g, list, core.ORDER_ROAM, 0, 0x20)
  if s then
    s.leader.target = { x = bx, y = by }
    core.moveTo(g, s, bx, by)
    for _, a in ipairs(list) do
      if core.alive(g, a) and a.owner == side.index and dest then
        a.aiOrder, a.aiDest = core.ORDER_CITY, dest
      end
    end
  end
  return 2
end

--- A staged stack's step (563e:0b49): strike an enemy stack in reach, and
--- then the nearest city of the target side within its move that it beats
--- three times in four.
local function stagedStep(g, side, gi, list, target, flood, sel)
  local nearest, nd = nil, 1000
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if c.ownerIndex == target then
      local dd = core.cityDistance(g, flood, c)
      if dd < 50 and dd < sel.minMoves then
        local o = core.odds(g, sel, c.x, c.y)
        if o > 75 and dd < nd then nearest, nd = c, dd end
      end
    end
  end
  local r = hitArmies(g, side, gi, list, target, 15, flood, sel)
  if r ~= 0 and nearest then r = groups.march(g, side, gi, list, nearest) end
  return r
end

--- Move the group's staged stacks on (563e:0996): each hits what it can,
--- or carries on to the city it was sent at.
local function gather(g, side, gi)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  for s = 4, 1, -1 do
    local again = true
    while again do
      again = false
      cleanStaged(g, side, gi)
      local a = grp.staged[s]
      if not a then break end
      local list = core.collectOrdered(g, side.index, a.x, a.y, a.aiGroup, a.aiOrder, 0)
      if #list == 0 then break end
      local sel = core.select(g, list)
      local flood = core.flood(g, side.index, a.x, a.y, 15, sel)
      local was, armies = {}, #g.armies
      for i, b in ipairs(list) do was[i] = { b.x, b.y, b.moves } end
      local r = stagedStep(g, side, gi, list, grp.target, flood, sel)
      if r == 0 then
        local last = list[#list]
        if last and core.alive(g, last) and last.aiOrder == core.ORDER_CITY and city(g, last.aiDest) then
          groups.march(g, side, gi, list, city(g, last.aiDest))
        end
      elseif r == 2 then
        -- The original goes round again for as long as a stack is found to
        -- strike. One that cannot take a step would be found every time --
        -- until the odds' dice fall short, which for a strong stack is
        -- never -- so a pass that changed nothing ends it here.
        again = #g.armies ~= armies
        for i, b in ipairs(list) do
          if b.x ~= was[i][1] or b.y ~= was[i][2] or b.moves ~= was[i][3] then again = true end
        end
      end
    end
  end
end

--- Strike from the rally city (563e:06e9): the stack on its first tile goes
--- for the target it reaches soonest and beats -- more than 75% of the
--- time, or with eight armies whatever the odds. It becomes a staged stack.
local function strike(g, side, gi)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  local r = city(g, grp.rally)
  if not r then return false end
  local list = core.collect(g, side.index, r.x + 1, r.y, 0)
  local n = #list
  if n == 0 then return false end
  local sel = core.select(g, list)
  local best, bestScore = nil, -1
  for k = 1, 6 do
    local t = city(g, grp.cities[k])
    if t then
      local turns = div(grp.dist[k] or -1, math.max(1, sel.minMoves - 2)) + 1
      local o = core.odds(g, sel, t.x, t.y)
      local score = (turns < 11 and 10 - turns or 0) + o + (grp.bonus[k] or 0) + g.rng:dice(1, 4, 0)
      if turns == 1 then score = score + 100 end
      if (o > 75 or n > 7) and bestScore < score then best, bestScore = t, score end
    end
  end
  if not best then return false end
  -- the stack's lead -- a hero, else the strongest -- is staged (563e:08b2)
  local slot
  for s = 4, 1, -1 do if not grp.staged[s] then slot = s break end end
  if slot then
    local pick, strongest = nil, -1
    for i = #list, 1, -1 do
      local a = list[i]
      if core.isHero(a) then pick = a break end
      if strongest < (a.strength or 0) then pick, strongest = a, a.strength or 0 end
    end
    grp.staged[slot] = pick
  end
  for _, a in ipairs(list) do a.aiGroup = gi end
  groups.march(g, side, gi, list, best)
  return true
end

--- Follow up from the cities the group took (563e:16fd): each one no enemy
--- is close to is looked over. The original then collects the stack whose
--- order is the group's wanted size -- no order is -- so nothing more moves.
function groups.followUp(g, side, gi)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  for k = 1, 6 do
    local t = city(g, grp.taken[k])
    if t and t.index ~= grp.rally and t.ownerIndex == side.index then
      local nb, nd = core.neighbours(g, t)
      local near = 0
      for j = #nb, 1, -1 do
        local o = nb[j].ownerIndex
        if o ~= nil and o ~= side.index and nd[j] < 25 then near = near + 1 end
      end
      if near == 0 then
        require("warlords.ai.cities").garrison(g, side, t, true)
        local list = core.collectOrdered(g, side.index, t.x + 1, t.y, 0, grp.size, 0)
        if #list > 3 then
          local sel = core.order(g, list, core.ORDER_CITY, grp.rally, 0)
          if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
        end
      end
    end
  end
end

--- Cancel the longest-running group (563e:0607).
local function cancelOldest(g, side)
  local d = core.data(g, side)
  local oldest, age = nil, -1
  for gi = d.maxGroups, 1, -1 do
    local grp = d.groups[gi]
    if grp.active ~= 0 and age < grp.active then oldest, age = gi, grp.active end
  end
  if oldest then groups.cancel(g, side, oldest) end
end

--- A group's turn (563e:00ca). With the side's capital in enemy hands and no
--- group going for it, the oldest group is given up instead.
local function runGroup(g, side, gi)
  local d = core.data(g, side)
  local grp = d.groups[gi]
  local cap = side.capital
  if cap and cap.ownerIndex ~= nil and cap.ownerIndex ~= side.index then
    local going = false
    for k = d.maxGroups, 1, -1 do
      local o = d.groups[k]
      if o.active ~= 0 and o.target == cap.ownerIndex then going = true end
    end
    if not going then
      cancelOldest(g, side)
      return false
    end
  end
  if checkPlan(g, side, gi) == 0 then
    groups.cancel(g, side, gi)
    return false
  end
  adjustRally(g, side, gi)
  local shared = false
  for k = d.maxGroups, 1, -1 do
    local o = d.groups[k]
    if o.active ~= 0 and k ~= gi and o.rally == grp.rally then shared = true end
  end
  local r = city(g, grp.rally)
  if (not shared and r and r.ownerIndex == side.index) or relocate(g, side, gi) then
    gather(g, side, gi)
    if d.groups[gi].active ~= 0 then
      local cities = require("warlords.ai.cities")
      local rc = city(g, d.groups[gi].rally)
      if rc then cities.garrison(g, side, rc, true) end
      if strike(g, side, gi) then
        rc = city(g, d.groups[gi].rally)
        if rc then cities.garrison(g, side, rc, true) end
      end
      if d.groups[gi].active ~= 0 then
        groups.followUp(g, side, gi)
        return true
      end
    end
  end
  return false
end

--- assault (ai_phase_assault, 563e:0000).
function groups.assault(g, side)
  local d = core.data(g, side)
  if d.turns == 0 then return end
  groups.prepare(g, side)
  for _, c in ipairs(own(g, side)) do
    if core.role(d, c) == core.RALLY then core.setRole(d, c, core.STOP) end
  end
  for gi = 1, d.maxGroups do
    local grp = d.groups[gi]
    if grp.active ~= 0 then
      local r = city(g, grp.rally)
      if r then core.setRole(d, r, core.RALLY) end
      if runGroup(g, side, gi) then
        d.groups[gi].active = d.groups[gi].active + 1
      end
    end
  end
end

------------------------------------------------------------ new groups

--- The enemy city with most of its own side's cities round it (5f19:04d2);
--- ties are a coin toss.
local function hub(g, enemy)
  local best, most = nil, 0
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if c.ownerIndex == enemy then
      local n = 1
      local nb = core.neighbours(g, c)
      for j = #nb, 1, -1 do if nb[j].ownerIndex == enemy then n = n + 1 end end
      if n > most or (n == most and g.rng:dice(1, 2, -1) ~= 0) then best, most = c, n end
    end
  end
  return best
end

--- Distances over the neighbour graph from a city (5f19:058a).
local function spread(g, from)
  local reached, dist, done = { [from.index] = true }, { [from.index] = 0 }, {}
  local changed = true
  while changed do
    changed = false
    for i = #g.map.cities, 1, -1 do
      local c = g.map.cities[i]
      if reached[c.index] and not done[c.index] then
        changed = true
        done[c.index] = true
        local nb, nd = core.neighbours(g, c)
        for j = #nb, 1, -1 do
          local m = nb[j]
          local nd2 = dist[c.index] + nd[j]
          if not reached[m.index] then
            reached[m.index], dist[m.index] = true, nd2
          elseif nd2 < dist[m.index] then
            dist[m.index] = nd2
          end
        end
      end
    end
  end
  return reached, dist
end

--- Plan a group against a side (5f19:01bb): its hub city; the side's own
--- city nearest it over the neighbour graph, of a quiet role, becomes the
--- rally; the six enemy cities nearest the rally are the targets. When no
--- city of the side connects, the nearest on the map goes for the hub alone.
local function plan(g, side, grp, enemy)
  local d = core.data(g, side)
  grp.cities, grp.dist = {}, {}
  local h = hub(g, enemy)
  if not h then return nil end
  local reached, dist = spread(g, h)
  local rally, best = nil, 1e9
  for _, c in ipairs(own(g, side)) do
    if RALLY_ROLES[core.role(d, c)] and reached[c.index] and dist[c.index] < best then
      rally, best = c, dist[c.index]
    end
  end
  if not rally then
    if not core.cflag(d, h, core.CF_UNSEEN) then
      local bestD = 1e9
      for _, c in ipairs(own(g, side)) do
        if RALLY_ROLES[core.role(d, c)] then
          local dd = core.dist(c.x, c.y, h.x, h.y)
          if dd < bestD then rally, bestD = c, dd end
        end
      end
      if rally then grp.cities[1], grp.dist[1] = h.index, 100 end
    end
    return rally
  end
  reached, dist = spread(g, rally)
  local n = 0
  for k = 1, 6 do
    local pick, pd = nil, 1e9
    for i = #g.map.cities, 1, -1 do
      local c = g.map.cities[i]
      if c.ownerIndex == enemy and not core.cflag(d, c, core.CF_UNSEEN)
         and reached[c.index] and dist[c.index] < pd then
        pick, pd = c, dist[c.index]
      end
    end
    if not pick then break end
    grp.cities[k], grp.dist[k] = pick.index, pd
    reached[pick.index] = false
    n = n + 1
  end
  if n == 0 and not core.cflag(d, h, core.CF_UNSEEN) then
    grp.cities[1], grp.dist[1] = h.index, 100
    n = 1
  end
  if n == 0 then return nil end
  return rally
end

--- Which side to attack (ai_pick_enemy, 5f19:0e04), or nil. Each side
--- scores 1d10, +20 for holding our capital, +15 when we hold its, +4 a city
--- of its we hold, its record against us, the card's constant, a
--- difference in reputation /8 and in size /4; then sides are ruled out:
--- ones we have not fought before turn 8 (4 with Quick Start), ones we see
--- no city of, ones already a quarter-over-targeted, ones at peace with us
--- while we propose war elsewhere, and -- I am the Greatest -- fellow
--- computers. Whoever holds our capital is the pick unless a group is on it.
function groups.pickEnemy(g, side)
  local d = core.data(g, side)
  local me = side.index
  local cap = side.capital
  local capOwner = cap and core.owner(cap) or core.NEUTRAL
  local held, score, count, capHeld, fromThem, targeted = {}, {}, {}, {}, {}, {}
  local rnd = {}
  for s = 0, 7 do
    held[s], score[s], count[s], capHeld[s], fromThem[s], targeted[s] = 0, 0, 0, 0, 0, 0
    rnd[s] = g.rng:dice(1, 10, 0)
  end
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    local o = c.ownerIndex
    if o ~= nil then
      count[o] = count[o] + 1
      if not core.cflag(d, c, core.CF_UNSEEN) then held[o] = held[o] + 1 end
      if o == me and c.claim ~= nil and c.claim ~= core.NEUTRAL then
        fromThem[c.claim] = fromThem[c.claim] + 1
      end
    end
  end
  local nGroups = 0
  for gi = d.maxGroups, 1, -1 do
    local grp = d.groups[gi]
    if grp.active ~= 0 and grp.target then
      targeted[grp.target] = targeted[grp.target] + 1
      nGroups = nGroups + 1
    end
  end
  if capOwner ~= core.NEUTRAL and capOwner ~= me then score[capOwner] = score[capOwner] + 20 end
  local others = 0
  for s = 7, 0, -1 do
    if count[s] ~= 0 then
      if s ~= me then others = others + 1 end
      local theirs = g.map.sides[s + 1].capital
      if theirs and theirs.ownerIndex == me then capHeld[s] = 1 end
    end
  end
  local constant
  if not side.computer then constant = d.dieHuman
  elseif side.level == 2 then constant = d.dieWarlord
  elseif side.level == 1 then constant = d.dieLord
  else constant = d.dieKnight end
  for s = 7, 0, -1 do
    local o = g.map.sides[s + 1]
    score[s] = score[s] + rnd[s] + capHeld[s] * 15 + fromThem[s] * 4
             + d.heroesKilled[s] * 4 + d.armiesKilled[s] + d.battles[s]
             + d.lost[s] * 2 + d.cityBattles[s] * 2 + d.citiesLost[s] * 2 + (constant or 0)
    score[s] = score[s] + math.floor(math.abs((side.diploScore or 0) - (o.diploScore or 0)) / 8)
    score[s] = score[s] + math.floor(math.abs(count[me] - count[s]) / 4)
  end
  local proposingWar = false
  for s = 7, 0, -1 do
    if core.inPlay(g, s) and s ~= me and core.proposal(g, me, s) == 2 then proposingWar = true end
  end
  if g.greatest and side.computer then
    local fights = groups.fightingHumans(g)
    for s = 7, 0, -1 do
      if core.inPlay(g, s) and core.isComputer(g, s) and (fights[me] or fights[s]) then score[s] = 0 end
    end
  end
  local early = g.map.options.quickStart ~= 0 and 4 or 8
  local diplo = require("warlords.ai.diplomacy")
  for s = 7, 0, -1 do
    local fought = d.battles[s] + d.cityBattles[s]
    if diplo.spared(g, side, s) then score[s] = 0 end
    if proposingWar and core.proposal(g, me, s) == 0 then score[s] = 0 end
    if nGroups == 0 and held[s] == 0 then score[s] = 0 end
    if g.turn < early and fought == 0 then score[s] = 0 end
    if others > 1 and math.floor(count[s] / 4) < targeted[s] then score[s] = 0 end
  end
  score[me] = 0
  local pick, top = nil, 0
  for s = 7, 0, -1 do
    if count[s] ~= 0 and top < score[s] then pick, top = s, score[s] end
  end
  if capOwner ~= core.NEUTRAL and capOwner ~= me then
    -- 5f19:1393 compares the capital's holder with the groups-per-side
    -- counts, not with the groups' targets: kept as it is
    local found = false
    for gi = d.maxGroups - 1, 0, -1 do
      if capOwner == targeted[gi] then found = true end
    end
    if not found then pick = capOwner end
  end
  if pick and held[pick] == 0 then pick = nil end
  return pick
end

--- For I am the Greatest: which computer sides are at war with a human.
function groups.fightingHumans(g)
  local out = {}
  for s = 0, 7 do
    if core.inPlay(g, s) and core.isComputer(g, s) then
      for h = 0, 7 do
        if core.inPlay(g, h) and not core.isComputer(g, h) and core.state(g, s, h) == 2 then
          out[s] = true
        end
      end
    end
  end
  return out
end

--- assault XX (ai_phase_assault_xx, 5f19:0000): start a new group when one
--- is free and there are quiet cities to build for it -- three of them once
--- a group is running -- against the side ai_pick_enemy names, and propose
--- war on it.
function groups.assaultXX(g, side)
  local d = core.data(g, side)
  if d.turns == 0 then return end
  local quiet = 0
  for _, c in ipairs(own(g, side)) do
    local r = core.role(d, c)
    if r == core.BUILDING or r == core.STOP then quiet = quiet + 1 end
  end
  local active = 0
  for gi = 1, core.MAX_GROUPS do if d.groups[gi].active ~= 0 then active = active + 1 end end
  if not (active < d.maxGroups and quiet ~= 0 and (active == 0 or quiet > 2)) then return end
  local slot
  for gi = d.maxGroups, 1, -1 do if d.groups[gi].active == 0 then slot = gi end end
  if not slot then return end
  d.groups[slot] = core.emptyGroup()
  local enemy = groups.pickEnemy(g, side)
  if enemy == nil then return end
  local grp = d.groups[slot]
  local rally = plan(g, side, grp, enemy)
  if not rally then
    d.groups[slot] = core.emptyGroup()
    return
  end
  grp.active, grp.target, grp.rally = 1, enemy, rally.index
  core.setRole(d, rally, core.RALLY)
  rollSpoils(g, side, grp)
  core.propose(g, side.index, enemy, 2)
  -- the rally city is no group's member any more (5f19:083e)
  for gi = d.maxGroups, 1, -1 do
    local o = d.groups[gi]
    if o.active ~= 0 then
      for k = 1, 4 do
        if o.members[k] == rally.index then o.members[k] = nil; rally.vectorTo = nil end
      end
    end
  end
  -- each target's bonus: its own side's cities round it (5f19:09ca)
  for k = 1, 6 do
    local t = city(g, grp.cities[k])
    if t then
      local n = 0
      for _, m in ipairs(core.neighbours(g, t)) do
        if m.ownerIndex == enemy then n = n + 1 end
      end
      grp.bonus[k] = n
    end
  end
  groups.prepare(g, side)
end

return groups
