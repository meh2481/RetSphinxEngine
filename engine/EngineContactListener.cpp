#include "EngineContactListener.h"

void EngineContactListener::BeginContact(b2Contact *contact)
{
	currentContacts.insert(contact);	//Keep track of all contacts
	if(!contact->IsTouching()) return;	//Don't keep track of temp contacts if they're not touching, however //TODO: Can we miss short contacts this way?
	
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

obj* EngineContactListener::getObj(b2Fixture* fix)
{
	obj* result = NULL;
	if(fix)
	{
		b2Body* bod = fix->GetBody();
		if(bod)
		{
			void* data = bod->GetUserData();
			if(data)
			{
				physSegment* seg = (physSegment*)data;
				result = seg->parent;
			}
		}
	}
	return result;
}

collision EngineContactListener::getCollision(b2Contact* c)
{
	collision cResult = {NULL, NULL};
	if(!c->IsTouching()) return cResult;
	
	b2Fixture* fixA = c->GetFixtureA();
	b2Fixture* fixB = c->GetFixtureB();
	
	cResult.objA = getObj(fixA);
	cResult.objB = getObj(fixB);
	
	return cResult;
}