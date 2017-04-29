#pragma once

class ActionBind
{
public:
	//Get an action true/false
	virtual bool getDigitalAction() = 0;

	//Get a normalized 0..1 for this action
	virtual float getAnalogAction() = 0;

};
