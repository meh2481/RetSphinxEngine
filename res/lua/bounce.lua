local bounce = {}
bounce.__index = bounce

function bounce:init()
	self.SEG = 0;
	self.NUM = 64;
	self.SIZE_X, self.SIZE_Y = seg_getSize(self, self.SEG);
	self.CHANNEL = music_play('res/mus/song.ogg')
end

function bounce:collide(other)
end

function bounce:collidewall(wallnormalX, wallnormalY)
end

function bounce:update(dt)
	local spectrum = {music_spectrumL(self.CHANNEL, self.NUM)}
	
	--print(self.CHANNEL, spectrum[1])
	--for k, v in pairs(spectrum) do
	--   print(k, v)
	--end
	
	print(self.SIZE_X, self.SIZE_Y)
	seg_setSize(self, self.SEG, self.SIZE_X * spectrum[1], self.SIZE_Y * spectrum[1])
end

function bounce:destroy()
end

return bounce
