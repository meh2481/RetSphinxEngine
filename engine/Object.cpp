/*
    GameEngine source - Object.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "Object.h"
#include "LuaFuncs.h"
#include "Quad.h"
#include "ObjSegment.h"

#include <Box2D/Box2D.h>
#include "tinyxml2.h"

//----------------------------------------------------------------------------------------------------
// obj class
//----------------------------------------------------------------------------------------------------
Object::Object()
{
  lua = NULL;
  glueObj = NULL;
  luaClass = "templateobj";
  active = true;
  alive = true;
  segments.reserve(1);    //don't expect very many segments
}

Object::~Object()
{
    if(lua)
    {
        //Call Lua destroy()
        lua->callMethod(this, "destroy");

        //Cleanup Lua glue object
        lua->deleteObject(glueObj);
    }
    for(std::vector<ObjSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
        delete (*i);
}

void Object::draw(RenderState renderState)
{
    if(!active)
        return;

    //Draw segments of this object
    for(std::vector<ObjSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
    {
        if((*i)->active)    //Skip frames that shouldn't be drawn up front
            (*i)->draw(renderState);
    }
}

void Object::addSegment(ObjSegment* seg)
{
    if(seg == NULL) return;
    segments.push_back(seg);
    seg->parent = this;
}

ObjSegment* Object::getSegment(unsigned int idx)
{
    if(segments.size() > idx)
        return segments[idx];
    return NULL;
}

void Object::update(float dt)
{
    if(lua)
        lua->callMethod(this, "update", dt);
}

b2Body* Object::getBody()
{
    if(segments.size())
        return (*segments.begin())->body;
    return NULL;
}

void Object::setImage(Image* img, unsigned int seg)
{
    if(segments.size() > seg)
        segments[seg]->img = img;
}

Vec2 Object::getPos()
{
    b2Body* b = getBody();
    b2Vec2 p = b ? b->GetPosition() : b2Vec2(0,0);
    return Vec2(p.x, p.y);
}

void Object::collide(Object* other, float impulse)
{
    if(lua)
        lua->callMethod(this, "collide", other, impulse);
}

void Object::collideWall(Vec2 ptNormal, float impulse)
{
    if(lua)
        lua->callMethod(this, "collidewall", ptNormal.x, ptNormal.y, impulse);
}

void Object::initLua()
{
    if(lua && !glueObj)
    {
        lua->call("loadclass", ("obj" + luaClass).c_str(), luaDef.c_str());    //Create this class if it hasn't been created already

        //Parse this lua object first
        glueObj = lua->createObject(this, TYPE, ("obj" + luaClass).c_str());

        //Call Lua init() function in file defined by luaFile
        lua->callMethod(this, "init");
    }
}

void Object::setPosition(Vec2 p)
{
    b2Body* b = getBody();
    if(b != NULL)
    {
        b2Vec2 ptDiff = b->GetPosition();    //Get relative offset for all bodies
        ptDiff = b2Vec2(p.x, p.y) - ptDiff;
        for(std::vector<ObjSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
        {
            if((*i)->body != NULL)
            {
                b2Vec2 ptNew = (*i)->body->GetPosition() + ptDiff;
                (*i)->body->SetTransform(ptNew, (*i)->body->GetAngle());
                (*i)->body->SetAwake(true);
            }
        }
    }
}



