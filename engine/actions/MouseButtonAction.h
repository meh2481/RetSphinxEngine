#pragma once
#include "ActionBind.h"

class MouseButtonAction : public ActionBind
{
private:
	int button;
	MouseButtonAction() {};

public:
	MouseButtonAction(int button);

	bool getDigitalAction();
	float getAnalogAction();
};