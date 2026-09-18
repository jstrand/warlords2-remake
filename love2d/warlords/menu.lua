-- The original's menu bar.
--
-- Unlike the rest of the interface, this is not read from the game's files:
-- the menu lives in WARLORD2.EXE's data segment, not in a data file, so it is
-- written out here. Recovered in docs/re/ui.md > The menu, which also lists
-- the command each accelerator reaches.
--
-- Layout follows 7ae8:0052: an item's width is its text width rounded up to a
-- multiple of 8 plus 8 pixels of padding each side, and the bar packs left to
-- right from x = 8. Nothing is hard-coded, so the bar re-measures itself for
-- whatever font it is given.

local menu = {}

menu.BAR_X, menu.BAR_Y = 8, 2
menu.PAD = 8

-- "-" is a separator line. The key is the accelerator, and is what the front
-- end dispatches on, so it matches the command table in docs/re/ui.md.
menu.MENUS = {
  { title = "SSG", items = {
      { "About Warlords II" },
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

--- Measure the bar and every dropdown for a font. Returns a table of menus
--- with `x`, `w` on each, and `drop` = the dropdown rect and its item rects.
function menu.layout(font, barHeight, screenWidth)
  local out, x = {}, menu.BAR_X
  for i, m in ipairs(menu.MENUS) do
    local tw = math.ceil(font.width(m.title) / 8) * 8
    local w = tw + menu.PAD * 2

    -- the dropdown is as wide as its widest line, label and key together
    local itemW, lines = 0, {}
    for j, it in ipairs(m.items) do
      local label, key = it[1], it[2]
      local lw = (label == "-") and 0
                 or font.width(label) + (key and (font.width(key) + 16) or 0)
      itemW = math.max(itemW, lw)
      lines[j] = { label = label, key = key }
    end
    itemW = itemW + menu.PAD * 2

    local dropX = math.min(x, screenWidth - itemW - 1)
    local y = barHeight
    local rows = {}
    for j, ln in ipairs(lines) do
      local h = (ln.label == "-") and 4 or (font.lineHeight + 2)
      rows[j] = { label = ln.label, key = ln.key, x = dropX, y = y, w = itemW, h = h }
      y = y + h
    end

    out[i] = {
      title = m.title, items = m.items, x = x, w = w,
      drop = { x = dropX, y = barHeight, w = itemW, h = y - barHeight, rows = rows },
    }
    x = x + w
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
