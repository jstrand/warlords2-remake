-- The computer players.
--
-- A port of WARLORD2.EXE's AI (docs/re/ai.md). Every side has a block of AI
-- data (warlords/ai/core.lua) filled at game start from its level and its
-- character card (docs/formats/crd.md); each computer turn runs the
-- original's nineteen phases in the original's order (ai_turn, 5db9:0000).
--
--   warlords/ai/core.lua      data, neighbours, the selection, flood, battles, walk
--   warlords/ai/cities.lua    evaluate, garrisons, neutrals, production, vectoring
--   warlords/ai/groups.lua    the assault groups and picking an enemy
--   warlords/ai/moves.lua     standing orders, rescue, explorers, hero parties
--   warlords/ai/heroes.lua    the hero phase
--   warlords/ai/diplomacy.lua proposals

local aicard = require("warlords.aicard")
local core   = require("warlords.ai.core")

local ai = {}

ai.core = core

------------------------------------------------------------------ game start

--- The level's built-in settings (59bf:0d7b). Only a side with no card on
--- disk -- or a human, whose AI data nothing reads -- keeps them.
local function levelDefaults(g, d, level)
  local r = g.rng
  if level == 0 then
    d.rebuildLimit, d.cautious = 30, 1
    d.dieHuman, d.dieWarlord, d.dieLord = r:dice(1, 4, 0), r:dice(1, 4, 0), r:dice(1, 4, 0)
    d.maxGroups, d.solidarity = 1, 0
    d.rebuildType, d.rebuildTypeRich = 3, 3
    d.raze, d.sack, d.pillage, d.perCity, d.bonusHuman = 0, 0, 0, 0, 0
    d.bonusWarlord, d.bonusLord, d.bonusKnight, d.poor, d.early = 0, 0, 0, 0, 0
    d.humanShare = 80
  elseif level == 1 then
    d.rebuildLimit, d.cautious = 20, 0
    d.dieHuman, d.dieWarlord, d.dieKnight = r:dice(1, 4, 0), r:dice(1, 4, 0), r:dice(1, 8, 0)
    d.bold = true
    d.solidarity, d.maxGroups = 0, 2
    d.rebuildType, d.rebuildTypeRich = 6, 18
    d.raze, d.sack, d.pillage, d.perCity, d.bonusHuman = 5, 10, 20, 1, 5
    d.bonusKnight, d.bonusLord, d.bonusWarlord, d.poor, d.early = 0, 0, 0, 0, 0
    d.humanShare = 35
  else
    d.rebuildLimit, d.cautious = 10, 0
    d.dieHuman, d.dieLord, d.dieKnight = r:dice(1, 10, 0), r:dice(1, 8, 0), r:dice(1, 6, 0)
    d.maxGroups, d.solidarity = 4, 1
    d.bold = true
    d.rebuildType, d.rebuildTypeRich = 7, 0
    d.raze, d.sack, d.pillage, d.perCity, d.bonusHuman = 5, 10, 20, 5, 50
    d.bonusKnight, d.bonusLord, d.bonusWarlord, d.poor, d.early = 0, 0, 0, 50, 1
    d.humanShare = 35
  end
end

--- Fill a computer side's AI data from its card (59bf:0d7b), dice and all.
local function fromCard(g, side, d, card)
  local r = g.rng
  d.dieHuman = r:dice(1, card.dieHuman, 0)
  d.dieLord = r:dice(1, card.dieLord, 0)
  d.dieKnight = r:dice(1, card.dieKnight, 0)
  d.dieWarlord = r:dice(1, card.dieWarlord, 0)
  d.maxGroups = math.max(0, math.min(core.MAX_GROUPS, card.groups))
  d.solidarity = card.solidarity
  if card.bold ~= 0 then d.bold = true end
  d.cautious = card.cautious
  d.rebuildType, d.rebuildTypeRich = card.rebuildType, card.rebuildTypeRich
  d.rebuildLimit = card.rebuildLimit
  d.raze, d.sack, d.pillage, d.perCity = card.raze, card.sack, card.pillage, card.perCity
  d.bonusHuman, d.bonusWarlord = card.bonusHuman, card.bonusWarlord
  d.bonusLord, d.bonusKnight = card.bonusLord, card.bonusKnight
  d.poor, d.early = card.poor, card.early
  d.humanShare = card.humanShare * 10 + r:dice(1, 10, 0)
  -- the card's fight order replaces the side's
  g.map.fightOrder[side.index] = card.fightOrder
end

--- A side's AI data at game start (ai_init_side, 59bf:084d).
function ai.initSide(g, side)
  local d = core.newData(g)
  side.ai = d
  local quick = g.map.options.quickStart ~= 0
  for _, c in ipairs(g.map.cities) do
    d.roles[c.index] = quick and core.WEAK or 0
    d.held[c.index] = 0
    d.flags[c.index] = g.map.options.hiddenMap ~= 0 and core.CF_UNSEEN or 0
  end
  local level = 2
  if side.computer and (side.level == 0 or side.level == 1) then level = side.level end
  levelDefaults(g, d, level)
  if side.inUse and side.computer then
    local card = g.dataDir and aicard.load(g.dataDir, level, side.card or 0)
    if card then fromCard(g, side, d, card) end
  end
  return d
end

--- Split the cities no side starts with among the computer players, for the
--- AI to think of as its own ground (623c:010b). Each side's capital is its
--- own; then, round the computer sides in turn, each takes the city nearest
--- its last one (or, half the time, nearest its capital) that is nobody's.
--- With no computer player, every side but one picked at random shares out.
local function shareOutCities(g)
  for _, c in ipairs(g.map.cities) do c.claim = c.ownerIndex or core.NEUTRAL end
  local shares, humans, computers = {}, 0, 0
  local from = {}
  for i = 0, 7 do
    local s = g.map.sides[i + 1]
    shares[i] = false
    if s and s.inUse then
      if s.computer then shares[i], computers = true, computers + 1
      else humans = humans + 1 end
      from[i] = { s.capX, s.capY }
      local c = core.cityAt(g, s.capX, s.capY)
      if c then c.claim = i end
    end
  end
  if computers == 0 and humans ~= 0 then
    local best, bestRoll
    for i = 7, 0, -1 do
      local s = g.map.sides[i + 1]
      if s and s.inUse then
        shares[i] = true
        local roll = g.rng:dice(1, 100, 0)
        if best == nil or bestRoll < roll then best, bestRoll = i, roll end
      end
    end
    if best then shares[best] = false end
  end
  local turn
  for i = 7, 0, -1 do if shares[i] then turn = i break end end
  if not turn then return end
  while true do
    local x, y = from[turn][1], from[turn][2]
    local pick, bestD
    for i = #g.map.cities, 1, -1 do
      local c = g.map.cities[i]
      if core.standing(c) and c.claim == core.NEUTRAL then
        local d = core.dist(x, y, c.x, c.y)
        if not bestD or d < bestD then pick, bestD = c, d end
      end
    end
    if not pick then break end
    pick.claim = turn
    local s = g.map.sides[turn + 1]
    if g.rng:dice(1, 10, -1) < 5 then
      from[turn] = { s.capX, s.capY }
    else
      from[turn] = { pick.x, pick.y }
    end
    repeat turn = (turn + 1) % 8 until shares[turn]
  end
end

--- Set the computer players up for a new game: every side's AI data (all
--- eight have it in the original), the cities' claims, and the diplomatic
--- scores -- 1d8 each, and 400 more for a human when *I am the Greatest* is
--- on (79fa:0000).
function ai.startGame(g)
  for _, s in ipairs(g.map.sides) do ai.initSide(g, s) end
  shareOutCities(g)
  for _, s in ipairs(g.map.sides) do
    s.diploScore = g.rng:dice(1, 8, 0)
    if g.greatest and not s.computer then s.diploScore = s.diploScore + 400 end
  end
end

------------------------------------------------------------------ the turn

--- ai_turn_setup (5db9:0386): the side's solidarity is put where the others
--- can see it, the explorer roles lapse, and each city's "cleaned" mark goes.
local function turnSetup(g, side)
  local d = core.data(g, side)
  side.aiSolidarity = d.solidarity
  local quick = g.map.options.quickStart ~= 0 and g.map.options.hiddenMap ~= 0
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if c.ownerIndex == side.index then
      local r = core.role(d, c)
      if r == core.EXPLORER or r == core.EXPLORER2 then
        core.setRole(d, c, g.turn < 5 and core.WEAK or core.STOP)
      end
      if quick then
        if g.turn == 1 then core.setRole(d, c, core.EXPLORER)
        elseif g.turn < 3 then core.setRole(d, c, core.EXPLORER2) end
      end
      core.clearCflag(d, c, core.CF_CLEANED)
    end
  end
end

--- Play one computer turn: ai_turn (5db9:0000), phase by phase. `yield`, if
--- given, is called between phases so a front end can breathe.
function ai.playTurn(g, side, yield)
  local cities = require("warlords.ai.cities")
  local groups = require("warlords.ai.groups")
  local moves  = require("warlords.ai.moves")
  local heroes = require("warlords.ai.heroes")
  local diplo  = require("warlords.ai.diplomacy")
  local pause = yield or function() end
  local hidden = g.map.options.hiddenMap ~= 0

  -- the computer always hires an offered hero it can afford (hero_offer_check
  -- priced it; the side pays on recruiting)
  if side.heroOffer and side.gold >= (side.heroOffer.price or 0) then
    require("warlords.hero").recruit(g, side, side.heroOffer)
    side.heroOffer = nil
  end

  -- every army starts the phases unmoved
  for _, a in ipairs(g.armies) do
    if a.owner == side.index then a.aiMoved = nil end
  end

  turnSetup(g, side)
  diplo.phase(g, side);               pause("diplomacy")
  heroes.phase(g, side);              pause("move hero")
  if hidden then moves.search(g, side) end
  pause("move search")
  moves.heroParties(g, side);         pause("move explore")
  groups.assault(g, side);            pause("assault")
  moves.move(g, side);                pause("move #1")
  moves.rescue(g, side);              pause("rescue")
  cities.evaluate(g, side, side.index); pause("evaluate")
  cities.clean(g, side);              pause("clean city")
  cities.neutral(g, side);            pause("neutral")
  moves.move(g, side);                pause("move #2")
  cities.quickAttack(g, side)
  if hidden then cities.updateHide(g, side) end
  groups.assaultXX(g, side);          pause("assault XX")
  moves.specials(g, side);            pause("specials")
  cities.rebuild(g, side);            pause("rebuilding")
  moves.lastRescue(g, side);          pause("last rescue")
  cities.production(g, side);         pause("production")
  cities.vectoring(g, side);          pause("vectoring")
end

------------------------------------------------------------------ hooks

--- Every walk a computer stack makes is reported here once it is made, so a
--- front end can show it: `ai.onWalk(g, stack, result)`, if one is set.
function ai.walked(g, stack, r)
  if ai.onWalk and r and (r.steps or 0) > 0 then ai.onWalk(g, stack, r) end
end

--- Where a hired hero appears for a computer (ai_hero_city, 5db9:0919): the
--- city whose role scores best -- a rally city 1d100+100, one taking a
--- neutral 1d100+50, one next to a neutral 1d100 -- or `default`.
function ai.heroCity(g, side, default)
  local d = core.data(g, side)
  local best, bestScore = default, 0
  for i = #g.map.cities, 1, -1 do
    local c = g.map.cities[i]
    if c.ownerIndex == side.index then
      local r, score = core.role(d, c), 0
      if r == core.RALLY then score = g.rng:dice(1, 100, 100)
      elseif r == core.TAKING_NEUTRAL then score = g.rng:dice(1, 100, 50)
      elseif r == core.NEAR_NEUTRAL then score = g.rng:dice(1, 100, 0) end
      if bestScore < score then best, bestScore = c, score end
    end
  end
  return best
end

--- A computer's quest hero has taken a city (5e97:038d, from the capture,
--- 67cc:124a). When it is the quest's city the quest is done: a city to be
--- razed is razed -- sacked if it is worth 400 -- and the side stops
--- steering clear of it. Returns true when it razed or sacked, so the
--- capture is not also taken for an occupation.
function ai.questCapture(g, side, c, stack)
  local quest = require("warlords.quest")
  local q = side.quest
  if g.map.options.quests == 0 or not q or q.target ~= c then return false end
  if q.type ~= quest.OCCUPY and q.type ~= quest.RAZE then return false end
  local withHero = false
  for _, a in ipairs(stack or {}) do if a == q.hero then withHero = true end end
  if not withHero then return false end
  local d = core.data(g, side)
  d.questsDone = d.questsDone + 1
  d.questCity = nil
  if q.type == quest.RAZE then
    require("warlords.ai.groups").raze(g, side, c, true, core.select(g, stack))
    return true
  end
  return false
end

--- A battle has been fought on a side's tile (ai_record_battle, 5db9:09d7):
--- the defending side's data remembers who attacked it and what it cost.
function ai.recordBattle(g, defender, attacker, x, y, heroesLost, armiesLost, allLost, cityTile)
  if defender == nil or attacker == nil or defender == core.NEUTRAL then return end
  local d = core.data(g, defender)
  d.heroesKilled[attacker] = d.heroesKilled[attacker] + heroesLost
  d.armiesKilled[attacker] = d.armiesKilled[attacker] + armiesLost
  d.battles[attacker] = d.battles[attacker] + 1
  if allLost then d.lost[attacker] = d.lost[attacker] + 1 end
  if cityTile then
    d.cityBattles[attacker] = d.cityBattles[attacker] + 1
    if allLost then d.citiesLost[attacker] = d.citiesLost[attacker] + 1 end
  end
end

return ai
