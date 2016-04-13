#include "luafuncs.h"
#include "luainterface.h"
#include "GameEngine.h"


extern GameEngine* g_pGlobalEngine;

//-----------------------------------------------------------------------------------------------------------
//Class for interfacing between GameEngine and Lua
//(defined here instead of inside header because of weird cross-inclusion stuff)
//-----------------------------------------------------------------------------------------------------------
class GameEngineLua
{
public:
	static void rumble(float32 fAmt, float32 len, int priority = 0)
	{
		g_pGlobalEngine->rumbleController(fAmt, len, priority);
	}
};




//-----------------------------------------------------------------------------------------------------------
// Lua->C++ casting functions and such
//-----------------------------------------------------------------------------------------------------------

struct ObjGlue
{
	void *ptr;
	ObjMainType maintype;
};

class LuaStackCheck
{
public:
	LuaStackCheck(lua_State *L, int offs = 0) : _L(L), _top(lua_gettop(L)), _offs(offs) {}
	~LuaStackCheck() 
	{ 
		int top = lua_gettop(_L);
		if(top != _top+_offs)
			errlog << "Uh, oh, lua stack baaaaaad times " << top << ", " << _top+_offs << endl;
#ifdef DEBUG
		assert(top == _top+_offs);
#endif
		//UNUSED(top);
	}
private:
	lua_State *_L;
	int _top;
	int _offs;
};


//Glue helper functions from fg
template <typename T1> T1* getObjPtr(ObjMainType ty, lua_State *L, int idx = 1, bool allowNil = false)
{
	ObjGlue *glue = static_cast<ObjGlue*>(lua_touserdata(L, idx));
	if(!glue)
	{
		if(allowNil && lua_isnoneornil(L, idx))
			return NULL;
		const char *tyname = lua_typename(L, lua_type(L, idx));
		lua_pushfstring(L, "Invalid userdata! Got type: %s", tyname);
		lua_error(L);
		//UNREACHABLE;
	}
	if(glue->maintype != ty)
	{
		lua_pushliteral(L, "Unexpected object type!");
		lua_error(L);
	}
	return static_cast<T1*>(glue->ptr);
}

// creates glue object and leaves it on the stack
// TODO: Improve this function or get new version from fgenesis
void pushObjPtr(ObjMainType ty, lua_State *L, void *ptr)
{
	LuaStackCheck chk(L, 1);

	ObjGlue *glue = NULL;
	if(ptr)
	{
		glue = static_cast<ObjGlue*>(lua_newuserdata(L, sizeof(ObjGlue))); // [glue]
		glue->ptr = ptr;
		glue->maintype = ty;

		// Create reverse mapping so that the userdata can be looked up given ptr only.
		lua_getfield(L, LUA_REGISTRYINDEX, "_objects");   // [glue][_objects]
		lua_pushvalue(L, -2);                             // [glue][_objects][glue]
		lua_rawsetp(L, -2, ptr);                          // [glue][_objects]   ; _objects[ptr] = glue
		lua_pop(L, 1);                                    // [glue]
	}
	else
		lua_pushnil(L);
}

//Functions for casting object types from Lua->C++
static Image *getImage(lua_State *L, int idx = 1, bool allowNil = false)
{
	return getObjPtr<Image>(OT_IMAGE, L, idx, allowNil);
}

//TODO: Other functions for casting


//-----------------------------------------------------------------------------------------------------------
// Lua interface functions (can be called from lua)
//-----------------------------------------------------------------------------------------------------------

luaFunc(rumblecontroller)	//rumblecontroller(float force, float sec) --force is range [0,1]
{
	float32 force = lua_tonumber(L, 1);
	float32 sec = lua_tonumber(L, 2);
	int priority = lua_tointeger(L, 3);
	GameEngineLua::rumble(force, sec, priority);
}

//TODO: lua call node to get properties (Node::values<> map)

template<typename T> T *getObj(lua_State *L, unsigned pos = 1)
{
	LuaObjGlue *glue = (LuaObjGlue*)lua_touserdata(L, pos);
	void *p = T::TYPE == glue->type ? glue->obj : NULL; // TODO ALSO ERROR CHECKS
	return (T*)p;
}

luaFunc(obj_setVelocity)
{
	obj *o = getObj<obj>(L);
	//o->setVelocity(lua_tonumber(L, 2), lua_tonumber(L, 3)); // TIODO WROTEI ME
	luaReturnSelf();
}




//-----------------------------------------------------------------------------------------------------------
// Lua function registerer
//-----------------------------------------------------------------------------------------------------------

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
