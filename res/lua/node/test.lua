local test = {}
test.__index = test
test.name = "test"

--Called when this node is created
function test:init()
	--TODO
	--local strength = node_getProperty(self, "strength")
	--print('node ' .. self.name .. ' created with strength = '..strength)
	--self.strength = strength
end

--Called when an object enters this node
function test:collide(object)
	--TODO
	--local x,y = obj_getPos(object)
	
	--print(self.name .. ' colliding with object at '..x..', '..y..' '..self.strength)
end

--Called every timestep to update the node
function test:update(dt)
	--TODO
end

--Called when node is destroyed
function test:destroy()
	--TODO
	--print('node ' .. self.name .. ' destroyed!')
end

return test