local node_loadmap = {}
node_loadmap.__index = node_loadmap

--Called when this node is created
function node_loadmap:init()
	self.mapfile = node_getProperty(self, "map")
	self.mapnode = node_getProperty(self, "node")
		--print('durr ' .. self.mapnode)
end

--Called when an object enters this node
function node_loadmap:collide(object)
	--Load a new map when the player collides with this node
	if object == obj_getPlayer() then
		map_load(self.mapfile, self.mapnode)
		--print('durr ' .. self.mapnode)
	end
end

--Called every timestep to update the node
function node_loadmap:update(dt)
end

--Called when node is destroyed
function node_loadmap:destroy()
end

return node_loadmap