-- In the browser (love.js), where the game's files are not on a disk.
--
-- Everything else reads and writes with io.open, by paths relative to the
-- repository root: original/..., pre-rendered-sound/..., the saves and the
-- prefs. In the browser Lua's io only sees Emscripten's own file system, and
-- the game's files are inside the .love, where only love.filesystem reaches
-- them. So here io.open is put back on love.filesystem: a read looks in the
-- save directory first and then in the .love, which the web build lays out
-- as the repository is (tools/build-web.sh); a write goes to the save
-- directory, which love.js keeps in the browser's IndexedDB.
--
-- The desktop game ran on case-blind file systems, and the browser's is not:
-- a path not found as written is looked up again a folder at a time, ignoring
-- case.
--
-- Nothing happens anywhere but the browser.

local web = {}

web.on = love ~= nil and love.system ~= nil and love.system.getOS and love.system.getOS() == "Web"

if not web.on then return web end

local fs = love.filesystem

-- "a/./b//c" -> "a/b/c"
local function clean(path)
  local parts = {}
  for p in path:gmatch("[^/\\]+") do
    if p ~= "." then parts[#parts + 1] = p end
  end
  return table.concat(parts, "/")
end

-- the path as it really is, matched a folder at a time ignoring case
local found = {}
local function resolve(path)
  if fs.getInfo(path) then return path end
  local key = path:lower()
  if found[key] ~= nil then return found[key] or nil end
  local at = ""
  for part in path:gmatch("[^/]+") do
    local want, hit = part:lower(), nil
    for _, item in ipairs(fs.getDirectoryItems(at)) do
      if item:lower() == want then hit = item break end
    end
    if not hit then found[key] = false return nil end
    at = at == "" and hit or at .. "/" .. hit
  end
  found[key] = at
  return at
end

-- a file read whole, handed out the way io's files hand it out
local Reader = {}
Reader.__index = Reader

function Reader:read(fmt)
  fmt = fmt or "*l"
  if type(fmt) == "number" then
    if self.at > #self.data then return nil end
    local s = self.data:sub(self.at, self.at + fmt - 1)
    self.at = self.at + fmt
    return s
  end
  fmt = fmt:gsub("^%*", "")
  if fmt == "a" then
    local s = self.data:sub(self.at)
    self.at = #self.data + 1
    return s
  elseif fmt == "l" or fmt == "L" then
    if self.at > #self.data then return nil end
    local nl = self.data:find("\n", self.at, true)
    local stop = nl or #self.data
    local s = self.data:sub(self.at, fmt == "L" and stop or (nl and nl - 1 or stop))
    self.at = stop + 1
    if fmt == "l" then s = s:gsub("\r$", "") end
    return s
  end
  error("web: read format not handled: " .. tostring(fmt))
end

function Reader:lines()
  return function() return self:read("*l") end
end

function Reader:close() return true end

-- a file written, kept until it is closed
local Writer = {}
Writer.__index = Writer

function Writer:write(...)
  for i = 1, select("#", ...) do self.parts[#self.parts + 1] = tostring((select(i, ...))) end
  return self
end

function Writer:close()
  local dir = self.path:match("^(.*)/[^/]*$")
  if dir then fs.createDirectory(dir) end
  local ok, err = fs.write(self.path, table.concat(self.parts))
  if not ok then return nil, err end
  return true
end

local ioOpen = io.open
function io.open(path, mode)
  mode = mode or "r"
  local p = clean(path)
  if mode:find("[wa]") then
    local w = setmetatable({ path = p, parts = {} }, Writer)
    if mode:find("a") then
      local real = resolve(p)
      w.parts[1] = real and fs.read(real) or ""
    end
    return w
  end
  local real = resolve(p)
  if not real or not (fs.getInfo(real, "file")) then
    return nil, path .. ": No such file or directory"
  end
  local data = fs.read(real)
  if not data then return ioOpen(path, mode) end
  return setmetatable({ data = data, at = 1 }, Reader)
end

return web
