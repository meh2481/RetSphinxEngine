//This program takes any file and outputs raw WFLZ-compressed data
#include "main.h"
#include "wfLZ.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <vector>
#include <sstream>
#include "StringUtils.h"
#include "FileOperations.h"
#include "tinyxml2.h"

#define PAD_32BIT 0x50444150
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

bool g_bImageOut;

typedef struct
{
	float left, right, top, bottom;
} cRect;

typedef struct
{
	uint64_t id;
	std::vector<const char*> strings;
} StringBankHelper;

//Helper functions ----------------------------------------

bool hasUpper(const std::string& s)
{
	for(unsigned int i = 0; i < s.length(); i++)
	{
		if(isupper(s.at(i)))
			return true;
	}
	return false;
}

bool compare_stringID(const StringBankHelper& first, const StringBankHelper& second)
{
	return (first.id < second.id);
}

uint32_t parseTime(const char* cStr)	//Parse time in the form mm:ss.sss
{
	if(!cStr)
		return 0;

	std::string s = cStr;

	size_t colonPos = s.find_first_of(':');
	std::string minutes = s.substr(0, colonPos);
	std::string seconds = s.substr(colonPos + 1);

	uint32_t m = atoi(minutes.c_str());
	double sec = atof(seconds.c_str());

	uint32_t ret = sec * 1000;
	ret += m * 60 * 1000;

	std::cout << "Converting " << s << " to " << ret << " msec" << std::endl;

	return ret;
}

unsigned char* extractStringbank(const std::string& sFilename, unsigned int* fileSize)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLError err = doc->LoadFile(sFilename.c_str());
	if(err != tinyxml2::XML_NO_ERROR)
	{
		std::cout << "Unable to open font XML " << sFilename << std::endl;
		delete doc;
		return NULL;
	}

	tinyxml2::XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		std::cout << "Unable to find root element in XML " << sFilename << std::endl;
		delete doc;
		return NULL;
	}
	tinyxml2::XMLElement* languages = root->FirstChildElement("languages");
	tinyxml2::XMLElement* strings = root->FirstChildElement("strings");
	if(strings == NULL || languages == NULL)
	{
		std::cout << "Unable to find languages and strings elements in XML " << sFilename << std::endl;
		delete doc;
		return NULL;
	}

	//Read in languages and offsets
	std::list<LanguageOffset> offsets;
	uint32_t off = 0;
	for(tinyxml2::XMLElement* language = languages->FirstChildElement("language"); language != NULL; language = language->NextSiblingElement())
	{
		LanguageOffset offset;
		memset(offset.languageID, 0, 4);	//Fill this language ID with '\0' to start
		const char* langCode = language->Attribute("code");
		assert(langCode != NULL);
		assert(strlen(langCode) < 4);		//Language codes can't be more than 3 characters
		assert(!hasUpper(langCode));		//Language codes should be all lowercase
		strcpy(offset.languageID, langCode);
		offset.offset = off++;
		offsets.push_back(offset);
	}

	//Read in actual strings
	std::list<StringBankHelper> sbHelpers;
	for(tinyxml2::XMLElement* stringid = strings->FirstChildElement("string"); stringid != NULL; stringid = stringid->NextSiblingElement())
	{
		StringBankHelper sbHelper;
		const char* idStr = stringid->Attribute("id");
		assert(idStr != NULL);
		sbHelper.id = Hash::hash(idStr);
		for(tinyxml2::XMLElement* locString = stringid->FirstChildElement("loc"); locString != NULL; locString = locString->NextSiblingElement())
		{
			const char* loc = locString->Attribute("text");
			assert(loc != NULL);
			sbHelper.strings.push_back(loc);
		}
		assert(sbHelper.strings.size() == offsets.size());	//Shouldn't be missing any translations for any strings
		sbHelpers.push_back(sbHelper);
	}

	//Sort by ID
	sbHelpers.sort(compare_stringID);

	//Calculate final file size
	*fileSize = sizeof(StringBankHeader);
	*fileSize += offsets.size() * sizeof(LanguageOffset);
	*fileSize += sbHelpers.size() * sizeof(StringID);
	*fileSize += sbHelpers.size() * offsets.size() * sizeof(StringDataPointer);
	for(std::list<StringBankHelper>::iterator i = sbHelpers.begin(); i != sbHelpers.end(); i++)	//Add in length of all strings
	{
		for(std::vector<const char*>::iterator j = i->strings.begin(); j != i->strings.end(); j++)
		{
			*fileSize += strlen(*j) + 1;	//Each string will have an additional null char appended
		}
	}

	//Allocate memory
	unsigned char* buf = (unsigned char*)malloc(*fileSize);

	//Write header
	StringBankHeader header;
	header.numLanguages = offsets.size();
	header.numStrings = sbHelpers.size();
	memcpy(buf, &header, sizeof(StringBankHeader));
	uint64_t offset = sizeof(StringBankHeader);

	//Write LanguageOffsets
	for(std::list<LanguageOffset>::iterator i = offsets.begin(); i != offsets.end(); i++)
	{
		memcpy(&buf[offset], &(*i), sizeof(LanguageOffset));
		offset += sizeof(LanguageOffset);
	}

	//Write StringIDs
	for(std::list<StringBankHelper>::iterator i = sbHelpers.begin(); i != sbHelpers.end(); i++)
	{
		memcpy(&buf[offset], &(i->id), sizeof(StringID));
		offset += sizeof(StringID);
	}

	//Write string pointers
	uint64_t stringWriteOffset = offset;	//Offset we're writing the strings to 
	uint64_t actualStringOffset = 0;		//Offset from start of string data to the pointer we're currently writing
	stringWriteOffset += sizeof(StringDataPointer)*header.numLanguages*header.numStrings;
	for(std::list<StringBankHelper>::iterator i = sbHelpers.begin(); i != sbHelpers.end(); i++)
	{
		for(std::vector<const char*>::iterator j = i->strings.begin(); j != i->strings.end(); j++)
		{
			//Write the pointer
			memcpy(&buf[offset], &actualStringOffset, sizeof(StringDataPointer));
			offset += sizeof(StringDataPointer);

			//Write the string data
			uint32_t stringLen = strlen(*j)+1;	//Write '\0' char also
			memcpy(&buf[stringWriteOffset], *j, stringLen);

			stringWriteOffset += stringLen;
			actualStringOffset += stringLen;
		}
	}

	//Done
	delete doc;
	return buf;
}

unsigned char* extractSoundLoop(const std::string& sFilename, unsigned int* fileSize)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLError err = doc->LoadFile(sFilename.c_str());
	if(err != tinyxml2::XML_NO_ERROR)
	{
		std::cout << "Unable to open song loop XML " << sFilename << std::endl;
		delete doc;
		return NULL;
	}

	tinyxml2::XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		std::cout << "Unable to find root element in XML " << sFilename << std::endl;
		delete doc;
		return NULL;
	}

	*fileSize = sizeof(SoundLoop);
	SoundLoop* sl = (SoundLoop*)malloc(*fileSize);

	sl->loopStartMsec = parseTime(root->Attribute("start"));
	sl->loopEndMsec = parseTime(root->Attribute("end"));

	delete doc;
	return (unsigned char*)sl;
}

cRect rectFromString(const std::string& in)
{
	cRect rc;
	std::string s = StringUtils::stripCommas(in);

	//Now, parse
	std::istringstream iss(s);
	if(!(iss >> rc.left >> rc.top >> rc.right >> rc.bottom))
		rc.left = rc.top = rc.right = rc.bottom = 0.0f;

	return rc;
}

std::string remove_extension(const std::string& filename)
{
	size_t lastdot = filename.find_last_of(".");
	if(lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

uint32_t getCodepoint(const char* str)
{
	//Determine UTF-8 codepoint
	int seqlen = 0;
	uint8_t pointHeader = *((uint8_t*)str);
	//Determine how many characters in this UTF-8 codepoint
	while(pointHeader & 0x80)
	{
		seqlen++;
		pointHeader <<= 1;
	}
	//Build codepoint by combining bits
	uint32_t codepoint = pointHeader >> seqlen;
	for(int i = 1; i < seqlen; i++)
	{
		codepoint <<= 6;
		str++;
		codepoint |= (*str & 0x3F);
	}
	return codepoint;
}

unsigned char* extractImage(const std::string& filename, unsigned int* fileSize)
{
	int comp = 0;
	int width = 0;
	int height = 0;
	unsigned char* imageBuf = stbi_load(filename.c_str(), &width, &height, &comp, 0);

	if(!imageBuf || width < 1 || height < 1)
	{
		std::cout << "Unable to load image " << filename << std::endl;
		return NULL;
	}

	TextureHeader textureHeader;
	textureHeader.width = width;
	textureHeader.height = height;
	textureHeader.bpp = TEXTURE_BPP_RGBA;
	if(comp == STBI_rgb)
		textureHeader.bpp = TEXTURE_BPP_RGB;

	int size = sizeof(TextureHeader) + textureHeader.width*textureHeader.height*textureHeader.bpp / 8;

	if(fileSize)
		*fileSize = size;

	unsigned char* finalBuf = (unsigned char*)malloc(size);

	memcpy(finalBuf, &textureHeader, sizeof(TextureHeader));
	memcpy(&finalBuf[sizeof(TextureHeader)], imageBuf, size - sizeof(TextureHeader));

	stbi_image_free(imageBuf);
	return finalBuf;
}

unsigned char* extractFont(const std::string& filename, unsigned int* fileSize)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLError err = doc->LoadFile(filename.c_str());
	if(err != tinyxml2::XML_NO_ERROR)
	{
		std::cout << "Unable to open font XML " << filename << std::endl;
		delete doc;
		return NULL;
	}

	tinyxml2::XMLElement* root = doc->RootElement();

	const char* imgFileName = root->Attribute("img");
	if(!imgFileName)
	{
		std::cout << "No img for font XML " << filename << std::endl;
		delete doc;
		return NULL;
	}

	//Load image just to get width/height
	int comp = 0;
	int imgWidth = 0;
	int imgHeight = 0;
	unsigned char* imageBuf = stbi_load(imgFileName, &imgWidth, &imgHeight, &comp, 0);
	imgWidth--;
	imgHeight--;
	if(!imageBuf || imgWidth < 1 || imgHeight < 1)
	{
		std::cout << "Unable to load image " << imgFileName << " for font " << filename << std::endl;
		return NULL;
	}
	stbi_image_free(imageBuf);

	//Create font header
	FontHeader fontHeader;
	fontHeader.pad = PAD_32BIT;
	fontHeader.textureId = Hash::hash(imgFileName);
	fontHeader.numChars = 1;

	std::vector<uint32_t> codepoints;
	std::vector<cRect> imgRects;

	//First codepoint (0) is always 0,0,0,0
	codepoints.push_back(0);
	imgRects.push_back({ 0,0,0,0 });

	for(tinyxml2::XMLElement* elem = root->FirstChildElement("char"); elem != NULL; elem = elem->NextSiblingElement())
	{
		const char* codepointStr = elem->Attribute("codepoint");
		if(!codepointStr)
		{
			std::cout << "Skipping char missing codepoint " << fontHeader.numChars << std::endl;
			continue;
		}
		uint32_t codepoint = getCodepoint(codepointStr);

		const char* imgRectStr = elem->Attribute("rect");
		if(!imgRectStr)
		{
			std::cout << "Skipping char missing rect " << fontHeader.numChars << std::endl;
			continue;
		}

		cRect rc = rectFromString(imgRectStr);
		rc.left = rc.left / (float)imgWidth;
		rc.right = rc.right / (float)imgWidth;
		rc.top = rc.top / (float)imgHeight;
		rc.bottom = rc.bottom / (float)imgHeight;
		//cout << "Image rect for " << codepointStr << ": " << rc.left << ", " << rc.right << ", " << rc.top << ", " << rc.bottom << endl;

		imgRects.push_back(rc);
		codepoints.push_back(codepoint);
		fontHeader.numChars++;
	}
	delete doc;


	unsigned char* fontBuf = (unsigned char*)malloc(sizeof(FontHeader) + fontHeader.numChars * sizeof(uint32_t) + fontHeader.numChars * 8 * sizeof(float));

	memcpy(fontBuf, &fontHeader, sizeof(FontHeader));
	uint32_t pos = sizeof(FontHeader);

	memcpy(&fontBuf[pos], codepoints.data(), sizeof(uint32_t)*fontHeader.numChars);
	pos += sizeof(uint32_t)*fontHeader.numChars;

	for(std::vector<cRect>::iterator i = imgRects.begin(); i != imgRects.end(); i++)
	{
		//CW winding from upper left
		const float texCoords[] =
		{
			i->left, i->top, // upper left
			i->right, i->top, // upper right
			i->right, i->bottom, // lower right
			i->left, i->bottom, // lower left
		};

		memcpy(&fontBuf[pos], texCoords, sizeof(float) * 8);
		pos += sizeof(float) * 8;
	}
	if(fileSize)
		*fileSize = pos;
	return fontBuf;
}

std::list<std::string> readFilenames(const std::string& filelistFile)
{
	std::list<std::string> lFilenames;
	std::ifstream infile(filelistFile.c_str());
	if(infile.fail())
	{
		std::cout << "Cannot open " << filelistFile << ". Skipping..." << std::endl;
		return lFilenames;
	}

	while(!infile.fail() && !infile.eof())	//Pull in all the lines out of this file
	{
		std::string s;
		std::getline(infile, s);
		s = StringUtils::trim(s);
		if(!s.length())
			continue;	//Ignore blank lines
		if(s.find('#') != std::string::npos)
			continue;	//Ignore comment lines
		if(s.find(' ') != std::string::npos)
		{
			std::cout << "Ignoring file \"" << s << "\" because it contains spaces." << std::endl;
			continue;	//Ignore lines with spaces
		}
		if(hasUpper(s))
		{
			std::cout << "Ignoring file \"" << s << "\" because it contains uppercase characters." << std::endl;
			continue;
		}
		lFilenames.push_back(s);	//Add this to our list of files to package
	}
	infile.close();
	return lFilenames;
}

static uint8_t* workMem;
void createCompressionHelper(CompressionHelper* helper, unsigned char* decompressed, unsigned int size)
{
	helper->header.compressionType = COMPRESSION_FLAGS_WFLZ;
	helper->header.decompressedSize = size;
	uint8_t* compressed = (uint8_t*)malloc(wfLZ_GetMaxCompressedSize(helper->header.decompressedSize));
	helper->header.compressedSize = wfLZ_CompressFast(decompressed, size, compressed, workMem, 0);

	//See if compression made the file larger
	if(helper->header.compressedSize >= helper->header.decompressedSize)
	{
		//It did; use the uncompressed data instead
		helper->header.compressionType = COMPRESSION_FLAGS_UNCOMPRESSED;
		helper->header.compressedSize = helper->header.decompressedSize;
		helper->data = decompressed;
		free(compressed);
	}
	else
	{
		//It didn't; use the compressed data
		helper->data = compressed;
		free(decompressed);
	}

	//Add this compression helper to our list
	helper->size = helper->header.compressedSize;
	//helper->id = Hash::hash(filename.c_str());
}

static std::list<CompressionHelper> compressedFiles;
void addHelper(const CompressionHelper& helper)
{
	compressedFiles.push_back(helper);
}

void compress(std::list<std::string> filesToPak, const std::string& in)
{
	compressedFiles.clear();

	std::string pakFilename = remove_extension(in);
	pakFilename += ".pak";
	std::cout << "Packing pak file \"" << pakFilename << "\"..." << std::endl;

	for(std::list<std::string>::iterator i = filesToPak.begin(); i != filesToPak.end(); i++)
	{
		std::cout << "Compressing \"" << *i << "\"..." << std::endl;
		unsigned int size = 0;
		unsigned char* decompressed;

		CompressionHelper helper;
		helper.header.type = RESOURCE_TYPE_UNKNOWN;	//Default unknown type
		helper.id = Hash::hash(i->c_str());

		//Package these file types properly if needed
		if(i->find(".png") != std::string::npos)
		{
			addImage(*i);	//TODO: Exclude 3D model textures, or re-map coords at pack time somehow
			continue;		//TODO: Skip this, as we'll add the image/atlas later
			//decompressed = extractImage(*i, &size);	
			//helper.header.type = RESOURCE_TYPE_IMAGE;
		}
		else if(i->find(".font") != std::string::npos)
		{
			decompressed = extractFont(*i, &size);
			helper.header.type = RESOURCE_TYPE_FONT;
		}
		else if(i->find("stringbank.xml") != std::string::npos)
		{
			decompressed = extractStringbank(*i, &size);
			helper.header.type = RESOURCE_TYPE_STRINGBANK;
		}
		else if(i->find(".loop") != std::string::npos)
		{
			decompressed = extractSoundLoop(*i, &size);
			helper.header.type = RESOURCE_TYPE_SOUND_LOOP;
		}
		else if(i->find(".tiny3d") != std::string::npos)
		{
			//TODO: A 3D model is the linking between a mesh ID and a texture ID
			helper.header.type = RESOURCE_TYPE_OBJ;
			decompressed = FileOperations::readFile(*i, &size);
		}
		else if(i->find(".json") != std::string::npos)
		{
			helper.header.type = RESOURCE_TYPE_JSON;
			decompressed = FileOperations::readFile(*i, &size);
		}
		else if(i->find(".xml") != std::string::npos)
		{
			helper.header.type = RESOURCE_TYPE_XML;
			decompressed = FileOperations::readFile(*i, &size);
		}
		else
		{
			decompressed = FileOperations::readFile(*i, &size);	//Store as-is
			std::cout << "Warning: unknown resource type for file " << *i << std::endl;
		}

		if(!size || !decompressed)
		{
			std::cout << "Unable to load file " << *i << std::endl;
			continue;
		}

		createCompressionHelper(&helper, decompressed, size);
		addHelper(helper);
	}

	//Now that we have all images, pack any atlases
	packImages(in);

	//Open output file
	FILE *fOut = fopen(pakFilename.c_str(), "wb");

	//Create header
	PakFileHeader fileHeader;
	fileHeader.sig[0] = 'P';
	fileHeader.sig[1] = 'A';
	fileHeader.sig[2] = 'K';
	fileHeader.sig[3] = 'C';
	fileHeader.version = VERSION_1_0;
	fileHeader.numResources = compressedFiles.size();
	fileHeader.pad = PAD_32BIT;

	//Write header
	fwrite(&fileHeader, 1, sizeof(PakFileHeader), fOut);

	//Make sure we're doing 64-bit math on the current offset
	uint64_t curOffset = (uint64_t)sizeof(PakFileHeader) + (uint64_t)compressedFiles.size() * (uint64_t)sizeof(ResourcePtr);	//Start after both these

	//Write resource pointers
	for(std::list<CompressionHelper>::iterator i = compressedFiles.begin(); i != compressedFiles.end(); i++)
	{
		ResourcePtr resPtr;
		resPtr.id = i->id;
		resPtr.offset = curOffset;

		fwrite(&resPtr, 1, sizeof(ResourcePtr), fOut);
		curOffset += sizeof(CompressionHeader) + i->header.compressedSize;
	}

	//Write resource data
	for(std::list<CompressionHelper>::iterator i = compressedFiles.begin(); i != compressedFiles.end(); i++)
	{
		fwrite(&i->header, 1, sizeof(CompressionHeader), fOut);
		fwrite(i->data, 1, i->size, fOut);

		free(i->data);	//Free image data while we're at it
	}

	//Close output file
	fclose(fOut);
}

int main(int argc, char** argv)
{
	g_bImageOut = false;
	workMem = (uint8_t*)malloc(wfLZ_GetWorkMemSize());
	std::list<std::string> sFilelistNames;

	//Parse commandline
	for(int i = 1; i < argc; i++)
	{
		std::string s = argv[i];
		if(s == "--img")
			g_bImageOut = true;
		else
			sFilelistNames.push_back(s);
	}

	//Compress files
	for(std::list<std::string>::iterator i = sFilelistNames.begin(); i != sFilelistNames.end(); i++)
		compress(readFilenames(*i), *i);

	//Free our WFLZ working memory
	free(workMem);
	return 0;
}
