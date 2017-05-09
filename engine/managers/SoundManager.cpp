#include "SoundManager.h"
#include "easylogging++.h"
#include <cstring>

const float soundFreqDefault = 44100.0;

#define ERRCHECK(x) { LOG_IF(x != 0, WARNING) << "FMOD Error: " << x; }

//Example initialization code from FMOD API doc
int SoundManager::init()
{
	LOG(INFO) << "Initializing FMOD...";
	FMOD_RESULT result;
	unsigned int version;
	int numdrivers;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS caps;
	char name[256];
	/*
	Create a System object and initialize.
	*/
	result = FMOD::System_Create(&system);
	ERRCHECK(result);
	result = system->getVersion(&version);
	ERRCHECK(result);
	if(version < FMOD_VERSION)
	{
		LOG(ERROR) << "Error! You are using an old version of FMOD " << version << ". This program requires " << FMOD_VERSION;
		return 1;
	}
	result = system->getNumDrivers(&numdrivers);
	ERRCHECK(result);
	if(numdrivers == 0)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ERRCHECK(result);
		LOG(WARNING) << "No sound driver";
	}
	else
	{
		result = system->getDriverCaps(0, &caps, 0, &speakermode);
		ERRCHECK(result);
		/*
		Set the user selected speaker mode.
		*/
		result = system->setSpeakerMode(speakermode);
		ERRCHECK(result);
		if(caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			/*
			The user has the 'Acceleration' slider set to off! This is really bad
			for latency! You might want to warn the user about this.
			*/
			result = system->setDSPBufferSize(1024, 10);
			ERRCHECK(result);
		}
		result = system->getDriverInfo(0, name, 256, 0);
		ERRCHECK(result);
		if(strstr(name, "SigmaTel"))
		{
			/*
			Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			PCM floating point output seems to solve it.
			*/
			result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			ERRCHECK(result);
		}
	}
	result = system->init(100, FMOD_INIT_NORMAL, 0);
	if(result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		/*
		Ok, the speaker mode selected isn't supported by this soundcard. Switch it
		back to stereo...
		*/
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ERRCHECK(result);
		/*
		... and re-init.
		*/
		result = system->init(100, FMOD_INIT_NORMAL, 0);
	}
	ERRCHECK(result);

	result = system->createChannelGroup("Master", &masterChannelGroup);
	ERRCHECK(result);
	result = system->createChannelGroup("Music", &musicGroup);
	ERRCHECK(result);
	result = system->createChannelGroup("SFX", &sfxGroup);
	ERRCHECK(result);

	masterChannelGroup->addGroup(musicGroup);
	masterChannelGroup->addGroup(sfxGroup);

	LOG(INFO) << "FMOD Init success";
	musicChannel = NULL;

	return 0;
}

SoundManager::SoundManager()
{
	init();
}

SoundManager::~SoundManager()
{
	FMOD_RESULT result = system->release();
	if(result)
		LOG(WARNING) << "Unable to close FMOD: " << result;
}

void SoundManager::update()
{
	system->update();
}

SoundHandle* SoundManager::loadSound(const std::string & filename)
{
	LOG(INFO) << "Loading sound " << filename;
	SoundHandle* handle = NULL;

	FMOD_RESULT result = system->createSound(filename.c_str(), FMOD_CREATESAMPLE, NULL, &handle);
	if(result)
		LOG(WARNING) << "Unable to create sound resource " << filename << ", error " << result;

	return handle;
}

MusicHandle* SoundManager::loadMusic(const std::string & filename)
{
	LOG(INFO) << "Loading music " << filename;
	SoundHandle* handle = NULL;

	FMOD_RESULT result = system->createSound(filename.c_str(), FMOD_CREATESTREAM, NULL, &handle);
	if(result)
		LOG(WARNING) << "Unable to create music resource " << filename << ", error " << result;

	return handle;
}

Channel * SoundManager::playSound(SoundHandle * sound)
{
	Channel* ret = NULL;
	FMOD_RESULT result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &ret);
	if(result)
		LOG(WARNING) << "Unable to play sound: " << result;
	ret->setChannelGroup(sfxGroup);
	return ret;
}

Channel * SoundManager::playMusic(MusicHandle * music)
{
	FMOD_RESULT result = system->playSound(FMOD_CHANNEL_FREE, music, false, &musicChannel);
	if(result)
		LOG(WARNING) << "Unable to play music: " << result;
	musicChannel->setChannelGroup(musicGroup);
	return musicChannel;
}

void SoundManager::pause(Channel * channel)
{
	channel->setPaused(true);
}

void SoundManager::resume(Channel * channel)
{
	channel->setPaused(false);
}

bool SoundManager::isPaused(Channel * channel)
{
	bool paused;
	channel->getPaused(&paused);
	return paused;
}

void SoundManager::stop(Channel * channel)
{
	channel->stop();
}

void SoundManager::pauseMusic()
{
	if(musicChannel)
		musicChannel->setPaused(true);
}

void SoundManager::resumeMusic()
{
	if(musicChannel)
		musicChannel->setPaused(false);
}

void SoundManager::pauseAll()
{
	if(masterChannelGroup)
		masterChannelGroup->setPaused(true);
}

void SoundManager::resumeAll()
{
	if(masterChannelGroup)
		masterChannelGroup->setPaused(false);
}
