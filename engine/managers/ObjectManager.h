#pragma once
#include <list>
#include "globaldefs.h"	//TODO Remove
#include "glmx.h"
using namespace std;

class Object;
class b2World;

class ObjectManager
{
	bool updating;
	list<Object*> m_lObjects;	//Object list
	list<Object*> m_lUpdateObjects;	//Temp holder for objects added while iterating over the object list
	b2World* m_physicsWorld;

public:
	ObjectManager(b2World* world);
	~ObjectManager();

	void render(glm::mat4 mat);
	void add(Object* o);
	void cleanup();
	void update(float dt);

	//TODO What are the differences between these two? Why do we need two?
	Object* get(Vec2 p);	//Get first object at this point
	Object* getClosest(Vec2 p);	//Get closest object to this point

};