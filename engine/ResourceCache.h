#pragma once
#include <map>
#include <string>

class ResourceCache
{
	std::map<uint64_t, void*> map;

public:
	~ResourceCache();

	void* find(uint64_t id);
	void add(uint64_t id, void* item);

	void clear();
};
