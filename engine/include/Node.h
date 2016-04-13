#ifndef NODE_H
#define NODE_H

#include "Object.h"

class Node {
	LuaObjGlue* glueObj;
public:
	enum { TYPE = OT_NODE };
	string luaClass;
	LuaInterface* lua;
	map<string, string> propertyValues;	//This can be populated by XML and called from Lua! For userdata and such
	Point pos;
	string name;
	
	Node();
	~Node();
	
	void update(float32 dt);	//Update the node
	void collided(obj* o);		//Collided with an object
	void init();				//Create stuff in lua for this object
};


#endif	//defined NODE_H