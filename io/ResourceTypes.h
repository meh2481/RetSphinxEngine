#pragma once
#include <stdlib.h>
#include <stdint.h>	//Use specific-sized ints for this

#define VERSION_1_0		1

//------------------------------------
// Header of the .pak file
//------------------------------------
typedef struct
{
	char sig[4];		//PAKC in big endian for current version
	uint32_t version;	//VERSION_1_0 for current version of the game
	uint32_t numResources;
	uint32_t pad;
	//Followed by numResources ResourcePtrs
} PakFileHeader;


//------------------------------------
// Pointer to resource within file
//------------------------------------

// Resource types
#define RESOURCE_TYPE_XML		1
#define RESOURCE_TYPE_MESH3D	2
#define RESOURCE_TYPE_IMAGE		3
#define RESOURCE_TYPE_LUASCRIPT	4
#define RESOURCE_TYPE_SHADER	5
#define RESOURCE_TYPE_SOUND		6

typedef struct
{
	uint32_t id;
	uint32_t type;		//One of the resource types above
	uint64_t offset;	//Offset from start of file to CompressionHeader
} ResourcePtr;


//--------------------------------------------------------------
// Compressed data
//--------------------------------------------------------------
#define COMPRESSION_FLAGS_UNCOMPRESSED	0
#define COMPRESSION_FLAGS_WFLZ			1

typedef struct
{
	uint32_t flags;	//One of the compression flags above
	uint32_t compressedSize;
	uint32_t decompressedSize;
	uint32_t pad;
	//Followed by compressed data
} CompressionHeader;


//--------------------------------------------------------------
// Textures - RESOURCE_TYPE_IMAGE
//--------------------------------------------------------------
#define TEXTURE_BPP_RGBA	32
#define TEXTURE_BPP_RGB		24

typedef struct
{
	uint32_t bpp; //One of the texture BPPs above
	uint32_t width;
	uint32_t height;
	uint32_t pad;
	//Followed by image data
} TextureHeader;


//TODO Move these to resource loader
unsigned char* decompressResource(unsigned char* resource, unsigned int* decompressedSize = NULL, unsigned int len = 0);
void freeResource(unsigned char* resource);
unsigned char* readFile(const char* filename, unsigned int* fileSize = NULL);	//TODO do whatever with this
