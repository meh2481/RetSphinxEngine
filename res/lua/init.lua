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

--Load a class' lua definition from a string
local function loadclass(s, def)
	if rawget(_G, s) == nil then
	    local classFunc = load(def)
		local cls = classFunc()
		setglobal(s, cls)
		local reg = debug.getregistry()	--HACK
		reg[s] = cls
		classes[s] = true
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
