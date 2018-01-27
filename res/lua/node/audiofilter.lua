local audiofilter = {}
audiofilter.__index = audiofilter

--[=[
Audio filters and params to pass via xml:

faderate: float, speed to fade filter in/out when entering/leaving node. Default to 20

highpass:
	freq: float between 20 and 22000, required
	
lowpass:
	freq: float between 20 and 22000, required
	
bandpass:
	freq: float between 20 and 22000, required
	bandwidth: float between 0.1 and 10, required

oscillator:
	rate: float between 1 and 22000, required
	wavetype: one of "square", "sawup", "sawdown", "triangle", "noise", "sine". sine default, optional

flange:
	mix: float between 0 and 100. Default: 50. Optional
	depth: float between 0.01 and 1.0. Default: 1.0. Optional
	rate: float between 0 and 20. Default: 0.1. Optional
	
pitchshift:
	pitch: float between 0.5 to 2.0, required
	
chorus:
	mix: float between 0 and 100. Default: 50. Optional
	depth: float between 0 and 100. Default: 20. Optional
	rate: float between 0 and 20. Required
	
reverb:
	decay: float between 100 and 20,000. Default: 1500. Optional
	earlydelay: float between 0 and 300. Default: 20. Optional
	latedelay: float between 0 and 100. Default: 40. Optional
	diffusion: float between 0 and 100. Default: 100. Optional
	mix: float between 0 and 100. Default: 50. Optional
	wetlevel: float between -80 and 20. Default: -6. Optional
	drylevel: float between -80 and 20. Default: 0. Optional

tremolo:
	freq: float between 0.1 and 20. Default: 5. Optional
	depth: float between 0 and 1. Default: 1. Optional
	shape: float between 0 and 1. Default: 0. Optional
	skew: float between -1 and 1. Default: 0. Optional
	duty: float between 0 and 1. Default: 0.5. Optional
	square: float between 0 and 1. Default: 0. Optional
	phase: float between 0 and 1. Default: 0. Optional
	spread: float between -1 and 1. Default: 0. Optional
	
--]=]

--Called when this node is created
function audiofilter:init()
	local filtertype = node_getProperty(self, "filtertype")
	if filtertype == "highpass" then
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_MULTIBAND_EQ)
		local freq = tonumber(node_getProperty(self, "freq"))
		audio_setFilterInt(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_HIGHPASS_12DB)
		audio_setFilterFloat(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, freq)
	elseif filtertype == "lowpass" then
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_MULTIBAND_EQ)
		local freq = tonumber(node_getProperty(self, "freq"))
		audio_setFilterInt(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_LOWPASS_12DB)
		audio_setFilterFloat(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, freq)
	elseif filtertype == "bandpass" then
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_MULTIBAND_EQ)
		local freq = tonumber(node_getProperty(self, "freq"))
		local bandwidth = tonumber(node_getProperty(self, "bandwidth"))
		audio_setFilterInt(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_BANDPASS)
		audio_setFilterFloat(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, freq)	--Bandpass frequency
		audio_setFilterFloat(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_Q, bandwidth)	--Bandpass band width
	elseif filtertype == "oscillator" then
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_OSCILLATOR)
		local rate = tonumber(node_getProperty(self, "rate"))
		local wavetype = node_getProperty(self, "wavetype")
		local wt = FMOD_DSP_OSCILLATOR_SINE
		if wavetype == "square" then
			wt = FMOD_DSP_OSCILLATOR_SQUARE
		elseif wavetype == "sawup" then
			wt = FMOD_DSP_OSCILLATOR_SAWUP
		elseif wavetype == "sawdown" then
			wt = FMOD_DSP_OSCILLATOR_SAWDOWN
		elseif wavetype == "triangle" then
			wt = FMOD_DSP_OSCILLATOR_TRIANGLE
		elseif wavetype == "noise" then
			wt = FMOD_DSP_OSCILLATOR_NOISE
		end
		audio_setFilterInt(self.dsp, FMOD_DSP_OSCILLATOR_TYPE, wt)
		audio_setFilterFloat(self.dsp, FMOD_DSP_OSCILLATOR_RATE, rate)
	elseif filtertype == "flange" then
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_FLANGE)
		local mix = node_getProperty(self, "mix")
		if mix ~= nil then
			mix = tonumber(mix)
		else
			mix = 50
		end
		local depth = node_getProperty(self, "depth")
		if depth ~= nil then
			depth = tonumber(depth)
		else
			depth = 1.0
		end
		local rate = node_getProperty(self, "rate")
		if rate ~= nil then
			rate = tonumber(rate)
		else
			rate = 0.1
		end
		audio_setFilterFloat(self.dsp, FMOD_DSP_FLANGE_MIX, mix)
		audio_setFilterFloat(self.dsp, FMOD_DSP_FLANGE_DEPTH, depth)
		audio_setFilterFloat(self.dsp, FMOD_DSP_FLANGE_RATE, rate)
	elseif filtertype == "pitchshift" then
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_PITCHSHIFT)
		audio_setFilterFloat(self.dsp, FMOD_DSP_PITCHSHIFT_FFTSIZE, 2048)
		local pitch = tonumber(node_getProperty(self, "pitch"))
		audio_setFilterFloat(self.dsp, FMOD_DSP_PITCHSHIFT_PITCH, pitch)
	elseif filtertype == "chorus" then
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_CHORUS)
		local mix = node_getProperty(self, "mix")
		if mix ~= nil then
			mix = tonumber(mix)
		else
			mix = 50
		end
		local depth = node_getProperty(self, "depth")
		if depth ~= nil then
			depth = tonumber(depth)
		else
			depth = 20.0
		end
		local rate = tonumber(node_getProperty(self, "rate"))
		audio_setFilterFloat(self.dsp, FMOD_DSP_CHORUS_RATE, rate)
		audio_setFilterFloat(self.dsp, FMOD_DSP_CHORUS_MIX, mix)
		audio_setFilterFloat(self.dsp, FMOD_DSP_CHORUS_DEPTH, depth)
	elseif filtertype == "reverb" then
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_SFXREVERB)
		local decay = node_getProperty(self, "decay")
		if decay ~= nil then
			decay = tonumber(decay)
		else
			decay = 1500
		end
		local earlydelay = node_getProperty(self, "earlydelay")
		if earlydelay ~= nil then
			earlydelay = tonumber(earlydelay)
		else
			earlydelay = 20
		end
		local latedelay = node_getProperty(self, "latedelay")
		if latedelay ~= nil then
			latedelay = tonumber(latedelay)
		else
			latedelay = 40
		end
		local diffusion = node_getProperty(self, "diffusion")
		if diffusion ~= nil then
			diffusion = tonumber(diffusion)
		else
			diffusion = 50
		end
		local mix = node_getProperty(self, "mix")
		if mix ~= nil then
			mix = tonumber(mix)
		else
			mix = 50
		end
		local wetlevel = node_getProperty(self, "wetlevel")
		if wetlevel ~= nil then
			wetlevel = tonumber(wetlevel)
		else
			wetlevel = -6
		end
		local drylevel = node_getProperty(self, "drylevel")
		if drylevel ~= nil then
			drylevel = tonumber(drylevel)
		else
			drylevel = 0
		end
		audio_setFilterFloat(self.dsp, FMOD_DSP_SFXREVERB_DECAYTIME, decay)
		audio_setFilterFloat(self.dsp, FMOD_DSP_SFXREVERB_EARLYDELAY, earlydelay)
		audio_setFilterFloat(self.dsp, FMOD_DSP_SFXREVERB_LATEDELAY, latedelay)
		audio_setFilterFloat(self.dsp, FMOD_DSP_SFXREVERB_DIFFUSION, diffusion)
		audio_setFilterFloat(self.dsp, FMOD_DSP_SFXREVERB_EARLYLATEMIX, mix)
		audio_setFilterFloat(self.dsp, FMOD_DSP_SFXREVERB_WETLEVEL, wetlevel)
		audio_setFilterFloat(self.dsp, FMOD_DSP_SFXREVERB_DRYLEVEL, drylevel)
	else	--Default to tremolo, which is super obvious if we get it wrong
		self.dsp = audio_createFilter(FMOD_DSP_TYPE_TREMOLO)
		local freq = node_getProperty(self, "freq")
		if freq ~= nil then
			freq = tonumber(freq)
		else
			freq = 5
		end
		local depth = node_getProperty(self, "depth")
		if depth ~= nil then
			depth = tonumber(depth)
		else
			depth = 1
		end
		local shape = node_getProperty(self, "shape")
		if shape ~= nil then
			shape = tonumber(shape)
		else
			shape = 0
		end
		local skew = node_getProperty(self, "skew")
		if skew ~= nil then
			skew = tonumber(skew)
		else
			skew = 0
		end
		local duty = node_getProperty(self, "duty")
		if duty ~= nil then
			duty = tonumber(duty)
		else
			duty = 0.5
		end
		local square = node_getProperty(self, "square")
		if square ~= nil then
			square = tonumber(square)
		else
			square = 0
		end
		local phase = node_getProperty(self, "phase")
		if phase ~= nil then
			phase = tonumber(phase)
		else
			phase = 0
		end
		local spread = node_getProperty(self, "spread")
		if spread ~= nil then
			spread = tonumber(spread)
		else
			spread = 0
		end
		audio_setFilterFloat(self.dsp, FMOD_DSP_TREMOLO_FREQUENCY, freq)
		audio_setFilterFloat(self.dsp, FMOD_DSP_TREMOLO_DEPTH, depth)
		audio_setFilterFloat(self.dsp, FMOD_DSP_TREMOLO_SHAPE, shape)
		audio_setFilterFloat(self.dsp, FMOD_DSP_TREMOLO_SKEW, skew)
		audio_setFilterFloat(self.dsp, FMOD_DSP_TREMOLO_DUTY, duty)
		audio_setFilterFloat(self.dsp, FMOD_DSP_TREMOLO_SQUARE, square)
		audio_setFilterFloat(self.dsp, FMOD_DSP_TREMOLO_PHASE, phase)
		audio_setFilterFloat(self.dsp, FMOD_DSP_TREMOLO_SPREAD, spread)
	end
	self.x, self.y = node_getPos(self)
	self.sizex, self.sizey = node_getVec2Property(self, "size")
	--Use wet/dry mix instead of directly activating/deactivating the filter because of popping noise when doing the latter
	--audio_deactivateFilter(self.dsp)
	audio_setWetDryMix(self.dsp, 0, 0, 1)	--Deactivate to start
	
	--Get rate to turn filter on/off
	self.fadeRate = node_getProperty(self, "faderate")
	if self.fadeRate ~= nil then
		self.fadeRate = tonumber(self.fadeRate)
	else
		self.fadeRate = 20.0	--Default to 1/20th of a second
	end
	audio_addFilterToGroup(self.dsp, GROUP_MASTER, FMOD_CHANNELCONTROL_DSP_TAIL)	--Default to the master channel group, tail of the dsp chain
end

--Called when an object enters this node
function audiofilter:collide(object)
end

--Called every timestep to update the node
function audiofilter:update(dt)
	local player = obj_getPlayer()
	if player == nil then return end
	local objx, objy = obj_getPos(player)
	
	--Test if contains the center of player (Not precise but good enough for audio node)
	if rect_contains(objx, objy, self.x, self.y, self.sizex, self.sizey) then 
		--audio_activateFilter(self.dsp)
		local pre, post, dry = audio_getWetDryMix(self.dsp)
		if post < 1.0 then
			post = post + self.fadeRate * dt
		end
		
		if post > 1.0 then
			post = 1.0
		end
		pre = post
		dry = 1.0 - pre
		
		audio_setWetDryMix(self.dsp, pre, post, dry)
	else
		--audio_deactivateFilter(self.dsp)
		local pre, post, dry = audio_getWetDryMix(self.dsp)
		if post > 0.0 then
			post = post - self.fadeRate * dt
		end
		
		if post < 0.0 then
			post = 0.0
		end
		pre = post
		dry = 1.0 - pre
		
		audio_setWetDryMix(self.dsp, pre, post, dry)
	end
end

--Called when node is destroyed
function audiofilter:destroy()
	audio_destroyFilter(self.dsp)
end

return audiofilter