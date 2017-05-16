#pragma once
#include "glmx.h"
#include <string>

//------------------------------------------------------
// Other helpful things
//------------------------------------------------------
typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;

//TODO: move somewhere where it makes sense
Vec2 pointFromString(const std::string& s);
std::string pointToString(Vec2 pt);
Vec3 vec3FromString(const std::string& s);
std::string vec3ToString(Vec3 vec);

//------------------------------------------------------
// Rect class
//------------------------------------------------------
class Rect
{
public:
	float left, top, right, bottom;

	Rect();
	Rect(float l, float t, float r, float b);

	//Helper accessor methods
	float width();
	float height();
	float area();
	Vec2 center();
	void center(float* x, float* y);

	//Transform methods
	void offset(float x, float y);
	void offset(Vec2 pt);
	void scale(float fScale);
	void scale(float fScalex, float fScaley);
	void centerOn(Vec2 p);
	
	//Test method
	bool inside(Vec2 p);

	//Setter methods
	void fromString(const std::string& s);	  //Get a rectangle from comma-separated values in a string
	std::string toString();
	void set(float fleft, float ftop, float fright, float fbottom);
};
