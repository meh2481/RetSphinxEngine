#include "EngineContactListener.h"
#include <Box2D/Box2D.h>

void EngineContactListener::BeginContact(b2Contact *contact)
{
	currentContacts.insert(contact);	//Keep track of all contacts
	if(!contact->IsTouching()) return;	//Don't keep track of temp contacts if they're not touching, however. Can we miss short contacts this way? I dunno, don't care
	
	m_tmpFrameContacts.insert(contact);	//Keep track of all contacts that started this frame
}

void EngineContactListener::EndContact(b2Contact *contact)
{
	currentContacts.erase(contact);			//Don't track this contact anymore
	if(m_tmpFrameContacts.count(contact))	//If we've added this this frame, add this to our set that keeps track of short fast contacts (so they can still be registered in this frame)
		frameContacts.insert(contact);
}

void EngineContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
	
}

void EngineContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
{
	
}

Object* EngineContactListener::getObj(b2Fixture* fix)
{
	Object* result = NULL;
	if(fix)
	{
		b2Body* bod = fix->GetBody();
		if(bod)
		{
			void* data = bod->GetUserData();
			if(data)
			{
				ObjSegment* seg = (ObjSegment*)data;
				result = seg->parent;
			}
		}
	}
	return result;
}

Collision EngineContactListener::getCollision(b2Contact* c)
{
	Collision cResult = {NULL, NULL, NULL, NULL, c->GetManifold()->points[0].normalImpulse};
	if(!c->IsTouching()) return cResult;
	
	b2Fixture* fixA = c->GetFixtureA();
	b2Fixture* fixB = c->GetFixtureB();
	
	void* fixAUser = fixA->GetUserData();
	void* fixBUser = fixB->GetUserData();
	
	if(fixAUser)
		cResult.nodeA = (Node*)fixAUser;
	if(fixBUser)
		cResult.nodeB = (Node*)fixBUser;
	
	cResult.objA = getObj(fixA);
	cResult.objB = getObj(fixB);
	
	return cResult;
}

void EngineContactListener::clearFrameContacts() 
{
	frameContacts.clear();
	m_tmpFrameContacts.clear();
}