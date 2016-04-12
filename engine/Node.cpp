#include "Node.h"
#include "lua.hpp"

Node::Node()
{
	 lua = NULL;
}

Node::~Node()
{
	if(lua)
	{
		//TODO: Call Lua destroy()
	}	
}

//Node creation
void Node::init()
{
	if(lua && luaFile.length())
	{
		lua_State* L = lua->getState();
		
		//Parse this lua file first
		//luaL_loadfile(L, luaFile.c_str());
		
		
		//TODO: Call Lua init()
	}
}

//Update the node
void Node::update(float32 dt)
{
	if(lua)
	{
		//TODO: Call Lua update(dt)
	}
}

//Collided with an object
void Node::collided(obj* o)
{
	if(lua)
	{
		//TODO: Call Lua collide(obj)
	}
}