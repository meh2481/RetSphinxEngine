#include "ResourceCache.h"
#include "Image.h"
#include "Mesh3D.h"

ResourceCache::~ResourceCache()
{
	clearImages();
	clearMeshes();
}

Image* ResourceCache::findImage(uint32_t id)
{
	map<uint32_t, Image*>::iterator i = imageIDMap.find(id);
	if(i == imageIDMap.end())	//This image isn't here
		return NULL;
	return i->second;
}

void ResourceCache::addImage(uint32_t id, Image* img)
{
	imageIDMap[id] = img;
}

void ResourceCache::clearImages()
{
	for(map<uint32_t, Image*>::iterator i = imageIDMap.begin(); i != imageIDMap.end(); i++)
		delete (i->second);
	imageIDMap.clear();
}

#ifdef _DEBUG
void ResourceCache::reloadImages()
{
	for(map<uint32_t, Image*>::iterator i = imageIDMap.begin(); i != imageIDMap.end(); i++)
		i->second->_reload();
}
#endif //_DEBUG


Mesh3D* ResourceCache::findMesh(uint32_t id)
{
	map<uint32_t, Mesh3D*>::iterator i = meshIDMap.find(id);
	if(i == meshIDMap.end())	//This image isn't here
		return NULL;
	return i->second;
}

void ResourceCache::addMesh(uint32_t id, Mesh3D* mesh)
{
	meshIDMap[id] = mesh;
}

void ResourceCache::clearMeshes()
{
	for(map<uint32_t, Mesh3D*>::iterator i = meshIDMap.begin(); i != meshIDMap.end(); i++)
		delete (i->second);
	meshIDMap.clear();
}

#ifdef _DEBUG
void ResourceCache::reloadMeshes()
{
	for(map<uint32_t, Mesh3D*>::iterator i = meshIDMap.begin(); i != meshIDMap.end(); i++)
		i->second->_reload();
}
#endif //_DEBUG
