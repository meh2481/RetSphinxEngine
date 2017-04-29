#pragma once
#include "ActionBind.h"
#include "SDL_gamecontroller.h"

class InputDevice;

class AxisAction : public ActionBind
{
private:
	int joyAxisTrip;
	int axis;
	SDL_GameController* controller;

	AxisAction() {};
public:
	AxisAction(SDL_GameController* controller, int axisToUse, int trip);

	bool getDigitalAction();
	float getAnalogAction();
};