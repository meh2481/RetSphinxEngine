#pragma once

#include "Box2D/Box2D.h"
#include <vector>

//-----------------------------------------------------
// Callback helper class for finding objects
//-----------------------------------------------------
class PointQueryCallback : public b2QueryCallback
{
public:
    std::vector<b2Body*> foundBodies;

    bool ReportFixture(b2Fixture* fixture)
    {
        foundBodies.push_back(fixture->GetBody());
        return true;
    }
};
