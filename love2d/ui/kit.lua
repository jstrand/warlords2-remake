-- The pieces every dialog is made of, and the stack they are shown on.
--
-- The original builds its dialogs out of a handful of routines, and so does
-- this: a popup (54f6:0000) is a black outline and a two-pixel shadow round a
-- crop of MARBLE.PCK, text is drawn centred (7ecb:00d6) or from its left edge
-- (7ecb:00bf), an army is drawn by 8611:08be with a ring under it, a shield by
-- 8611:0bf7, and a dialog's buttons are its BUTTON.DAT controls. The numbers
-- in each dialog module are the original's own, read out of the routine that
-- draws it.
--
-- A dialog is a table with draw() and, as it needs them, mousepressed(x, y,
-- button), keypressed(key) and textinput(text). kit.push puts one on top;
-- the front end sends input to the top one only, which is what makes it
-- modal.

local screen = require("warlords.screen")
local uidata = require("warlords.uidata")

local kit = {}

local G                                    -- the front end's state, from init

function kit.init(state)
  G = state
  kit.G = state
  G.modals = G.modals or {}
end

--------------------------------------------------------------------- the stack

function kit.push(d)
  G.modals[#G.modals + 1] = d
  return d
end

function kit.pop(d)
  for i = #G.modals, 1, -1 do
    if G.modals[i] == d then table.remove(G.modals, i) return end
  end
end

function kit.top()
  return G.modals[#G.modals]
end

--------------------------------------------------------------------- colour

--- Set a palette colour by its index, 0-15.
function kit.setPal(i)
  local c = G.palette[i + 1] or G.palette[1]        -- pal.lua is 1-based
  love.graphics.setColor(c[1], c[2], c[3])
end

--- A one-pixel outline, x..x+w-1 by y..y+h-1, in the current colour -- the
--- rect 216d:01fd draws.
function kit.outline(x, y, w, h)
  love.graphics.rectangle("fill", x, y, w, 1)
  love.graphics.rectangle("fill", x, y + h - 1, w, 1)
  love.graphics.rectangle("fill", x, y, 1, h)
  love.graphics.rectangle("fill", x + w - 1, y, 1, h)
end

--------------------------------------------------------------------- popups

--- The frame 54f6:0000 gives a popup: a black outline one pixel outside the
--- rect and a two-pixel black shadow beyond it, down and to the right.
--- Measured off the original running at its own 640x480.
function kit.popupFrame(R)
  love.graphics.setColor(0, 0, 0)
  kit.outline(R.x - 1, R.y - 1, R.w + 2, R.h + 2)
  love.graphics.rectangle("fill", R.x + 1, R.y + R.h + 1, R.w + 2, 2)
  love.graphics.rectangle("fill", R.x + R.w + 1, R.y + 1, 2, R.h + 2)
end

--- A popup with no picture of its own: MARBLE.PCK, from its top-left corner,
--- cropped to the rect, inside the frame. MARBLE is 480x360, which is why no
--- such popup is wider than 480.
function kit.popup(R)
  kit.popupFrame(R)
  love.graphics.setColor(1, 1, 1)
  love.graphics.setScissor(R.x, R.y, R.w, R.h)
  love.graphics.draw(G.marble, R.x, R.y)
  love.graphics.setScissor()
end

--------------------------------------------------------------------- text

-- 78a8:06ae picks the font by number: 1 is CHANCE36, the titles, and 2 is
-- CHANCE17, the body text. TEXT is the menus' and the bar's.
function kit.font(n)
  if n == 1 then return G.titleFont end
  if n == 2 then return G.bigFont end
  return G.font
end

--- Centred on x, with y the top of the line (7ecb:00d6).
function kit.centred(f, text, x, y)
  f.draw(text, x - math.floor(f.width(text) / 2), y)
end

--- Right-aligned, ending at x (7ecb:0103).
function kit.right(f, text, x, y)
  f.draw(text, x - f.width(text), y)
end

--- A one-pixel bevel (24d0:02e5): colour `a` along the top and left, `b`
--- along the right and bottom. Whichever colour has the higher number is
--- drawn second, so it takes the two corners where the edges meet -- (4, 2),
--- the sunken look, has colour 4 at both.
function kit.bevel(x, y, w, h, a, b)
  local function topLeft()
    kit.setPal(a)
    love.graphics.rectangle("fill", x, y, w, 1)
    love.graphics.rectangle("fill", x, y, 1, h)
  end
  local function bottomRight()
    kit.setPal(b)
    love.graphics.rectangle("fill", x + w - 1, y, 1, h)
    love.graphics.rectangle("fill", x, y + h - 1, w, 1)
  end
  if b < a then bottomRight() topLeft() else topLeft() bottomRight() end
end

--- A text field (7ecb:0058): filled with colour 3, sunk into the panel with a
--- (4, 2) bevel, and the text three pixels in and two down.
function kit.field(x, y, w, h, text, f)
  kit.setPal(3)
  love.graphics.rectangle("fill", x, y, w, h)
  kit.bevel(x, y, w, h, 4, 2)
  love.graphics.setColor(1, 1, 1)
  if text then (f or G.bigFont).draw(text, x + 3, y + 2) end
end

--------------------------------------------------------------------- art

--- An army the way 8611:08be draws one: the ring first if there is one, then
--- the army's cell of its side's sheet -- or of ASHADOW.PCK when `shadow` is
--- set, which is how a type that cannot be had is shown. `ring` is the
--- routine's own argument: 0 none, 1 the grey ring, 2-9 a side's colour.
--- `type` nil draws the ring alone.
function kit.army(typeId, side, x, y, ring, shadow)
  if side == nil or side == 15 then side = 8 end
  love.graphics.setColor(1, 1, 1)
  if ring and ring > 0 then
    love.graphics.draw(G.abits, G.ringQuads[ring - 1], x, y)
  end
  if typeId then
    local img = shadow and G.shadowImg or G.armyImg[side]
    love.graphics.draw(img, G.armyQuads[side][typeId % 32], x, y)
  end
end

--- A side's big shield (8611:0bf7, size 0): SHIELDS.PCK's 40x40 cell at
--- (side * 40, 0), frame and all. Neutral is the ninth, the question mark.
function kit.shield(side, x, y)
  if side == nil or side == 15 then side = 8 end
  if not G.shieldsImg then return end
  love.graphics.setColor(1, 1, 1)
  love.graphics.draw(G.shieldsImg,
    love.graphics.newQuad(side * 40, 0, 40, 40, G.shieldsImg:getDimensions()), x, y)
end

--------------------------------------------------------------------- controls

--- A dialog's BUTTON.DAT controls, each in its own state.
function kit.view(dialogId)
  return screen.dialog(G.screen, dialogId)
end

function kit.drawControls(view, hidden)
  if hidden then
    -- draw only the controls not hidden, keeping each one's state
    local keep = view.dialog.controls
    local shown = {}
    for _, c in ipairs(keep) do if not hidden[c.id] then shown[#shown + 1] = c end end
    view.dialog.controls = shown
    screen.drawDialogControls(G.screen, view)
    view.dialog.controls = keep
  else
    screen.drawDialogControls(G.screen, view)
  end
end

--- The live control under a point: not disabled and not hidden.
function kit.controlAt(view, x, y, hidden)
  for _, c in ipairs(view.dialog.controls) do
    if c.w > 0 and c.h > 0 and not (hidden and hidden[c.id])
       and x >= c.x and x < c.x + c.w and y >= c.y and y < c.y + c.h then
      if view.state[c.id] == uidata.DISABLED then return nil end
      return c
    end
  end
  return nil
end

function kit.control(view, id)
  return screen.dialogControl(view, id)
end

--- A string from STRING.DAT, numbered as get_string numbers them.
function kit.text(group, i)
  return uidata.text(G.screen.ui, group, i)
end

return kit
