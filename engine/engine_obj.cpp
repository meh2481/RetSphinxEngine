#include "Engine.h"

void Engine::addNode(Node* n)
{
	if(n != NULL)
	{
		m_nodes[n->name] = n;
		n->init();
	}
}

void Engine::addObject(obj* o)
{
	if(o != NULL)
	{
		m_lObjects.push_back(o);
		o->initLua();
	}
}

/*/ This seems like a bad idea overall
void Engine::updateSceneryLayer(physSegment* seg)
{
	for(multiset<physSegment*>::iterator layer = m_lScenery.begin(); layer != m_lScenery.end(); layer++)
	{
		if((*layer) == seg)
		{
			m_lScenery.erase(layer);
			break;
		}
	}
	m_lScenery.insert(seg);
}*/

//TODO: I hate this a lot
void Engine::drawAll()
{
	multimap<float, Drawable*> drawList;	//Depth matters, and some Drawables will be at different depths, so draw all in one pass

	for(multiset<physSegment*>::iterator i = m_lScenery.begin(); i != m_lScenery.end(); i++)	//Add bg layers
		drawList.insert(make_pair((*i)->depth, (Drawable*)(*i)));
		
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)	//Add objects
		drawList.insert(make_pair((*i)->depth, (Drawable*)(*i)));
		
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)	//Add particles
		drawList.insert(make_pair((*i)->depth, (Drawable*)(*i)));
		
	//Draw everything in one pass
	for(multimap<float, Drawable*>::iterator i = drawList.begin(); i != drawList.end(); i++)
		i->second->draw(m_bObjDebugDraw);
}

void Engine::cleanupObjects()
{
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
		delete (*i);
	for(multiset<physSegment*>::iterator i = m_lScenery.begin(); i != m_lScenery.end(); i++)
		delete (*i);
	for(map<string, Node*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
		delete it->second;
	m_lScenery.clear();
	m_lObjects.clear();
	m_nodes.clear();
	
	//Wipe Box2D physics data that's left over
	list<b2Body*> bodies;
	for(b2Body* bod = m_physicsWorld->GetBodyList(); bod != NULL; bod = bod->GetNext())
		bodies.push_back(bod);
	
	for(list<b2Body*>::iterator i = bodies.begin(); i != bodies.end(); i++)
		m_physicsWorld->DestroyBody(*i);
}

void Engine::updateObjects(float32 dt)
{
	//Update objects
	//TODO: This whole object returning a b2BodyDef* can be removed
	list<b2BodyDef*> lAddObj;	//For holding objects generated by these objects updating
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
	{
		b2BodyDef* def = (*i)->update(dt);
		if(def != NULL)
			lAddObj.push_back(def);
	}
	//Add generated objects
	//TODO: Remove
	for(list<b2BodyDef*>::iterator i = lAddObj.begin(); i != lAddObj.end(); i++)
	{
		objFromXML(*((string*)(*i)->userData), (*i)->position, (*i)->linearVelocity);
		delete (*i);	//Free up memory
	}
	//Update nodes
	for(map<string, Node*>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
		i->second->update(dt);
}

Node* Engine::getNode(string sNodeName)
{
	map<string, Node*>::iterator i = m_nodes.find(sNodeName);
	if(i != m_nodes.end())
		return i->second;
	return NULL;
}

class PointQueryCallback : public b2QueryCallback
{
public:
	list<b2Body*> foundBodies;
	
	bool ReportFixture(b2Fixture* fixture)
	{
		foundBodies.push_back(fixture->GetBody()); 
		return true;
	}
};

obj* Engine::getClosestObject(Point p)
{
	obj* closest = NULL;
	float32 len2 = FLT_MAX;
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
	{
		Point offset = (*i)->getPos() - p;
		float32 dist = offset.LengthSquared();
		if(dist < len2)
		{
			closest = *i;
			len2 = dist;
		}
	}
	return closest;
}

obj* Engine::getObject(Point p)
{
	PointQueryCallback pqc;
	b2AABB aabb;
	aabb.lowerBound = p;
	aabb.upperBound = p;
	m_physicsWorld->QueryAABB(&pqc, aabb);
	
	//This returns a list of possible bodies; loop through and check for actual containment
	for(list<b2Body*>::iterator i = pqc.foundBodies.begin(); i != pqc.foundBodies.end(); i++)
	{
		for(b2Fixture* fix = (*i)->GetFixtureList(); fix != NULL; fix = fix->GetNext())
		{
			if(fix->TestPoint(p))	//we have a hit
			{
				void* data = (*i)->GetUserData();
				if(data)
				{
					physSegment* seg = (physSegment*)data;
					if(seg->parent != NULL)
						return seg->parent;	//Done!
				}
			}
		}
	}
	
	return NULL;
}

Node* Engine::getNode(Point p)
{
	PointQueryCallback pqc;
	b2AABB aabb;
	aabb.lowerBound = p;
	aabb.upperBound = p;
	m_physicsWorld->QueryAABB(&pqc, aabb);
	
	//This returns a list of possible bodies; loop through and check for actual containment
	for(list<b2Body*>::iterator i = pqc.foundBodies.begin(); i != pqc.foundBodies.end(); i++)
	{
		for(b2Fixture* fix = (*i)->GetFixtureList(); fix != NULL; fix = fix->GetNext())
		{
			if(fix->TestPoint(p))	//we have a hit
			{
				void* data = fix->GetUserData();
				if(data)
					return (Node*)(data);
			}
		}
	}
	
	return NULL;
}