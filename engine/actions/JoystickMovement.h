#pragma once
#include "MovementBind.h"
#include "SDL_gamecontroller.h"

class JoystickMovement : public MovementBind
{
private:
	int xAxis, yAxis;
	int joyAxisTrip;
	SDL_GameController* controller;

	JoystickMovement() {};

public:
	JoystickMovement(SDL_GameController* controller, int xAxis, int yAxis, int trip);

	Vec2 getMovement();
};
