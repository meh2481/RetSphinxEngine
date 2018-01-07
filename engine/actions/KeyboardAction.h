#pragma once
#include "ActionBind.h"
#include "SDL_scancode.h"

class KeyboardAction : public ActionBind
{
private:
    int key;
    const Uint8* keystates;
    KeyboardAction() {};

public:
    KeyboardAction(int keyToUse);

    bool getDigitalAction(InputDevice* d);
    float getAnalogAction(InputDevice* d);
};