#pragma once
#include <map>
#include <string>
#include <stdint.h>

typedef struct
{
    void* data;
    unsigned int length;
} ResourceItem;

class ResourceCache
{
    std::map<uint64_t, ResourceItem> map;

public:
    ~ResourceCache();

    void* find(uint64_t id, unsigned int* length = NULL);
    void add(uint64_t id, void* item, unsigned int length);

    void clear();
};
