#pragma once
#include <map>
#include <string>
#include "Quad.h"

class Mesh3D;
class ImgFont;

class ResourceCache
{
	//std::map<Image*, uint32_t> imageUses;
	std::map<uint64_t, Img*> imageIDMap;

	//std::map<Mesh3D*, uint32_t> meshUses;
	std::map<uint64_t, Mesh3D*> meshIDMap;

	std::map<uint64_t, ImgFont*> fontIDMap;

	std::map<uint64_t, std::string> textIDMap;

	std::map<uint64_t, TextureHandle*> textureIDMap;

	//std::map<uint64_t, MouseCursor*> cursorIDMap;

	void clearImages();
	void clearFonts();
	void clearMeshes();
	void clearTextFiles();
	void clearTextures();
	//void clearCursors();

public:
	~ResourceCache();

	Img* findImage(uint64_t id);
	void addImage(uint64_t id, Img* img);

	Mesh3D* findMesh(uint64_t id);
	void addMesh(uint64_t id, Mesh3D* mesh);

	ImgFont* findFont(uint64_t id);
	void addFont(uint64_t id, ImgFont* font);

	//MouseCursor* findCursor(uint64_t id);
	//void addCursor(uint64_t id, MouseCursor* cur);

	std::string findTextFile(uint64_t id);
	void addTextFile(uint64_t id, const std::string& str);

	TextureHandle* findTexture(uint64_t id);
	void addTexture(uint64_t id, TextureHandle* handle);

	void clear();
};
