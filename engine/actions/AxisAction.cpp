#include "AxisAction.h"
#include "InputDevice.h"
#include <algorithm>    // std::min

//Defined by SDL
#define JOY_AXIS_MAX	32767.0f	//Technically 32768 on the negative axis but who's counting

AxisAction::AxisAction(SDL_GameController* c, int axisToUse, int trip)
{
	joyAxisTrip = trip;
	axis = axisToUse;
	controller = c;
}

bool AxisAction::getDigitalAction()
{
	return (SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)axis) > joyAxisTrip);
}

float AxisAction::getAnalogAction()
{
	int val = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)axis);
	if(val < joyAxisTrip)
		return 0.0f;
	return std::min(val / JOY_AXIS_MAX, 1.0f);
}
