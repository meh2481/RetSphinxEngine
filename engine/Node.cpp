#include "Node.h"

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
	if(lua)
	{
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