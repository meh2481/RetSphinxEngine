/*
    GameEngine header - Object.h
    Copyright (c) 2014 Mark Hutcheson
*/

#pragma once

#include "globaldefs.h"
#include "Mesh3D.h"
#include "Drawable.h"
#include "LuaInterface.h"
#include "luafuncs.h"
#include "tinyxml2.h"

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
	
	Point pos;		//Offset (after rotation)
	Point center;	//Center of rotation (Offset before rotation)
	Point shear;	//Shear for drawing the image
	Point tile;		//tile image in x and y
	float rot;
	Point size;	//Actual texel size; not pixels

    ObjSegment();
    ~ObjSegment();
	
	void draw(bool bDebugInfo = false);
	void update(float dt);
	void fromXML(tinyxml2::XMLElement* layer);
};

//Collections of the above all stuffed into one object for ease of use.
class Object : public Drawable
{
	LuaObjGlue* glueObj;
	map<string, string> propertyValues;
public:
	enum { TYPE = OT_OBJECT };
    vector<ObjSegment*> 	segments;
	Lattice*				meshLattice;
	LatticeAnim*			meshAnim;
	Point					meshSize;
	LuaInterface* 			lua;
	string 					luaClass;
    
    Object();
    ~Object();

    void draw(bool bDebugInfo = false);
    void addSegment(ObjSegment* seg);
	void update(float dt);
	b2Body* getBody();
	Point getPos();
	void collide(Object* other);
	void collideWall(Point ptNormal);	//ptNormal will be a normal vector from the wall to this object
	void initLua();	
	void setPosition(Point p);	//Best to call this not on object creation, but only when needed (makes box2d unhappy if done too much)
	
	void setProperty(string prop, string value)	{propertyValues[prop] = value;};
	void addProperty(string prop, string value) {setProperty(prop, value);};
	string getProperty(string prop)				{if(propertyValues.count(prop)) return propertyValues[prop]; return "";};
	
	void setImage(Image* img, unsigned int seg = 0);	//Sets the image of the given physSegment
};



