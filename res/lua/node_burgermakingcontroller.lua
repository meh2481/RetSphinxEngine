local node_burgermakingcontroller = {}
node_burgermakingcontroller.__index = node_burgermakingcontroller

--Called when this node is created
function node_burgermakingcontroller:init()
	self.LAUNCH_TOP = node_get("noteLauncherTop")
	self.LAUNCH_BOTTOM = node_get("noteLauncherBottom")
	self.LAUNCH_LEFT = node_get("noteLauncherLeft")
	self.LAUNCH_RIGHT = node_get("noteLauncherRight")
	self.RECEIVE_TOP = node_get("noteReceiverTop")
	self.RECEIVE_BOTTOM = node_get("noteReceiverBottom")
	self.RECEIVE_LEFT = node_get("noteReceiverLeft")
	self.RECEIVE_RIGHT = node_get("noteReceiverRight")
	self.TIME = 0
	self.NOTE_DELAY = 0.5	--Seconds between notes
	self.NEXT_NOTE = self.NOTE_DELAY
	self.NOTE_TIME = 1.5	--Seconds note is travelling for
end

--Called when an object enters this node
function node_burgermakingcontroller:collide(object)
end

--Called every timestep to update the node
function node_burgermakingcontroller:update(dt)
	self.TIME = self.TIME + dt
	
	--TODO Actually in time with the music or whatevs
	if self.TIME > self.NEXT_NOTE then
		self.NEXT_NOTE = self.NEXT_NOTE + self.NOTE_DELAY

		--Fire from a random location
		local firer = math.random(4)
		local launchfrom
		local receiveto
		local image
		if firer == 1 then
			launchfrom = self.LAUNCH_TOP
			receiveto = self.RECEIVE_TOP
			image = "res/gfx/marbleyellow.png"
		elseif firer == 2 then
			launchfrom = self.LAUNCH_BOTTOM
			receiveto = self.RECEIVE_BOTTOM
			image = "res/gfx/marblegreen.png"
		elseif firer == 3 then
			launchfrom = self.LAUNCH_LEFT
			receiveto = self.RECEIVE_LEFT
			image = "res/gfx/marbleblue.png"
		else
			launchfrom = self.LAUNCH_RIGHT
			receiveto = self.RECEIVE_RIGHT
			image = "res/gfx/marblered.png"
		end
		
		local nodex, nodey = node_getPos(launchfrom)
		local destx, desty = node_getPos(receiveto)
		local sizex, sizey = node_getVec2Property(launchfrom, "size")
		--Randomize x pos along node
		local xpos = nodex + (sizex * math.random()) - (sizex / 2.0)
		local ypos = nodey + (sizey * math.random()) - (sizey / 2.0)
		
		--Set vel to get in there in the given period of time
		local xvel = (destx - xpos) / self.NOTE_TIME
		local yvel = (desty - ypos) / self.NOTE_TIME
		local o = obj_create("note", xpos, ypos)
		
		obj_setVelocity(o, xvel, yvel)
		obj_setImage(o, image)
		--TODO: Object collision mask, so they don't hit each other
	end
end

--Called when node is destroyed
function node_burgermakingcontroller:destroy()
end

return node_burgermakingcontroller
