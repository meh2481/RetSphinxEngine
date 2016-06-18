//This program takes PNG images and outputs WFLZ-compressed image data
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

void compress(string filename)
{
	cout << filename << endl;
	int comp = 0;
	int width = 0;
	int height = 0;
	unsigned char* cBuf = stbi_load(filename.c_str(), &width, &height, &comp, 0);
	filename = remove_extension(filename) + ".img";
	
	if((cBuf == 0) || (width == 0) || (height == 0))
	{
		cout << "Unable to load image " << filename << endl;
		return;
	}
	TextureHeader textureHeader;
	textureHeader.width = width;
	textureHeader.height = height;
	textureHeader.bpp = TEXTURE_BPP_RGBA;
	if(comp == 3)
		textureHeader.bpp = TEXTURE_BPP_RGB;
	
	int decompressedSize = textureHeader.width*textureHeader.height*textureHeader.bpp/8 + sizeof(TextureHeader);
	
	//Add texheader to buf
	uint8_t* decompressed = ( uint8_t* )malloc( decompressedSize );
	memcpy(decompressed, &textureHeader, sizeof(TextureHeader));
	memcpy(&decompressed[sizeof(TextureHeader)], cBuf, decompressedSize-sizeof(TextureHeader));
	
	CompressionHeader header;
	header.flags = COMPRESSION_FLAGS_WFLZ;
	header.decompressedSize = decompressedSize;
	uint8_t* compressed = ( uint8_t* )malloc( wfLZ_GetMaxCompressedSize( header.decompressedSize ) );
	uint8_t* workMem = ( uint8_t* )malloc( wfLZ_GetWorkMemSize() );
	header.compressedSize = wfLZ_CompressFast( decompressed, decompressedSize, compressed, workMem, 0 );
	
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
