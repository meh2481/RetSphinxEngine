#pragma once
#include <map>
using namespace std;

class Image;

class ResourceCache
{
	map<Image*, int> imageUses;
	map<int, Image*> imageIDMap;

	void clearImages();

public:
	~ResourceCache();

	Image* findImage(int id);
	void addImage(int id, Image* img);

#ifdef _DEBUG
	//TODO See if we even need this or not; shouldn't outside of F5 reloading
	void reloadImages();
#endif //_DEBUG
};