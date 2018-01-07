local linearforcefield = {}
linearforcefield.__index = linearforcefield

--Called when this node is created
function linearforcefield:init()
	self.forcex, self.forcey = node_getVec2Property(self, "force")
	
	--print('force: '..self.forcex..', '..self.forcey)
end

--Called when an object enters this node
function linearforcefield:collide(object)
	obj_applyForce(object, self.forcex, self.forcey)
end

--Called every timestep to update the node
function linearforcefield:update(dt)
end

--Called when node is destroyed
function linearforcefield:destroy()
end

return linearforcefield