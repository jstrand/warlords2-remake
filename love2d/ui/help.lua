-- A help screen from a .GFX file (8065:168d), e.g. HELP\HITEM.GFX.
--
-- The file's #D number picks the popup -- 14 + n, so a file with none gets
-- popup 14, (256, 40) 352x340, and #D001 popup 15, (32, 40) (7ecb:062a) --
-- and 7ecb:06de lays the page out inside it, every position counted from the
-- popup's corner:
--
--   #H          font 1, colour 15        #T      font 2, colour 15
--   #Fnnn       font 2, colour nnn       #E      the end
--   #C(x,y)|t|  centred on x             #L(x,y)|t|  from x
--   #R(x,y)|t|  ending at x
--   #G(x,y,w,h)nnn(dx,dy)  the (x, y) w x h of bitmap nnn, at (dx, dy)
--
-- A key or a click puts it away (8065:10fb with no text). The control
-- panel's "?" (188, 8065:104e) shows HMOUSE and then HKEYS through
-- 8065:16f5 instead, which always uses popup 4, (120, 50) 400x360.

local kit = require("ui.kit")

local M = {}

local POPUPS = {
  [0] = { x = 256, y = 40, w = 352, h = 340 },       -- popup 14
  [1] = { x = 32, y = 40, w = 352, h = 340 },        -- popup 15
}

--- The page's drawing steps, parsed once.
local function parse(text)
  local page, steps = 0, {}
  for line in text:gmatch("[^\r\n]+") do
    local op, rest = line:match("^#(%u)(.*)$")
    if op == "D" then page = tonumber(rest:match("^(%d+)")) or 0
    elseif op == "H" then steps[#steps + 1] = { font = 1, colour = 15 }
    elseif op == "T" then steps[#steps + 1] = { font = 2, colour = 15 }
    elseif op == "F" then steps[#steps + 1] = { font = 2, colour = tonumber(rest:match("^(%d+)")) or 15 }
    elseif op == "C" or op == "L" or op == "R" then
      local x, y, t = rest:match("^%((%d+),(%d+)%)|(.-)|")
      if x then steps[#steps + 1] = { text = t, align = op, x = tonumber(x), y = tonumber(y) } end
    elseif op == "G" then
      local sx, sy, w, h, id, dx, dy = rest:match("^%((%d+),(%d+),(%d+),(%d+)%)(%d+)%((%d+),(%d+)%)")
      if sx then
        steps[#steps + 1] = { bitmap = tonumber(id), sx = tonumber(sx), sy = tonumber(sy),
                              w = tonumber(w), h = tonumber(h), x = tonumber(dx), y = tonumber(dy) }
      end
    elseif op == "E" then break
    end
  end
  return page, steps
end

M.POPUP4 = { x = 120, y = 50, w = 400, h = 360 }

--- Show the help file `name` -- as the game names it, "HELP\HITEM.GFX" --
--- in popup `R` or the one its #D picks, then run `after` when it is put
--- away.
function M.open(name, after, R)
  local G = kit.G
  local path = G.dataDir .. "/" .. name:gsub("\\", "/")
  local f = io.open(path, "rb")
  if not f then return nil end
  local page, steps = parse(f:read("*a"))
  f:close()
  R = R or POPUPS[page] or POPUPS[0]

  local d = {}
  local function close() kit.pop(d) if after then after() end end

  function d.draw()
    kit.popup(R)
    local font, colour = kit.font(2), 15
    for _, s in ipairs(steps) do
      if s.font then
        font, colour = kit.font(s.font), s.colour
      elseif s.text then
        local fc = font.colours(colour, 0)
        love.graphics.setColor(1, 1, 1)
        if s.align == "C" then kit.centred(fc, s.text, R.x + s.x, R.y + s.y)
        elseif s.align == "R" then kit.right(fc, s.text, R.x + s.x, R.y + s.y)
        else fc.draw(s.text, R.x + s.x, R.y + s.y) end
      elseif s.bitmap then
        local art = G.screen.art_for(s.bitmap)
        if art then
          love.graphics.setColor(1, 1, 1)
          love.graphics.draw(art.image, love.graphics.newQuad(s.sx, s.sy, s.w, s.h, art.w, art.h),
                             R.x + s.x, R.y + s.y)
        end
      end
    end
  end
  function d.mousepressed() close() end
  function d.keypressed() close() end

  return kit.push(d)
end

M.parse = parse
return M
