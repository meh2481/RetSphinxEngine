#pragma once
#include "glm\glm.hpp"

class ParticleSystemManager;
class ResourceLoader;
class ParticleSystem;

class EntityManager
{
	ParticleSystemManager* particleSystemManager;

public:
	EntityManager(ResourceLoader* resourceLoader);
	~EntityManager();

	void update(float dt);
	void render(glm::mat4 mat);

	void cleanupParticles();
	void addParticles(ParticleSystem* pSys);

};