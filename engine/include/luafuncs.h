#ifndef LUA_ENGINE_BINDINGS_H
#define LUA_ENGINE_BINDINGS_H

//TODO: This should be a base class or something, so that we don't have to define this in the engine, and then implement on the game side.
#include "luadefines.h"

struct lua_State;

struct LuaFunctions
{
    const char *name;
    lua_CFunction func;
};

void lua_register_enginefuncs(lua_State *);


#endif

