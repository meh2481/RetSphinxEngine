#pragma once
#include "ActionBind.h"
#include "SDL_gamecontroller.h"

class JoyButtonAction : public ActionBind
{
private:
	int button;
	SDL_GameController* controller;

	JoyButtonAction() {};

public:
	JoyButtonAction(SDL_GameController* c, int buttonIndex);

	bool getDigitalAction();
	float getAnalogAction();
};