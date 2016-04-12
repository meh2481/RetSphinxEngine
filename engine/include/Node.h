#ifndef NODE_H
#define NODE_H

#include "Object.h"

class Node {
public:
	string luaFile;
	LuaInterface* lua;
	
	Node();
	~Node();
	
	void update(float32 dt);	//Update the node
	void collided(obj* o);		//Collided with an object
	void init();				//Create stuff in lua for this object
};


#endif	//defined NODE_H