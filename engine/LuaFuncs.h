#pragma once
#include "LuaDefines.h"

struct lua_State;

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

void lua_register_all(lua_State *);


