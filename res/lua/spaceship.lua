local spaceship = {}
spaceship.__index = spaceship
spaceship.name = "spaceship"

--Called when object is created
function spaceship:init()
	self.MAX_SHIP_SPEED = 22.5
    self.SHIP_ACCEL = 400.0--140.0
    self.SHIP_SLOW_FAC = 0.985
	self.CAMERA_ZOOM_FAC = 1.0
	self.MAX_THRUST = 2.5
	self.THRUST_ACCEL = 2.1
	
	self.particles = particles_create('res/particles/shiptrail.xml')
	particles_setFireRate(self.particles, 0)
	particles_setFiring(self.particles, true)
	
	self.fireParticles = particles_create('res/particles/lazar.xml')
	particles_setFireRate(self.fireParticles, 0)
	particles_setFiring(self.fireParticles, true)
	
	obj_registerPlayer(self)
	--music_play('res/mus/song.ogg')
end

--Rumble when hitting something
function spaceship:rumble(force)
	local rumbleMsec = force * 10
	if rumbleMsec <= 0 then return end
	local rumbleStrength = force / 25.0
	if rumbleStrength > 1.0 then 
		rumbleStrength = 1.0
	end
	local smallMotor = rumbleStrength
	local largeMotor = rumbleStrength / 2.0
	controller_rumbleLR(math.floor(rumbleMsec), largeMotor, smallMotor)
end

--Called when this object collides with another
function spaceship:collide(other, force)
	self:rumble(force)
end

--Called when this object hits a wall
function spaceship:collidewall(wallnormalX, wallnormalY, force)
	self:rumble(force)
end

--Zoom in and out using the dpad on the joystick
function spaceship:checkDpadZoom()
	local x,y,z = camera_getPos()
	local panX, panY = movement_vec(PAN)
	if panY > 0 then
		z = z + self.CAMERA_ZOOM_FAC
	elseif panY < 0 then
		z = z - self.CAMERA_ZOOM_FAC
	end
	camera_setPos(x, y, z)
end

--Fire particle effect that looks like a weapon thing
function spaceship:shoot(x, y, vx, vy, dt)

	particles_setEmitPos(self.fireParticles, x-vx*dt, y-vy*dt)
	particles_setEmitVel(self.fireParticles, vx, vy)
	--local horizAxis = joy_getAxis(JOY_AXIS2_HORIZ)
	--local vertAxis = joy_getAxis(JOY_AXIS2_VERT)
	local rStickX, rStickY = movement_vec(AIM)
	--Make sure we're outside of our trip radius before moving
	--joytripmove = vec2_length(horizAxis, vertAxis)
	if rStickX ~= 0 or rStickY ~= 0 then
		local angle = 1.0
		particles_setFireRate(self.fireParticles, 1.0)	--firing
		--[=[if vertAxis > 0 then		-- down
			rStickY = vertAxis / JOY_AXIS_MAX
			rStickY = -rStickY
		else						-- up
			rStickY = vertAxis / JOY_AXIS_MIN
		end
		if horizAxis > 0 then		-- right
			rStickX = horizAxis / JOY_AXIS_MAX
		else						-- left
			rStickX = horizAxis / JOY_AXIS_MIN
			rStickX = -rStickX
		end--]=]
		
		particles_setEmitAngle(self.fireParticles, vec2_angle(rStickX,rStickY)*180/3.14159)
	else
		particles_setFireRate(self.fireParticles, 0.0)	--not firing
	end
	
end

--Called every timestep to update the object
function spaceship:update(dt)
	--Center the camera on me
	local x,y = obj_getPos(self)
	camera_centerOnXY(x, y)
	
	--Scroll camera in/out on dpad
	self:checkDpadZoom()
	
	--Now handle input
	--Check for thrusting input
	local max_ship_vel = self.MAX_SHIP_SPEED
	local ship_accel = self.SHIP_ACCEL
	local fac = action_analog(SHIP_THRUST)
	if fac > 0 then
		--local diff = ltAxis - JOY_AXIS_TRIP
		--local totalDiff = JOY_AXIS_MAX - JOY_AXIS_TRIP
		--local fac = diff/totalDiff
		max_ship_vel = max_ship_vel * (1 + (self.MAX_THRUST - 1) * fac)
		ship_accel = ship_accel * (1 + (self.THRUST_ACCEL - 1) * fac)
	--elseif key_isDown(SDL_SCANCODE_SPACE) then --TODO Determine keyconfig for C/Lua interaction instead of just SDL_SCANCODE_SPACE here
	--	max_ship_vel = max_ship_vel * self.MAX_THRUST	--Thrusting increases maximum speed
	--	ship_accel = ship_accel * self.THRUST_ACCEL
	end
	
	--Keep track of if there is current ship-move input with this vector
	local shipMoveVecX, shipMoveVecY = movement_vec(MOVE)
	
	--Check for joypad input
	--[=[local horizAxis = joy_getAxis(JOY_AXIS_HORIZ)
	local vertAxis = joy_getAxis(JOY_AXIS_VERT)
	print(horizAxis, vertAxis)
	--Make sure we're outside of our trip radius before moving
	local joytripmove = vec2_length(horizAxis, vertAxis)
	if joytripmove > JOY_AXIS_TRIP then
		if vertAxis > 0 then		-- down
			shipMoveVecY = vertAxis / JOY_AXIS_MAX
			shipMoveVecY = -shipMoveVecY
		else						-- up
			shipMoveVecY = vertAxis / JOY_AXIS_MIN
		end
		if horizAxis > 0 then		-- right
			shipMoveVecX = horizAxis / JOY_AXIS_MAX
		else						-- left
			shipMoveVecX = horizAxis / JOY_AXIS_MIN
			shipMoveVecX = -shipMoveVecX
		end
	end
	
	--Check for keyboard input
	if key_isDown(KEY_UP1) or key_isDown(KEY_UP2) then
		shipMoveVecY = shipMoveVecY + 1.0
	end
	if key_isDown(KEY_DOWN1) or key_isDown(KEY_DOWN2) then
		shipMoveVecY = shipMoveVecY - 1.0
	end
	if key_isDown(KEY_RIGHT1) or key_isDown(KEY_RIGHT2) then
		shipMoveVecX = shipMoveVecX + 1.0
	end
	if key_isDown(KEY_LEFT1) or key_isDown(KEY_LEFT2) then
		shipMoveVecX = shipMoveVecX - 1.0
	end
	
	--Since the input is in the form of a 1,1 box, make sure the overall length isn't greater than 1
	if vec2_length(shipMoveVecX, shipMoveVecY) > 1.0 then
		shipMoveVecX, shipMoveVecY = vec2_normalize(shipMoveVecX, shipMoveVecY)
	end--]=]
	
	local nonex = 0.0
	local noney = 0.0
	local vx, vy = obj_getVelocity(self)
	if shipMoveVecX ~= 0 or shipMoveVecY ~= 0 then
		max_ship_vel = max_ship_vel * vec2_length(shipMoveVecX, shipMoveVecY)	--multiply velocity for joypad input
		nonex = nonex + dt * ship_accel * shipMoveVecX
		noney = noney + dt * ship_accel * shipMoveVecY
	end
	nonex = nonex + vx
	noney = noney + vy
	
	--Cap our movement speed
	if vec2_length(nonex, noney) > max_ship_vel then
		nonex, noney = vec2_normalize(nonex, noney)
		nonex, noney = vec2_mul(nonex, noney, max_ship_vel)
	end
	
	--If we're not pressing any input, slow the ship down gradually
	if shipMoveVecX == 0 and shipMoveVecY == 0 then
		local f = vec2_length(nonex, noney)
		if f ~= 0 then
			f = f * self.SHIP_SLOW_FAC
			nonex, noney = vec2_normalize(nonex, noney)
			nonex, noney = vec2_mul(nonex, noney, f)
		end
	end
	
	--Finally, set our velocity
	obj_setVelocity(self, nonex, noney)
	
	--Rotate ship
	if vx ~= 0 or vy ~= 0 then	--Only if moving (prevents snap on stop)
		local angle = vec2_angle(vx, vy)
		obj_setAngle(self, angle)
	end
	
	--Ship trail
	--TODO: Fix jerky-looking trail when bursting
	x = x + -vx*1.0/getFramerate()/4.0
	y = y + -vy*1.0/getFramerate()/4.0
	particles_setEmitPos(self.particles, x, y)
	particles_setEmitVel(self.particles, vx, vy)
	local length = vec2_length(vx, vy)
	local particleRate = (length*length*length) / (self.MAX_SHIP_SPEED*self.MAX_SHIP_SPEED*self.MAX_SHIP_SPEED)
	if particleRate > 2.0 then particleRate = 2.0 end
	particles_setFireRate(self.particles, particleRate)
	
	--Check shooting
	self:shoot(x, y, vx, vy, dt)
	
end

--Called when object is destroyed
function spaceship:destroy()
end

return spaceship
