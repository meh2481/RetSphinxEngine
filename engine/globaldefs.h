/*
	GameEngine header - globaldefs.h
	Global definitions for data types
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <set>
using namespace std;
#include "glmx.h"
#include <SDL.h>

//-------------------------------
// Math stuff
//-------------------------------

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;

class Rect {
public:
	Rect() {left=right=top=bottom=0;};
	Rect(float l,float t,float r,float b) {left=l;right=r;top=t;bottom=b;};
	float left,top,right,bottom;
	float width() {return right-left;};
	float height() {return bottom-top;};
	float area()  {return width()*height();};
	void	offset(float x, float y)	{left+=x;right+=x;top+=y;bottom+=y;};
	void	offset(Vec2 pt)				{offset(pt.x,pt.y);};
	Vec2	center() {Vec2 pt; pt.x = (right-left)*0.5f + right; pt.y = (bottom-top)*0.5f + top; return pt;};
	void	center(float* x, float* y)	{Vec2 pt = center(); if(x!=NULL)*x = pt.x; if(y!=NULL)*y = pt.y;};
	void	scale(float fScale) {left*=fScale;right*=fScale;top*=fScale;bottom*=fScale;};
	void	scale(float fScalex, float fScaley) {left*=fScalex;right*=fScalex;top*=fScaley;bottom*=fScaley;};
	void	set(float fleft,float ftop,float fright,float fbottom)  {left=fleft;top=ftop;right=fright;bottom=fbottom;};
	bool	inside(Vec2 p)	{return(p.x >= left && p.x <= right && p.y <= top && p.y >= bottom);};
	void	centerOn(Vec2 p);

	void fromString(string s);	  //Get a rectangle from comma-separated values in a string
};


//--------------------------------------------------
// Parsing functions
//--------------------------------------------------

string stripCommas(string s);	   //Strip all the commas from s, leaving spaces in their place
Vec2 pointFromString(string s);	//Get a point from comma-separated values in a string
string pointToString(Vec2 p);


