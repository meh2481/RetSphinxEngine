local soundband = {}
soundband.__index = soundband

function soundband:init()
	self.NUM = 16
	self.SIZE_X, self.SIZE_Y = seg_getSize(self, 0)
	self.CHANNEL = music_play('res/mus/song.ogg')
end

function soundband:collide(other)
end

function soundband:collidewall(wallnormalX, wallnormalY)
end

function soundband:update(dt)
	local spectrum = table.pack(music_spectrum(self.CHANNEL, self.NUM))
	
	local count = self.NUM - 1
	
	for i = 0, count do
		--Set size for this frame
		seg_setSize(self, i, self.SIZE_X, spectrum[i + 1] * 2)
	end
end

function soundband:destroy()
end

return soundband
