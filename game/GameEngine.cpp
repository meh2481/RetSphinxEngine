/*
	GameEngine source - GameEngine.cpp
	Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include <float.h>
#include <sstream>

//Keybinding stuff!
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

//#define DEBUG_INPUT

//For our engine functions to be able to call our Engine class functions
GameEngine* g_pGlobalEngine;
float32 g_fParticleFac;

void signalHandler(string sSignal)
{
	g_pGlobalEngine->hudSignalHandler(sSignal);
}

void spawnNewParticleSystem(string sFilename, Point ptPos)
{
	g_pGlobalEngine->spawnNewParticleSystem(sFilename, ptPos);
}

GameEngine::GameEngine(uint16_t iWidth, uint16_t iHeight, string sTitle, string sAppName, string sIcon, bool bResizable) : 
Engine(iWidth, iHeight, sTitle, sAppName, sIcon, bResizable)
{
	g_pGlobalEngine = this;
	
	//Set camera position for this game
	m_fDefCameraZ = -16;
	CameraPos.set(0,0,m_fDefCameraZ);
#ifdef DEBUG
	m_bMouseGrabOnWindowRegain = false;
#else
	m_bMouseGrabOnWindowRegain = true;
#endif
	showCursor();
	
	m_Cursor = new myCursor();
	m_Cursor->fromXML("res/cursor/arrow.xml");
	setCursor(m_Cursor);
	
	setTimeScale(DEFAULT_TIMESCALE);
	
	m_joy = NULL;
	m_rumble = NULL;
	player = NULL;
	
	//Keybinding stuff!
	JOY_AXIS_HORIZ = 0;
	JOY_AXIS_VERT = 1;
	JOY_AXIS_RT = 5;
	JOY_AXIS_TRIP = 20000;
	KEY_UP1 = SDL_SCANCODE_W;
	KEY_UP2 = SDL_SCANCODE_UP;
	KEY_DOWN1 = SDL_SCANCODE_S;
	KEY_DOWN2 = SDL_SCANCODE_DOWN;
	KEY_LEFT1 = SDL_SCANCODE_A;
	KEY_LEFT2 = SDL_SCANCODE_LEFT;
	KEY_RIGHT1 = SDL_SCANCODE_D;
	KEY_RIGHT2 = SDL_SCANCODE_RIGHT;
	KEY_ENTER1 = SDL_SCANCODE_SPACE;
	KEY_ENTER2 = SDL_SCANCODE_RETURN;
	
	//Apparently our Xbox drivers for different OS's can't agree on which buttons are which
	//TODO This doesn't seem to apply to me now in Win10. Check what was going on there.
/*#ifdef _WIN32
	JOY_BUTTON_BACK = 5;
	JOY_BUTTON_START = 4;
	JOY_BUTTON_X = 12;
	JOY_BUTTON_Y = 13;
	JOY_BUTTON_A = 10;
	JOY_BUTTON_B = 11;
	JOY_BUTTON_LB = 8;
	JOY_BUTTON_RB = 9;
	JOY_BUTTON_LSTICK = 6;
	JOY_BUTTON_RSTICK = 7;
	JOY_AXIS2_HORIZ = 2;
	JOY_AXIS2_VERT = 3;
	JOY_AXIS_LT = 4;
#else*/
	JOY_BUTTON_BACK = 6;
	JOY_BUTTON_START = 7;
	JOY_BUTTON_X = 2;
	JOY_BUTTON_Y = 3;
	JOY_BUTTON_A = 0;
	JOY_BUTTON_B = 1;
	JOY_BUTTON_LB = 4;
	JOY_BUTTON_RB = 5;
	JOY_BUTTON_LSTICK = 9;
	JOY_BUTTON_RSTICK = 10;
	JOY_AXIS2_HORIZ = 3;
	JOY_AXIS2_VERT = 4;
	JOY_AXIS_LT = 2;
//#endif
	g_fParticleFac = 1.0f;
}

GameEngine::~GameEngine()
{
	errlog << "~GameEngine()" << endl;
	saveConfig(getSaveLocation() + "config.xml");
	cleanupObjects();
	delete m_Cursor;
	//delete m_lTest;
	//delete m_lAnimTest;
	//delete testObj;
}

void GameEngine::frame(float32 dt)
{
	handleKeys();
	stepPhysics(dt);
	updateParticles(dt);
	updateObjects(dt);
	
	//Add any objects created during updating this frame
	for(list<obj*>::iterator i = m_lAddLater.begin(); i != m_lAddLater.end(); i++)
		addObject(*i);
	m_lAddLater.clear();
	
	//m_lAnimTest->update(dt);
	
	
	//m_sun->pos.x = cos(DEG2RAD*fSunRotAmt) * 50;
	//m_sun->depth = -sin(DEG2RAD*fSunRotAmt) * 50;
	//updateSceneryLayer(m_sun);
	
	//Load a new scene after updating if we've been told to
	if(m_sLoadScene.size())
	{
		//cout << "Load scene " << m_sLoadScene << ", " << m_sLoadNode << endl;
		reloadImages();
		loadScene(m_sLoadScene);
		m_sLoadScene.clear();
		if(m_sLoadNode.size())
		{
			//cout << "warp to node " << m_sLoadNode << endl;
			//Warp to node on map
			warpObjectToNode(player, getNode(m_sLoadNode));
			m_sLoadNode.clear();
		}
	}
}

#define CAMERA_ANGLE_RAD PI/2.0//1.152572

void GameEngine::draw()
{
	//Clear bg (not done with OpenGL funcs, cause of weird black frame glitch when loading stuff)
	glDisable(GL_CULL_FACE);	//Draw both sides of 2D objects (So we can flip images for free)
	glDisable(GL_LIGHTING);
	fillScreen(Color(0,0,0,1));
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(CameraPos.x, CameraPos.y, CameraPos.z);
	
	/*/Set mouse cursor to proper location
	for(map<string, myCursor*>::iterator i = m_mCursors.begin(); i != m_mCursors.end(); i++)
	{
		//i->second->pos = worldPosFromCursor(getCursorPos());
		if(i->first == "dir")
			i->second->rot = -RAD2DEG * atan2(i->second->pos.x, i->second->pos.y) + 90;
	}*/
	glColor4f(1,1,1,1);
	
	
	
	//Rotate sun point around planet
	//glRotatef(fSunRotAmt, 0.0f, 1.0f, 0.0f);
	
	//Set up OpenGL lights
	GLfloat lightPosition[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat lightAmbient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat lightDiffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightSpecular[]  = {1.0f, 1.0f, 1.0f, 1.0f};
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
	
	
	//Set up OpenGL materials
	GLfloat materialAmbient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat materialDiffuse[] = {1, 1, 1, 1};
	GLfloat materialSpecular[] = {0.6, 0.6, 0.6, 1};
	GLfloat materialEmission[] = {0, 0, 0, 1};
	GLfloat materialShininess = 50;
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialEmission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
		
	//Set up global OpenGL lighting
	GLfloat globalAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	
	//Keep camera within camera bounds
	if(rcSceneBounds.area())	//If it's not unset
	{
		Rect rcCam = getCameraView(CameraPos);
		//cout << "camera view: " << rectToString(rcCam) << endl;
		if(rcCam.left < rcSceneBounds.left)
		{
			CameraPos.x += rcSceneBounds.left - rcCam.left;
			rcCam = getCameraView(CameraPos);
		}
		if(rcCam.right > rcSceneBounds.right)
		{
			CameraPos.x -= rcCam.right - rcSceneBounds.right;
			rcCam = getCameraView(CameraPos);
		}
		if(rcCam.top < rcSceneBounds.top)
		{
			CameraPos.y += rcSceneBounds.top - rcCam.top;
			rcCam = getCameraView(CameraPos);
		}
		if(rcCam.bottom > rcSceneBounds.bottom)
		{
			CameraPos.y -= rcCam.bottom - rcSceneBounds.bottom;
			rcCam = getCameraView(CameraPos);
		}
	}
	
	glLoadIdentity();
	gluLookAt(-CameraPos.x, -CameraPos.y + cos(CAMERA_ANGLE_RAD)*CameraPos.z, -sin(CAMERA_ANGLE_RAD)*CameraPos.z, -CameraPos.x, -CameraPos.y, 0.0f, 0, 0, 1);
			
	
	glDisable(GL_LIGHTING);
	drawAll();	//Draw everything in one pass
	drawDebug();
	
	glLoadIdentity();
	glTranslatef(-CameraPos.x, -CameraPos.y, m_fDefCameraZ);		//translate back to put cursor in the right position
	Vec3 cam(CameraPos.x, CameraPos.y, m_fDefCameraZ);
	m_Cursor->pos = worldPosFromCursor(getCursorPos(), cam);
	drawCursor();
	
	glTranslatef(0, 0, m_fDefCameraZ);
	
}

void GameEngine::init(list<commandlineArg> sArgs)
{
	//Run through list for arguments we recognize
	for(list<commandlineArg>::iterator i = sArgs.begin(); i != sArgs.end(); i++)
		errlog << "Commandline argument. Switch: " << i->sSwitch << ", value: " << i->sValue << endl;
		
	//Load our last screen position and such
	loadConfig(getSaveLocation() + "config.xml");
	
	lua_State* L = Lua->getState();
	
	//Have to do this manually because non-constants?
	//TODO: Fix/move from here
	//TODO: Also update these on user key/joystick config
	luaSetGlobal(JOY_BUTTON_BACK);
	luaSetGlobal(JOY_BUTTON_START);
	luaSetGlobal(JOY_BUTTON_X);
	luaSetGlobal(JOY_BUTTON_Y);
	luaSetGlobal(JOY_BUTTON_A);
	luaSetGlobal(JOY_BUTTON_B);
	luaSetGlobal(JOY_BUTTON_LB);
	luaSetGlobal(JOY_BUTTON_RB);
	luaSetGlobal(JOY_BUTTON_LSTICK);
	luaSetGlobal(JOY_BUTTON_RSTICK);
	luaSetGlobal(JOY_AXIS_HORIZ);
	luaSetGlobal(JOY_AXIS_VERT);
	luaSetGlobal(JOY_AXIS2_HORIZ);
	luaSetGlobal(JOY_AXIS2_VERT);
	luaSetGlobal(JOY_AXIS_LT);
	luaSetGlobal(JOY_AXIS_RT);
	luaSetGlobal(JOY_AXIS_TRIP);
	luaSetGlobal(KEY_UP1);
	luaSetGlobal(KEY_UP2);
	luaSetGlobal(KEY_DOWN1);
	luaSetGlobal(KEY_DOWN2);
	luaSetGlobal(KEY_LEFT1);
	luaSetGlobal(KEY_LEFT2);
	luaSetGlobal(KEY_RIGHT1);
	luaSetGlobal(KEY_RIGHT2);
	luaSetGlobal(KEY_ENTER1);
	luaSetGlobal(KEY_ENTER2);
	
	//Set gravity to about normal
	//getWorld()->SetGravity(b2Vec2(0,-9.8));
	getWorld()->SetGravity(b2Vec2(0,0));
	
	loadScene("res/3d/solarsystem.scene.xml");
}


void GameEngine::hudSignalHandler(string sSignal)
{
	
}

void GameEngine::pause()
{
	pauseMusic();
}

void GameEngine::resume()
{
	resumeMusic();
}

void GameEngine::spawnNewParticleSystem(string sFilename, Point ptPos)
{
	ParticleSystem* pSys = new ParticleSystem();
	pSys->fromXML(sFilename);
	pSys->emitFrom.centerOn(ptPos);
	pSys->init();
	pSys->firing = true;
	
	addParticles(pSys);
}

void GameEngine::warpObjectToNode(obj* o, Node* n)
{
	if(o && n)
		o->setPosition(n->pos);
}





















