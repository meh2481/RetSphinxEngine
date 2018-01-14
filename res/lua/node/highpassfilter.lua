local highpassfilter = {}
highpassfilter.__index = highpassfilter

--Called when this node is created
function highpassfilter:init()
	self.freq = tonumber(node_getProperty(self, "freq"))
	self.dsp = audio_createHighpassFilter(self.freq)
	self.x, self.y = node_getPos(self)
	self.sizex, self.sizey = node_getVec2Property(self, "size")
	audio_deactivateFilter(self.dsp)
end

--Called when an object enters this node
function highpassfilter:collide(object)
end

--Called every timestep to update the node
function highpassfilter:update(dt)
	local player = obj_getPlayer()
	if player == nil then return end
	local objx, objy = obj_getPos(player)
	
	--Don't divide by zero; if it's centered over us, just don't apply force this frame
	--Also ignore if they're outside our radius (so we don't push away because math)
	if rect_contains(objx, objy, self.x, self.y, self.sizex, self.sizey) then 
		audio_activateFilter(self.dsp)
	else
		audio_deactivateFilter(self.dsp)
	end
end

--Called when node is destroyed
function highpassfilter:destroy()
	audio_destroyFilter(self.dsp)
end

return highpassfilter