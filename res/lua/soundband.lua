local soundband = {}
soundband.__index = soundband

function soundband:init()
	self.NUM = 16
	self.HEIGHT = 1.5
	self.SIZE_X, self.SIZE_Y = seg_getSize(self, 0)
	self.POS_X, self.POS_Y = seg_getPos(self, 0)
	self.SIDE = obj_getProperty(self, 'side')
end

function soundband:collide(other)
end

function soundband:collidewall(wallnormalX, wallnormalY)
end

function soundband:update(dt)
	local channel = music_getChannel()
	local spectrum
	if self.SIDE == 'r' then
		spectrum = table.pack(music_spectrumR(channel, self.NUM))
	elseif self.SIDE == 'l' then
		spectrum = table.pack(music_spectrumL(channel, self.NUM))
	else
		spectrum = table.pack(music_spectrum(channel, self.NUM))
	end
		
	--Error check
	if spectrum[0] == nil then
		return
	end
	
	local count = self.NUM - 1
	
	for i = 0, count do
		local fac = spectrum[i + 1] * self.HEIGHT
		--Set size for this frame
		seg_setSize(self, i, self.SIZE_X, fac)
		local x, y = seg_getPos(self, i)
		seg_setPos(self, i, x, self.POS_Y + fac / 2)	--Center so that bottom stays still
	end
end

function soundband:destroy()
end

return soundband
