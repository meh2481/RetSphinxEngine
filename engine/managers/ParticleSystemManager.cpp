#include "ParticleSystemManager.h"
#include "ParticleSystem.h"

ParticleSystemManager::ParticleSystemManager(ResourceLoader* loader)
{
	updating = false;
	m_notifySubject = new Subject();
	m_notifySubject->addObserver(this);
	m_loader = loader;
}

ParticleSystemManager::~ParticleSystemManager()
{
	cleanup();
	delete m_notifySubject;
}

void ParticleSystemManager::add(ParticleSystem * sys)
{
	if(sys)
	{
		sys->setSubject(m_notifySubject);
		if(updating)
			m_updateParticles.push_back(sys);
		else
			m_particles.push_back(sys);
	}
}

void ParticleSystemManager::cleanup()
{
	for(std::list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
		delete *i;
	for(std::list<ParticleSystem*>::iterator i = m_updateParticles.begin(); i != m_updateParticles.end(); i++)
		delete *i;
	m_particles.clear();
	m_updateParticles.clear();
}

void ParticleSystemManager::render(glm::mat4 mat)
{
	for(std::list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
		(*i)->draw(mat);
}

void ParticleSystemManager::update(float dt)
{
	updating = true;
	for(std::list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
	{
		(*i)->update(dt);
		if((*i)->done())
		{
			delete *i;
			i = m_particles.erase(i);
			continue;
		}
	}
	updating = false;
	for(std::list<ParticleSystem*>::iterator i = m_updateParticles.begin(); i != m_updateParticles.end(); i++)
		m_particles.push_back(*i);

	m_updateParticles.clear();
}

void ParticleSystemManager::onNotify(const std::string& sParticleFilename, Vec2 pos)
{
	ParticleSystem* pSys = m_loader->getParticleSystem(sParticleFilename);
	pSys->emitFrom.centerOn(pos);
	add(pSys);
}
