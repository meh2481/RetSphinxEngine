#include "luafuncs.h"
#include "luainterface.h"
#include "GameEngine.h"

extern GameEngine* g_pGlobalEngine;

//Class for interfacing between GameEngine and Lua
//(defined here instead of inside header because of weird cross-inclusion stuff)
class PonyLua
{
public:
	static void rumble(float32 fAmt, float32 len)
	{
		g_pGlobalEngine->rumbleController(fAmt, len, true);
	}
};

luaFunc(rumblecontroller)	//rumblecontroller(float force, float sec) --force is range [0,1]
{
	float32 force = lua_tonumber(L, 1);
	float32 sec = lua_tonumber(L, 2);
	PonyLua::rumble(force, sec);
}


static LuaFunctions s_functab[] =
{
	luaRegister(rumblecontroller),
	//luaRegister(),
	{NULL, NULL}
};

void lua_register_enginefuncs(lua_State *L)
{	
	for(unsigned int i = 0; s_functab[i].name; ++i)
		lua_register(L, s_functab[i].name, s_functab[i].func);
}
