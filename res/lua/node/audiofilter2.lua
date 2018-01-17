local audiofilter = {}
audiofilter.__index = audiofilter

--Called when this node is created
function audiofilter:init()
	self.dsp = audio_createFilter(FMOD_DSP_TYPE_FLANGE)
	self.x, self.y = node_getPos(self)
	self.sizex, self.sizey = node_getVec2Property(self, "size")
	audio_deactivateFilter(self.dsp)	--Deactivate to start
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
	--TODO: Sharp click when enable/disable abruptly like this. Prolly want to interpolate wet/dry or such
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

--[=[

--Bandpass example
self.dsp = audio_createFilter(FMOD_DSP_TYPE_MULTIBAND_EQ)
audio_setFilterInt(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_BANDPASS)
audio_setFilterFloat(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, 5000.0)	--Bandpass frequency
audio_setFilterFloat(self.dsp, FMOD_DSP_MULTIBAND_EQ_A_Q, 50.0)	--Bandpass band width

--Oscillator example (Booooooop noise, can use to create own synth)
self.dsp = audio_createFilter(FMOD_DSP_TYPE_OSCILLATOR)
audio_setFilterInt(self.dsp, FMOD_DSP_OSCILLATOR_TYPE, FMOD_DSP_OSCILLATOR_SINE)
audio_setFilterFloat(self.dsp, FMOD_DSP_OSCILLATOR_RATE, 500.0)

--Pan example (pan around where the ship is)
self.dsp = audio_createFilter(FMOD_DSP_TYPE_PAN)
audio_setFilterInt(self.dsp, FMOD_DSP_PAN_MODE, FMOD_DSP_PAN_MODE_STEREO)
audio_setFilterFloat(self.dsp, FMOD_DSP_PAN_2D_STEREO_POSITION, (self.x - objx) * 10.0)	--In update method

--Flange example (Super cool wub effect with this dubstep song)
self.dsp = audio_createFilter(FMOD_DSP_TYPE_FLANGE)

--Pitchshift example (Shift pitch up 1 octave)
self.dsp = audio_createFilter(FMOD_DSP_TYPE_PITCHSHIFT)
audio_setFilterFloat(self.dsp, FMOD_DSP_PITCHSHIFT_FFTSIZE, 2048)
audio_setFilterFloat(self.dsp, FMOD_DSP_PITCHSHIFT_PITCH, 2.0)

--Chorus example (some sort of reverby thing around the sound?)
self.dsp = audio_createFilter(FMOD_DSP_TYPE_CHORUS)
audio_setFilterFloat(self.dsp, FMOD_DSP_CHORUS_RATE, 1.6)
audio_setFilterFloat(self.dsp, FMOD_DSP_CHORUS_DEPTH, 20.0)

--Reverb (like a fast-decay echo, particularly around each bass drum hit)
self.dsp = audio_createFilter(FMOD_DSP_TYPE_SFXREVERB)

--Tremolo (Volume going up and down)
self.dsp = audio_createFilter(FMOD_DSP_TYPE_TREMOLO)



--]=]
