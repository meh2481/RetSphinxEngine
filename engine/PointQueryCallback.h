#pragma once

#include "Box2D/Box2D.h"
#include <list>

//-----------------------------------------------------
// Callback helper class for finding objects
//-----------------------------------------------------
class PointQueryCallback : public b2QueryCallback
{
public:
	std::list<b2Body*> foundBodies;

	bool ReportFixture(b2Fixture* fixture)
	{
		foundBodies.push_back(fixture->GetBody());
		return true;
	}
};