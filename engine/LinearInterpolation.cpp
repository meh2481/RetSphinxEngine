#include "LinearInterpolation.h"

LinearInterpolation::LinearInterpolation(float* v, float d, float t) : Interpolation(v, d, t)
{
}

bool LinearInterpolation::update(float dt)
{
	curTime += dt;

	//See if done
	if(curTime >= time)
	{
		*val = dest;
		return true;
	}

	float timeFac = curTime / time;	// range 0..1 for how far along we are
	float amtFac = dest - start;	// total difference between start and target value

	*val = timeFac * amtFac + start;	// Set to proper linearly-interpolated value

	return false;
}
