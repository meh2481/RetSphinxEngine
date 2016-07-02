#pragma once
#include <map>
#include <string>
#include "Observer.h"
using namespace std;

class Image;
class ParticleSystem;
class MouseCursor;

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
};