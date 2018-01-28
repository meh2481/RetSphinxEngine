#pragma once
#include "fmod.hpp"
#include "ResourceTypes.h"
#include "Rect.h"
#include <string>
#include <map>
#include <vector>

#define MUSIC_FADE_TIME 1.0f

typedef FMOD::Sound SoundHandle;
typedef FMOD::Sound StreamHandle;
typedef FMOD::Channel Channel;
typedef FMOD::DSP SoundFilter;
typedef FMOD::Geometry SoundGeometry;

class ResourceLoader;
class SoundVol;
class InterpolationManager;

#ifdef _DEBUG
    class DebugDraw;
#endif

typedef enum
{
    GROUP_MUSIC,
    GROUP_SFX,
    GROUP_BGFX,
    GROUP_VOX,
    GROUP_MASTER
} SoundGroup;

//My own constants for these, since FMOD doesn't have them
typedef enum
{
    FMOD_DSP_OSCILLATOR_SINE = 0,
    FMOD_DSP_OSCILLATOR_SQUARE = 1,
    FMOD_DSP_OSCILLATOR_SAWUP = 2,
    FMOD_DSP_OSCILLATOR_SAWDOWN = 3,
    FMOD_DSP_OSCILLATOR_TRIANGLE = 4,
    FMOD_DSP_OSCILLATOR_NOISE = 5
} FMOD_DSP_OSCILLATOR_WAVE_TYPE;

class SoundManager
{
private:
    //TODO: Free sounds if not used after a period of time? Or does FMOD handle this already?
    std::map<const std::string, FMOD::Sound*> sounds;    //Cache for loaded sounds
    std::map<StreamHandle*, SoundLoop*> soundLoopPoints;    //Cache for sound looping points
    std::map<StreamHandle*, unsigned int> musicPositions;    //Last play position for each song
    std::map<SoundFilter*, FMOD::ChannelGroup*> filterGroups;    //Groups each filter belongs to
    std::vector<SoundVol*> soundVolumes;
    std::vector<unsigned char*> soundResources;
    FMOD::System* system;
    Channel* musicChannel;
    FMOD::ChannelGroup* masterChannelGroup;
    FMOD::ChannelGroup* musicGroup;
    FMOD::ChannelGroup* sfxGroup;
    FMOD::ChannelGroup* bgFxGroup;
    FMOD::ChannelGroup* voxGroup;
    FMOD::DSP* fftdsp;
    ResourceLoader* loader;
    InterpolationManager* interpolationManager;
    SoundGeometry* soundGeometry;
#ifdef _DEBUG
    bool bDebugDraw;
#endif // _DEBUG

    int init();
    void setGroup(Channel* ch, SoundGroup group);
    void loadLoopPoints(StreamHandle* mus, const std::string& filename);    //Load the loop points for a particular song
    FMOD::ChannelGroup* getGroup(SoundGroup group);
    SoundManager() {};
public:
    SoundManager(ResourceLoader* loader, InterpolationManager* interp);
    ~SoundManager();

    void update();    //Call every frame
    void setListener(const Vec2& listenerPos, const Vec2& listenerVel);

    SoundHandle* loadSound(const std::string& filename);
    StreamHandle* loadStream(const std::string& filename);

    Channel* playSound(SoundHandle* sound, SoundGroup group, const Vec2& pos, const Vec2& vel);
    Channel* playLoop(StreamHandle* music, SoundGroup group, const Vec2& pos, const Vec2& vel);

    //Group functions
    void setVolume(SoundGroup group, float fvol);
    float getVolume(SoundGroup group);
    void stopSounds(SoundGroup group);    //Stop/free all sounds in the given group

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

    //Filter functions
    SoundFilter* createFilter(int filter);
    void destroyFilter(SoundFilter* f);
    void assignFilter(SoundGroup group, SoundFilter* f, int idx);

    //Geometry functions
    void setGeometryWorldSize(float sizeCenterToEdge);                  //Call before calls to createGeometry
    SoundGeometry* createGeometry(int maxpolygons, int maxvertices);    //Create a Geometry object (or return the active if one already exists)
    SoundGeometry* getGeometry() { return soundGeometry; }
    void clearGeometry();                                               //Delete the current Geometry object
#ifdef _DEBUG
    bool shouldDrawDebug() { return bDebugDraw; }
    void drawDebug(DebugDraw* debugDraw);
#endif

    //Global functions
    void pauseAll();    //Pause all sounds/music
    void resumeAll();    //Resume all paused sounds/music
    void setPlaybackRate(float rate);    //Multiplied times default playback freq
    void setVolume(float fvol);    //Global volume
    float getVolume();
};
