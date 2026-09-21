-- The eight army slots under the map, and which of their armies move.
--
-- Clicking a tile does not select the whole stack: it selects one army, and
-- the bottom bar is where the player builds up the group that will move. The
-- original keeps three parallel arrays over the tile's armies -- the group
-- each belongs to, whether that group is the one moving, and the mark drawn
-- under it -- and four handlers that change them. 89e0:0d30 builds them,
-- 89e0:17e7 sorts and marks them, 89e0:000a writes the grouping back into the
-- armies, and 89e0:0963 / 0910 / 0a55 / 0a99 are the four ways a player
-- changes them. docs/re/ui.md > The army slots.
--
-- Pure Lua: this module never touches love.*, so the model can be exercised
-- without a window.

local slots = {}

slots.MAX = 8

-- The mark under a slot (89e0:17e7): the head of the group that moves takes
-- the tick, the head of every other group the cross, and the armies below a
-- head take nothing. They are the two 32x16 cells at the right-hand end of
-- ABITS.PCK.
slots.CROSS, slots.TICK = 0, 1

-- An army remembers its group in its own record, so a stack you grouped is
-- still grouped when you come back to it. Group 0 means "not grouped": each
-- such army is a group of its own.
slots.UNGROUPED = 0

--- A slot's army sorts by its owner's fight order, highest first -- the same
--- table that decides which army a tile shows. docs/rules.md > Combat.
local function rank(g, s, i)
  local row = g.map.fightOrder[s.side or 8]
  return row and row[s.army[i].type] or 0
end

local function swap(s, i, j)
  s.army[i],    s.army[j]    = s.army[j],    s.army[i]
  s.group[i],   s.group[j]   = s.group[j],   s.group[i]
  s.inGroup[i], s.inGroup[j] = s.inGroup[j], s.inGroup[i]
end

--- Insertion-sort the slots so each group is contiguous: by group, then by
--- fight order within it (89e0:17e7 with its first argument set).
local function order(s, g)
  for i = 2, s.n do
    local j = i
    while j > 1 and (s.group[j] < s.group[j - 1]
                     or (s.group[j] == s.group[j - 1]
                         and rank(g, s, j - 1) < rank(g, s, j))) do
      swap(s, j, j - 1)
      j = j - 1
    end
  end
end

--- Work out the marks, and which group is the one that moves. A group is the
--- moving one only when the selection is *exactly* that whole group, so a
--- half-selected group shows no tick at all (89e0:17e7's tail).
local function remark(s)
  for i = 1, s.n do
    if i > 1 and s.group[i] == s.group[i - 1] then
      s.mark[i] = nil                          -- not the head of its group
    else
      s.mark[i] = slots.CROSS
    end
  end
  s.active = nil

  local head
  for i = 1, s.n do
    if s.inGroup[i] then head = i break end
  end
  if not head then return end
  for i = 1, s.n do
    if s.inGroup[i] ~= (s.group[i] == s.group[head]) then return end
  end
  s.mark[head], s.active = slots.TICK, s.group[head]
end

--- Redraw-time bookkeeping: `resort` re-sorts first, as 89e0:17e7's argument
--- selects. The two handlers that only change which group moves do not.
local function refresh(s, g, resort)
  if resort then order(s, g) end
  remark(s)
  return s
end

--------------------------------------------------------------------- building

--- The slots for the armies standing on one tile, at most eight of them.
--- `selected` is the set of armies that should be moving; left out, only the
--- first slot moves -- the army the tile itself shows -- and the rest wait in
--- the bar to be added.
function slots.build(g, armies, side, selected)
  local s = { n = math.min(#armies, slots.MAX), side = side,
              army = {}, group = {}, inGroup = {}, mark = {} }
  for i = 1, s.n do s.army[i] = armies[i] end

  -- 1b62:0a03's order: the remembered groups first, and within a group the
  -- army highest in the fight order.
  local at = {}
  for i = 1, s.n do at[s.army[i]] = i end
  table.sort(s.army, function(p, q)
    local gp, gq = p.group or 0, q.group or 0
    if gp ~= gq then return gp > gq end
    local rowT = g.map.fightOrder[side or 8]
    local rp = rowT and rowT[p.type] or 0
    local rq = rowT and rowT[q.type] or 0
    if rp ~= rq then return rp > rq end
    return at[p] < at[q]
  end)

  -- 89e0:0d30 numbers the groups off down the list: a run of armies sharing
  -- one remembered group is one group, and anything ungrouped is its own.
  local n, previous = -1, nil
  for i = 1, s.n do
    local key = s.army[i].group or slots.UNGROUPED
    if not (key == previous and key ~= slots.UNGROUPED and n >= 0) then
      n = n + 1
    end
    s.group[i] = n
    s.inGroup[i] = selected and selected[s.army[i]] == true or (not selected and i == 1)
    previous = key
  end
  return refresh(s, g)
end

--------------------------------------------------------------------- changing

--- Click a slot (controls 224-231, 89e0:0963): add that army to the moving
--- group, or drop it out into a group of its own. The last army of a group
--- cannot be dropped -- something must move.
function slots.toggle(s, g, i)
  if i < 1 or i > s.n then return s end
  if not s.inGroup[i] then
    for j = 1, s.n do
      if s.inGroup[j] then s.group[i] = s.group[j] break end
    end
    s.inGroup[i] = true
  else
    local members = {}
    for j = 1, s.n do
      members[s.group[j]] = (members[s.group[j]] or 0) + 1
    end
    if members[s.group[i]] ~= 1 then
      local free = 0
      while members[free] do free = free + 1 end
      s.inGroup[i], s.group[i] = false, free
    end
  end
  return refresh(s, g, true)
end

--- Click the mark under a slot (controls 232-239, 89e0:0910): make that
--- army's group the one that moves, and nothing else.
function slots.pickGroup(s, g, i)
  if i < 1 or i > s.n then return s end
  local want = s.group[i]
  for j = 1, s.n do s.inGroup[j] = s.group[j] == want end
  return refresh(s, g)
end

--- The Grp button while it is red, and the space bar (control 240,
--- 89e0:0a55): put the whole stack in one group and move it together.
function slots.all(s, g)
  for i = 1, s.n do s.group[i], s.inGroup[i] = 0, true end
  return refresh(s, g, true)
end

--- The Grp button while it is green (control 241, 89e0:0a99): break the
--- stack up again, leaving only the first army moving.
function slots.single(s, g)
  for i = 1, s.n do s.group[i], s.inGroup[i] = i - 1, false end
  if s.n > 0 then s.inGroup[1] = true end
  return refresh(s, g, true)
end

--------------------------------------------------------------------- asking

--- The armies that move: the ones in the group that moves.
function slots.selected(s)
  local out = {}
  for i = 1, s.n do
    if s.inGroup[i] then out[#out + 1] = s.army[i] end
  end
  return out
end

--- What the bar shows as Group Move: the whole group travels at the pace of
--- its slowest army (1c8c:0912).
function slots.moves(s)
  local least
  for i = 1, s.n do
    if s.inGroup[i] then
      local m = s.army[i].moves or 0
      if not least or m < least then least = m end
    end
  end
  return least or 0
end

--- Whether the Grp button shows green: the whole stack moves as one
--- (89e0:0567).
function slots.grouped(s)
  if s.n == 0 then return false end
  if s.n == 1 then return true end
  for i = 1, s.n do
    if not s.inGroup[i] then return false end
  end
  return true
end

--- Rebuild the slots over a fresh list of armies -- the survivors of a
--- battle, or whoever stands on the tile the group has just walked to --
--- keeping whichever of them were moving. Returns nil when none are left.
function slots.keep(s, g, armies)
  local was = {}
  for i = 1, s.n do
    if s.inGroup[i] then was[s.army[i]] = true end
  end
  local selected, any = {}, false
  for _, a in ipairs(armies) do
    if was[a] then selected[a], any = true, true end
  end
  if #armies == 0 then return nil end
  return slots.build(g, armies, s.side, any and selected or nil)
end

--------------------------------------------------------------------- writing

--- Write the grouping back into the armies, so the stack is still grouped
--- next time it is picked up (89e0:000a's tail). A group of one is not a
--- group; the one that moves takes a fresh id, and any other group of more
--- than one shares id 1 until it is picked up again.
function slots.commit(s, g)
  local members = {}
  for i = 1, s.n do
    members[s.group[i]] = (members[s.group[i]] or 0) + 1
  end
  -- the original hands out ids from a counter of its own (1b62:0cde); the
  -- lowest id no army is using does as well and cannot drift.
  local used = {}
  for _, a in ipairs(g.armies) do used[a.group or 0] = true end
  local fresh = 2
  while used[fresh] do fresh = fresh + 1 end

  for i = 1, s.n do
    local id
    if members[s.group[i]] == 1 then id = slots.UNGROUPED
    elseif s.group[i] == s.active then id = fresh
    else id = 1 end
    s.army[i].group = id
  end
  return s
end

return slots
