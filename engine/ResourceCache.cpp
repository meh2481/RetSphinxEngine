#include "ResourceCache.h"
#include "Image.h"
#include "Mesh3D.h"
#include "Font.h"
using namespace std;

ResourceCache::~ResourceCache()
{
	clear();
}

Image* ResourceCache::findImage(uint64_t id)
{
	map<uint64_t, Image*>::iterator i = imageIDMap.find(id);
	if(i == imageIDMap.end())	//This image isn't here
		return NULL;
	return i->second;
}

void ResourceCache::addImage(uint64_t id, Image* img)
{
	imageIDMap[id] = img;
}

void ResourceCache::clearImages()
{
	for(map<uint64_t, Image*>::iterator i = imageIDMap.begin(); i != imageIDMap.end(); i++)
		delete (i->second);
	imageIDMap.clear();
}

Mesh3D* ResourceCache::findMesh(uint64_t id)
{
	map<uint64_t, Mesh3D*>::iterator i = meshIDMap.find(id);
	if(i == meshIDMap.end())	//This mesh isn't here
		return NULL;
	return i->second;
}

Font* ResourceCache::findFont(uint64_t id)
{
	map<uint64_t, Font*>::iterator i = fontIDMap.find(id);
	if(i == fontIDMap.end())	//This font isn't here
		return NULL;
	return i->second;
}

void ResourceCache::addFont(uint64_t id, Font* font)
{
	fontIDMap[id] = font;
}

void ResourceCache::addMesh(uint64_t id, Mesh3D* mesh)
{
	meshIDMap[id] = mesh;
}

void ResourceCache::clearMeshes()
{
	for(map<uint64_t, Mesh3D*>::iterator i = meshIDMap.begin(); i != meshIDMap.end(); i++)
		delete (i->second);
	meshIDMap.clear();
}