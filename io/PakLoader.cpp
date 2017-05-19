#include "PakLoader.h"
#include "wfLZ.h"
#include "easylogging++.h"
#include "StringUtils.h"
#include "FileOperations.h"
#include <sstream>

void PakLoader::loadFromDir(const std::string& sDirName)
{
	std::set<std::string> pakFiles = FileOperations::readFilesFromDir(sDirName);

	for(std::set<std::string>::iterator i = pakFiles.begin(); i != pakFiles.end(); i++)
	{
		if(StringUtils::getExtension(*i) == PAK_FILE_TYPE)
			parseFile(*i);
	}
}

PakLoader::PakLoader(const std::string& sDirName)
{
	loadFromDir(sDirName);
}

PakLoader::~PakLoader()
{
	clear();
}

void PakLoader::clear()
{
	for(std::list<FILE*>::iterator i = openedFiles.begin(); i != openedFiles.end(); i++)
		fclose(*i);	//Close all our opened files

	openedFiles.clear();
	m_pakFiles.clear();
}

void PakLoader::parseFile(const std::string& sFileName)
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
		for(unsigned int i = 0; i < header.numResources; i++)
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

unsigned char* PakLoader::loadResource(uint64_t id, unsigned int* len)
{
	LOG(TRACE) << "load resource with id " << id;
	std::map<uint64_t, PakPtr>::iterator it = m_pakFiles.find(id);
	if(it == m_pakFiles.end())
	{
		LOG(TRACE) << "resource was not in pak files";
		return NULL;
	}

	//Load resource
	if(fseek(it->second.fp, (long)it->second.ptr.offset, SEEK_SET))
	{
		LOG(WARNING) << "Unable to seek to proper location in resource file for resource ID " << id;
		return NULL;
	}

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
