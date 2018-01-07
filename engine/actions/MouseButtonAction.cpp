#include "MouseButtonAction.h"
#include "SDL_mouse.h"

MouseButtonAction::MouseButtonAction(int b)
{
    button = b;
}

bool MouseButtonAction::getDigitalAction(InputDevice* d)
{
    return !!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button));
}

float MouseButtonAction::getAnalogAction(InputDevice* d)
{
    if(getDigitalAction(d))
        return 1.0f;
    return 0.0f;
}
