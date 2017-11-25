local node_sunlight = {}
node_sunlight.__index = node_sunlight

--Called when this node is created
function node_sunlight:init()
	self.x, self.y = node_getPos(self)
	self.z = 3
end

--Called when an object enters this node
function node_sunlight:collide(object)
end

--Called every timestep to update the node
function node_sunlight:update(dt)
	--Set up OpenGL things here for lack of a better place
	opengl_light(GL_LIGHT1, GL_AMBIENT, .02, .02, .02, 1)
	opengl_light(GL_LIGHT1, GL_DIFFUSE, 1, 1, 1, 1)
	opengl_light(GL_LIGHT1, GL_SPECULAR, 0.0, 0.0, 0.0, 1)	--Causes jagged edges if both light specular and mat specular > 0
	opengl_mat(GL_FRONT_AND_BACK, GL_AMBIENT, 0.2, 0.2, 0.2, 1.0)
	opengl_mat(GL_FRONT_AND_BACK, GL_DIFFUSE, 1, 1, 1, 1)
	opengl_mat(GL_FRONT_AND_BACK, GL_SPECULAR, 1, 1, 1, 1.0)
	opengl_mat(GL_FRONT_AND_BACK, GL_EMISSION, 0, 0, 0, 1.0)
	opengl_light(GL_LIGHT1, GL_POSITION, self.x, self.y, self.z, 1)
end

--Called when node is destroyed
function node_sunlight:destroy()
end

return node_sunlight