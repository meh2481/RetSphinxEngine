/*
 GameEngine source - globaldefs.cpp
 Copyright (c) 2014 Mark Hutcheson
*/

#include "globaldefs.h"
#include "simplexnoise1234.h"
#include "opengl-api.h"

Color::Color()
{
	clear();
}

void Color::from256(int ir, int ig, int ib, int ia)
{
	r = (float)ir/255.0;
	g = (float)ig/255.0;
	b = (float)ib/255.0;
	a = (float)ia/255.0;
}

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

Rect rectFromString(string s)
{
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
	Rect rc;
	if(!(iss >> rc.left >> rc.top >> rc.right >> rc.bottom))
		rc.set(0,0,0,0);
	return rc;
}

Point pointFromString(string s)
{
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
	Point pt;
	if(!(iss >> pt.x >> pt.y))
		pt = Point(0,0);
	return pt;
}

string pointToString(Point p)
{
	ostringstream oss;
	oss << p.x << "," << p.y;
	return oss.str();
}

void Color::fromString(string s)
{	
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
	int r, g, b, a;
	if(iss >> r >> g >> b)
	{
		if(!(iss >> a))
			from256(r,g,b);
		else
			from256(r,g,b,a);
	}
}

void Rect::centerOn(Point p)
{
	Point cen = center();
	offset(p.x - cen.x, p.y - cen.y);
}
