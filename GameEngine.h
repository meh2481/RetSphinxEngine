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
	friend class PonyLua;
private:
	//Important general-purpose game variables
	ttvfs::VFSHelper vfs;
	Vec3 CameraPos;
	HUD* m_hud;
	bool m_bMouseGrabOnWindowRegain;
	float32 m_fDefCameraZ;	//Default position of camera on z axis
	list<ColorPhase> m_ColorsChanging;
	SDL_Joystick *m_joy;
	SDL_Haptic* m_rumble;
	map<string, myCursor*> m_mCursors;
	
	//Game stuff!
	LuaInterface* Lua;
	myCursor* m_Cursor;
	
	//Testing stuff!
	//Object3D* testObj;
	//physSegment* m_sun;
	obj* ship;
	ParticleSystem* shipTrail;
	Point shipMoveVec;

protected:
	void frame(float32 dt);
	void draw();
	void init(list<commandlineArg> sArgs);
	void handleEvent(SDL_Event event);
	void pause();
	void resume();

public:
	//GameEngine.cpp functions - fairly generic 
	GameEngine(uint16_t iWidth, uint16_t iHeight, string sTitle, string sAppName, string sIcon, bool bResizable = false);
	~GameEngine();
	
	void setLua(LuaInterface* l)	{Lua = l;};
	
	bool _shouldSelect(b2Fixture* fix);

	void hudSignalHandler(string sSignal);	//For handling signals that come from the HUD
	void handleKeys();						//Poll the keyboard state and update the game accordingly
	Point worldPosFromCursor(Point cursorpos);	//Get the worldspace position of the given mouse cursor position
	Point worldMovement(Point cursormove);		//Get the worldspace transform of the given mouse transformation
	
	//Functions dealing with program defaults
	bool loadConfig(string sFilename);
	void saveConfig(string sFilename);
	
	//Other stuff in GameEngine.cpp
	obj* objFromXML(string sXMLFilename, Point ptOffset = Point(0,0), Point ptVel = Point(0,0));
	Rect getCameraView();		//Return the rectangle, in world position z=0, that the camera can see
	void rumbleController(float32 strength, float32 sec, bool priority = false);	//Rumble the controller, if certain conditions are met
	void spawnNewParticleSystem(string sFilename, Point ptPos);
	void loadScene(string sXMLFilename);	//Load scene from file
	void updateShip();
	
	//color.cpp functions
	void updateColors(float32 dt);
	void phaseColor(Color* src, Color dest, float time, bool bPingPong = false);
	void clearColors();
	
};

void signalHandler(string sSignal); //Stub function for handling signals that come in from our HUD, and passing them on to the engine
float myAbs(float v);	//Because stinking namespace stuff


#endif
