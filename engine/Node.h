#pragma once
#include "LuaFuncs.h"
#include "Rect.h"
#include <string>
#include <map>

class Object;
class b2Body;
class LuaObjGlue;
class LuaInterface;

class Node {
    LuaObjGlue*         glueObj;
    std::map<std::string, std::string> propertyValues;    //This can be populated by XML and called from Lua! For userdata and such
public:
    enum { TYPE = OT_NODE };
    std::string     luaClass;
    std::string     luaDef;
    LuaInterface*     lua;
    Vec2             pos;
    std::string     name;
    b2Body*         body;

    Node();
    ~Node();

    void update(float dt);    //Update the node
    void collided(Object* o);        //Collided with an object
    void init();                //Create stuff in lua for this object

    void setProperty(const std::string& prop, const std::string& value)    {propertyValues[prop] = value;};
    void addProperty(const std::string& prop, const std::string& value) {setProperty(prop, value);};
    std::string getProperty(const std::string prop)                {if(propertyValues.count(prop)) return propertyValues[prop]; return "";};
};

