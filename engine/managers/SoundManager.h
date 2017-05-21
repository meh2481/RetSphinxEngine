#pragma once
#include "fmod.hpp"
#include "ResourceTypes.h"
#include <string>
#include <map>
#include <vector>

#define SoundHandle FMOD::Sound
#define MusicHandle FMOD::Sound
#define Channel FMOD::Channel

class ResourceLoader;

typedef enum
{
	GROUP_MUSIC,
	GROUP_SFX,
	GROUP_BGFX,
	GROUP_VOX
} SoundGroup;

class SoundManager
{
private:
	//TODO: Free sounds if not used after a period of time?
	std::map<const std::string, FMOD::Sound*> sounds;	//Cache for loaded sounds
	std::map<MusicHandle*, SongLoop*> musicLoopPoints;	//Cache for music looping points
	std::map<MusicHandle*, unsigned int> musicPositions;	//Last play position for each song
	std::vector<unsigned char*> soundResources;
	FMOD::System* system;
	Channel* musicChannel;
	FMOD::ChannelGroup* masterChannelGroup;
	FMOD::ChannelGroup* musicGroup;
	FMOD::ChannelGroup* sfxGroup;
	FMOD::ChannelGroup* bgFxGroup;
	FMOD::ChannelGroup* voxGroup;
	ResourceLoader* loader;

	int init();
	void setGroup(Channel* ch, SoundGroup group);
	void loadMusicLoopPoints(MusicHandle* mus, const std::string& filename);	//Load the loop points for a particular song
	SoundManager() {};
public:
	SoundManager(ResourceLoader* loader);
	~SoundManager();

	void update();

	SoundHandle* loadSound(const std::string& filename);
	MusicHandle* loadMusic(const std::string& filename);

	Channel* playSound(SoundHandle* sound, SoundGroup group = GROUP_SFX);
	Channel* playMusic(MusicHandle* music, SoundGroup group = GROUP_MUSIC);

	//Channel functions
	void pause(Channel* channel);
	void resume(Channel* channel);
	bool isPaused(Channel* channel);
	void stop(Channel* channel);
	float getFreq(Channel* channel);
	void setFreq(Channel* channel, float freq);
	void getSpectrum(Channel* channel, float* outSpec, int specLen);
	void getSpectrumL(Channel* channel, float* outSpec, int specLen);
	void getSpectrumR(Channel* channel, float* outSpec, int specLen);
	Channel* getChannel(int channelIdx);

	//Music functions
	void pauseMusic();
	void resumeMusic();
	Channel* getMusicChannel() { return musicChannel; }

	//Global functions
	void pauseAll();
	void resumeAll();

	void setPlaybackRate(float rate);	//Multiplied times default playback freq
};