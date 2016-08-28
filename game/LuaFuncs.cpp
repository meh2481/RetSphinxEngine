#include "LuaFuncs.h"
#include "LuaInterface.h"
#include "GameEngine.h"
#include "Image.h"
#include "ResourceLoader.h"
#include "EntityManager.h"
#include "ParticleSystem.h"
#include "StringUtils.h"
#include "SteelSeriesCommunicator.h"
using namespace std;

//Defined by SDL
#define JOY_AXIS_MIN	-32768
#define JOY_AXIS_MAX	32767

extern GameEngine* g_pGlobalEngine;

//-----------------------------------------------------------------------------------------------------------
//Class for interfacing between GameEngine and Lua
//(defined here instead of inside header because of weird cross-inclusion stuff)
//-----------------------------------------------------------------------------------------------------------
class GameEngineLua
{
public:
	static void rumble(float fAmt, float len, int priority = 0)
	{
		g_pGlobalEngine->rumbleController(fAmt, len, priority);
	}

	static void camera_centerOnXY(Vec2 pt)
	{
		g_pGlobalEngine->CameraPos.x = -pt.x;
		g_pGlobalEngine->CameraPos.y = -pt.y;
	}

	static Object* xmlParseObj(string sClassName, Vec2 ptOffset = Vec2(0, 0), Vec2 ptVel = Vec2(0, 0))
	{
		Object* o = g_pGlobalEngine->getResourceLoader()->objFromXML(sClassName, ptOffset, ptVel);
		if(o)
			o->lua = g_pGlobalEngine->Lua;	//TODO Better way to load lua
		return o;
	}

	static void addObject(Object* o)
	{
		g_pGlobalEngine->getEntityManager()->add(o);
	}

	static Object* getPlayerObject()
	{
		return g_pGlobalEngine->player;
	}

	static void setPlayerObject(Object* o)
	{
		g_pGlobalEngine->player = o;
	}

	static void loadMap(string sMap, string sNode = "")
	{
		g_pGlobalEngine->m_sLoadScene = sMap;
		g_pGlobalEngine->m_sLoadNode = sNode;
	}

	static bool keyDown(int key)
	{
		return g_pGlobalEngine->keyDown(key);
	}

	static bool joyDown(int button)
	{
		if(g_pGlobalEngine->m_controller)
			return (SDL_GameControllerGetButton(g_pGlobalEngine->m_controller, (SDL_GameControllerButton)button) > 0);
		return false;
	}

	static int joyAxis(int axis)
	{
		if(g_pGlobalEngine->m_controller)
			return SDL_GameControllerGetAxis(g_pGlobalEngine->m_controller, (SDL_GameControllerAxis)axis);
		return 0;
	}

	static Vec2 getMousePos()
	{
		return g_pGlobalEngine->getCursorPos();
	}

	static bool getMouseDown(int button)
	{
		return g_pGlobalEngine->getCursorDown(button);
	}

	static Vec2 getWorldMousePos(Vec2 p)
	{
		return g_pGlobalEngine->worldPosFromCursor(p, g_pGlobalEngine->CameraPos);
	}

	static ParticleSystem* createParticles(string sName)
	{
		ParticleSystem* pSys = g_pGlobalEngine->getResourceLoader()->getParticleSystem(sName);
		g_pGlobalEngine->getEntityManager()->add(pSys);
		return pSys;
	}

	static LuaInterface* getLua()
	{
		return g_pGlobalEngine->Lua;
	}

	static float getFramerate()
	{
		return g_pGlobalEngine->getFramerate();
	}

	static Object* getObjAtPoint(Vec2 p)
	{
		return g_pGlobalEngine->getEntityManager()->getObject(p);
	}

	static Node* getNodeAtPoint(Vec2 p)
	{
		return g_pGlobalEngine->getEntityManager()->getNode(p);
	}

	static b2World* getPhysWorld()
	{
		return g_pGlobalEngine->getWorld();
	}

	static Node* getNode(string sNodeName)
	{
		return g_pGlobalEngine->getEntityManager()->getNode(sNodeName);
	}

	static Object* getClosestObject(Vec2 p)
	{
		return g_pGlobalEngine->getEntityManager()->getClosestObject(p);
	}

	static void setCursor(string s)
	{
		g_pGlobalEngine->setCursor(g_pGlobalEngine->getResourceLoader()->getCursor(s));
	}

	static void rumbleLR(uint32_t duration, uint16_t large, uint16_t small)
	{
		g_pGlobalEngine->rumbleLR(duration, large, small);
	}

	static void rumbleMouse()
	{
		g_pGlobalEngine->steelSeriesCommunicator->sendTestEvent();
	}
};




//-----------------------------------------------------------------------------------------------------------
// Lua->C++ casting functions and such
//-----------------------------------------------------------------------------------------------------------
template<typename T> T *getObj(lua_State *L, unsigned pos = 1)
{
	LuaObjGlue *glue = (LuaObjGlue*)lua_touserdata(L, pos);
	void *p = T::TYPE == glue->type ? glue->obj : NULL; //FG TODO ALSO ERROR CHECKS
	return (T*)p;
}

//-----------------------------------------------------------------------------------------------------------
// Lua interface functions (can be called from lua)
// TODO Error checking in these
//-----------------------------------------------------------------------------------------------------------

luaFunc(controller_rumble)	//void controller_rumble(float force, float sec) --force is range [0,1]
{
	float force = (float)lua_tonumber(L, 1);
	float sec = (float)lua_tonumber(L, 2);
	int priority = (int)lua_tointeger(L, 3);
	GameEngineLua::rumble(force, sec, priority);
	luaReturnNil();
}

luaFunc(controller_rumbleLR) //void controller_rumbleLR(uint msec, float forceLarge, float forceSmall)
{
	int msec = lua_tointeger(L, 1);
	float forceLarge = (float)lua_tonumber(L, 2);
	float forceSmall = (float)lua_tonumber(L, 3);

	GameEngineLua::rumbleLR(msec, forceLarge * USHRT_MAX, forceSmall * USHRT_MAX);
	luaReturnNil();
}

luaFunc(map_load)	//void map_load(string sFilename, string sNodeToPlacePlayerAt = "")
{
	if(lua_isstring(L, 2))
		GameEngineLua::loadMap(lua_tostring(L, 1), lua_tostring(L, 2));
	else
		GameEngineLua::loadMap(lua_tostring(L, 1));
	luaReturnNil();
}

luaFunc(getFramerate)	//float getFramerate()
{
	luaReturnNum(GameEngineLua::getFramerate());
}

//-----------------------------------------------------------------------------------------------------------
// Object functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(obj_setAngle)	//void obj_setAngle(obj* o, float angle)
{
	Object *o = getObj<Object>(L);
	float f = (float)lua_tonumber(L, 2);
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
		{
			b2Vec2 p = b->GetPosition();
			b->SetTransform(p, f);
		}
	}
	luaReturnNil();
}

luaFunc(obj_getAngle)	//float obj_getAngle(obj* o)
{
	Object *o = getObj<Object>(L);
	float f = 0.0f;
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
			f = b->GetAngle();
	}
	luaReturnNum(f);
}

luaFunc(obj_setVelocity)	//void obj_setVelocity(obj* o, float xvel, float yvel)
{
	Object *o = getObj<Object>(L);
	b2Vec2 p((float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3));
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
			b->SetLinearVelocity(p);
	}
	luaReturnNil();
}

luaFunc(obj_getVelocity)	//float x, y obj_getVelocity(obj* o)
{
	b2Vec2 p(0, 0);
	Object *o = getObj<Object>(L);
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
			p = b->GetLinearVelocity();
	}
	luaReturnVec2(p.x, p.y);
}

luaFunc(obj_applyForce)	//void obj_applyForce(obj* o, float x, float y)
{
	Object *o = getObj<Object>(L);
	b2Vec2 pForce((float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3));
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
	Object *o = getObj<Object>(L);
	b2Vec2 pt(0, 0);
	if(o)
	{
		b2Body* bod = o->getBody();
		if(bod != NULL)
			pt = bod->GetPosition();
	}
	luaReturnVec2(pt.x, pt.y);
}

luaFunc(obj_setPos)	//void obj_setPos(obj* o, float x, float y)
{
	Object *o = getObj<Object>(L);
	Vec2 p(lua_tonumber(L, 2), lua_tonumber(L, 3));
	if(o)
		o->setPosition(p);
	luaReturnNil();
}

luaFunc(obj_create) //obj* obj_create(string className, float xpos, float ypos, float xvel, float yvel)
{
	if(!lua_isstring(L, 1)) luaReturnNil();

	Vec2 ptPos(0, 0);
	Vec2 ptVel(0, 0);
	if(lua_isnumber(L, 2))
		ptPos.x = (float)lua_tonumber(L, 2);
	if(lua_isnumber(L, 3))
		ptPos.y = (float)lua_tonumber(L, 3);
	if(lua_isnumber(L, 4))
		ptVel.x = (float)lua_tonumber(L, 4);
	if(lua_isnumber(L, 5))
		ptVel.y = (float)lua_tonumber(L, 5);
	Object* o = GameEngineLua::xmlParseObj(lua_tostring(L, 1), ptPos, ptVel);
	if(o)
	{
		GameEngineLua::addObject(o);
		luaReturnObj(o);
	}
	luaReturnNil();
}

luaFunc(obj_getPlayer)	//obj* obj_getPlayer()
{
	Object* o = GameEngineLua::getPlayerObject();
	if(o)
		luaReturnObj(o);
	luaReturnNil();
}

luaFunc(obj_registerPlayer)	//void obj_registerPlayer(obj* o)
{
	Object *o = getObj<Object>(L);
	if(o)
		GameEngineLua::setPlayerObject(o);
	luaReturnNil();
}

luaFunc(obj_getFromPoint) //obj* obj_getFromPoint(float x, float y)
{
	Vec2 p(lua_tonumber(L, 1), lua_tonumber(L, 2));
	Object* o = GameEngineLua::getObjAtPoint(p);
	if(o == NULL)
		luaReturnNil();
	luaReturnObj(o);
}

//Set physics off or on for an object's body
luaFunc(obj_setActive) //void obj_setActive(obj* o, bool b)
{
	Object *o = getObj<Object>(L);
	bool b = (lua_toboolean(L, 2) != 0);
	if(o)
	{
		b2Body* bod = o->getBody();
		if(bod)
			bod->SetActive(b);
		o->active = b;
	}
	luaReturnNil();
}

luaFunc(obj_getProperty)	//string obj_getProperty(obj* o, string sProp)
{
	string s;
	Object *o = getObj<Object>(L);
	if(o)
		s = o->getProperty(lua_tostring(L, 2));
	luaReturnString(s);
}

luaFunc(obj_setImage)	//void obj_setImage(obj o, string sImgFilename, int seg = 1)
{
	Object *o = getObj<Object>(L);
	if(o)
	{
		lua_Integer seg = 1;
		if(lua_isinteger(L, 3))
			seg = lua_tointeger(L, 3);
		o->setImage(g_pGlobalEngine->getResourceLoader()->getImage(lua_tostring(L, 2)), (unsigned int)seg - 1);	//Lua remains 1-indexed, C++ side 0-indexed
	}
	luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Camera functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(camera_centerOnXY)	//camera_centerOnXY(float x, float y)
{
	Vec2 pt(lua_tonumber(L, 1), lua_tonumber(L, 2));
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
		s = n->getProperty(lua_tostring(L, 2));
	luaReturnString(s);
}

luaFunc(node_getVec2Property)	//float x, float y node_getVec2Property(Node* n, string propName)
{
	Vec2 pt(0, 0);
	Node* n = getObj<Node>(L);
	if(n)
		pt = pointFromString(n->getProperty(lua_tostring(L, 2)));
	luaReturnVec2(pt.x, pt.y);
}

luaFunc(node_getPos)	//float x, float y node_getPos(Node* n)
{
	Vec2 pos(0, 0);
	Node* n = getObj<Node>(L);
	if(n)
		pos = n->pos;
	luaReturnVec2(pos.x, pos.y);
}

luaFunc(node_getNearestObj) //obj* node_getNearestObj(Node* n)
{
	Node* n = getObj<Node>(L);
	if(n)
	{
		Object* o = GameEngineLua::getClosestObject(n->pos);
		if(o)
			luaReturnObj(o);
	}
	luaReturnNil();
}

//Get a table of all objects colliding with this node
luaFunc(node_getCollidingObj) //obj[]* node_getCollidingObj(Node* n)
{
	Node* n = getObj<Node>(L);
	lua_newtable(L);	//Create table
	int cur = 0;
	if(n)
	{
		b2World* world = GameEngineLua::getPhysWorld();
		if(world != NULL)
		{
			for(b2Contact* contact = world->GetContactList(); contact != NULL; contact = contact->GetNext())
			{
				Collision coll = EngineContactListener::getCollision(contact);
				if(coll.nodeA == n && coll.objB != NULL)
				{
					//Push key/value pairs into table
					lua_pushnumber(L, cur++);
					lua_rawgetp(L, LUA_REGISTRYINDEX, coll.objB);
					lua_settable(L, -3);
				}
				if(coll.nodeB == n && coll.objA != NULL)
				{
					//Push key/value pairs into table
					lua_pushnumber(L, cur++);
					lua_rawgetp(L, LUA_REGISTRYINDEX, coll.objA);
					lua_settable(L, -3);
				}
			}
		}
	}
	return 1;	//return table
}

luaFunc(node_get)	//Node* node_get(string nodeName)
{
	Node* n = GameEngineLua::getNode(lua_tostring(L, 1));
	if(n)
		luaReturnObj(n);
	luaReturnNil();
}

luaFunc(node_isInside) //bool node_isInside(Node* n, float x, float y)
{
	bool bInside = false;
	Node* n = getObj<Node>(L);
	if(n)
	{
		Node* test = GameEngineLua::getNodeAtPoint(Vec2(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		if(test && test == n)
			bInside = true;
	}
	luaReturnBool(bInside);
}

//-----------------------------------------------------------------------------------------------------------
// Particle functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(particles_create)	//ParticleSystem* particles_create(string sXMLFile)
{
	if(!lua_isstring(L, 1)) luaReturnNil();

	ParticleSystem* ps = GameEngineLua::createParticles(lua_tostring(L, 1));
	LuaInterface* LI = GameEngineLua::getLua();
	ps->glue = LI->createObject(ps, ps->TYPE, NULL);
	luaReturnObj(ps);
}

luaFunc(particles_setFiring)	//void particles_setFiring(ParticleSystem* p, bool fire)
{
	ParticleSystem* ps = getObj<ParticleSystem>(L);
	if(ps)
		ps->firing = (lua_toboolean(L, 2) != 0);
	luaReturnNil();
}

luaFunc(particles_setFireRate)	//void particles_setFirerate(ParticleSystem* p, float rate [0..1+])
{
	ParticleSystem* ps = getObj<ParticleSystem>(L);
	if(ps)
		ps->curRate = (float)lua_tonumber(L, 2);
	luaReturnNil();
}

luaFunc(particles_setEmitPos)	//void particles_setEmitPos(ParticleSystem* p, float x, float y)
{
	ParticleSystem* ps = getObj<ParticleSystem>(L);
	if(ps)
		ps->emitFrom.centerOn(Vec2((float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3)));
	luaReturnNil();
}

luaFunc(particles_setEmitVel)	//void particles_setEmitVel(ParticleSystem* p, float x, float y)
{
	ParticleSystem* ps = getObj<ParticleSystem>(L);
	if(ps)
	{
		ps->emissionVel.x = (float)lua_tonumber(L, 2);
		ps->emissionVel.y = (float)lua_tonumber(L, 3);
	}
	luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Input functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(key_isDown) //bool key_isDown(SDL_Scancode key)
{
	luaReturnBool(GameEngineLua::keyDown((int)lua_tointeger(L, 1)));
}

luaFunc(joy_isDown) //bool joy_isDown(int button)
{
	luaReturnBool(GameEngineLua::joyDown((int)lua_tointeger(L, 1)));
}

luaFunc(joy_getAxis) //int joy_getAxis(int axis)
{
	luaReturnInt(GameEngineLua::joyAxis((int)lua_tointeger(L, 1)));
}

luaFunc(mouse_getPos) //int x, int y mouse_getPos()
{
	Vec2 p = GameEngineLua::getMousePos();
	luaReturnVec2(p.x, p.y);
}

luaFunc(mouse_isDown) //bool mouse_isDown(int button)
{
	if(!lua_isinteger(L, 1))
		luaReturnBool(GameEngineLua::getMouseDown(SDL_BUTTON_LEFT));
	luaReturnBool(GameEngineLua::getMouseDown((int)lua_tointeger(L, 1)));
}

luaFunc(mouse_transformToWorld) // float x, float y mouse_transformToWorld(int x, int y)
{
	Vec2 pMouse(lua_tointeger(L, 1), lua_tointeger(L, 2));
	Vec2 p = GameEngineLua::getWorldMousePos(pMouse);
	luaReturnVec2(p.x, p.y);
}

luaFunc(mouse_setCursor)	//void mouse_setCursor(string cursorFile)
{
	if(!lua_isstring(L, 1))
		luaReturnNil();

	GameEngineLua::setCursor(lua_tostring(L, 1));

	luaReturnNil();
}

luaFunc(mouse_rumble)
{
	GameEngineLua::rumbleMouse();
	luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Lua constants & functions registerer
//-----------------------------------------------------------------------------------------------------------
static LuaFunctions s_functab[] =
{
	luaRegister(controller_rumble),
	luaRegister(controller_rumbleLR),
	luaRegister(getFramerate),
	luaRegister(obj_setVelocity),
	luaRegister(obj_getVelocity),
	luaRegister(obj_getPos),
	luaRegister(obj_setPos),
	luaRegister(obj_create),
	luaRegister(obj_applyForce),
	luaRegister(obj_getPlayer),
	luaRegister(obj_registerPlayer),
	luaRegister(obj_setAngle),
	luaRegister(obj_getAngle),
	luaRegister(obj_getFromPoint),
	luaRegister(obj_setActive),
	luaRegister(obj_getProperty),
	luaRegister(obj_setImage),
	luaRegister(camera_centerOnXY),
	luaRegister(node_getProperty),
	luaRegister(node_getVec2Property),
	luaRegister(node_getPos),
	luaRegister(node_getCollidingObj),
	luaRegister(node_getNearestObj),
	luaRegister(node_get),
	luaRegister(node_isInside),
	luaRegister(map_load),
	luaRegister(particles_create),
	luaRegister(particles_setFiring),
	luaRegister(particles_setFireRate),
	luaRegister(particles_setEmitPos),
	luaRegister(particles_setEmitVel),
	luaRegister(key_isDown),
	luaRegister(joy_isDown),
	luaRegister(joy_getAxis),
	luaRegister(mouse_isDown),
	luaRegister(mouse_getPos),
	luaRegister(mouse_transformToWorld),
	luaRegister(mouse_setCursor),
	luaRegister(mouse_rumble),
	{NULL, NULL}
};

//TODO: This is messed up; shouldn't be using SDL scancodes in Lua, shouldn't be defining joy axis min/max outside of where we define joy buttons
static const struct
{
	const char *name;
	int value;
} luaConstantTable[] = {

	//Joystick
	luaConstant(JOY_AXIS_MIN),
	luaConstant(JOY_AXIS_MAX),

	//Mouse
	luaConstant(SDL_BUTTON_LEFT),
	luaConstant(SDL_BUTTON_RIGHT),
	luaConstant(SDL_BUTTON_MIDDLE),
	luaConstant(SDL_BUTTON_FORWARD),
	luaConstant(SDL_BUTTON_BACK),

	//Keyboard
	luaConstant(SDL_SCANCODE_SPACE),
};

void lua_register_all(lua_State *L)
{
	//Register functions
	for(unsigned int i = 0; s_functab[i].name; ++i)
		lua_register(L, s_functab[i].name, s_functab[i].func);

	//Register constants
	for(unsigned int i = 0; i < sizeof(luaConstantTable) / sizeof(*luaConstantTable); i++)
	{
		lua_pushinteger(L, luaConstantTable[i].value);
		lua_setglobal(L, luaConstantTable[i].name);
	}
}
