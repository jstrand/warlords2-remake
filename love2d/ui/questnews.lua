-- A quest's end, as quest_check (4976:1ded) tells a human it.
--
-- Failed: the message box (8065:1160) with the two lines of its cause,
-- STRING.DAT groups 32-42 -- "Alas! Thy hero is dead!" / "Thy quest hath
-- become impossible!" and the rest.
--
-- Done: the triumph's music, and the reward (4976:1520, its text drawn by
-- 4976:160e). Popup 2 with dialog 17 -- Done (331); the strategic map on the
-- left with no city shields, and the hero's figure on it (834b:1f5f);
-- "Quest" in font 1 centred on (432, 64); SCROLL.PCK at (304, 95); and on
-- it, in font 2 black on yellow, centred on x = 440:
--   y 145  "%s's Quest", the hero's name, over a black rule at (360, 165)
--          160 long
--   y 175, 195  "Thou hast completed thy" / "quest!" (group 28)
--   y 225, 245  what the priests do (groups 29-31 by the reward)
--   y 265  the site's name, the item's, or "%d %s" -- so many allies of a
--          kind, or so much "gold"
-- A site shown is marked on the map the way the quest report marks a
-- target: the line from the hero, the shield and the box (828e:0a3f).

local kit    = require("ui.kit")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 312 }      -- popup 2
local MAP = { x = 80, y = 60 }
local DIALOG, DONE = 17, 331

local function reward(news, after)
  local G = kit.G
  local q, r = news.quest, news.reward
  local d = { view = kit.view(DIALOG) }
  d.view.state[DONE] = uidata.NORMAL
  local h = q.hero
  local function close() kit.pop(d) if after then after() end end

  local grp, last
  if r.kind == "revealed" then
    grp, last = 0x1d, r.site and r.site.name or ""
  elseif r.kind == "item" then
    grp, last = 0x1e, r.item and r.item.name or ""
  elseif r.kind == "allies" then
    grp, last = 0x1f, kit.text(0x1f, 2):format(#(r.armies or {}), r.type and r.type.name or "")
  else
    grp, last = 0x1f, kit.text(0x1f, 2):format(r.gold or 0, "gold")     -- 4125:00d9
  end

  function d.draw()
    kit.popup(R)
    G.drawStrategicMap(MAP.x, MAP.y, nil, true)
    if r.kind == "revealed" and r.site and h and h.x then
      kit.mapTarget(MAP.x, MAP.y, h.x, h.y, r.site.x, r.site.y)
    end
    if h and h.x then G.drawHeroFigure(MAP.x, MAP.y, h.x, h.y) end
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), "Quest", 432, 64)                 -- 4125:00c8
    love.graphics.draw(G.scrollPic, 304, 95)
    local f = kit.font(2).colours(0, 7)
    kit.centred(f, ("%s's Quest"):format(h and h.name or ""), 440, 145)   -- 4125:00ce
    kit.setPal(0)
    love.graphics.rectangle("fill", 360, 165, 160, 1)
    kit.centred(f, kit.text(0x1c, 0), 440, 175)
    kit.centred(f, kit.text(0x1c, 1), 440, 195)
    kit.centred(f, kit.text(grp, 0), 440, 225)
    kit.centred(f, kit.text(grp, 1), 440, 245)
    kit.centred(f, last, 440, 265)
    kit.drawControls(d.view)
  end
  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if c and c.id == DONE then close() end
  end
  function d.keypressed(key)
    if key == "escape" or key == "return" or key == "kpenter" then close() end
  end
  return kit.push(d)
end

--- Tell the player how the quest ended: `news` is what quest.event gave
--- (a reward, or `failed` with `why`, its STRING.DAT group). `after` runs
--- once it has been read.
function M.show(news, after)
  if news.failed then
    local grp = news.why or 0x20
    return require("ui.search").message(kit.text(grp, 0), kit.text(grp, 1), after)
  end
  require("sound").music(require("warlords.cues").TRIUMPH)
  return reward(news, after)
end

--- Show the player's news, if there is any and nothing else is up: the
--- front end calls this when the screen is its own again.
function M.poll(G)
  local side = G.player
  local news = side and not side.computer and side.questNews
  if not news then return end
  side.questNews = nil
  M.show(news)
end

return M
