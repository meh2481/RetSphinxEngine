local heartbeat = {}
heartbeat.__index = heartbeat

--Called when this node is created
function heartbeat:init()
	self.curTime = 0
	self.last = 0
	self.dub = false
	self.offset = 0.85
	self.duboffset = 0.15
	
	self.amplitude = 1.0
	self.decay = 0.05
	--ss_bindEvent("res/json/heartbeat.json")
	self.eventName = "HEARTBEAT"
	self.val = 0
end

--Called when an object enters this node
function heartbeat:collide(object)
end

--Called every timestep to update the node
function heartbeat:update(dt)
	self.curTime = self.curTime + dt
	
	if self.curTime - self.last >= self.offset then
		controller_rumbleLR(95, self.amplitude, 0.0)
		--ss_sendEvent(self.eventName, self.val)
		self.val = self.val + 1
		if self.val > 100 then
			self.val = 0
		end
		self.last = self.curTime
		self.dub = false
	end
	
	if self.curTime - self.last >= self.duboffset and self.dub == false then
		controller_rumbleLR(30, 0.0, self.amplitude)
		self.dub = true
		--self.amplitude = self.amplitude - self.decay
		--if self.amplitude <= 0 then
		--	self.amplitude = 1.0
		--end
	end
	
end

--Called when node is destroyed
function heartbeat:destroy()
end

return heartbeat