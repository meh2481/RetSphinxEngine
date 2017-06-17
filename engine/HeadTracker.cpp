#include "HeadTracker.h"
#include "easylogging++.h"

#define AXIS_MIN -32768.0
#define AXIS_MAX 32767.0

HeadTracker::HeadTracker(SDL_Joystick* joy)
{
	LOG(INFO) << "Initializing headtracker from joystick " << SDL_JoystickName(joy);
	m_joy = joy;
}

HeadTracker::~HeadTracker()
{
	SDL_JoystickClose(m_joy);
}

float HeadTracker::getX()
{
	return scale(SDL_JoystickGetAxis(m_joy, 0));
}

float HeadTracker::getY()
{
	return scale(SDL_JoystickGetAxis(m_joy, 1));
}

float HeadTracker::getZ()
{
	return scale(SDL_JoystickGetAxis(m_joy, 2));
}

float HeadTracker::scale(Sint16 val)
{
	float actual = val;
	if(actual < 0)
		return actual / AXIS_MIN;
	return actual / AXIS_MAX;
}

