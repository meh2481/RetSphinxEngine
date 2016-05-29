#pragma once

#include "Object.h"

class Node {
	LuaObjGlue* 		glueObj;
	map<string, string> propertyValues;	//This can be populated by XML and called from Lua! For userdata and such
public:
	enum { TYPE = OT_NODE };
	string 			luaClass;
	LuaInterface* 	lua;
	Point 			pos;
	string 			name;
	
	Node();
	~Node();
	
	void update(float dt);	//Update the node
	void collided(Object* o);		//Collided with an object
	void init();				//Create stuff in lua for this object
	
	void setProperty(string prop, string value)	{propertyValues[prop] = value;};
	void addProperty(string prop, string value) {setProperty(prop, value);};
	string getProperty(string prop)				{if(propertyValues.count(prop)) return propertyValues[prop]; return "";};
};

