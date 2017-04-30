#include "KeyboardAction.h"
#include "SDL_keyboard.h"

KeyboardAction::KeyboardAction(int keyToUse)
{
	key = keyToUse;
	keystates = SDL_GetKeyboardState(NULL);
}

bool KeyboardAction::getDigitalAction()
{
	return(keystates[key]);
}

float KeyboardAction::getAnalogAction()
{
	if(getDigitalAction())
		return 1.0f;
	return 0.0f;
}
