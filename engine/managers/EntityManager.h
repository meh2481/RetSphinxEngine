#pragma once
#include "glm/glm.hpp"
#include <string>
#include "globaldefs.h" //TODO Remove
using namespace std;

class ParticleSystemManager;
class ResourceLoader;
class ParticleSystem;
class NodeManager;
class Node;
class ObjectManager;
class Object;
class b2World;

class EntityManager
{
	ParticleSystemManager* particleSystemManager;
	NodeManager* nodeManager;
	ObjectManager* objectManager;

public:
	EntityManager(ResourceLoader* resourceLoader, b2World* world);
	~EntityManager();

	void update(float dt);
	void render(glm::mat4 mat);

	void cleanup();

	//Particle system functions
	void add(ParticleSystem* pSys);

	//Node functions
	void add(Node* n);
	Node* getNode(Point pos);
	Node* getNode(string sNodeName);

	//Object funtions
	void add(Object* o);
	Object* getObject(Point p);
	Object* getClosestObject(Point p);
};