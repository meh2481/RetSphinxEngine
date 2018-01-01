--Fix obvious exploits
--NOTE: Does not mean that scripts are safe or sandboxed!
os = nil
io = nil


--Set global variables in global var table
local function setglobal(name, v)
	rawset(_G, name, v)
end
setglobal("setglobal", setglobal)

local classes = {}	--List of all loaded classes (so we know to reload them)

--Load a class' lua file from class name
local function loadclass(s, prefix)
	local name = prefix..s
	if rawget(_G, name) == nil then
		local cls = dofile("res/lua/" .. prefix .. "/" .. s .. ".lua")
		setglobal(name, cls)
		local reg = debug.getregistry()	--HACK
		reg[name] = cls
		classes[name] = true
	end
end
setglobal("loadclass", loadclass)

--Clear all loaded classes so we'll reload them when we call loadclass() again
local function clearClasses()
	for k, v in pairs(classes) do
		rawset(_G, k, nil)
		classes[k] = nil
	end
end
setglobal("clearClasses", clearClasses)

--Lua entry point after all game engine loading is done
local function loadLua(map)
	if map == nil then
		mouse_setCursor("res/cursor/arrow.xml")
		map = "res/map/solarsystem.scene.xml"
	end
	map_load(map)
end
setglobal("loadLua", loadLua)

--Load our math functions
dofile("res/lua/math.lua")
