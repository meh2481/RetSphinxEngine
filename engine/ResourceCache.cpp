#include "ResourceCache.h"
#include "Mesh3D.h"
#include "ImgFont.h"

ResourceCache::~ResourceCache()
{
	clear();
}

void ResourceCache::add(uint64_t id, void* item)
{
	map[id] = item;
}

void* ResourceCache::find(uint64_t id)
{
	std::map<uint64_t, void*>::iterator i = map.find(id);
	if(i == map.end())	//This image isn't here
		return NULL;
	return i->second;
}
void ResourceCache::clear()
{
	for(std::map<uint64_t, void*>::iterator i = map.begin(); i != map.end(); i++)
		delete (i->second);
	map.clear();
}
