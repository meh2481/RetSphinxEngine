#pragma once
#include <map>
#include <string>
using namespace std;

class Image;
class ParticleSystem;
class MouseCursor;

class ResourceLoader
{

	map<string, Image*> m_mImages;
public:
	//Images
	Image* getImage(string sID);
	void clearImages();
	void reloadImages();
	//Particles
	ParticleSystem* getParticleSystem(string sID);
	MouseCursor* getCursor(string sID);
};