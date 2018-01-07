#pragma once
#include "MovementBind.h"

//Class for unbound movements
class NoMovement : public MovementBind
{
public:
    Vec2 getMovement(InputDevice* d) { return Vec2(); }
};
