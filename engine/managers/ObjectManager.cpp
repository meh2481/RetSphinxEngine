#include "ObjectManager.h"
#include "Object.h"
#include "Box2D/Box2D.h"

//-----------------------------------------------------
// Callback helper class for finding objects
//-----------------------------------------------------
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

//-----------------------------------------------------
// ObjectManager class functions
//-----------------------------------------------------

ObjectManager::ObjectManager(b2World * world)
{
	m_physicsWorld = world;
	updating = false;
}

ObjectManager::~ObjectManager()
{
	cleanup();
}

void ObjectManager::render(glm::mat4 mat)	//TODO Use mat
{
	for(list<Object*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)	//Add objects
		(*i)->draw();
}

void ObjectManager::add(Object * o)
{
	if(o)
	{
		if(updating)
			m_lUpdateObjects.push_back(o);
		else
			m_lObjects.push_back(o);
		o->initLua();
	}
}

void ObjectManager::cleanup()
{
	for(list<Object*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
		delete (*i);
	m_lObjects.clear();

	//Wipe Box2D physics data that's left over
	list<b2Body*> bodies;
	for(b2Body* bod = m_physicsWorld->GetBodyList(); bod != NULL; bod = bod->GetNext())
		bodies.push_back(bod);

	for(list<b2Body*>::iterator i = bodies.begin(); i != bodies.end(); i++)
		m_physicsWorld->DestroyBody(*i);
}

void ObjectManager::update(float dt)
{
	//TODO Have way for objects to die
	updating = true;
	for(list<Object*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
		(*i)->update(dt);
	updating = false;
	for(list<Object*>::iterator i = m_lUpdateObjects.begin(); i != m_lUpdateObjects.end(); i++)
		m_lObjects.push_back(*i);
	m_lUpdateObjects.clear();
}

Object* ObjectManager::get(Vec2 p)
{
	PointQueryCallback pqc;
	b2AABB aabb;
	aabb.lowerBound.Set(p.x, p.y);
	aabb.upperBound.Set(p.x, p.y);
	m_physicsWorld->QueryAABB(&pqc, aabb);

	//This returns a list of possible bodies; loop through and check for actual containment
	for(list<b2Body*>::iterator i = pqc.foundBodies.begin(); i != pqc.foundBodies.end(); i++)
	{
		for(b2Fixture* fix = (*i)->GetFixtureList(); fix != NULL; fix = fix->GetNext())
		{
			if(fix->TestPoint(b2Vec2(p.x, p.y)))	//we have a hit
			{
				void* data = (*i)->GetUserData();
				if(data)
				{
					ObjSegment* seg = (ObjSegment*)data;
					if(seg->parent != NULL)
						return seg->parent;	//Done!
				}
			}
		}
	}

	return NULL;
}

Object* ObjectManager::getClosest(Vec2 p)
{
	Object* closest = NULL;
	float len2 = FLT_MAX;
	for(list<Object*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
	{
		Vec2 offset = (*i)->getPos() - p;
		float dist = glmx::lensqr(offset);
		if(dist < len2)
		{
			closest = *i;
			len2 = dist;
		}
	}
	return closest;
}