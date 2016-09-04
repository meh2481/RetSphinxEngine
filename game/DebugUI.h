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

	//Helper functions for dealing with SteelSeries mouse testing
	void bindTactileEvent(std::string eventType, std::string eventId, float rumbleFreq, int rumbleCount, int rumbleLen = 100);
	void bindScreenEvent(std::string eventId, int iconId, int ms = 0, std::string prefixText = "", std::string suffixText = "");
	void bindColorEvent(std::string eventId, std::string zone, float zeroColor[3], float hundredColor[3], bool flashCol = false, float colorFlashFreq = 1.0f, int colorFlashCount = 0);
};
