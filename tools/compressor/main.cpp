//This program takes any file and outputs raw WFLZ-compressed data
#include "wfLZ.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <vector>
#include <sstream>
#include "Parse.h"
#include "FileOperations.h"
#include "tinyxml2.h"
#include "Hash.h"
using namespace std;

#define PAD_32BIT 0x50444150
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "ResourceTypes.h"

//Helper struct for compression
typedef struct
{
	CompressionHeader header;
	uint8_t* data;
	unsigned int size;
	uint64_t id;
	string filename;
} compressionHelper;

typedef struct
{
	float left, right, top, bottom;
}cRect;

cRect rectFromString(string s)
{
	cRect rc;
	s = Parse::stripCommas(s);

	//Now, parse
	istringstream iss(s);
	if(!(iss >> rc.left >> rc.top >> rc.right >> rc.bottom))
		rc.left = rc.top = rc.right = rc.bottom = 0.0f;

	return rc;
}

string remove_extension(const string& filename)
{
	size_t lastdot = filename.find_last_of(".");
	if(lastdot == string::npos) return filename;
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

unsigned char* extractImage(string filename, unsigned int* fileSize)
{
	int comp = 0;
	int width = 0;
	int height = 0;
	unsigned char* imageBuf = stbi_load(filename.c_str(), &width, &height, &comp, 0);

	if(!imageBuf || width < 1 || height < 1)
	{
		cout << "Unable to load image " << filename << endl;
		return NULL;
	}

	TextureHeader textureHeader;
	textureHeader.width = width;
	textureHeader.height = height;
	textureHeader.bpp = TEXTURE_BPP_RGBA;
	if(comp == 3)
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

unsigned char* extractFont(string filename, unsigned int* fileSize)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLError err = doc->LoadFile(filename.c_str());
	if(err != tinyxml2::XML_NO_ERROR)
	{
		cout << "Unable to open font XML " << filename << endl;
		delete doc;
		return NULL;
	}

	tinyxml2::XMLElement* root = doc->RootElement();

	const char* imgFileName = root->Attribute("img");
	if(!imgFileName)
	{
		cout << "No img for font XML " << filename << endl;
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
		cout << "Unable to load image " << imgFileName << " for font " << filename << endl;
		return NULL;
	}
	stbi_image_free(imageBuf);

	//Create font header
	FontHeader fontHeader;
	fontHeader.pad = PAD_32BIT;
	fontHeader.textureId = Hash::hash(imgFileName);
	fontHeader.numChars = 1;

	vector<uint32_t> codepoints;
	vector<cRect> imgRects;

	//First codepoint (0) is always 0,0,0,0
	codepoints.push_back(0);
	imgRects.push_back({ 0,0,0,0 });

	for(tinyxml2::XMLElement* elem = root->FirstChildElement("char"); elem != NULL; elem = elem->NextSiblingElement())
	{
		const char* codepointStr = elem->Attribute("codepoint");
		if(!codepointStr)
		{
			cout << "Skipping char missing codepoint " << fontHeader.numChars << endl;
			continue;
		}
		uint32_t codepoint = getCodepoint(codepointStr);

		const char* imgRectStr = elem->Attribute("rect");
		if(!imgRectStr)
		{
			cout << "Skipping char missing rect " << fontHeader.numChars << endl;
			continue;
		}

		cRect rc = rectFromString(imgRectStr);
		rc.left = rc.left / (float)imgWidth;
		rc.right = rc.right / (float)imgWidth;
		rc.top = rc.top / (float)imgHeight;
		rc.bottom = rc.bottom / (float)imgHeight;
		cout << "Image rect for " << codepointStr << ": " << rc.left << ", " << rc.right << ", " << rc.top << ", " << rc.bottom << endl;

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

	for(vector<cRect>::iterator i = imgRects.begin(); i != imgRects.end(); i++)
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

bool hasUpper(string s)
{
	for(unsigned int i = 0; i < s.length(); i++)
	{
		if(isupper(s.at(i)))
			return true;
	}
	return false;
}

list<string> readFilenames(string filelistFile)
{
	list<string> lFilenames;
	ifstream infile(filelistFile.c_str());
	if(infile.fail())
	{
		cout << "Cannot open " << filelistFile << ". Skipping..." << endl;
		return lFilenames;
	}

	while(!infile.fail() && !infile.eof())	//Pull in all the lines out of this file
	{
		string s;
		getline(infile, s);
		s = Parse::trim(s);
		if(!s.length())
			continue;	//Ignore blank lines
		if(s.find('#') != string::npos)
			continue;	//Ignore comment lines
		if(s.find(' ') != string::npos)
		{
			cout << "Ignoring file \"" << s << "\" because it contains spaces." << endl;
			continue;	//Ignore lines with spaces
		}
		if(hasUpper(s))
		{
			cout << "Ignoring file \"" << s << "\" because it contains uppercase characters." << endl;
			continue;
		}
		lFilenames.push_back(s);	//Add this to our list of files to package
	}
	infile.close();
	return lFilenames;
}

void compress(list<string> filesToPak, string pakFilename)
{
	pakFilename = remove_extension(pakFilename);
	pakFilename += ".pak";
	cout << "Packing pak file \"" << pakFilename << "\"..." << endl;

	uint8_t* workMem = (uint8_t*)malloc(wfLZ_GetWorkMemSize());

	list<compressionHelper> compressedFiles;

	for(list<string>::iterator i = filesToPak.begin(); i != filesToPak.end(); i++)
	{
		cout << "Compressing \"" << *i << "\"..." << endl;
		unsigned int size = 0;
		unsigned char* decompressed;

		//Package these file types properly if needed
		if(i->find(".png") != string::npos)
			decompressed = extractImage(*i, &size);
		else if(i->find(".font") != string::npos)
			decompressed = extractFont(*i, &size);
		else
			decompressed = FileOperations::readFile(*i, &size);

		if(!size || !decompressed)
		{
			cout << "Unable to load file " << *i << endl;
			continue;
		}

		compressionHelper helper;
		helper.filename = *i;
		helper.header.pad = PAD_32BIT;
		helper.header.compressionType = COMPRESSION_FLAGS_WFLZ;
		helper.header.decompressedSize = size;
		uint8_t* compressed = (uint8_t*)malloc(wfLZ_GetMaxCompressedSize(helper.header.decompressedSize));
		helper.header.compressedSize = wfLZ_CompressFast(decompressed, size, compressed, workMem, 0);

		//See if compression made the file larger
		if(helper.header.compressedSize >= helper.header.decompressedSize)
		{
			//It did; use the uncompressed data instead
			helper.header.compressionType = COMPRESSION_FLAGS_UNCOMPRESSED;
			helper.header.compressedSize = helper.header.decompressedSize;
			helper.data = decompressed;
			free(compressed);
		}
		else
		{
			//It didn't; use the compressed data
			helper.data = compressed;
			free(decompressed);
		}

		//Add this compression helper to our list
		helper.size = helper.header.compressedSize;
		helper.id = Hash::hash(i->c_str());

		compressedFiles.push_back(helper);
	}

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
	for(list<compressionHelper>::iterator i = compressedFiles.begin(); i != compressedFiles.end(); i++)
	{
		ResourcePtr resPtr;
		resPtr.id = i->id;
		resPtr.offset = curOffset;

		fwrite(&resPtr, 1, sizeof(ResourcePtr), fOut);
		curOffset += sizeof(CompressionHeader) + i->header.compressedSize;
	}

	//Write resource data
	for(list<compressionHelper>::iterator i = compressedFiles.begin(); i != compressedFiles.end(); i++)
	{
		fwrite(&i->header, 1, sizeof(CompressionHeader), fOut);
		fwrite(i->data, 1, i->size, fOut);

		free(i->data);	//Free image data while we're at it
	}

	//Free our WFLZ working memory
	free(workMem);

	//Close output file
	fclose(fOut);
}

int main(int argc, char** argv)
{
	list<string> sFilelistNames;
	//Parse commandline
	for(int i = 1; i < argc; i++)
	{
		string s = argv[i];
		sFilelistNames.push_back(s);
	}
	//Compress files
	for(list<string>::iterator i = sFilelistNames.begin(); i != sFilelistNames.end(); i++)
	{
		compress(readFilenames(*i), *i);
	}
	return 0;
}
