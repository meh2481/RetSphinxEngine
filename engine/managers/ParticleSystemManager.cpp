#include "ParticleSystemManager.h"

ParticleSystemManager::ParticleSystemManager(ResourceLoader* loader)
{
	m_notifySubject = new Subject();
	m_notifySubject->addObserver(this);
	m_loader = loader;
}

void ParticleSystemManager::addParticles(ParticleSystem * sys)
{
	if(sys)
	{
		m_particles.push_back(sys);
		sys->setSubject(m_notifySubject);
	}
}

void ParticleSystemManager::cleanupParticles()
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
		delete *i;
	m_particles.clear();
}

void ParticleSystemManager::drawParticles()
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
		(*i)->draw();
}

void ParticleSystemManager::updateParticles(float dt)
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
	{
		(*i)->update(dt);
		if((*i)->done())
		{
			delete *i;
			i = m_particles.erase(i);
			continue;
		}
	}
}

void ParticleSystemManager::onNotify(string sParticleFilename, Point pos)
{
}
