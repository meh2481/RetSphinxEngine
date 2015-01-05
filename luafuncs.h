#ifndef LUA_ENGINE_BINDINGS_H
#define LUA_ENGINE_BINDINGS_H

#include "luadefines.h"

struct lua_State;

struct LuaFunctions
{
    const char *name;
    lua_CFunction func;
};

void lua_register_enginefuncs(lua_State *);


#endif

