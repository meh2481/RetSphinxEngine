/*
	Pony48 source - Engine.h
	Copyright (c) 2014 Mark Hutcheson
*/

#ifndef ENGINE_H
#define ENGINE_H

#include "globaldefs.h"
#include "Image.h"
#include "Object.h"
#include "Text.h"
#include "hud.h"
#include "particles.h"
#include "cursor.h"
#include <fmod.h>
#include <map>
#include <set>

#define LMB	1
#define RMB	0
#define MMB 2

const float soundFreqDefault = 44100.0;

typedef struct
{
	string sSwitch, sValue;
} commandlineArg;

class depthComparator
{
public:
	bool operator()(const physSegment* s1, const physSegment* s2)
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
	Point m_ptCursorPos;
	bool  m_bShowCursor;
	float32 m_fFramerate;
	float32 m_fAccumulatedTime;
	float32 m_fTargetTime;
	list<obj*> m_lObjects;	//Object handler
	multiset<physSegment*, depthComparator> m_lScenery;
	bool m_bQuitting;   //Stop the game if this turns true
	float32 m_fTimeScale;	//So we can scale time if we want
	uint16_t m_iWidth, m_iHeight;
	const Uint8 *m_iKeystates;	//Keep track of keys that are pressed/released so we can poll as needed
	int m_iNumScreenModes;	  //Number of screen modes that are available
	bool m_bFullscreen;
	bool m_bResizable;
	float32 m_fGamma;		//Overall screen brightness
	bool m_bPaused;			//If the game is paused due to not being focused
	bool m_bPauseOnKeyboardFocus;	//If the game pauses when keyboard focus is lost
	bool m_bSoundDied;  //If tyrsound fails to load, don't try to use it
	int m_iMSAA;		//Antialiasing (0x, 2x, 4x, 8x, etc)
	myCursor* m_cursor;
	bool m_bCursorShow;
	bool m_bCursorOutOfWindow;	//If the cursor is outside of the window, don't draw it
	list<ParticleSystem*> m_particles;
	
	multimap<string, FMOD_CHANNEL*> m_channels;
	map<string, FMOD_SOUND*> m_sounds;
	FMOD_SYSTEM* m_audioSystem;

	//Engine-use function definitions
	bool _frame();
	void _render();
	
	void setup_sdl();
	void setup_opengl();
	void _loadicon();					//Load icon and set window to have said icon

	Engine(){}; //Default constructor isn't callable

protected:

	//Functions to override in your own class definition
	virtual void frame(float32 dt) = 0;   //Function that's called every frame
	virtual void draw() = 0;	//Actual function that draws stuff
	virtual void init(list<commandlineArg> sArgs) = 0;	//So we can load all our images and such
	virtual void handleEvent(SDL_Event event) = 0;  //Function that's called for each SDL input event
	virtual void pause() = 0;	//Called when the window is deactivated
	virtual void resume() = 0;	//Called when the window is activated again
	virtual obj* objFromXML(string sXMLFilename, Point ptOffset, Point ptVel) = 0;	//Function called when an object should be created
	
	//Helper functions for your own class definition
	b2World* getWorld() {return m_physicsWorld;};

public:
	//Constructor/destructor
	Engine(uint16_t iWidth, uint16_t iHeight, string sTitle, string sAppName, string sIcon, bool bResizable = false);
	~Engine();

	//Misc. methods
	void commandline(list<string> argv);	//Pass along commandline arguments for the engine to use
	void start();   //Runs engine and doesn't exit until the engine ends
	void quit() {m_bQuitting = true;};  //Stop the engine and quit nicely
	string getSaveLocation();		//Get good location to save config files/save files
	
	//Drawing functions
	void fillRect(Point p1, Point p2, Color col);
	void fillScreen(Color col);		//Fill entire screen rect with color
	Rect getScreenRect()	{Rect rc(0,0,getWidth(),getHeight()); return rc;};
	
	//Window functions
	void changeScreenResolution(float32 w, float32 h);  //Change resolution mid-game and reload OpenGL textures as needed
	void toggleFullscreen();							//Switch between fullscreen/windowed modes
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
	
	//Sound functions
	void createSound(string sPath, string sName);   //Creates a sound from this name and file path
	virtual void playSound(string sName, float32 volume = 1.0f, float32 pan = 0.0f, float32 pitch = 1.0f);	 //Play a sound
	FMOD_CHANNEL* getChannel(string sSoundName);	//Return the channel of this sound
	void playMusic(string sName, float32 volume = 1.0f, float32 pan = 0.0f, float32 pitch = 1.0f);	 //Play looping music, or resume paused music
	void musicLoop(float32 startSec, float32 endSec);	//Set the starting and ending loop points for the currently-playing song
	void pauseMusic();									//Pause music that's currently playing
	void resumeMusic();									//Resume music that was paused
	void restartMusic();
	void stopMusic();
	void seekMusic(float32 fTime);
	float32 getMusicPos();								//Opposite of seekMusic() -- get where we currently are
	void volumeMusic(float32 fVol);						//Set the music to a particular volume
	void setMusicFrequency(float32 freq);
	float32 getMusicFrequency();
	bool hasMic();										//If we have some form of mic-like input
	void updateSound();
	
	//Keyboard functions
	bool keyDown(int32_t keyCode);  //Test and see if a key is currently pressed
	
	//Physics functions
	b2Body* createBody(b2BodyDef* bdef) {return m_physicsWorld->CreateBody(bdef);};
	void setGravity(Point ptGravity)	{m_physicsWorld->SetGravity(ptGravity);};
	void setGravity(float32 x, float32 y)   {setGravity(Point(x,y));};
	void stepPhysics(float32 dt)	{m_physicsWorld->Step(dt * m_fTimeScale, VELOCITY_ITERATIONS, PHYSICS_ITERATIONS);};
	
	//Mouse functions
	Point getCursorPos()	{return m_ptCursorPos;};
	void setCursorPos(int32_t x, int32_t y);
	void setCursorPos(Point ptPos)  {setCursorPos(ptPos.x, ptPos.y);};
	bool getCursorDown(int iButtonCode);
	void showCursor()	{m_bCursorShow = true;};
	void hideCursor()	{m_bCursorShow = false;};
	void setCursor(myCursor* cur)	{m_cursor = cur;};
	bool isMouseGrabbed()	{return SDL_GetWindowGrab(m_Window);};
	void grabMouse(bool bGrab = true) {SDL_SetWindowGrab(m_Window, (SDL_bool)bGrab);};
	
	//Time functions
	float32 getTimeScale()	{return m_fTimeScale;};
	void setTimeScale(float32 fScale)	{m_fTimeScale = fScale;};
	Uint32 getTicks()	{return SDL_GetTicks();};
	float32 getSeconds()	{return (float32)SDL_GetTicks()/1000.0;};
	void setFramerate(float32 fFramerate);
	float32 getFramerate()   {return m_fFramerate;};
	
	//Object functions
	void addObject(obj* o);
	void addScenery(physSegment* seg) 	{m_lScenery.insert(seg);};
	void drawObjects();
	void cleanupObjects();
	void updateObjects(float32 dt);

	//OpenGL methods
	void setDoubleBuffered(bool bDoubleBuffered)	{SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, bDoubleBuffered);};
	bool getDoubleBuffered()	{int val = 1; SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &val); return val;};
	void setVsync(int iVsync)	{SDL_GL_SetSwapInterval(iVsync);};
	int getVsync()				{return SDL_GL_GetSwapInterval();};
	int getMSAA()				{return m_iMSAA;};
	void setMSAA(int iMSAA);
	bool getImgBlur()			{return g_imageBlur;};
	void setImgBlur(bool b)		{g_imageBlur = b;};
	void setGamma(float32 fGamma)	{m_fGamma = fGamma;};
	float32 getGamma()				{return m_fGamma;};
	
	//Particle functions
	void addParticles(ParticleSystem* sys)	{if(sys)m_particles.push_back(sys);};
	void cleanupParticles();
	void drawParticles();
	void updateParticles(float32 dt);

};

#endif
