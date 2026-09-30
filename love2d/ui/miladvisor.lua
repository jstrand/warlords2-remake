-- The Military Advisor (military_advisor, 67cc:1f19): Shift and a click on
-- an enemy beside the selected stack -- the "?" pointer, which only shows
-- with the Military Advisor option on -- asks how the fight would go.
--
-- Popup 12, (176, 60) 288x300, marble, with dialog 25's one button, 424, to
-- close it (67cc:2050). On it:
--
--   "Advisor!" (group 123) in font 1, centred on (320, 67)
--   ADVISOR.PCK (bitmap 48), its (0, 0) 128x130, at (256, 117)
--   in font 2, centred on 320: a random line of group 124 ("O Mighty
--     Leader,") at y = 258, a random one of group 125 ("this battle would
--     be") at 278, and at 298 the verdict, group 126's line number wins / 2
--     (0-9) out of 19 battles fought in secret (advisor_simulate_wins,
--     67cc:2065, which counts a battle won when an attacker is left alive)
--
-- The simulated battles draw on the game's own dice, as the original's do.

local kit    = require("ui.kit")
local combat = require("warlords.combat")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 176, y = 60, w = 288, h = 300 }     -- popup 12
local DIALOG, OK = 25, 424
local TITLE, HAIL, BATTLE, VERDICT = 0x7b, 0x7c, 0x7d, 0x7e
local PICTURE = 48                                  -- ADVISOR.PCK

--- The advice for the selected stack attacking (x, y), or nil when the
--- option is off or nothing is selected.
function M.open(stack, x, y)
  local G = kit.G
  local g = G.g
  if g.map.options.militaryAdvisor == 0 or not stack or #stack == 0 then return nil end
  local attackers, defenders = combat.lines(g, stack, x, y)
  local _, wins = combat.advise(g, attackers, defenders, x, y)
  local rng = g.rng
  local d = {
    view = kit.view(DIALOG),
    wins = wins,
    hail = kit.text(HAIL, rng:dice(1, 5, -1)),       -- get_string(0x7c, -1)
    battle = kit.text(BATTLE, rng:dice(1, 5, -1)),   -- get_string(0x7d, -1)
    verdict = kit.text(VERDICT, math.floor(wins / 2)),
  }
  d.view.state[OK] = uidata.NORMAL

  local function close() kit.pop(d) end

  function d.draw()
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(TITLE, 0), 320, 67)
    local art = G.screen.art_for(PICTURE)
    if art then
      love.graphics.setColor(1, 1, 1)
      love.graphics.draw(art.image, love.graphics.newQuad(0, 0, 128, 130, art.w, art.h), 256, 117)
    end
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    kit.centred(f, d.hail, 320, 258)
    kit.centred(f, d.battle, 320, 278)
    kit.centred(f, d.verdict, 320, 298)
    kit.drawControls(d.view)
  end

  function d.mousepressed(mx, my)
    local c = kit.controlAt(d.view, mx, my)
    if c and c.id == OK then close() end
  end

  -- 424 is on both the default and the cancel lists (4125:155c, :1514)
  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then close() end
  end

  return kit.push(d)
end

return M
