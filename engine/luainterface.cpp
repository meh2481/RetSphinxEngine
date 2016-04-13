#include "luainterface.h"
#include "luadefines.h"
#include "luafuncs.h"
#include "globaldefs.h"

#include <assert.h>
#include <sstream>
#include <new>

static int the_panic (lua_State *L) {
    errlog << "PANIC: unprotected error in call to Lua API " << lua_tostring(L, -1) << endl;
	assert(false);
    return 0;  /* return to Lua to abort */
}


LuaInterface::LuaInterface(const char *script, int argc, const char * const *argv)
 : script(script), argc(argc), argv(argv), _lua(NULL)
{
#ifdef DEBUG
	cout
#else
	errlog
#endif
	<< "LuaInterface: Using " << LUA_RELEASE << endl;
}

LuaInterface::~LuaInterface()
{
    Shutdown();
}

void LuaInterface::Shutdown()
{
	if(_lua)
	{
		lua_close(_lua);
		_lua = NULL;
	}
}

void LuaInterface::GC()
{
    lua_gc(_lua, LUA_GCCOLLECT, 0);
}

unsigned int LuaInterface::MemUsed()
{
    return lua_gc(_lua, LUA_GCCOUNT, 0);
}

bool LuaInterface::Init()
{
	if(!_lua)
	{
		_lua = luaL_newstate();
		if(!_lua)
			return false;

		lua_atpanic(_lua, the_panic);

		// Lua internal functions
		luaL_openlibs(_lua);

		// Own functions.
		lua_register_enginefuncs(_lua);
	}

	if(luaL_loadfile(_lua, script) != LUA_OK)
	{
		const char *err = lua_tostring(_lua, -1);
		if(err)
			puts(err);
		return false;
	}

	// push args
	for(int i = 0; i < argc; ++i)
		lua_pushstring(_lua, argv[i]);

	return doCall(argc);
}
static std::string luaFormatStackInfo(lua_State *L, int level = 1)
{
    lua_Debug ar;
    std::ostringstream os;
    if (lua_getstack(L, level, &ar) && lua_getinfo(L, "Sln", &ar))
    {
        os << ar.short_src << ":" << ar.currentline
            << " ([" << ar.what << "] "  << ar.namewhat << " " << (ar.name ? ar.name : "(?)") << ")";
    }
    else
    {
        os << "???:0";
    }

    return os.str();
}

static void printCallstack(lua_State *L, const char *errmsg = "<unspecified error>")
{
	lua_Debug dummy;
	std::ostringstream os;
	os << "Lua Error: " << errmsg << std::endl;
	int level = 0;
	while(true)
	{
		int gotstack = lua_getstack(L, level, &dummy);
		if(!gotstack)
			break;
		os << luaFormatStackInfo(L, level) << "\n";
		++level;
	}
#ifdef DEBUG
	printf("%s\n", os.str().c_str());
#else
	errlog << os.str() << endl;
#endif
}

void LuaInterface::lookupFunc(const char *f)
{
    lua_getglobal(_lua, f);
}

void LuaInterface::lookupMethod(void *o, const char *func)
{
    int lty = lua_rawgetp(_lua, LUA_REGISTRYINDEX, o);
#ifdef DEBUG
	assert(lty == LUA_TUSERDATA);
#endif
    // now [Lglue]
    lty = lua_getfield(_lua, -1, func);
#ifdef DEBUG
	assert(lty == LUA_TFUNCTION);
#endif
    // now [Lglue][func]
    lua_insert(_lua, -2);
    // now [func][Lglue]
}

LuaObjGlue *LuaInterface::createObject(void *o, unsigned ty, const char *classname)
{
	LuaObjGlue *glue = new(lua_newuserdata(_lua, sizeof(LuaObjGlue))) LuaObjGlue(o, ty);
	// [Lglue] // Lua glue object - not the same as the glue pointer
	lua_createtable(_lua, 0, 8); // LOL GUESS
	// [Lglue][t]
	lua_pushvalue(_lua, -1);
	// [Lglue][t][t]
	lua_setfield(_lua, -2, "__index"); // t.__index = t
	// [Lglue][t]
	lua_pushvalue(_lua, -1);
	// [Lglue][t][t]
	lua_setfield(_lua, -2, "__newindex"); // t.__newindex = t
	// [Lglue][t]
	int lty = luaL_getmetatable(_lua, classname); // cls = REG[classname]
	assert(lty == LUA_TTABLE);
	// [Lglue][t][cls]
	lua_setmetatable(_lua, -2); // setmetatable(t, cls)
	// [Lglue][t]
	lua_setmetatable(_lua, -2); // setmetatable(Lglue, t)
	// [Lglue]
	lua_rawsetp(_lua, LUA_REGISTRYINDEX, o); // REG[o] = Lglue // Now we can access Lglue given o
	// []
	return glue;
}

void LuaInterface::deleteObject(LuaObjGlue *glue)
{
	lua_pushnil(_lua);
    lua_rawsetp(_lua, LUA_REGISTRYINDEX, glue->obj);
	glue->obj = NULL;
}

bool LuaInterface::doCall(int nparams, int nrets /* = 0 */)
{
    if(lua_pcall(_lua, nparams, nrets, 0) != LUA_OK)
    {
        printCallstack(_lua, getCStr(_lua, -1));
        lua_pop(_lua, 1);
        return false;
    }
    return true;
}

bool LuaInterface::callMethod(void *o, const char *func)
{
    lookupMethod(o, func);
    return doCall(0+1); // first parameter is self (aka o)
}

bool LuaInterface::callMethod(void *o, const char *func, void* other)
{
    lookupMethod(o, func);
	int lty = lua_rawgetp(_lua, LUA_REGISTRYINDEX, other);
#ifdef DEBUG
	assert(lty == LUA_TUSERDATA);
#endif
    return doCall(1+1); // first parameter is self (aka o)
}

bool LuaInterface::callMethod(void *o, const char *func, float a, float b)
{
    lookupMethod(o, func);
	lua_pushnumber(_lua, a);
	lua_pushnumber(_lua, b);
    return doCall(2+1); // first parameter is self (aka o)
}

bool LuaInterface::callMethod(void *o, const char *func, float a)
{
    lookupMethod(o, func);
	lua_pushnumber(_lua, a);
    return doCall(1+1); // first parameter is self (aka o)
}

bool LuaInterface::call(const char *func)
{
    lookupFunc(func);
    return doCall(0);
}

bool LuaInterface::call(const char *func, const char *a)
{
	lookupFunc(func);
	lua_pushstring(_lua, a);
	return doCall(1);
}

bool LuaInterface::call(const char *func, const char *a, const char *b, const char *c, const char *d, const char *e)
{
	lookupFunc(func);
	lua_pushstring(_lua, a);
	lua_pushstring(_lua, b);
	lua_pushstring(_lua, c);
	lua_pushstring(_lua, d);
	lua_pushstring(_lua, e);
	return doCall(5);
}

bool LuaInterface::call(const char *func, const char *a, const char *b, const char *c, const char *d)
{
	lookupFunc(func);
	lua_pushstring(_lua, a);
	lua_pushstring(_lua, b);
	lua_pushstring(_lua, c);
	lua_pushstring(_lua, d);
	return doCall(4);
}

bool LuaInterface::call(const char *func, const char *a, const char *b)
{
	lookupFunc(func);
	lua_pushstring(_lua, a);
	lua_pushstring(_lua, b);
	return doCall(2);
}

bool LuaInterface::call(const char *func, float f)
{
    lookupFunc(func);
    lua_pushnumber(_lua, f);
    return doCall(1);
}

bool LuaInterface::call(const char *func, int a, int b)
{
    lookupFunc(func);
    lua_pushinteger(_lua, a);
    lua_pushinteger(_lua, b);
    return doCall(2);
}

bool LuaInterface::call(const char *func, int a, int b, bool c)
{
	lookupFunc(func);
	lua_pushinteger(_lua, a);
	lua_pushinteger(_lua, b);
	lua_pushboolean(_lua, c);
	return doCall(3);
}

bool LuaInterface::call(const char *func, int a, int b, int c, int d)
{
	lookupFunc(func);
	lua_pushinteger(_lua, a);
	lua_pushinteger(_lua, b);
	lua_pushinteger(_lua, c);
	lua_pushinteger(_lua, d);
	return doCall(4);
}

bool LuaInterface::call(const char *func, int a, int b, int c, int d, int e)
{
	lookupFunc(func);
	lua_pushinteger(_lua, a);
	lua_pushinteger(_lua, b);
	lua_pushinteger(_lua, c);
	lua_pushinteger(_lua, d);
	lua_pushinteger(_lua, e);
	return doCall(5);
}
