local testblob = {}
testblob.__index = testblob

function testblob:init()
end

function testblob:collide(other)
end

function testblob:collidewall(wallnormalX, wallnormalY)
end

function testblob:update(dt)
	--TEST
	if mouse_isDown() then
		obj_setPos(self, 0, 3)
	end
end

function testblob:destroy()
end

return testblob
