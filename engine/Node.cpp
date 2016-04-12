#include "Node.h"

Node::Node()
{
	 lua = NULL;
}

Node::~Node()
{
	
}

//Node creation
void Node::init()
{
	if(lua)
	{
		//TODO: Call Lua
	}
}

//Update the node
void Node::update(float32 dt)
{
	if(lua)
	{
		//TODO: Call Lua
	}
}

//Collided with an object
void Node::collided(obj* o)
{
	if(lua)
	{
		//TODO: Call Lua
	}
}