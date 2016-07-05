/*
 GameEngine source - globaldefs.cpp
 Copyright (c) 2014 Mark Hutcheson
*/

#include "globaldefs.h"
#include "simplexnoise1234.h"
#include "opengl-api.h"

string stripCommas(string s)
{
	//Replace all ',' characters with ' '
	for(;;)
	{
		size_t iPos = s.find(',');
		if(iPos == s.npos)
			break;  //Done

		s.replace(iPos, 1, " ");
	}
	return s;
}

void Rect::fromString(string s)
{
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
	if(!(iss >> left >> top >> right >> bottom))
		set(0,0,0,0);
}

Vec2 pointFromString(string s)
{
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
	Vec2 pt;
	if(!(iss >> pt.x >> pt.y))
		pt = Vec2(0,0);
	return pt;
}

string pointToString(Vec2 p)
{
	ostringstream oss;
	oss << p.x << "," << p.y;
	return oss.str();
}

void Rect::centerOn(Vec2 p)
{
	Vec2 cen = center();
	offset(p.x - cen.x, p.y - cen.y);
}
