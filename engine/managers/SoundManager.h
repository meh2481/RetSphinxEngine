#pragma once
#include "fmod.hpp"
#include <string>
#include <map>

#define SoundHandle FMOD::Sound
#define MusicHandle FMOD::Sound
#define Channel FMOD::Channel

class SoundManager
{
private:
	//TODO: Free sounds if not used after a period of time?
	std::map<const std::string, FMOD::Sound*> sounds;	//Cache for loaded sounds
	std::map<FMOD::Sound*, unsigned int> musicPositions;	//Last play position for each song
	FMOD::System* system;
	Channel* musicChannel;
	FMOD::ChannelGroup* masterChannelGroup;
	FMOD::ChannelGroup* musicGroup;
	FMOD::ChannelGroup* sfxGroup;

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
	void resumeMusic();

	void pauseAll();
	void resumeAll();
};