#pragma once
#include <list>
#include "ParticleSystem.h"
#include "Observer.h"
#include "Subject.h"
#include "ResourceLoader.h"
using namespace std;

class ParticleSystemManager : public Observer
{
	ParticleSystemManager() {};

	list<ParticleSystem*> m_particles;
	Subject* m_notifySubject;
	ResourceLoader* m_loader;	//TODO This should just be a ParticleSystemLoader, not ResourceLoader?

public:
	ParticleSystemManager(ResourceLoader* loader);

	void addParticles(ParticleSystem* sys);
	void cleanupParticles();
	void drawParticles();
	void updateParticles(float dt);

	virtual void onNotify(string sParticleFilename, Point pos);
};

