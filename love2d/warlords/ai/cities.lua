-- The computer players' city phases: evaluate, clean city, neutral,
-- quick attack, update hide, rebuilding, production and vectoring.
-- docs/re/ai.md; addresses are Ghidra's.

local armytype = require("warlords.armytype")
local rules    = require("warlords.rules")
local core     = require("warlords.ai.core")

local cities = {}

-- garrison wanted on the first tile, by how many armies the city holds
-- (DS:0824), 8 from thirteen up
local KEEP = { [0] = 0, 0, 0, 0, 3, 3, 4, 5, 5, 6, 7, 7, 7 }

-- the city's four tiles in the order the garrison fills them (DS:0814/081c)
local TILE_DX = { [0] = 1, 0, 0, 1 }
local TILE_DY = { [0] = 0, 0, 1, 1 }

-- the roles that make a city try a quick attack (DS:0932)
local QUICK_ROLES = { [5] = true, [8] = true, [6] = true, [4] = true, [14] = true, [7] = true }

------------------------------------------------------------------ helpers

local function mine(c, side) return c.ownerIndex == side.index end

--- The side's own cities, last first, as the original walks them.
local function own(g, side)
  local out = {}
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if c.ownerIndex == side.index then out[#out + 1] = c end
  end
  return out
end
cities.own = own

--- The neutral cities among a city's neighbours (57ea:01f6): standing, not
--- the quest city, and -- after the first two places and the first found --
--- less than 20 away. Returns a list of { city, dist, slot }.
function cities.neutralNeighbours(g, side, c)
  local d = core.data(g, side)
  local nb, nd = core.neighbours(g, c)
  local out = {}
  for j = 1, #nb do
    local n = nb[j]
    if n.index ~= d.questCity and core.standing(n) and n.ownerIndex == nil
       and (#out == 0 or j <= 2 or nd[j] < 20) then
      out[#out + 1] = { city = n, dist = nd[j], slot = j }
    end
  end
  return out
end

--- The distance from (x, y) to the nearest army of another side -- or of
--- side `of` -- that the side can see (57ea:0a3e). 1000 when there is none.
function cities.nearestArmy(g, side, x, y, of)
  local best = 1000
  for i = #g.armies, 1, -1 do
    local a = g.armies[i]
    if a.owner ~= nil and not a.transit then
      local ok
      if of == nil then ok = a.owner ~= side.index else ok = a.owner == of end
      if ok and core.explored(g, side.index, a.x, a.y) then
        local dd = core.dist(x, y, a.x, a.y)
        if dd < best then best = dd end
      end
    end
  end
  return best
end

--- A slot's strength as the AI weighs it: +2 for a siege engine.
local function slotStrength(g, slot)
  local s = slot.strength
  if g.types.byId[slot.type].siege then s = s + 2 end
  return s
end

--- Can this city build something worth having (623c:1a4d)? Its strongest
--- type must be above 2 once the side has 12 cities, above 1 from 8. Also
--- returns that slot.
function cities.buildsWell(g, side, c)
  local d = core.data(g, side)
  local slot = rules.bestSlot(c.slots, 3, g.types, side.enhanced)
  if not slot then return false end
  local s = slotStrength(g, slot)
  if (d.own < 12 or s > 2) and (d.own < 8 or s > 1) then return true, slot end
  return false, slot
end

--- The slot to buy over (623c:1ae9): the first empty one, else the weakest
--- under 10. nil when all four are full of strong types.
local function slotToReplace(g, c)
  if #c.slots < rules.PRODUCTION_SLOTS then return #c.slots + 1 end
  local best, least = nil, 10
  for i, slot in ipairs(c.slots) do
    local s = slotStrength(g, slot)
    if s < least then best, least = i, s end
  end
  return best
end

--- Is a city part way through building something (623c:0fd5)?
function cities.building(c)
  if not c.producing then return false end
  local slot = c.slots[c.producing]
  return slot ~= nil and c.countdown ~= 0 and c.countdown ~= slot.time
end

--- Set what a city builds (set_city_production, 623c:0e5a): refused after
--- turn 5 when the side has less than the type's cost + 30 in gold.
local function setProduction(g, side, c, slot)
  for i, s in ipairs(c.slots) do
    if s == slot then
      if side.gold < (s.cost or 0) + 30 and g.turn > 5 then return false end
      require("warlords.game").setProduction(g, c, i)
      return true
    end
  end
  return false
end

--- Build for a purpose (best_production_for, 623c:103d).
local function produceFor(g, side, c, purpose)
  local slot = rules.bestSlot(c.slots, purpose, g.types, side.enhanced)
  if slot then setProduction(g, side, c, slot) end
end

--- Vector a city's production (623c:0f10): only while it is building.
function cities.vector(g, c, dest)
  if not cities.building(c) then
    c.vectorTo = nil
  else
    c.vectorTo = dest and dest.index or nil
  end
end

--- Buy a type into a city (ai_buy_production_type, 5db9:0bd1), if the side
--- has 30 over its price. The city stops building and is to fill up.
local function buyType(g, side, c, typeId)
  local t = g.types.byId[typeId]
  if not t then return false end
  if not (side.gold > t.price + 30) then return false end
  local n = slotToReplace(g, c)
  if not n then return false end
  local game = require("warlords.game")
  game.buyProduction(g, side, c, n, typeId)
  game.sortProduction(c)
  local d = core.data(g, side)
  d.bought = d.bought + 1
  c.producing, c.countdown, c.vectorTo = nil, 0, nil
  core.setRole(d, c, core.BUILDING)
  return true
end

--- Buy the first flying type the side can afford into a city (623c:11d5).
local function buyFlier(g, side, c)
  local n = slotToReplace(g, c)
  if not n then return end
  local d = core.data(g, side)
  for id = 0, 28 do
    local t = g.types.byId[id]
    if t and t.flies and (t.bonus[48] or 0) == 0 and t.price + 30 <= side.gold then
      local game = require("warlords.game")
      game.buyProduction(g, side, c, n, id)
      game.sortProduction(c)
      core.setCflag(d, c, core.CF_FLIER)
      c.producing, c.countdown, c.vectorTo = nil, 0, nil
      return
    end
  end
end

------------------------------------------------------------------ evaluate

--- Which cities can build a flier (59bf:09cf).
local function markFliers(g, d)
  for _, c in ipairs(g.map.cities) do
    if core.standing(c) then
      core.clearCflag(d, c, core.CF_FLIER)
      for _, slot in ipairs(c.slots) do
        if g.types.byId[slot.type].flies then core.setCflag(d, c, core.CF_FLIER) end
      end
    end
  end
end

--- Clear "not seen yet" for cities the side has seen round (59bf:0b55,
--- 0b95): any tile from one before to two after the city's corner.
function cities.clearUnseen(g, side)
  if g.map.options.hiddenMap == 0 then return end
  local game = require("warlords.game")
  local d = core.data(g, side)
  for _, c in ipairs(g.map.cities) do
    if core.cflag(d, c, core.CF_UNSEEN) then
      local seen = false
      for x = c.x - 1, c.x + 2 do
        for y = c.y - 1, c.y + 2 do
          if x >= 0 and y >= 0 and x < g.map.width and y < g.map.height
             and game.seen(g, side.index, x, y) then
            seen = true
          end
        end
      end
      if seen then core.clearCflag(d, c, core.CF_UNSEEN) end
    end
  end
end

--- evaluate (ai_phase_evaluate, 59bf:0000): recount the cities, age the
--- side's own, and settle the roles of cities just taken.
function cities.evaluate(g, side, who)
  local d = core.data(g, side)
  markFliers(g, d)
  cities.clearUnseen(g, side)
  d.own, d.enemy, d.neutral, d.unseen = 0, 0, 0, 0
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if core.standing(c) then
      if c.ownerIndex == who then
        d.own = d.own + 1
        d.held[c.index] = (d.held[c.index] or 0) + 1
        if core.role(d, c) == 0 then core.setRole(d, c, core.JUST_TAKEN) end
      else
        if core.cflag(d, c, core.CF_UNSEEN) then d.unseen = d.unseen + 1 end
        core.setRole(d, c, 0)
        d.held[c.index] = 0
        if c.ownerIndex == nil then d.neutral = d.neutral + 1 else d.enemy = d.enemy + 1 end
      end
    else
      core.setRole(d, c, 0)
      d.held[c.index] = 0
    end
  end
  if d.own ~= 0 then
    for i = #g.map.cities, 1, -1 do
      local c = g.map.cities[i]
      if core.role(d, c) == core.STOP and d.unseen ~= 0 and d.explorers < 5
         and core.cflag(d, c, core.CF_FLIER) then
        core.setRole(d, c, core.EXPLORER2)
      end
      if core.role(d, c) == core.JUST_TAKEN then
        core.setRole(d, c, #cities.neutralNeighbours(g, side, c) > 0
                           and core.NEAR_NEUTRAL or core.BUILDING)
      end
    end
    d.turns = d.turns + 1
  end
end

------------------------------------------------------------------ garrisons

--- The garrison a city wants (ai_wanted_garrison, 5ca7:0a3d): 8 when a
--- neighbouring city belongs to a side it is at war with, else 4 with two
--- or more other sides' cities about, 3 with one, 2 with none.
function cities.wanted(g, side, c)
  local nb = core.neighbours(g, c)
  local foreign, war = 0, 0
  for j = #nb, 1, -1 do
    local n = nb[j]
    if n.ownerIndex ~= nil and n.ownerIndex ~= side.index and core.standing(n) then
      foreign = foreign + 1
      if core.state(g, side.index, n.ownerIndex) == 2 then war = war + 1 end
    end
  end
  if war ~= 0 then return 8 end
  if foreign >= 2 then return 4 end
  if foreign == 1 then return 3 end
  return 2
end

--- Put the city's armies in the order they stand in (5ca7:0b5c): the ones to
--- keep on the first tile first. For a rally city those are the assault's
--- pick -- a hero, fliers, the special types. Returns the list and how many
--- to keep.
local function arrange(g, side, c, list)
  local d = core.data(g, side)
  local n = #list
  local keepWanted = KEEP[n] or 8
  local mask, minMax, keep = 0, 0, 0
  local attack = false
  local pool = {}
  for i = 1, n do pool[i] = list[i] end
  if core.role(d, c) == core.RALLY then
    local fliers, heroes = 0, 0
    for i = n, 1, -1 do
      local a = pool[i]
      if core.isHero(a) then heroes = heroes + 1
      elseif core.flies(g, a) and ((a.strength or 0) > 3 or fliers == 0) then fliers = fliers + 1 end
    end
    if fliers ~= 0 and heroes ~= 0 then fliers = fliers + heroes end
    if (fliers > 3 and n < 17) or (fliers > 2 and n < 9) then attack = true end
    for gi = core.MAX_GROUPS, 1, -1 do
      local grp = d.groups[gi]
      if grp.active ~= 0 and grp.rally == c.index then
        if core.has(grp.flags, core.GF_MOVE12) then minMax = 12
        elseif core.has(grp.flags, core.GF_MOVE16) then minMax = 16
        else minMax = grp.size end
        if core.has(grp.flags, core.GF_FLY) then attack = true end
      end
    end
  end
  local k = 3 - math.floor((d.held[c.index] or 0) / 3)
  if k > 0 and k < n and n - keepWanted < k then keepWanted = n - k end

  local out = {}
  while true do
    local bestKeep, bestKeepI, bestOther, bestOtherI = -1, nil, -1, nil
    for i = n, 1, -1 do
      local a = pool[i]
      if a then
        local score = a.strength or 0
        if #out < keepWanted then
          local ab = core.ability(g, a)
          if not core.has(mask, 1) and core.isHero(a) then
            score = score + 1000
          elseif minMax <= (a.maxMoves or 0) then
            if not core.has(mask, 0x10) and core.flies(g, a) then score = score + 900
            elseif not attack then
              if not core.has(mask, 2) and ab == 2 then score = score + 800
              elseif not core.has(mask, 4) and ab == 3 then score = score + 700
              elseif not core.has(mask, 0x80) and core.magical(g, a) then score = score + 600
              elseif not core.has(mask, 8) and ab == 1 then score = score + 500
              elseif not core.has(mask, 0x20) and core.woods(g, a) then score = score + 400
              elseif not core.has(mask, 0x40) and core.hills(g, a) then score = score + 300
              end
            end
          end
        else
          mask = 0xfff
        end
        if core.has(mask, 1) and core.isHero(a) then score = 1 end
        if core.has(mask, 0x80) and core.magical(g, a) then score = 2 end
        if (a.maxMoves or 0) < minMax
           or (attack and not core.flies(g, a) and not core.isHero(a)) then
          if bestOther < score then bestOther, bestOtherI = score, i end
        elseif bestKeep < score then
          bestKeep, bestKeepI = score, i
        end
      end
    end
    if not bestKeepI and not bestOtherI then break end
    if mask ~= 0xfff then
      local a = pool[bestKeepI or bestOtherI]
      local ab = core.ability(g, a)
      if core.isHero(a) then mask = core.set(mask, 1)
      else
        if ab == 2 then mask = core.set(mask, 2) end
        if ab == 3 then mask = core.set(mask, 4) end
        if core.flies(g, a) then mask = core.set(mask, 0x10) end
        if core.magical(g, a) then mask = core.set(mask, 0x80) end
        if ab == 1 then mask = core.set(mask, 8) end
        if core.woods(g, a) then mask = core.set(mask, 0x20) end
        if core.hills(g, a) then mask = core.set(mask, 0x40) end
      end
    end
    if not bestKeepI then
      out[#out + 1] = pool[bestOtherI]; pool[bestOtherI] = nil
    else
      out[#out + 1] = pool[bestKeepI]; pool[bestKeepI] = nil
      keep = keep + 1
    end
  end
  if keepWanted < keep then keep = keepWanted end
  return out, keep
end

local function disband(g, side, a)
  require("warlords.game").disband(g, side, { a })
end

--- Hand items round between a city's heroes (6087:17db, 1a3d): a hero with
--- two or more double-movement, flight or gold items gives one to a hero
--- with none; then battle, command and standard items go from a hero with
--- more to the one with fewest.
local function shareItems(g, d, heroes)
  local function count(h, types)
    local n = 0
    for _, it in ipairs(h.items or {}) do if types[it.type] then n = n + 1 end end
    return n
  end
  local function give(from, to, types)
    for i, it in ipairs(from.items or {}) do
      if types[it.type] then
        table.remove(from.items, i)
        to.items = to.items or {}
        to.items[#to.items + 1] = it
        d.itemsPassed = d.itemsPassed + 1
        return true
      end
    end
    return false
  end
  for _, t in ipairs({ { [6] = true }, { [5] = true }, { [7] = true } }) do
    local moved = true
    while moved do
      moved = false
      for _, h in ipairs(heroes) do
        if count(h, t) > 1 then
          for _, o in ipairs(heroes) do
            if o ~= h and count(o, t) == 0 and count(h, t) > 1 then
              if give(h, o, t) then moved = true end
            end
          end
        end
      end
    end
  end
  -- battle, command and standard items: each hero passes on only what it
  -- started with, so nothing goes back and forth
  local fight = { [1] = true, [2] = true, [8] = true }
  local start, own = {}, {}
  for i, h in ipairs(heroes) do
    start[i] = {}
    for _, it in ipairs(h.items or {}) do
      if fight[it.type] then start[i][#start[i] + 1] = it end
    end
    own[i] = #start[i]
  end
  local moved = true
  while moved do
    moved = false
    for i, h in ipairs(heroes) do
      if #start[i] > 0 then
        local least, to = own[i], nil
        for j in ipairs(heroes) do
          if j ~= i and own[j] < least then least, to = own[j], j end
        end
        if to then
          local it = table.remove(start[i], 1)
          for k, x in ipairs(h.items) do
            if x == it then table.remove(h.items, k) break end
          end
          local o = heroes[to]
          o.items = o.items or {}
          o.items[#o.items + 1] = it
          d.itemsPassed = d.itemsPassed + 1
          own[i], own[to] = own[i] - 1, own[to] + 1
          moved = true
        end
      end
    end
  end
end

--- Look over a city's armies and stand them where they belong
-- (ai_city_garrison_check, 5ca7:023f). `force` does it however little has
-- changed. The role follows the garrison: under two armies 4, under the
-- wanted size 5, else 8. The keepers stand on the first tile, the rest eight
-- to a tile after them -- a rally city's first tile is its strike force.
function cities.garrison(g, side, c, force)
  local d = core.data(g, side)
  local game = require("warlords.game")
  local cx, cy = c.x, c.y
  local last = d.garrison[c.index] or 0
  core.setCflag(d, c, core.CF_CLEANED)
  d.garrison[c.index] = 0
  local list, heroes, ordered, fliers, magic = {}, {}, 0, 0, 0
  local lastHero
  for i = #g.armies, 1, -1 do
    local a = g.armies[i]
    if a and not a.transit and a.x and a.x >= cx and a.x <= cx + 1 and a.y >= cy and a.y <= cy + 1 then
      if a.owner ~= side.index then
        disband(g, g.map.sides[(a.owner or 8) + 1] or side, a)
      else
        d.garrison[c.index] = d.garrison[c.index] + 1
        if a.aiOrder == core.ORDER_CITY and a.aiDest == c.index then core.clearOrder(a) end
        if (a.aiOrder or 0) ~= 0 then ordered = ordered + 1 end
        if core.isHero(a) and #heroes < 8 then heroes[#heroes + 1] = a; lastHero = a end
        if core.flies(g, a) then fliers = fliers + 1 end
        if core.magical(g, a) then magic = magic + 1 end
        if #list < 32 then list[#list + 1] = a else disband(g, side, a) end
      end
    end
  end
  if lastHero then core.pickUp(g, { hero = lastHero, armies = { lastHero } }) end
  if #heroes > 1 then shareItems(g, d, heroes) end
  core.clearCflag(d, c, core.CF_HERO)
  core.clearCflag(d, c, core.CF_MAGIC)
  if #heroes ~= 0 then core.setCflag(d, c, core.CF_HERO) end
  if magic ~= 0 then core.setCflag(d, c, core.CF_MAGIC) end
  if #list > 16 then last, ordered = 0, 0 end

  local role = core.role(d, c)
  if not ((ordered < 3 or role == core.RALLY)
          and (force or #heroes ~= 0 or d.garrison[c.index] ~= last
               or g.rng:dice(1, 6, -1) == 0
               or #game.armiesAt(g, cx + 1, cy + 1) > 0)) then
    return true
  end
  d.keep[c.index] = 0
  if role == core.WEAK or role == core.BUILDING or role == core.STOP then
    local want = cities.wanted(g, side, c)
    if #list < 2 then core.setRole(d, c, core.WEAK)
    elseif #list < want then core.setRole(d, c, core.BUILDING)
    else core.setRole(d, c, core.STOP) end
  end
  local sorted, keepN = arrange(g, side, c, list)
  local n = #sorted
  if not force and side.gold < 300 and (side.income or 0) < (side.upkeepTotal or 0) * 2 then
    if core.role(d, c) ~= core.RALLY and (d.held[c.index] or 0) > 10
       and #heroes + magic + 4 < n
       and cities.nearestArmy(g, side, cx, cy) > 10 then
      for i = #heroes + magic + 4 + 1, n do
        if sorted[i] then disband(g, side, sorted[i]); sorted[i] = false end
      end
    end
  end
  if n > 24 then
    for i = 25, n do
      if sorted[i] then disband(g, side, sorted[i]); sorted[i] = false end
    end
  end
  if (g.map.options.quickStart == 0 or g.turn > 2) and n < 2 then
    core.setRole(d, c, core.WEAK)
  end
  if g.map.options.hiddenMap ~= 0 and core.role(d, c) == core.EXPLORER2 and d.explorers < 5 then
    local go = fliers
    local short = 2 - (n - fliers)
    if short > 0 then go = fliers - short end
    if g.map.options.quickStart ~= 0 and g.turn < 3 then go = 1 end
    for i = 1, n do
      local a = sorted[i]
      if go > 0 and a and core.alive(g, a) and core.flies(g, a) then
        d.explorers = d.explorers + 1
        a.aiExplore = true
        require("warlords.ai.moves").explore(g, side, a, nil)
        sorted[i] = false
        go = go - 1
      end
    end
  end
  local tile, perTile
  local r = core.role(d, c)
  if r == core.NEAR_NEUTRAL or r == core.TAKING_NEUTRAL then
    tile, perTile = 1, 8
  else
    tile = keepN == 0 and 1 or 0
    perTile = keepN == 0 and 8 or keepN
  end
  for i = 1, n do
    local a = sorted[i]
    if a and core.alive(g, a) and tile <= 3 then
      if tile == 0 then d.keep[c.index] = (d.keep[c.index] or 0) + 1 end
      a.x, a.y = cx + TILE_DX[tile], cy + TILE_DY[tile]
      a.atSea = false
      a.target = nil
      a.group = nil
      a.aiGroup = 0
      a.done = nil
      perTile = perTile - 1
      if perTile == 0 then tile, perTile = tile + 1, 8 end
    end
  end
  return true
end

--- clean city (ai_phase_clean_city, 5ca7:01f1): every city not yet looked
--- over this turn.
function cities.clean(g, side)
  local d = core.data(g, side)
  for _, c in ipairs(own(g, side)) do
    if not core.cflag(d, c, core.CF_CLEANED) then cities.garrison(g, side, c, false) end
  end
end

------------------------------------------------------------------ neutral

--- Send stacks from a city at the neutral cities round it, one after
--- another (57ea:06c7): each goes for the neighbour it has the best chance
--- at -- 75% or more -- and the nearest, and a city taken becomes the next
--- starting point unless the side is cautious or an enemy is near.
local function takeNeutrals(g, side, c, count, heading, stack)
  local d = core.data(g, side)
  local neutral = g.map.options.neutralCities ~= 0
  local odds = 100
  local from = c
  local lastFrom, lastPick
  while true do
    local keep = {}
    for _, a in ipairs(stack) do
      if core.alive(g, a) and a.owner == side.index then keep[#keep + 1] = a end
    end
    stack = keep
    if #stack == 0 then return end
    local sel = core.select(g, stack)
    local nb = cities.neutralNeighbours(g, side, from)
    if #nb == 0 then return end
    local best, bestScore = nil, 1000
    for j = #nb, 1, -1 do
      local e = nb[j]
      local n = e.city
      if not core.cflag(d, n, core.CF_UNSEEN) and (heading[n.index] or 0) < 3 then
        if neutral then odds = core.odds(g, sel, n.x, n.y) end
        if odds > 74 then
          local score = g.rng:dice(1, 10, 0) + e.dist + (heading[n.index] or 0) * 10
                      + (e.dist >= 41 and 10 or 0) + (e.dist >= 51 and 30 or 0) + (100 - odds)
          if score < bestScore then best, bestScore = n, score end
        end
      end
    end
    if not best then break end
    heading[best.index] = (heading[best.index] or 0) + count
    lastFrom, lastPick = c, best
    sel = core.order(g, stack, core.ORDER_CITY, best.index, 0x80)
    if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
    if best.ownerIndex ~= side.index then return end
    if d.cautious ~= 0 then return end
    from = best
    if cities.nearestArmy(g, side, best.x, best.y) < 10 then return end
  end
  if g.map.options.hiddenMap ~= 0 and lastFrom == c and lastPick then
    core.order(g, stack, core.ORDER_CITY, lastPick.index, 0x80)
  end
end

--- How many armies a city holds back (57ea:03aa): 2 with an enemy within 5,
--- 1 more within 15, 1 more when cautious, and one for each city lost, up
--- to 2.
local function holdBack(g, side, c)
  local d = core.data(g, side)
  local near = cities.nearestArmy(g, side, c.x, c.y)
  local extra = (near < 5 and 2 or 0) + (near < 15 and 1 or 0) + (d.cautious ~= 0 and 1 or 0)
  local lost = 0
  for i = 0, 7 do lost = lost + d.citiesLost[i] end
  return extra, math.min(2, lost), near
end

--- The idle armies on a city's corner tile, beyond the ones held back, the
--- fastest first.
local function idle(g, side, c, reserve, test)
  local found = {}
  for _, a in ipairs(core.onTile(g, side.index, c.x, c.y)) do
    if (a.aiGroup or 0) == 0 and not a.done and (a.aiOrder or 0) == 0 and (not test or test(a)) then
      if reserve > 0 then reserve = reserve - 1
      elseif #found < 8 then found[#found + 1] = a end
    end
  end
  table.sort(found, function(p, q) return (p.maxMoves or 0) > (q.maxMoves or 0) end)
  return found
end

--- Send a city's idle armies at its neutral neighbours (57ea:03aa), a stack
--- of 8 at a time -- one at a time with Neutral Cities off. Returns true with
--- the map hidden (the city then sends explorers), false otherwise.
local function attackNeutrals(g, side, c, heading)
  local extra, lost = holdBack(g, side, c)
  local list = idle(g, side, c, extra + lost)
  if #list == 0 then return false end
  local per = g.map.options.neutralCities ~= 0 and 8 or 1
  local i = 1
  while true do
    local stack = {}
    while i <= #list and #stack < per do stack[#stack + 1] = list[i]; i = i + 1 end
    if #stack == 0 then break end
    if extra + lost == 0 and g.map.options.hiddenMap ~= 0
       and cities.nearestArmy(g, side, c.x, c.y) < 15 then
      break
    end
    takeNeutrals(g, side, c, #stack, heading, stack)
  end
  return g.map.options.hiddenMap ~= 0
end

--- Send a city's idle armies to explore, one by one (57ea:0b19); only
--- fliers when `fliersOnly`. Heroes stay while an assault group is running.
local function sendExplorers(g, side, c, fliersOnly)
  local d = core.data(g, side)
  local grouping = false
  for i = 1, core.MAX_GROUPS do if d.groups[i].active ~= 0 then grouping = true end end
  local extra, lost = holdBack(g, side, c)
  local list = idle(g, side, c, extra + lost, function(a)
    if core.isHero(a) and grouping then return false end
    if fliersOnly and not core.flies(g, a) then return false end
    return true
  end)
  for _, a in ipairs(list) do
    if extra + lost == 0 and g.map.options.hiddenMap ~= 0
       and cities.nearestArmy(g, side, c.x, c.y) < 15 then
      return
    end
    if core.alive(g, a) then require("warlords.ai.moves").explore(g, side, a, c) end
  end
end

--- The first neutral neighbour fewer than three stacks are heading for
--- (57ea:0935), or nil.
local function openNeutral(g, side, c, heading)
  local nb = cities.neutralNeighbours(g, side, c)
  for j = #nb, 1, -1 do
    local n = nb[j].city
    if (heading[n.index] or 0) <= 2 then return n end
  end
  return nil
end
cities.openNeutral = openNeutral

--- One pass of the neutral phase (57ea:00b5). Returns how many cities acted.
local function neutralPass(g, side, pass, heading)
  local d = core.data(g, side)
  local acted = 0
  for _, c in ipairs(own(g, side)) do
    if c.ownerIndex == side.index and (pass == 0 or core.role(d, c) == core.JUST_TAKEN) then
      if core.role(d, c) == core.JUST_TAKEN then
        cities.garrison(g, side, c, true)
        core.setRole(d, c, core.NEAR_NEUTRAL)
      end
      local act = #cities.neutralNeighbours(g, side, c) > 0
      if not act then
        local r = core.role(d, c)
        if r == core.NEAR_NEUTRAL or r == core.TAKING_NEUTRAL then core.setRole(d, c, core.WEAK) end
        if g.map.options.quickStart == 0 and core.role(d, c) == core.BUILDING and g.turn < 6 then
          act = true
        end
      end
      if act then
        acted = acted + 1
        if not attackNeutrals(g, side, c, heading) then
          core.setRole(d, c, openNeutral(g, side, c, heading) and core.NEAR_NEUTRAL or core.WEAK)
        else
          sendExplorers(g, side, c, g.turn > 10)
          core.setRole(d, c, core.TAKING_NEUTRAL)
        end
      end
    end
  end
  return acted
end

--- neutral (ai_phase_neutral, 57ea:0000): up to ten passes, the first over
--- every city, the rest over the cities the passes before have just taken.
function cities.neutral(g, side)
  local heading = {}
  for _, a in ipairs(core.armies(g, side.index)) do
    if a.aiOrder == core.ORDER_CITY and a.aiDest and a.aiDest < #g.map.cities then
      heading[a.aiDest] = (heading[a.aiDest] or 0) + 1
    end
  end
  for pass = 0, 9 do
    if neutralPass(g, side, pass, heading) == 0 then break end
  end
end

------------------------------------------------------------ quick attack

--- A strong garrison strikes at a weak neighbour (5e97:053f): the stack of
--- armies with 8 moves or more, when no neighbour it cannot beat is close,
--- goes for the neighbour it reaches soonest with 85% odds (65% when broke).
local function quickStrike(g, side, c)
  local d = core.data(g, side)
  local role = core.role(d, c)
  local x, y = c.x, c.y
  if role ~= core.RALLY then x = x + 1 end
  if not (role ~= core.RALLY or (d.garrison[c.index] or 0) > 11) then return end
  local need = (side.gold < 40 and (side.income or 0) < (side.upkeepTotal or 0)) and 65 or 85
  local list = core.collect(g, side.index, x, y, 8)
  local n = #list
  if n == 0 then return end
  if n < 4 and (role == core.MEMBER or role == core.RALLY) then return end
  local sel = core.select(g, list)
  local nb, nd = core.neighbours(g, c)
  local danger, best, bestOdds, bestTurns = false, nil, 0, 100
  for j = #nb, 1, -1 do
    local t = nb[j]
    if not core.cflag(d, t, core.CF_UNSEEN) and t.ownerIndex ~= side.index and core.standing(t)
       and t.index ~= d.questCity
       and (t.ownerIndex == nil or core.state(g, side.index, t.ownerIndex) == 2) then
      local o = core.odds(g, sel, t.x, t.y)
      if o == 0 then
        if (n < 4 and nd[j] < 25) or (n < 6 and nd[j] < 15) then danger = true end
      elseif o >= need then
        local turns = math.floor(nd[j] / math.max(1, sel.minMoves - 2)) + 1
        if turns < bestTurns or (turns == bestTurns and bestOdds < o) then
          best, bestOdds, bestTurns = t, o, turns
        end
      end
    end
  end
  if not danger and best then
    sel = core.order(g, list, core.ORDER_CITY, best.index, 0)
    if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
  end
end

--- quick attack (ai_phase_quick_attack, 5e97:04ba): not for a cautious side.
function cities.quickAttack(g, side)
  local d = core.data(g, side)
  if d.cautious ~= 0 then return end
  for _, c in ipairs(own(g, side)) do
    if c.ownerIndex == side.index and QUICK_ROLES[core.role(d, c)]
       and (d.garrison[c.index] or 0) > 3 then
      quickStrike(g, side, c)
    end
  end
end

------------------------------------------------------------ update hide

--- update hide (ai_phase_update_hide, 59bf:0c1c): a city taking a neutral
--- goes back to "next to a neutral" once one of its neutral neighbours has
--- been seen.
function cities.updateHide(g, side)
  local d = core.data(g, side)
  cities.clearUnseen(g, side)
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if core.role(d, c) == core.TAKING_NEUTRAL then
      for _, e in ipairs(cities.neutralNeighbours(g, side, c)) do
        if not core.cflag(d, e.city, core.CF_UNSEEN) then
          core.setRole(d, c, core.NEAR_NEUTRAL)
          break
        end
      end
    end
  end
end

------------------------------------------------------------ rebuilding

--- The city that should buy a new type (ai_city_for_type, 5db9:0c83): one
--- whose best build is poor, the longest held; a city taking or near a
--- neutral only after four turns, and counted four turns younger.
local function cityForType(g, side)
  local d = core.data(g, side)
  local best, bestScore = nil, -1
  for _, c in ipairs(own(g, side)) do
    local r = core.role(d, c)
    local candidate, younger = false, 0
    if r == core.TAKING_NEUTRAL or r == core.NEAR_NEUTRAL then
      if (d.held[c.index] or 0) > 3 then
        younger = 4
        candidate = not cities.buildsWell(g, side, c)
      end
    elseif r == core.WEAK or r == core.BUILDING then
      candidate = not cities.buildsWell(g, side, c)
    elseif r == core.STOP then
      if not cities.buildsWell(g, side, c) then
        core.setRole(d, c, core.NOWHERE)
        candidate = true
      end
    elseif r == core.NOWHERE then
      if not cities.buildsWell(g, side, c) then candidate = true
      else core.setRole(d, c, core.STOP) end
    end
    if candidate then
      local score = (d.held[c.index] or 0) - younger
      if bestScore < score then best, bestScore = c, score end
    end
  end
  return best
end

--- rebuilding (ai_rebuilding, 5db9:0af2): with 500 gold, and heroes enough
--- for the cities held, buy the card's type -- its second choice from 2000
--- gold.
function cities.rebuild(g, side)
  local d = core.data(g, side)
  local limit = math.max(5, math.floor(d.rebuildLimit * #g.map.cities / 80))
  if (d.heroes > 4 or d.own <= limit * d.heroes) and side.gold > 499 then
    local t = side.gold < 2001 and d.rebuildType or d.rebuildTypeRich
    local c = cityForType(g, side)
    if c and side.gold > 499 then buyType(g, side, c, t) end
  end
end

------------------------------------------------------------ production

--- The member cities of an assault group (5f19:0b72): up to four of the
--- side's quiet cities (role 5 or 8, no group's rally) that build well and
--- fast enough for the group -- the strongest builders win a place.
local function pickMembers(g, side, grp)
  local d = core.data(g, side)
  local groups = require("warlords.ai.groups")
  local minMax = core.has(grp.flags, core.GF_MOVE12) and 12
              or core.has(grp.flags, core.GF_MOVE16) and 16 or grp.size
  local members, strength = {}, {}
  for _, c in ipairs(own(g, side)) do
    if groups.free(g, side, c, false) then
      local f = d.flags[c.index] or 0
      local fits = (not core.has(grp.flags, core.GF_MOVE12) or core.has(f, core.CF_MOVE12))
               and (not core.has(grp.flags, core.GF_MOVE16) or core.has(f, core.CF_MOVE16))
               and (not core.has(grp.flags, core.GF_FLY) or core.has(f, core.CF_FLYGROUP))
      local plain = core.has(grp.flags, core.GF_MOVE16) or core.has(grp.flags, core.GF_MOVE12)
                 or core.has(grp.flags, core.GF_FLY)
                 or not (core.has(f, core.CF_MOVE12) or core.has(f, core.CF_FLYGROUP)
                         or core.has(f, core.CF_MOVE16))
      if fits and plain then
        local good, slot = cities.buildsWell(g, side, c)
        if good and minMax <= slot.move then
          local at
          if #members < 4 then at = #members + 1
          else
            local least = 100
            for k = 1, 4 do
              if strength[k] < least then least, at = strength[k], k end
            end
            if at and not (least <= slot.strength) then at = nil end
          end
          if at then members[at], strength[at] = c, slot.strength end
        end
      end
    end
  end
  grp.members = {}
  for k, c in ipairs(members) do
    grp.members[k] = c.index
    core.setRole(d, c, core.MEMBER)
  end
end

--- production (ai_production, 5db9:06d4): members are chosen afresh, then
--- every city not part way through something is set to build what its role
--- asks for. The phase stops when the side is broke and losing money.
function cities.production(g, side)
  local d = core.data(g, side)
  for _, c in ipairs(own(g, side)) do
    if core.role(d, c) == core.MEMBER then
      core.setRole(d, c, core.STOP)
      c.vectorTo = nil
    end
  end
  for gi = core.MAX_GROUPS, 1, -1 do
    if d.groups[gi].active ~= 0 then pickMembers(g, side, d.groups[gi]) end
  end
  if g.map.options.hiddenMap ~= 0 then
    -- a city whose neutral neighbours are all unseen goes looking (57ea:02ff)
    for _, c in ipairs(own(g, side)) do
      if core.role(d, c) == core.NEAR_NEUTRAL then
        local nb = cities.neutralNeighbours(g, side, c)
        if #nb > 0 then
          local seen = false
          for _, e in ipairs(nb) do
            if not core.cflag(d, e.city, core.CF_UNSEEN) then seen = true end
          end
          if not seen then core.setRole(d, c, core.TAKING_NEUTRAL) end
        end
      end
    end
  end
  for _, c in ipairs(own(g, side)) do
    c.vectorTo = nil
    if not cities.building(c) then
      if side.gold < 40 and (side.income or 0) < (side.upkeepTotal or 0) then return end
      local r = core.role(d, c)
      local purpose = 3
      if r == core.TAKING_NEUTRAL or r == core.EXPLORER then
        if not core.cflag(d, c, core.CF_FLIER) then buyFlier(g, side, c) end
        purpose = 4
      elseif r == core.NEAR_NEUTRAL then
        purpose = g.map.options.neutralCities ~= 0 and 2 or 1
      elseif r == core.WEAK then
        purpose = 2
      elseif r == core.EXPLORER2 then
        purpose = 4
      elseif r == core.STOP then
        purpose = nil
        c.producing, c.countdown = nil, 0
      end
      if purpose then produceFor(g, side, c, purpose) end
    end
  end
end

--- vectoring (ai_vectoring, 5db9:085f): each group's members send what they
--- build to its rally city.
function cities.vectoring(g, side)
  local d = core.data(g, side)
  for gi = d.maxGroups, 1, -1 do
    local grp = d.groups[gi]
    local rally = grp.rally and g.map.cities[grp.rally + 1]
    if grp.active ~= 0 and rally and rally.ownerIndex == side.index then
      core.setRole(d, rally, core.RALLY)
      for k = 4, 1, -1 do
        local m = grp.members[k] and g.map.cities[grp.members[k] + 1]
        if m and core.role(d, m) == core.MEMBER then cities.vector(g, m, rally) end
      end
    end
  end
end

return cities
