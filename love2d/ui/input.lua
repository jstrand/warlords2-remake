-- The original's one text-entry dialog, used for renaming a city and for
-- anything else that asks for a line of text.
--
-- 7b4c:0000 takes a title, up to two lines of prompt, the text to edit, and
-- limits on its length; 7b4c:00e2 pushes popup 1 -- (160, 90) 320x200 -- and
-- draws:
--
--   the title in font 1, centred on x = 320 at y = 92
--   the prompt in font 2, centred on 320: one line at y = 150, two at 140
--     and 163 (the table at 4125:2316, a row per line count)
--   the field (7ecb:0058) at (184, 197) 256x20, showing the text as it is
--
-- over dialog 5: OK (189) at (400, 260), Cancel (190) at (176, 260), and the
-- field's own hit area (191). Cancel is greyed when the caller allows none.
--
-- Clicking the field starts an edit (7b4c:03a6), and the edit starts from an
-- EMPTY line rather than the old text: typing replaces the name, it does not
-- append to it. A character from 0x20 to 0x7a is taken while the line is
-- shorter than the limit and narrower than the width limit; Backspace takes
-- one back; Enter keeps the line and Escape puts the old text back. The
-- cursor is a "`" -- the font's own cursor glyph -- blinking between colours
-- 15 and 9.
--
-- Escape and Enter outside an edit are Cancel and OK: 190 and 189 head the
-- original's lists of cancel and default buttons (4125:1514, :155c).
--
-- The same dialog asks yes-or-no questions (7b4c:0088): no field, up to four
-- lines of prompt, and OK and Cancel -- Raze City is one.

local kit    = require("ui.kit")
local uidata = require("warlords.uidata")

local M = {}

local R = { x = 160, y = 90, w = 320, h = 200 }     -- popup 1
local DIALOG = 5
local OK, CANCEL, FIELD = 189, 190, 191
local TITLE_Y = 92
-- Where the prompt goes, by how many lines there are: the table at
-- 4125:2316, a row of four points per count. The text-entry form counts two
-- more lines than it has, for the field, so its two lines take row 4.
local ROWS = {
  [1] = { 150 }, [2] = { 150, 173 }, [3] = { 150, 173, 196 },
  [4] = { 140, 163, 186, 209 },
}
local BOX = { x = 184, y = 197, w = 256, h = 20 }
local CURSOR = "`"
local BLINK = 0.25                                   -- seconds each way

local function now()
  return (love.timer and love.timer.getTime and love.timer.getTime()) or 0
end

--- A line being typed into a field (7b4c:03a6). It starts EMPTY; a
--- character from 0x20 to 0x7a goes in while the line is shorter than
--- `maxChars` - 1 and, measured before it goes in, narrower than `maxWidth`.
--- key() answers "keep" for Enter, "undo" for Escape.
function M.editor(maxChars, maxWidth)
  local e = { text = "" }
  function e.key(key)
    if key == "return" or key == "kpenter" then return "keep"
    elseif key == "escape" then return "undo"
    elseif key == "backspace" then e.text = e.text:sub(1, -2)
    end
  end
  function e.input(t)
    for ch in t:gmatch(".") do
      local b = ch:byte()
      if b >= 0x20 and b < 0x7b and #e.text < maxChars - 1
         and kit.font(2).width(e.text) < maxWidth then
        e.text = e.text .. ch
      end
    end
  end
  --- the cursor, after the text in a field at (x, y), blinking 15 and 9
  function e.drawCursor(x, y)
    local f = kit.font(2)
    if math.floor(now() / BLINK) % 2 == 0 then kit.setPal(9) else love.graphics.setColor(1, 1, 1) end
    f.draw(CURSOR, x + f.width(e.text) + 5, y + 2)
  end
  return e
end

--- Ask for a line of text.
---   opts.title, opts.lines   the title and up to two lines of prompt
---   opts.text                what the field holds to start with
---   opts.maxChars            the longest line, counting the NUL -- a city
---                            name's 16 bytes give 15 (7204:2013 passes 15)
---   opts.maxWidth            the widest line in pixels (128 for a city)
---   opts.cancel              false to grey Cancel out
---   opts.confirm             a question, with no field to type in
---   opts.ok(text), opts.cancelled()
function M.open(opts)
  local d = {
    view = kit.view(DIALOG),
    text = opts.text or "",
    editing = nil,                                  -- the line being typed
  }
  local maxChars = opts.maxChars or 15
  local maxWidth = opts.maxWidth or 232              -- 7b4c:0000's default
  d.view.state[OK] = uidata.NORMAL
  d.view.state[CANCEL] = (opts.cancel == false) and uidata.DISABLED or uidata.NORMAL

  local function finish(ok)
    kit.pop(d)
    if ok then
      if opts.ok then opts.ok(d.text) end
    elseif opts.cancelled then
      opts.cancelled()
    end
  end

  function d.draw()
    local G = kit.G
    kit.popup(R)
    love.graphics.setColor(1, 1, 1)
    kit.centred(kit.font(1), opts.title or "", 320, TITLE_Y)
    local lines = opts.lines or {}
    local ys = ROWS[opts.confirm and #lines or #lines + 2]
    if ys then
      for i, line in ipairs(lines) do kit.centred(kit.font(2), line, 320, ys[i]) end
    end

    if opts.confirm then kit.drawControls(d.view, { [FIELD] = true }) return end
    kit.field(BOX.x, BOX.y, BOX.w, BOX.h, d.editing and d.editing.text or d.text, kit.font(2))
    if d.editing then d.editing.drawCursor(BOX.x, BOX.y) end

    kit.drawControls(d.view)
  end

  function d.mousepressed(x, y)
    if d.editing then d.text, d.editing = d.editing.text, nil end
    local c = kit.controlAt(d.view, x, y, opts.confirm and { [FIELD] = true } or nil)
    if not c then return end
    if c.id == OK then finish(true)
    elseif c.id == CANCEL then finish(false)
    elseif c.id == FIELD then d.editing = M.editor(maxChars, maxWidth)
    end
  end

  function d.keypressed(key)
    if d.editing then
      local r = d.editing.key(key)
      if r == "keep" then d.text, d.editing = d.editing.text, nil
      elseif r == "undo" then d.editing = nil end
      return
    end
    if key == "return" or key == "kpenter" then finish(true)
    elseif key == "escape" and d.view.state[CANCEL] ~= uidata.DISABLED then finish(false)
    end
  end

  function d.textinput(t)
    if d.editing then d.editing.input(t) end
  end

  return kit.push(d)
end

return M
