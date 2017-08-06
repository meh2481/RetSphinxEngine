#pragma once
#include <map>
#include <string>
#include <inttypes.h>
#include "tinyxml2.h"
#include "Box2D/Box2D.h"
#include "Rect.h"
#include "SDL.h"
#include "Quad.h"
#include "ResourceTypes.h"

class Object;
class ParticleSystem;
class ObjSegment;
class ResourceCache;
class Mesh3D;
class PakLoader;
class ImgFont;
class Stringbank;

class ResourceLoader
{
	b2World* m_world;
	ResourceCache* m_cache;
	PakLoader* m_pakLoader;
	std::string m_sPakDir;

	std::string readTextFile(const std::string& filename);
	Img* loadImageFromFile(std::string filename); 
	Img* loadImageFromData(unsigned char* data, unsigned int len);
	Img* bindImage(unsigned char* data, unsigned int width, unsigned int height, int mode, const float* uvs);

	ResourceLoader() {};
public:
	ResourceLoader(b2World* physicsWorld, const std::string& sPakDir);
	~ResourceLoader();

	//Utility
	void clearCache();

	//Images
	Img* getImage(const std::string& sID);
	Img* getImage(uint64_t hashID);
	SDL_Surface* getSDLImage(const std::string& sID);

	//Meshes
	Mesh3D* getMesh(const std::string& sID);

	//Particles
	ParticleSystem* getParticleSystem(const std::string& sID);

	//Mouse cursors
	SDL_Cursor* getCursor(const std::string& sID);

	//Fonts
	ImgFont* getFont(const std::string& sID);

	//Text
	Stringbank* getStringbank(const std::string& sID);
	std::string getTextFile(const std::string& sID);	//Read a whole text file into one string

	//Objects
	Object* getObject(const std::string& sType, Vec2 ptOffset, Vec2 ptVel);
	ObjSegment* getObjectSegment(tinyxml2::XMLElement* layer);
	b2Fixture* getObjectFixture(tinyxml2::XMLElement* fixture, b2Body* bod);

	//Sounds
	unsigned char* getSound(const std::string& sID, unsigned int* length);	// Raw data since we load with FMOD
	SoundLoop* getSoundLoop(const std::string& sID);
};
