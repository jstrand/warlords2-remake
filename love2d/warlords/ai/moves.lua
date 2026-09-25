-- The computer players' movement phases: move (standing orders), rescue,
-- last rescue, move search and move explore, specials -- and the explorer
-- walk and hero parties they use. docs/re/ai.md > Movement phases.

local armytype = require("warlords.armytype")
local core     = require("warlords.ai.core")

local moves = {}

local function city(g, i) return i and g.map.cities[i + 1] or nil end

local function onCityTile(g, a)
  local c = core.cityAt(g, a.x, a.y)
  return c ~= nil and core.standing(c)
end

------------------------------------------------------------ searching

--- The computer's visit to a sage (5e97:0080): with the map hidden, the
--- sage shows it the area round the unseen neutral city, next to one of its
--- own, with most unseen neutral cities within 20 (four at least); failing
--- that, a gem -- 3d500+500 gold -- is taken.
local function sage(g, side)
  local d = core.data(g, side)
  local gold = g.rng:dice(3, 500, 500)
  local best, bestCount, bestD = nil, -1, 0
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if c.ownerIndex == nil and core.cflag(d, c, core.CF_UNSEEN) then
      local nextToUs = false
      for _, n in ipairs(core.neighbours(g, c)) do
        if n.ownerIndex == side.index then nextToUs = true end
      end
      if nextToUs then
        local count, dOwn = 0, 1000
        for _, o in ipairs(g.map.cities) do
          if o.ownerIndex == nil then
            if core.cflag(d, o, core.CF_UNSEEN) and core.dist(c.x, c.y, o.x, o.y) < 20 then
              count = count + 1
            end
          elseif o.ownerIndex == side.index then
            local dd = core.dist(c.x, c.y, o.x, o.y)
            if dd < dOwn then dOwn = dd end
          end
        end
        if count > 3 and (bestCount < count or (count == bestCount and bestD < dOwn)) then
          best, bestCount, bestD = c, count, dOwn
        end
      end
    end
  end
  if g.map.options.hiddenMap == 0 or not best then
    side.gold = side.gold + gold
    return
  end
  local x = math.max(0, best.x + g.rng:dice(1, 11, -6))
  local y = math.max(0, best.y + g.rng:dice(1, 11, -6))
  x, y = math.min(x, g.map.width - 1), math.min(y, g.map.height - 1)
  require("warlords.site").sageMap(g, side, x, y)
  require("warlords.move").invalidate(g)
end

--- A hero searches the site it has reached (5ad0:15c3): a ruin not yet
--- searched, or a temple -- where it also takes a quest if it has none.
--- Returns 2 when it searched.
function moves.searchSite(g, sel, h, s)
  if not (h and core.alive(g, h) and h.x == s.x and h.y == s.y and (h.moves or 0) ~= 0) then
    return 0
  end
  local siteMod = require("warlords.site")
  local side = g.map.sides[h.owner + 1]
  if core.siteOpen(s) then
    local stack = sel and sel.armies or { h }
    local r = siteMod.search(g, stack, s.x, s.y, false)
    if r and r.kind == "sage" then sage(g, side) end
    if not core.isHero(h) then return 0 end
    for _, a in ipairs(stack) do a.done = nil end
    if g.map.options.hiddenMap ~= 0 and s.content == siteMod.ALLIES then
      for _, a in ipairs(core.onTile(g, side.index, s.x, s.y)) do
        if (a.aiOrder or 0) == 0 and core.magical(g, a) then
          a.aiExplore = true
          moves.explore(g, side, a, nil)
        end
      end
    end
  end
  return 2
end

------------------------------------------------------------ hero parties

--- Send a hero and one flying companion off (5ad0:11a6): to the nearest
--- unsearched ruin seen round -- 215 - d inside 15, 90 - d inside 40 --
--- that no other hero is bound for; else to the side's own city scoring
--- best the same way (115 - d, 40 - d; a rally city 80 nearer) when more
--- than 2 away. Returns 0 when it went, 1 when the hero has no flier with
--- it, 2 when it searched.
function moves.party(g, side, list)
  local hero, flier
  for _, a in ipairs(list) do
    if not hero and core.isHero(a) then hero = a end
    if not flier and core.flies(g, a) then flier = a end
  end
  if not hero then return 0 end
  if not flier then return 1 end
  local d = core.data(g, side)
  list = { hero, flier }
  core.clearOrder(hero); core.clearOrder(flier)
  local siteMod = require("warlords.site")
  local score, bestSite, bestCity = -1, nil, nil
  for i = #g.map.sites, 1, -1 do
    local s = g.map.sites[i]
    local taken = false
    for _, o in ipairs(core.armies(g, side.index)) do
      if o ~= hero and core.isHero(o) and o.aiOrder == core.ORDER_SITE and o.aiDest == s.index then
        taken = true
      end
    end
    if not taken and core.explored(g, side.index, s.x, s.y)
       and s.content ~= siteMod.TEMPLE and not s.searched then
      local dd = core.dist(hero.x, hero.y, s.x, s.y)
      local base
      if dd < 15 and score < 215 - dd then base = 215
      elseif not (dd > 39 or 90 - dd <= score) then base = 90 end
      if base then score, bestSite = base - dd, s end
    end
  end
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if c.ownerIndex == side.index then
      local dd = core.dist(hero.x, hero.y, c.x, c.y)
      if core.role(d, c) == core.RALLY then dd = dd - 80 end
      local base
      if dd < 15 and score < 115 - dd then base = 115
      elseif not (dd > 39 or 40 - dd <= score) then base = 40 end
      if base then score, bestCity = base - dd, c end
    end
  end
  if bestSite then
    local sel = core.order(g, list, core.ORDER_SITE, bestSite.index, 0x100)
    if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
    return moves.searchSite(g, sel, hero, bestSite)
  end
  if bestCity and core.dist(hero.x, hero.y, bestCity.x, bestCity.y) > 2 then
    local sel = core.order(g, list, core.ORDER_CITY, bestCity.index, 0)
    if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
  end
  return 0
end

------------------------------------------------------------ explorers

--- Look over the 10 x 10 tiles round an explorer (57ea:177f) for where to
--- go: a city or a site next to explored ground close by, else the unseen
--- ground scoring best -- far from where it is and from home, with much
--- unseen round it, open land (road best) for a land stack, water for one
--- at sea or leaving port.
local function scan(g, side, a, sel)
  local move = require("warlords.move")
  local game = require("warlords.game")
  local scn = require("warlords.scn")
  local home = city(g, a.homeCity) or side.capital or { x = a.x, y = a.y }
  local coast = a.atSea
  if g.rng:dice(1, 4, -1) == 0 and onCityTile(g, a) and not core.isHero(a) then
    local c = core.cityAt(g, a.x, a.y)
    if c and move.isPort and move.isPort(g, c) then coast = true end
  end
  local flying = sel.mode == move.FLYING
  local _, woods, hills = move.stackMode(g, sel.armies)
  local kind, ex, ey = 0, nil, nil
  local tx, ty, found, best = nil, nil, false, 0
  local function exploredRound(x, y)
    for nx = x - 1, x + 1 do
      for ny = y - 1, y + 1 do
        if nx >= 0 and ny >= 0 and nx < g.map.width and ny < g.map.height
           and game.seen(g, side.index, nx, ny) then
          return true
        end
      end
    end
    return false
  end
  for x = a.x - 5, a.x + 4 do
    for y = a.y - 5, a.y + 4 do
      if x >= 0 and y >= 0 and x < g.map.width and y < g.map.height then
        local t = core.terrain(g, x, y)
        local c = core.cityAt(g, x, y)
        if t == move.CITY and (not c or c.ownerIndex ~= side.index) then
          local dd = flying and 2 or core.dist(a.x, a.y, x, y)
          if dd < 4 and (not a.atSea or (c and move.isPort and move.isPort(g, c)))
             and exploredRound(x, y) then
            kind, ex, ey = 1, x, y
          end
        end
        if t == move.SITE and kind == 0 and not a.atSea and exploredRound(x, y) then
          kind, ex, ey = 2, x, y
        end
        local road = scn.roadAt(g.map, x, y) % 0x20 ~= 0
        if t ~= move.CITY and (flying or road or hills or t ~= move.HILLS)
           and not game.seen(g, side.index, x, y)
           and (flying or move.COST[t] ~= 0) then
          local unseen = 0
          for nx = x - 1, x + 1 do
            for ny = y - 1, y + 1 do
              if nx >= 0 and ny >= 0 and nx < g.map.width and ny < g.map.height
                 and not game.seen(g, side.index, nx, ny) then
                unseen = unseen + 1
              end
            end
          end
          if unseen > 1 then
            local bonus = 0
            local wet = t == move.WATER or t == move.SHORE
            if coast then
              if wet then bonus = bonus + 200 end
            elseif not wet then
              if t ~= move.HILLS then bonus = 100 end
              if road then bonus = bonus + 200 end
            end
            local fromHome = core.dist(x, y, home.x, home.y)
            local s = g.rng:dice(1, 10, 0) + core.dist(x, y, a.x, a.y) * 2 + fromHome
                    + unseen * 10 + bonus
            if coast and bonus ~= 0 then s = s + math.min(fromHome, 50) * 10 end
            if best < s then tx, ty, found, best = x, y, true, s end
          end
        end
      end
    end
  end
  return found, tx, ty, kind, ex, ey
end

--- A flier's fallback: the nearest unseen city, give or take 1d10
--- (57ea:164e); it aims one up and left of it.
local function unseenCity(g, side, a)
  local d = core.data(g, side)
  local best, bestD
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if core.standing(c) and core.cflag(d, c, core.CF_UNSEEN) then
      local dd = core.dist(c.x, c.y, a.x, a.y) + g.rng:dice(1, 10, 0)
      if not bestD or dd < bestD then best, bestD = c, dd end
    end
  end
  if best then return best.x - 1, best.y - 1 end
  return nil
end

--- An explorer next to a city goes for it (57ea:10a8) if the side may attack
--- it and the odds are good: 75%, or for a weak army 50% while few neutral
--- cities are left, 40% while the side is small. Returns true if it went.
local function tryCity(g, side, a, ex, ey)
  local d = core.data(g, side)
  local diplo = require("warlords.ai.diplomacy")
  local cities = require("warlords.ai.cities")
  local pick
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if core.standing(c) and c.ownerIndex ~= side.index and d.questCity ~= c.index
       and core.dist(c.x, c.y, ex, ey) <= 2 and diplo.canAttack(g, side, c) then
      if core.cflag(d, c, core.CF_UNSEEN) then cities.clearUnseen(g, side) end
      if not core.cflag(d, c, core.CF_UNSEEN) then pick = c break end
    end
  end
  if not pick then return false end
  local sel = core.order(g, { a }, core.ORDER_ROAM, 0, 0x20)
  if not sel then return false end
  local o = core.odds(g, sel, pick.x, pick.y)
  local need = 75
  if not core.isHero(a) and (a.strength or 0) < 4 then
    if d.neutral < math.floor(#g.map.cities / 10) then need = 50
    elseif d.own <= math.min(math.floor(g.turn / 2), 10) then need = 40 end
  end
  if o < need then return false end
  a.aiExplore = nil
  sel = core.order(g, { a }, core.ORDER_CITY, pick.index, 0x80)
  if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
  return true
end

--- An explorer next to a site visits it (57ea:12eb): a hero any ruin not yet
--- searched, anyone a temple that has not blessed it -- if nobody else
--- stands there. Returns true if it went.
local function trySite(g, side, a, sx, sy)
  local siteMod = require("warlords.site")
  local game = require("warlords.game")
  local s = core.siteAt(g, sx, sy)
  if not s or not core.siteOpen(s) or a.atSea then return false end
  local temple = s.content == siteMod.TEMPLE
  if temple and a.blessings and s.templeIndex and a.blessings[s.templeIndex] then return false end
  if not (core.isHero(a) or temple) then return false end
  local there = game.armiesAt(g, sx, sy)
  if there[1] and there[1].owner ~= side.index then return false end
  local sel = core.order(g, { a }, core.ORDER_ROAM, 0, 0)
  if not sel then return false end
  sel.leader.target = { x = sx, y = sy }
  local r = core.moveTo(g, sel, sx, sy)
  if r == 1 then return false end
  if core.alive(g, a) then
    if a.x == sx and a.y == sy then
      if core.isHero(a) then moves.searchSite(g, sel, a, s)
      else siteMod.search(g, { a }, sx, sy, false) end
      if s.content == siteMod.ALLIES then
        for _, b in ipairs(core.onTile(g, side.index, sx, sy)) do
          if (b.aiOrder or 0) == 0 and core.magical(g, b) then b.aiExplore = true end
        end
      end
    else
      if temple and a.atSea and s.templeIndex then
        a.blessings = a.blessings or {}
        a.blessings[s.templeIndex] = true
      end
      a.done = true
    end
    a.aiExplore = true
    core.clearOrder(a)
    a.target = nil
  end
  return true
end

--- Walk an explorer's stack to (x, y) (57ea:1581).
local function walkTo(g, a, x, y)
  a.target = { x = x, y = y }
  local sel = core.selectStackOf(g, a)
  local r = core.moveTo(g, sel, x, y)
  if core.alive(g, a) then
    if r == 1 then
      a.aiExplore = nil
      core.clearOrder(a)
    end
    a.done = true
    a.target = nil
    a.group = nil
  end
end

--- One step of an explorer (57ea:0f46).
local function exploreStep(g, side, a, home)
  local sel = core.select(g, { a })
  local found, tx, ty, kind, ex, ey = scan(g, side, a, sel)
  if not found then
    if core.flies(g, a) then tx, ty = unseenCity(g, side, a) end
    if not tx then
      local n = home and require("warlords.ai.cities").openNeutral(g, side, home, {})
      if not n then
        core.clearOrder(a)
        a.aiExplore = nil
        a.target = nil
        a.group = nil
        return
      end
      tx, ty = n.x, n.y
    end
  elseif kind == 1 then
    if tryCity(g, side, a, ex, ey) then return end
  elseif kind == 2 then
    if trySite(g, side, a, ex, ey) then return end
  end
  walkTo(g, a, tx, ty)
end

--- Send an army exploring (57ea:0e4b): steps while it has moves, one step
--- per 5 movement points and one more, as long as each step gets it
--- somewhere.
function moves.explore(g, side, a, home)
  if not core.alive(g, a) then return end
  core.order(g, { a }, core.ORDER_ROAM, 0, 0x20)
  local steps = math.floor((a.moves or 0) / 5) + 1
  while true do
    if not core.alive(g, a) or (a.moves or 0) < 2 then return end
    local px, py = a.x, a.y
    exploreStep(g, side, a, home)
    if not core.alive(g, a) or (a.x == px and a.y == py) then return end
    if (a.moves or 0) < 4 then return end
    if steps == 0 then return end
    if not a.aiExplore then return end
    steps = steps - 1
  end
end

------------------------------------------------------------ the phases

--- move search (ai_phase_move_search, 5ad0:0284), with the map hidden: the
--- explorers go on -- heroes first, then the rest -- save heroes while a
--- rally city stands and weak fliers once seven are out.
function moves.search(g, side)
  local d = core.data(g, side)
  local rally = false
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex == side.index and core.role(d, c) == core.RALLY then rally = true end
  end
  d.searchers, d.explorers = 0, 0
  for pass = 0, 1 do
    for _, a in ipairs(core.armies(g, side.index)) do
      if core.alive(g, a) and not a.transit and a.aiExplore
         and ((pass == 0) == core.isHero(a)) then
        if (core.isHero(a) and rally)
           or (core.flies(g, a) and d.explorers > 7 and (a.strength or 0) < 4) then
          core.clearOrder(a)
          a.aiExplore = nil
        else
          moves.explore(g, side, a, nil)
          if core.alive(g, a) and a.owner == side.index then
            if core.flies(g, a) then d.explorers = d.explorers + 1
            else d.searchers = d.searchers + 1 end
          end
        end
      end
    end
  end
end

--- move explore (ai_phase_move_explore, 5ad0:0458): each hero sent off in a
--- party keeps going while it has 3 moves and gets somewhere.
function moves.heroParties(g, side)
  for _, a in ipairs(core.armies(g, side.index)) do
    if core.alive(g, a) and not a.transit and a.aiParty then
      a.aiParty = nil
      if core.isHero(a) then
        local going, guard = true, 0
        while going and guard < 20 do
          guard = guard + 1
          if not core.alive(g, a) then break end
          local list = core.collectOrdered(g, side.index, a.x, a.y, a.aiGroup, a.aiOrder, 0)
          if #list == 0 or (a.moves or 0) < 3 then break end
          local px, py = a.x, a.y
          local r = moves.party(g, side, list)
          if r == 0 or r == 1 then going = false end
          if core.alive(g, a) and a.x == px and a.y == py and (a.maxMoves or 0) > (a.moves or 0) then
            break
          end
        end
      end
    end
  end
end

--- Is the stack's neutral target still the thing to do (5ad0:05ff)? A stack
--- heading for one of the side's own cities is sent instead at the nearest
--- neutral neighbour it beats three times in four -- or, early in a hidden
--- game, off exploring. One heading for another side's city stops if the
--- side may not attack it.
local function neutralStillOn(g, side, sel)
  local d = core.data(g, side)
  local lead = sel.leader
  local dest = city(g, lead.aiDest)
  if not dest then return true end
  local diplo = require("warlords.ai.diplomacy")
  local cities = require("warlords.ai.cities")
  if dest.ownerIndex == side.index then
    local here = core.cityAt(g, lead.x, lead.y)
    local nb = here and cities.neutralNeighbours(g, side, here) or {}
    if #nb == 0 then nb = cities.neutralNeighbours(g, side, dest) end
    local odds = 100
    local best, bd = nil, 1000
    for j = #nb, 1, -1 do
      local n = nb[j].city
      if not core.cflag(d, n, core.CF_UNSEEN) then
        if g.map.options.neutralCities ~= 0 then odds = core.odds(g, sel, n.x, n.y) end
        local dd = core.dist(lead.x, lead.y, n.x, n.y)
        if odds > 74 and dd < bd then best, bd = n, dd end
      end
    end
    if not best then
      if g.map.options.hiddenMap ~= 0 and g.turn < 10 then
        for _, a in ipairs(sel.armies) do moves.explore(g, side, a, nil) end
        return false
      end
      return true
    end
    for _, a in ipairs(sel.armies) do
      a.aiOrder, a.aiDest = core.ORDER_CITY, best.index
      core.setTarget(g, a, core.ORDER_CITY, best.index)
    end
    return true
  end
  if not diplo.canAttack(g, side, dest) then
    for _, a in ipairs(sel.armies) do
      core.clearOrder(a)
      a.target = nil
    end
    return false
  end
  return true
end

--- move #1 and #2 (ai_phase_move, 5ad0:0000): every stack with somewhere
--- to go walks there, the nearest to the last one first; stale orders --
--- to a city gone, or none at all -- are dropped.
function moves.move(g, side)
  local d = core.data(g, side)
  for _, a in ipairs(core.armies(g, side.index)) do
    if not a.transit then a.aiMoved = (a.moves or 0) < 2 or nil end
  end
  d.cursor = d.cursor or { x = side.capX or 0, y = side.capY or 0 }
  while true do
    local pick, pd
    for _, a in ipairs(core.armies(g, side.index)) do
      if not a.transit and a.x and not a.done and not a.aiMoved then
        local dd = math.abs(a.x - d.cursor.x) + math.abs(a.y - d.cursor.y)
        if dd == 0 then dd = 9000 end
        if not pd or dd < pd then pick, pd = a, dd end
      end
    end
    if not pick then break end
    d.cursor = { x = pick.x, y = pick.y }
    pick.aiMoved = true
    if pick.target and pick.target.x and pick.target.x >= 0 and (pick.moves or 0) > 1 then
      local sel = core.selectStackOf(g, pick)
      local lead = sel.leader
      local order, dest = lead.aiOrder or 0, city(g, lead.aiDest)
      if order == core.ORDER_NONE or (order == core.ORDER_CITY
          and (not dest or lead.aiDest == d.questCity or not core.standing(dest))) then
        for _, a in ipairs(sel.armies) do
          core.clearOrder(a)
          a.target = nil
          a.done = true
        end
      elseif not lead.aiNeutral or neutralStillOn(g, side, sel) then
        dest = city(g, lead.aiDest)
        if lead.aiOrder == core.ORDER_CITY and dest and dest.ownerIndex == side.index then
          lead.target = { x = dest.x + 1, y = dest.y + 1 }
        end
        if lead.target and core.alive(g, lead) then
          core.moveTo(g, sel, lead.target.x, lead.target.y)
        end
      end
    end
  end
end

--- The best city for an idle stack by the flood (5ad0:0f3c): the nearest
--- neutral or enemy city it beats three times in four, or one of its own
--- (30 farther, a rally city 10, and 100 more for a stack of four or more).
local function idleTarget(g, side, sel, flood, n)
  local d = core.data(g, side)
  local best, bestD = nil, 1000
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    local owner = core.owner(c)
    if core.standing(c) and not core.cflag(d, c, core.CF_UNSEEN) and d.questCity ~= c.index
       and (owner == core.NEUTRAL or owner == side.index or core.state(g, side.index, owner) == 2) then
      local dd, px = core.cityDistance(g, flood, c)
      if px then
        if owner == side.index then
          dd = dd + (core.role(d, c) == core.RALLY and 10 or 30)
          if n > 3 then dd = dd + 100 end
          if dd < bestD then best, bestD = c, dd end
        elseif core.odds(g, sel, c.x, c.y) > 74 and dd < bestD then
          best, bestD = c, dd
        end
      end
    end
  end
  return best
end

--- An idle army out in the field (5ad0:0c5b): a weak land army is simply
--- disbanded; otherwise the stack it stands in (armies with 8 moves left,
--- leaving behind all but fliers and heroes when it has all three kinds)
--- goes to the best city in reach -- or is disbanded when there is none.
function moves.sendIdle(g, side, a)
  local game = require("warlords.game")
  local cities = require("warlords.ai.cities")
  local home = 1000
  for _, c in ipairs(g.map.cities) do
    if c.ownerIndex == side.index and core.standing(c) then
      local dd = core.dist(a.x, a.y, c.x, c.y)
      if dd < home then home = dd end
    end
  end
  local weak = (a.strength or 0) < 3 or (a.maxMoves or 0) < 8
  if not core.flies(g, a) and weak then
    game.disband(g, side, { a })
    return
  end
  local list = core.collect(g, side.index, a.x, a.y, 8)
  if #list == 0 then return end
  local fl, he, ot = 0, 0, 0
  for _, b in ipairs(list) do
    if core.flies(g, b) then fl = fl + 1
    elseif core.isHero(b) then he = he + 1
    else ot = ot + 1 end
  end
  if fl ~= 0 and he ~= 0 and ot ~= 0 then
    local keep = {}
    for _, b in ipairs(list) do
      if core.flies(g, b) or core.isHero(b) then keep[#keep + 1] = b end
    end
    list = keep
  end
  if #list < 2 and weak then
    game.disband(g, side, { a })
    return
  end
  local sel = core.select(g, list)
  local range = sel.hero and 50 or math.floor((a.strength or 0) / 2) * 10
  range = math.min(range, home + 10)
  local flood = core.flood(g, side.index, a.x, a.y, range, sel)
  local c = idleTarget(g, side, sel, flood, #list)
  if not c then
    game.disband(g, side, list)
    return
  end
  sel = core.order(g, list, core.ORDER_CITY, c.index, 0)
  if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
end

--- rescue (ai_phase_rescue, 5ad0:0888): armies that have not moved this turn.
--- A stack sent at a city that is gone forgets it; an idle army out of town
--- -- not an explorer -- is put to use.
function moves.rescue(g, side)
  for _, a in ipairs(core.armies(g, side.index)) do
    if core.alive(g, a) and not a.transit and a.x and (a.maxMoves or 0) <= (a.moves or 0) then
      if (a.moves or 0) == (a.maxMoves or 0) + 2 and not onCityTile(g, a)
         and a.aiOrder == core.ORDER_CITY then
        local dest = city(g, a.aiDest)
        if not dest or not core.standing(dest) then
          local gone = a.aiDest
          for _, b in ipairs(core.onTile(g, side.index, a.x, a.y)) do
            if b.aiOrder == core.ORDER_CITY and b.aiDest == gone then
              core.clearOrder(b)
              b.aiGroup = 0
            end
          end
        end
      end
      if core.alive(g, a) and not a.aiExplore and (a.aiOrder or 0) == 0
         and not onCityTile(g, a) then
        moves.sendIdle(g, side, a)
      end
    end
  end
end

--- last rescue (ai_phase_last_rescue, 5ad0:0b4d): an army out of town that
--- has not moved for a whole turn loses its orders and is put to use.
function moves.lastRescue(g, side)
  for _, a in ipairs(core.armies(g, side.index)) do
    if core.alive(g, a) and not a.transit and a.x
       and (a.moves or 0) == (a.maxMoves or 0) + 2 and not onCityTile(g, a) then
      a.aiExplore, a.aiNeutral = nil, nil
      core.clearOrder(a)
      a.target = nil
      moves.sendIdle(g, side, a)
    end
  end
end

--- specials (ai_phase_specials, 5ad0:10e3): a city with a hero in it that is
--- no rally city sends the hero's party off; if the hero has no flier to go
--- with and the city is not busy building, the city turns explorer, to buy
--- one.
function moves.specials(g, side)
  local d = core.data(g, side)
  local cities = require("warlords.ai.cities")
  for _, c in ipairs(cities.own(g, side)) do
    if core.cflag(d, c, core.CF_HERO) and core.role(d, c) ~= core.RALLY then
      local list = core.collectOrdered(g, side.index, c.x + 1, c.y, 0, 0, 0)
      if #list ~= 0 and moves.party(g, side, list) == 1 and not cities.building(c) then
        core.setRole(d, c, core.EXPLORER)
      end
    end
  end
end

return moves
