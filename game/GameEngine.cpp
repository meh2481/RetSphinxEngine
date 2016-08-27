/*
	GameEngine source - GameEngine.cpp
	Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include <float.h>
#include <sstream>
#include "Image.h"
#include "opengl-api.h"
#include "easylogging++.h"
#include "DebugUI.h"
#include "ResourceLoader.h"
#include "EntityManager.h"
#include "Stringbank.h"
#include "SystemUtils.h"
#include "SteelSeriesCommunicator.h"
#include "NetworkThread.h"
using namespace std;

//#define DEBUG_INPUT

//For our engine functions to be able to call our Engine class functions
GameEngine* g_pGlobalEngine;
float g_fParticleFac;

GameEngine::GameEngine(uint16_t iWidth, uint16_t iHeight, string sTitle, string sAppName, string sIcon, bool bResizable) : 
Engine(iWidth, iHeight, sTitle, sAppName, sIcon, bResizable)
{
	g_pGlobalEngine = this;
	
	//Set camera position for this game
	m_fDefCameraZ = -16;
	CameraPos = Vec3(0,0,m_fDefCameraZ);
#ifdef _DEBUG
	m_bMouseGrabOnWindowRegain = false;
#else
	m_bMouseGrabOnWindowRegain = true;
#endif
	showCursor();
	
	m_controller = NULL;
	m_rumble = NULL;
	player = NULL;
	
	//Keybinding stuff!
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

	g_fParticleFac = 1.0f;

	m_debugUI = new DebugUI(this);

	steelSeriesCommunicator = new SteelSeriesCommunicator();
}

GameEngine::~GameEngine()
{
	LOG(INFO) << "~GameEngine()";
	saveConfig(getSaveLocation() + "config.xml");
	getEntityManager()->cleanup();
	delete m_debugUI;
	delete steelSeriesCommunicator;
	NetworkThread::stop();
}

void GameEngine::frame(float dt)
{
	handleKeys();

	stepPhysics(dt);
	getEntityManager()->update(dt);
	steelSeriesCommunicator->update(dt);
	
	//Load a new scene after updating if we've been told to
	if(m_sLoadScene.size())
	{
		loadScene(m_sLoadScene);
		m_sLoadScene.clear();
		if(m_sLoadNode.size())
		{
			//Warp to node on map
			warpObjectToNode(player, getEntityManager()->getNode(m_sLoadNode));
			m_sLoadNode.clear();
		}
	}
}

void GameEngine::draw()
{
	//Clear bg (not done with OpenGL funcs, cause of weird black frame glitch when loading stuff)
	glDisable(GL_CULL_FACE);	//Draw both sides of 2D objects (So we can flip images for free)
	glDisable(GL_LIGHTING);
	fillScreen(Color(0,0,0,1));
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(CameraPos.x, CameraPos.y, CameraPos.z);

	m_debugUI->draw();
	
	glColor4f(1,1,1,1);
	
	//-------------------------------------------------------------
	//Set up OpenGL lights
	//TODO: Remove or move to an actual class
	//-------------------------------------------------------------
	float lightPosition[] = {0.0, 0.0, 0.0, 1.0};
	float lightAmbient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
	float lightDiffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
	float lightSpecular[]  = {1.0f, 1.0f, 1.0f, 1.0f};
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
	float materialAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
	float materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f };
	float materialSpecular[] = {0.6f, 0.6f, 0.6f, 1.0f };
	float materialEmission[] = {0.0f, 0.0f, 0.0f, 1.0f };
	float materialShininess = 50.0f;
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialEmission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
		
	//Set up global OpenGL lighting
	float globalAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	//-------------------------------------------------------------
	
	//Keep camera within camera bounds
	if(rcSceneBounds.area())	//If it's not unset
	{
		Rect rcCam = getCameraView(CameraPos);
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
	glTranslatef(CameraPos.x, CameraPos.y, CameraPos.z);
	//glLoadIdentity();
	//gluLookAt(-CameraPos.x, -CameraPos.y + cos(CAMERA_ANGLE_RAD)*CameraPos.z, -sin(CAMERA_ANGLE_RAD)*CameraPos.z, -CameraPos.x, -CameraPos.y, 0.0f, 0, 0, 1);
    //Vec3 eye(-CameraPos.x, -CameraPos.y + cos(CAMERA_ANGLE_RAD)*CameraPos.z, -sin(CAMERA_ANGLE_RAD)*CameraPos.z);
    //Vec3 center(-CameraPos.x, -CameraPos.y, 0.0f);
    //Vec3 up(0.0f, 0.0f, -1.0f); // working as intended
    //glm::mat4 look = glm::lookAt(eye, center, up);
    //glLoadMatrixf(glm::value_ptr(look));

	
	glDisable(GL_LIGHTING);
	glm::mat4 mat;	//TODO Use real mat
	getEntityManager()->render(mat);
	drawDebug();
	
}

void GameEngine::init(list<commandlineArg> sArgs)
{
	//Run through list for arguments we recognize
	for (list<commandlineArg>::iterator i = sArgs.begin(); i != sArgs.end(); i++)
		LOG(DEBUG) << "Commandline argument. Switch: " << i->sSwitch << ", value: " << i->sValue;
		
	//Load our last screen position and such
	loadConfig(getSaveLocation() + "config.xml");
	
	lua_State* L = Lua->getState();
	
	//Have to do this manually because non-constants?
	//TODO: Fix/move from here
	//TODO: Also update these on user key/joystick config
	unsigned int JOY_BUTTON_BACK = SDL_CONTROLLER_BUTTON_BACK;
	unsigned int JOY_BUTTON_START = SDL_CONTROLLER_BUTTON_START;
	unsigned int JOY_BUTTON_X = SDL_CONTROLLER_BUTTON_X;
	unsigned int JOY_BUTTON_Y = SDL_CONTROLLER_BUTTON_Y;
	unsigned int JOY_BUTTON_A = SDL_CONTROLLER_BUTTON_A;
	unsigned int JOY_BUTTON_B = SDL_CONTROLLER_BUTTON_B;
	unsigned int JOY_BUTTON_LB = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	unsigned int JOY_BUTTON_RB = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	unsigned int JOY_BUTTON_LSTICK = SDL_CONTROLLER_BUTTON_LEFTSTICK;
	unsigned int JOY_BUTTON_RSTICK = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
	unsigned int JOY_AXIS_HORIZ = SDL_CONTROLLER_AXIS_LEFTX;
	unsigned int JOY_AXIS_VERT = SDL_CONTROLLER_AXIS_LEFTY;
	unsigned int JOY_AXIS2_HORIZ = SDL_CONTROLLER_AXIS_RIGHTX;
	unsigned int JOY_AXIS2_VERT = SDL_CONTROLLER_AXIS_RIGHTY;
	unsigned int JOY_AXIS_LT = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
	unsigned int JOY_AXIS_RT = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;

	//Set joystick config
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

	//Set key config
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
	
	Lua->call("loadLua");

	string sLocale = SystemUtils::getCurLocale();
	if(sLocale.size())
	{
		LOG(INFO) << "Current system locale: " << sLocale;
		getStringbank()->setLanguage(sLocale.c_str());
	}

	//Start network thread
	if(!NetworkThread::start())
		LOG(ERROR) << "Unable to start networking thread";

	//Open communication to SteelSeries drivers
	if(steelSeriesCommunicator->init(getAppName()))
		LOG(INFO) << "Initialized with SteelSeries drivers";
	else
		LOG(WARNING) << "Unable to communcate with SteelSeries drivers";

}

void GameEngine::pause()
{
	pauseMusic();
}

void GameEngine::resume()
{
	resumeMusic();
}

void GameEngine::warpObjectToNode(Object* o, Node* n)
{
	if(o && n)
		o->setPosition(n->pos);
}





















