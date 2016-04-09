#ifndef ENGINE_CONTACT_LISTENER_H
#define ENGINE_CONTACT_LISTENER_H

#include "Box2D/Dynamics/b2WorldCallbacks.h"
#include "Object.h"

typedef struct {
	obj* objA;
	obj* objB;
}collision;
	
class EngineContactListener : public b2ContactListener
{
	set<b2Contact*> m_tmpFrameContacts;
public:
	set<b2Contact*> frameContacts;
	set<b2Contact*> currentContacts;
	
	//Implementations from Box2D
	virtual void BeginContact(b2Contact *contact);
	virtual void EndContact(b2Contact *contact);
	virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
	virtual void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
	
	//Helper functions for my use
	static collision getCollision(b2Contact* c);	//Get two objects that are colliding on this fixture (either one can be NULL)
	static obj* getObj(b2Fixture* fix);				//Get the object (or NULL) this fixture is associated with
	void clearFrameContacts();						//Call every frame to wipe temporary (hit and leave) contacts for that frame
};





#endif //ifndef ENGINE_CONTACT_LISTENER_H