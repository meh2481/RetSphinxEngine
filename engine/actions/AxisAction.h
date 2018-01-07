#pragma once
#include "ActionBind.h"

class InputDevice;

class AxisAction : public ActionBind
{
private:
    int joyAxisTrip;
    int axis;

    AxisAction() {};
public:
    AxisAction(int axisToUse, int trip);

    bool getDigitalAction(InputDevice* d);
    float getAnalogAction(InputDevice* d);
};