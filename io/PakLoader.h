#pragma once
#include <stdio.h>
#include <inttypes.h>
#include <map>
#include <string>
#include <vector>
#include "ResourceTypes.h"

#define PAK_FILE_TYPE "pak"

class PakLoader
{
    typedef struct
    {
        ResourcePtr ptr;
        FILE* fp;
    } PakPtr;

    std::map<uint64_t, PakPtr> m_pakFiles;    //Maps resource IDs to particular pak files
    std::vector<FILE*> openedFiles;            //Keeps track of all opened file handles so we can close them easily

    void parseFile(const std::string& sFileName);

    PakLoader() {};

public:
    PakLoader(const std::string& sDirName);
    ~PakLoader();

    void clear();
    void loadFromDir(const std::string& sDirName);

    //Load a resource from the opened pak files. Returns NULL if it's not here or on error,
    // returns a pointer to the data otherwise. This pointer must be delete[]d.
    unsigned char* loadResource(uint64_t id, unsigned int* len = NULL);
};