#pragma once
#include <string>
#include "ResourceTypes.h"
#include "Hash.h"

//Helper struct for compression
typedef struct
{
    CompressionHeader header;
    uint8_t* data;
    unsigned int size;
    uint64_t id;
    uint32_t type;    //One of the CompressionHeader types
} CompressionHelper;

//main.cpp
void addHelper(const CompressionHelper& helper);
void createCompressionHelper(CompressionHelper* helper, unsigned char* decompressed, unsigned int size);

//images.cpp
void addImage(const std::string& img);
void packImages(const std::string& filename);
unsigned char* extractImage(const std::string& filename, unsigned int* size);

//mesh.cpp
unsigned char* extractMesh(const std::string& objFilename, unsigned int* size);
unsigned char* extract3dObject(const std::string& xmlFilename, unsigned int* size);
