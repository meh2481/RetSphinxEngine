/*
	GameEngine source - GameEngine.h
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "Engine.h"

#include <vector>
#include <set>
#include "LuaInterface.h"

#include "Mesh3D.h"
#include "Font.h"
#include "tinyxml2.h"

class DebugUI;

#define DEFAULT_WIDTH	800
#define DEFAULT_HEIGHT	600

class ColorPhase
{
public:
	Color* colorToChange;
	bool pingpong;
	float destr, destg, destb;
	float srcr, srcg, srcb;
	float amtr, amtg, amtb;
	bool dir;
};

class GameEngine : public Engine
{
	friend class GameEngineLua;
private:
	//Keybinding stuff
	//TODO Keybinding class
	uint32_t JOY_BUTTON_BACK;
	uint32_t JOY_BUTTON_START;
	uint32_t JOY_BUTTON_X;
	uint32_t JOY_BUTTON_Y;
	uint32_t JOY_BUTTON_A;
	uint32_t JOY_BUTTON_B;
	uint32_t JOY_BUTTON_LB;
	uint32_t JOY_BUTTON_RB;
	uint32_t JOY_BUTTON_LSTICK;
	uint32_t JOY_BUTTON_RSTICK;
	uint32_t JOY_AXIS_HORIZ;
	uint32_t JOY_AXIS_VERT;
	uint32_t JOY_AXIS2_HORIZ;
	uint32_t JOY_AXIS2_VERT;
	uint32_t JOY_AXIS_LT;
	uint32_t JOY_AXIS_RT;
	int32_t JOY_AXIS_TRIP;
	SDL_Scancode KEY_UP1;
	SDL_Scancode KEY_UP2;
	SDL_Scancode KEY_DOWN1;
	SDL_Scancode KEY_DOWN2;
	SDL_Scancode KEY_LEFT1;
	SDL_Scancode KEY_LEFT2;
	SDL_Scancode KEY_RIGHT1;
	SDL_Scancode KEY_RIGHT2;
	SDL_Scancode KEY_ENTER1;
	SDL_Scancode KEY_ENTER2;

	//Important general-purpose game variables!
	Vec3 CameraPos;
	bool m_bMouseGrabOnWindowRegain;
	float m_fDefCameraZ;	//Default position of camera on z axis
	std::list<ColorPhase> m_ColorsChanging;
	SDL_Joystick *m_joy;	//TODO: Handle more than one gamepad at a time, also manage with Engine
	SDL_Haptic* m_rumble;
	
	//Game-specific stuff!
	LuaInterface* Lua;
	Object* player;		//TODO Support multiple players
	Rect rcSceneBounds;
	
	//Testing stuff that should eventually be finalized/fixed!
	std::string m_sLoadScene;	//If this is ever set, on the next frame we'll load this map
	std::string m_sLoadNode;		//If the above is set and this is also set, warp to this named node when loading the map
	std::string m_sLastScene;

	DebugUI *m_debugUI;

protected:
	void frame(float dt);
	void draw();
	void init(std::list<commandlineArg> sArgs);
	void pause();
	void resume();
	
	//Defined in GameEngine_events.cpp
	void handleEvent(SDL_Event event);

public:
	//GameEngine.cpp functions - fairly generic 
	GameEngine(uint16_t iWidth, uint16_t iHeight, std::string sTitle, std::string sAppName, std::string sIcon, bool bResizable = false);
	~GameEngine();
	
	void setLua(LuaInterface* l)	{Lua = l;};

	//Functions dealing with input and user IO - GameEngine_events.cpp
	void handleKeys();						//Poll the keyboard state and update the game accordingly
	
	//Functions dealing with loading/saving from XML - defined in GameEngine_xmlparse.cpp
	bool loadConfig(std::string sFilename);
	void saveConfig(std::string sFilename);
	void loadScene(std::string sXMLFilename);	//Load scene from file
	
	//Other stuff in GameEngine.cpp
	void rumbleController(float strength, float sec, int priority = 0);	//Rumble the controller
	void warpObjectToNode(Object* o, Node* n);
	
	//GameEngine_color.cpp functions
	void updateColors(float dt);
	void phaseColor(Color* src, Color dest, float time, bool bPingPong = false);
	void clearColors();
	
};


