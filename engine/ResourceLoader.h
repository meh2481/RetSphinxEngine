#pragma once
#include <map>
#include <string>
#include <inttypes.h>
#include "tinyxml2.h"
#include "Rect.h"
using namespace std;

class Image;
class Object;
class ParticleSystem;
class MouseCursor;
class ObjSegment;
class b2World;
class b2Body;
class ResourceCache;
class Mesh3D;
class PakLoader;

class ResourceLoader
{
	b2World* m_world;
	ResourceCache* m_cache;
	PakLoader* m_pakLoader;

	uint64_t hash(string sHashStr);

	void readFixture(tinyxml2::XMLElement* fixture, b2Body* bod);
	ResourceLoader() {};
public:
	ResourceLoader(b2World* physicsWorld);
	~ResourceLoader();

	ResourceCache* getCache() { return m_cache; };

	//Images
	Image* getImage(string sID);

	//Meshes
	Mesh3D* getMesh(string sID);

	//Particles
	ParticleSystem* getParticleSystem(string sID);

	//Mouse cursors
	MouseCursor* getCursor(string sID);

	//TODO: Private
	ObjSegment* getObjSegment(tinyxml2::XMLElement* layer);

	Object* objFromXML(string sType, Vec2 ptOffset, Vec2 ptVel);
};