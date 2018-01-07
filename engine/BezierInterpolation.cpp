#include "BezierInterpolation.h"

BezierInterpolation::BezierInterpolation(float* v, float d, float t) : Interpolation(v, d, t)
{
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

    float t = curTime / time;    // range 0..1 for how far along we are (t)
    float negt = 1.0f - t;
    float p = (dest - start);    //Value is y axis, control points on top/bottom of that

    //In our case, we only care about Y for 1D interpolation
    *val = start + 3 * negt*t*t*p + t*t*t*p;    //Simplified a lot

    return false;
}
