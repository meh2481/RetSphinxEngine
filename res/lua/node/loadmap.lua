local loadmap = {}
loadmap.__index = loadmap

--Called when this node is created
function loadmap:init()
	self.mapfile = node_getProperty(self, "map")
	self.mapnode = node_getProperty(self, "node")
		--print('durr ' .. self.mapnode)
end

--Called when an object enters this node
function loadmap:collide(object)
	--Load a new map when the player collides with this node
	if object == obj_getPlayer() then
		map_load(self.mapfile, self.mapnode)
		--print('durr ' .. self.mapnode)
	end
end

--Called every timestep to update the node
function loadmap:update(dt)
end

--Called when node is destroyed
function loadmap:destroy()
end

return loadmap