/*
    GameEngine source - color.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include "tinyxml2.h"
#include <float.h>
#include <sstream>

float myAbs(float v)
{
	if(v < 0.0)
		return -v;
	return v;
}

void Pony48Engine::updateColors(float32 dt)
{
	//Update our colors that are phasing into one another
	for(list<ColorPhase>::iterator i = m_ColorsChanging.begin(); i != m_ColorsChanging.end(); i++)
	{
		if(i->dir)
		{
			if(myAbs(i->colorToChange->r - i->destr) >= myAbs(i->amtr*dt))
				i->colorToChange->r += i->amtr * dt;
			if(myAbs(i->colorToChange->g - i->destg) >= myAbs(i->amtg*dt))
				i->colorToChange->g += i->amtg * dt;
			if(myAbs(i->colorToChange->b - i->destb) >= myAbs(i->amtb*dt))
				i->colorToChange->b += i->amtb * dt;
			if(myAbs(i->colorToChange->r - i->destr) <= myAbs(i->amtr*dt) &&
			   myAbs(i->colorToChange->g - i->destg) <= myAbs(i->amtg*dt) &&
			   myAbs(i->colorToChange->b - i->destb) <= myAbs(i->amtb*dt))
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
			if(myAbs(i->colorToChange->r - i->srcr) >= myAbs(i->amtr*dt))
				i->colorToChange->r -= i->amtr * dt;
			if(myAbs(i->colorToChange->g - i->srcg) >= myAbs(i->amtg*dt))
				i->colorToChange->g -= i->amtg * dt;
			if(myAbs(i->colorToChange->b - i->srcb) >= myAbs(i->amtb*dt))
				i->colorToChange->b -= i->amtb * dt;
			if(myAbs(i->colorToChange->r - i->srcr) <= myAbs(i->amtr*dt) &&
			   myAbs(i->colorToChange->g - i->srcg) <= myAbs(i->amtg*dt) &&
			   myAbs(i->colorToChange->b - i->srcb) <= myAbs(i->amtb*dt))
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
	
void Pony48Engine::phaseColor(Color* src, Color dest, float time, bool bPingPong)
{
	ColorPhase cp;
	cp.pingpong = bPingPong;
	cp.srcr = src->r;
	cp.srcg = src->g;
	cp.srcb = src->b;
	cp.dir = true;
	cp.colorToChange = src;
	cp.destr = dest.r;
	cp.destg = dest.g;
	cp.destb = dest.b;
	cp.amtr = (dest.r - src->r) / time;
	cp.amtg = (dest.g - src->g) / time;
	cp.amtb = (dest.b - src->b) / time;
	bool bSet = false;
	for(list<ColorPhase>::iterator i = m_ColorsChanging.begin(); i != m_ColorsChanging.end(); i++)
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

void Pony48Engine::clearColors()
{
	for(list<ColorPhase>::iterator i = m_ColorsChanging.begin(); i != m_ColorsChanging.end(); i++)
	{
		i->colorToChange->r = i->srcr;
		i->colorToChange->g = i->srcg;
		i->colorToChange->b = i->srcb;
	}
	m_ColorsChanging.clear();
}






