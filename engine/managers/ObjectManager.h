#pragma once
#include <list>
#include "Rect.h"
#include "glmx.h"

class Object;
class b2World;

class ObjectManager
{
	bool updating;
	std::list<Object*> m_lObjects;	//Object list
	std::list<Object*> m_lUpdateObjects;	//Temp holder for objects added while iterating over the object list
	b2World* m_physicsWorld;

public:
	ObjectManager(b2World* world);
	~ObjectManager();

	void render(glm::mat4 mat);
	void add(Object* o);
	void cleanup();
	void update(float dt);

	Object* getAt(Vec2 p);	//Get first object at this point
	Object* getClosest(Vec2 p);	//Get closest object to this point

};