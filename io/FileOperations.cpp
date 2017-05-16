#include <cstdlib>
#include <cstdio>
#include <sstream>
#include "FileOperations.h"
#include "tinydir.h"
using namespace std;

namespace FileOperations
{
	unsigned char* readFile(const string& filename, unsigned int* fileSize)
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

	set<string> readFilesFromDir(const string& sDirPath, bool fullPath)
	{
		set<string> returnedFiles;

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
					ostringstream oss;
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

}