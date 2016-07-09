//This program takes any file and outputs raw WFLZ-compressed data
#include "wfLZ.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <list>
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

uint64_t hashString(string sHashStr)
{
	const char* str = sHashStr.c_str();
	uint64_t hash = 5381;
	int c;

	while(c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}

string remove_extension(const string& filename)
{
	size_t lastdot = filename.find_last_of(".");
	if(lastdot == string::npos) return filename;
	return filename.substr(0, lastdot);
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

unsigned char* readFile(string filename, unsigned int* fileSize)
{
	//Extract an image from this file if it is one
	if(filename.find(".png") != string::npos)
		return extractImage(filename, fileSize);

	//Otherwise, fall back on writing raw file
	FILE *f = fopen(filename.c_str(), "rb");
	if(f == NULL)
	{
		cout << "unable to open? " << filename << endl;
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);

	unsigned char *buf = (unsigned char*)malloc(size);
	fread(buf, 1, size, f);
	fclose(f);

	if(fileSize)
		*fileSize = size;


	return buf;
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
		if(!s.length())
			continue;	//Ignore blank lines
		if(s.find('#') != string::npos)
			continue;	//Ignore comment lines
		lFilenames.push_back(s);	//Add this to our list of files to package
	}
	infile.close();
	return lFilenames;
}

uint32_t getResourceType(string sFilename)
{
	return RESOURCE_TYPE_IMAGE;	//TODO Support other resource types
}

void compress(list<string> filesToPak, string pakFilename)
{
	pakFilename = remove_extension(pakFilename);
	pakFilename += ".pak";
	cout << "Compressing " << pakFilename << "..." << endl;

	uint8_t* workMem = (uint8_t*)malloc(wfLZ_GetWorkMemSize());

	list<compressionHelper> compressedFiles;


	for(list<string>::iterator i = filesToPak.begin(); i != filesToPak.end(); i++)
	{
		unsigned int size = 0;
		unsigned char* decompressed = readFile(*i, &size);

		if(!size)
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

		helper.data = compressed;

		//See if compression made the file larger
		if(helper.header.compressedSize >= helper.header.decompressedSize)
		{
			helper.header.compressionType = COMPRESSION_FLAGS_UNCOMPRESSED;
			helper.header.compressedSize = helper.header.decompressedSize;
			helper.data = decompressed;
			free(compressed);
		}
		else
			free(decompressed);

		//TODO: Pad to end of 16-bit row

		//Add this compression helper to our list
		helper.size = helper.header.compressedSize;
		helper.id = hashString(*i);

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

	uint32_t curOffset = sizeof(PakFileHeader) + compressedFiles.size() * sizeof(ResourcePtr);	//Start after both these

	//Write resource pointers
	for(list<compressionHelper>::iterator i = compressedFiles.begin(); i != compressedFiles.end(); i++)
	{
		ResourcePtr resPtr;
		resPtr.id = i->id;
		resPtr.offset = curOffset;
		resPtr.type = getResourceType(i->filename);

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
