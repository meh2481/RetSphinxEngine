#pragma once
#include <string>
#include "MemoryEditor.h"

#define SS_BUF_SZ 33

class GameEngine;


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
	int windowFlags;

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
	std::string colorZone;
	int selectedSSMouseColorZone;
	int colorValue;
	float mouse0Color[3];
	float mouse100Color[3];
	bool colorFlash;
	float colorFlashFreq;
	int colorFlashCount;

	//Mouse screen testing helper vars
	int selectedEventIcon;
	int percentHealth;
	int screenMs;
	char prefixBuf[SS_BUF_SZ];
	char suffixBuf[SS_BUF_SZ];
};
