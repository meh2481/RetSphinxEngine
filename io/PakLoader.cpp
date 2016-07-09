#include "PakLoader.h"
#include "tinydir.h"
#include "wfLZ.h"
#include "easylogging++.h"

PakLoader::PakLoader(string sDirName)
{
	tinydir_dir dir;
	tinydir_open(&dir, sDirName.c_str());

	while(dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		if(!file.is_dir)
		{
			LOG(INFO) << "opening pak file " << file.name;
			//TODO Test filename for including ".pak" at least
			parseFile(file.name);
		}

		tinydir_next(&dir);
	}

	tinydir_close(&dir);
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
	FILE* fp = fopen(sFileName.c_str(), "rb");
	if(fp)
	{
		//Try to read in file header
		PakFileHeader header;
		if(fread(&header, 1, sizeof(PakFileHeader), fp) != sizeof(PakFileHeader))
		{
			fclose(fp);
			return;
		}

		//Check file signature
		if(header.sig[0] != 'P' || header.sig[1] != 'A' || header.sig[2] != 'K' || header.sig[3] != 'C')
		{
			fclose(fp);
			return;
		}

		//Load ResourcePtrs
		for(int i = 0; i < header.numResources; i++)
		{
			ResourcePtr resPtr;
			if(fread(&resPtr, 1, sizeof(ResourcePtr), fp) != sizeof(ResourcePtr))
			{
				fclose(fp);
				return;
			}

			//TODO Validate resource type

			PakPtr pakPtr = {resPtr, fp};
			m_pakFiles[resPtr.id] = pakPtr;

		}

		openedFiles.push_back(fp);	//Hang onto this to close later
	}
}

//TODO: File size error checking and such here
unsigned char* PakLoader::loadResource(uint64_t id, unsigned int* len)
{
	map<uint64_t, PakPtr>::iterator it = m_pakFiles.find(id);
	if(it == m_pakFiles.end())
		return NULL;

	//Load resource
	fseek(it->second.fp, it->second.ptr.offset, SEEK_SET);

	//Read file
	CompressionHeader compHeader;
	if(fread(&compHeader, 1, sizeof(CompressionHeader), it->second.fp) != sizeof(CompressionHeader))
		return NULL;

	if(compHeader.compressionType == COMPRESSION_FLAGS_UNCOMPRESSED)
	{
		//Make sure we don't have an empty resource
		if(!compHeader.decompressedSize)
		{
			if(!compHeader.compressedSize)
				return NULL;
			
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

		return uncompressedData;
	}
	else if(compHeader.compressionType == COMPRESSION_FLAGS_WFLZ)
	{
		if(!compHeader.compressedSize)
			return NULL;

		unsigned char* compressedData = new unsigned char[compHeader.compressedSize];
		if(fread(compressedData, 1, compHeader.compressedSize, it->second.fp) != compHeader.compressedSize)
		{
			delete[] compressedData;
			return NULL;
		}

		//If for some reason we don't have the decompressed size, generate it now
		if(!compHeader.decompressedSize)
			compHeader.decompressedSize = wfLZ_GetDecompressedSize(compressedData);

		if(!compHeader.decompressedSize)
		{
			delete[] compressedData;
			return NULL;
		}

		//Allocate memory and decompress
		unsigned char* decompressedData = new unsigned char[compHeader.decompressedSize];
		wfLZ_Decompress(compressedData, decompressedData);

		//Clean up
		if(len)
			*len = compHeader.decompressedSize;

		delete[] compressedData;
		return decompressedData;
	}

	return NULL;
}
