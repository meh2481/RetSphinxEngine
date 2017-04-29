#pragma once
#include "ActionBind.h"

//Class for unbound actions
class NoAction : ActionBind
{
public:
	bool getDigitalAction() { return false; }
	float getAnalogAction() { return 0.0f; }
};
