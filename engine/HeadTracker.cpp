#include "HeadTracker.h"
#include "Logger.h"

#define AXIS_MAX 32767.0f

HeadTracker::HeadTracker(SDL_Joystick* joy)
{
    LOG_info("Initializing headtracker from joystick %s", SDL_JoystickName(joy));
    m_joy = joy;
}

HeadTracker::~HeadTracker()
{
    SDL_JoystickClose(m_joy);
}

float HeadTracker::getX()    //-x is left
{
    return scale(SDL_JoystickGetAxis(m_joy, 0));
}

float HeadTracker::getY()    //-y is down
{
    return scale(SDL_JoystickGetAxis(m_joy, 1));
}

float HeadTracker::getZ()    //-z is tilt clockwise as facing screen
{
    return scale(SDL_JoystickGetAxis(m_joy, 2));
}

float HeadTracker::scale(Sint16 val)
{
    return (float)(val) / AXIS_MAX;
}

