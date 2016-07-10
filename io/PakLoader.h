#pragma once
#include <stdio.h>
#include <inttypes.h>
#include <map>
#include <string>
#include <list>
#include "ResourceTypes.h"

class PakLoader
{
	typedef struct
	{
		ResourcePtr ptr;
		FILE* fp;
	} PakPtr;

	std::map<uint64_t, PakPtr> m_pakFiles;
	std::list<FILE*> openedFiles;

	void parseFile(std::string sFileName);

	PakLoader() {};

	void loadPaksFromDir(std::string sDirName);

public:
	PakLoader(std::string sDirName);
	~PakLoader();

	unsigned char* loadResource(uint64_t id, unsigned int* len = NULL);
};