#include "main.h"
#include "FileOperations.h"
#include <sstream>
#include <iostream>

#define GLSLANGVALIDATOR "glslangValidator"
#define SPIRV_OPT "spirv-opt"

extern bool g_bReduceShader;

#ifdef _WIN32
#include <Windows.h>
#define DEFAULT_TMP_NAME "tmp.spv"
//Convert wstring to string since Windows is dumb and uses UTF-16
std::string ws2s(const std::wstring& s)
{
    int len;
    int slength = (int)s.length();
    len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
    std::string r(len, '\0');
    WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
    return r;
}
#endif

std::string getTempFilename()
{
#ifdef _WIN32
    WCHAR szTempFileName[MAX_PATH];
    WCHAR lpTempPathBuffer[MAX_PATH];

    //Get a temporary path
    DWORD dwRetVal = GetTempPathW(MAX_PATH, lpTempPathBuffer);
    if(dwRetVal > MAX_PATH || (dwRetVal == 0))
        return DEFAULT_TMP_NAME;

    //Generate a temporary file name
    UINT uRetVal = GetTempFileNameW(lpTempPathBuffer, TEXT(L"SPV"), 0, szTempFileName);
    if(uRetVal == 0)
        return ws2s(lpTempPathBuffer) + "\\" + DEFAULT_TMP_NAME;

    std::wstring wFilename(szTempFileName);
    return ws2s(wFilename);
#else
#warning todo
    return "/tmp/tmp.spv";
#endif
}

//Assumes glslangValidator tools are in your PATH
unsigned char* extractShader(const std::string& glslFilename, unsigned int* size)
{
    //Read the GLSL shader code
    unsigned int glslSize;
    unsigned char* shaderCode = FileOperations::readFile(glslFilename, &glslSize);

    std::string tempFilename1 = getTempFilename();

    //Convert glsl to SPIR-V
    std::ostringstream oss;
    oss << GLSLANGVALIDATOR << " -Os -o \""
        << tempFilename1
        << "\" -V \""
        << glslFilename << "\"";
    //std::cout << "Invoking " << oss.str() << std::endl;

    int result = system(oss.str().c_str());
    if(result != EXIT_SUCCESS)
    {
        std::cout << "Unable to invoke " << GLSLANGVALIDATOR << std::endl;
        return NULL;
    }

    if(g_bReduceShader)
    {
        std::string tempFilename2 = getTempFilename();

        //Reduce file size of SPIR-V (still bigger than glsl; meh)
        oss.str("");
        oss << SPIRV_OPT << " --strip-debug -Os \""
            << tempFilename1
            << "\" -o \""
            << tempFilename2
            << "\"";
        //std::cout << "Invoking " << oss.str() << std::endl;

        result = system(oss.str().c_str());
        unsigned char* ret = NULL;
        if(result != EXIT_SUCCESS)
        {
            std::cout << "Unable to invoke " << SPIRV_OPT << std::endl;
            //Return the non-minified version
            ret = FileOperations::readFile(tempFilename1.c_str(), size);
            remove(tempFilename1.c_str());
        }
        else
        {
            ret = FileOperations::readFile(tempFilename2.c_str(), size);
            remove(tempFilename1.c_str());
            remove(tempFilename2.c_str());
        }
        return ret;
    }

    //Read back output
    unsigned char* ret = FileOperations::readFile(tempFilename1.c_str(), size);

    //Cleanup temp files
    remove(tempFilename1.c_str());

    return ret;
}
