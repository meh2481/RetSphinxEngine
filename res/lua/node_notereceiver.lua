local node_notereceiver = {}
node_notereceiver.__index = node_notereceiver

--Called when this node is created
function node_notereceiver:init()
	
end

--Called when an object enters this node
function node_notereceiver:collide(object)
end

--Called every timestep to update the node
function node_notereceiver:update(dt)
    --print "Hoi I'm updating. Hoi updating I'm tem"
	
end

--Called when node is destroyed
function node_notereceiver:destroy()
end

return node_notereceiver
