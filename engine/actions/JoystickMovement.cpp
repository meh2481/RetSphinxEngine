#include "JoystickMovement.h"
#include "InputDevice.h"
#include <algorithm>    // std::min

#define JOY_AXIS_MAX	32767.0f

JoystickMovement::JoystickMovement(int x, int y, float trip)
{
	xAxis = x;
	yAxis = y;
	joyAxisTrip = trip;
}

Vec2 JoystickMovement::getMovement(InputDevice* d)
{
	int x = d->getAxis(xAxis);
	int y = -d->getAxis(yAxis);	//Flip y so -y = down
	if(abs(x) < joyAxisTrip && abs(y) < joyAxisTrip)
		return Vec2(0.0f, 0.0f);

	//Slightly complex math here so that right outside of dead zone will be 0.0f, rather than stick center being 0.0f
	const float trip = joyAxisTrip / JOY_AXIS_MAX;	//Dead zone size, in 0.0-1.0f range
	float xf = std::max(std::min(x / JOY_AXIS_MAX, 1.0f), -1.0f);	//Current stick pos in 0.0-1.0f range
	float yf = std::max(std::min(y / JOY_AXIS_MAX, 1.0f), -1.0f);

	float angle = atan2(yf, xf);	//Angle of stick movement
	Vec2 tripPt(cos(angle)*trip, sin(angle)*trip);	//Point on trip circle

	Vec2 ret(xf, yf);	//Actual joystick pos
	ret -= tripPt;		//Distance from trip point
	ret /= 1.0f - trip;	//Scale according to maximum

	return ret;
}
