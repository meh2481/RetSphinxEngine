#pragma once
#include <map>

class Image;
class Mesh3D;
class Font;

class ResourceCache
{
	std::map<Image*, uint32_t> imageUses;
	std::map<uint64_t, Image*> imageIDMap;

	std::map<Mesh3D*, uint32_t> meshUses;
	std::map<uint64_t, Mesh3D*> meshIDMap;

	std::map<uint64_t, Font*> fontIDMap;

	void clearImages();
	void clearMeshes();

public:
	~ResourceCache();

	Image* findImage(uint64_t id);
	void addImage(uint64_t id, Image* img);

	Mesh3D* findMesh(uint64_t id);
	void addMesh(uint64_t id, Mesh3D* mesh);

	Font* findFont(uint64_t id);
	void addFont(uint64_t id, Font* font);

	void clear() { clearImages(); clearMeshes(); };
};