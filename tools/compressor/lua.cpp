#include "main.h"
#include "LuaInterface.h"
#include "Logger.h"
#include "FileOperations.h"
#include "lua.hpp"
#include <cstring>
#include <iostream>

static LuaInterface* lua = NULL;

unsigned char* extractLua(const std::string& luaFilename, unsigned int* size)
{
    //Call main_minify in lua
    lua_State* L = lua->getState();
    lua_getglobal(L, "main_minify");
    lua_pushstring(L, luaFilename.c_str());
    lua_call(L, 1, 1);
    const char* result = lua_tostring(L, -1);
    lua_pop(L, 1);

    if(result)
    {
        *size = strlen(result);
        unsigned char* ret = (unsigned char*)malloc(*size);
        memcpy(ret, result, *size);
        return ret;
    }
    else
    {
        std::cout << "not string" << std::endl;
    }
    return NULL;
}

void initLua()
{
    logger_init("compressor_out.log", SDL_LOG_PRIORITY_DEBUG);
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
