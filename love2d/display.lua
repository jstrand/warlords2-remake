-- The screen, in real pixels, and the two scales everything is drawn at.
--
-- Not the original's: it drew 640x480 and nothing else. Here there are three
-- kinds of coordinate:
--
--   device  the display's own pixels (LOVE's "pixel dimensions")
--   UI      the interface's pixels: every rect in the game's data, the menu
--           bar, the dialogs, the pointer. One UI pixel is `scale` device
--           pixels across, a whole number, so the art stays sharp.
--   map     the map's own pixels, 40 to a tile, drawn at `zoom` device
--           pixels each -- a whole number too, and apart from the UI's, so
--           the map can be zoomed while the chrome stays put.
--
-- Handlers and hit tests all work in UI pixels: the mouse is brought down to
-- them before main.lua sees it. The start screens are the original's 640x480,
-- centred; the game itself is as many UI pixels as the screen holds.
--
-- love.graphics.setScissor works in window units and ignores the transform,
-- so it is wrapped here to go through the transform first. That is what lets
-- every dialog keep calling it with its own coordinates.

local display = {}

display.W, display.H = 640, 480    -- the original's screen, and the smallest

-- the device, as last measured
display.dpi = 1
display.pw, display.ph = 640, 480
display.scale = 1
display.maxScale = 1               -- the biggest the screen allows
display.chosen = nil               -- a scale picked from the menu, if any

-- the frame being drawn: its size in UI pixels and where its (0, 0) sits
display.w, display.h = 640, 480
display.ox, display.oy = 0, 0

local lg = love.graphics

--- Read the window's size, and pick the UI's scale: the one chosen from the
--- menu, or else the biggest whole number that still fits the original's
--- 640x480, which the start screens need.
function display.measure()
  display.dpi = lg.getDPIScale and lg.getDPIScale() or 1
  if lg.getPixelDimensions then
    display.pw, display.ph = lg.getPixelDimensions()
  else
    local w, h = lg.getDimensions()
    display.pw, display.ph = w * display.dpi, h * display.dpi
  end
  display.maxScale = math.max(1, math.floor(math.min(display.pw / display.W,
                                                     display.ph / display.H)))
  display.scale = math.min(display.chosen or display.maxScale, display.maxScale)
end

--- Draw the UI at `n` device pixels a pixel from now on, as far as it fits.
function display.choose(n)
  display.chosen = math.max(1, math.floor(n))
  display.measure()
end

--- How many UI pixels the game gets: the whole screen, or `display.wanted`
--- ({ w, h }) where that is set and fits.
function display.uiSize()
  local w = math.floor(display.pw / display.scale)
  local h = math.floor(display.ph / display.scale)
  local want = display.wanted
  if want then w, h = math.min(w, want.w), math.min(h, want.h) end
  return w, h
end

--- Draw a frame of w x h UI pixels from here on, centred on the screen.
function display.setFrame(w, h)
  display.w, display.h = w, h
  display.ox = math.floor((display.pw - w * display.scale) / 2)
  display.oy = math.floor((display.ph - h * display.scale) / 2)
end

--- Draw in UI pixels until the matching pop.
function display.pushUI()
  lg.push()
  lg.origin()
  lg.translate(display.ox / display.dpi, display.oy / display.dpi)
  lg.scale(display.scale / display.dpi)
end

--- Draw in map pixels until the matching pop, one map pixel to `zoom`
--- device pixels. The map is slid `camX, camY` device pixels left and up
--- from the UI point (x, y): the map's own (0, 0) sits that far off.
function display.pushMap(x, y, camX, camY, zoom)
  lg.push()
  lg.origin()
  local dx = display.ox + x * display.scale - camX
  local dy = display.oy + y * display.scale - camY
  lg.translate(dx / display.dpi, dy / display.dpi)
  lg.scale(zoom / display.dpi)
end

--- A window position, in LOVE's units, in UI pixels -- held to the frame.
function display.toUI(x, y)
  local s = display.scale
  x = math.floor((x * display.dpi - display.ox) / s)
  y = math.floor((y * display.dpi - display.oy) / s)
  return math.max(0, math.min(display.w - 1, x)), math.max(0, math.min(display.h - 1, y))
end

--- Put the conversions in place: the mouse, as main.lua sees it, is in UI
--- pixels, and a scissor goes through the transform. Without a real LOVE
--- (the test harness) there is nothing to convert, and nothing is changed.
function display.install()
  if lg.setDefaultFilter then lg.setDefaultFilter("nearest", "nearest") end
  if lg.setLineStyle then lg.setLineStyle("rough") end

  if lg.transformPoint then
    local setScissor = lg.setScissor
    lg.setScissor = function(x, y, w, h)
      if not x then return setScissor() end
      local x1, y1 = lg.transformPoint(x, y)
      local x2, y2 = lg.transformPoint(x + w, y + h)
      setScissor(math.min(x1, x2), math.min(y1, y2), math.abs(x2 - x1), math.abs(y2 - y1))
    end
  end

  if love.handlers and love.mouse and love.mouse.getPosition then
    local getPosition = love.mouse.getPosition
    love.mouse.getPosition = function() return display.toUI(getPosition()) end
    for _, name in ipairs({ "mousepressed", "mousereleased" }) do
      local handle = love.handlers[name]
      love.handlers[name] = function(x, y, ...)
        x, y = display.toUI(x, y)
        return handle(x, y, ...)
      end
    end
    local moved = love.handlers.mousemoved
    love.handlers.mousemoved = function(x, y, dx, dy, ...)
      x, y = display.toUI(x, y)
      local k = display.dpi / display.scale
      return moved(x, y, dx * k, dy * k, ...)
    end
  end
end

return display
