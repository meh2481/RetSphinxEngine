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
	std::string filename;
	uint32_t type;	//One of the CompressionHeader types
} CompressionHelper;

//main.cpp
void addHelper(const CompressionHelper& helper);
void compressHelper(CompressionHelper* helper, unsigned char* decompressed, unsigned int size, const std::string& filename);

//images.cpp
void addImage(const std::string& img);
void packImages(const std::string& filename);
