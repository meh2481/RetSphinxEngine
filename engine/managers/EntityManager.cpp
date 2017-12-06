#include "EntityManager.h"
#include "ParticleSystemManager.h"
#include "ResourceLoader.h"
#include "NodeManager.h"
#include "Object.h"
#include "ObjectManager.h"
#include "SceneryManager.h"

EntityManager::EntityManager(ResourceLoader* resourceLoader, b2World* world)
{
	particleSystemManager = new ParticleSystemManager(resourceLoader);
	nodeManager = new NodeManager(world);
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

void EntityManager::render(const RenderState& renderState)
{
	sceneryManager->renderBackground(renderState);
	objectManager->render(renderState);
	particleSystemManager->render(renderState);
	sceneryManager->renderForeground(renderState);
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

Node* EntityManager::getNodeUnder(Vec2 pos)
{
	return nodeManager->getNodeUnder(pos);
}

Node* EntityManager::getNode(const std::string& sNodeName)
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
	return objectManager->getAt(p);
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

