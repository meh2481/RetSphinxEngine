#pragma once
#include "ActionBind.h"

//Class for unbound actions
class NoAction : public ActionBind
{
public:
	bool getDigitalAction(InputDevice* d) { return false; }
	float getAnalogAction(InputDevice* d) { return 0.0f; }
};
