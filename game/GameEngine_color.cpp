/*
    GameEngine source - color.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include "tinyxml2.h"
#include "Color.h"
#include <float.h>
#include <sstream>

void GameEngine::updateColors(float dt)
{
    //Update our colors that are phasing into one another
    for(std::vector<ColorPhase>::iterator i = m_ColorsChanging.begin(); i != m_ColorsChanging.end(); i++)
    {
        if(i->dir)
        {
            if(abs(i->colorToChange->r - i->destr) >= abs(i->amtr*dt))
                i->colorToChange->r += i->amtr * dt;
            if(abs(i->colorToChange->g - i->destg) >= abs(i->amtg*dt))
                i->colorToChange->g += i->amtg * dt;
            if(abs(i->colorToChange->b - i->destb) >= abs(i->amtb*dt))
                i->colorToChange->b += i->amtb * dt;
            if(abs(i->colorToChange->r - i->destr) <= abs(i->amtr*dt) &&
               abs(i->colorToChange->g - i->destg) <= abs(i->amtg*dt) &&
               abs(i->colorToChange->b - i->destb) <= abs(i->amtb*dt))
            {
                i->colorToChange->r = i->destr;
                i->colorToChange->g = i->destg;
                i->colorToChange->b = i->destb;
                if(!i->pingpong)
                {
                    i = m_ColorsChanging.erase(i);
                    if(i != m_ColorsChanging.begin())
                        i--;
                }
                else
                    i->dir = false;
            }
        }
        else
        {
            if(abs(i->colorToChange->r - i->srcr) >= abs(i->amtr*dt))
                i->colorToChange->r -= i->amtr * dt;
            if(abs(i->colorToChange->g - i->srcg) >= abs(i->amtg*dt))
                i->colorToChange->g -= i->amtg * dt;
            if(abs(i->colorToChange->b - i->srcb) >= abs(i->amtb*dt))
                i->colorToChange->b -= i->amtb * dt;
            if(abs(i->colorToChange->r - i->srcr) <= abs(i->amtr*dt) &&
               abs(i->colorToChange->g - i->srcg) <= abs(i->amtg*dt) &&
               abs(i->colorToChange->b - i->srcb) <= abs(i->amtb*dt))
            {
                i->colorToChange->r = i->srcr;
                i->colorToChange->g = i->srcg;
                i->colorToChange->b = i->srcb;
                if(!i->pingpong)
                {
                    i = m_ColorsChanging.erase(i);
                    if(i != m_ColorsChanging.begin())
                        i--;
                }
                else
                    i->dir = true;
            }
        }
    }
}
    
void GameEngine::phaseColor(Color* src, Color* dest, float time, bool bPingPong)
{
    ColorPhase cp;
    cp.pingpong = bPingPong;
    cp.srcr = src->r;
    cp.srcg = src->g;
    cp.srcb = src->b;
    cp.dir = true;
    cp.colorToChange = src;
    cp.destr = dest->r;
    cp.destg = dest->g;
    cp.destb = dest->b;
    cp.amtr = (dest->r - src->r) / time;
    cp.amtg = (dest->g - src->g) / time;
    cp.amtb = (dest->b - src->b) / time;
    bool bSet = false;
    for(std::vector<ColorPhase>::iterator i = m_ColorsChanging.begin(); i != m_ColorsChanging.end(); i++)
    {
        if(i->colorToChange == src)
        {
            bSet = true;
            *i = cp;
            break;
        }
    }
    if(!bSet)
        m_ColorsChanging.push_back(cp);
}

void GameEngine::clearColors()
{
    for(std::vector<ColorPhase>::iterator i = m_ColorsChanging.begin(); i != m_ColorsChanging.end(); i++)
    {
        i->colorToChange->r = i->srcr;
        i->colorToChange->g = i->srcg;
        i->colorToChange->b = i->srcb;
    }
    m_ColorsChanging.clear();
}






