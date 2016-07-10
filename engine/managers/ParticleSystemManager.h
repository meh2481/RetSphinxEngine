#pragma once
#include <list>
#include "Observer.h"
#include "Subject.h"
#include "ResourceLoader.h"
#include "glmx.h"

class ParticleSystem;

class ParticleSystemManager : public Observer
{
	ParticleSystemManager() {};

	std::list<ParticleSystem*> m_particles;
	std::list<ParticleSystem*> m_updateParticles;
	Subject* m_notifySubject;
	ResourceLoader* m_loader;	//TODO This should just be a ParticleSystemLoader, not ResourceLoader?
	bool updating;

public:
	ParticleSystemManager(ResourceLoader* loader);
	~ParticleSystemManager();

	void add(ParticleSystem* sys);
	void cleanup();
	void render(glm::mat4 mat);
	void update(float dt);

	virtual void onNotify(std::string sParticleFilename, Vec2 pos);
};

