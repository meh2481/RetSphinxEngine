#pragma once
#include <string>

class GameEngine;

#include "MemoryEditor.h"

class DebugUI
{
public:
	DebugUI(GameEngine*);
	~DebugUI();
	void draw();
	bool hasFocus();

	bool visible;
	bool hadFocus;

private:
	void _draw();
	MemoryEditor memEdit;
	GameEngine *_ge;

	bool showTestWindow;
	bool rumbleMenu;

	//Rumble testing helper vars
	int largeMotorStrength;
	int smallMotorStrength;
	int motorDuration;
	std::string eventType;
	int selectedSSMouseRumble;
	int rumbleCount;
	float rumbleFreq;
	int rumbleLen;

	//Mouse color testing helper vars
	float mouseWheelColor[3];
	float mouseLogoColor[3];

	//Mouse screen testing helper vars
	int selectedEventIcon;
};
