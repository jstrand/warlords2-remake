-- Build Production: buying a new army type for a city.
--
-- The city dialog's Build Prod button (control 204, 7087:0978) pushes popup
-- 11 -- (80, 60) 480x350, marble -- and opens dialog 23 over it.
-- auto_ui_build_production (7087:09da) draws it:
--
--   the title, get_string(0x70, 0), centred on x = 320 at y = 64, in font 1,
--     with the side's shield either side of it at (88, 64) and (512, 64)
--   "The %s city of %s" centred at (320, 104)
--   every type with a price, four to a row: the army on the grey ring at
--     (88 + col * 120, 136 + row * 31) -- ghosted when the city already has
--     it or the side cannot pay -- and "%d gp" at (+40, +6)
--   "Currently Producing" centred at (176, 348), and the city's four slots
--     along y = 370 from x = 104, 40 apart, each on its side's ring
--   "Thou hast %d gold" centred at (352, 365)
--
-- The slot being bought into is framed: a colour 0 box at (x - 2, y - 2)
-- 37x35 and a colour 9 one a pixel up and left of it. Every other slot gets
-- the same two boxes in colour 3.
--
-- Control 396 is Done, 397-400 pick the slot (live only once all four are
-- full: until then a type goes into the first empty one), and 401 on are the
-- types in list order (7087:0dae decides which are live). Done puts the list
-- back in price order (7087:0eca) and returns to the city dialog's City mode.

local kit    = require("ui.kit")
local game   = require("warlords.game")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 80, y = 60, w = 480, h = 350 }      -- popup 11
local DIALOG = 23
local DONE, SLOT_FIRST, TYPE_FIRST = 396, 397, 401
local STR = 0x70                                    -- STRING.DAT group 112

local function refresh(d)
  local G, st = kit.G, d.view.state
  st[DONE] = uidata.NORMAL
  local full = #d.city.slots >= 4
  for i = 0, 3 do st[SLOT_FIRST + i] = full and uidata.NORMAL or uidata.DISABLED end
  for n, a in ipairs(d.types) do
    st[TYPE_FIRST + n - 1] =
      game.cannotBuy(G.g, G.player, d.city, a) and uidata.DISABLED or uidata.NORMAL
  end
end

--- Open the screen for a city. `done` runs when it closes.
function M.open(city, done)
  local G = kit.G
  local d = {
    city = city, done = done,
    view = kit.view(DIALOG),
    types = game.buyableTypes(G.g),
    slot = game.buySlot(city),
  }
  refresh(d)

  function d.close()
    game.sortProduction(city)
    kit.pop(d)
    if d.done then d.done() end
  end

  function d.buy(n)
    local a = d.types[n]
    if not a or game.cannotBuy(G.g, G.player, city, a) then return end
    game.buyProduction(G.g, G.player, city, d.slot, a.id)
    -- on to the next empty slot, if there is one (7087:0ee3)
    if d.slot < 4 and not city.slots[d.slot + 1] then d.slot = d.slot + 1 end
    refresh(d)
  end

  function d.draw()
    local side = G.player.index
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), kit.text(STR, 0), 320, 64)
    kit.shield(side, 88, 64)
    kit.shield(side, 512, 64)
    local body = kit.font(2)
    love.graphics.setColor(1, 1, 1)
    kit.centred(body, kit.text(STR, 1):format(G.player.name, city.name), 320, 104)
    kit.centred(body, kit.text(STR, 2):format(G.player.gold), 352, 365)
    kit.centred(body, kit.text(STR, 3), 176, 348)

    -- the city's four slots, and the frame round the one being bought into
    for i = 0, 3 do
      local slot = city.slots[i + 1]
      local x = 104 + i * 40
      kit.army(slot and slot.type, side, x, 370, slot and side + 2 or 1)
      local fx, fy = x - 2, 368
      if d.slot == i + 1 then
        kit.setPal(0)
        kit.outline(fx, fy, 37, 35)
        kit.setPal(9)
        kit.outline(fx - 1, fy - 1, 37, 35)
      else
        kit.setPal(3)
        kit.outline(fx, fy, 37, 35)
        kit.outline(fx - 1, fy - 1, 37, 35)
      end
    end

    -- everything for sale, with its price
    for n, a in ipairs(d.types) do
      local col, row = (n - 1) % 4, math.floor((n - 1) / 4)
      local x, y = 88 + col * 120, 136 + row * 31
      kit.army(a.id, side, x, y, 1, game.cannotBuy(G.g, G.player, city, a) ~= nil)
      love.graphics.setColor(1, 1, 1)
      body.draw(("%d gp"):format(a.price), x + 40, y + 6)
    end

    kit.drawControls(d.view)
  end

  function d.mousepressed(x, y)
    local c = kit.controlAt(d.view, x, y)
    if not c then return end
    if c.id == DONE then d.close()
    elseif c.id >= SLOT_FIRST and c.id < SLOT_FIRST + 4 then d.slot = c.id - SLOT_FIRST + 1
    elseif c.id >= TYPE_FIRST then d.buy(c.id - TYPE_FIRST + 1)
    end
  end

  -- Done is both the default and the cancel button
  function d.keypressed(key)
    if key == "return" or key == "kpenter" or key == "escape" then d.close() end
  end

  return kit.push(d)
end

return M
