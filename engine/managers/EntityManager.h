#pragma once
#include "glmx.h"
#include <string>
#include "Rect.h"

class ParticleSystemManager;
class ResourceLoader;
class ParticleSystem;
class NodeManager;
class Node;
class ObjectManager;
class Object;
class ObjSegment;
class b2World;
class SceneryManager;

class EntityManager
{
	ParticleSystemManager* particleSystemManager;
	NodeManager* nodeManager;
	ObjectManager* objectManager;
	SceneryManager* sceneryManager;

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
	Node* getNode(Vec2 pos);
	Node* getNode(std::string sNodeName);

	//Object funtions
	void add(Object* o);
	Object* getObject(Vec2 p);
	Object* getClosestObject(Vec2 p);

	//Scenery functions
	void add(ObjSegment* o);
};