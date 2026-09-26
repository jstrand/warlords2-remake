-- Saving and loading a game in progress.
--
-- This is the *engine's* save format, not the original's: a plain Lua table
-- written as source, which `load` reads back. The original's SAVE\*.SAV layout
-- is decoded in docs/formats/save.md if reading those is ever wanted; it holds
-- a memory image that only makes sense to the DOS build.
--
-- Only what cannot be recomputed is written. Everything derived from the data
-- files -- the map, the army types, the terrain -- is reloaded from them, so a
-- save is small and survives changes to the engine's derived state.

local game = require("warlords.game")

local save = {}

save.VERSION = 1

--------------------------------------------------------------------- writing

local function quote(s)
  return ("%q"):format(s)
end

-- A table may have holes -- the diplomacy matrix is keyed 0..63 and mostly
-- empty -- so the array part is whatever ipairs actually reaches, and the
-- length operator is never trusted.
local function serialise(v, out)
  local t = type(v)
  if t == "number" then
    out[#out + 1] = (v % 1 == 0) and ("%d"):format(v) or ("%.17g"):format(v)
  elseif t == "string" then
    out[#out + 1] = quote(v)
  elseif t == "boolean" then
    out[#out + 1] = tostring(v)
  elseif t == "nil" then
    out[#out + 1] = "nil"
  elseif t == "table" then
    out[#out + 1] = "{"
    local written, n = 0, 0
    for _, item in ipairs(v) do
      if written > 0 then out[#out + 1] = "," end
      serialise(item, out)
      written, n = written + 1, n + 1
    end

    local keys = {}
    for k in pairs(v) do
      if not (type(k) == "number" and k % 1 == 0 and k >= 1 and k <= n) then
        keys[#keys + 1] = k
      end
    end
    table.sort(keys, function(a, b)
      if type(a) == type(b) then return tostring(a) < tostring(b) end
      return type(a) < type(b)
    end)

    for _, k in ipairs(keys) do
      if written > 0 then out[#out + 1] = "," end
      if type(k) == "string" and k:match("^[%a_][%w_]*$") then
        out[#out + 1] = k .. "="
      else
        out[#out + 1] = "["
        serialise(k, out)
        out[#out + 1] = "]="
      end
      serialise(v[k], out)
      written = written + 1
    end
    out[#out + 1] = "}"
  else
    error("cannot save a " .. t)
  end
end

--- Turn a game into a string. Armies are written with an id so that the
--- references between them (a quest's hero, a city's vector) survive.
-- A side's AI data is plain numbers and tables, but for the armies its
-- assault groups have staged: those go by id.
local function saveAI(d, ids)
  if not d then return nil end
  local out = {}
  for k, v in pairs(d) do out[k] = v end
  out.groups = {}
  for i, grp in ipairs(d.groups or {}) do
    local copy = {}
    for k, v in pairs(grp) do copy[k] = v end
    copy.staged = {}
    for s = 1, 4 do copy.staged[s] = grp.staged[s] and ids[grp.staged[s]] or nil end
    out.groups[i] = copy
  end
  return out
end

function save.encode(g)
  local ids = {}
  for i, a in ipairs(g.armies) do ids[a] = i end

  local armies = {}
  for i, a in ipairs(g.armies) do
    local items = nil
    if a.items and #a.items > 0 then
      items = {}
      for _, it in ipairs(a.items) do items[#items + 1] = it.index end
    end
    armies[i] = {
      x = a.x, y = a.y, owner = a.owner, type = a.type, name = a.name,
      female = a.female or nil,
      strength = a.strength, moves = a.moves, maxMoves = a.maxMoves,
      upkeep = a.upkeep, homeCity = a.homeCity, atSea = a.atSea or nil,
      -- the army's own record carries which group it moves with (+17 in the
      -- original's, docs/re/ui.md > The army slots), so a grouped stack is
      -- still grouped when the game is picked up again
      group = (a.group or 0) ~= 0 and a.group or nil,
      -- where it has been told to go, the army record's own move target
      target = a.target and { x = a.target.x, y = a.target.y } or nil,
      -- dug in, and so out of the army cycle until it is picked up again.
      -- "Done for this turn" is not saved: the turn's start clears it.
      fortified = a.fortified or nil,
      level = a.level, experience = a.experience, title = a.title,
      items = items, blessings = a.blessings,
      transit = a.transit and { turns = a.transit.turns, dest = a.transit.dest } or nil,
      returning = a.returning or nil,
      -- the computer players' standing order (record +14/+15) and marks
      aiOrder = (a.aiOrder or 0) ~= 0 and a.aiOrder or nil, aiDest = a.aiDest,
      aiGroup = (a.aiGroup or 0) ~= 0 and a.aiGroup or nil,
      aiExplore = a.aiExplore or nil, aiNeutral = a.aiNeutral or nil,
      aiParty = a.aiParty or nil,
    }
  end

  local sides = {}
  for _, s in ipairs(g.sides) do
    sides[#sides + 1] = {
      index = s.index, gold = s.gold, alive = s.alive, computer = s.computer,
      level = s.level, enhanced = s.enhanced, observe = s.observe or nil,
      diploScore = s.diploScore,
      income = s.income, upkeepTotal = s.upkeepTotal, produced = s.produced,
      ai = saveAI(s.ai, ids), aiSolidarity = s.aiSolidarity, card = s.card,
      -- the advisor's marks, .SCN 0x100 and 0x108 (warlords/cues.lua)
      advisor = s.advisor,
      quest = s.quest and {
        type = s.quest.type, hero = ids[s.quest.hero], done = s.quest.done,
        required = s.quest.required, targetKind = s.quest.targetKind,
        -- the target is saved by reference, according to what kind it is
        target = (s.quest.targetKind == "army" and ids[s.quest.target])
                 or (s.quest.targetKind == "armytype" and s.quest.target.id)
                 or (s.quest.targetKind ~= "none" and s.quest.target.index)
                 or nil,
      } or nil,
    }
  end

  local cities = {}
  for _, c in ipairs(g.map.cities) do
    cities[#cities + 1] = {
      index = c.index, name = c.name, ownerIndex = c.ownerIndex, producing = c.producing,
      countdown = c.countdown, vectorTo = c.vectorTo, razed = c.razed or nil,
      defence = c.defence, income = c.income, claim = c.claim,
      razedBy = c.razedBy, slots = c.slots,
    }
  end

  local sites = {}
  for _, s in ipairs(g.map.sites) do
    sites[#sites + 1] = {
      index = s.index, content = s.content, item = s.item, guardian = s.guardian,
      allyType = s.allyType, rich = s.rich or nil, revealed = s.revealed,
      searched = s.searched or nil, band = s.band, templeIndex = s.templeIndex,
    }
  end

  local items = {}
  for _, it in ipairs(g.map.items) do
    items[#items + 1] = {
      index = it.index, name = it.name, type = it.type, value = it.value,
      status = it.status, x = it.x, y = it.y, planted = it.planted,
    }
  end

  local signs = {}
  for i, sg in ipairs(g.map.signs or {}) do signs[i] = { sg[1], sg[2] } end

  local state = {
    version = save.VERSION,
    scenario = g.map.name,
    turn = g.turn, current = g.current, seed = g.rng.state,
    won = g.won or nil, over = g.over or nil, noHumansSaid = g.noHumansSaid or nil,
    greatest = g.greatest or nil,
    surrenderOffered = g.surrenderOffered or nil,
    options = g.map.options,
    diplomacy = g.diplomacy,
    sides = sides, cities = cities, sites = sites, items = items, armies = armies,
    signs = signs, fightOrder = g.map.fightOrder,
    history = g.history, deeds = g.deeds, triumphs = g.triumphs,
    tutorialSeen = g.tutorialSeen,
    log = g.log,
  }

  local out = { "return " }
  serialise(state, out)
  return table.concat(out)
end

--------------------------------------------------------------------- reading

-- Falsey values are left out of the save, so they come back as nil; the
-- engine treats the two alike, but restoring them as false keeps a reloaded
-- game identical to the one that was saved.
local function bool(v) return v and true or false end

--- Rebuild a game from a saved string. `dataDir` must hold the same data files
--- the game was started from.
function save.decode(text, dataDir)
  local chunk = assert(load(text, "save", "t"))
  local state = chunk()
  assert(state.version == save.VERSION, "unsupported save version")

  local g = game.new(dataDir, state.scenario, { seed = 0, options = state.options })
  g.armies = {}
  g.turn, g.current = state.turn, state.current
  g.rng.state = state.seed
  g.won, g.over, g.surrenderOffered = state.won, state.over, state.surrenderOffered
  g.noHumansSaid = state.noHumansSaid
  g.greatest = state.greatest
  g.diplomacy, g.log = state.diplomacy, state.log or {}

  local itemByIndex = {}
  for _, saved in ipairs(state.items) do
    for _, it in ipairs(g.map.items) do
      if it.index == saved.index then
        it.name, it.type, it.value = saved.name, saved.type, saved.value
        it.status, it.x, it.y = saved.status, saved.x, saved.y
        it.planted = saved.planted
        itemByIndex[it.index] = it
      end
    end
  end

  for _, saved in ipairs(state.cities) do
    local c = g.map.cities[saved.index + 1]
    c.ownerIndex, c.producing, c.countdown = saved.ownerIndex, saved.producing, saved.countdown
    c.vectorTo, c.razed, c.defence = saved.vectorTo, bool(saved.razed), saved.defence
    c.income, c.slots = saved.income, saved.slots
    c.claim = saved.claim or saved.previousOwner or c.claim
    c.razedBy = saved.razedBy
    c.name = saved.name or c.name            -- renamed; older saves lack it
  end
  -- the map comes back off disk with every city as tile 96, so restamp the
  -- castles from the ownership we have just restored
  require("warlords.scn").refreshCityTiles(g.map)

  for _, saved in ipairs(state.sites) do
    local s = g.map.sites[saved.index + 1]
    s.content, s.item, s.guardian = saved.content, saved.item, saved.guardian
    s.allyType, s.rich, s.revealed = saved.allyType, bool(saved.rich), saved.revealed
    s.searched, s.band = bool(saved.searched), saved.band
    s.templeIndex = saved.templeIndex
  end
  if state.fightOrder then g.map.fightOrder = state.fightOrder end
  g.history, g.deeds, g.triumphs = state.history, state.deeds, state.triumphs
  g.tutorialSeen = state.tutorialSeen
  for i, saved in ipairs(state.signs or {}) do    -- older saves lack them
    local sg = g.map.signs[i]
    if sg then sg[1], sg[2] = saved[1], saved[2] end
  end

  g.map.siteAt = {}
  for _, s in ipairs(g.map.sites) do g.map.siteAt[s.y * g.map.width + s.x] = s end

  local armies = {}
  for i, saved in ipairs(state.armies) do
    local a = {}
    for k, v in pairs(saved) do a[k] = v end
    a.atSea, a.returning = bool(saved.atSea), bool(saved.returning)
    a.items = nil
    if saved.items then
      a.items = {}
      for _, idx in ipairs(saved.items) do
        a.items[#a.items + 1] = itemByIndex[idx]
      end
    end
    armies[i] = a
    g.armies[i] = a
  end

  for _, saved in ipairs(state.sides) do
    local s = g.map.sides[saved.index + 1]
    s.gold, s.alive, s.computer = saved.gold, saved.alive, saved.computer
    s.level, s.enhanced, s.diploScore = saved.level, saved.enhanced, saved.diploScore
    s.observe = saved.observe or false
    s.income, s.upkeepTotal = saved.income, saved.upkeepTotal
    s.produced = saved.produced
    -- an older save kept the roles alone; it keeps the data set up anew
    if saved.ai and saved.ai.groups then
      s.ai = saved.ai
      for _, grp in ipairs(s.ai.groups) do
        for k = 1, 4 do grp.staged[k] = grp.staged[k] and armies[grp.staged[k]] or nil end
      end
    end
    s.aiSolidarity, s.card = saved.aiSolidarity, saved.card
    s.advisor = saved.advisor
    s.quest = nil
    if saved.quest then
      local q = {
        type = saved.quest.type, done = saved.quest.done,
        required = saved.quest.required, hero = armies[saved.quest.hero],
      }
      local kind, ref = saved.quest.targetKind, saved.quest.target
      q.targetKind = kind
      if kind == "city" then q.target = g.map.cities[ref + 1]
      elseif kind == "side" then q.target = g.map.sides[ref + 1]
      elseif kind == "item" then q.target = itemByIndex[ref]
      elseif kind == "armytype" then q.target = g.types.byId[ref]
      elseif kind == "army" then q.target = armies[ref]
      elseif kind == "none" then q.target = true end
      if q.hero and q.target then s.quest = q end
    end
  end

  -- saves made before the walk kept to 1a8b:04c8 could put a flier at sea,
  -- and a hero flying with one: nothing the original can reach, so undone
  local fliers, walkers = {}, {}
  for _, a in ipairs(g.armies) do
    if a.x then
      local k = a.y * g.map.width + a.x
      if g.types.byId[a.type].flies then fliers[k] = true
      elseif a.type ~= 28 then walkers[k] = true end
    end
  end
  for _, a in ipairs(g.armies) do
    if a.atSea and a.x then
      local k = a.y * g.map.width + a.x
      if g.types.byId[a.type].flies or (a.type == 28 and fliers[k] and not walkers[k]) then
        a.atSea = false
      end
    end
  end

  g.side = g.sides[g.current]
  require("warlords.move").invalidate(g)
  return g
end

--------------------------------------------------------------------- files

function save.write(g, path)
  local f = assert(io.open(path, "w"))
  f:write(save.encode(g))
  f:close()
  return path
end

function save.read(path, dataDir)
  local f = assert(io.open(path, "r"), "cannot open: " .. path)
  local text = f:read("*a")
  f:close()
  return save.decode(text, dataDir)
end

return save
