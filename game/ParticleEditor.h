#pragma once
#include "Color.h"
#include <string>

#define SAVE_BUF_SZ 256

class GameEngine;
class ParticleSystem;

class ParticleEditor
{
public:
	Color particleBgColor;
	ParticleSystem* particles;
	bool open;

	ParticleEditor(GameEngine* ge);
	~ParticleEditor();

	void draw(int windowFlags);

private:
	GameEngine* _ge;
	ParticleEditor() {};

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
	int curSelectedSpawn;
	bool spawnParticleSelect;
	int curSelectedSpawnSystem;
	int curSelectedImgRect;
	bool loadParticleImage;
	int curSelectedLoadImage;

	//Helper functions
	void updateHelperVars();	//Keep the above up-to-date with the particle system
	void saveParticleSystemXML(std::string filename);	//Save the current particle system to XML
};