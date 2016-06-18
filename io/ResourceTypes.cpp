#include "ResourceTypes.h"
#include "wfLZ.h"
#include <string.h>
#include <stdio.h>

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

unsigned char* decompressResource(unsigned char* resource, unsigned int* decompressedSize, unsigned int len)
{
	if(len && len < sizeof(CompressionHeader)) return NULL;

	//Read header
	CompressionHeader header;
	memcpy(&header, resource, sizeof(CompressionHeader));
	resource += sizeof(CompressionHeader);

	if(len && len < header.compressedSize + sizeof(CompressionHeader)) return NULL;

	//If for some reason we don't have the decompressed size, generate it now
	if(!header.decompressedSize)
		header.decompressedSize = wfLZ_GetDecompressedSize(resource);

	//Allocate memory and decompress
	unsigned char* decompressedBuf = (unsigned char*)malloc(header.decompressedSize);
	wfLZ_Decompress(resource, decompressedBuf);
	
	//Clean up
	if(decompressedSize)
		*decompressedSize = header.decompressedSize;
	return decompressedBuf;
}

void freeResource(unsigned char* resource)
{
	free(resource);
}