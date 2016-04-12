#ifndef LUA_ENGINE_BINDINGS_H
#define LUA_ENGINE_BINDINGS_H

//TODO: This should be a base class or something, so that we don't have to define this in the engine, and then implement on the game side.
#include "luadefines.h"

struct lua_State;

//TODO: Do we need this here?
enum ObjMainType
{
	OT_IMAGE,
	OT_RENDERER,
	OT_CAM,
	OT_WORLD,
	OT_FILM,
	OT_LIGHT,
	OT_MATERIAL,
	OT_TEXTURE,
	OT_PRIMITIVE,
	OT_INTEGRATOR,
	OT_FOG,
	OT_COORDMAPPER,
	OT_MATLIB,
	OT_BSPLINE,
};

struct LuaFunctions
{
    const char *name;
    lua_CFunction func;
};

void lua_register_enginefuncs(lua_State *);
void pushObjPtr(ObjMainType ty, lua_State *L, void *ptr);	//TODO: Do we need this here?

#endif

