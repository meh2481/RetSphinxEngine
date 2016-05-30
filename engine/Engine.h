/*
	GameEngine header - Engine.h
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "globaldefs.h"
#include "Object.h"
#include "Text.h"
#include "HUD.h"
#include "ParticleSystem.h"
#include "MouseCursor.h"
#include "EngineContactListener.h"
#include "Node.h"
#include "DebugDraw.h"

class b2World;
class Image;

#define VELOCITY_ITERATIONS 8
#define PHYSICS_ITERATIONS 3

#define SDL_BUTTON_FORWARD	SDL_BUTTON_X2
#define SDL_BUTTON_BACK		SDL_BUTTON_X1

const float soundFreqDefault = 44100.0;

typedef struct
{
	string sSwitch, sValue;
} commandlineArg;

class DepthComparator
{
public:
	bool operator()(const ObjSegment* s1, const ObjSegment* s2)
	{
		return s1->depth < s2->depth;
	}
};

class Engine
{
private:
	//Variables for use by the engine
	string m_sTitle;
	string m_sAppName;
	string m_sIcon;
	SDL_Window* m_Window;
	list<commandlineArg> lCommandLine;
	b2World* m_physicsWorld;
	EngineContactListener m_clContactListener;
	DebugDraw m_debugDraw;
	bool m_bDebugDraw;
	bool m_bObjDebugDraw;
	Point m_ptCursorPos;
	bool  m_bShowCursor;
	float m_fFramerate;
	float m_fAccumulatedTime;
	float m_fTargetTime;
	list<Object*> m_lObjects;	//Object handler
	multiset<ObjSegment*, DepthComparator> m_lScenery;
	bool m_bQuitting;   //Stop the game if this turns true
	float m_fTimeScale;	//So we can scale time if we want
	uint16_t m_iWidth, m_iHeight;
	const Uint8 *m_iKeystates;	//Keep track of keys that are pressed/released so we can poll as needed
	int m_iNumScreenModes;	  //Number of screen modes that are available
	bool m_bFullscreen;
	bool m_bResizable;
	float m_fGamma;		//Overall screen brightness
	bool m_bPaused;			//If the game is paused due to not being focused
	bool m_bPauseOnKeyboardFocus;	//If the game pauses when keyboard focus is lost
	bool m_bSoundDied;  //If tyrsound fails to load, don't try to use it
	int m_iMSAA;		//Antialiasing (0x, 2x, 4x, 8x, etc)
	MouseCursor* m_cursor;
	bool m_bCursorShow;
	bool m_bCursorOutOfWindow;	//If the cursor is outside of the window, don't draw it
	list<ParticleSystem*> m_particles;
	map<string, Node*> m_nodes;
#ifdef _DEBUG
	bool m_bStepFrame;
	bool m_bSteppingPhysics;
#endif
	
	
	//multimap<string, FMOD_CHANNEL*> m_channels;
	//map<string, FMOD_SOUND*> m_sounds;
	//FMOD_SYSTEM* m_audioSystem;

	//Engine-use function definitions
	bool _frame();
	void _render();
	
	void setup_sdl();
	void setup_opengl();
	void _loadicon();					//Load icon and set window to have said icon

	Engine(){}; //Default constructor isn't callable

protected:

	//Functions to override in your own class definition
	virtual void frame(float dt) = 0;   //Function that's called every frame
	virtual void draw() = 0;	//Actual function that draws stuff
	virtual void init(list<commandlineArg> sArgs) = 0;	//So we can load all our images and such
	virtual void handleEvent(SDL_Event event) = 0;  //Function that's called for each SDL input event
	virtual void pause() = 0;	//Called when the window is deactivated
	virtual void resume() = 0;	//Called when the window is activated again
	virtual Object* objFromXML(string sXMLFilename, Point ptOffset, Point ptVel) = 0;	//Function called when an object should be created

public:
	//Constructor/destructor
	Engine(uint16_t iWidth, uint16_t iHeight, string sTitle, string sAppName, string sIcon, bool bResizable = false);
	~Engine();

	//Misc. methods
	void commandline(list<string> argv);	//Pass along commandline arguments for the engine to use
	void start();   //Runs engine and doesn't exit until the engine ends
	void quit() {m_bQuitting = true;};  //Stop the engine and quit nicely
	string getSaveLocation();		//Get good location to save config files/save files
	Rect getCameraView(Vec3 Camera);		//Return the rectangle, in world position z=0, that the camera can see
	Point worldPosFromCursor(Point cursorpos, Vec3 Camera);	//Get the worldspace position of the given mouse cursor position
	Point worldMovement(Point cursormove, Vec3 Camera);		//Get the worldspace transform of the given mouse transformation
	
	//Drawing functions
	Rect getScreenRect()	{Rect rc(0,0,getWidth(),getHeight()); return rc;};
	void drawDebug();
	void drawCursor();
	
	//Window functions - engine_window.cpp
	void changeScreenResolution(float w, float h);  //Change resolution mid-game and reload OpenGL textures as needed
	//void toggleFullscreen();							//Switch between fullscreen/windowed modes
	void setFullscreen(bool bFullscreen);				//Set fullscreen to true or false as needed
	void setInitialFullscreen() {SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);};
	bool isFullscreen()	{return m_bFullscreen;};
	bool isMaximized();	
	Point getWindowPos();	//Get the window position
	void setWindowPos(Point pos);	//Set window position
	void maximizeWindow();								//Call window manager to maximize application window
	void pauseOnKeyboard(bool p)	{m_bPauseOnKeyboardFocus = p;};
	bool pausesOnFocusLost()		{return m_bPauseOnKeyboardFocus;};
	uint16_t getWidth() {return m_iWidth;};
	uint16_t getHeight() {return m_iHeight;};
	
	//Sound functions - engine_sound.cpp
	void createSound(string sPath, string sName);   //Creates a sound from this name and file path
	virtual void playSound(string sName, float volume = 1.0f, float pan = 0.0f, float pitch = 1.0f);	 //Play a sound
	//FMOD_CHANNEL* getChannel(string sSoundName);	//Return the channel of this sound
	void playMusic(string sName, float volume = 1.0f, float pan = 0.0f, float pitch = 1.0f);	 //Play looping music, or resume paused music
	void musicLoop(float startSec, float endSec);	//Set the starting and ending loop points for the currently-playing song
	void pauseMusic();									//Pause music that's currently playing
	void resumeMusic();									//Resume music that was paused
	void restartMusic();
	void stopMusic();
	void seekMusic(float fTime);
	float getMusicPos();								//Opposite of seekMusic() -- get where we currently are
	void volumeMusic(float fVol);						//Set the music to a particular volume
	void setMusicFrequency(float freq);
	float getMusicFrequency();
	bool hasMic();										//If we have some form of mic-like input
	void updateSound();
	
	//Keyboard functions
	bool keyDown(int32_t keyCode);  //Test and see if a key is currently pressed
	
	//Physics functions
	b2Body* createBody(b2BodyDef* bdef);
	void setGravity(Point ptGravity);
	void setGravity(float x, float y);
	void stepPhysics(float dt);	//Update our physics world and handle collisions
	void setDebugDraw(bool b) {m_bDebugDraw = b;};
	bool getDebugDraw() {return m_bDebugDraw;};
	void setObjDebugDraw(bool b) {m_bObjDebugDraw = b;};
	bool getObjDebugDraw() {return m_bObjDebugDraw;};
	void toggleDebugDraw() {m_bDebugDraw = !m_bDebugDraw;};
	void toggleObjDebugDraw() {m_bObjDebugDraw = !m_bObjDebugDraw;};
	b2World* getWorld() {return m_physicsWorld;};
#ifdef _DEBUG
	void playPausePhysics()	{m_bSteppingPhysics = !m_bSteppingPhysics;};
	void pausePhysics()		{m_bSteppingPhysics = true;};
	void stepPhysics()		{m_bStepFrame = true;};
#endif
	
	//Mouse functions
	Point getCursorPos()	{return m_ptCursorPos;};
	void setCursorPos(int32_t x, int32_t y);
	//void setCursorPos(Point ptPos)  {setCursorPos(ptPos.x, ptPos.y);};
	bool getCursorDown(int iButtonCode);
	void showCursor()	{m_bCursorShow = true;};
	void hideCursor()	{m_bCursorShow = false;};
	void setCursor(MouseCursor* cur)	{m_cursor = cur;};
	bool isMouseGrabbed();
	void grabMouse(bool bGrab = true);
	
	//Time functions
	float getTimeScale()	{return m_fTimeScale;};
	void setTimeScale(float fScale)	{m_fTimeScale = fScale;};
	unsigned getTicks();
	float getSeconds();
	void setFramerate(float fFramerate);
	float getFramerate()   {return m_fFramerate;};
	
	//Object management functions - engine_obj.cpp
	void addObject(Object* o);
	void addScenery(ObjSegment* seg) 	{m_lScenery.insert(seg);};
	//void updateSceneryLayer(physSegment* seg);
	void drawAll();
	void cleanupObjects();
	void updateObjects(float dt);
	void addNode(Node* n);
	Node* getNode(string sNodeName);
	Object* getObject(Point p);	//Get first object at this point
	Object* getClosestObject(Point p);	//Get closest object to this point
	Node* getNode(Point p);		//Get first node at this point

	//OpenGL methods
	void setDoubleBuffered(bool bDoubleBuffered);
	bool getDoubleBuffered();
	void setVsync(int iVsync);
	int getVsync();
	int getMSAA()				{return m_iMSAA;};
	void setMSAA(int iMSAA);
	void setGamma(float fGamma)	{m_fGamma = fGamma;};
	float getGamma()				{return m_fGamma;};
	
	//Particle functions - engine_particle.cpp
	void addParticles(ParticleSystem* sys)	{if(sys)m_particles.push_back(sys);};
	void cleanupParticles();
	void drawParticles();
	void updateParticles(float dt);

};
