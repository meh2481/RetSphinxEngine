--Math functions yaaay

--Find the length of a given vector
local function vec2_length(x, y)
	local length = math.sqrt((x*x)+(y*y))
	return length
end
setglobal("vec2_length", vec2_length)

--Normalize a given vector (make the length = 1)
local function vec2_normalize(x, y)
	local length = vec2_length(x,y)
	if length == 0 then
		return x, y
	end
	return x/length, y/length
end
setglobal("vec2_normalize", vec2_normalize)

--Multiply a given vector by a scaling factor
local function vec2_mul(x,y,fac)
	if fac then
		return x * fac, y * fac
	end
	return x, y
end
setglobal("vec2_mul", vec2_mul)

--Get the angle a vector is pointing in
local function vec2_angle(x,y)
	return math.atan(y,x)
end
setglobal("vec2_angle", vec2_angle)

--See if a point is in a rect
local function rect_contains(x,y,rx,ry,rw,rh)
	local left = rx - rw / 2.0
	local right = rx + rw / 2.0
	local top = ry + rh / 2.0
	local bottom = ry - rh / 2.0
	if x >= left and x <= right and y <= top and y >= bottom then
		return true
	end
	return false
end
setglobal("rect_contains", rect_contains)