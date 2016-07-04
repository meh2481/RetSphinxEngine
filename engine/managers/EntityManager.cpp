#include "EntityManager.h"
#include "ParticleSystemManager.h"
#include "ResourceLoader.h"
#include "NodeManager.h"
#include "globaldefs.h"	//TODO Remove
#include "Object.h"
#include "ObjectManager.h"

EntityManager::EntityManager(ResourceLoader* resourceLoader, b2World* world)
{
	particleSystemManager = new ParticleSystemManager(resourceLoader);
	nodeManager = new NodeManager();
	objectManager = new ObjectManager(world);
}

EntityManager::~EntityManager()
{
	delete particleSystemManager;
	delete nodeManager;
	delete objectManager;
}

void EntityManager::update(float dt)
{
	particleSystemManager->update(dt);
	nodeManager->update(dt);
	objectManager->update(dt);
}

void EntityManager::render(glm::mat4 mat)
{
	objectManager->render(mat);
	particleSystemManager->render(mat);
}

void EntityManager::cleanup()
{
	particleSystemManager->cleanup();
	nodeManager->cleanup();
	objectManager->cleanup();
}

void EntityManager::add(ParticleSystem * pSys)
{
	particleSystemManager->add(pSys);
}

void EntityManager::add(Node * n)
{
	nodeManager->add(n);
}

Node* EntityManager::getNode(Point pos)
{
	return nodeManager->getNode(pos);
}

Node* EntityManager::getNode(string sNodeName)
{
	return nodeManager->getNode(sNodeName);
}

void EntityManager::add(Object * o)
{
	objectManager->add(o);
}

Object * EntityManager::getObject(Point p)
{
	return objectManager->get(p);
}

Object * EntityManager::getClosestObject(Point p)
{
	return objectManager->getClosest(p);
}
