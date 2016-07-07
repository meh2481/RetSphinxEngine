#include "ResourceCache.h"
#include "Image.h"

ResourceCache::~ResourceCache()
{
	clearImages();
}

Image* ResourceCache::findImage(int id)
{
	map<int, Image*>::iterator i = imageIDMap.find(id);
	if(i == imageIDMap.end())	//This image isn't here
		return NULL;
	return i->second;
}

void ResourceCache::addImage(int id, Image* img)
{
	imageIDMap[id] = img;
}

void ResourceCache::clearImages()
{
	for(map<int, Image*>::iterator i = imageIDMap.begin(); i != imageIDMap.end(); i++)
		delete (i->second);
	imageIDMap.clear();
}

#ifdef _DEBUG
void ResourceCache::reloadImages()
{
	for(map<int, Image*>::iterator i = imageIDMap.begin(); i != imageIDMap.end(); i++)
		i->second->_reload();
}
#endif //_DEBUG
