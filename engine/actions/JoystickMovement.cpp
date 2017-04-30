#include "JoystickMovement.h"
#include <algorithm>    // std::min

#define JOY_AXIS_MAX	32767.0f

JoystickMovement::JoystickMovement(SDL_GameController* c, int x, int y, int trip)
{
	xAxis = x;
	yAxis = y;
	controller = c;
	joyAxisTrip = trip;
}

Vec2 JoystickMovement::getMovement()
{
	int x = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)xAxis);
	int y = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)yAxis);
	if(abs(x) < joyAxisTrip && abs(y) < joyAxisTrip)
		return Vec2(0.0f, 0.0f);
	float xf = std::max(std::min(x / JOY_AXIS_MAX, 1.0f), -1.0f);
	float yf = -std::max(std::min(y / JOY_AXIS_MAX, 1.0f), -1.0f);	//Flip y so -y = down
	return Vec2(xf, yf);
}
