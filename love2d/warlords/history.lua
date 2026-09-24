-- The game's history: what the History menu plays back.
--
-- docs/formats/history.md. Once a round, when the turn counter goes up
-- (8065:17f6 -> 6d51:0d60), a record is taken of every side's gold, score
-- and city count, every city's owner, and the round's deeds. The deeds are
-- kept as they happen, two a side, a lower type counting for more
-- (6d51:1244); the record takes each side's first, then second ones while
-- fewer than ten are taken (6d51:132f), and the slots are cleared.

local history = {}

-- the deed types, which are also their rank: lower counts for more
history.EMERGES, history.KILLED, history.QUEST_DONE, history.QUEST_GIVEN = 0, 1, 2, 3
history.VANQUISHED, history.WON, history.FINDS, history.VICTORIOUS = 4, 5, 6, 7
history.TREACHERY, history.WAR, history.PEACE = 8, 9, 10
-- first values with a meaning of their own
history.IN_BATTLE, history.SEARCHING = -1, -2           -- KILLED
history.BY_NAME = -1                                     -- WON: the name won it
history.ALLIES, history.SAGE, history.GOLD = 100, 101, 102   -- FINDS

history.LAST_TURN = 201
history.MAX_EVENTS = 10

--- Note a deed of a side's (6d51:1244). `name` is the hero's or the side's.
function history.deed(g, side, type, v1, v2, name)
  if not side then return end
  g.deeds = g.deeds or {}
  local d = g.deeds[side.index] or {}
  g.deeds[side.index] = d
  local e = { side = side.index, type = type, v1 = v1 or 0, v2 = v2 or 0,
              name = (name or ""):sub(1, 15) }
  if #d < 2 then d[#d + 1] = e return end
  local worse = (d[2].type >= d[1].type) and 2 or 1
  if d[worse].type > type then d[worse] = e end
end

-------------------------------------------------------------------- triumphs

-- What History > Triumphs counts (2c04:1163: 80 bytes a side, 10 an
-- opponent, five words). A side's own row counts what it lost; its row for
-- another side, what it killed of them. Counted after each battle
-- (67cc:1b43-1e92).
history.ARMIES, history.CREATURES, history.HEROES, history.NAVIES, history.STANDARDS = 0, 1, 2, 3, 4

local function bump(g, side, opp, k, n)
  if side == nil or opp == nil or side > 7 or opp > 7 then return end
  g.triumphs = g.triumphs or {}
  local t = g.triumphs[side] or {}
  g.triumphs[side] = t
  local row = t[opp] or { [0] = 0, 0, 0, 0, 0 }
  t[opp] = row
  row[k] = row[k] + (n or 1)
end

--- An army of `loser`'s killed by `killer`'s: a hero, an unnatural creature
--- (ARMYTYPE +48 set), or an army; a navy as well when it was at sea; and
--- the standards a hero carried.
function history.tally(g, army, killer)
  local armytype = require("warlords.armytype")
  local loser = army.owner
  local k
  if army.type == armytype.HERO then k = history.HEROES
  elseif (g.types.byId[army.type].bonus[48] or 0) ~= 0 then k = history.CREATURES
  else k = history.ARMIES end
  local std = 0
  for _, it in ipairs(army.items or {}) do if it.index < 8 then std = std + 1 end end
  for _, row in ipairs({ { loser, loser }, { killer, loser } }) do
    bump(g, row[1], row[2], k)
    if army.atSea then bump(g, row[1], row[2], history.NAVIES) end
    if std > 0 then bump(g, row[1], row[2], history.STANDARDS, std) end
  end
end

--- The count for side `me` against `opp`, kind k.
function history.triumph(g, me, opp, k)
  local t = g.triumphs and g.triumphs[me] and g.triumphs[me][opp]
  return t and t[k] or 0
end

--- The round's record (6d51:0d60), taken as the turn counter goes up.
function history.record(g)
  if g.turn > history.LAST_TURN then return end
  local report = require("warlords.report")
  local game = require("warlords.game")
  local r = { gold = {}, score = {}, cities = {}, owners = {}, events = {} }
  local win = report.figures(g, g.sides[1], report.WINNING)
  for i = 0, 7 do
    local s = g.map.sides[i + 1]
    r.gold[i] = (s and s.inUse) and (s.gold or 0) or 0
    r.score[i] = win.value[i] or 0
    r.cities[i] = s and #game.sideCities(g, s) or 0
  end
  for i, c in ipairs(g.map.cities) do
    r.owners[i] = c.razed and 0xff or (c.ownerIndex or 8)
  end
  local deeds, n = g.deeds or {}, 0
  local take = {}
  for i = 0, 7 do
    if deeds[i] and deeds[i][1] then take[i] = 1 n = n + 1 end
  end
  for i = 0, 7 do
    if deeds[i] and deeds[i][2] and n < history.MAX_EVENTS then take[i] = 2 n = n + 1 end
  end
  for i = 0, 7 do
    for k = 1, take[i] or 0 do r.events[#r.events + 1] = deeds[i][k] end
  end
  g.deeds = {}
  g.history = g.history or {}
  g.history[#g.history + 1] = r
end

return history
