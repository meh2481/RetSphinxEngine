local bounce = {}
bounce.__index = bounce

function bounce:init()
	self.SEG = 0
	self.NUM = 4
	self.ADD_FAC = 0.2
	self.FALL_FAC = 0.15
	self.SIZE_X, self.SIZE_Y = seg_getSize(self, self.SEG)
	self.CUR_X = self.SIZE_X
	self.CUR_Y = self.SIZE_Y
	self.MAX_X = self.SIZE_X * 2.0
	self.MAX_Y = self.SIZE_Y * 2.0
end

function bounce:collide(other)
end

function bounce:collidewall(wallnormalX, wallnormalY)
end

function bounce:update(dt)
	local channel = music_getChannel()
	local spectrum = table.pack(music_spectrum(channel, self.NUM))
	
	--Error check
	if spectrum[1] == nil then
		return
	end
	
	--print(self.CHANNEL, spectrum[1])
	--for k, v in pairs(spectrum) do
	--   print(k, v)
	--end
	
	--print(self.SIZE_X, self.SIZE_Y)
	
	--Add fac
	self.CUR_X = self.CUR_X + self.ADD_FAC * spectrum[1]
	self.CUR_Y = self.CUR_Y + self.ADD_FAC * spectrum[1]
	
	--Bounds check
	if self.CUR_X > self.MAX_X then
		self.CUR_X = self.MAX_X
	end
	if self.CUR_Y > self.MAX_Y then
		self.CUR_Y = self.MAX_Y
	end
	
	--Set size for this frame
	seg_setSize(self, self.SEG, self.CUR_X, self.CUR_Y)
	
	--Fall back for next frame
	self.CUR_X = self.CUR_X - self.FALL_FAC
	self.CUR_Y = self.CUR_Y - self.FALL_FAC
	
	--Bounds check
	if self.CUR_X < self.SIZE_X then
		self.CUR_X = self.SIZE_X
	end
	if self.CUR_Y < self.SIZE_Y then
		self.CUR_Y = self.SIZE_Y
	end
end

function bounce:destroy()
end

return bounce
