-- The computer players' hero phase (ai_phase_move_hero, 6087:0000): each
-- hero weighs a quest temple, a site, an item lying about and an enemy city
-- -- 100 - distance + 1d20 each -- and goes for the best, twice at most.
-- docs/re/ai.md; addresses are Ghidra's.

local core = require("warlords.ai.core")

local heroes = {}

local function city(g, i) return i and g.map.cities[i + 1] or nil end

-- the temples' blessing marks, one per temple (DS:0958)
local function blessed(h, s)
  return s.templeIndex ~= nil and h.blessings ~= nil and h.blessings[s.templeIndex] == true
end

--- Is a site still worth the hero's while (6087:1637): near enough -- twice
--- the turn plus 3, plus 12 more for a rich one -- seen round, and a temple
--- or a ruin not yet searched.
local function worth(g, side, s, h)
  local dd = core.dist(s.x, s.y, h.x, h.y)
  local reach = g.turn * 2 + (s.rich and 15 or 3)
  return dd < reach and core.explored(g, side.index, s.x, s.y) and core.siteOpen(s)
end

--- Is an item lying where a hero may take it (6087:1736): on the ground,
--- seen round, and not in somebody else's city.
local function lying(g, side, it)
  if it.status ~= 1 or not it.x or it.planted then return false end
  if not core.explored(g, side.index, it.x, it.y) then return false end
  local c = core.cityAt(g, it.x, it.y)
  return not c or not core.standing(c) or c.ownerIndex == side.index
end

--- The side's heroes, six at most, with the city or site each stands on
--- (6087:065d).
local function survey(g, side)
  local d = core.data(g, side)
  local info = { heroes = {}, city = {}, site = {} }
  d.heroes = 0
  for _, a in ipairs(core.armies(g, side.index)) do
    if core.isHero(a) and not a.transit and #info.heroes < 6 then
      local k = #info.heroes + 1
      info.heroes[k] = a
      local c = core.cityAt(g, a.x, a.y)
      if c and core.standing(c) then info.city[k] = c end
      local s = core.siteAt(g, a.x, a.y)
      if s then info.site[k] = s end
      d.heroes = d.heroes + 1
    end
  end
  return info
end

--- Temples and ruins in reach (6087:0fda): within 5 for a hero sent at a
--- city, 25 otherwise. The nearest temple where it could take a quest is
--- remembered apart.
local function sites(g, side, k, info)
  local h = info.heroes[k]
  local siteMod = require("warlords.site")
  local range = h.aiOrder == core.ORDER_CITY and 5 or 25
  local questD, quest, siteD, site = 100, nil, 100, nil
  for _, s in ipairs(g.map.sites) do
    if worth(g, side, s, h) then
      local dd = core.dist(s.x, s.y, h.x, h.y)
      local ok = true
      if s.content == siteMod.TEMPLE then
        local free = g.map.options.quests ~= 0 and side.quest == nil
                     and dd < range and dd < questD
        if free then questD, quest = dd, s
        elseif blessed(h, s) then ok = false end
      else
        for j, o in ipairs(info.heroes) do
          if j ~= k and o.aiOrder == core.ORDER_SITE and o.aiDest == s.index then ok = false end
        end
      end
      if ok and dd <= range and dd < siteD then siteD, site = dd, s end
    end
  end
  info.quest, info.questD, info.target, info.siteD = quest, questD, site, siteD
end

--- Items lying about within 5 (sent at a city) or 15 (6087:11f8).
local function items(g, side, k, info)
  local h = info.heroes[k]
  local range = h.aiOrder == core.ORDER_CITY and 5 or 15
  local best, bestD = nil, 100
  for i = #g.map.items, 1, -1 do
    local it = g.map.items[i]
    if lying(g, side, it) then
      local taken = false
      for j, o in ipairs(info.heroes) do
        if j ~= k and o.aiOrder == core.ORDER_ITEM and o.aiDest == it.index then taken = true end
      end
      if not taken then
        local dd = core.dist(it.x, it.y, h.x, h.y)
        if dd <= range and dd < bestD then best, bestD = it, dd end
      end
    end
  end
  info.item, info.itemD = best, bestD
end

--- An enemy city the hero's stack beats (6087:1348): of a side it is at
--- war with, within 20 or the one it was sent at (+20 on the odds), with
--- 75% odds -- 95% for fewer than three.
local function enemy(g, side, k, info)
  local d = core.data(g, side)
  local h = info.heroes[k]
  info.enemy, info.enemyD = nil, 20
  local dest = h.aiOrder == core.ORDER_CITY and h.aiDest or nil
  local list = core.collectOrdered(g, side.index, h.x, h.y, h.aiGroup, h.aiOrder, 12)
  if #list == 0 then return end
  local sel = core.select(g, list)
  local need = #list < 3 and 95 or 75
  local bestOdds = 0
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if core.standing(c) and not core.cflag(d, c, core.CF_UNSEEN) and c.ownerIndex ~= nil
       and core.state(g, side.index, c.ownerIndex) == 2 then
      local dd = core.dist(c.x, c.y, h.x, h.y)
      if dd <= info.enemyD or dest == c.index then
        local o = core.odds(g, sel, c.x, c.y) + (dest == c.index and 20 or 0)
        if o >= need and (bestOdds < o or (o == bestOdds and dd < info.enemyD)) then
          info.enemy, info.enemyD, bestOdds = c, dd, o
        end
      end
    end
  end
end

--- Pick (ai_choose_target, 6087:0efc): 1 the quest temple, 3 the site,
--- 4 the enemy city, 2 the item, 0 nothing. A hero in a city with four or
--- fewer armies does not leave it for an enemy city or an item.
local function choose(g, side, k, info)
  local d = core.data(g, side)
  local r = g.rng:dice(1, 20, 0)
  local pick = info.quest and 1 or 0
  local best = 0
  if info.quest then best = 100 - info.questD + r end
  r = g.rng:dice(1, 20, 0)
  local s = 100 - info.siteD + r
  if info.target and best < s then pick, best = 3, s end
  local c = info.city[k]
  if not c or (d.garrison[c.index] or 0) > 4 then
    r = g.rng:dice(1, 20, 0)
    s = 100 - info.enemyD + r
    if info.enemy and best < s then pick, best = 4, s end
    r = g.rng:dice(1, 20, 0)
    if info.item and best < 100 - info.itemD + r then pick = 2 end
  end
  return pick
end

--- Go for the site (3) or the item (2) (6087:07ea). A hero leaving a city
--- takes one flier with it, or goes alone when enemies are 15 away or more.
--- Returns 1 when it got there.
local function go(g, side, k, kind, info)
  local d = core.data(g, side)
  local cities = require("warlords.ai.cities")
  local h = info.heroes[k]
  local tx, ty, target
  if kind == 3 then
    target = info.target
  else
    target = info.item
  end
  if not target then return 0 end
  tx, ty = target.x, target.y
  local list = core.collectOrdered(g, side.index, h.x, h.y, h.aiGroup, h.aiOrder, 12)
  local n = #list
  if n == 0 then return 0 end
  local c = info.city[k]
  local near = 100
  if c then
    near = cities.nearestArmy(g, side, h.x, h.y)
    local flier
    for i = #list, 1, -1 do
      if core.flies(g, list[i]) then flier = list[i] end
    end
    if not flier then
      if near >= 15 then list, n = { h }, 1 end
    else
      list, n = { h, flier }, 2
    end
  end
  if not c or (d.garrison[c.index] or 0) ~= n or n > 2 or near > 14 then
    local sel
    if kind == 3 then
      sel = core.order(g, list, core.ORDER_SITE, target.index, 0)
    else
      sel = core.order(g, list, core.ORDER_ROAM, 0, 0)
      if sel then sel.leader.target = { x = tx, y = ty } end
    end
    if sel and core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) == 4 then
      return 1
    end
  end
  return 0
end

--- Go for the enemy city (6087:0b75) -- unless the hero's stack is its
--- city's whole garrison and an enemy is near (10, or 15 from turn 6).
local function attack(g, side, k, info)
  local d = core.data(g, side)
  local cities = require("warlords.ai.cities")
  local h = info.heroes[k]
  local target = info.enemy
  if not target then return 0 end
  local list = core.collectOrdered(g, side.index, h.x, h.y, h.aiGroup, h.aiOrder, 12)
  if #list == 0 then return 0 end
  local c = info.city[k]
  if c and (d.garrison[c.index] or 0) == #list then
    local near = cities.nearestArmy(g, side, h.x, h.y)
    if near < (g.turn < 6 and 10 or 15) then return 0 end
  end
  local sel = core.order(g, list, core.ORDER_CITY, target.index, 0)
  if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
  return 0
end

--- The quest hero goes for the city it is to take or raze (6087:0cf3) --
--- with one army for a neutral city or before turn 8, two after -- when it
--- beats it three times in four or was already sent there.
local function questGo(g, side, k, info)
  local d = core.data(g, side)
  local h = info.heroes[k]
  local q = side.quest
  local quest = require("warlords.quest")
  if not (q and (q.type == quest.OCCUPY or q.type == quest.RAZE)) then return false end
  local c = q.target
  if not c or core.cflag(d, c, core.CF_UNSEEN) then return false end
  local need = (c.ownerIndex == nil or g.turn < 8) and 1 or 2
  local list = core.collect(g, side.index, h.x, h.y, 8)
  if #list < need then return false end
  if not (h.aiOrder == core.ORDER_CITY and h.aiDest == c.index) then
    local sel = core.select(g, list)
    if core.odds(g, sel, c.x, c.y) < 75 then return false end
  end
  local sel = core.order(g, list, core.ORDER_CITY, c.index, 0)
  if sel then core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y) end
  return true
end

--- move Hero (ai_phase_move_hero, 6087:0000).
function heroes.phase(g, side)
  local d = core.data(g, side)
  local quest = require("warlords.quest")
  local cities = require("warlords.ai.cities")
  d.questCity = nil
  local q = side.quest
  if g.map.options.quests ~= 0 and q and (q.type == quest.OCCUPY or q.type == quest.RAZE)
     and q.target and q.target.index then
    d.questCity = q.target.index
  end
  local info = survey(g, side)
  for k = #info.heroes, 1, -1 do
    local h = info.heroes[k]
    if h and core.alive(g, h) and h.owner == side.index and core.isHero(h) then
      if info.city[k] then cities.garrison(g, side, info.city[k], true) end
      local skip = g.map.options.quests ~= 0 and side.quest and side.quest.hero == h
                   and questGo(g, side, k, info)
      if not skip and core.alive(g, h) then
        if h.aiOrder == core.ORDER_SITE then
          local s = g.map.sites[(h.aiDest or -1) + 1]
          if not s or not worth(g, side, s, h) then
            for _, a in ipairs(core.onTile(g, side.index, h.x, h.y)) do
              if (a.aiOrder or 0) == (h.aiOrder or 0) then
                core.clearOrder(a)
                a.target = nil
              end
            end
          else
            info.target = s
            go(g, side, k, 3, info)
          end
        end
        local step, tries, moved = 1, 0, false
        while step ~= 0 and tries < 2 and core.alive(g, h) and (h.moves or 0) > 3 do
          tries = tries + 1
          sites(g, side, k, info)
          items(g, side, k, info)
          enemy(g, side, k, info)
          local pick = choose(g, side, k, info)
          if pick == 1 then
            step = 0
          elseif pick == 2 or pick == 3 then
            moved = true
            step = go(g, side, k, pick, info)
          elseif pick == 4 then
            moved = true
            step = attack(g, side, k, info)
          else
            if moved and g.map.options.hiddenMap ~= 0 and g.turn < 10
               and #core.collect(g, side.index, h.x, h.y, 8) == 1 then
              require("warlords.ai.moves").explore(g, side, h, nil)
            end
            step = 0
          end
          if not core.alive(g, h) or h.owner ~= side.index then step = 0 end
        end
      end
    end
  end
end

return heroes
