#include "PakLoader.h"
#include "tinydir.h"
#include "wfLZ.h"
#include "easylogging++.h"
#include "Parse.h"
#include <sstream>
using namespace std;

void PakLoader::loadPaksFromDir(string sDirName)
{
	LOG(TRACE) << "Parse pak files from dir " << sDirName;
	tinydir_dir dir;
	tinydir_open(&dir, sDirName.c_str());

	while(dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		if(!file.is_dir)
		{
			string fileExt = Parse::getExtension(file.name);
			ostringstream oss;
			oss << sDirName << "/" << file.name;
			if(fileExt == "pak")
			{
				//should be pak file
				LOG(TRACE) << "open pak file " << sDirName << "/" << file.name;
				parseFile(oss.str());
			}
			else
				LOG(TRACE) << "Ignoring non-pak file " << oss.str();
		}

		tinydir_next(&dir);
	}

	tinydir_close(&dir);
}

PakLoader::PakLoader(string sDirName)
{
	loadPaksFromDir(sDirName);
}

PakLoader::~PakLoader()
{
	for(list<FILE*>::iterator i = openedFiles.begin(); i != openedFiles.end(); i++)
		fclose(*i);	//Close all our opened files

	openedFiles.clear();
	m_pakFiles.clear();
}

void PakLoader::parseFile(string sFileName)
{
	LOG(TRACE) << "Parse pak file " << sFileName;
	FILE* fp = fopen(sFileName.c_str(), "rb");
	if(fp)
	{
		//Try to read in file header
		PakFileHeader header;
		if(fread(&header, 1, sizeof(PakFileHeader), fp) != sizeof(PakFileHeader))
		{
			LOG(TRACE) << "couldn't read header";
			fclose(fp);
			return;
		}

		//Check file signature
		if(header.sig[0] != 'P' || header.sig[1] != 'A' || header.sig[2] != 'K' || header.sig[3] != 'C')
		{
			LOG(TRACE) << "sig incorrect";
			fclose(fp);
			return;
		}

		//Load ResourcePtrs
		for(int i = 0; i < header.numResources; i++)
		{
			ResourcePtr resPtr;
			if(fread(&resPtr, 1, sizeof(ResourcePtr), fp) != sizeof(ResourcePtr))
			{
				LOG(TRACE) << "couldn't read resptr";
				fclose(fp);
				return;
			}

			PakPtr pakPtr = {resPtr, fp};
			m_pakFiles[resPtr.id] = pakPtr;

		}

		openedFiles.push_back(fp);	//Hang onto this to close later
	}
	else
		LOG(TRACE) << "unable to open file";
}

//TODO: File size error checking and such here
unsigned char* PakLoader::loadResource(uint64_t id, unsigned int* len)
{
	LOG(TRACE) << "load resource with id " << id;
	map<uint64_t, PakPtr>::iterator it = m_pakFiles.find(id);
	if(it == m_pakFiles.end())
	{
		LOG(TRACE) << "resource was not in pak files";
		return NULL;
	}

	//Load resource
	fseek(it->second.fp, it->second.ptr.offset, SEEK_SET);

	//Read file
	CompressionHeader compHeader;
	if(fread(&compHeader, 1, sizeof(CompressionHeader), it->second.fp) != sizeof(CompressionHeader))
	{
		LOG(TRACE) << "couldn\'t read compression header";
		return NULL;
	}

	if(compHeader.compressionType == COMPRESSION_FLAGS_UNCOMPRESSED)
	{
		//Make sure we don't have an empty resource
		if(!compHeader.decompressedSize)
		{
			if(!compHeader.compressedSize)
			{
				LOG(TRACE) << "compressed size 0";
				return NULL;
			}
			
			compHeader.decompressedSize = compHeader.compressedSize;
		}

		unsigned char* uncompressedData = new unsigned char[compHeader.decompressedSize];
		if(fread(uncompressedData, 1, compHeader.decompressedSize, it->second.fp) != compHeader.decompressedSize)
		{
			delete[] uncompressedData;
			return NULL;
		}

		if(len)
			*len = compHeader.decompressedSize;

		LOG(TRACE) << "all good uncompressed";
		return uncompressedData;
	}
	else if(compHeader.compressionType == COMPRESSION_FLAGS_WFLZ)
	{
		if(!compHeader.compressedSize)
			return NULL;

		unsigned char* compressedData = new unsigned char[compHeader.compressedSize];
		if(fread(compressedData, 1, compHeader.compressedSize, it->second.fp) != compHeader.compressedSize)
		{
			LOG(TRACE) << "couldn\'t read compressed data";
			delete[] compressedData;
			return NULL;
		}

		//If for some reason we don't have the decompressed size, generate it now
		if(!compHeader.decompressedSize)
			compHeader.decompressedSize = wfLZ_GetDecompressedSize(compressedData);

		if(!compHeader.decompressedSize)
		{
			LOG(TRACE) << "decompressed size wrong";
			delete[] compressedData;
			return NULL;
		}

		//Allocate memory and decompress
		unsigned char* decompressedData = new unsigned char[compHeader.decompressedSize];
		wfLZ_Decompress(compressedData, decompressedData);

		//Clean up
		if(len)
			*len = compHeader.decompressedSize;

		LOG(TRACE) << "all good wflz";
		delete[] compressedData;
		return decompressedData;
	}

	LOG(WARNING) << "Unknown compression header type " << compHeader.compressionType;
	return NULL;
}
