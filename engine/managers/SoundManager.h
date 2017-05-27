#pragma once
#include "fmod.hpp"
#include "ResourceTypes.h"
#include <string>
#include <map>
#include <vector>

#define MUSIC_FADE_TIME 1.0f

typedef FMOD::Sound SoundHandle;
typedef FMOD::Sound StreamHandle;
typedef FMOD::Channel Channel;

class ResourceLoader;
class SoundVol;
class InterpolationManager;

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
	//TODO: Free sounds if not used after a period of time? Or does FMOD handle this already?
	std::map<const std::string, FMOD::Sound*> sounds;	//Cache for loaded sounds
	std::map<StreamHandle*, SoundLoop*> soundLoopPoints;	//Cache for sound looping points
	std::map<StreamHandle*, unsigned int> musicPositions;	//Last play position for each song
	std::vector<SoundVol*> soundVolumes;
	std::vector<unsigned char*> soundResources;
	FMOD::System* system;
	Channel* musicChannel;
	FMOD::ChannelGroup* masterChannelGroup;
	FMOD::ChannelGroup* musicGroup;
	FMOD::ChannelGroup* sfxGroup;
	FMOD::ChannelGroup* bgFxGroup;
	FMOD::ChannelGroup* voxGroup;
	ResourceLoader* loader;
	InterpolationManager* interpolationManager;

	int init();
	void setGroup(Channel* ch, SoundGroup group);
	void loadLoopPoints(StreamHandle* mus, const std::string& filename);	//Load the loop points for a particular song
	FMOD::ChannelGroup* getGroup(SoundGroup group);
	SoundManager() {};
public:
	SoundManager(ResourceLoader* loader, InterpolationManager* interp);
	~SoundManager();

	void update();	//Call every frame

	SoundHandle* loadSound(const std::string& filename);
	StreamHandle* loadStream(const std::string& filename);

	Channel* playSound(SoundHandle* sound, SoundGroup group = GROUP_SFX);
	Channel* playLoop(StreamHandle* music, SoundGroup group = GROUP_MUSIC);

	//Group functions
	void setVolume(SoundGroup group, float fvol);
	float getVolume(SoundGroup group);
	void stopSounds(SoundGroup group);	//Stop/free all sounds in the given group

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
	void fadeOutChannel(Channel* ch, float time);
	void fadeInChannel(Channel* ch, float time);

	//Music functions
	void pauseMusic();
	void resumeMusic();
	Channel* getMusicChannel() { return musicChannel; }

	//Global functions
	void pauseAll();	//Pause all sounds/music
	void resumeAll();	//Resume all paused sounds/music
	void setPlaybackRate(float rate);	//Multiplied times default playback freq
	void setVolume(float fvol);	//Global volume
	float getVolume();
};