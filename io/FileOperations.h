#pragma once
#include <string>
#include <set>
#include <ctime>

namespace FileOperations
{
    //Read an entire file into memory and return the buffer, NULL on error.
    //The pointer returned from this function must be free()d.
    unsigned char* readFile(const std::string& filename, unsigned int* fileSize);

    std::set<std::string> readFilesFromDir(const std::string& sDirPath, bool fullPath = true);  //Return a list of all files in the given folder

    time_t timeModified(const std::string& filename);   //Get the last modified time of the given file

    bool fileExists(const std::string& filename);   //Return true if file exists, false if no
}
