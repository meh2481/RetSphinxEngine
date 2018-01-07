local templateobj = {}
templateobj.__index = templateobj

function templateobj:init()
end

function templateobj:collide(other)
end

function templateobj:collidewall(wallnormalX, wallnormalY)
end

function templateobj:update(dt)
end

function templateobj:destroy()
end

return templateobj
