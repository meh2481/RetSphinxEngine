#pragma once
#include <map>
using namespace std;

class Image;
class Mesh3D;

class ResourceCache
{
	map<Image*, uint32_t> imageUses;
	map<uint32_t, Image*> imageIDMap;

	map<Mesh3D*, uint32_t> meshUses;
	map<uint32_t, Mesh3D*> meshIDMap;

	void clearImages();
	void clearMeshes();

public:
	~ResourceCache();

	Image* findImage(uint32_t id);
	void addImage(uint32_t id, Image* img);

	Mesh3D* findMesh(uint32_t id);
	void addMesh(uint32_t id, Mesh3D* mesh);

#ifdef _DEBUG
	void reloadImages();
	void reloadMeshes();
#endif //_DEBUG
};