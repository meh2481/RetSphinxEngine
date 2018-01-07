local gravwell = {}
gravwell.__index = gravwell

--Called when this node is created
function gravwell:init()
	self.force = tonumber(node_getProperty(self, "force"))
	--self.rad = tonumber(node_getProperty(self, "radius"))
	self.posx, self.posy = node_getPos(self)
end

--Called when an object enters this node
function gravwell:collide(object)
	local objx, objy = obj_getPos(object)
	
	--Get vector from object to me
	local x = objx - self.posx
	local y = objy - self.posy
	
	--Save distance between me and object
	local dist = vec2_length(x,y)
	
	--Don't divide by zero; if it's centered over us, just don't apply force this frame
	if dist == 0 then return end
	
	--Normalize the obj->me vector
	x,y = vec2_normalize(x,y)
	
	--Find force we need
	local force = self.force / (dist * dist)	--Note: splodes physics when you get too close
	
	--Multiply the direction vector by this force
	x,y = vec2_mul(x, y, force)
	
	--Apply the force
	obj_applyForce(object, x, y)
end

--Called every timestep to update the node
function gravwell:update(dt)
end

--Called when node is destroyed
function gravwell:destroy()
end

return gravwell