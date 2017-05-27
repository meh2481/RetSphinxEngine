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
	std::vector<Interpolation*>::iterator i = interpolations.begin();
	while(i != interpolations.end())
	{
		if((*i)->update(dt))
		{
			//Delete & remove if done updating
			delete *i;
			i = interpolations.erase(i);
		}
		else
			i++;
	}
}

void InterpolationManager::interpolate(float * val, float end, float time, InterpType type)
{
	switch(type)
	{
		case LINEAR:
			interpolations.push_back(new LinearInterpolation(val, end, time));
			break;

		case BEZIER:
			interpolations.push_back(new BezierInterpolation(val, end, time));
			break;
	}
}

void InterpolationManager::clear()
{
	for(std::vector<Interpolation*>::iterator i = interpolations.begin(); i != interpolations.end(); i++)
		delete *i;
	interpolations.clear();
}
