/*
    GameEngine header - Object.h
    Copyright (c) 2014 Mark Hutcheson
*/

#pragma once

#include "Object3D.h"
#include "LuaInterface.h"
#include "LuaFuncs.h"
#include "Rect.h"
#include "Color.h"
#include <vector>
#include <string>
#include <map>

class Object;
class ObjSegment;
class b2Body;
class Image;
struct b2BodyDef;

//Physical segments of objects - be they actual physics bodies or just images
//TODO Make SceneryLayer class that this can contain
class ObjSegment
{
public:
	enum { TYPE = OT_SEGMENT };

    b2Body*         body;		//Physics body associated with this segment
	Object* 		parent;		//Parent object
	Object3D*		obj3D;		//3D object
	
	Vec2 pos;		//Offset (after rotation)
	Vec2 tile;		//tile image in x and y
	float rot;
	Vec2 size;	//Actual texel size; not pixels
	float depth;
	Image* img;
	bool active;
	Color col;

    ObjSegment();
    ~ObjSegment();
	
	void draw(glm::mat4 mat);
};

//Collections of the above all stuffed into one object for ease of use.
class Object
{
	LuaObjGlue* glueObj;
	std::map<std::string, std::string> propertyValues;
	std::vector<ObjSegment*> 	segments;

public:
	enum { TYPE = OT_OBJECT };

	Vec2 meshSize;
	LuaInterface* lua;
	std::string luaClass;
	float depth;
	Image* img;
	bool active;
	bool alive;
	Color col;
    
    Object();
    ~Object();

    void draw(glm::mat4 mat);
    void addSegment(ObjSegment* seg);
	ObjSegment* getSegment(unsigned int idx);
	void update(float dt);
	b2Body* getBody();
	Vec2 getPos();
	void collide(Object* other, float impulse);
	void collideWall(Vec2 ptNormal, float impulse);	//ptNormal will be a normal vector from the wall to this object
	void initLua();	
	void setPosition(Vec2 p);	//Best to call this not on object creation, but only when needed (makes box2d unhappy if done too much)
	
	void setProperty(const std::string& prop, const std::string& value)	{propertyValues[prop] = value;};
	void addProperty(const std::string& prop, const std::string& value) {setProperty(prop, value);};
	std::string getProperty(const std::string& prop)				{if(propertyValues.count(prop)) return propertyValues[prop]; return "";};
	
	void setImage(Image* img, unsigned int seg = 0);	//Sets the image of the given physSegment
};



