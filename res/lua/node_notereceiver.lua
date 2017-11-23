local node_notereceiver = {}
node_notereceiver.__index = node_notereceiver

--Called when this node is created
function node_notereceiver:init()
	self.X, self.Y = node_getPos(self)
	self.DIRECTION = node_getProperty(self, "direction")
	self.RADIUS = tonumber(node_getProperty(self, "radius"))
	self.PRESS = false
	self.KEYDOWN = false
	
	if self.DIRECTION == "up" then
		self.ACTION_DIR = NOTE_UP
	elseif self.DIRECTION == "down" then
		self.ACTION_DIR = NOTE_DOWN
	elseif self.DIRECTION == "left" then
		self.ACTION_DIR = NOTE_LEFT
	elseif self.DIRECTION == "right" then
		self.ACTION_DIR = NOTE_RIGHT
	end
end

--Test to see if a point is inside this node
function node_notereceiver:isInside(x, y)
	local distance = vec2_length(x - self.X, y - self.Y)
	if distance <= self.RADIUS then
		return true
	end
	return false
end

--Called when an object enters this node
function node_notereceiver:collide(object)
	local objx, objy = obj_getPos(object)
	local inside = self:isInside(objx, objy)
		
	if inside and self.PRESS then
		obj_destroy(object)
		--print "hit"
	elseif not inside then	
		if self.DIRECTION == "up" then
			if objy < self.Y then
				obj_destroy(object)
				--print "miss"
			end
		elseif self.DIRECTION == "down" then
			if objy > self.Y then
				obj_destroy(object)
				--print "miss"
			end
		elseif self.DIRECTION == "left" then
			if objx > self.X then
				obj_destroy(object)
				--print "miss"
			end
		elseif self.DIRECTION == "right" then
			if objx < self.X then
				obj_destroy(object)
				--print "miss"
			end
		end
	end
end

--Called every timestep to update the node
function node_notereceiver:update(dt)

	--Update key-down state
    if action_digital(self.ACTION_DIR) then
		if self.KEYDOWN == false then
			self.PRESS = true
			sound_play('res/sfx/laser.wav')
			--print (self.DIRECTION.." pos: "..music_getPos())
		else
			self.PRESS = false
		end
		self.KEYDOWN = true
	else
		self.KEYDOWN = false
		self.PRESS = false
	end
	
end

--Called when node is destroyed
function node_notereceiver:destroy()
end

return node_notereceiver
