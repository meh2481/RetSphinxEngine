#include "main.h"
#include "LuaInterface.h"
#include "Logger.h"

static LuaInterface* lua;

unsigned char* extractLua(const std::string& luaFilename, unsigned int* size)
{
    return NULL;
}

void initLua()
{
    logger_init("compressor_out.log", DBG);
    lua = new LuaInterface("LuaMinify/CommandLineMinify.lua");
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
