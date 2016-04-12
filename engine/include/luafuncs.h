#ifndef LUA_ENGINE_BINDINGS_H
#define LUA_ENGINE_BINDINGS_H

//TODO: This should be a base class or something, so that we don't have to define this in the engine, and then implement on the game side.
#include "luadefines.h"

struct lua_State;

//TODO: Do we need this here?
enum ObjMainType
{
	OT_IMAGE,
	OT_OBJECT,
	OT_B2BODY,
	OT_ARC,
	OT_BG,
	OT_CURSOR,
	OT_ENGINE,
	OT_GAMEENGINE,
	OT_HUD,
	OT_LATTICE,
	OT_NODE,
	OT_PARTICLESYSTEM,
	OT_TEXT,
	OT_TINYXML2,
	//TODO: Figure out what we should have, and add moar / remove some
};

struct LuaFunctions
{
    const char *name;
    lua_CFunction func;
};

void lua_register_enginefuncs(lua_State *);
void pushObjPtr(ObjMainType ty, lua_State *L, void *ptr);	//TODO: Do we need this here?

#endif

