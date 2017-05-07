#pragma once
#include "fmod.hpp"
#include <string>

#define SoundHandle FMOD::Sound
#define MusicHandle FMOD::Sound
#define Channel FMOD::Channel

class SoundManager
{
private:
	FMOD::System* system;

	int init();
public:
	SoundManager();
	~SoundManager();

	void update();

	SoundHandle* loadSound(const std::string& filename);
	MusicHandle* loadMusic(const std::string& filename);

	Channel* playSound(SoundHandle* sound);
	Channel* playMusic(MusicHandle* music);

	void pause(Channel* channel);
	void resume(Channel* channel);
	bool isPaused(Channel* channel);
	void stop(Channel* channel);

	void pauseMusic();
	void playMusic();
};