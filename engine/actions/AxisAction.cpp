#include "AxisAction.h"
#include "InputDevice.h"
#include <algorithm>    // std::min

//Defined by SDL
#define JOY_AXIS_MAX	32767.0f	//Technically 32768 on the negative axis but who's counting

AxisAction::AxisAction(int axisToUse, int trip)
{
	joyAxisTrip = trip;
	axis = axisToUse;
}

bool AxisAction::getDigitalAction(InputDevice* d)
{
	return (abs(d->getAxis(axis)) > joyAxisTrip);
}

float AxisAction::getAnalogAction(InputDevice* d)
{
	int val = d->getAxis(axis);
	if(abs(val) < joyAxisTrip)
		return 0.0f;
	return std::max(std::min(val / JOY_AXIS_MAX, 1.0f), -1.0f);
}
