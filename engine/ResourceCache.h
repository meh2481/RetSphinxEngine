#pragma once
#include <map>

class Image;
class Mesh3D;

class ResourceCache
{
	std::map<Image*, uint32_t> imageUses;
	std::map<uint64_t, Image*> imageIDMap;

	std::map<Mesh3D*, uint32_t> meshUses;
	std::map<uint64_t, Mesh3D*> meshIDMap;

	void clearImages();
	void clearMeshes();

public:
	~ResourceCache();

	Image* findImage(uint64_t id);
	void addImage(uint64_t id, Image* img);

	Mesh3D* findMesh(uint64_t id);
	void addMesh(uint64_t id, Mesh3D* mesh);

	void clear() { clearImages(); clearMeshes(); };
};