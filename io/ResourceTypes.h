#pragma once
#include <stdlib.h>

unsigned char* readFile(const char* filename, unsigned int* fileSize = NULL);
//--------------------------------------------------------------
//Compressed data
//--------------------------------------------------------------
#define COMPRESSION_FLAGS_WFLZ	1

typedef struct
{
	unsigned int flags;
	unsigned int compressedSize;
	unsigned int decompressedSize;
} CompressionHeader;


unsigned char* decompressResource(unsigned char* resource, unsigned int* decompressedSize = NULL, unsigned int len = 0);
void freeResource(unsigned char* resource);

//--------------------------------------------------------------
//Textures
//--------------------------------------------------------------
#define TEXTURE_BPP_RGBA	32
#define TEXTURE_BPP_RGB		24

typedef struct
{
	unsigned int bpp;
	unsigned int width;
	unsigned int height;
} TextureHeader;
