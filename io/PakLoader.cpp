#include "PakLoader.h"
#include "wfLZ.h"
#include "Logger.h"
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
    for(std::vector<FILE*>::iterator i = openedFiles.begin(); i != openedFiles.end(); i++)
        fclose(*i);    //Close all our opened files

    openedFiles.clear();
    m_pakFiles.clear();
}

void PakLoader::parseFile(const std::string& sFileName)
{
    LOG_info("Parse pak file %s", sFileName.c_str());
    FILE* fp = fopen(sFileName.c_str(), "rb");
    if(fp)
    {
        //Try to read in file header
        PakFileHeader header;
        if(fread(&header, 1, sizeof(PakFileHeader), fp) != sizeof(PakFileHeader))
        {
            LOG_err("couldn't read header for pak %s", sFileName.c_str());
            fclose(fp);
            return;
        }

        //Check file signature
        if(header.sig[0] != 'P' || header.sig[1] != 'A' || header.sig[2] != 'K' || header.sig[3] != 'C')
        {
            LOG_err("sig incorrect for pak %s", sFileName.c_str());
            fclose(fp);
            return;
        }

        if(header.version != VERSION_1_0)
        {
            LOG_err("Unexpected version %u for pak; expected %d", header.version, VERSION_1_0);
            fclose(fp);
            return;
        }

        //Load ResourcePtrs
        for(unsigned int i = 0; i < header.numResources; i++)
        {
            ResourcePtr resPtr;
            if(fread(&resPtr, 1, sizeof(ResourcePtr), fp) != sizeof(ResourcePtr))
            {
                LOG_err("couldn't read resptr %d for pak %s", i, sFileName.c_str());
                fclose(fp);
                return;
            }

            PakPtr pakPtr = {resPtr, fp};
            m_pakFiles[resPtr.id] = pakPtr;

        }

        openedFiles.push_back(fp);    //Hang onto this to close later
    }
    else
        LOG_err("unable to open file %s for reading", sFileName.c_str());
}

unsigned char* PakLoader::loadResource(uint64_t id, unsigned int* len)
{
    LOG_info("load resource with id %u", id);
    std::map<uint64_t, PakPtr>::iterator it = m_pakFiles.find(id);
    if(it == m_pakFiles.end())
    {
        LOG_info("resource was not in pak files");
        return NULL;
    }

    //Load resource
    if(fseek(it->second.fp, (long)it->second.ptr.offset, SEEK_SET))
    {
        LOG_warn("Unable to seek to proper location in resource file for resource ID %ul", id);
        return NULL;
    }

    //Read file
    CompressionHeader compHeader;
    if(fread(&compHeader, 1, sizeof(CompressionHeader), it->second.fp) != sizeof(CompressionHeader))
    {
        LOG_warn("couldn\'t read compression header");
        return NULL;
    }

    if(compHeader.compressionType == COMPRESSION_FLAGS_UNCOMPRESSED)
    {
        //Make sure we don't have an empty resource
        if(!compHeader.decompressedSize)
        {
            if(!compHeader.compressedSize)
            {
                LOG_warn("compressed size 0");
                return NULL;
            }

            compHeader.decompressedSize = compHeader.compressedSize;
        }

        unsigned char* uncompressedData = (unsigned char*)malloc(compHeader.decompressedSize);
        if(fread(uncompressedData, 1, compHeader.decompressedSize, it->second.fp) != compHeader.decompressedSize)
        {
            LOG_warn("couldn\'t read decompressed size of bytes");
            free(uncompressedData);
            return NULL;
        }

        if(len)
            *len = compHeader.decompressedSize;

        LOG_info("all good uncompressed");
        return uncompressedData;
    }
    else if(compHeader.compressionType == COMPRESSION_FLAGS_WFLZ)
    {
        if(!compHeader.compressedSize)
        {
            LOG_warn("compressed size is zero");
            return NULL;
        }

        unsigned char* compressedData = (unsigned char*)malloc(compHeader.compressedSize);
        if(fread(compressedData, 1, compHeader.compressedSize, it->second.fp) != compHeader.compressedSize)
        {
            LOG_warn("couldn\'t read compressed data");
            free(compressedData);
            return NULL;
        }

        //If for some reason we don't have the decompressed size, generate it now
        if(!compHeader.decompressedSize)
            compHeader.decompressedSize = wfLZ_GetDecompressedSize(compressedData);

        if(!compHeader.decompressedSize)
        {
            LOG_warn("decompressed size wrong");
            free(compressedData);
            return NULL;
        }

        //Allocate memory and decompress
        unsigned char* decompressedData = (unsigned char*)malloc(compHeader.decompressedSize);
        wfLZ_Decompress(compressedData, decompressedData);

        //Clean up
        if(len)
            *len = compHeader.decompressedSize;

        LOG_info("all good wflz");
        free(compressedData);
        return decompressedData;
    }

    LOG_warn("Unknown compression header type %u", compHeader.compressionType);
    return NULL;
}
