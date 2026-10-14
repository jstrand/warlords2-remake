-- A hero's promotion and an army's medal, as the original shows them to a
-- human: a picture popup at (160, 90) 320x200 with lines of text over it,
-- closed by any key or click.
--
-- A promotion (hero_promote, 7563:0672) is popup 16, 17 or 18 -- LEVEL2-4.PCK
-- -- with music cue 7: STRING.DAT group 101-103 (lines 6 on for a heroine),
-- the title in font 1 at y 93, the hero's name at 140 and the rest in font 2
-- at 160, 180, 200, 232 and 252, all centred on 320.
--
-- A medal (auto_ui_medal_effect, 67cc:2274) is popup 24-27 -- MEDAL0-3.PCK
-- for the army's first to fourth -- with music cue 8 and then 1 again: the
-- army at (176, 155), and in font 2 a line of group 158 at (216, 155), "your
-- unit of %s" (159) at (216, 175), a line of 160 at (176, 195), the medal's
-- name (161) at (176, 215), and what grew (162) at (176, 245) and (176, 265).

local kit   = require("ui.kit")
local sound = require("sound")
local cues  = require("warlords.cues")

local M = {}

local R = { x = 160, y = 90, w = 320, h = 200 }
local PROMOTION_GROUP, MEDAL_FOR, MEDAL_UNIT, MEDAL_GIVEN = 101, 158, 159, 160
local MEDAL_NAME, MEDAL_GREW = 161, 162

local function show(pic, draw, after)
  local d = {}
  local function close() kit.pop(d) if after then after() end end
  function d.draw()
    kit.popupFrame(R)
    love.graphics.setColor(1, 1, 1)
    if pic then love.graphics.draw(pic, R.x, R.y) end
    draw()
  end
  function d.mousepressed() close() end
  function d.keypressed() close() end
  return kit.push(d)
end

--- Show each of `list` -- hero.checkPromotions's { hero, gain } -- in turn,
--- then `after`.
function M.promotions(list, after)
  local G = kit.G
  local function nextOne(i)
    if not list or i > #list then
      if after then after() end
      return
    end
    local h, gain = list[i].hero, list[i].gain
    local level = h.level or 2                       -- the new level, 2-4
    local group = PROMOTION_GROUP + level - 2
    local base = h.female and 6 or 0
    sound.music(cues.PROMOTION)
    show(G.levelPic[level], function()
      kit.centred(kit.font(1), kit.text(group, base), 320, 93)
      local f = kit.font(2)
      kit.centred(f, h.name or "", 320, 140)
      kit.centred(f, kit.text(group, base + 1), 320, 160)
      kit.centred(f, kit.text(group, base + 2), 320, 180)
      kit.centred(f, kit.text(group, base + 3), 320, 200)
      kit.centred(f, kit.text(group, base + 4), 320, 232)
      kit.centred(f, kit.text(group, base + 5):format(level, h.strength or 0, gain or 0,
                                                       h.maxMoves or 0, 2), 320, 252)
    end, function() nextOne(i + 1) end)
  end
  nextOne(1)
end

--- An army's new medal (game.applyAttack's result.medal), then `after`.
function M.medal(a, after)
  local G = kit.G
  local n = a.medals or 1
  local grewMove = (a.strength or 0) >= 15
  local now = grewMove and (a.maxMoves or 0) or (a.strength or 0)
  local function pick(group, count) return kit.text(group, G.g.rng:dice(1, count, -1)) end
  local line1, line3 = pick(MEDAL_FOR, 7), pick(MEDAL_GIVEN, 4)
  local t = G.g.types.byId[a.type]
  sound.music(cues.MEDAL)
  show(G.medalPic[n], function()
    kit.army(a.type, a.owner, 176, 155, 0)
    love.graphics.setColor(1, 1, 1)
    local f = kit.font(2)
    f.draw(line1, 216, 155)
    f.draw(kit.text(MEDAL_UNIT, 0):format(t and t.name or ""), 216, 175)
    f.draw(line3, 176, 195)
    f.draw(kit.text(MEDAL_NAME, n - 1), 176, 215)
    f.draw(kit.text(MEDAL_GREW, grewMove and 1 or 0), 176, 245)
    f.draw(kit.text(MEDAL_GREW, 2):format(now - 1, now), 176, 265)
  end, function()
    sound.music(cues.PLAY)
    if after then after() end
  end)
end

return M
