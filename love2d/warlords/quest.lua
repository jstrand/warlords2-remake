-- Quests: taking one at a temple, checking it off, and the reward.
--
-- docs/rules.md > Quests. `quest_assign` is Ghidra 4976:0d7a, `quest_check`
-- 4976:1ded and `quest_choose_reward` 4976:1909. One quest per side at a time.

local armytype = require("warlords.armytype")

local quest = {}

quest.SLAY_HERO, quest.RETRIEVE_ITEM, quest.SLAY_TYPE = 0, 1, 2
quest.SLAUGHTER, quest.OCCUPY, quest.RAZE, quest.PILLAGE_GOLD = 3, 4, 5, 6

-- DS:00a0, rolled with 1d10: types 4, 5 and 6 come up twice as often.
quest.TYPE_TABLE = { 0, 1, 2, 3, 4, 5, 6, 4, 5, 6 }

quest.DESCRIPTIONS = {
  [0] = "slay the enemy hero",
  "retrieve a magic item",
  "slay a unit of an enemy army type",
  "slaughter %d armies of one side",
  "force a city into submission and occupy it",
  "conquer a city and raze it",
  "sack and pillage %d gold",
}

-- how far a target may be from the hero: a human's, a computer's
quest.ITEM_RANGE, quest.CITY_RANGE = 50, 60
quest.AI_ITEM_RANGE, quest.AI_CITY_RANGE, quest.RAZE_RANGE = 30, 40, 60

-- what a quest's target *is*, so it can be saved and restored by reference
quest.TARGET_KIND = {
  [0] = "army", "item", "armytype", "side", "city", "city", "none",
}
quest.EXPERIENCE = 10

-- The allies a quest gives are always Archons (4976:1d5b pushes 19).
quest.REWARD_ALLY = 19

--------------------------------------------------------------------- targets

local function dist(x1, y1, x2, y2)
  return math.floor(math.sqrt((x1 - x2) ^ 2 + (y1 - y2) ^ 2))
end

--- Pick a target for a quest of this type (quest_assign, 4976:0d7a), or nil
--- if there is none. `computer` is quest_assign(1): shorter ranges.
-- Read from raw disassembly; Ghidra mis-disassembles the switch.
function quest.pickTarget(g, side, type, h, computer)
  if type == quest.SLAY_HERO then
    -- another side's hero, counted from the top of the army table down
    local heroes = {}
    for i = #g.armies, 1, -1 do
      local a = g.armies[i]
      if a.type == armytype.HERO and a.owner ~= side.index then
        heroes[#heroes + 1] = a
      end
    end
    if #heroes == 0 then return nil end
    return heroes[g.rng:dice(1, #heroes, 0)]

  elseif type == quest.RETRIEVE_ITEM then
    -- an ordinary item still hidden in a ruin, less than 50 (30) away each way
    local site = require("warlords.site")
    local range = computer and quest.AI_ITEM_RANGE or quest.ITEM_RANGE
    local choices = {}
    for _, it in ipairs(g.map.items) do
      if it.index <= 21 and it.status == 2 and not site.itemReserved(it) then
        local at
        for _, s in ipairs(g.map.sites) do
          if s.content == site.ITEM and s.item == it.index then at = s break end
        end
        if at and math.abs(h.x - at.x) < range and math.abs(h.y - at.y) < range then
          choices[#choices + 1] = { item = it, site = at }
        end
      end
    end
    if #choices == 0 then return nil end
    local pick = choices[g.rng:dice(1, #choices, 0)]
    -- the priests show the side where it lies
    if not site.shownTo(pick.site, side) then
      pick.site.revealed = (pick.site.revealed or 0) + math.floor(2 ^ side.index)
    end
    require("warlords.game").reveal(g, side.index, pick.site.x, pick.site.y, false)
    return pick.item

  elseif type == quest.SLAY_TYPE then
    -- up to five magical types, one of which some other army on the map is
    local heroMod = require("warlords.hero")
    for _ = 1, 5 do
      local want = heroMod.magicalType(g)
      for i = #g.armies, 1, -1 do
        local a = g.armies[i]
        if a.type == want.id and a.owner ~= side.index and not a.transit and a.x then
          return want
        end
      end
    end
    return nil

  elseif type == quest.SLAUGHTER then
    if g.won then return nil end
    for _ = 1, 200 do
      local o = g.map.sides[g.rng:dice(1, 8, 0)]
      if o and o.index ~= side.index and o.inUse and o.alive then return o end
    end
    return nil

  elseif type == quest.OCCUPY or type == quest.RAZE then
    -- any standing city not the side's, near the hero -- after a hundred
    -- tries, anywhere
    if g.won then return nil end
    local range = type == quest.RAZE and quest.RAZE_RANGE
                  or (computer and quest.AI_CITY_RANGE or quest.CITY_RANGE)
    for t = 0, 199 do
      local c = g.map.cities[g.rng:dice(1, #g.map.cities, 0)]
      if c and c.ownerIndex ~= side.index and not c.razed
         and (dist(h.x, h.y, c.x, c.y) <= range or t > 99) then
        return c
      end
    end
    return nil

  elseif type == quest.PILLAGE_GOLD then
    return true                                  -- no target, just a count
  end
  return nil
end

--- The type of a quest's `n`th try (from 1). A human's is rolled from the
--- table; a computer's (quest_assign(1)) is raze or occupy first, then
--- slaughter, then pillage gold.
local function questType(g, n, computer)
  if not computer then return quest.TYPE_TABLE[g.rng:dice(1, 10, 0)] end
  if n == 1 then return g.rng:dice(1, 5, -1) == 0 and quest.RAZE or quest.OCCUPY end
  if n == 2 then return quest.SLAUGHTER end
  return quest.PILLAGE_GOLD
end

--- Take a quest at a temple. Returns the quest, or nil if none could be made
--- (quest_assign, Ghidra 4976:0d7a): tries until a quest finds a target --
--- pillage gold always does. `computer` for a computer player's (5ad0:15c3).
function quest.assign(g, side, h, computer)
  if side.quest then return nil end              -- one at a time
  if g.map.options.quests == 0 then return nil end
  local history = require("warlords.history")
  history.deed(g, side, history.QUEST_GIVEN, 0, 0, h and h.name)   -- 4976:0dae
  local n = 0
  while true do
    n = n + 1
    local type = questType(g, n, computer)
    local target = quest.pickTarget(g, side, type, h, computer)
    if target then
      local q = { type = type, hero = h, target = target, done = 0,
                  targetKind = quest.TARGET_KIND[type] }
      if type == quest.SLAUGHTER then q.required = g.rng:dice(1, 12, 10)
      elseif type == quest.PILLAGE_GOLD then q.required = g.rng:dice(3, 300, 500) end
      side.quest = q
      return q
    end
  end
end

--- A line of prose for a quest.
function quest.describe(q)
  local text = quest.DESCRIPTIONS[q.type]
  if q.required then return text:format(q.required) end
  if q.type == quest.OCCUPY or q.type == quest.RAZE then
    return ("%s: %s"):format(text, q.target.name)
  end
  if q.type == quest.SLAY_TYPE then
    return ("%s: %s"):format(text, q.target.name)
  end
  return text
end

--------------------------------------------------------------- checking it off

local function heroInStack(q, stack)
  for _, a in ipairs(stack) do if a == q.hero then return true end end
  return false
end

-- The quest is over. A failure carries the STRING.DAT group quest_check
-- tells a human it with (0x20-0x2a, two lines in the message box); a human
-- side keeps the outcome as `questNews` for the front end to show -- the
-- computer's quests end unannounced (is_computer_turn).
local function finish(g, side, reason, why)
  local q = side.quest
  side.quest = nil
  if not q then return nil end
  local out
  if reason == "done" then
    local history = require("warlords.history")
    history.deed(g, side, history.QUEST_DONE, 0, 0, q.hero and q.hero.name)   -- 4976:1da8
    require("warlords.hero").addExperience(g, q.hero, quest.EXPERIENCE)
    out = { quest = q, reward = quest.reward(g, side, q) }
  else
    out = { quest = q, failed = reason, why = why }
  end
  if not side.computer then side.questNews = out end
  return out
end

--- Tell the quest what just happened. `event` is one of "battle", "item",
--- "pillage", "occupy", "raze" or "turn". quest_check, Ghidra 4976:1ded.
function quest.event(g, side, event, data)
  local q = side.quest
  if not q then return nil end
  data = data or {}

  if event == "turn" then
    -- the hero must still be alive and still ours
    local alive = false
    for _, a in ipairs(g.armies) do
      if a == q.hero and a.owner == side.index then alive = true end
    end
    if not alive then return finish(g, side, "the hero is lost", 0x20) end
    if q.type == quest.OCCUPY or q.type == quest.RAZE then
      -- razed by another hand, or taken without being razed
      if q.target.razed then
        return finish(g, side, "the city is ruins", 0x21)
      end
      if q.target.ownerIndex == side.index then
        -- taken, but not by the quest hero
        return finish(g, side, "another took the city", 0x2a)
      end
    elseif q.type == quest.SLAUGHTER then
      if not q.target.alive then return finish(g, side, "that side is gone", 0x27) end
    elseif q.type == quest.SLAY_HERO then
      local still = false
      for _, a in ipairs(g.armies) do if a == q.target then still = true end end
      if not still then return finish(g, side, "the quarry is gone", 0x29) end
    elseif q.type == quest.RETRIEVE_ITEM then
      if q.target.status == 0 then return finish(g, side, "the item is lost", 0x28) end
    end
    return nil
  end

  if event == "battle" then
    if not heroInStack(q, data.stack or {}) then return nil end
    if q.type == quest.SLAY_HERO then
      for _, d in ipairs(data.killed or {}) do
        if d == q.target then return finish(g, side, "done") end
      end
    elseif q.type == quest.SLAY_TYPE then
      for _, d in ipairs(data.killed or {}) do
        if d.type == q.target.id then return finish(g, side, "done") end
      end
    elseif q.type == quest.SLAUGHTER then
      for _, d in ipairs(data.killed or {}) do
        if d.owner == q.target.index then q.done = q.done + 1 end
      end
      if q.done >= q.required then return finish(g, side, "done") end
    end

  elseif event == "item" then
    if q.type == quest.RETRIEVE_ITEM and q.hero.items then
      for i, it in ipairs(q.hero.items) do
        if it == q.target then
          table.remove(q.hero.items, i)          -- the priests take it away
          it.status = 0
          return finish(g, side, "done")
        end
      end
    end

  elseif event == "pillage" then
    if q.type == quest.PILLAGE_GOLD and heroInStack(q, data.stack or {}) then
      q.done = q.done + (data.gold or 0)
      if q.done >= q.required then return finish(g, side, "done") end
    elseif (q.type == quest.OCCUPY or q.type == quest.RAZE)
           and data.city == q.target then
      return finish(g, side, "that was not to pillage", 0x24)
    end

  elseif event == "occupy" then
    if q.type == quest.OCCUPY and data.city == q.target then
      if heroInStack(q, data.stack or {}) then return finish(g, side, "done") end
      return finish(g, side, "the hero was not there", 0x22)
    elseif q.type == quest.RAZE and data.city == q.target then
      return finish(g, side, "the quest was to raze it", 0x23)
    end

  elseif event == "raze" then
    if q.type == quest.RAZE and data.city == q.target then
      if heroInStack(q, data.stack or {}) then return finish(g, side, "done") end
      return finish(g, side, "the hero was not there", 0x25)
    elseif q.type == quest.OCCUPY and data.city == q.target then
      return finish(g, side, "the quest was to keep it", 0x26)
    end
  end
  return nil
end

--------------------------------------------------------------------- reward

--- Choose and give the reward (quest_choose_reward, Ghidra 4976:1909, and
--- quest_give_reward, 4976:1c02).
function quest.reward(g, side, q)
  local gameMod = require("warlords.game")
  local site = require("warlords.site")
  local cities = #gameMod.sideCities(g, side)
  local h = q and q.hero

  -- Archons, on the quest hero's tile (create_ally_army)
  local function allies(n)
    local heroMod = require("warlords.hero")
    local type = g.types.byId[quest.REWARD_ALLY]
    local joined = {}
    for _ = 1, n do
      local a = h and heroMod.placeAlly(g, type, h.x, h.y, side)
      if a then joined[#joined + 1] = a end
    end
    return { kind = "allies", armies = joined, type = type, count = n }
  end

  local function gold()
    local n = g.rng:dice(2, 1000, 1000)
    side.gold = side.gold + n
    return { kind = "gold", gold = n }
  end

  if cities < 10 and g.turn > 15 then return allies(g.rng:dice(1, 3, 5)) end
  if side.gold < 100 then return gold() end

  -- an unused reserved item -- of the standards only the side's own
  local unused = {}
  for _, it in ipairs(g.map.items) do
    if it.index <= 21 and (it.status or 0) == 0 and site.itemReserved(it)
       and (it.index > 7 or it.index == side.index) then
      unused[#unused + 1] = it
    end
  end
  -- a rich site the side does not know of yet, not emptied
  local unknown = {}
  for _, s in ipairs(g.map.sites) do
    if s.rich and not site.shownTo(s, side) and s.content ~= site.EMPTY then
      unknown[#unknown + 1] = s
    end
  end
  if #unused > 0 and g.rng:dice(1, 3, 0) == 1 then
    local it = unused[g.rng:dice(1, #unused, 0)]
    if h then
      h.items = h.items or {}
      h.items[#h.items + 1] = it
      it.status = 3
    end
    return { kind = "item", item = it }
  end
  if #unused == 0 and #unknown > 0 and g.rng:dice(1, 3, 0) < 3 then
    -- the nearest to the hero, the first of equals
    local best, bestD = nil, 10000
    for _, s in ipairs(unknown) do
      local d = h and math.max(math.abs(s.x - h.x), math.abs(s.y - h.y)) or 0
      if d < bestD then best, bestD = s, d end
    end
    best.revealed = (best.revealed or 0) + math.floor(2 ^ side.index)
    if not side.computer or side.observe then gameMod.reveal(g, side.index, best.x, best.y, false) end
    return { kind = "revealed", site = best }
  end

  if g.rng:dice(1, 2, 0) == 1 then return allies(g.rng:dice(1, 3, 2)) end
  return gold()
end

return quest
