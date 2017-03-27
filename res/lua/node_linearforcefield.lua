local node_linearforcefield = {}
node_linearforcefield.__index = node_linearforcefield

--Called when this node is created
function node_linearforcefield:init()
	self.forcex, self.forcey = node_getVec2Property(self, "force")
	
	--print('force: '..self.forcex..', '..self.forcey)
end

--Called when an object enters this node
function node_linearforcefield:collide(object)
	obj_applyForce(object, self.forcex, self.forcey)
end

--Called every timestep to update the node
function node_linearforcefield:update(dt)
end

--Called when node is destroyed
function node_linearforcefield:destroy()
end

return node_linearforcefield