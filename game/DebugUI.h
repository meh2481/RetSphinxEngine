#pragma once
#include <string>
#include "MemoryEditor.h"
#include "Color.h"

#define SS_BUF_SZ 33
#define SAVE_BUF_SZ 256

class GameEngine;
class ParticleSystem;

class DebugUI
{
public:
	DebugUI(GameEngine*);
	~DebugUI();
	void draw();
	bool hasFocus();

	bool visible;
	bool hadFocus;


	//Variables for particle system editing
	bool particleSystemEdit;
	ParticleSystem* particles;
	Color particleBgColor;

private:
	void _draw();
	MemoryEditor memEdit;
	GameEngine *_ge;

	bool rumbleMenu;
	int windowFlags;
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

	//Helper variables for particle-system editing
	float emitRect[4];
	float emitVel[2];
	float startSz[2];
	float endSz[2];
	float accel[2];
	float accelVar[2];
	float rotAxis[3];
	float rotAxisVar[3];
	float startCol[4];
	float endCol[4];
	float colVar[4];
	float bgCol[3];
	bool psysDecay;
	bool loadParticles;
	bool saveParticles;
	int curSelectedLoadSaveItem;
	char saveFilenameBuf[SAVE_BUF_SZ];
	bool fireOnStart;

	//Helper functions
	void updateHelperVars();	//Keep the above up-to-date with the particle system
	void saveParticleSystemXML(std::string filename);	//Save the current particle system to XML
};
