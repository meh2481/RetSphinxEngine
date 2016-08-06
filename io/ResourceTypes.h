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
	//Followed by numChars * 8 floats (the UV coordinate rectangles for the characters, each float in range [0..1])
} FontHeader;

//--------------------------------------------------------------
// Text data
//--------------------------------------------------------------
//String bank file format
typedef struct
{
	uint32_t numStrings;
	uint32_t numLanguages;
	//Followed by numLangages LanguageOffsets
	//Followed by numStrings StringIDs (sorted from least to greatest)
	//Followed by numStrings*numLanguages StringDataPointers
	//Followed by actual string data
} StringBankHeader;

//See https://www.loc.gov/standards/iso639-2/php/code_list.php for ISO 639 codes
typedef struct
{
	char languageID[4];	//ISO 639-1 (if existing) or 639-2 language code in all-caps (EN for English, ES for Spanish, etc). Should only be two or three chars, others '\0'
	uint32_t offset;	//Offset from first StringDataPointer for a string's ID to the StringDataPointer for this language

//For example, for languages EN=0 and ES=1, for a StringID that's number 7 in the list (8th entry), 
	//EN's StringDataPointer is number 7*2+0=14 and the English string can be found at that pointer's offset
	//ES's StringDataPointer is number 7*2+1=15 and the Spanish string can be found at that pointer's offset

} LanguageOffset;

typedef struct
{
	uint64_t id;		//ID of the string
} StringID;

typedef struct
{
	uint64_t offset;	//Offset from start of string data to the start of the actual, null-terminated string
} StringDataPointer;

