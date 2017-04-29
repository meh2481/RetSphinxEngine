#include "KeyboardAction.h"

KeyboardAction::KeyboardAction(int keyToUse, Uint8* keystatesToUse)
{
	key = keyToUse;
	keystates = keystatesToUse;
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
