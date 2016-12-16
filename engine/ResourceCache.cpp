#include "ResourceCache.h"
#include "Image.h"
#include "Mesh3D.h"
#include "ImgFont.h"
using namespace std;

ResourceCache::~ResourceCache()
{
	clear();
}

void ResourceCache::addImage(uint64_t id, Image* img)
{
	imageIDMap[id] = img;
}

void ResourceCache::addFont(uint64_t id, ImgFont* font)
{
	fontIDMap[id] = font;
}

void ResourceCache::addMesh(uint64_t id, Mesh3D* mesh)
{
	meshIDMap[id] = mesh;
}

//void ResourceCache::addCursor(uint64_t id, MouseCursor* cur)
//{
//	cursorIDMap[id] = cur;
//}

Image* ResourceCache::findImage(uint64_t id)
{
	map<uint64_t, Image*>::iterator i = imageIDMap.find(id);
	if(i == imageIDMap.end())	//This image isn't here
		return NULL;
	return i->second;
}

ImgFont* ResourceCache::findFont(uint64_t id)
{
	map<uint64_t, ImgFont*>::iterator i = fontIDMap.find(id);
	if(i == fontIDMap.end())	//This font isn't here
		return NULL;
	return i->second;
}

Mesh3D* ResourceCache::findMesh(uint64_t id)
{
	map<uint64_t, Mesh3D*>::iterator i = meshIDMap.find(id);
	if(i == meshIDMap.end())	//This mesh isn't here
		return NULL;
	return i->second;
}

//MouseCursor* ResourceCache::findCursor(uint64_t id)
//{
//	map<uint64_t, MouseCursor*>::iterator i = cursorIDMap.find(id);
//	if(i == cursorIDMap.end())	//This cursor isn't here
//		return NULL;
//	return i->second;
//}

std::string ResourceCache::findTextFile(uint64_t id)
{
	map<uint64_t, std::string>::iterator i = textIDMap.find(id);
	if(i == textIDMap.end())	//This mesh isn't here
		return std::string();
	return i->second;
}

void ResourceCache::addTextFile(uint64_t id, const std::string & str)
{
	textIDMap[id] = str;
}

void ResourceCache::clear()
{
	clearImages(); 
	clearMeshes();
	clearFonts();
	clearTextFiles();
//	clearCursors();
}

void ResourceCache::clearImages()
{
	for(map<uint64_t, Image*>::iterator i = imageIDMap.begin(); i != imageIDMap.end(); i++)
		delete (i->second);
	imageIDMap.clear();
}

void ResourceCache::clearFonts()
{
	for(map<uint64_t, ImgFont*>::iterator i = fontIDMap.begin(); i != fontIDMap.end(); i++)
		delete (i->second);
	fontIDMap.clear();
}

void ResourceCache::clearMeshes()
{
	for(map<uint64_t, Mesh3D*>::iterator i = meshIDMap.begin(); i != meshIDMap.end(); i++)
		delete (i->second);
	meshIDMap.clear();
}

void ResourceCache::clearTextFiles()
{
	textIDMap.clear();
}

//void ResourceCache::clearCursors()
//{
//	for(map<uint64_t, MouseCursor*>::iterator i = cursorIDMap.begin(); i != cursorIDMap.end(); i++)
//		delete (i->second);
//	cursorIDMap.clear();
//}
