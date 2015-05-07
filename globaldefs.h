/*
	Pony48 source - globaldefs.h
	Global definitions for data types
	Copyright (c) 2014 Mark Hutcheson
*/
#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#include <string>
#include <fstream>
#include <sstream>
using namespace std;
#include "Box2D.h"
#include "tinyxml2.h"
using namespace tinyxml2;
#include "include/VFS.h"
#include "include/VFSTools.h"
#ifdef USE_SDL_FRAMEWORK
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif
#include "FreeImage.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

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
#define JOY_MINMOVE_TRIP	3000
#define MOUSE_MOVE_TRIP_AMT	20


#define Point b2Vec2	//Our point structure
/*class Point : public b2Vec2
{
	//TODO: copy operators and such
public:
	Point(float32 x, float32 y) : b2Vec2(x,y) {};
	Point() {x=y=0;};
};*/
#define Vec2 Point
#define PI 3.1415926535 //Close enough for my calculations
#define DEG2RAD PI/180.0f  //Convert degrees to radians
#define RAD2DEG 180.0f/PI  //Convert radians to degrees
#define DIFF_EPSILON 0.0000001	  //HACK: How much different two vectors must be to register as a difference
#define G 66.7384	//Gravitational constant times one trillion (Becase we aren't exactly on a planetary scale here)

//HACK: SDL scancodes that really should be defined but aren't
#define SDL_SCANCODE_CTRL	SDL_NUM_SCANCODES
#define SDL_SCANCODE_SHIFT 	SDL_NUM_SCANCODES+1
#define SDL_SCANCODE_ALT	SDL_NUM_SCANCODES+2
#define SDL_SCANCODE_GUI	SDL_NUM_SCANCODES+3

class Color
{
public:
	float32 r,g,b,a;
	Color();
	Color(int32_t ir, int32_t ig, int32_t ib) {from256(ir,ig,ib);};
	Color(float32 fr, float32 fg, float32 fb, float32 fa) {r=fr;g=fg;b=fb;a=fa;};

	void from256(int32_t ir, int32_t ig, int32_t ib, int32_t a = 255);
	void fromHSV(float h, float s, float v, float fa = 1.0f);
	void set(float32 fr, float32 fg, float32 fb, float32 fa = 1.0) {r=fr;g=fg;b=fb;a=fa;};
	void clear()	{r=g=b=a=1.0f;};
};

class Rect {
public:
	Rect() {left=right=top=bottom=0;};
	Rect(float32 l,float32 t,float32 r,float32 b) {left=l;right=r;top=t;bottom=b;};
	float32 left,top,right,bottom;
	float32 width() {return right-left;};
	float32 height() {return bottom-top;};
	float32 area()  {return width()*height();};
	void	offset(float32 x, float32 y)	{left+=x;right+=x;top+=y;bottom+=y;};
	void	offset(Point pt)				{offset(pt.x,pt.y);};
	Point	center() {Point pt; pt.x = (right-left)/2.0 + right; pt.y = (bottom-top)/2.0 + top; return pt;};
	void	center(float32* x, float32* y)	{Point pt = center(); if(x!=NULL)*x = pt.x; if(y!=NULL)*y = pt.y;};
	void	scale(float32 fScale) {left*=fScale;right*=fScale;top*=fScale;bottom*=fScale;};
	void	scale(float32 fScalex, float32 fScaley) {left*=fScalex;right*=fScalex;top*=fScaley;bottom*=fScaley;};
	void	set(float32 fleft,float32 ftop,float32 fright,float32 fbottom)  {left=fleft;top=ftop;right=fright;bottom=fbottom;};
	bool	inside(Point p)	{return(p.x >= left && p.x <= right && p.y <= top && p.y >= bottom);};
	void	centerOn(Point p);
};

class Vec3
{
public:
	float32 x, y, z;
	
	//Constructor
	Vec3();

	void set(float32 fx, float32 fy, float32 fz)	{x=fx;y=fy;z=fz;};
	void setZero()  {set(0,0,0);};

	//Helpful math functions
	void normalize();
	Vec3 normalized();  //Doesn't modify original vector, returns normalized version

	//Operators for easy use
	bool operator!=(const Vec3& v);
	
};

extern ofstream errlog;

//Helper functions
Vec3 crossProduct(Vec3 vec1, Vec3 vec2);	//Cross product of two vectors
float32 dotProduct(Vec3 vec1, Vec3 vec2);   //Dot product of two vectors
Vec3 rotateAroundVector(Vec3 vecToRot, Vec3 rotVec, float32 fAngleDeg);	//Rotate one vector around another
Point rotateAroundPoint(Point vecToRot, float32 fAngleDeg, Point ptRot = Point(0,0)); //Rotate point around given point (Default: origin)
string stripCommas(string s);	   //Strip all the commas from s, leaving spaces in their place
Rect rectFromString(string s);	  //Get a rectangle from comma-separated values in a string
string rectToString(Rect r);
Point pointFromString(string s);	//Get a point from comma-separated values in a string
string pointToString(Point p);
Color colorFromString(string s);	//Get a color from comma-separated values in a string
string colorToString(Color c);
Vec3 vec3FromString(string s);		//Get a 3D point from comma-separated values in a string
string vec3ToString(Vec3 vec);
int32_t randInt(int32_t min, int32_t max);  		//Get a random integer
int32_t randInt();									//Get an unbounded random integer
float32 randFloat(float32 min, float32 max);		//Get a random float32
float32 randFloat();								//Get an unbounded random float
void randSeed(unsigned long s);						//Seed the random number generator
float32 distanceSquared(Vec3 vec1, Vec3 vec2);		//Get the distance between two vectors squared
float32 distanceBetween(Vec3 vec1, Vec3 vec2);				//Get the distance between two vectors (slower than above)
Color HsvToRgb(int h, int s, int v);		//Convert HSV values to RGB
float32 wrapAngle(float32 fAngle);

float sumOcatave(int num_iterations, float x, float y, float persistence, float scalex, float scaley, float low, float high, float freqinc);

#endif
