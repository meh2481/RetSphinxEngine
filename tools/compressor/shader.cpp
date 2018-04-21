#include "main.h"
#include "FileOperations.h"
#include <sstream>
#include <iostream>

#define GLSLANGVALIDATOR "glslangValidator"

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
        return DEFAULT_TMP_NAME;

    std::wstring wFilename(szTempFileName);
    return ws2s(wFilename);
#else
#warning todo
    return "/tmp/tmp.spv";
#endif
}

unsigned char* extractShader(const std::string& glslFilename, unsigned int* size)
{
    //Read the GLSL shader code
    unsigned int glslSize;
    unsigned char* shaderCode = FileOperations::readFile(glslFilename, &glslSize);

    std::string tempFilename = getTempFilename();

    //Assumes glslangValidator is in your PATH
    std::ostringstream oss;
    oss << GLSLANGVALIDATOR << " -Os -o \""
        << tempFilename
        << "\" -V \""
        << glslFilename << "\"";
    std::cout << "Invoking " << oss.str() << std::endl;

    int result = system(oss.str().c_str());
    if(result != EXIT_SUCCESS)
    {
        std::cout << "Unable to invoke glslangValidator" << std::endl;
        return NULL;
    }
    //Read back output
    return FileOperations::readFile(tempFilename.c_str(), size);
}
