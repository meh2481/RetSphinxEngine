#include "StringUtils.h"
#include "Rect.h"
#include <sstream>
using namespace std;

Rect::Rect() 
{ 
	left = right = top = bottom = 0; 
}

Rect::Rect(float l, float t, float r, float b) 
{ 
	left = l; 
	right = r; 
	top = t; 
	bottom = b; 
}

float Rect::width() 
{ 
	return abs(right - left); 
}

float Rect::height() 
{ 
	return abs(bottom - top); 
}

float Rect::area() 
{ 
	return width()*height();
}

void Rect::offset(float x, float y)
{ 
	left += x; 
	right += x;
	top += y; 
	bottom += y;
}

void Rect::offset(Vec2 pt) 
{ 
	offset(pt.x, pt.y); 
}

Vec2 Rect::center()
{ 
	Vec2 pt;
	pt.x = (right - left)*0.5f + left;
	pt.y = (bottom - top)*0.5f + top; 
	return pt;
}

void Rect::center(float* x, float* y) 
{ 
	Vec2 pt = center(); 
	if(x != NULL)
		*x = pt.x; 
	if(y != NULL)
		*y = pt.y; 
}

void Rect::scale(float fScale)
{ 
	left *= fScale;
	right *= fScale;
	top *= fScale;
	bottom *= fScale;
}

void Rect::scale(float fScalex, float fScaley) 
{ 
	left *= fScalex;
	right *= fScalex; 
	top *= fScaley; 
	bottom *= fScaley;
}

void Rect::set(float fleft, float ftop, float fright, float fbottom) 
{ 
	left = fleft;
	top = ftop; 
	right = fright;
	bottom = fbottom;
}

bool Rect::inside(Vec2 p)
{ 
	return(p.x >= left && p.x <= right && p.y <= top && p.y >= bottom); 
}

void Rect::centerOn(Vec2 p)
{
	Vec2 cen = center();
	offset(p.x - cen.x, p.y - cen.y);
}

void Rect::fromString(const string& input)
{
	std::string s = input;
	s = StringUtils::stripCommas(s);

	//Now, parse
	istringstream iss(s);
	if(!(iss >> left >> top >> right >> bottom))
		set(0, 0, 0, 0);
}

std::string Rect::toString()
{
	ostringstream oss;
	oss << left << ", " << top << ", " << right << ", " << bottom;
	return oss.str();
}

Vec2 pointFromString(const std::string& input)
{
	std::string s = input;
	s = StringUtils::stripCommas(s);

	//Now, parse
	std::istringstream iss(s);
	Vec2 pt;
	if(!(iss >> pt.x >> pt.y))
		pt = Vec2(0, 0);
	return pt;
}

Vec3 vec3FromString(const std::string& input)
{
	std::string s = input;
	s = StringUtils::stripCommas(s);

	//Now, parse
	std::istringstream iss(s);
	Vec3 pt;
	if(!(iss >> pt.x >> pt.y >> pt.z))
		pt = Vec3(0, 0, 0);
	return pt;
}

std::string pointToString(Vec2 pt)
{
	ostringstream oss;
	oss << pt.x << ", " << pt.y;
	return oss.str();
}

std::string vec3ToString(Vec3 vec)
{
	ostringstream oss;
	oss << vec.x << ", " << vec.y << ", " << vec.z;
	return oss.str();
}
