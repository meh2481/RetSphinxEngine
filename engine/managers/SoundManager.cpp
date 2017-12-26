#include "SoundManager.h"
#include "Logger.h"
#include "ResourceLoader.h"
#include "Hash.h"
#include "ResourceTypes.h"
#include "InterpolationManager.h"
#include "SoundVol.h"
#include <cstring>

#define ERRCHECK(x) //{ LOG_IF(x != 0, WARNING) << "FMOD Error: " << x; }

#define WINDOW_TYPE FMOD_DSP_FFT_WINDOW_RECT
#define LOOP_FOREVER -1
#define DEFAULT_SOUND_FREQ 44100.0f
#define SONG_LOOP_FILE_EXT ".loop"

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
    ch->setChannelGroup(getGroup(group));
}

void SoundManager::loadLoopPoints(StreamHandle* mus, const std::string& filename)
{
    std::map<StreamHandle*, SoundLoop*>::iterator existing = soundLoopPoints.find(mus);
    if(existing == soundLoopPoints.end())
    {
        SoundLoop* sl = loader->getSoundLoop(filename);
        if(sl != NULL)
            soundLoopPoints[mus] = sl;
    }
}

FMOD::ChannelGroup * SoundManager::getGroup(SoundGroup group)
{
    switch(group)
    {
        case GROUP_MUSIC:
            return musicGroup;

        case GROUP_BGFX:
            return bgFxGroup;

        case GROUP_VOX:
            return voxGroup;
    }
    return sfxGroup;
}

SoundManager::SoundManager(ResourceLoader* l, InterpolationManager* interp)
{
    loader = l;
    musicChannel = NULL;
    interpolationManager = interp;
    init();
}

SoundManager::~SoundManager()
{
    //TODO: Save/load song location on app exit?
    FMOD_RESULT result = system->release();
    if(result)
        LOG(WARNING) << "Unable to close FMOD: " << result;
    for(std::vector<unsigned char*>::iterator i = soundResources.begin(); i != soundResources.end(); i++)
        free(*i);
    for(std::map<StreamHandle*, SoundLoop*>::iterator i = soundLoopPoints.begin(); i != soundLoopPoints.end(); i++)
        free(i->second);
    for(std::vector<SoundVol*>::iterator i = soundVolumes.begin(); i != soundVolumes.end(); i++)
        delete *i;
}

void SoundManager::update()
{
    system->update();
    std::vector<SoundVol*>::iterator i = soundVolumes.begin();
    while(i != soundVolumes.end())
    {
        if((*i)->update())
        {
            //Delete & remove if done updating
            delete *i;
            i = soundVolumes.erase(i);
        }
        else
            i++;
    }
}

SoundHandle* SoundManager::loadSound(const std::string& filename)
{
    std::map<const std::string, FMOD::Sound*>::iterator existing = sounds.find(filename);
    if(existing == sounds.end())    //Doesn't exist; load
    {
        LOG(INFO) << "Loading sound " << filename;
        SoundHandle* handle = NULL;

        //Attempt to load from pak
        unsigned int length = 0;

        unsigned char* data = loader->getSound(filename, &length);
        if(data)
        {
            FMOD_CREATESOUNDEXINFO info;
            memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));    //Clear all fields to 0
            info.length = length;    //Set length
            //In pak; load from memory
            FMOD_RESULT result = system->createSound((const char*)data, FMOD_CREATESAMPLE | FMOD_OPENMEMORY_POINT, &info, &handle);
            if(result)
                LOG(WARNING) << "Unable to create sound resource " << filename << "from pak, error " << result;
            soundResources.push_back(data);    //Free this later
        }
        else
        {
            //Not in pak; load from file
            FMOD_RESULT result = system->createSound(filename.c_str(), FMOD_CREATESAMPLE, NULL, &handle);
            if(result)
                LOG(WARNING) << "Unable to create sound resource " << filename << " from file, error " << result;
        }
        sounds[filename] = handle;
        return handle;
    }
    return existing->second;
}

StreamHandle* SoundManager::loadStream(const std::string& filename)
{
    std::map<const std::string, FMOD::Sound*>::iterator existing = sounds.find(filename);
    if(existing == sounds.end())    //Doesn't exist; load
    {
        LOG(INFO) << "Loading music " << filename;
        SoundHandle* handle = NULL;

        //Create a streamed, loopable sound
        FMOD_RESULT result = system->createSound(filename.c_str(), FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, NULL, &handle);
        if(result)
        {
            LOG(WARNING) << "Unable to create music resource " << filename << ", error " << result;
            return NULL;
        }

        sounds[filename] = handle;
        loadLoopPoints(handle, filename + SONG_LOOP_FILE_EXT);
        return handle;
    }
    return existing->second;
}

Channel* SoundManager::playSound(SoundHandle* sound, SoundGroup group)
{
    Channel* ret = NULL;
    FMOD_RESULT result = system->playSound(FMOD_CHANNEL_FREE, sound, true, &ret);
    ERRCHECK(result);
    setGroup(ret, group);
    result = ret->setPaused(false);
    ERRCHECK(result);
    return ret;
}

Channel* SoundManager::playLoop(StreamHandle* stream, SoundGroup group)
{
    //Check if this is a song and we have a song currently playing
    if(musicChannel != NULL && group == GROUP_MUSIC)
    {
        bool playing = false;
        FMOD_RESULT result = musicChannel->isPlaying(&playing);
        ERRCHECK(result);
        if(playing)
        {
            //First check if we're playing this song already
            FMOD::Sound* curPlaying = NULL;
            result = musicChannel->getCurrentSound(&curPlaying);
            ERRCHECK(result);
            if(curPlaying != NULL && curPlaying == stream)
            {
                musicChannel->setPaused(false);
                return musicChannel;    //Don't play over a song we're already playing
            }

            //Save the last playing position for the currently-playing song
            unsigned int pos = 0;
            result = musicChannel->getPosition(&pos, FMOD_TIMEUNIT_MS);
            ERRCHECK(result);
            musicPositions[curPlaying] = pos;
            fadeOutChannel(musicChannel, MUSIC_FADE_TIME);
        }
    }
    Channel* channel;
    //Paused at start so can seek
    FMOD_RESULT result = system->playSound(FMOD_CHANNEL_FREE, stream, true, &channel);
    ERRCHECK(result);

    //Set looping
    result = channel->setLoopCount(LOOP_FOREVER);
    ERRCHECK(result);
    std::map<StreamHandle*, SoundLoop*>::iterator loop = soundLoopPoints.find(stream);
    if(loop != soundLoopPoints.end())
    {
        result = channel->setLoopPoints(loop->second->loopStartMsec, FMOD_TIMEUNIT_MS, loop->second->loopEndMsec, FMOD_TIMEUNIT_MS);
        ERRCHECK(result);
    }

    //Set group
    setGroup(channel, group);

    if(group == GROUP_MUSIC)
    {
        //Start where we last stopped playing this song
        std::map<StreamHandle*, unsigned int>::iterator existing = musicPositions.find(stream);
        if(existing != musicPositions.end())
            result = channel->setPosition(existing->second, FMOD_TIMEUNIT_MS);
        else
            result = channel->setPosition(0, FMOD_TIMEUNIT_MS);    //Set at start to force reflush of stream buffer (See FMOD API)
        ERRCHECK(result);

        musicChannel = channel;    //Save this as our new music channel
        fadeInChannel(musicChannel, MUSIC_FADE_TIME);
    }

    //Start playing
    result = channel->setPaused(false);
    ERRCHECK(result);
    return channel;
}

void SoundManager::setVolume(SoundGroup group, float fvol)
{
    getGroup(group)->setVolume(fvol);
}

float SoundManager::getVolume(SoundGroup group)
{
    float fvol = 1.0f;
    getGroup(group)->getVolume(&fvol);
    return fvol;
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
    ERRCHECK(result);    //0 = Left
    result = channel->getSpectrum(outR, specLen, 1, WINDOW_TYPE);
    ERRCHECK(result);    //1 = Right

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
    ERRCHECK(result);    //0 = Left
}

void SoundManager::getSpectrumR(Channel* channel, float* outSpec, int specLen)
{
    FMOD_RESULT result = channel->getSpectrum(outSpec, specLen, 1, WINDOW_TYPE);
    ERRCHECK(result);    //1 = Right
}

Channel* SoundManager::getChannel(int channelIdx)
{
    Channel* channel;
    FMOD_RESULT result = system->getChannel(channelIdx, &channel);
    ERRCHECK(result);
    return channel;
}

void SoundManager::fadeOutChannel(Channel * ch, float time)
{
    if(ch == musicChannel)
    {
        FMOD::Sound* curPlaying = NULL;
        FMOD_RESULT result = musicChannel->getCurrentSound(&curPlaying);
        ERRCHECK(result);
        if(curPlaying != NULL)
        {
            //Save the last playing position for the currently-playing song
            unsigned int pos = 0;
            result = musicChannel->getPosition(&pos, FMOD_TIMEUNIT_MS);
            ERRCHECK(result);
            musicPositions[curPlaying] = pos;
        }
        musicChannel = NULL;    //This is no longer the music channel (fixes some fadeout issues)
    }
    SoundVol* v = new SoundVol(ch, 0.0f);
    float* start = v->getCur();
    ch->getVolume(start);
    interpolationManager->interpolate(start, 0.0f, time, BEZIER);
    soundVolumes.push_back(v);
}

void SoundManager::fadeInChannel(Channel * ch, float time)
{
    SoundVol* v = new SoundVol(ch, 1.0f);
    float* start = v->getCur();
    *start = 0.0f;
    interpolationManager->interpolate(start, 1.0f, time, BEZIER);
    soundVolumes.push_back(v);
    ch->setVolume(*start);
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

void SoundManager::setPlaybackRate(float rate)
{
    masterChannelGroup->overrideFrequency(DEFAULT_SOUND_FREQ * rate);
}

void SoundManager::setVolume(float fvol)
{
    masterChannelGroup->setVolume(fvol);
}

float SoundManager::getVolume()
{
    float fvol = 1.0f;
    masterChannelGroup->getVolume(&fvol);
    return fvol;
}

void SoundManager::stopSounds(SoundGroup group)
{
    FMOD::ChannelGroup* sg = getGroup(group);
    FMOD_RESULT result = sg->stop();
    ERRCHECK(result);
}
