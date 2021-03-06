#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#else
#define stat _stat
#endif
#include "FileOperations.h"
#include "tinydir.h"

namespace FileOperations
{
    unsigned char* readFile(const std::string& filename, unsigned int* fileSize)
    {
        FILE *f = fopen(filename.c_str(), "rb");
        if(f == NULL)
            return NULL;    //File not here

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

    std::set<std::string> readFilesFromDir(const std::string& sDirPath, bool fullPath)
    {
        std::set<std::string> returnedFiles;

        tinydir_dir dir;
        tinydir_open(&dir, sDirPath.c_str());

        while(dir.has_next)
        {
            tinydir_file file;
            tinydir_readfile(&dir, &file);

            if(!file.is_dir)
            {
                if(fullPath)
                {
                    std::ostringstream oss;
                    oss << sDirPath << "/" << file.name;
                    returnedFiles.insert(oss.str());
                }
                else
                    returnedFiles.insert(file.name);
            }

            tinydir_next(&dir);
        }

        tinydir_close(&dir);

        return returnedFiles;
    }

    time_t timeModified(const std::string& filename)
    {
        struct stat result;
        if(stat(filename.c_str(), &result) == 0)
        {
            return result.st_mtime;
        }
        return time(0); //Return current time if can't stat last modified time
    }

    bool fileExists(const std::string& filename)
    {
        struct stat buffer;
        return (stat(filename.c_str(), &buffer) == 0);
    }

}
