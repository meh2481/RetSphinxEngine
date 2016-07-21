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

typedef struct
{
	uint64_t id;		//Resource ID
	uint64_t offset;	//Offset from start of file to CompressionHeader
} ResourcePtr;


//--------------------------------------------------------------
// Compressed data
//--------------------------------------------------------------
#define COMPRESSION_FLAGS_UNCOMPRESSED	0
#define COMPRESSION_FLAGS_WFLZ			1

typedef struct
{
	uint32_t compressionType;	//One of the compression flags above
	uint32_t compressedSize;
	uint32_t decompressedSize;
	uint32_t pad;
	//Followed by compressed data
} CompressionHeader;


//--------------------------------------------------------------
// Textures
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


//--------------------------------------------------------------
// Fonts
//--------------------------------------------------------------

typedef struct
{
	uint32_t numChars;
	uint64_t textureId;	//ID of texture resource to use
	uint32_t pad;
	//Followed by numChars uint32_t's (aka 32-bit UTF-8 codepoints), sorted from lowest to highest
	//Followed by numChars * 8 float32s (the rectangles for the characters)
} FontHeader;



