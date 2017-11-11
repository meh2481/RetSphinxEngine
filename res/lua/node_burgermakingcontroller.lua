local node_burgermakingcontroller = {}
node_burgermakingcontroller.__index = node_burgermakingcontroller

--Called when this node is created
function node_burgermakingcontroller:init()
	
end

--Called when an object enters this node
function node_burgermakingcontroller:collide(object)
end

--Called every timestep to update the node
function node_burgermakingcontroller:update(dt)
    --print "Hoi I'm updating. Hoi updating I'm tem"
	
end

--Called when node is destroyed
function node_burgermakingcontroller:destroy()
end

return node_burgermakingcontroller
