local node_template = {}
node_template.__index = node_template

--Called when this node is created
function node_template:init()
end

--Called when an object enters this node
function node_template:collide(object)
end

--Called every timestep to update the node
function node_template:update(dt)
end

--Called when node is destroyed
function node_template:destroy()
end

return node_template