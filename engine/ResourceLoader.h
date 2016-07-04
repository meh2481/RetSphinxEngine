#pragma once
#include <map>
#include <string>
#include "tinyxml2.h"
#include "globaldefs.h" //TODO Remove
using namespace std;

class Image;
class Object;
class ParticleSystem;
class MouseCursor;
class ObjSegment;
class b2World;
class b2Body;

class ResourceLoader
{
	map<string, Image*> m_mImages;
	b2World* m_world;

	void readFixture(tinyxml2::XMLElement* fixture, b2Body* bod);
	unsigned char* getResource(string sID);
	ResourceLoader() {};
public:
	ResourceLoader(b2World* physicsWorld);

	//Images
	Image* getImage(string sID);

	//TODO: These should be implemented in a cache, not here
	void clearImages();
	void reloadImages();

	//Particles
	ParticleSystem* getParticleSystem(string sID);

	//Mouse cursor
	MouseCursor* getCursor(string sID);

	//TODO: Private
	ObjSegment* getObjSegment(tinyxml2::XMLElement* layer);

	Object* objFromXML(string sType, Point ptOffset, Point ptVel);
};