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
	KeyboardAction(int keyToUse, Uint8* keystatesToUse);

	bool getDigitalAction();
	float getAnalogAction();
};