/*
	GameEngine source - GameEngine.h
	Copyright (c) 2014 Mark Hutcheson
*/
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Engine.h"
#include "bg.h"
#include <vector>
#include <set>
#include "webcam.h"
#include "luainterface.h"
#include "arc.h"
#include "3DObject.h"

#define DEFAULT_WIDTH	800
#define DEFAULT_HEIGHT	600

#define DEFAULT_TIMESCALE	1.0

//Keybinding stuff!
extern uint32_t JOY_BUTTON_BACK;
extern uint32_t JOY_BUTTON_START;
extern uint32_t JOY_BUTTON_X;
extern uint32_t JOY_BUTTON_Y;
extern uint32_t JOY_BUTTON_A;
extern uint32_t JOY_BUTTON_B;
extern uint32_t JOY_BUTTON_LB;
extern uint32_t JOY_BUTTON_RB;
extern uint32_t JOY_BUTTON_LSTICK;
extern uint32_t JOY_BUTTON_RSTICK;
extern uint32_t JOY_AXIS_HORIZ;
extern uint32_t JOY_AXIS_VERT;
extern uint32_t JOY_AXIS2_HORIZ;
extern uint32_t JOY_AXIS2_VERT;
extern uint32_t JOY_AXIS_LT;
extern uint32_t JOY_AXIS_RT;
extern int32_t JOY_AXIS_TRIP;
#define JOY_MAX 32767
#define JOY_MIN -32768
extern SDL_Scancode KEY_UP1;
extern SDL_Scancode KEY_UP2;
extern SDL_Scancode KEY_DOWN1;
extern SDL_Scancode KEY_DOWN2;
extern SDL_Scancode KEY_LEFT1;
extern SDL_Scancode KEY_LEFT2;
extern SDL_Scancode KEY_RIGHT1;
extern SDL_Scancode KEY_RIGHT2;
extern SDL_Scancode KEY_ENTER1;
extern SDL_Scancode KEY_ENTER2;

class ColorPhase
{
public:
	Color* colorToChange;
	bool pingpong;
	float32 destr, destg, destb;
	float32 srcr, srcg, srcb;
	float32 amtr, amtg, amtb;
	bool dir;
};

class GameEngine : public Engine
{
	friend class GameEngineLua;
private:
	//Important general-purpose game variables
	Vec3 CameraPos;
	HUD* m_hud;
	bool m_bMouseGrabOnWindowRegain;
	float32 m_fDefCameraZ;	//Default position of camera on z axis
	list<ColorPhase> m_ColorsChanging;
	list<obj*> m_lAddLater;
	SDL_Joystick *m_joy;	//TODO: Handle more than one gamepad at a time
	SDL_Haptic* m_rumble;
	
	//Game stuff!
	LuaInterface* Lua;
	myCursor* m_Cursor;
	
	//Testing stuff!
	//Object3D* testObj;
	//physSegment* m_sun;
	obj* player;
	Rect rcSceneBounds;
	string m_sLoadScene;	//If this is ever set, on the next frame we'll load this map
	string m_sLoadNode;		//If the above is set and this is also set, warp to this named node when loading the map

protected:
	void frame(float32 dt);
	void draw();
	void init(list<commandlineArg> sArgs);
	void pause();
	void resume();
	
	//Defined in GameEngine_events.cpp
	void handleEvent(SDL_Event event);

public:
	//GameEngine.cpp functions - fairly generic 
	GameEngine(uint16_t iWidth, uint16_t iHeight, string sTitle, string sAppName, string sIcon, bool bResizable = false);
	~GameEngine();
	
	void setLua(LuaInterface* l)	{Lua = l;};
	
	bool _shouldSelect(b2Fixture* fix);

	//Functions dealing with input and user IO - GameEngine_events.cpp
	void hudSignalHandler(string sSignal);	//For handling signals that come from the HUD //TODO Rip out or fix
	void handleKeys();						//Poll the keyboard state and update the game accordingly
	
	//Functions dealing with loading/saving from XML - defined in GameEngine_xmlparse.cpp
	bool loadConfig(string sFilename);
	void saveConfig(string sFilename);
	obj* objFromXML(string sType, Point ptOffset = Point(0,0), Point ptVel = Point(0,0));
	void loadScene(string sXMLFilename);	//Load scene from file
	void readFixture(XMLElement* fixture, b2Body* bod);	//Load a fixture from an XML element & add it to the given body
	
	//Other stuff in GameEngine.cpp
	void rumbleController(float32 strength, float32 sec, int priority = 0);	//Rumble the controller
	void spawnNewParticleSystem(string sFilename, Point ptPos);
	void addAfterUpdate(obj* o) {m_lAddLater.push_back(o);};	//Add an object after upating all the objects is done (so we don't error out when adding objects during an object's update function)
	void warpObjectToNode(obj* o, Node* n);
	
	//GameEngine_color.cpp functions
	void updateColors(float32 dt);
	void phaseColor(Color* src, Color dest, float time, bool bPingPong = false);
	void clearColors();
	
};

void signalHandler(string sSignal); //Stub function for handling signals that come in from our HUD, and passing them on to the engine
float myAbs(float v);	//Because stinking namespace stuff


#endif
