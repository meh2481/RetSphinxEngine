local radialforcefield = {}
radialforcefield.__index = radialforcefield

--Called when this node is created
function radialforcefield:init()
	self.forcecenter = tonumber(node_getProperty(self, "forceatcenter"))
	self.forceedge = tonumber(node_getProperty(self, "forceatedge"))
	self.rad = tonumber(node_getProperty(self, "radius"))
	self.posx, self.posy = node_getPos(self)
	self.dsp = audio_createLowpassFilter(5000.0)
	audio_deactivateFilter(self.dsp)
end

--Called when an object enters this node
function radialforcefield:collide(object)
	local objx, objy = obj_getPos(object)
	
	--Get vector from object to me
	local x = objx - self.posx
	local y = objy - self.posy
	
	--Save distance between me and object
	local dist = vec2_length(x,y)
	
	--Don't divide by zero; if it's centered over us, just don't apply force this frame
	--Also ignore if they're outside our radius (so we don't push away because math)
	if dist == 0 or dist > self.rad then return end
	
	--Normalize the obj->me vector
	x,y = vec2_normalize(x,y)
	
	--Find force we need
	local fac = dist / self.rad	--Factor for how far away from me the object is
	
	--lerp force applied between forceatcenter and forceatedge
	local force = fac * (self.forceedge - self.forcecenter) + self.forcecenter
	
	--Multiply the direction vector by this force
	x,y = vec2_mul(x, y, force)
	
	--Apply the force
	obj_applyForce(object, x, y)
end

--Called every timestep to update the node
function radialforcefield:update(dt)
	local player = obj_getPlayer()
	if player == nil then return end
	local objx, objy = obj_getPos(player)
	
	--Get vector from object to me
	local x = objx - self.posx
	local y = objy - self.posy
	
	--Save distance between me and object
	local dist = vec2_length(x,y)
	
	--Don't divide by zero; if it's centered over us, just don't apply force this frame
	--Also ignore if they're outside our radius (so we don't push away because math)
	if dist > self.rad then 
		audio_deactivateFilter(self.dsp)
	else
		audio_activateFilter(self.dsp)
	end
end

--Called when node is destroyed
function radialforcefield:destroy()
	audio_destroyFilter(self.dsp)
end

return radialforcefield