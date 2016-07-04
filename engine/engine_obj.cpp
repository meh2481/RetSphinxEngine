#include "Engine.h"
#include "EntityManager.h"

//TODO: I hate this a lot
void Engine::drawAll()
{
	glm::mat4 mat; //TODO use this for drawing

	for(multiset<ObjSegment*>::iterator i = m_lScenery.begin(); i != m_lScenery.end(); i++)	//Add bg layers
		(*i)->draw();

	m_entityManager->render(mat);	//TODO: Better draw order
}

//void Engine::cleanupObjects()
//{
//	for(multiset<ObjSegment*>::iterator i = m_lScenery.begin(); i != m_lScenery.end(); i++)
//		delete (*i);
//	
//	m_lScenery.clear();
//
//	m_entityManager->cleanup();
//	
//	
//}

//void Engine::updateObjects(float dt)
//{
//	//Update objects
//	
//
//	//TODO: Add any added objects that were generated while updating
//}

