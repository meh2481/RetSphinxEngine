#pragma once
#include "SDL_joystick.h"

class HeadTracker
{
    SDL_Joystick* m_joy;

    HeadTracker() {};
    float scale(Sint16 val);
public:
    HeadTracker(SDL_Joystick* joy);
    ~HeadTracker();

    float getX();    //Get -1..1 range for X
    float getY();    //Get -1..1 range for Y
    float getZ();    //Get -1..1 range for Z
};