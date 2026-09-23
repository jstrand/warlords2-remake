-- Hero > Search: what a ruin or a temple says (site_search, 6536:0000), and
-- the game's plain message box.
--
-- A **ruin** (ruin_search, 6536:01ab) opens popup 4 -- (120, 50) 400x360 --
-- with "Searching" in font 1 centred on (320, 53) and SEARCH.PCK at (160, 92)
-- in a black frame one pixel out (6536:175b). Its story is told a line at a
-- time from (128, 300), 20 apart (6536:17c9), each waiting for a key or a
-- click (8065:10fb): who the hero meets and how that went, or that the place
-- is empty; then what is found. Dialog 13 follows: Done (292) and, when an
-- item was found and left on the ground, Take (293), which picks up all
-- that lies there (6536:186e).
--
-- A **temple** (auto_ui_temple, 4976:0000) is popup 9 -- TEMPLE.PCK at
-- (160, 60) -- with its name and two lines of greeting centred on x = 320 at
-- y = 270, 290 and 310, in font 2 coloured 7 on 6, over dialog 15: Bless
-- (328) and Quest (329), greyed when quests are off, the side already has
-- one, or there is no hero.
--
-- The **message box** (8065:1160) is popup 5 -- (144, 179) 352x64 -- with a
-- line centred on (320, 190) and another on (320, 212), closed by any key or
-- click.

local kit    = require("ui.kit")
local game   = require("warlords.game")
local hero   = require("warlords.hero")
local quest  = require("warlords.quest")
local site   = require("warlords.site")
local uidata = require("warlords.uidata")

local M = {}

--------------------------------------------------------------------- message

local MSG = { x = 144, y = 179, w = 352, h = 64 }  -- popup 5

--- A message box. `after` runs when it is closed.
function M.message(line1, line2, after)
  local d = {}
  local function close() kit.pop(d) if after then after() end end
  function d.draw()
    kit.popup(MSG)
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    if line1 then kit.centred(f, line1, 320, 190) end
    if line2 then kit.centred(f, line2, 320, 212) end
  end
  function d.mousepressed() close() end
  function d.keypressed() close() end
  return kit.push(d)
end

--------------------------------------------------------------------- ruins

local RUIN = { x = 120, y = 50, w = 400, h = 360 } -- popup 4
local RUIN_DIALOG, DONE, TAKE = 13, 292, 293

local function ruinLines(r)
  local lines = {}
  local name = r.hero and r.hero.name or ""
  local monster = r.monster or r.guardian
  if r.kind ~= "allies" then
    if monster then
      lines[#lines + 1] = kit.text(0x33, 0):format(name, monster.name or "")
      lines[#lines + 1] = kit.text(0x33, r.kind == "killed" and 1 or 2)
    else
      lines[#lines + 1] = kit.text(0x32, 0)
    end
  end
  if r.kind == "item" and r.item then
    lines[#lines + 1] = kit.text(0x34, 0):format(name, r.item.name or "")
  elseif r.kind == "gold" then
    lines[#lines + 1] = kit.text(0x35, 0):format(name, r.gold)
  elseif r.kind == "allies" then
    local n = #r.armies
    local tname = r.type and r.type.name or ""
    lines[#lines + 1] = n == 1 and kit.text(0x36, 0):format(tname, name)
                        or kit.text(0x36, 1):format(n, tname, name)
  end
  return lines
end

local function openRuin(r)
  local G = kit.G
  local d = { lines = ruinLines(r), shown = 1, view = kit.view(RUIN_DIALOG) }
  local canTake = r.kind == "item" and r.item and r.item.status == 1
  d.view.state[DONE] = uidata.NORMAL
  d.view.state[TAKE] = uidata.NORMAL
  d.hidden = { [TAKE] = not canTake or nil }

  local function done() return d.shown >= #d.lines end
  local function close()
    kit.pop(d)
    quest.event(G.g, G.player, "item", {})
  end

  function d.draw()
    kit.popup(RUIN)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), "Searching", 320, 53)        -- 4125:0c90
    love.graphics.draw(G.searchPic, 160, 92)
    kit.setPal(0)
    kit.outline(159, 91, 322, 202)
    local f = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    for i = 1, d.shown do f.draw(d.lines[i] or "", 128, 280 + 20 * i) end
    if done() then kit.drawControls(d.view, d.hidden) end
  end

  function d.mousepressed(x, y)
    if not done() then d.shown = d.shown + 1 return end
    local c = kit.controlAt(d.view, x, y, d.hidden)
    if not c then return end
    if c.id == TAKE and r.hero then
      for _, it in ipairs(G.g.map.items) do
        if it.status == 1 and it.x == r.hero.x and it.y == r.hero.y then
          hero.takeItem(G.g, r.hero, it)
        end
      end
      close()
    elseif c.id == DONE then close()
    end
  end

  function d.keypressed(key)
    if not done() then d.shown = d.shown + 1 return end
    if key == "return" or key == "kpenter" or key == "escape" then close() end
  end

  return kit.push(d)
end

--------------------------------------------------------------------- temples

local TEMPLE = { x = 160, y = 60, w = 320, h = 280 } -- popup 9
local TEMPLE_DIALOG, BLESS, QUEST = 15, 328, 329

local function openTemple(r, stack)
  local G = kit.G
  local d = { view = kit.view(TEMPLE_DIALOG) }
  d.view.state[BLESS] = uidata.NORMAL
  local canQuest = G.g.map.options.quests ~= 0 and not G.player.quest and r.hero
  d.view.state[QUEST] = canQuest and uidata.NORMAL or uidata.DISABLED

  function d.draw()
    kit.popupFrame(TEMPLE)
    love.graphics.setColor(1, 1, 1)
    love.graphics.draw(G.templePic, TEMPLE.x, TEMPLE.y)
    local f = kit.font(2).colours(7, 6)
    kit.centred(f, kit.text(0x13, 0):format(r.site.name or ""), 320, 270)
    kit.centred(f, kit.text(0x13, 1), 320, 290)
    kit.centred(f, kit.text(0x13, 2), 320, 310)
    kit.drawControls(d.view)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == BLESS then
      -- temple_bless (6536:08a1): how many were blessed, and a parting line
      kit.pop(d)
      local n = site.bless(G.g, r.site, stack)
      local line
      if n == 0 then line = kit.text(0x37, 0)
      elseif n == 1 then line = kit.text(0x38, 0)
      else line = kit.text(0x38, 1):format(n) end
      M.message(line, kit.text(0x38, 2))
    elseif c.id == QUEST then
      kit.pop(d)
      quest.assign(G.g, G.player, r.hero)
      if G.openQuest then G.openQuest() end
    end
  end

  function d.keypressed() end

  return kit.push(d)
end

--------------------------------------------------------------------- search

--- Hero > Search with the selected stack: say nothing where there is nothing
--- to search, as the original does.
function M.open(stack)
  local G = kit.G
  if not stack or #stack == 0 then return end
  local r = game.searchHere(G.g, stack, true)
  if not r or r.kind == "no hero" then return end
  if r.kind == "temple" then return openTemple(r, stack) end
  if r.kind == "sage" then
    if G.openSage then return G.openSage(r) end
    return nil
  end
  if G.stratDirty then G.stratDirty() end
  return openRuin(r)
end

return M
