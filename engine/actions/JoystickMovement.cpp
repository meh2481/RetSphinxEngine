#include "JoystickMovement.h"
#include "InputDevice.h"
#include <algorithm>    // std::min

#define JOY_AXIS_MAX	32767.0f

JoystickMovement::JoystickMovement(int x, int y, int trip)
{
	xAxis = x;
	yAxis = y;
	joyAxisTrip = trip;
}

Vec2 JoystickMovement::getMovement(InputDevice* d)
{
	int x = d->getAxis(xAxis);
	int y = d->getAxis(yAxis);
	if(abs(x) < joyAxisTrip && abs(y) < joyAxisTrip)
		return Vec2(0.0f, 0.0f);
	float xf = std::max(std::min(x / JOY_AXIS_MAX, 1.0f), -1.0f);
	float yf = -std::max(std::min(y / JOY_AXIS_MAX, 1.0f), -1.0f);	//Flip y so -y = down
	return Vec2(xf, yf);
}
