#pragma once
#include <map>
#include <string>
#include "tinyxml2.h"
using namespace std;

class Image;
class ParticleSystem;
class MouseCursor;
class ObjSegment;

class ResourceLoader
{
	map<string, Image*> m_mImages;

	unsigned char* getResource(string sID);
public:

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
};