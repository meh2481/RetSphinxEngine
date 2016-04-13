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
	
	static void camera_centerOnXY(Point pt)
	{
		g_pGlobalEngine->CameraPos.x = -pt.x;
		g_pGlobalEngine->CameraPos.y = -pt.y;
	}
	
	static obj* xmlParseObj(string sClassName, Point ptOffset = Point(0,0), Point ptVel = Point(0,0))
	{
		return g_pGlobalEngine->objFromXML(sClassName, ptOffset, ptVel);
	}
	
	static void addObject(obj* o)
	{
		g_pGlobalEngine->addAfterUpdate(o);
	}
	
	static obj* getPlayerObject()
	{
		return g_pGlobalEngine->ship;
	}
	
	static void loadMap(string sMap, string sNode = "")
	{
		g_pGlobalEngine->m_sLoadScene = sMap;
		g_pGlobalEngine->m_sLoadNode = sNode;
	}
};




//-----------------------------------------------------------------------------------------------------------
// Lua->C++ casting functions and such
//-----------------------------------------------------------------------------------------------------------
template<typename T> T *getObj(lua_State *L, unsigned pos = 1)
{
	LuaObjGlue *glue = (LuaObjGlue*)lua_touserdata(L, pos);
	void *p = T::TYPE == glue->type ? glue->obj : NULL; // TODO ALSO ERROR CHECKS
	return (T*)p;
}

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

luaFunc(map_load)
{
	if(lua_isstring(L, 2))
		GameEngineLua::loadMap(lua_tostring(L, 1), lua_tostring(L, 2));
	else
		GameEngineLua::loadMap(lua_tostring(L, 1));
}

//-----------------------------------------------------------------------------------------------------------
// Object functions
//-----------------------------------------------------------------------------------------------------------

luaFunc(obj_setVelocity)	//TODO REMOVE OR IMPLEMENT
{
	obj *o = getObj<obj>(L);
	//o->setVelocity(lua_tonumber(L, 2), lua_tonumber(L, 3)); // TODO WRITE ME
	//cout << o->luaClass << endl;
	luaReturnNil();
}

luaFunc(obj_applyForce)	//void obj_applyForce(obj* o, float x, float y)
{
	obj *o = getObj<obj>(L);
	Point pForce(lua_tonumber(L,2),lua_tonumber(L, 3));
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
			b->ApplyForceToCenter(pForce, true);
	}
	luaReturnNil();
}

luaFunc(obj_getPos)		//float x, float y obj_getPos(obj* o)
{
	obj *o = getObj<obj>(L);
	Point pt(0,0);
	if(o)
	{
		b2Body* bod = o->getBody();
		if(bod != NULL)
			pt = bod->GetPosition();
	}
	luaReturnVec2(pt.x, pt.y);
}

luaFunc(obj_create) //obj* obj_create(string className, float xpos, float ypos, float xvel, float yvel)
{
	if(!lua_isstring(L,1)) luaReturnNil();
	
	Point ptPos(0,0);
	Point ptVel(0,0);
	if(lua_isnumber(L, 2))
		ptPos.x = lua_tonumber(L, 2);
	if(lua_isnumber(L, 3))
		ptPos.y = lua_tonumber(L, 3);
	if(lua_isnumber(L, 4))
		ptVel.x = lua_tonumber(L, 4);
	if(lua_isnumber(L, 5))
		ptVel.y = lua_tonumber(L, 5);
	obj* o = GameEngineLua::xmlParseObj(lua_tostring(L, 1), ptPos, ptVel);
	if(o)
	{
		GameEngineLua::addObject(o);
		lua_rawgetp(L, LUA_REGISTRYINDEX, o);
		return 1;
	}
	luaReturnNil();
}

luaFunc(obj_getPlayer)	//obj* obj_getPlayer()
{
	obj* o = GameEngineLua::getPlayerObject();
	if(o)
	{
		lua_rawgetp(L, LUA_REGISTRYINDEX, o);
		return 1;
	}
	luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Camera functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(camera_centerOnXY)	//camera_centerOnXY(float x, float y)
{
	Point pt(lua_tonumber(L, 1), lua_tonumber(L, 2));
	GameEngineLua::camera_centerOnXY(pt);
	luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Node functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(node_getProperty)	//string node_getProperty(Node* n, string propName)
{
	string s;
	Node* n = getObj<Node>(L);
	if(n)
	{
		string sProp = lua_tostring(L, 2);
		if(n->propertyValues.count(sProp))
			s = n->propertyValues[sProp];
	}
	luaReturnString(s);
}

luaFunc(node_getVec2Property)	//float x, float y node_getVec2Property(Node* n, string propName)
{
	Point pt(0,0);
	Node* n = getObj<Node>(L);
	if(n)
	{
		string sProp = lua_tostring(L, 2);
		if(n->propertyValues.count(sProp))
			pt = pointFromString(n->propertyValues[sProp]);
	}
	luaReturnVec2(pt.x, pt.y);
}

luaFunc(node_getPos)	//float x, float y node_getPos(Node* n)
{
	Point pos(0,0);
	Node* n = getObj<Node>(L);
	if(n)
		pos = n->pos;
	luaReturnVec2(pos.x, pos.y);
}

//-----------------------------------------------------------------------------------------------------------
// Lua function registerer
//-----------------------------------------------------------------------------------------------------------

static LuaFunctions s_functab[] =
{
	luaRegister(rumblecontroller),
	luaRegister(obj_setVelocity),
	luaRegister(obj_getPos),
	luaRegister(obj_create),
	luaRegister(obj_applyForce),
	luaRegister(obj_getPlayer),
	luaRegister(camera_centerOnXY),
	luaRegister(node_getProperty),
	luaRegister(node_getVec2Property),
	luaRegister(node_getPos),
	luaRegister(map_load),
	//luaRegister(),
	{NULL, NULL}
};

void lua_register_enginefuncs(lua_State *L)
{	
	for(unsigned int i = 0; s_functab[i].name; ++i)
		lua_register(L, s_functab[i].name, s_functab[i].func);
}
