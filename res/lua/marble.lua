local marble = {}
marble.__index = marble

--Called when this node is created
function marble:init()
	local id = obj_getProperty(self, "id")
	obj_setImage(self, "res/gfx/"..id..".png")
end

--Called when an object enters this node
function marble:collide(object)
end

--Called when this object hits a wall
function marble:collidewall(wallnormalX, wallnormalY)
end

--Called every timestep to update the node
function marble:update(dt)
end

--Called when node is destroyed
function marble:destroy()
end

return marble