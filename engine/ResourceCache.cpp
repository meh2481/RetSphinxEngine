#include "ResourceCache.h"
#include "Object3D.h"
#include "ImgFont.h"
#include <cstdlib>

ResourceCache::~ResourceCache()
{
    clear();
}

void ResourceCache::add(uint64_t id, void* item, unsigned int length)
{
    ResourceItem it;
    it.data = item;
    it.length = length;
    map[id] = it;
}

void* ResourceCache::find(uint64_t id, unsigned int* length)
{
    auto i = map.find(id);
    if(i == map.end())    //This image isn't here
        return NULL;
    if(length != NULL)
        *length = i->second.length;
    return i->second.data;
}

void ResourceCache::clear()
{
    for(auto i = map.begin(); i != map.end(); i++)
        free(i->second.data);
    map.clear();
}
