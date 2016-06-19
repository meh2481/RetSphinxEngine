//This program takes any file and outputs raw WFLZ-compressed data
#include "wfLZ.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cstdio>
#ifdef _WIN32
	#include <windows.h>
#endif

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "ResourceTypes.h"
#include <list>
#include <cmath>
#include <cstring>
#include <iomanip>
using namespace std;

string remove_extension(const string& filename)
{
	size_t lastdot = filename.find_last_of(".");
	if(lastdot == string::npos) return filename;
	return filename.substr(0, lastdot);
}

unsigned char* readFile(const char* filename, unsigned int* fileSize)
{
	FILE *f = fopen(filename, "rb");
	if(f == NULL)
	{
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

void compress(string filename)
{
	cout << filename << endl;
	unsigned int size = 0;
	unsigned char* decompressed = readFile(filename.c_str(), &size);
	
	if(size == 0)
	{
		cout << "Unable to load file " << filename << endl;
		return;
	}
	filename = remove_extension(filename) + ".wflz";
	
	CompressionHeader header;
	header.flags = COMPRESSION_FLAGS_WFLZ;
	header.decompressedSize = size;
	uint8_t* compressed = ( uint8_t* )malloc( wfLZ_GetMaxCompressedSize( header.decompressedSize) );
	uint8_t* workMem = ( uint8_t* )malloc( wfLZ_GetWorkMemSize() );
	header.compressedSize = wfLZ_CompressFast( decompressed, size, compressed, workMem, 0 );
	
	FILE *f = fopen(filename.c_str(), "wb");
	fwrite(&header, 1, sizeof(CompressionHeader), f);
	fwrite(compressed, 1, header.compressedSize, f);
	fclose(f);
	
	free(decompressed);
	free(compressed);
	free(workMem);
}

int main(int argc, char** argv)
{
	list<string> sFilenames;
	//Parse commandline
	for(int i = 1; i < argc; i++)
	{
		string s = argv[i];
		sFilenames.push_back(s);
	}
	//Compress files
	for(list<string>::iterator i = sFilenames.begin(); i != sFilenames.end(); i++)
		compress((*i).c_str());
	return 0;
}
