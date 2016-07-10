#include <cstdlib>
#include <cstdio>
#include "FileOperations.h"
using namespace std;

namespace FileOperations
{
	unsigned char* readFile(string filename, unsigned int* fileSize)
	{
		FILE *f = fopen(filename.c_str(), "rb");
		if(f == NULL)
			return NULL;	//File not here

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

}