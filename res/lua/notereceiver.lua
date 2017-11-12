local notereceiver = {}
notereceiver.__index = notereceiver

function notereceiver:init()
	self.SEG = 0
	self.SIZE_X, self.SIZE_Y = seg_getSize(self, self.SEG)
	local posprop = obj_getProperty(self, "pos")
	local i = 1
	local posx = 0
	local posy = 0
	--TODO: We need to get the obj position from XML cause this is silly
	for str in string.gmatch(posprop, "([^,]+)") do
		str = str:gsub("^%s*(.-)%s*$", "%1")
		if i == 1 then
			posx = tonumber(str)
		elseif i == 2 then
			posy = tonumber(str)
		end
		i = i + 1
	end
	
	--Set segment position
	seg_setPos(self, self.SEG, posx, posy)
	
	--Set image
	local imgProp = obj_getProperty(self, "img")
	obj_setImage(self, imgProp, self.SEG)
	
	--Get direction
	local dir = obj_getProperty(self, "direction");
	if dir == "up" then
		self.DIR = NOTE_UP
	elseif dir == "down" then
		self.DIR = NOTE_DOWN
	elseif dir == "left" then 
		self.DIR = NOTE_LEFT
	else
		self.DIR = NOTE_RIGHT
	end
end

function notereceiver:collide(other)
end

function notereceiver:collidewall(wallnormalX, wallnormalY)
end

function notereceiver:update(dt)

	local cur_x = self.SIZE_X
	local cur_y = self.SIZE_Y
	
	if action_digital(self.DIR) then
		cur_x = cur_x / 2.0
		cur_y = cur_y / 2.0
	end
	
	--Set size for this frame
	seg_setSize(self, self.SEG, cur_x, cur_y)
end

function notereceiver:destroy()
end

return notereceiver
