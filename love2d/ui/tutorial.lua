-- The tutorial's help pages (the Tutorial option, .SCN 0x12e): each shown
-- once, the first time its moment comes, its bit set in 2c04:0134. The
-- pages are FILE.DAT group 0x19 -- TUTORIA\T*.GFX -- shown by 8065:168d:
--
--   hero     THERO     the hero offer (7563:11f5)                  bit 0x01
--   prod     TPROD     the city dialog opened (7204:027f)          bit 0x02
--   select   TSELECT   the city dialog closed (7204:0321)          bit 0x04
--   move     TMOVE     a stack picked up (1b62:051d)               bit 0x08
--   fight    TFIGHT    ... next to an enemy city (1b62:05af)       bit 0x10
--   prod2    TPROD2    the city dialog, with two cities (7204:02c5) bit 0x20
--   turn2    TTURN2    the second turn begins (8cc6:05df)          bit 0x40
--   search   TSEARCH   a hero picked up on a site (1b62:062a)      bit 0x80
--   endturn  TENDTURN  a stack's move ends on turn 1 (1c8c:02db)
--   fresult  TFRESULT  the first battle's result (63fa:0283)
--
-- TWARLORD (7bab:00d1) waits on a count of 10 whose meaning is not traced,
-- and is not shown.

local kit = require("ui.kit")

local M = {}

local PAGES = {
  hero = 0, prod = 1, select = 2, move = 3, fight = 4, search = 5,
  prod2 = 6, turn2 = 7, endturn = 9, fresult = 10,
}
local FILES = {
  [0] = "THERO", "TPROD", "TSELECT", "TMOVE", "TFIGHT", "TSEARCH",
  "TPROD2", "TTURN2", "TWARLORD", "TENDTURN", "TFRESULT",
}

--- Show the page for `moment`, if the tutorial is on and it has not been
--- shown yet; `after` runs once it is put away, or at once if it is not
--- shown.
function M.show(moment, after)
  local G = kit.G
  local g = G.g
  after = after or function() end
  local i = PAGES[moment]
  if not g or not i or (g.map.options.tutorial or 0) == 0 then return after() end
  g.tutorialSeen = g.tutorialSeen or {}
  if g.tutorialSeen[moment] then return after() end
  g.tutorialSeen[moment] = true
  if not require("ui.help").open("TUTORIA\\" .. FILES[i] .. ".GFX", after) then after() end
end

--- Several moments one after another.
function M.chain(moments, after)
  local k = 0
  local function nextOne()
    k = k + 1
    if moments[k] then M.show(moments[k], nextOne) elseif after then after() end
  end
  nextOne()
end

return M
