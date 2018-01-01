#include "LuaFuncs.h"
#include "LuaInterface.h"
#include "GameEngine.h"
#include "ResourceLoader.h"
#include "EntityManager.h"
#include "ParticleSystem.h"
#include "StringUtils.h"
#include "SteelSeriesClient.h"
#include "InputDevice.h"
#include "InputManager.h"
#include "Action.h"
#include "Movement.h"
#include "SoundManager.h"
#include "ObjSegment.h"
#include "Object.h"
#include "Node.h"
#include "EngineContactListener.h"

//Defined by SDL
#define JOY_AXIS_MIN    -32768
#define JOY_AXIS_MAX    32767

extern GameEngine* g_pGlobalEngine;

//-----------------------------------------------------------------------------------------------------------
//Class for interfacing between GameEngine and Lua
//(defined here instead of inside header because of weird cross-inclusion stuff)
//-----------------------------------------------------------------------------------------------------------
class GameEngineLua
{
public:

    static void camera_centerOnXY(Vec2 pt)
    {
        g_pGlobalEngine->cameraPos.x = -pt.x;
        g_pGlobalEngine->cameraPos.y = -pt.y;
    }

    static void camera_getPos(float* x, float* y, float* z)
    {
        *x = g_pGlobalEngine->cameraPos.x;
        *y = g_pGlobalEngine->cameraPos.y;
        *z = g_pGlobalEngine->cameraPos.z;
    }

    static void camera_setPos(float x, float y, float z)
    {
        g_pGlobalEngine->cameraPos.x = x;
        g_pGlobalEngine->cameraPos.y = y;
        g_pGlobalEngine->cameraPos.z = z;
    }

    static Object* xmlParseObj(const std::string& sClassName, Vec2 ptOffset = Vec2(0, 0), Vec2 ptVel = Vec2(0, 0))
    {
        Object* o = g_pGlobalEngine->getResourceLoader()->getObject(sClassName, ptOffset, ptVel, g_pGlobalEngine->Lua);
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

    static void loadMap(const std::string& sMap, const std::string& sNode = "")
    {
        g_pGlobalEngine->m_sLoadScene = sMap;
        g_pGlobalEngine->m_sLoadNode = sNode;
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
        return g_pGlobalEngine->worldPosFromCursor(p, g_pGlobalEngine->cameraPos);
    }

    static ParticleSystem* createParticles(const std::string& sName)
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

    static Node* getNode(const std::string& sNodeName)
    {
        return g_pGlobalEngine->getEntityManager()->getNode(sNodeName);
    }

    static Object* getClosestObject(Vec2 p)
    {
        return g_pGlobalEngine->getEntityManager()->getClosestObject(p);
    }

    static void setCursor(const std::string& s)
    {
        g_pGlobalEngine->setCursor(g_pGlobalEngine->getResourceLoader()->getCursor(s));
    }

    static void rumbleLR(uint32_t duration, uint16_t large, uint16_t small)
    {
        InputDevice* controller = g_pGlobalEngine->getInputManager()->getCurController();
        if(controller != NULL)
            controller->rumbleLR(duration, large, small, g_pGlobalEngine->getSeconds());
    }

    static void sendSSEvent(const std::string& eventId, int value)
    {
        g_pGlobalEngine->getSteelSeriesClient()->sendEvent(eventId, value);
    }

    static void bindSSEvent(const std::string& filename)
    {
        g_pGlobalEngine->getSteelSeriesClient()->bindEvent(g_pGlobalEngine->getResourceLoader()->getTextFile(filename));
    }

    static float getAnalogAction(int action)
    {
        return g_pGlobalEngine->getInputManager()->getAnalogAction((Action)action);
    }

    static bool getDigitalAction(int action)
    {
        return g_pGlobalEngine->getInputManager()->getDigitalAction((Action)action);
    }

    static Vec2 getMovement(int movement)
    {
        return g_pGlobalEngine->getInputManager()->getMovement((Movement)movement);
    }

    static int playSong(const std::string& songFilename, SoundGroup group = GROUP_MUSIC)
    {
        StreamHandle* mus = g_pGlobalEngine->getSoundManager()->loadStream(songFilename);
        Channel* channel = g_pGlobalEngine->getSoundManager()->playLoop(mus, group);
        int channelIdx = 0;
        channel->getIndex(&channelIdx);
        return channelIdx;
    }

    static void getSpectrum(int channel, int num, float* data)
    {
        Channel* ch = g_pGlobalEngine->getSoundManager()->getChannel(channel);
        g_pGlobalEngine->getSoundManager()->getSpectrum(ch, data, num);
    }

    static void getSpectrumL(int channel, int num, float* data)
    {
        Channel* ch = g_pGlobalEngine->getSoundManager()->getChannel(channel);
        g_pGlobalEngine->getSoundManager()->getSpectrumL(ch, data, num);
    }

    static void getSpectrumR(int channel, int num, float* data)
    {
        Channel* ch = g_pGlobalEngine->getSoundManager()->getChannel(channel);
        g_pGlobalEngine->getSoundManager()->getSpectrumR(ch, data, num);
    }

    static int getMusicChannelIndex()
    {
        Channel* ch = g_pGlobalEngine->getSoundManager()->getMusicChannel();
        if(ch)
        {
            int channel = -1;
            ch->getIndex(&channel);
            return channel;
        }
        return -1;
    }

    static Channel* getMusicChannel()
    {
        return g_pGlobalEngine->getSoundManager()->getMusicChannel();
    }

    static int playSound(const std::string& soundFilename, SoundGroup group = GROUP_SFX)
    {
        SoundHandle* sound = g_pGlobalEngine->getSoundManager()->loadSound(soundFilename);
        Channel* channel = g_pGlobalEngine->getSoundManager()->playSound(sound, group);
        int channelIdx = 0;
        channel->getIndex(&channelIdx);
        return channelIdx;
    }

    static void preloadSound(const std::string& soundFilename)
    {
        g_pGlobalEngine->getSoundManager()->loadSound(soundFilename);
    }

    static Vec3 getHeadMovement()
    {
        return g_pGlobalEngine->getInputManager()->getHeadMovement();
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
luaFunc(controller_rumbleLR) //void controller_rumbleLR(uint msec, float forceLarge, float forceSmall)
{
    int msec = (int)lua_tointeger(L, 1);
    float forceLarge = (float)lua_tonumber(L, 2);
    float forceSmall = (float)lua_tonumber(L, 3);

    GameEngineLua::rumbleLR(msec, (uint16_t)(forceLarge * USHRT_MAX), (uint16_t)(forceSmall * USHRT_MAX));
    luaReturnNil();
}

luaFunc(map_load)    //void map_load(string sFilename, string sNodeToPlacePlayerAt = "")
{
    if(lua_isstring(L, 2))
        GameEngineLua::loadMap(lua_tostring(L, 1), lua_tostring(L, 2));
    else
        GameEngineLua::loadMap(lua_tostring(L, 1));
    luaReturnNil();
}

luaFunc(getFramerate)    //float getFramerate()
{
    luaReturnNum(GameEngineLua::getFramerate());
}

//-----------------------------------------------------------------------------------------------------------
// Object functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(obj_setAngle)    //void obj_setAngle(Object* o, float angle)
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

luaFunc(obj_getAngle)    //float obj_getAngle(Object* o)
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

luaFunc(obj_setVelocity)    //void obj_setVelocity(Object* o, float xvel, float yvel)
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

luaFunc(obj_getVelocity)    //float x, y obj_getVelocity(Object* o)
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

luaFunc(obj_applyForce)    //void obj_applyForce(Object* o, float x, float y)
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

luaFunc(obj_getPos)        //float x, float y obj_getPos(Object* o)
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

luaFunc(obj_setPos)    //void obj_setPos(Object* o, float x, float y)
{
    Object *o = getObj<Object>(L);
    Vec2 p(lua_tonumber(L, 2), lua_tonumber(L, 3));
    if(o)
        o->setPosition(p);
    luaReturnNil();
}

luaFunc(obj_create) //Object* obj_create(string className, float xpos, float ypos, float xvel, float yvel)
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
    //TODO: Seems to ignore vel
    Object* o = GameEngineLua::xmlParseObj(lua_tostring(L, 1), ptPos, ptVel);
    if(o)
    {
        GameEngineLua::addObject(o);
        luaReturnObj(o);
    }
    luaReturnNil();
}

luaFunc(obj_getPlayer)    //Object* obj_getPlayer()
{
    Object* o = GameEngineLua::getPlayerObject();
    if(o)
        luaReturnObj(o);
    luaReturnNil();
}

luaFunc(obj_registerPlayer)    //void obj_registerPlayer(Object* o)
{
    Object *o = getObj<Object>(L);
    if(o)
        GameEngineLua::setPlayerObject(o);
    luaReturnNil();
}

luaFunc(obj_getFromPoint) //Object* obj_getFromPoint(float x, float y)
{
    Vec2 p(lua_tonumber(L, 1), lua_tonumber(L, 2));
    Object* o = GameEngineLua::getObjAtPoint(p);
    if(o == NULL)
        luaReturnNil();
    luaReturnObj(o);
}

//Set physics off or on for an object's body
luaFunc(obj_setActive) //void obj_setActive(Object* o, bool b)
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

luaFunc(obj_isActive) //bool obj_isActive(Object* o)
{
    Object *o = getObj<Object>(L);
    if(o)
    {
        b2Body* bod = o->getBody();
        if(bod)
            luaReturnBool(bod->IsActive());
    }
    luaReturnBool(false);
}

luaFunc(obj_getProperty)    //string obj_getProperty(Object* o, string sProp)
{
    std::string s;
    Object *o = getObj<Object>(L);
    if(o)
        s = o->getProperty(lua_tostring(L, 2));
    luaReturnString(s);
}

luaFunc(obj_setImage)    //void obj_setImage(Object* o, string sImgFilename, int seg = 0)
{
    Object *o = getObj<Object>(L);
    if(o)
    {
        lua_Integer seg = 0;
        if(lua_isinteger(L, 3))
            seg = lua_tointeger(L, 3);
        o->setImage(g_pGlobalEngine->getResourceLoader()->getImage(lua_tostring(L, 2)), (unsigned int)seg);    //Lua remains 1-indexed, C++ side 0-indexed
    }
    luaReturnNil();
}

luaFunc(obj_destroy)    //void obj_destroy(Object* o)
{
    Object *o = getObj<Object>(L);
    if(o)
    {
        o->active = false;
        o->alive = false;
    }
    luaReturnNil();
}


//-----------------------------------------------------------------------------------------------------------
// Object segment functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(seg_getSize)    //float x, float y seg_getSize(Object* o, int idx)
{
    Object *o = getObj<Object>(L);
    if(o && lua_isinteger(L, 2))
    {
        ObjSegment* seg = o->getSegment((unsigned int)lua_tointeger(L, 2));
        if(seg)
            luaReturnVec2(seg->size.x, seg->size.y);
    }
    luaReturnNil();
}

luaFunc(seg_setSize)    //void seg_setSize(Object* o, int idx, float x, float y)
{
    Object *o = getObj<Object>(L);
    if(o && lua_isinteger(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4))
    {
        ObjSegment* seg = o->getSegment((unsigned int)lua_tointeger(L, 2));
        if(seg)
        {
            seg->size.x = (float)lua_tonumber(L, 3);
            seg->size.y = (float)lua_tonumber(L, 4);
        }
    }
    luaReturnNil();
}

luaFunc(seg_getPos)    //float x, float y seg_getPos(Object* o, int idx)
{
    Object *o = getObj<Object>(L);
    if(o && lua_isinteger(L, 2))
    {
        ObjSegment* seg = o->getSegment((unsigned int)lua_tointeger(L, 2));
        if(seg)
            luaReturnVec2(seg->pos.x, seg->pos.y);
    }
    luaReturnNil();
}

luaFunc(seg_setPos)    //void seg_setPos(Object* o, int idx, float x, float y)
{
    Object *o = getObj<Object>(L);
    if(o && lua_isinteger(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4))
    {
        ObjSegment* seg = o->getSegment((unsigned int)lua_tointeger(L, 2));
        if(seg)
        {
            seg->pos.x = (float)lua_tonumber(L, 3);
            seg->pos.y = (float)lua_tonumber(L, 4);
        }
    }
    luaReturnNil();
}

luaFunc(seg_getRot)    //float seg_getRot(Object* o, int idx)
{
    Object *o = getObj<Object>(L);
    if(o && lua_isinteger(L, 2))
    {
        ObjSegment* seg = o->getSegment((unsigned int)lua_tointeger(L, 2));
        if(seg)
            luaReturnNum(seg->rot);
    }
    luaReturnNil();
}

luaFunc(seg_setRot)    //void seg_setRot(Object* o, int idx, float angle)
{
    Object *o = getObj<Object>(L);
    if(o && lua_isinteger(L, 2) && lua_isnumber(L, 3))
    {
        ObjSegment* seg = o->getSegment((unsigned int)lua_tointeger(L, 2));
        if(seg)
            seg->rot = (float)lua_tonumber(L, 2);
    }
    luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Camera functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(camera_centerOnXY)    //camera_centerOnXY(float x, float y)
{
    Vec2 pt(lua_tonumber(L, 1), lua_tonumber(L, 2));
    GameEngineLua::camera_centerOnXY(pt);
    luaReturnNil();
}

luaFunc(camera_getPos)    //x,y,z camera_getPos()
{
    float x, y, z;
    GameEngineLua::camera_getPos(&x, &y, &z);
    luaReturnVec3(x, y, z);
}

luaFunc(camera_setPos)    //void camera_setPos(x,y,z)
{
    if(lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3))
    {
        float x = (float)lua_tonumber(L, 1);
        float y = (float)lua_tonumber(L, 2);
        float z = (float)lua_tonumber(L, 3);
        GameEngineLua::camera_setPos(x, y, z);
    }
    luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Node functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(node_getProperty)    //string node_getProperty(Node* n, string propName)
{
    std::string s;
    Node* n = getObj<Node>(L);
    if(n)
        s = n->getProperty(lua_tostring(L, 2));
    luaReturnString(s);
}

luaFunc(node_getVec2Property)    //float x, float y node_getVec2Property(Node* n, string propName)
{
    Vec2 pt(0, 0);
    Node* n = getObj<Node>(L);
    if(n)
        pt = pointFromString(n->getProperty(lua_tostring(L, 2)));
    luaReturnVec2(pt.x, pt.y);
}

luaFunc(node_getPos)    //float x, float y node_getPos(Node* n)
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
    lua_newtable(L);    //Create table
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
    return 1;    //return table
}

luaFunc(node_get)    //Node* node_get(string nodeName)
{
    Node* n = GameEngineLua::getNode(lua_tostring(L, 1));
    if(n)
        luaReturnObj(n);
    luaReturnNil();
}

//TODO: Doesn't work?
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
luaFunc(particles_create)    //ParticleSystem* particles_create(string sXMLFile)
{
    if(!lua_isstring(L, 1)) luaReturnNil();

    ParticleSystem* ps = GameEngineLua::createParticles(lua_tostring(L, 1));
    LuaInterface* LI = GameEngineLua::getLua();
    ps->glue = LI->createObject(ps, ps->TYPE, NULL);
    luaReturnObj(ps);
}

luaFunc(particles_setFiring)    //void particles_setFiring(ParticleSystem* p, bool fire)
{
    ParticleSystem* ps = getObj<ParticleSystem>(L);
    if(ps)
        ps->firing = (lua_toboolean(L, 2) != 0);
    luaReturnNil();
}

luaFunc(particles_setFireRate)    //void particles_setFirerate(ParticleSystem* p, float rate [0..1+])
{
    ParticleSystem* ps = getObj<ParticleSystem>(L);
    if(ps)
        ps->curRate = (float)lua_tonumber(L, 2);
    luaReturnNil();
}

luaFunc(particles_setEmitPos)    //void particles_setEmitPos(ParticleSystem* p, float x, float y)
{
    ParticleSystem* ps = getObj<ParticleSystem>(L);
    if(ps)
        ps->emitFrom.centerOn(Vec2((float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3)));
    luaReturnNil();
}

luaFunc(particles_setEmitVel)    //void particles_setEmitVel(ParticleSystem* p, float x, float y)
{
    ParticleSystem* ps = getObj<ParticleSystem>(L);
    if(ps)
    {
        ps->emissionVel.x = (float)lua_tonumber(L, 2);
        ps->emissionVel.y = (float)lua_tonumber(L, 3);
    }
    luaReturnNil();
}

luaFunc(particles_setEmitAngle)    //void particles_setEmitAngle(ParticleSystem* p, float a)
{
    ParticleSystem* ps = getObj<ParticleSystem>(L);
    if(ps)
        ps->emissionAngle = (float)lua_tonumber(L, 2);
    luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Input functions
//-----------------------------------------------------------------------------------------------------------
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

luaFunc(mouse_setCursor)    //void mouse_setCursor(string cursorFile)
{
    if(!lua_isstring(L, 1))
        luaReturnNil();

    GameEngineLua::setCursor(lua_tostring(L, 1));

    luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// SteelSeries events
//-----------------------------------------------------------------------------------------------------------
luaFunc(ss_bindEvent)    //void ss_bindEvent(string eventFilename)
{
    if(lua_isstring(L, 1))
    {
        GameEngineLua::bindSSEvent(lua_tostring(L, 1));
    }
    luaReturnNil();
}

luaFunc(ss_sendEvent)    //void ss_sendEvent(string eventId, int value)
{
    if(lua_isstring(L, 1) && lua_isinteger(L, 2))
    {
        GameEngineLua::sendSSEvent(lua_tostring(L, 1), (int)lua_tointeger(L, 2));
    }
    luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Actions
//-----------------------------------------------------------------------------------------------------------
luaFunc(action_analog)    //float action_analog(int actionId)    //return in range 0..1
{
    if(lua_isinteger(L, 1))
    {
        int action = (int)lua_tointeger(L, 1);
        if(action >= NUM_ACTIONS)
            luaReturnNum(0.0f);
        luaReturnNum(GameEngineLua::getAnalogAction(action));
    }
    luaReturnNum(0.0f);
}

luaFunc(action_digital)    //bool action_digital(int actionId)
{
    if(lua_isinteger(L, 1))
    {
        int action = (int)lua_tointeger(L, 1);
        if(action >= NUM_ACTIONS)
            luaReturnBool(false);
        luaReturnBool(GameEngineLua::getDigitalAction(action));
    }
    luaReturnBool(false);
}

luaFunc(movement_vec)    //x,y movement_vec(int movementId)
{
    if(lua_isinteger(L, 1))
    {
        int movement = (int)lua_tointeger(L, 1);
        if(movement >= NUM_MOVEMENTS)
            luaReturnVec2(0.0f, 0.0f);
        Vec2 v = GameEngineLua::getMovement(movement);
        luaReturnVec2(v.x, v.y);
    }
    luaReturnVec2(0.0f, 0.0f);
}

luaFunc(movement_head)    //x,y,z movement_head()
{
    Vec3 v = GameEngineLua::getHeadMovement();
    luaReturnVec3(v.x, v.y, v.z);
}

//-----------------------------------------------------------------------------------------------------------
// Sound functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(music_play)    //int music_play(string songPath, int soundGroup)
{
    SoundGroup group = GROUP_MUSIC;
    if(lua_isinteger(L, 2))
        group = (SoundGroup)lua_tointeger(L, 2);
    if(lua_isstring(L, 1))
        luaReturnInt(GameEngineLua::playSong(lua_tostring(L, 1), group));
    luaReturnNil();
}

luaFunc(music_rewind)    //void music_rewind()
{
    Channel* ch = GameEngineLua::getMusicChannel();
    if(ch)
        ch->setPosition(0, FMOD_TIMEUNIT_MS);
    luaReturnNil();
}

luaFunc(music_getChannel)    //int music_getChannel(void)
{
    int channel = GameEngineLua::getMusicChannelIndex();
    if(channel > 0)
        luaReturnInt(channel);
    luaReturnNil();
}

luaFunc(music_spectrum) //float[] music_spectrum(int channel, int num)
{
    if(lua_isinteger(L, 1) && lua_isinteger(L, 2))
    {
        int channel = (int)lua_tointeger(L, 1);
        int num = (int)lua_tointeger(L, 2);
        int actualNum = num;
        if(actualNum < 64)    //FMOD minimum
            actualNum = 64;

        float* data = new float[actualNum];
        GameEngineLua::getSpectrum(channel, actualNum, data);
        for(int i = 0; i < num; i++)
            lua_pushnumber(L, data[i]);
        delete[] data;
        return num;
    }
    luaReturnNil();
}

luaFunc(music_spectrumR) //float[] music_spectrumR(int channel, int num)
{
    if(lua_isinteger(L, 1) && lua_isinteger(L, 2))
    {
        int channel = (int)lua_tointeger(L, 1);
        int num = (int)lua_tointeger(L, 2);
        int actualNum = num;
        if(actualNum < 64)    //FMOD minimum
            actualNum = 64;

        float* data = new float[actualNum];
        GameEngineLua::getSpectrumR(channel, actualNum, data);
        for(int i = 0; i < num; i++)
            lua_pushnumber(L, data[i]);
        delete[] data;
        return num;
    }
    luaReturnNil();
}

luaFunc(music_spectrumL) //float[] music_spectrumL(int channel, int num)
{
    if(lua_isinteger(L, 1) && lua_isinteger(L, 2))
    {
        int channel = (int)lua_tointeger(L, 1);
        int num = (int)lua_tointeger(L, 2);
        int actualNum = num;
        if(actualNum < 64)    //FMOD minimum
            actualNum = 64;

        float* data = new float[actualNum];
        GameEngineLua::getSpectrumL(channel, actualNum, data);
        for(int i = 0; i < num; i++)
            lua_pushnumber(L, data[i]);
        delete[] data;
        return num;
    }
    luaReturnNil();
}

luaFunc(music_getPos)    //double music_getPos()        //Return music pos in seconds
{
    Channel* ch = GameEngineLua::getMusicChannel();
    if(ch)
    {
        unsigned int positionMs;
        FMOD_RESULT result = ch->getPosition(&positionMs, FMOD_TIMEUNIT_MS);
        if(result == FMOD_OK)
        {
            double seconds = positionMs;
            seconds = seconds / 1000.0;
            luaReturnNum(seconds);
        }
    }
    luaReturnNil();
}

luaFunc(sound_play)    //int sound_play(string soundPath, int soundGroup)
{
    SoundGroup group = GROUP_SFX;
    if(lua_isinteger(L, 2))
        group = (SoundGroup)lua_tointeger(L, 2);
    if(lua_isstring(L, 1))
        luaReturnInt(GameEngineLua::playSound(lua_tostring(L, 1), group));
    luaReturnNil();
}

luaFunc(sound_preload)    //void sound_preload(string soundPath)
{
    if(lua_isstring(L, 1))
        GameEngineLua::preloadSound(lua_tostring(L, 1));
    luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Lua constants & functions registerer
//-----------------------------------------------------------------------------------------------------------
static LuaFunctions s_functab[] =
{
    //Actions (input)
    luaRegister(action_analog),
    luaRegister(action_digital),
    //Camera
    luaRegister(camera_centerOnXY),
    luaRegister(camera_getPos),
    luaRegister(camera_setPos),
    //Controller rumble
    luaRegister(controller_rumbleLR),
    //framerate
    luaRegister(getFramerate),
    //map
    luaRegister(map_load),
    //mouse input (deprecated)
    luaRegister(mouse_isDown),
    luaRegister(mouse_getPos),
    luaRegister(mouse_transformToWorld),
    luaRegister(mouse_setCursor),
    //Controller/kb player directional input
    luaRegister(movement_vec),
    luaRegister(movement_head),
    //Music
    luaRegister(music_getChannel),
    luaRegister(music_play),
    luaRegister(music_rewind),
    luaRegister(music_spectrumL),
    luaRegister(music_spectrumR),
    luaRegister(music_spectrum),
    luaRegister(music_getPos),
    //Nodes
    luaRegister(node_getProperty),
    luaRegister(node_getVec2Property),
    luaRegister(node_getPos),
    luaRegister(node_getCollidingObj),
    luaRegister(node_getNearestObj),
    luaRegister(node_get),
    luaRegister(node_isInside),
    //Objects
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
    luaRegister(obj_isActive),
    luaRegister(obj_getProperty),
    luaRegister(obj_setImage),
    luaRegister(obj_destroy),
    //particle fx
    luaRegister(particles_create),
    luaRegister(particles_setFiring),
    luaRegister(particles_setFireRate),
    luaRegister(particles_setEmitPos),
    luaRegister(particles_setEmitVel),
    luaRegister(particles_setEmitAngle),
    //Object segments
    luaRegister(seg_getSize),
    luaRegister(seg_setSize),
    luaRegister(seg_getPos),
    luaRegister(seg_setPos),
    luaRegister(seg_getRot),
    luaRegister(seg_setRot),
    //Sound functions
    luaRegister(sound_play),
    luaRegister(sound_preload),
    //Steelseries events
    luaRegister(ss_bindEvent),
    luaRegister(ss_sendEvent),

    {NULL, NULL}
};

static const struct
{
    const char *name;
    int value;
} luaConstantTable[] = {

    //Actions
    luaConstant(JUMP),
    luaConstant(RUN),
    luaConstant(SHIP_THRUST),
    luaConstant(EXAMINE),
    luaConstant(ATTACK),
    luaConstant(NOTE_UP),
    luaConstant(NOTE_DOWN),
    luaConstant(NOTE_LEFT),
    luaConstant(NOTE_RIGHT),

    //Movement actions
    luaConstant(MOVE),
    luaConstant(AIM),
    luaConstant(PAN),

    //Sound groups
    luaConstant(GROUP_MUSIC),
    luaConstant(GROUP_SFX),
    luaConstant(GROUP_BGFX),
    luaConstant(GROUP_VOX),

    //gfx
    luaConstant(GL_LIGHT0),
    luaConstant(GL_LIGHT1),
    luaConstant(GL_POSITION),
    luaConstant(GL_AMBIENT),
    luaConstant(GL_DIFFUSE),
    luaConstant(GL_SPECULAR),
    luaConstant(GL_FRONT_AND_BACK),
    luaConstant(GL_EMISSION),
    luaConstant(GL_SHININESS),
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
