local node_burgermakingcontroller = {}
node_burgermakingcontroller.__index = node_burgermakingcontroller

--Called when this node is created
function node_burgermakingcontroller:init()
	self.LAUNCH_TOP = node_get("noteLauncherTop")
	self.LAUNCH_BOTTOM = node_get("noteLauncherBottom")
	self.LAUNCH_LEFT = node_get("noteLauncherLeft")
	self.LAUNCH_RIGHT = node_get("noteLauncherRight")
	self.TIME = 0
	self.NOTE_DELAY = 0.5
	self.NEXT_NOTE = self.NOTE_DELAY
end

--Called when an object enters this node
function node_burgermakingcontroller:collide(object)
end

--Called every timestep to update the node
function node_burgermakingcontroller:update(dt)
	self.TIME = self.TIME + dt
	
	if self.TIME > self.NEXT_NOTE then
		self.NEXT_NOTE = self.NEXT_NOTE + self.NOTE_DELAY
		
		local nodex, nodey = node_getPos(self.LAUNCH_TOP)
		--TODO
		local xpos = nodex
		local ypos = nodey
		
		--obj_create(string className, float xpos, float ypos, float xvel, float yvel)
		local xvel = 0
		local yvel = -20.0
		local o = obj_create("note", xpos, ypos)
		
		obj_setVelocity(o, xvel, yvel)
	end
end

--Called when node is destroyed
function node_burgermakingcontroller:destroy()
end

return node_burgermakingcontroller
