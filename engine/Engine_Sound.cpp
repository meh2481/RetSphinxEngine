#include "Engine.h"
#include "easylogging++.h"
using namespace std;

void Engine::createSound(string sPath, string sName)
{
    /*
	if(m_bSoundDied || m_sounds.count(sName)) return;	//Don't duplicate sounds or attempt to play sounds if we can't
	LOG(INFO) << "Load sound " << sPath
	FMOD_SOUND* handle;
	FMOD_MODE open_mode = FMOD_CREATESAMPLE;
#ifndef DEBUG_REVSOUND
	if(sName == "music")
		open_mode = FMOD_CREATESTREAM;	//Open music as a stream, sounds as samples (so the sounds can play multiple times at once)
#endif
	if(FMOD_System_CreateSound(m_audioSystem, sPath.c_str(), open_mode, 0, &handle) == FMOD_OK)
		m_sounds[sName] = handle;
    */
}

void Engine::playSound(string sName, float volume, float pan, float pitch)
{
    /*
	if(m_bSoundDied || !m_sounds.count(sName)) return;
	FMOD_CHANNEL* channel;
	FMOD_System_PlaySound(m_audioSystem, FMOD_CHANNEL_FREE, m_sounds[sName], false, &channel);
	pair<string, FMOD_CHANNEL*> chan;
	chan.first = sName;
	chan.second = channel;
	FMOD_Channel_SetVolume(channel, volume);
	FMOD_Channel_SetPan(channel, pan);
	FMOD_Channel_SetFrequency(channel, pitch * soundFreqDefault);
	m_channels.insert(chan);
    */
}

/*
FMOD_CHANNEL* Engine::getChannel(string sSoundName)
{
	multimap<string, FMOD_CHANNEL*>::iterator i = m_channels.find(sSoundName);
	if(i != m_channels.end())
		return i->second;
	return NULL;
}
*/

void Engine::playMusic(string sName, float volume, float pan, float pitch)
{
    /*
	if(m_bSoundDied) return;
	if(m_sounds.count("music"))
	{
		stopMusic();
		m_sounds.erase("music");
		m_channels.erase("music");
	}
	createSound(sName, "music");
	playSound("music", volume, pan, pitch);
	if(m_channels.count("music"))
	{
		FMOD_CHANNEL* mus = getChannel("music");
		FMOD_Channel_SetLoopCount(mus, -1);
		FMOD_Channel_SetMode(mus, FMOD_LOOP_NORMAL);
		FMOD_Channel_SetPosition(mus, 0, FMOD_TIMEUNIT_MS);
	}
    */
}

void Engine::musicLoop(float startSec, float endSec)
{
    /*
	if(m_bSoundDied) return;
	if(m_channels.count("music"))
	{
		FMOD_CHANNEL* mus = getChannel("music");
		FMOD_Channel_SetLoopPoints(mus, startSec * 1000, FMOD_TIMEUNIT_MS, endSec * 1000, FMOD_TIMEUNIT_MS);
		//Flush music stream
		FMOD_Channel_SetMode(mus, FMOD_LOOP_NORMAL);
		unsigned int ms;
		FMOD_Channel_GetPosition(mus, &ms, FMOD_TIMEUNIT_MS);
		FMOD_Channel_SetPosition(mus, ms, FMOD_TIMEUNIT_MS);
	}
    */
}

void Engine::pauseMusic()
{
    /*
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPaused(getChannel("music"), true);
    */
}

void Engine::resumeMusic()
{
    /*
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPaused(getChannel("music"), false);
    */
}

void Engine::restartMusic()
{
    /*
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPosition(getChannel("music"), 0, FMOD_TIMEUNIT_MS);
    */
}

void Engine::stopMusic()
{
    /*
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPaused(getChannel("music"), true);
    */
}

void Engine::seekMusic(float fTime)
{
    /*
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPosition(getChannel("music"), fTime * 1000.0, FMOD_TIMEUNIT_MS);
    */
}

float Engine::getMusicPos()
{
    /*
	if(m_bSoundDied) return -1;
	if(m_channels.count("music"))
	{
		FMOD_CHANNEL* mus = getChannel("music");
		unsigned int ms;
		FMOD_Channel_GetPosition(mus, &ms, FMOD_TIMEUNIT_MS);
		return (float)ms / 1000.0f;
	}
    */
	return -1;
}

void Engine::volumeMusic(float fVol)
{
    /*
	if(m_bSoundDied) return;
	if(m_channels.count("music"))
	{
		FMOD_CHANNEL* mus = getChannel("music");
		FMOD_Channel_SetVolume(mus, fVol);
	}
    */
}

void Engine::setMusicFrequency(float freq)
{
    /*
	if(m_bSoundDied) return;
	if(m_channels.count("music"))
		FMOD_Channel_SetFrequency(getChannel("music"), freq);
    */
}

float Engine::getMusicFrequency()
{
    /*
	if(m_bSoundDied) return -1;
	if(m_channels.count("music"))
	{
		float freq;
		FMOD_Channel_GetFrequency(getChannel("music"), &freq);
		return freq;
	}
    */
	return -1;
}

bool Engine::hasMic()
{
    /*
	if(m_bSoundDied) return false;
	int numDrivers = 0;
	FMOD_System_GetRecordNumDrivers(m_audioSystem, &numDrivers);
	return numDrivers;
    */
    return false;
}

void Engine::updateSound()
{
    /*
	if(m_bSoundDied) return;
	
	//Update FMOD
	FMOD_System_Update(m_audioSystem);
	
	//Get rid of sounds that aren't currently playing
	for(multimap<string, FMOD_CHANNEL*>::iterator i = m_channels.begin(); i != m_channels.end();i++)
	{
		FMOD_BOOL bPlaying;
		if(FMOD_Channel_IsPlaying(i->second, &bPlaying) != FMOD_OK) continue;
		if(!bPlaying && i->first != "music")
		{
			m_channels.erase(i);
			continue;
		}
	}
    */
}
