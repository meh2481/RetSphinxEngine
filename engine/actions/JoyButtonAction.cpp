#include "JoyButtonAction.h"
#include "InputDevice.h"

JoyButtonAction::JoyButtonAction(SDL_GameController* c, int buttonIndex)
{
	controller = c;
	button = buttonIndex;
}

bool JoyButtonAction::getDigitalAction()
{
	return(SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)button) > 0);
}

float JoyButtonAction::getAnalogAction()
{
	if(getDigitalAction())
		return 1.0f;
	return 0.0f;
}
