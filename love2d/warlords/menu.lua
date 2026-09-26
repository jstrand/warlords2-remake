-- The original's menu bar.
--
-- Unlike the rest of the interface, this is not read from the game's files:
-- the menu lives in WARLORD2.EXE's data segment, not in a data file, so it is
-- written out here. Recovered in docs/re/ui.md > The menu, which also lists
-- the command each accelerator reaches.
--
-- Layout follows 7ae8:0052 and 2372:049b (below). Nothing is hard-coded, so
-- the bar re-measures itself for whatever font it is given.

local menu = {}

menu.BAR_X, menu.BAR_Y = 8, 1

-- "-" is a separator line. The key is the accelerator, and is what the front
-- end dispatches on, so it matches the command table in docs/re/ui.md.
menu.MENUS = {
  { title = "SSG", items = {
      { "About Warlords II", nil, "?" },
  } },
  { title = "Game", items = {
      { "Settings", "alt X" }, { "Shortcuts", "alt U" }, { "-" },
      { "New game", "alt N" }, { "Save game", "alt S" }, { "Load game", "alt L" },
      { "-" },
      { "Save map", "alt M" }, { "Load map", "alt Z" }, { "-" },
      { "Quit", "^Q" },
  } },
  { title = "Order", items = {
      { "Fight Order", "i" }, { "Move All", "m" }, { "Disband", "q" },
      { "Signpost", "x" }, { "-" }, { "Resign", "r" },
  } },
  { title = "Report", items = {
      { "Army", "a" }, { "City", "k" }, { "Gold", "g" },
      { "Production", "n" }, { "Winning", "w" }, { "-" },
      { "Diplomacy", "d" }, { "-" }, { "Quest", "=" },
  } },
  { title = "Hero", items = {
      { "Inspect", "," }, { "Plant Flag", "f" }, { "Levels", "u" },
      { "Search", "z" },
  } },
  { title = "View", items = {
      { "Army Bonus", "o" }, { "Items", "t" }, { "-" },
      { "Build", "b" }, { "Cities", "c" }, { "Production", "p" },
      { "Vectoring", "v" }, { "Ruins", "." }, { "Stack", "s" },
  } },
  { title = "History", items = {
      { "City", "h" }, { "Events", "e" }, { "Gold", "j" },
      { "Winners", "y" }, { "-" }, { "Triumphs", "l" },
  } },
  { title = "Turn", items = {
      { "End Turn", "alt E" },
  } },
}

--- The menus with the screen's zooms added to View -- not the original's,
--- which had one size of everything. An item for each zoom the map can take
--- and each scale the interface can, run by its act ("map zoom 2",
--- "ui scale 3") and ticked while it is the one in use; and the screen's
--- shape, all of it or the original's own 640x480 with black round it.
function menu.withZooms(maxUI, maxMap)
  local out = {}
  for i, m in ipairs(menu.MENUS) do
    local items = m.items
    if m.title == "View" then
      items = {}
      for _, it in ipairs(m.items) do items[#items + 1] = it end
      items[#items + 1] = { "-" }
      for z = 1, maxMap do items[#items + 1] = { ("Map %dx"):format(z), nil, "map zoom " .. z } end
      items[#items + 1] = { "-" }
      for s = 1, maxUI do items[#items + 1] = { ("Interface %dx"):format(s), nil, "ui scale " .. s } end
      items[#items + 1] = { "-" }
      items[#items + 1] = { "Full screen", nil, "screen full" }
      items[#items + 1] = { "4:3 (original)", nil, "screen 4:3" }
    end
    out[i] = { title = m.title, items = items }
  end
  return out
end

-- The bar is 17 pixels deep -- the font's line height plus two -- and white
-- (7ae8:02d8 fills (0, 0, 640, 17) with colour 15).
menu.BAR_H = 17

--- Measure the bar and every dropdown for a font, the way 7ae8:0052 and
--- 2372:049b do. Returns a table of menus, each with the title's rect
--- (`x`, `w`, text at `x + 2`) and `drop`: the dropdown's rect, the column
--- its accelerators start in, and a rect per row.
---
--- A title's rect is its text width rounded up to 8, plus 8; the next title
--- starts 8 further on still. A dropdown sits at the title's x, one pixel
--- below the bar. Its labels are measured as if 12 pixels in (they are drawn
--- 3 in), the accelerators start in a column 5 past the widest labelled
--- item, and the whole is 5 wider than its widest line. Rows are the line
--- height plus 2, a separator 2, with 2 above the first and 1 below the last.
--- `menus` is the list to lay out, menu.MENUS unless given.
function menu.layout(font, barHeight, screenWidth, menus)
  local out, x = {}, menu.BAR_X
  local lh = font.lineHeight
  for i, m in ipairs(menus or menu.MENUS) do
    local tw = math.ceil(font.width(m.title) / 8) * 8

    local w, keyCol, keyW = 0, 0, 0
    for _, it in ipairs(m.items) do
      local label, key = it[1], it[2]
      if label ~= "-" then
        local lw = 12 + font.width(label)
        if key then
          keyCol = math.max(keyCol, lw + 5)
          keyW = math.max(keyW, font.width(key))
          lw = keyCol + keyW
        end
        w = math.max(w, lw + 5)
      end
    end

    local y = menu.BAR_H + 1
    local rows, at = {}, 2
    for j, it in ipairs(m.items) do
      local h = (it[1] == "-") and 2 or (lh + 2)
      -- a third field is what an item with no accelerator does
      rows[j] = { label = it[1], key = it[2], act = it[3], x = x, y = y + at, w = w, h = h }
      at = at + h
    end
    local h = at + 1

    local dropX = math.min(x, screenWidth - w)
    for _, r in ipairs(rows) do r.x = dropX end
    out[i] = {
      title = m.title, items = m.items, x = x, w = tw + 8,
      drop = { x = dropX, y = y, w = w, h = h, keyCol = keyCol, rows = rows },
    }
    x = x + tw + 16
  end
  return out
end

--- Which menu title is at this point, or nil.
function menu.titleAt(layout, x, y, barHeight)
  if y < 0 or y >= barHeight then return nil end
  for i, m in ipairs(layout) do
    if x >= m.x and x < m.x + m.w then return i end
  end
  return nil
end

--- Which row of an open menu is at this point, or nil. Separators never hit.
function menu.rowAt(layout, index, x, y)
  local m = layout[index]
  if not m then return nil end
  for _, r in ipairs(m.drop.rows) do
    if r.label ~= "-" and x >= r.x and x < r.x + r.w
       and y >= r.y and y < r.y + r.h then
      return r
    end
  end
  return nil
end

return menu
