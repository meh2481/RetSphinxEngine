#include "SceneryManager.h"
#include "Object.h"

SceneryManager::~SceneryManager()
{
	cleanup();
}

void SceneryManager::update(float dt)
{
	//for(std::multiset<ObjSegment*>::iterator i = m_lSceneryFg.begin(); i != m_lSceneryFg.end(); i++)
	//	(*i)->update(dt);
	//for(std::multiset<ObjSegment*>::iterator i = m_lSceneryBg.begin(); i != m_lSceneryBg.end(); i++)
	//	(*i)->update(dt);
}

void SceneryManager::renderForeground(glm::mat4 mat)
{
	for(std::multiset<ObjSegment*>::iterator i = m_lSceneryFg.begin(); i != m_lSceneryFg.end(); i++)
		(*i)->draw();	//TODO Use mat
}

void SceneryManager::renderBackground(glm::mat4 mat)
{
	for(std::multiset<ObjSegment*>::iterator i = m_lSceneryBg.begin(); i != m_lSceneryBg.end(); i++)
		(*i)->draw();	//TODO Use mat
}

void SceneryManager::add(ObjSegment * seg)
{
	if(seg->depth > 0)
		m_lSceneryFg.insert(seg);
	else
		m_lSceneryBg.insert(seg);
}

void SceneryManager::cleanup()
{
	for(std::multiset<ObjSegment*>::iterator i = m_lSceneryFg.begin(); i != m_lSceneryFg.end(); i++)
		delete (*i);
	for(std::multiset<ObjSegment*>::iterator i = m_lSceneryBg.begin(); i != m_lSceneryBg.end(); i++)
		delete (*i);

	m_lSceneryBg.clear();
	m_lSceneryFg.clear();
}
