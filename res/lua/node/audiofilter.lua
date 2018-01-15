local audiofilter = {}
audiofilter.__index = audiofilter

--Called when this node is created
function audiofilter:init()
	self.freq = tonumber(node_getProperty(self, "freq"))
	local filtertype = node_getProperty(self, "filtertype")
	self.dsp = audio_createFilter(FMOD_DSP_TYPE_MULTIBAND_EQ)
	if filtertype == "highpass" then
		audio_setFilterInt(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_HIGHPASS_12DB)
	else
		audio_setFilterInt(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_LOWPASS_12DB)
	end
	audio_setFilterFloat(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, self.freq)
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