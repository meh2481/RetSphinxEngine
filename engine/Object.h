/*
    GameEngine header - Object.h
    Copyright (c) 2014 Mark Hutcheson
*/

#pragma once

#include "Mesh3D.h"
#include "Drawable.h"
#include "LuaInterface.h"
#include "luafuncs.h"
#include "Rect.h"
#include <vector>
#include <string>
#include <map>
using namespace std;

class Object;
class ObjSegment;
class b2Body;
struct b2BodyDef;
class Lattice;
class LatticeAnim;
class Image;

//Physical segments of objects - be they actual physics bodies or just images
//TODO: Why would they just be images unless they're scenery? Make second class for scenery?
class ObjSegment : public Drawable
{
public:
    b2Body*         body;		//Physics body associated with this segment
	Lattice*		lat;		//Lattice to apply to image
	LatticeAnim*	latanim;	//Animation to apply to lattice
	Object* 		parent;		//Parent object
	Mesh3D*		obj3D;		//3D object
	
	Vec2 pos;		//Offset (after rotation)
	Vec2 tile;		//tile image in x and y
	float rot;
	Vec2 size;	//Actual texel size; not pixels

    ObjSegment();
    ~ObjSegment();
	
	void draw(bool bDebugInfo = false);
	void update(float dt);
};

//Collections of the above all stuffed into one object for ease of use.
class Object : public Drawable
{
	LuaObjGlue* glueObj;
	map<string, string> propertyValues;
public:
	enum { TYPE = OT_OBJECT };
    vector<ObjSegment*> 	segments;	//TODO Should be private
	Lattice*				meshLattice;
	LatticeAnim*			meshAnim;
	Vec2					meshSize;
	LuaInterface* 			lua;
	string 					luaClass;
    
    Object();
    ~Object();

    void draw(bool bDebugInfo = false);
    void addSegment(ObjSegment* seg);
	void update(float dt);
	b2Body* getBody();
	Vec2 getPos();
	void collide(Object* other);
	void collideWall(Vec2 ptNormal);	//ptNormal will be a normal vector from the wall to this object
	void initLua();	
	void setPosition(Vec2 p);	//Best to call this not on object creation, but only when needed (makes box2d unhappy if done too much)
	
	void setProperty(string prop, string value)	{propertyValues[prop] = value;};
	void addProperty(string prop, string value) {setProperty(prop, value);};
	string getProperty(string prop)				{if(propertyValues.count(prop)) return propertyValues[prop]; return "";};
	
	void setImage(Image* img, unsigned int seg = 0);	//Sets the image of the given physSegment
};



