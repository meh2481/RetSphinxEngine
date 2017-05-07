#pragma once
#include "ActionBind.h"
#include "SDL_gamecontroller.h"

class JoyButtonAction : public ActionBind
{
private:
	int button;

	JoyButtonAction() {};

public:
	JoyButtonAction(int buttonIndex);

	bool getDigitalAction(InputDevice* d);
	float getAnalogAction(InputDevice* d);
};