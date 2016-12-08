#pragma once

#include "Object.h"

class Node {
	LuaObjGlue* 		glueObj;
	std::map<std::string, std::string> propertyValues;	//This can be populated by XML and called from Lua! For userdata and such
public:
	enum { TYPE = OT_NODE };
	std::string 	luaClass;
	LuaInterface* 	lua;
	Vec2 			pos;
	std::string 	name;
	b2Body*         body;
	
	Node();
	~Node();
	
	void update(float dt);	//Update the node
	void collided(Object* o);		//Collided with an object
	void init();				//Create stuff in lua for this object
	
	void setProperty(std::string prop, std::string value)	{propertyValues[prop] = value;};
	void addProperty(std::string prop, std::string value) {setProperty(prop, value);};
	std::string getProperty(std::string prop)				{if(propertyValues.count(prop)) return propertyValues[prop]; return "";};
};

