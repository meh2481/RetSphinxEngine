#pragma once
#include "MovementBind.h"
#include "SDL_gamecontroller.h"

class JoystickMovement : public MovementBind
{
private:
    int xAxis, yAxis;
    float joyAxisTrip;

    JoystickMovement() {};

public:
    JoystickMovement(int xAxis, int yAxis, float trip);

    Vec2 getMovement(InputDevice* d);
};
