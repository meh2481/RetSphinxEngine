#include "JoyButtonAction.h"
#include "InputDevice.h"

JoyButtonAction::JoyButtonAction(int buttonIndex)
{
    button = buttonIndex;
}

bool JoyButtonAction::getDigitalAction(InputDevice* d)
{
    return d->getButton(button);
}

float JoyButtonAction::getAnalogAction(InputDevice* d)
{
    if(getDigitalAction(d))
        return 1.0f;
    return 0.0f;
}
