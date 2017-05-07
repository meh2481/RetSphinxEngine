#pragma once
#include "Rect.h"

class InputDevice;

class MovementBind
{
public:
	virtual Vec2 getMovement(InputDevice* d) = 0;
};
