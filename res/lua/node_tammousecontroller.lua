local node_tammousecontroller = {}
node_tammousecontroller.__index = node_tammousecontroller

local REDNONE = 1
local REDGREEN = 2
local NONEYELLOW = 3
local GREENYELLOW = 4
local GREENPURPLE = 5
local YELLOWBLUE = 6
local PURPLEBLUE = 7

local HOLE_RED = "holered"
local HOLE_GREEN = "holegreen"
local HOLE_BLUE = "holeblue"
local HOLE_YELLOW = "holeyellow"
local HOLE_PURPLE = "holepurple"
local HOLE_BLANK = "holeblank"

local MARBLE_RED = "marblered"
local MARBLE_GREEN = "marblegreen"
local MARBLE_BLUE = "marbleblue"
local MARBLE_YELLOW = "marbleyellow"
local MARBLE_PURPLE = "marblepurple"

local TAMWALL = "tamwall"

local RADIUS = 0.9
local XPOS = 3.0
local YPOS = 4.5
local YOFF = 3.0
local XOFF = 3.0
local Y1 = -YPOS
local Y2 = Y1 + YOFF
local Y3 = Y2 + YOFF
local Y4 = Y3 + YOFF
local X1 = -XPOS
local X2 = X1 + XOFF
local X3 = X2 + XOFF
local DISTFAC = 2.0

--Called when this node is created
function node_tammousecontroller:init()
	--Create walls
	self.walls = {}
	local wall = obj_create(TAMWALL, 0, 3)
	obj_setActive(wall, false)
	table.insert(self.walls, wall)
	wall = obj_create(TAMWALL, -1.5, 1.5)
	obj_setActive(wall, false)
	obj_setAngle(wall, math.pi / 2.0)
	table.insert(self.walls, wall)
	wall = obj_create(TAMWALL, 1.5, 1.5)
	obj_setAngle(wall, math.pi / 2.0)
	table.insert(self.walls, wall)
	wall = obj_create(TAMWALL)
	table.insert(self.walls, wall)
	wall = obj_create(TAMWALL, -1.5, -1.5)
	obj_setAngle(wall, math.pi / 2.0)
	table.insert(self.walls, wall)
	wall = obj_create(TAMWALL, 1.5, -1.5)
	obj_setAngle(wall, math.pi / 2.0)
	table.insert(self.walls, wall)
	wall = obj_create(TAMWALL, 0, -3)
	table.insert(self.walls, wall)
	
	self.DRAGGING = nil
    self.GRABX = 0
    self.GRABY = 0
	self.RELEASEX = -1
	self.RELEASEY = -1
	
end

--Called when an object enters this node
function node_tammousecontroller:collide(object)
end

--Return false if any objects are in the node of this name, true if none are
local function testObj(name)
	local hole = node_get(name)
	local objsColliding = node_getCollidingObj(hole)
	for k, v in pairs(objsColliding) do
		if v ~= nil then
			local x, y = obj_getPos(v)
			if node_isInside(hole, x, y) then
				return false
			end
		end
	end
	return true
end

local function testIsInside(node, obj)
	local hole = node_get(node)
	local marble = node_getNearestObj(hole)
	if marble ~= nil then
		if obj_getProperty(marble, "id") == obj then
			local x, y = obj_getPos(marble)
            local nx, ny = node_getPos(hole)
            local dx = nx - x
            local dy = ny - y
            local targetDiff = tonumber(node_getProperty(hole, "radius")) / DISTFAC
			if vec2_length(dx, dy) < targetDiff then
				return true
			end
		end
	end
	return false
end

local function testIfWon()
	if testIsInside(HOLE_RED, MARBLE_RED) and 
		testIsInside(HOLE_GREEN, MARBLE_GREEN) and 
		testIsInside(HOLE_BLUE, MARBLE_BLUE) and 
		testIsInside(HOLE_YELLOW, MARBLE_YELLOW) and 
		testIsInside(HOLE_PURPLE, MARBLE_PURPLE) then
		return true
	end
	return false
end

function node_tammousecontroller:updateWalls()
	--Set all walls to active
	for i = 1,7 do
		obj_setActive(self.walls[i], true)
	end
	
	--Set walls to inactive as needed
	if testObj(HOLE_BLANK) then
		obj_setActive(self.walls[REDNONE], false)
		obj_setActive(self.walls[NONEYELLOW], false)
	end
	
	if testObj(HOLE_RED) then
		obj_setActive(self.walls[REDNONE], false)
		obj_setActive(self.walls[REDGREEN], false)
	end
	
	if testObj(HOLE_GREEN) then
		obj_setActive(self.walls[GREENYELLOW], false)
		obj_setActive(self.walls[REDGREEN], false)
		obj_setActive(self.walls[GREENPURPLE], false)
	end
	
	if testObj(HOLE_YELLOW) then
		obj_setActive(self.walls[GREENYELLOW], false)
		obj_setActive(self.walls[NONEYELLOW], false)
		obj_setActive(self.walls[YELLOWBLUE], false)
	end
	
	if testObj(HOLE_PURPLE) then
		obj_setActive(self.walls[GREENPURPLE], false)
		obj_setActive(self.walls[PURPLEBLUE], false)
	end
	
	if testObj(HOLE_BLUE) then
		obj_setActive(self.walls[PURPLEBLUE], false)
		obj_setActive(self.walls[YELLOWBLUE], false)
	end
end

--Called every timestep to update the node
function node_tammousecontroller:update(dt)
    
    self:updateWalls()
	
	if testIfWon() then
		map_load("res/map/solarsystem.scene.xml")
	end

	--Drag marbles around with the mouse
	self:mouseCheck()
	
end

function node_tammousecontroller:mouseCheck()
    local x, y = mouse_getPos()
	x, y = mouse_transformToWorld(x, y)
	local marble = obj_getFromPoint(x, y)
	
	local xCur, yCur = self:getLocation(x, y)
	
	if marble ~= nil and self.DRAGGING == nil and (xCur ~= self.RELEASEX or yCur ~= self.RELEASEY) then
		self.DRAGGING = marble
        self.GRABX = x  --Store where we grabbed, for collision testing later
        self.GRABY = y
	end
	
	if self.DRAGGING ~= nil then
        x, y = self:constrain(self.DRAGGING, x, y)
		obj_setPos(self.DRAGGING, x, y)
		obj_setVelocity(self.DRAGGING, 0, 0)
	end
	
	if marble == nil then
		self.DRAGGING = nil
	end
	
	if mouse_isDown() then
		self.DRAGGING = nil
		
		self.RELEASEX, self.RELEASEY = self:getLocation(x, y)
	end
	
	--Drop current marble if we're over a different place than we picked it up
	if self.DRAGGING ~= nil then
		self:dropCheck(x, y)
	end
	
	if xCur ~= self.RELEASEX or yCur ~= self.RELEASEY then
		self.RELEASEX = -1
		self.RELEASEY = -1
	end
end

function node_tammousecontroller:getLocation(x, y)
	local posX, posY
	
	if x < X1 then
		poxX = -1
	elseif x < X2 then
		posX = 0
	elseif x < X3 then
		posX = 1
	else
		posX = -1
	end
	
	if y > Y4 then
		posY = -1
	elseif y > Y3 then
		posY = 0
	elseif y > Y2 then
		posY = 1
	elseif y > Y1 then
		posY = 2
	else
		posY = -1
	end
	
	return posX, posY
end

function node_tammousecontroller:dropCheck(x, y)
	local xtile, ytile = self:getLocation(x, y)
	local prevX, prevY = self:getLocation(self.GRABX, self.GRABY)
	
	if xtile ~= prevX or ytile ~= prevY then
		if xtile ~= prevX and ytile ~= prevY then
			--Skipping a corner; set to old pos
			obj_setPos(self.DRAGGING, self.GRABX, self.GRABY)
			self.DRAGGING = nil
			self.RELEASEX = self.GRABX
			self.RELEASEY = self.GRABY
		else
			obj_setVelocity(self.DRAGGING, 8*(xtile - prevX), 8*(prevY - ytile))
			self.DRAGGING = nil
			self.RELEASEX = xtile
			self.RELEASEY = ytile
		end
	end
end

function node_tammousecontroller:constrainVert(marble, x, y, wall, yMin, yMax, xPos)
	if obj_isActive(wall) then
		if y <= yMin and y >= yMax then
			if self.GRABX < xPos then  --to the left to the left
				if x + RADIUS > xPos then
					x = xPos - RADIUS 
				end
			else                     --to the right to the right
				if x - RADIUS < xPos then
					x = RADIUS + xPos
				end
			end 
		end
	end
	return x, y
end

function node_tammousecontroller:constrainHoriz(marble, x, y, wall, xMin, xMax, yPos)
	if obj_isActive(wall) then
		if x >= xMin and x <= xMax then
			if self.GRABY > yPos then  --above
				if y - RADIUS < yPos then
					y = yPos + RADIUS 
				end
			else                     --below
				if y + RADIUS > yPos then
					y = yPos - RADIUS
				end
			end 
		end
	end
	return x, y
end
    
function node_tammousecontroller:constrain(marble, x, y)
	
	--x, y = obj_getPos(marble)

	--Constrain to borders of game board
	if x - RADIUS < -XPOS then
		x = -XPOS + RADIUS
	end
	if x + RADIUS > XPOS then
		x = XPOS - RADIUS
	end
	if y - RADIUS < -YPOS then
		y = -YPOS + RADIUS
	end
	if y + RADIUS > YPOS then
		y = YPOS - RADIUS
	end

	--Constrain purple-blue
	x, y = self:constrainVert(marble, x, y, self.walls[PURPLEBLUE], Y2, Y1, X2)
	--Constrain green-yellow
	x, y = self:constrainVert(marble, x, y, self.walls[GREENYELLOW], Y3, Y2, X2)
	--Constrain red-blank
	x, y = self:constrainVert(marble, x, y, self.walls[REDNONE], Y4, Y3, X2)
	--Constrain red-green
	x, y = self:constrainHoriz(marble, x, y, self.walls[REDGREEN], X1, X2, Y3)
	--Constrain blank-yellow
	x, y = self:constrainHoriz(marble, x, y, self.walls[NONEYELLOW], X2, X3, Y3)
	--Constrain green-purple
	x, y = self:constrainHoriz(marble, x, y, self.walls[GREENPURPLE], X1, X2, Y2)
	--Constrain yellow-blue
	x, y = self:constrainHoriz(marble, x, y, self.walls[YELLOWBLUE], X2, X3, Y2)

	return x, y
end

--Called when node is destroyed
function node_tammousecontroller:destroy()
end

return node_tammousecontroller
