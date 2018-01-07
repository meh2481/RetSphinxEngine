#pragma once

class InputDevice;

class ActionBind
{
public:
    //Get an action true/false
    virtual bool getDigitalAction(InputDevice* d) = 0;

    //Get a normalized 0..1 for this action
    virtual float getAnalogAction(InputDevice* d) = 0;

};
