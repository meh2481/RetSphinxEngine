#include "EntityManager.h"
#include "ParticleSystemManager.h"
#include "ResourceLoader.h"
#include "NodeManager.h"
#include "globaldefs.h"	//TODO Remove

EntityManager::EntityManager(ResourceLoader* resourceLoader)
{
	particleSystemManager = new ParticleSystemManager(resourceLoader);
	nodeManager = new NodeManager();
}

EntityManager::~EntityManager()
{
	delete particleSystemManager;
	delete nodeManager;
}

void EntityManager::update(float dt)
{
	particleSystemManager->update(dt);
	nodeManager->update(dt);
}

void EntityManager::render(glm::mat4 mat)
{
	//TODO Use the mat4
	particleSystemManager->render();
}

void EntityManager::cleanup()
{
	particleSystemManager->cleanup();
	nodeManager->cleanup();
}

void EntityManager::add(ParticleSystem * pSys)
{
	particleSystemManager->add(pSys);
}

void EntityManager::addNode(Node * n)
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
