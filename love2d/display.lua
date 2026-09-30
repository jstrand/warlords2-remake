-- The screen, in real pixels, and the two scales everything is drawn at.
--
-- Not the original's: it drew 640x480 and nothing else. Here there are three
-- kinds of coordinate:
--
--   device  the display's own pixels (LOVE's "pixel dimensions"). The game
--           puts the display in its own full-resolution mode where it can
--           (display.openWindow), so these are the panel's real pixels and
--           nothing resamples them on the way.
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
display.pw, display.ph = 640, 480   -- what the frame may use, below `top`
display.top = 0                    -- device rows kept clear at the top: the notch
display.scale = 1
display.maxScale = 1               -- the biggest the screen allows
display.chosen = nil               -- a scale picked from the menu, if any
display.windowed = false           -- in a window on the desktop, not full screen

-- the full-screen mode openWindow chose, and its notch strip, to go back to;
-- and the window's last size and place, to come back to
local fullMode, fullTop
local windowAt

-- the frame being drawn: its size in UI pixels and where its (0, 0) sits
display.w, display.h = 640, 480
display.ox, display.oy = 0, 0

local lg = love.graphics

--- Open the game's window, full screen in the display's own biggest mode --
--- on a Mac in a scaled resolution, the panel's real pixels rather than a
--- bigger picture macOS then shrinks to fit. Only when the window has been
--- left to the game (conf.lua's t.window = nil); a harness that opened its
--- own keeps it.
---
--- A notched panel hides a strip along its top. macOS keeps a desktop
--- full-screen window out of it but says nothing of it in a mode of the
--- game's own, so the window opens desktop full screen first, and the strip
--- that leaves at the top is kept clear once the mode has changed. Only a Mac
--- has one: anywhere else desktop full screen is the whole screen anyway, and
--- the measuring is not trusted to say so.
function display.openWindow(title)
  local win = love.window
  if not (win and win.setMode and win.isOpen) or win.isOpen() then return end
  -- in the browser the window is the page's canvas: twice the original, and
  -- the pointer free to leave it (web.lua)
  if love.system and love.system.getOS and love.system.getOS() == "Web" then
    win.setMode(display.W * 2, display.H * 2, { resizable = true })
    display.windowed = true
    if title then win.setTitle(title) end
    return
  end
  local _, deskH = win.getDesktopDimensions()
  win.setMode(0, 0, { fullscreen = true, fullscreentype = "desktop", highdpi = true })
  local _, fullH = lg.getDimensions()
  local mac = love.system and love.system.getOS and love.system.getOS() == "OS X"
  local strip = mac and math.max(0, deskH - fullH) / deskH or 0
  local best
  for _, m in ipairs(win.getFullscreenModes()) do
    if not best or m.width * m.height > best.width * best.height then best = m end
  end
  if best and win.setMode(best.width, best.height,
                          { fullscreen = true, fullscreentype = "exclusive", highdpi = true }) then
    local _, ph = lg.getPixelDimensions()
    display.top = math.ceil(strip * ph)
    fullMode, fullTop = best, display.top
  end
  if title then win.setTitle(title) end
end

--- The system's cursor is hidden over the game, which draws its own, and in
--- full screen the pointer is kept in the window. In a window it is free to
--- leave, and the system's shows over the black round the frame.
local away = false                 -- in a window, the pointer is off the frame
local function capture()
  if love.mouse.setVisible then love.mouse.setVisible(display.windowed and away) end
  if love.mouse.setGrabbed then love.mouse.setGrabbed(not display.windowed) end
end

--- Play in an ordinary window on the desktop, resizable, or back in the full
--- screen openWindow opened (View > Window, Full screen). The window opens
--- at four-fifths of the desktop, or at the size and place it last had, and
--- is never smaller than the original's 640x480. Without a window the game
--- opened itself (a harness) only the setting changes.
function display.setWindowed(on)
  if on == display.windowed then return end
  local win = love.window
  if fullMode and win and win.setMode then
    if on then
      local w, h, x, y
      if windowAt then
        w, h, x, y = windowAt[1], windowAt[2], windowAt[3], windowAt[4]
      else
        local dw, dh = win.getDesktopDimensions()
        w = math.max(display.W, math.floor(dw * 0.8))
        h = math.max(display.H, math.floor(dh * 0.8))
      end
      if not win.setMode(w, h, { fullscreen = false, resizable = true, highdpi = true,
                                 minwidth = display.W, minheight = display.H,
                                 x = x, y = y }) then return end
      display.top = 0
    else
      local w, h = win.getMode()
      local x, y = win.getPosition()
      windowAt = { w, h, x, y }
      if not win.setMode(fullMode.width, fullMode.height,
                         { fullscreen = true, fullscreentype = "exclusive", highdpi = true }) then
        return
      end
      display.top = fullTop
    end
  end
  display.windowed = on
  away = false
  if love.mouse then capture() end
  display.measure()
end

--- Is the pointer off the game, so that it draws none? Only ever in a
--- window: off the frame, or out of the window altogether.
function display.pointerAway()
  if not display.windowed then return false end
  local win = love.window
  return away or (win and win.hasMouseFocus and not win.hasMouseFocus()) or false
end

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
  display.ph = display.ph - display.top
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
  display.oy = display.top + math.floor((display.ph - h * display.scale) / 2)
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
    -- The real pointer is kept inside the frame: past its edge the game's own
    -- pointer would stop while the real one went on, into the black round
    -- the frame or the notch's strip, where the system shows a cursor of its
    -- own. The warp back reports a move of its own, which is not the
    -- player's and is passed on as none. In a window the pointer goes where
    -- it likes, and off the frame the system's cursor takes over from the
    -- game's.
    local warpedTo
    local moved = love.handlers.mousemoved
    love.handlers.mousemoved = function(x, y, dx, dy, ...)
      local k = display.dpi / display.scale
      if warpedTo and x == warpedTo[1] and y == warpedTo[2] then
        warpedTo, dx, dy = nil, 0, 0
      else
        local d = display.dpi
        local x0, y0 = display.ox / d, display.oy / d
        local x1 = (display.ox + display.w * display.scale - 1) / d
        local y1 = (display.oy + display.h * display.scale - 1) / d
        local cx, cy = math.max(x0, math.min(x1, x)), math.max(y0, math.min(y1, y))
        if display.windowed then
          local off = cx ~= x or cy ~= y
          if off ~= away then
            away = off
            capture()
          end
        elseif (cx ~= x or cy ~= y) and love.mouse.setPosition then
          love.mouse.setPosition(cx, cy)
          warpedTo = { cx, cy }
        end
      end
      x, y = display.toUI(x, y)
      return moved(x, y, dx * k, dy * k, ...)
    end

    -- no system cursor over the game, and in full screen the pointer kept in
    -- its window; both again whenever the game has the focus back
    capture()
    local focus = love.handlers.focus
    love.handlers.focus = function(f, ...)
      if f then capture() end
      if focus then return focus(f, ...) end
    end
  end
end

return display
