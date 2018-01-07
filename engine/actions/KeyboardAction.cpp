#include "KeyboardAction.h"
#include "SDL_keyboard.h"

KeyboardAction::KeyboardAction(int keyToUse)
{
    key = keyToUse;
    keystates = SDL_GetKeyboardState(NULL);
}

bool KeyboardAction::getDigitalAction(InputDevice* d)
{
    return !!(keystates[key]);
}

float KeyboardAction::getAnalogAction(InputDevice* d)
{
    if(getDigitalAction(d))
        return 1.0f;
    return 0.0f;
}
