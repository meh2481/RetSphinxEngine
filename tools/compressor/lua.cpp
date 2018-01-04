#include "main.h"
#include "LuaInterface.h"
#include "Logger.h"
#include "FileOperations.h"

static LuaInterface* lua;

unsigned char* extractLua(const std::string& luaFilename, unsigned int* size)
{
    return NULL;
}

void initLua()
{
    logger_init("compressor_out.log", DBG);
    unsigned int size = 0;
    char* luaFile = (char*)FileOperations::readFile("tools/compressor/LuaMinify/CommandLineMinify.lua", &size);
    if(luaFile == NULL || size < 1)
        std::cout << "Unable to load lua file" << std::endl;
    char* nullpadded = (char*)malloc(size + 1);
    nullpadded[size] = '\0';
    memcpy(nullpadded, luaFile, size);
    lua = new LuaInterface(nullpadded);
    free(luaFile);
    if(!lua->Init())
    {
        std::cout << "Failed to init lua" << std::endl;
        std::cout << nullpadded << std::endl;
    }
    free(nullpadded);
}

void teardownLua()
{
    delete lua;
    logger_quit();
}

void lua_register_all(lua_State *)
{
    //no-op to keep lua interface happy
}
