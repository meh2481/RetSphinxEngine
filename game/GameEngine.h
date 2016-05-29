/*
	GameEngine source - GameEngine.h
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "Engine.h"

#include <vector>
#include <set>
#include "luainterface.h"

#include "3DObject.h"
#include "tinyxml2.h"

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
	//Important general-purpose game variables
	Vec3 CameraPos;
	//HUD* m_hud;
	bool m_bMouseGrabOnWindowRegain;
	float m_fDefCameraZ;	//Default position of camera on z axis
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
	string m_sLastScene;

protected:
	void frame(float dt);
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

	//Functions dealing with input and user IO - GameEngine_events.cpp
//	void hudSignalHandler(string sSignal);	//For handling signals that come from the HUD //TODO Rip out or fix
	void handleKeys();						//Poll the keyboard state and update the game accordingly
	
	//Functions dealing with loading/saving from XML - defined in GameEngine_xmlparse.cpp
	bool loadConfig(string sFilename);
	void saveConfig(string sFilename);
	obj* objFromXML(string sType, Point ptOffset = Point(0,0), Point ptVel = Point(0,0));
	void loadScene(string sXMLFilename);	//Load scene from file
	void readFixture(tinyxml2::XMLElement* fixture, b2Body* bod);	//Load a fixture from an XML element & add it to the given body
	
	//Other stuff in GameEngine.cpp
	void rumbleController(float strength, float sec, int priority = 0);	//Rumble the controller
	void spawnNewParticleSystem(string sFilename, Point ptPos);
	void addAfterUpdate(obj* o) {m_lAddLater.push_back(o);};	//Add an object after upating all the objects is done (so we don't error out when adding objects during an object's update function)
	void warpObjectToNode(obj* o, Node* n);
	
	//GameEngine_color.cpp functions
	void updateColors(float dt);
	void phaseColor(Color* src, Color dest, float time, bool bPingPong = false);
	void clearColors();
	
};

//void signalHandler(string sSignal); //Stub function for handling signals that come in from our HUD, and passing them on to the engine
//float myAbs(float v);	//Because stinking namespace stuff


