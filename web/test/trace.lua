-- An all-computer game, one line per side's turn: the Lua half of the
-- Lua-vs-JS comparison (test/trace.js prints the same lines).
--
--     luajit web/test/trace.lua ERYTHEA 1 10 [hidden] [diplo]
--
-- Run from the repository root.
package.path = "love2d/?.lua;" .. package.path
local game = require("warlords.game")
local ai = require("warlords.ai")

local scen, seed, turns = arg[1] or "ERYTHEA", tonumber(arg[2] or "1"), tonumber(arg[3] or "10")
local opts = { seed = seed }
local flags = {}
for i = 4, #arg do flags[arg[i]] = true end
opts.options = {}
if flags.hidden then opts.options.hiddenMap = 1 end
-- "diplo": with Diplomacy on, so that sides make peace
if flags.diplo then opts.options.diplomacy = 1 end
-- RANDOM: a world made from the seed first, as test/trace.js makes it
if scen == "RANDOM" then
  local randommap = require("warlords.randommap")
  randommap.install("original", randommap.generateNow({ dataDir = "original", rng = require("warlords.rng").new(seed) }))
end
local g = game.new("original", scen, opts)
local save = require("warlords.save")
for _, s in ipairs(g.sides) do s.computer = true end

local function trace(side)
  local h = 0
  for _, a in ipairs(g.armies) do
    h = (h * 31 + (a.x or 999) * 7 + (a.y or 999) * 13 + a.type * 17
         + (a.owner or 15) * 19 + (a.strength or 0) + (a.moves or 0) * 3) % 1000000007
  end
  local owners = {}
  for _, c in ipairs(g.map.cities) do
    owners[#owners + 1] = c.razed and "x" or (c.ownerIndex and string.char(48 + c.ownerIndex) or ".")
  end
  print(("%d %d gold=%d armies=%d rng=%d h=%d %s"):format(g.turn, side.index, side.gold,
        #g.armies, g.rng.state, h, table.concat(owners)))
end

local side = game.begin(g)
while side and g.turn <= turns do
  ai.playTurn(g, side)
  trace(side)
  side = game.endTurn(g)
  if flags.save and side then
    g = save.decode(save.encode(g), "original")
    side = g.side
  end
end
