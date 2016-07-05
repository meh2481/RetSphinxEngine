#include "EntityManager.h"
#include "ParticleSystemManager.h"
#include "ResourceLoader.h"
#include "NodeManager.h"
#include "globaldefs.h"	//TODO Remove
#include "Object.h"
#include "ObjectManager.h"
#include "SceneryManager.h"

EntityManager::EntityManager(ResourceLoader* resourceLoader, b2World* world)
{
	particleSystemManager = new ParticleSystemManager(resourceLoader);
	nodeManager = new NodeManager();
	objectManager = new ObjectManager(world);
	sceneryManager = new SceneryManager();
}

EntityManager::~EntityManager()
{
	delete particleSystemManager;
	delete nodeManager;
	delete objectManager;
	delete sceneryManager;
}

//General methods
void EntityManager::update(float dt)
{
	particleSystemManager->update(dt);
	nodeManager->update(dt);
	objectManager->update(dt);
	sceneryManager->update(dt);
}

void EntityManager::render(glm::mat4 mat)
{
	sceneryManager->renderBackground(mat);
	objectManager->render(mat);
	particleSystemManager->render(mat);
	sceneryManager->renderForeground(mat);
}

void EntityManager::cleanup()
{
	particleSystemManager->cleanup();
	nodeManager->cleanup();
	objectManager->cleanup();
	sceneryManager->cleanup();
}

//Particle systems
void EntityManager::add(ParticleSystem * pSys)
{
	particleSystemManager->add(pSys);
}

//Nodes
void EntityManager::add(Node * n)
{
	nodeManager->add(n);
}

Node* EntityManager::getNode(Vec2 pos)
{
	return nodeManager->getNode(pos);
}

Node* EntityManager::getNode(string sNodeName)
{
	return nodeManager->getNode(sNodeName);
}

//Objects
void EntityManager::add(Object * o)
{
	objectManager->add(o);
}

Object* EntityManager::getObject(Vec2 p)
{
	return objectManager->get(p);
}

Object* EntityManager::getClosestObject(Vec2 p)
{
	return objectManager->getClosest(p);
}

//Scenery
void EntityManager::add(ObjSegment * o)
{
	sceneryManager->add(o);
}

