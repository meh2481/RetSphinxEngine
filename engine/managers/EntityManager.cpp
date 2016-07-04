#include "EntityManager.h"
#include "ParticleSystemManager.h"
#include "ResourceLoader.h"

EntityManager::EntityManager(ResourceLoader* resourceLoader)
{
	particleSystemManager = new ParticleSystemManager(resourceLoader);
}

EntityManager::~EntityManager()
{
	delete particleSystemManager;
}

void EntityManager::update(float dt)
{
	particleSystemManager->updateParticles(dt);
}

void EntityManager::render(glm::mat4 mat)
{
	//TODO Use the mat4
	particleSystemManager->drawParticles();
}

void EntityManager::cleanupParticles()
{
	particleSystemManager->cleanupParticles();
}

void EntityManager::addParticles(ParticleSystem * pSys)
{
	particleSystemManager->addParticles(pSys);
}
