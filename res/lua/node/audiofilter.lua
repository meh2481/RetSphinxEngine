local audiofilter = {}
audiofilter.__index = audiofilter

--Called when this node is created
function audiofilter:init()
	self.freq = tonumber(node_getProperty(self, "freq"))
	local filtertype = node_getProperty(self, "filtertype")
	if filtertype == "highpass" then
		self.dsp = audio_createFilter(self.freq, HIGHPASS)
	else
		self.dsp = audio_createFilter(self.freq, LOWPASS)
	end
	self.x, self.y = node_getPos(self)
	self.sizex, self.sizey = node_getVec2Property(self, "size")
	audio_deactivateFilter(self.dsp)
end

--Called when an object enters this node
function audiofilter:collide(object)
end

--Called every timestep to update the node
function audiofilter:update(dt)
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
function audiofilter:destroy()
	audio_destroyFilter(self.dsp)
end

return audiofilter