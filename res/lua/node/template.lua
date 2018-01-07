local template = {}
template.__index = template

--Called when this node is created
function template:init()
end

--Called when an object enters this node
function template:collide(object)
end

--Called every timestep to update the node
function template:update(dt)
end

--Called when node is destroyed
function template:destroy()
end

return template