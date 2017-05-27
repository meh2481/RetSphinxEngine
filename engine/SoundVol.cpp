#include "SoundVol.h"

SoundVol::SoundVol(FMOD::Channel* s, float d)
{
	cur = 1.0f;
	sfx = s;
	dest = d;
	sfx->getVolume(&cur);
}

bool SoundVol::update()
{
	sfx->setVolume(cur);
	if(cur == dest)
	{
		if(dest == 0.0f)
			sfx->stop();
		return true;
	}
	return false;
}
