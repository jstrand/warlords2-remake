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
