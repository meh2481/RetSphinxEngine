#pragma once
#include <map>

class Image;
class Mesh3D;
class Font;
class MouseCursor;

class ResourceCache
{
	//std::map<Image*, uint32_t> imageUses;
	std::map<uint64_t, Image*> imageIDMap;

	//std::map<Mesh3D*, uint32_t> meshUses;
	std::map<uint64_t, Mesh3D*> meshIDMap;

	std::map<uint64_t, Font*> fontIDMap;

	std::map<uint64_t, MouseCursor*> cursorIDMap;

	void clearImages();
	void clearFonts();
	void clearMeshes();
	void clearCursors();

public:
	~ResourceCache();

	Image* findImage(uint64_t id);
	void addImage(uint64_t id, Image* img);

	Mesh3D* findMesh(uint64_t id);
	void addMesh(uint64_t id, Mesh3D* mesh);

	Font* findFont(uint64_t id);
	void addFont(uint64_t id, Font* font);

	MouseCursor* findCursor(uint64_t id);
	void addCursor(uint64_t id, MouseCursor* cur);

	void clear();
};