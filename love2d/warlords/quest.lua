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

quest.ITEM_RANGE, quest.CITY_RANGE = 50, 60

-- what a quest's target *is*, so it can be saved and restored by reference
quest.TARGET_KIND = {
  [0] = "army", "item", "armytype", "side", "city", "city", "none",
}
quest.EXPERIENCE = 10

--------------------------------------------------------------------- targets

local function otherSides(g, side)
  local out = {}
  for _, s in ipairs(g.sides) do
    if s.alive and s.index ~= side.index then out[#out + 1] = s end
  end
  return out
end

--- Pick a target for a quest of this type, or nil if there is none.
-- Read from raw disassembly; Ghidra mis-disassembles the switch.
function quest.pickTarget(g, side, type, h)
  if type == quest.SLAY_HERO then
    local heroes = {}
    for _, a in ipairs(g.armies) do
      if a.type == armytype.HERO and a.owner ~= nil and a.owner ~= side.index then
        heroes[#heroes + 1] = a
      end
    end
    return g.rng:pick(heroes)

  elseif type == quest.RETRIEVE_ITEM then
    local site = require("warlords.site")
    local choices = {}
    for _, s in ipairs(g.map.sites) do
      if s.content == site.ITEM and not s.searched then
        local item
        for _, it in ipairs(g.map.items) do if it.index == s.item then item = it end end
        if item and not site.itemReserved(item)
           and math.abs(s.x - h.x) <= quest.ITEM_RANGE
           and math.abs(s.y - h.y) <= quest.ITEM_RANGE then
          choices[#choices + 1] = { item = item, site = s }
        end
      end
    end
    local pick = g.rng:pick(choices)
    if not pick then return nil end
    pick.site.revealed = 0                       -- the priests show where it lies
    return pick.item

  elseif type == quest.SLAY_TYPE then
    for _ = 1, 5 do                              -- up to five tries
      local magical = {}
      for _, a in ipairs(g.types.list) do
        if (a.bonus[48] or 0) ~= 0 then magical[#magical + 1] = a end
      end
      local want = g.rng:pick(magical)
      if want then
        for _, a in ipairs(g.armies) do
          if a.type == want.id and a.owner ~= nil and a.owner ~= side.index then
            return want
          end
        end
      end
    end
    return nil

  elseif type == quest.SLAUGHTER then
    return g.rng:pick(otherSides(g, side))

  elseif type == quest.OCCUPY or type == quest.RAZE then
    local choices, any = {}, {}
    for _, c in ipairs(g.map.cities) do
      if c.ownerIndex ~= side.index and not c.razed then
        any[#any + 1] = c
        local d = math.max(math.abs(c.x - h.x), math.abs(c.y - h.y))
        if d <= quest.CITY_RANGE then choices[#choices + 1] = c end
      end
    end
    return g.rng:pick(choices) or g.rng:pick(any)

  elseif type == quest.PILLAGE_GOLD then
    return true                                  -- no target, just a count
  end
  return nil
end

--- Take a quest at a temple. Returns the quest, or nil if none could be made.
-- quest_assign, Ghidra 4976:0d7a.
function quest.assign(g, side, h)
  if side.quest then return nil end              -- one at a time
  if g.map.options.quests == 0 then return nil end

  for _ = 1, 20 do                               -- reroll the type if it has no target
    local type = quest.TYPE_TABLE[g.rng:dice(1, 10, 0)]
    -- types 3, 4 and 5 are skipped once the game has been won
    if not (g.won and (type == quest.SLAUGHTER or type == quest.OCCUPY
                       or type == quest.RAZE)) then
      local target = quest.pickTarget(g, side, type, h)
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
  return nil
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

local function finish(g, side, reason)
  local q = side.quest
  side.quest = nil
  if not q then return nil end
  if reason == "done" then
    require("warlords.hero").addExperience(g, q.hero, quest.EXPERIENCE)
    return { quest = q, reward = quest.reward(g, side, q) }
  end
  return { quest = q, failed = reason }
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
    if not alive then return finish(g, side, "the hero is lost") end
    if q.type == quest.OCCUPY or q.type == quest.RAZE then
      if q.target.razed and q.type == quest.OCCUPY then
        return finish(g, side, "the city is ruins")
      end
      if q.target.ownerIndex == side.index and q.type == quest.OCCUPY then
        -- taken, but not by the quest hero
        return finish(g, side, "another took the city")
      end
    elseif q.type == quest.SLAUGHTER then
      if not q.target.alive then return finish(g, side, "that side is gone") end
    elseif q.type == quest.SLAY_HERO then
      local still = false
      for _, a in ipairs(g.armies) do if a == q.target then still = true end end
      if not still then return finish(g, side, "the quarry is gone") end
    elseif q.type == quest.RETRIEVE_ITEM then
      if q.target.status == 0 then return finish(g, side, "the item is lost") end
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
      return finish(g, side, "that was not to pillage")
    end

  elseif event == "occupy" then
    if q.type == quest.OCCUPY and data.city == q.target then
      if heroInStack(q, data.stack or {}) then return finish(g, side, "done") end
      return finish(g, side, "the hero was not there")
    elseif q.type == quest.RAZE and data.city == q.target then
      return finish(g, side, "the quest was to raze it")
    end

  elseif event == "raze" then
    if q.type == quest.RAZE and data.city == q.target then
      if heroInStack(q, data.stack or {}) then return finish(g, side, "done") end
      return finish(g, side, "the hero was not there")
    elseif q.type == quest.OCCUPY and data.city == q.target then
      return finish(g, side, "the quest was to keep it")
    end
  end
  return nil
end

--------------------------------------------------------------------- reward

--- Choose and give the reward. quest_choose_reward, Ghidra 4976:1909.
function quest.reward(g, side, q)
  local gameMod = require("warlords.game")
  local cities = #gameMod.sideCities(g, side)

  local function allies(n)
    local heroMod = require("warlords.hero")
    local type = heroMod.allies(g)               -- one random magical type
    local joined = {}
    local h = q and q.hero
    local home = g.map.cities[(h and h.homeCity or 0) + 1] or side.capital
    for _ = 1, n do
      local x, y = gameMod.freeTileIn(g, home, true)
      if x then
        local a = {
          x = x, y = y, owner = side.index, type = type.id, name = type.name,
          strength = type.strength, maxMoves = type.move, moves = 0,
          upkeep = type.cost // 2, homeCity = home.index,
        }
        g.armies[#g.armies + 1] = a
        joined[#joined + 1] = a
      end
    end
    return { kind = "allies", armies = joined, type = type }
  end

  local function gold()
    local n = g.rng:dice(2, 1000, 1000)
    side.gold = side.gold + n
    return { kind = "gold", gold = n }
  end

  if cities < 10 and g.turn > 15 then return allies(g.rng:dice(1, 3, 5)) end
  if side.gold < 100 then return gold() end

  -- an unclaimed magic item, one time in three
  local unclaimed = {}
  for _, it in ipairs(g.map.items) do
    if (it.status or 0) == 0 then unclaimed[#unclaimed + 1] = it end
  end
  if #unclaimed > 0 then
    if g.rng:dice(1, 3, 0) == 1 then
      local it = g.rng:pick(unclaimed)
      local h = q and q.hero
      if h then
        h.items = h.items or {}
        h.items[#h.items + 1] = it
        it.status = 3
      end
      return { kind = "item", item = it }
    end
  else
    -- otherwise the priests may point at a rich site, two times in three
    local hidden = {}
    for _, s in ipairs(g.map.sites) do
      if s.rich and not s.searched then hidden[#hidden + 1] = s end
    end
    if #hidden > 0 and g.rng:dice(1, 3, 0) <= 2 then
      local s = g.rng:pick(hidden)
      s.revealed = 0
      return { kind = "revealed", site = s }
    end
  end

  if g.rng:dice(1, 2, 0) == 1 then return allies(g.rng:dice(1, 3, 2)) end
  return gold()
end

return quest
