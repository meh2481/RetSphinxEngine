local node_test = {}
node_test.__index = node_test
node_test.name = "node_test"

--Called when this node is created
function node_test:init()
	--TODO
	--local strength = node_getProperty(self, "strength")
	--print('node ' .. self.name .. ' created with strength = '..strength)
	--self.strength = strength
end

--Called when an object enters this node
function node_test:collide(object)
	--TODO
	--local x,y = obj_getPos(object)
	
	--print(self.name .. ' colliding with object at '..x..', '..y..' '..self.strength)
end

--Called every timestep to update the node
function node_test:update(dt)
	--TODO
end

--Called when node is destroyed
function node_test:destroy()
	--TODO
	--print('node ' .. self.name .. ' destroyed!')
end

return node_test