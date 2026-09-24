-- The end of the game (8065:1aed and the screens it leads to).
--
-- The three pictures are popups 20-22, (136, 40) 368x390, each its own
-- bitmap blitted from (0, 0) with the words painted in:
--
--   20 RESIGN.PCK   "An offer of Peace" -- dialog 30: No (486), Yes (485)
--   21 RESIGNNO.PCK "Peace is not an option!" -- dialog 31: Done (484)
--   22 RESIGNYE.PCK "Congratulations!" -- dialog 32: Done (483)
--
-- The ways a game ends, as the check at the round's end finds them:
--
--   nobody left: "Alas! / No more players are left!", "So I bid thee a fond
--     'FAREWELL' / Hit any key to return to DOS." (group 12), then out
--   no human left: "No further human resistance is possible! / But the
--     battle will continue!" and the rest of group 13
--   one computer side left: "%s, thou hast triumphed!" (group 15), the side
--     handed to the player to look the world over
--   a lone human with more than half the cities: group 15's two lines, the
--     Congratulations picture (8065:1fbd), then the hidden map lifted
--     (8065:2004) and "At thy leisure / thou mayst inspect thy kingdom"
--     (group 16 -- where the original shows it is not traced; its words fit
--     here)
--   surrender offered (8065:1f68): the offer; Yes (8065:1e4e) puts every
--     computer side out and goes on as a win, No (8065:1ecd) shows the heads
--     on poles.

local kit      = require("ui.kit")
local searchUi = require("ui.search")
local game     = require("warlords.game")
local uidata   = require("warlords.uidata")

local M = {}

local R = { x = 136, y = 40, w = 368, h = 390 }     -- popups 20-22
local PICTURE = { [20] = 12, [21] = 14, [22] = 13 } -- FILE.DAT group 3

local function t(g, i) return kit.text(g, i) end

--- A picture popup with its dialog; `pick(id)` gets the button pressed.
local function picture(popup, dialog, ids, default, cancel, pick)
  local G = kit.G
  local d = { view = kit.view(dialog) }
  for _, id in ipairs(ids) do d.view.state[id] = uidata.NORMAL end
  function d.draw()
    kit.popupFrame(R)
    local art = G.screen.art_for(PICTURE[popup])
    if art then
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(art.image, love.graphics.newQuad(0, 0, R.w, R.h, art.w, art.h), R.x, R.y)
    end
    kit.drawControls(d.view)
  end
  local function press(id) kit.pop(d) pick(id) end
  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if c then press(c.id) end
  end
  function d.keypressed(key)
    if key == "return" or key == "kpenter" then press(default)
    elseif key == "escape" then press(cancel) end
  end
  return kit.push(d)
end

--- Congratulations (8065:1fbd), then the map shown whole (8065:2004).
local function victory(after)
  local G = kit.G
  picture(22, 32, { 483 }, 483, 483, function()
    G.g.map.options.hiddenMap = 0
    if G.stratDirty then G.stratDirty() end
    searchUi.message(t(0x10, 0), t(0x10, 1), after)
  end)
end

--- What the round's end found (g.ending), shown before the turn goes on.
--- `after` runs once it has all been seen.
function M.show(ending, after)
  local G = kit.G
  after = after or function() end
  if not ending then return after() end
  if ending.surrender and not ending.shown then
    ending.shown = true
    return picture(20, 30, { 485, 486 }, 485, 486, function(id)
      if id == 485 then
        game.acceptSurrender(G.g, G.player)
        victory(after)
      else
        picture(21, 31, { 484 }, 484, 484, after)
      end
    end)
  end
  if ending.won and ending.winner == G.player then
    return searchUi.message(t(0xf, 0):format(G.player.name or ""), t(0xf, 1),
                            function() victory(after) end)
  end
  after()
end

--- The game has stopped: nobody left, no human left, or a computer won.
function M.over(ending)
  local G = kit.G
  ending = ending or {}
  if ending.winner then
    return searchUi.say(t(0xf, 0):format(ending.winner.name or ""))
  end
  local humans = false
  for _, s in ipairs(G.g.sides) do
    if s.alive and not s.computer and #game.sideCities(G.g, s) > 0 then humans = true end
  end
  if not humans and ending.message and ending.message:find("No more players") then
    return searchUi.message(t(0xc, 0), t(0xc, 1), function()
      searchUi.message(t(0xc, 2), t(0xc, 3), function() love.event.quit() end)
    end)
  end
  return searchUi.message(t(0xd, 0), t(0xd, 1), function()
    searchUi.message(t(0xd, 2), t(0xd, 3))
  end)
end

return M
