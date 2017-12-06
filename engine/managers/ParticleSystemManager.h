#pragma once
#include <list>
#include "Observer.h"
#include "Subject.h"
#include "ResourceLoader.h"
#include "RenderState.h"

class ParticleSystem;

class ParticleSystemManager : public Observer
{
	ParticleSystemManager() {};

	std::list<ParticleSystem*> m_particles;
	std::list<ParticleSystem*> m_updateParticles;
	Subject* m_notifySubject;
	ResourceLoader* m_loader;
	bool updating;

public:
	ParticleSystemManager(ResourceLoader* loader);
	~ParticleSystemManager();

	void add(ParticleSystem* sys);
	void cleanup();
	void render(const RenderState& renderState);
	void update(float dt);

	virtual void onNotify(const std::string& sParticleFilename, Vec2 pos);
};

