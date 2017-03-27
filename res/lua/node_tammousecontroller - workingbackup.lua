local node_tammousecontroller = {}
node_tammousecontroller.__index = node_tammousecontroller

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

--Called every timestep to update the node
function node_tammousecontroller:update(dt)
	--obj_setActive(self.walls[4], false)
	if mouse_isDown() then
		--obj_setActive(self.greenYellowWall, false)
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