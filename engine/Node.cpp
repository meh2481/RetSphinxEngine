#include "Node.h"
#include "LuaInterface.h"

Node::Node()
{
    body = NULL;
    lua = NULL;
}

Node::~Node()
{
    if(lua)
        lua->callMethod(this, "destroy");
}

//Node creation
void Node::init()
{
    if(lua && luaClass.length())
    {
        lua_State* L = lua->getState();
        lua->call("loadclass", ("node" + luaClass).c_str(), luaDef.c_str());
        glueObj = lua->createObject(this, TYPE, ("node" + luaClass).c_str());
        lua->callMethod(this, "init");
    }
}

//Update the node
void Node::update(float dt)
{
    if(lua)
        lua->callMethod(this, "update", dt);
}

//Collided with an object
void Node::collided(Object* o)
{
    if(lua)
        lua->callMethod(this, "collide", o);
}
