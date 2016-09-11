#pragma once
#include <string>

#define SS_BUF_SZ 33

class GameEngine;

class SteelSeriesEditor
{
public:
	bool open;

	void draw(int windowFlags);

	SteelSeriesEditor(GameEngine* ge);

private:

	SteelSeriesEditor() {};

	GameEngine* _ge;

	std::string appName;

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
	void bindTactileEvent(std::string eventId);
	void bindScreenEvent(std::string eventId);
	void bindColorEvent(std::string eventId);
};
