#pragma once
#include <string>
#include "MemoryEditor.h"
#include "Color.h"

class GameEngine;
class ParticleSystem;
class SteelSeriesEditor;
class ParticleEditor;
class LevelEditor;

class DebugUI
{
public:
	DebugUI(GameEngine*);
	~DebugUI();
	void draw();
	bool hasFocus();

	bool visible;
	bool hadFocus;

	//Steel Series editor
	SteelSeriesEditor* steelSeriesEditor;

	//Particle system editor
	ParticleEditor* particleEditor;

private:
	void _draw();

	MemoryEditor memEdit;
	LevelEditor* levelEditor;
	GameEngine *_ge;

	bool rumbleMenu;
	int windowFlags;

	//Rumble testing helper vars
	int largeMotorStrength;
	int smallMotorStrength;
	int motorDuration;
};
