#ifndef LUA_MY_DEFINES_H
#define LUA_MY_DEFINES_H

#include <lua.hpp>
#include <string>
using namespace std;


#define luaFunc(func)       static int l_##func(lua_State *L)
#define luaFn(func)         static int func(lua_State *L)
#define luaRegister(func)	{#func, l_##func}
#define luaClassMethod(name, func) {"" name, l_##func}

#define luaConstant(x) {#x, x}

#define luaReturnNil() { return 0; }
#define luaReturnNum(x) { lua_pushnumber(L, x); return 1; }
#define luaReturnSelf() { lua_pushvalue(L, 1); return 1; } // This assumes 'self' is at bottom of stack, i.e. first arg
#define luaReturnNum(x) { lua_pushnumber(L, x); return 1; }
#define luaReturnInt(x) {lua_pushinteger(L, x); return 1; }
#define luaReturnBool(x) { lua_pushboolean(L, x); return 1; }
#define luaReturnStr(x) { lua_pushstring(L, x); return 1; }
#define luaReturn2Strings(x,y) { lua_pushstring(L, x); lua_pushstring(L, y); return 2; }
#define luaReturn3Strings(x,y,z) { lua_pushstring(L, x); lua_pushstring(L, y);  lua_pushstring(L, z); return 3; }
#define luaReturnPtr(x) { luaPushPointer(L, x); return 1; }
#define luaReturnVec2(x, y) { lua_pushnumber(L, x); lua_pushnumber(L, y); return 2; }
#define luaReturnVec3(x, y, z) { lua_pushnumber(L, x); lua_pushnumber(L, y); lua_pushnumber(L, z); return 3; }
#define luaReturnVec4(x, y, z, w) { lua_pushnumber(L, x); lua_pushnumber(L, y); lua_pushnumber(L, z); lua_pushnumber(L, w); return 4; }

#define lua_tofloat(L, x) (static_cast<float>(lua_tonumber(L, x)))


inline void luaPushPointer(lua_State *L, void *x)
{
	if(x)
		lua_pushlightuserdata(L, x);
	else
		lua_pushnil(L);
}

inline const char *getCStrSafe(lua_State *L, int idx = 1)
{
    const char *s = NULL;
    if(lua_isstring(L, idx))
        s = lua_tostring(L, idx);
    return s ? s : "";
}

inline const char *getCStr(lua_State *L, int idx = 1)
{
    const char *s = NULL;
    if(lua_isstring(L, idx))
        s = lua_tostring(L, idx);
    return s;
}

inline std::string getStr(lua_State *L, int idx = 1)
{
    std::string s;
    if(lua_isstring(L, idx))
        s = lua_tostring(L, idx);
    return s;
}

inline bool getBool(lua_State *L, int idx = 1)
{
    switch(lua_type(L, idx))
    {
        case LUA_TNONE:
        case LUA_TNIL: return false;
        case LUA_TBOOLEAN: return lua_toboolean(L, idx) != 0;
        case LUA_TNUMBER: return lua_tonumber(L, idx) != 0;
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA: return lua_touserdata(L, idx) != NULL;
        case LUA_TSTRING: return *lua_tostring(L, idx) != 0;

        default: return false;
    }
    return false;
}



#endif
