/*
    GameEngine header - Object.h
    Copyright (c) 2014 Mark Hutcheson
*/

#ifndef OBJECT_H
#define OBJECT_H

#include "globaldefs.h"
#include "3DObject.h"
#include "Drawable.h"
#include "LuaInterface.h"
#include "luafuncs.h"
#include "tinyxml2.h"

class obj;
class physSegment;
class b2Body;
struct b2BodyDef;
class lattice;
class latticeAnim;
class GLImage;

//Physical segments of objects - be they actual physics bodies or just images
//TODO: Why would they just be images unless they're scenery? Make second class for scenery?
class physSegment : public Drawable
{
public:
    b2Body*         body;		//Physics body associated with this segment
    GLImage*  		img;		//Image to draw
	lattice*		lat;		//Lattice to apply to image
	latticeAnim*	latanim;	//Animation to apply to lattice
	obj* 			parent;		//Parent object
	Object3D*		obj3D;		//3D object
	
	Point pos;		//Offset (after rotation)
	Point center;	//Center of rotation (Offset before rotation)
	Point shear;	//Shear for drawing the image
	Point tile;		//tile image in x and y
	float rot;
	Point size;	//Actual texel size; not pixels
	Color col;
	bool show;

    physSegment();
    ~physSegment();
	
	void draw(bool bDebugInfo = false);
	void update(float dt);
	void fromXML(tinyxml2::XMLElement* layer);
};

//Collections of the above all stuffed into one object for ease of use.
//TODO: Rename from 'obj' to something more intelligent
class obj : public Drawable
{
	LuaObjGlue* glueObj;
	map<string, string> propertyValues;
public:
	enum { TYPE = OT_OBJECT };
    vector<physSegment*> 	segments;
    b2Body*         		body; //TODO: is this ever even used?
	GLImage*					meshImg;
	lattice*				meshLattice;
	latticeAnim*			meshAnim;
	Point					meshSize;
    void* 					usr;
	LuaInterface* 			lua;
	string 					luaClass;
	bool 					active;
    
    obj();
    ~obj();

    void draw(bool bDebugInfo = false);
    void addSegment(physSegment* seg);
	b2BodyDef* update(float dt);
	b2Body* getBody();
	Point getPos();	//TODO: Find where we're using body->getPos and replace with this func
	void collide(obj* other);
	void collideWall(Point ptNormal);	//ptNormal will be a normal vector from the wall to this object
	void initLua();	
	void setPosition(Point p);	//Best to call this not on object creation, but only when needed (makes box2d unhappy if done too much)
	
	void setProperty(string prop, string value)	{propertyValues[prop] = value;};
	void addProperty(string prop, string value) {setProperty(prop, value);};
	string getProperty(string prop)				{if(propertyValues.count(prop)) return propertyValues[prop]; return "";};
	
	void setImage(GLImage* img, int seg = 0);	//Sets the image of the given physSegment
};



#endif
