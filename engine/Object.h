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
#include "RenderState.h"
#include <vector>
#include <string>
#include <map>

class ObjSegment;
class b2Body;
class Image;

//Collections of the above all stuffed into one object for ease of use.
class Object
{
    LuaObjGlue* glueObj;
    std::map<std::string, std::string> propertyValues;
    std::vector<ObjSegment*>     segments;

public:
    enum { TYPE = OT_OBJECT };

    Vec2 meshSize;
    LuaInterface* lua;
    std::string luaClass;
    std::string luaDef;
    float depth;
    Image* img;
    bool active;
    bool alive;
    Color col;

    Object();
    ~Object();

    void draw(RenderState renderState);
    void addSegment(ObjSegment* seg);
    ObjSegment* getSegment(unsigned int idx);
    void update(float dt);
    b2Body* getBody();
    Vec2 getPos();
    std::string getLuaClass() {return luaClass;}
    void collide(Object* other, float impulse);
    void collideWall(Vec2 ptNormal, float impulse);    //ptNormal will be a normal vector from the wall to this object
    void initLua();
    void setPosition(Vec2 p);    //Best to call this not on object creation, but only when needed (makes box2d unhappy if done too much)

    void setProperty(const std::string& prop, const std::string& value)    {propertyValues[prop] = value;};
    std::string getProperty(const std::string& prop)                {if(propertyValues.count(prop)) return propertyValues[prop]; return "";};

    void setImage(Image* img, unsigned int seg = 0);    //Sets the image of the given physSegment
};



