#pragma once
#include "MovementBind.h"

class DpadMovement : public MovementBind
{
private:
	int up, down, left, right;

	DpadMovement() {};

public:
	DpadMovement(int left, int right, int up, int down);

	Vec2 getMovement(InputDevice* d);

};