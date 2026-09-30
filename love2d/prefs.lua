-- The remake's own settings, none of them the original's: the interface's
-- scale, the map's zoom, full screen or a window, 4:3, and the synthesizer
-- the music plays on. Kept beside the saves in warlords-prefs.txt, a line
-- each of a name and its value:
--
--   scale 2
--   zoom 3
--   screen window
--   4:3 on
--   music mt32
--
-- A setting never changed is not written, and is left to its default.

local prefs = {}

prefs.FILE = "warlords-prefs.txt"

local values

local function load()
  values = {}
  local f = io.open(prefs.FILE, "rb")
  if not f then return end
  for line in f:lines() do
    local k, v = line:match("^%s*(%S+)%s+(%S+)")
    if k then values[k] = v end
  end
  f:close()
end

--- A setting's value as written, a string; nil when there is none.
function prefs.get(key)
  if not values then load() end
  return values[key]
end

--- Change a setting, and write them all back if it changed.
function prefs.set(key, value)
  if not values then load() end
  value = value ~= nil and tostring(value) or nil
  if values[key] == value then return end
  values[key] = value
  local keys = {}
  for k in pairs(values) do keys[#keys + 1] = k end
  table.sort(keys)
  local f = io.open(prefs.FILE, "wb")
  if not f then return end
  for _, k in ipairs(keys) do f:write(k, " ", values[k], "\n") end
  f:close()
end

--- Forget what was read, so the next get reads the file again.
function prefs.reload() values = nil end

return prefs
