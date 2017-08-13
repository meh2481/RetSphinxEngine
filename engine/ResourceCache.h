#pragma once
#include <map>
#include <string>

//TODO: Move this inside ResourceLoader or something, since this is just a map
class ResourceCache
{
	std::map<uint64_t, void*> map;

public:
	~ResourceCache();

	void* find(uint64_t id);
	void add(uint64_t id, void* item);

	void clear();
};
