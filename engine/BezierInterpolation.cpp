#include "BezierInterpolation.h"

BezierInterpolation::BezierInterpolation(float* v, float d, float t) : Interpolation(v, d, t)
{
	//Default Bezier curve
	p1.x = p2.x = t / 2.0f;	//Time is X axis, both control points midway through that
	p1.y = 0;
	p2.y = (d - *v);	//Value is y axis, control points on top/bottom of that
}

bool BezierInterpolation::update(float dt)
{
	curTime += dt;

	//See if done
	if(curTime >= time)
	{
		*val = dest;
		return true;
	}

	//Formula: B(t) = (1-t)^3P0+3(1-t)^2tP1+3(1-t)t^2P2+t^3P3, 0 <= t <= 1

	float t = curTime / time;	// range 0..1 for how far along we are (t)
	float negt = 1.0f - t;

	//In our case, we only care about Y for 1D interpolation
	*val = start + (negt*negt*negt) * 0 + 3 * (negt*negt)*t*p1.y + 3 * negt*t*t*p2.y + t*t*t*p2.y;

	return false;
}
