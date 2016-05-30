#include "Interpolator.h"
#include <math.h>
#include <algorithm>

template<typename T>
inline T interp_linear(const T& a, const T& b, const float t)
{
    return a + (b-a) * t;
}

template<typename T>
inline T interp_linear_easein(const T& a, const T& b, const float t)
{
    const float s = t * (sinf(t*PI_HALF) + (1-t));
    return interp_linear(a, b, s);
}

template<typename T>
inline T interp_linear_easeout(const T& a, const T& b, const float t)
{
    const float s = sinf(t*PI_HALF);
    return interp_linear(a, b, s);
}

template<typename T>
inline T interp_linear_easeinout(const T& a, const T& b, const float t)
{
    const float t2 = t * t;
    const float t3 = t2 * t;
    return a*(2*t3 - 3*t2 + 1) + b*(3*t2 - 2*t3);
}

template<typename T>
static T interp(InterpolationType ty, const T& a, const T& b, const float t)
{
    switch(ty)
    {
        case INTERP_LINEAR: return interp_linear(a, b, t);
        case INTERP_LINEAR_EASEIN: return interp_linear_easein(a, b, t);
        case INTERP_LINEAR_EASEOUT: return interp_linear_easeout(a, b, t);
        case INTERP_LINEAR_EASEINOUT: return interp_linear_easeinout(a, b, t);
    }

    ASSERT(false);
    return a;
}

template<typename T>
InterpolationData<T> *Interpolator<T>::allocData(const Data& dat)
{
    return new InterpolationData<T>(dat);
}

template<typename T>
InterpolationData<T> *Interpolator<T>::allocData()
{
    return new InterpolationData<T>;
}

template<typename T>
void Interpolator<T>::freeData()
{
    delete _data;
    _data = 0;
}

template<typename T>
Interpolator<T>::Interpolator()
: _data(0)
{
}

template<typename T>
Interpolator<T>::Interpolator(const T &val)
: _data(0)
{
}

template<typename T>
Interpolator<T>::Interpolator(const Interpolator& o)
: _data(o._data ? new InterpolationData(*o._data) : 0)
{
}

template<typename T>
Interpolator<T>::~Interpolator()
{
    freeData();
}

template<typename T>
Interpolator<T>& Interpolator<T>::operator=(const Interpolator& o)
{
    v = o.v;
    if(o._data)
    {
        if(_data)
            *_data = *o._data;
        else
            _data = allocData(*o._data);
    }
    else
        freeData();
    return *this;
}

template<typename T>
float Interpolator<T>::interpolateTo(const T& to, float t, int loop, bool pingpong, InterpolationType ty)
{
    if(t == 0)
    {
        freeData();
        v = to;
        return 0;
    }
    if (t < 0)
        t = length(to - v) / -t;

    _interpTo(ensureData(), to, t, loop, pingpong ? INTERP_PINGPONG : INTERP_NOFLAGS, ty);

    return t;
}

template<typename T>
void Interpolator<T>::_interpTo(Data *data, const T& to, float t, int loop, InterpolationFlags flags, InterpolationType ty)
{
    data->type = ty;
    data->timePassed = 0;
    data->timePeriod = t;
    data->to = to;
    data->from = (T)*this;
    data->loop = loop;
    data->flags = flags;
}

template<typename T>
void Interpolator<T>::_interpUpdate(float dt)
{
    Data *data = this->_data;
    const float passed = data->timePassed + dt;
    data->timePassed = passed;
    const float period = data->timePeriod;

    if(passed < period)
        v = interp(data->type, data->from, data->to, passed / period);
    else if (data->loop)
    {
        data->timePassed = 0;
        if (data->loop > 0)
            --data->loop;

        if (data->flags & INTERP_PINGPONG)
        {
            v = data->to;
            std::swap(data->to, data->from);
        }
        else
            v = data->from;
    }
    else
    {
        v = data->to;
        stop();
    }
}
