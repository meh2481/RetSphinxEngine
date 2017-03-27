local node_tammousecontroller = {}
node_tammousecontroller.__index = node_tammousecontroller

local REDNONE = 1
local REDGREEN = 2
local NONEYELLOW = 3
local GREENYELLOW = 4
local GREENPURPLE = 5
local YELLOWBLUE = 6
local PURPLEBLUE = 7

--Called when this node is created
function node_tammousecontroller:init()
	--Create walls
	self.walls = {}
	local wall = obj_create("tamwall", 0, 3)
	obj_setActive(wall, false)
	table.insert(self.walls, wall)
	wall = obj_create("tamwall", -1.5, 1.5)
	obj_setActive(wall, false)
	obj_setAngle(wall, math.pi / 2.0)
	table.insert(self.walls, wall)
	wall = obj_create("tamwall", 1.5, 1.5)
	obj_setAngle(wall, math.pi / 2.0)
	table.insert(self.walls, wall)
	wall = obj_create("tamwall")
	table.insert(self.walls, wall)
	wall = obj_create("tamwall", -1.5, -1.5)
	obj_setAngle(wall, math.pi / 2.0)
	table.insert(self.walls, wall)
	wall = obj_create("tamwall", 1.5, -1.5)
	obj_setAngle(wall, math.pi / 2.0)
	table.insert(self.walls, wall)
	wall = obj_create("tamwall", 0, -3)
	table.insert(self.walls, wall)
end

--Called when an object enters this node
function node_tammousecontroller:collide(object)
end

--Return false if any objects are in the node of this name, true if none are
local function testObj(name)
	local hole = node_get(name)
	local objsColliding = node_getCollidingObj(hole)
	for k, v in pairs(objsColliding) do
		if v ~= nil then
			local x, y = obj_getPos(v)
			if node_isInside(hole, x, y) then
				return false
			end
		end
	end
	return true
end

--Called every timestep to update the node
function node_tammousecontroller:update(dt)
	--Set all walls to active
	for i = 1,7 do
		obj_setActive(self.walls[i], true)
	end
	
	--Set walls to inactive as needed
	if testObj("holeblank") then
		obj_setActive(self.walls[REDNONE], false)
		obj_setActive(self.walls[NONEYELLOW], false)
	end
	
	if testObj("holered") then
		obj_setActive(self.walls[REDNONE], false)
		obj_setActive(self.walls[REDGREEN], false)
	end
	
	if testObj("holegreen") then
		obj_setActive(self.walls[GREENYELLOW], false)
		obj_setActive(self.walls[REDGREEN], false)
		obj_setActive(self.walls[GREENPURPLE], false)
	end
	
	if testObj("holeyellow") then
		obj_setActive(self.walls[GREENYELLOW], false)
		obj_setActive(self.walls[NONEYELLOW], false)
		obj_setActive(self.walls[YELLOWBLUE], false)
	end
	
	if testObj("holepurple") then
		obj_setActive(self.walls[GREENPURPLE], false)
		obj_setActive(self.walls[PURPLEBLUE], false)
	end
	
	if testObj("holeblue") then
		obj_setActive(self.walls[PURPLEBLUE], false)
		obj_setActive(self.walls[YELLOWBLUE], false)
	end
	

	--Drag marbles around with the mouse
	if mouse_isDown() then
		local x, y = mouse_getPos()
		x, y = mouse_transformToWorld(x, y)
		local marble = obj_getFromPoint(x, y)
		if marble ~= nil then
			obj_setPos(marble, x, y)
		end
	end
end

--Called when node is destroyed
function node_tammousecontroller:destroy()
end

return node_tammousecontroller