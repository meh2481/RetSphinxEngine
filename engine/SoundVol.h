#pragma once
#include "fmod.hpp"

class SoundVol
{
	float dest;
	float cur;
	FMOD::Channel* sfx;

	SoundVol() {};
public:

	SoundVol(FMOD::Channel* s, float d);	//Fade sound to d volume

	bool update();

	float* getCur() { return &cur; }
};