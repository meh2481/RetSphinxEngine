#include "InterpolationManager.h"
#include "Interpolation.h"
#include "LinearInterpolation.h"
#include "BezierInterpolation.h"

InterpolationManager::InterpolationManager()
{
}

InterpolationManager::~InterpolationManager()
{
    clear();
}

void InterpolationManager::update(float dt)
{
    std::vector<InterpHolder>::iterator i = interpolations.begin();
    while(i != interpolations.end())
    {
        if(i->interp->update(dt))    //Interpolation is done
        {
            if(i->count == REPEAT_FOREVER || i->count-- > 0)    //Repeat
            {
                if(i->repeat == REPEAT)    //Start over
                {
                    *i->interp->val = i->interp->start;
                    i->interp->curTime = 0.0f;
                }
                else if(i->repeat == PINGPONG)    //go back and forth
                {
                    //Swap start and dest
                    float tmp = i->interp->dest;
                    i->interp->dest = i->interp->start;
                    i->interp->start = tmp;
                    i->interp->curTime = 0.0f;
                }
                i++;    //Increment i here also
            }
            else
            {
                //Delete & remove if done updating
                delete i->interp;
                i = interpolations.erase(i);
            }
        }
        else
            i++;
    }
}

void InterpolationManager::interpolate(float * val, float end, float time, InterpType type, int count, InterpRepeat repeat)
{
    Interpolation* interp = NULL;
    switch(type)
    {
        case LINEAR:
            interp = new LinearInterpolation(val, end, time);
            break;

        case BEZIER:
            interp = new BezierInterpolation(val, end, time);
            break;
    }

    if(interp != NULL)
    {
        InterpHolder holder;
        holder.count = count;
        holder.interp = interp;
        holder.repeat = repeat;
        interpolations.push_back(holder);
    }
}

bool InterpolationManager::contains(float * val)
{
    for(std::vector<InterpHolder>::iterator i = interpolations.begin(); i != interpolations.end(); i++)
    {
        if(i->interp->val == val)
            return true;
    }
    return false;
}

void InterpolationManager::clear()
{
    for(std::vector<InterpHolder>::iterator i = interpolations.begin(); i != interpolations.end(); i++)
        delete i->interp;
    interpolations.clear();
}
