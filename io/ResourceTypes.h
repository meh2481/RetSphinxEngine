#pragma once
#include <stdlib.h>
#include <stdint.h>    //Use specific-sized ints for this

#define VERSION_1_0        1

#define f32_t float
#define f64_t double

//------------------------------------
// Header of the .pak file
//------------------------------------
typedef struct
{
    char sig[4];        //PAKC in big endian for current version
    uint32_t version;    //VERSION_1_0 for current version of the game
    uint32_t numResources;
    uint32_t pad;
    //Followed by numResources ResourcePtrs
} PakFileHeader;


//------------------------------------
// Pointer to resource within file
//------------------------------------

typedef struct
{
    uint64_t id;        //Resource ID
    uint64_t offset;    //Offset from start of file to CompressionHeader
} ResourcePtr;


//--------------------------------------------------------------
// Compressed data
//--------------------------------------------------------------
#define COMPRESSION_FLAGS_UNCOMPRESSED    0
#define COMPRESSION_FLAGS_WFLZ            1

typedef struct
{
    uint32_t compressionType;    //One of the compression flags above
    uint32_t compressedSize;
    uint32_t decompressedSize;
    uint32_t type;                //One of the resource types below
    //Followed by compressed data
} CompressionHeader;

#define RESOURCE_TYPE_UNKNOWN       0   //Default for don't care
#define RESOURCE_TYPE_IMAGE         1
#define RESOURCE_TYPE_IMAGE_ATLAS   2
#define RESOURCE_TYPE_SOUND         3
#define RESOURCE_TYPE_SOUND_LOOP    4
#define RESOURCE_TYPE_FONT          5
#define RESOURCE_TYPE_STRINGBANK    6
#define RESOURCE_TYPE_OBJ           7   //3D object (linking between a 3D mesh and a texture)
#define RESOURCE_TYPE_MESH          8   //3D mesh
#define RESOURCE_TYPE_JSON          9
#define RESOURCE_TYPE_XML           10  //Prolly wanna remove this at some point as we migrate away from xml formats
#define RESOURCE_TYPE_LUA           11  //Lua script
#define RESOURCE_TYPE_IMAGE_NO_ATLAS 12  //Icon image or other image without atlas
#define RESOURCE_TYPE_SHADER        13
//#define RESOURCE_TYPE_
//etc


//--------------------------------------------------------------
// Textures
//--------------------------------------------------------------
typedef struct //Structure for texture data
{
    uint16_t mode;        //One of the OpenGL texture modes
    uint8_t width;        //Width of image in power of 2
    uint8_t height;        //Height of image in power of 2
    //Followed by image data
} AtlasHeader;

typedef struct //Structure for image indices into the atlas AtlasHeader
{
    uint64_t atlasId;    //ID of AtlasHeader
    f32_t coordinates[8];    //UV texture coordinates for the image in the atlas
} TextureHeader;

typedef struct //Structure for (non-atlased) image data
{
    uint16_t bpp;           //bytes per pixel
    uint16_t width;         //Width of image
    uint16_t height;        //Height of image
    uint16_t pad;
                           //Followed by image data
} ImageHeader;


//--------------------------------------------------------------
// Fonts
//--------------------------------------------------------------

typedef struct
{
    uint32_t numChars;
    uint64_t textureId;    //ID of texture resource to use
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
    char languageID[4];    //ISO 639-1 (if existing) or 639-2 language code in all-lowercase (en for English, es for Spanish, etc). Should only be two or three chars, others '\0'
    uint32_t offset;    //Offset from first StringDataPointer for a string's ID to the StringDataPointer for this language

    //For example, for languages en=0 and es=1, for a StringID that's number 7 in the list (8th entry),
    //en's StringDataPointer is number 7*2+0=14 and the English string can be found at that pointer's offset
    //es's StringDataPointer is number 7*2+1=15 and the Spanish string can be found at that pointer's offset

} LanguageOffset;

typedef struct
{
    uint64_t id;        //ID of the string
} StringID;

typedef struct
{
    uint64_t offset;    //Offset from start of string data to the start of the actual, null-terminated string
} StringDataPointer;

//--------------------------------------------------------------
// Sound data
//--------------------------------------------------------------
//Song loop points
typedef struct
{
    uint32_t loopStartMsec;
    uint32_t loopEndMsec;
} SoundLoop;

typedef struct
{
    f32_t worldSize;
    uint32_t numGeometries;
    //Followed by numGeometries SoundGeom
} SoundGeomHeader;

typedef struct
{
    uint32_t size;
    //Followed by FMOD sound geometry data
    //uint64_t offset;
} SoundGeom;

//--------------------------------------------------------------
// Mesh data
//--------------------------------------------------------------
typedef struct
{
    uint32_t numVertices;
    //Followed by vertex data
    //Followed by texture coordinate data
    //Followed by normal data
} MeshHeader;

typedef struct
{
    uint64_t meshId;    //ID of MeshHeader
    uint64_t textureId;    //ID of TextureHeader
} Object3DHeader;

