#pragma once
#include <map>
#include <string>
#include <inttypes.h>
#include "tinyxml2.h"
#include "Box2D/Box2D.h"
#include "Rect.h"
#include "SDL.h"
#include "ResourceTypes.h"

class Object;
class ParticleSystem;
class ObjSegment;
class ResourceCache;
class Object3D;
class PakLoader;
class ImgFont;
class Stringbank;
class Image;
class Texture;
class LuaInterface;
class RenderState;

class ResourceLoader
{
    b2World* m_world;
    ResourceCache* m_cache;
    PakLoader* m_pakLoader;
    std::string m_sPakDir;
    RenderState* m_3dShader;
    RenderState* m_particleShader;

    std::string readTextFile(const std::string& filename);
    Image* loadImageFromFile(const std::string& filename);
    Image* loadImageFromData(unsigned char* data, unsigned int len);
    Texture* bindTexture(unsigned char* data, unsigned int width, unsigned int height, int mode, int len);
    Texture* getAtlas(uint64_t atlasId);

    ResourceLoader() {};
public:
    ResourceLoader(b2World* physicsWorld, const std::string& sPakDir);
    ~ResourceLoader();

    //Utility
    void clearCache();
    void set3dShader(RenderState* shader) { m_3dShader = shader; };
    void setParticleShader(RenderState* shader) { m_particleShader = shader; };
    RenderState* getParticleShader() { return m_particleShader; };

    //Images
    Image* getImage(const std::string& sID);
    Image* getImage(uint64_t hashID);
    SDL_Surface* getSDLImage(const std::string& sID);

    //Meshes
    Object3D* get3dObject(const std::string& sID);

    //Particles
    ParticleSystem* getParticleSystem(const std::string& sID);

    //Mouse cursors
    SDL_Cursor* getCursor(const std::string& sID);

    //Fonts
    ImgFont* getFont(const std::string& sID);

    //Text
    Stringbank* getStringbank(const std::string& sID);
    std::string getTextFile(const std::string& sID);    //Read a whole text file into one string

    //Objects
    Object* getObject(const std::string& sType, Vec2 ptOffset, Vec2 ptVel, LuaInterface* lua);
    ObjSegment* getObjectSegment(tinyxml2::XMLElement* layer);
    b2Fixture* getObjectFixture(tinyxml2::XMLElement* fixture, b2Body* bod);

    //Sounds
    unsigned char* getSound(const std::string& sID, unsigned int* length);    // Raw data since we load with FMOD
    SoundLoop* getSoundLoop(const std::string& sID);

    //Binary data
    unsigned char* getData(const std::string& sID);
};
