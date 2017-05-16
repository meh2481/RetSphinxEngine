#include "SoundManager.h"
#include "easylogging++.h"
#include <cstring>

#define ERRCHECK(x) { LOG_IF(x != 0, WARNING) << "FMOD Error: " << x; }

#define WINDOW_TYPE FMOD_DSP_FFT_WINDOW_RECT
#define LOOP_FOREVER -1

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
			LOG(WARNING) << "Sound acceleration disabled. May cause sound latency problems";
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

	//Set up sound groups
	//Can't use system->getMasterChannelGroup() as we can't pause/resume that group
	result = system->createChannelGroup("Master", &masterChannelGroup);
	ERRCHECK(result);
	result = system->createChannelGroup("Music", &musicGroup);
	ERRCHECK(result);
	result = system->createChannelGroup("SFX", &sfxGroup);
	ERRCHECK(result);
	result = system->createChannelGroup("Background Sounds", &bgFxGroup);
	ERRCHECK(result);
	result = system->createChannelGroup("Voices", &voxGroup);
	ERRCHECK(result);

	masterChannelGroup->addGroup(musicGroup);
	masterChannelGroup->addGroup(sfxGroup);
	masterChannelGroup->addGroup(bgFxGroup);
	masterChannelGroup->addGroup(voxGroup);

	LOG(INFO) << "FMOD Init success";

	return 0;
}

void SoundManager::setGroup(Channel* ch, SoundGroup group)
{
	switch(group)
	{
		case GROUP_MUSIC:
			ch->setChannelGroup(musicGroup);
			break;

		case GROUP_BGFX:
			ch->setChannelGroup(bgFxGroup);
			break;

		case GROUP_VOX:
			ch->setChannelGroup(voxGroup);
			break;

		case GROUP_SFX:
		default:
			ch->setChannelGroup(sfxGroup);
			break;
	}
}

SoundManager::SoundManager()
{
	musicChannel = NULL;
	init();
}

SoundManager::~SoundManager()
{
	//TODO: Save/load song location on exit?
	FMOD_RESULT result = system->release();
	if(result)
		LOG(WARNING) << "Unable to close FMOD: " << result;
}

void SoundManager::update()
{
	system->update();
}

SoundHandle* SoundManager::loadSound(const std::string& filename)
{
	std::map<const std::string, FMOD::Sound*>::iterator existing = sounds.find(filename);
	if(existing == sounds.end())	//Doesn't exist; load
	{
		LOG(INFO) << "Loading sound " << filename;
		SoundHandle* handle = NULL;

		FMOD_RESULT result = system->createSound(filename.c_str(), FMOD_CREATESAMPLE, NULL, &handle);
		if(result)
			LOG(WARNING) << "Unable to create sound resource " << filename << ", error " << result;

		sounds[filename] = handle;
		return handle;
	}
	return existing->second;
}

MusicHandle* SoundManager::loadMusic(const std::string& filename)
{
	std::map<const std::string, FMOD::Sound*>::iterator existing = sounds.find(filename);
	if(existing == sounds.end())	//Doesn't exist; load
	{
		LOG(INFO) << "Loading music " << filename;
		SoundHandle* handle = NULL;

		//Create a streamed, loopable sound
		FMOD_RESULT result = system->createSound(filename.c_str(), FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, NULL, &handle);
		if(result)
			LOG(WARNING) << "Unable to create music resource " << filename << ", error " << result;

		sounds[filename] = handle;
		return handle;
	}
	return existing->second;
}

Channel* SoundManager::playSound(SoundHandle* sound, SoundGroup group)
{
	Channel* ret = NULL;
	FMOD_RESULT result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &ret);
	ERRCHECK(result);
	setGroup(ret, group);
	return ret;
}

Channel* SoundManager::playMusic(MusicHandle* music, SoundGroup group)
{
	//Check if we have a song currently playing
	if(musicChannel != NULL)
	{
		bool playing = false;
		FMOD_RESULT result = musicChannel->isPlaying(&playing);
		ERRCHECK(result);
		if(playing)
		{
			//Save the last playing position
			unsigned int pos = 0;
			result = musicChannel->getPosition(&pos, FMOD_TIMEUNIT_MS);
			ERRCHECK(result);
			musicPositions[music] = pos;
			musicChannel->stop();
		}
	}
	//Paused at start so can seek
	FMOD_RESULT result = system->playSound(FMOD_CHANNEL_FREE, music, false, &musicChannel);
	ERRCHECK(result);
	
	//Set looping
	result = musicChannel->setLoopCount(LOOP_FOREVER);
	ERRCHECK(result);
	//TODO result = musicChannel->setLoopPoints(loopstart, FMOD_TIMEUNIT_MS, loopend, FMOD_TIMEUNIT_MS);
	//ERRCHECK(result);

	//Set group
	setGroup(musicChannel, group);

	//Start where we last stopped playing this song
	std::map<MusicHandle*, unsigned int>::iterator existing = musicPositions.find(music);
	if(existing != musicPositions.end())
		result = musicChannel->setPosition(existing->second, FMOD_TIMEUNIT_MS);
	else
		result = musicChannel->setPosition(0, FMOD_TIMEUNIT_MS);	//Set at start to force reflush of stream buffer (See FMOD API)
	ERRCHECK(result);
	
	//Start playing
	result = musicChannel->setPaused(false);
	ERRCHECK(result);
	return musicChannel;
}

void SoundManager::pause(Channel* channel)
{
	channel->setPaused(true);
}

void SoundManager::resume(Channel* channel)
{
	channel->setPaused(false);
}

bool SoundManager::isPaused(Channel* channel)
{
	bool paused;
	channel->getPaused(&paused);
	return paused;
}

void SoundManager::stop(Channel* channel)
{
	channel->stop();
}

float SoundManager::getFreq(Channel* channel)
{
	float freq = 0.0f;
	FMOD_RESULT result = channel->getFrequency(&freq);
	ERRCHECK(result);
	return freq;
}

void SoundManager::setFreq(Channel* channel, float freq)
{
	channel->setFrequency(freq);
}

void SoundManager::getSpectrum(Channel* channel, float* outSpec, int specLen)
{
	float* outL = new float[specLen];
	float* outR = new float[specLen];

	//Get spectrum for both left and right
	FMOD_RESULT result = channel->getSpectrum(outL, specLen, 0, WINDOW_TYPE);
	ERRCHECK(result);	//0 = Left
	result = channel->getSpectrum(outR, specLen, 1, WINDOW_TYPE);
	ERRCHECK(result);	//1 = Right

	//Average them
	float* l = outL;
	float* r = outR;
	for(int i = 0; i < specLen; i++)
		*outSpec++ = (*l++ + *r++) / 2.0f;

	delete[] outL;
	delete[] outR;
}

void SoundManager::getSpectrumL(Channel* channel, float* outSpec, int specLen)
{
	FMOD_RESULT result = channel->getSpectrum(outSpec, specLen, 0, WINDOW_TYPE);
	ERRCHECK(result);	//0 = Left
}

void SoundManager::getSpectrumR(Channel* channel, float* outSpec, int specLen)
{
	FMOD_RESULT result = channel->getSpectrum(outSpec, specLen, 1, WINDOW_TYPE);
	ERRCHECK(result);	//1 = Right
}

Channel* SoundManager::getChannel(int channelIdx)
{
	Channel* channel;
	FMOD_RESULT result = system->getChannel(channelIdx, &channel);
	ERRCHECK(result);
	return channel;
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
