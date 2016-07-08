#pragma once
#include <stdio.h>
#include <inttypes.h>
#include <map>
#include <string>
#include <list>
#include "ResourceTypes.h"
using namespace std;

class PakLoader
{
	typedef struct
	{
		ResourcePtr ptr;
		FILE* fp;
	} PakPtr;

	map<uint64_t, PakPtr> m_pakFiles;
	list<FILE*> openedFiles;

	void parseFile(string sFileName);

	PakLoader() {};

public:
	PakLoader(string sDirName);
	~PakLoader();

	unsigned char* loadResource(uint64_t id, unsigned int* len = NULL);
};