#pragma once
#include <list>
#include "Observer.h"
#include "Subject.h"
#include "ResourceLoader.h"
using namespace std;

class ParticleSystem;

class ParticleSystemManager : public Observer
{
	ParticleSystemManager() {};

	list<ParticleSystem*> m_particles;
	list<ParticleSystem*> m_updateParticles;
	Subject* m_notifySubject;
	ResourceLoader* m_loader;	//TODO This should just be a ParticleSystemLoader, not ResourceLoader?
	bool updating;

public:
	ParticleSystemManager(ResourceLoader* loader);
	~ParticleSystemManager();

	void addParticles(ParticleSystem* sys);
	void cleanupParticles();
	void drawParticles();
	void updateParticles(float dt);

	virtual void onNotify(string sParticleFilename, Point pos);
};

