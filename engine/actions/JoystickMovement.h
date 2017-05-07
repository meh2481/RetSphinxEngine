#pragma once
#include "MovementBind.h"
#include "SDL_gamecontroller.h"

class JoystickMovement : public MovementBind
{
private:
	int xAxis, yAxis;
	int joyAxisTrip;

	JoystickMovement() {};

public:
	JoystickMovement(int xAxis, int yAxis, int trip);

	Vec2 getMovement(InputDevice* d);
};
