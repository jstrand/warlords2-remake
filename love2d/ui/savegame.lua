-- Game > Save game and Load game (7721:093b, 7721:026b).
--
-- Ten slots, described in SAVEINFO.DAT -- a line a slot, a three-digit
-- length then the name, "Not_Used" for an empty one. Save lists all ten in
-- the list chooser titled "Save Game" (group 46), on the slot last used;
-- the one chosen asks for its name through the text-entry dialog -- "Type
-- the name of the game / you wish to save", 15 characters, 160 pixels --
-- and is written (7721:0995). Load lists the slots in use (group 47) and
-- tells the side whose turn it is that "thy turn continues!".
--
-- The remake keeps its slots beside its own save file: warlords-save<n>.lua
-- and warlords-saveinfo.txt, in SAVEINFO.DAT's form.

local kit      = require("ui.kit")
local chooseUi = require("ui.choose")
local input    = require("ui.input")
local searchUi = require("ui.search")
local saveMod  = require("warlords.save")

local M = {}

local SLOTS = 10
local UNUSED = "Not_Used"
local INFO = "warlords-saveinfo.txt"

local function slotPath(i) return ("warlords-save%d.lua"):format(i) end

local function readInfo()
  local names = {}
  local f = io.open(INFO, "rb")
  if f then
    for line in f:read("*a"):gmatch("[^\r\n]+") do
      local n, name = line:match("^(%d%d%d)(.*)$")
      if n then names[#names + 1] = name:sub(1, tonumber(n)) end
    end
    f:close()
  end
  for i = #names + 1, SLOTS do names[i] = UNUSED end
  return names
end

local function writeInfo(names)
  local f = io.open(INFO, "wb")
  if not f then return end
  -- the length counts the name's NUL, as SAVEINFO.DAT does (009Not_Used)
  for i = 1, SLOTS do f:write(("%03d%s\r\n"):format(#names[i] + 1, names[i])) end
  f:close()
end

M.last = 1

--- How many slots hold a game (7721:0e25), which is what lets Load game on.
function M.used()
  local n = 0
  for _, name in ipairs(readInfo()) do if name ~= UNUSED then n = n + 1 end end
  return n
end

function M.save()
  local G = kit.G
  local names = readInfo()
  local list = {}
  for i = 1, SLOTS do list[i] = { name = names[i], slot = i } end
  chooseUi.open(kit.text(0x2e, 0), list, M.last, function(e)
    if not e then return end
    M.last = e.slot
    input.open({
      title = kit.text(0x2e, 0), lines = { kit.text(0x2e, 1), kit.text(0x2e, 2) },
      text = e.name ~= UNUSED and e.name or "", maxChars = 15, maxWidth = 160,
      ok = function(name)
        if name == "" then name = UNUSED end
        saveMod.write(G.g, slotPath(e.slot))
        names[e.slot] = name
        writeInfo(names)
      end,
    })
  end)
end

--- `loaded(g)` takes the game read back.
function M.load(loaded)
  local names = readInfo()
  local list = {}
  for i = 1, SLOTS do
    if names[i] ~= UNUSED then list[#list + 1] = { name = names[i], slot = i } end
  end
  if #list == 0 then return nil end
  local start = 1
  for k, e in ipairs(list) do if e.slot == M.last then start = k end end
  chooseUi.open(kit.text(0x2f, 0), list, start, function(e)
    if not e then return end
    local ok, g = pcall(saveMod.read, slotPath(e.slot), kit.G.dataDir)
    if not ok then return end
    M.last = e.slot
    loaded(g)
    local side = g.sides[g.current]
    searchUi.say(kit.text(0x2f, 1):format(side and side.name or ""))
  end)
end

return M
