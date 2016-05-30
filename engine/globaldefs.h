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

//Defined by SDL
#define JOY_AXIS_MIN	-32768
#define JOY_AXIS_MAX	32767
//Keybinding stuff!
extern uint32_t JOY_BUTTON_BACK;
extern uint32_t JOY_BUTTON_START;
extern uint32_t JOY_BUTTON_X;
extern uint32_t JOY_BUTTON_Y;
extern uint32_t JOY_BUTTON_A;
extern uint32_t JOY_BUTTON_B;
extern uint32_t JOY_BUTTON_LB;
extern uint32_t JOY_BUTTON_RB;
extern uint32_t JOY_BUTTON_LSTICK;
extern uint32_t JOY_BUTTON_RSTICK;
extern uint32_t JOY_AXIS_HORIZ;
extern uint32_t JOY_AXIS_VERT;
extern uint32_t JOY_AXIS2_HORIZ;
extern uint32_t JOY_AXIS2_VERT;
extern uint32_t JOY_AXIS_LT;
extern uint32_t JOY_AXIS_RT;
extern int32_t JOY_AXIS_TRIP;
extern SDL_Scancode KEY_UP1;
extern SDL_Scancode KEY_UP2;
extern SDL_Scancode KEY_DOWN1;
extern SDL_Scancode KEY_DOWN2;
extern SDL_Scancode KEY_LEFT1;
extern SDL_Scancode KEY_LEFT2;
extern SDL_Scancode KEY_RIGHT1;
extern SDL_Scancode KEY_RIGHT2;
extern SDL_Scancode KEY_ENTER1;
extern SDL_Scancode KEY_ENTER2;

typedef glm::vec2 Point;
typedef glm::vec3 Vec3;

#define PI 3.1415926535f //Close enough for my calculations
#define DEG2RAD (PI/180.0f)  //Convert degrees to radians
#define RAD2DEG (180.0f/PI)  //Convert radians to degrees
#define DIFF_EPSILON 0.0000001	  //HACK: How much different two vectors must be to register as a difference
#define G 66.7384	//Gravitational constant times one trillion (Becase we aren't exactly on a planetary scale here)

//HACK: SDL scancodes that really should be defined but aren't
#define SDL_SCANCODE_CTRL	(SDL_NUM_SCANCODES)
#define SDL_SCANCODE_SHIFT 	(SDL_NUM_SCANCODES+1)
#define SDL_SCANCODE_ALT	(SDL_NUM_SCANCODES+2)
#define SDL_SCANCODE_GUI	(SDL_NUM_SCANCODES+3)

class Color
{
public:
	float r,g,b,a;
	Color();
	Color(int ir, int ig, int ib) {from256(ir,ig,ib);};
	Color(float fr, float fg, float fb, float fa) {r=fr;g=fg;b=fb;a=fa;};

	void from256(int ir, int ig, int ib, int a = 255);
	void fromHSV(float h, float s, float v, float fa = 1.0f);
	void set(float fr, float fg, float fb, float fa = 1.0) {r=fr;g=fg;b=fb;a=fa;};
	void clear()	{r=g=b=a=1.0f;};
};

class Rect {
public:
	Rect() {left=right=top=bottom=0;};
	Rect(float l,float t,float r,float b) {left=l;right=r;top=t;bottom=b;};
	float left,top,right,bottom;
	float width() {return right-left;};
	float height() {return bottom-top;};
	float area()  {return width()*height();};
	void	offset(float x, float y)	{left+=x;right+=x;top+=y;bottom+=y;};
	void	offset(Point pt)				{offset(pt.x,pt.y);};
	Point	center() {Point pt; pt.x = (right-left)*0.5f + right; pt.y = (bottom-top)*0.5f + top; return pt;};
	void	center(float* x, float* y)	{Point pt = center(); if(x!=NULL)*x = pt.x; if(y!=NULL)*y = pt.y;};
	void	scale(float fScale) {left*=fScale;right*=fScale;top*=fScale;bottom*=fScale;};
	void	scale(float fScalex, float fScaley) {left*=fScalex;right*=fScalex;top*=fScaley;bottom*=fScaley;};
	void	set(float fleft,float ftop,float fright,float fbottom)  {left=fleft;top=ftop;right=fright;bottom=fbottom;};
	bool	inside(Point p)	{return(p.x >= left && p.x <= right && p.y <= top && p.y >= bottom);};
	void	centerOn(Point p);
};

//Global Helper functions
//TODO: Make into class or package or something
//TODO: Some of these might not even be used. Check and see.
Vec3 crossProduct(Vec3 vec1, Vec3 vec2);	//Cross product of two vectors
float dotProduct(Vec3 vec1, Vec3 vec2);   //Dot product of two vectors
Vec3 rotateAroundVector(Vec3 vecToRot, Vec3 rotVec, float fAngleDeg);	//Rotate one vector around another
Point rotateAroundPoint(Point vecToRot, float fAngleDeg, Point ptRot = Point(0,0)); //Rotate point around given point (Default: origin)
string stripCommas(string s);	   //Strip all the commas from s, leaving spaces in their place
Rect rectFromString(string s);	  //Get a rectangle from comma-separated values in a string
string rectToString(Rect r);
Point pointFromString(string s);	//Get a point from comma-separated values in a string
string pointToString(Point p);
Color colorFromString(string s);	//Get a color from comma-separated values in a string
string colorToString(Color c);
Vec3 vec3FromString(string s);		//Get a 3D point from comma-separated values in a string
string vec3ToString(Vec3 vec);


void randSeed(unsigned long s);						//Seed the random number generator
float distanceSquared(Vec3 vec1, Vec3 vec2);		//Get the distance between two vectors squared
float distanceBetween(Vec3 vec1, Vec3 vec2);				//Get the distance between two vectors (slower than above)
Color HsvToRgb(int h, int s, int v);		//Convert HSV values to RGB
float wrapAngle(float fAngle);
float getAngle(const Point& p);	//Get angle of a vector

float sumOcatave(int num_iterations, float x, float y, float persistence, float scalex, float scaley, float low, float high, float freqinc);

void fillRect(Point p1, Point p2, Color col);
void fillScreen(Color col);

