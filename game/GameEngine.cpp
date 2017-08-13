/*
	GameEngine source - GameEngine.cpp
	Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include <float.h>
#include <sstream>
#include "opengl-api.h"
#include "easylogging++.h"
#include "DebugUI.h"
#include "ResourceLoader.h"
#include "EntityManager.h"
#include "Stringbank.h"
#include "SystemUtils.h"
#include "SteelSeriesClient.h"
#include "NetworkThread.h"
#include "ParticleSystem.h"
#include "ParticleEditor.h"
#include "InputDevice.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "InterpolationManager.h"

//#define DEBUG_INPUT
#define CONFIG_FILE "config.xml"
#define CONTROLLER_DISCONNECTED_IMAGE "res/util/disconnected.png"

//For our engine functions to be able to call our Engine class functions
GameEngine* g_pGlobalEngine;

GameEngine::GameEngine(uint16_t iWidth, uint16_t iHeight, const std::string& sTitle, const std::string& sCompanyName, const std::string& sAppName, const std::string& sIcon, bool bResizable) : Engine(iWidth, iHeight, sTitle, sCompanyName, sAppName, sIcon, bResizable)
{
	g_pGlobalEngine = this;
	
	//Set camera position for this game
	m_fDefCameraZ = -16;
	cameraPos = Vec3(0,0,m_fDefCameraZ);
#ifdef _DEBUG
	m_bMouseGrabOnWindowRegain = false;
#else
	m_bMouseGrabOnWindowRegain = true;
#endif
	showCursor();
	
	player = NULL;
	
	//Keybinding stuff!
	JOY_AXIS_TRIP = 20000;

	m_debugUI = new DebugUI(this);

	steelSeriesClient = new SteelSeriesClient();
}

GameEngine::~GameEngine()
{
	LOG(INFO) << "~GameEngine()";
	saveConfig(getSaveLocation() + CONFIG_FILE);
	getEntityManager()->cleanup();
	delete m_debugUI;
	delete steelSeriesClient;
	NetworkThread::stop();
}

void GameEngine::frame(float dt)
{
	handleKeys();

	stepPhysics(dt);
#ifdef _DEBUG
	if(m_debugUI->particleEditor->open && m_debugUI->visible)
		m_debugUI->particleEditor->particles->update(dt);
	else
#endif
	{
		getEntityManager()->update(dt);
	}
	steelSeriesClient->update(dt);
	getInterpolationManager()->update(dt);
	
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
	//fillScreen(Color(0,0,0,1));
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(cameraPos.x, cameraPos.y, cameraPos.z);

	m_debugUI->draw();
	
	glColor4f(1,1,1,1);
	
	//-------------------------------------------------------------
	// Set up OpenGL lighting
	//-------------------------------------------------------------
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	float materialShininess = 0.0f;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
		
	//Set up global OpenGL lighting
	float globalAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	//-------------------------------------------------------------
	
	//Keep camera within camera bounds
	if(rcSceneBounds.area())	//If it's not unset
	{
		//Check for within bounds
		Rect rcCam = getCameraView(cameraPos);
		if(rcCam.left < rcSceneBounds.left)
		{
			cameraPos.x -= rcSceneBounds.left - rcCam.left;
			rcCam = getCameraView(cameraPos);
		}
		if(rcCam.right > rcSceneBounds.right)
		{
			cameraPos.x += rcCam.right - rcSceneBounds.right;
			rcCam = getCameraView(cameraPos);
		}
		if(rcCam.top > rcSceneBounds.top)
		{
			cameraPos.y -= rcSceneBounds.top - rcCam.top;
			rcCam = getCameraView(cameraPos);
		}
		if(rcCam.bottom < rcSceneBounds.bottom)
		{
			cameraPos.y += rcCam.bottom - rcSceneBounds.bottom;
			rcCam = getCameraView(cameraPos);
		}
		//Secondary check to see if we're over both, in which case center
		if(rcCam.left < rcSceneBounds.left)
			cameraPos.x -= (rcSceneBounds.left - rcCam.left) / 2.0f;
		if(rcCam.top > rcSceneBounds.top)
			cameraPos.y -= (rcSceneBounds.top - rcCam.top) / 2.0f;
	}
	glLoadIdentity();
	glTranslatef(cameraPos.x, cameraPos.y, cameraPos.z);
	//Tilted view stuff
	//glLoadIdentity();
	//gluLookAt(-cameraPos.x, -cameraPos.y + cos(CAMERA_ANGLE_RAD)*cameraPos.z, -sin(CAMERA_ANGLE_RAD)*cameraPos.z, -cameraPos.x, -cameraPos.y, 0.0f, 0, 0, 1);
    //Vec3 eye(-cameraPos.x, -cameraPos.y + cos(CAMERA_ANGLE_RAD)*cameraPos.z, -sin(CAMERA_ANGLE_RAD)*cameraPos.z);
    //Vec3 center(-cameraPos.x, -cameraPos.y, 0.0f);
    //Vec3 up(0.0f, 0.0f, -1.0f); // working as intended
    //glm::mat4 look = glm::lookAt(eye, center, up);
    //glLoadMatrixf(glm::value_ptr(look));

	
	glDisable(GL_LIGHTING);
	glm::mat4 mat;	//TODO Use real mat
	getEntityManager()->render(mat);

#ifdef _DEBUG

	drawDebug();
	if(m_debugUI->particleEditor->open && m_debugUI->visible)
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, m_fDefCameraZ);
		glClear(GL_DEPTH_BUFFER_BIT);
		fillScreen(m_debugUI->particleEditor->particleBgColor);
		glClear(GL_DEPTH_BUFFER_BIT);
		m_debugUI->particleEditor->particles->draw();
	}
#endif

	if(isControllerDisconnected())
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, m_fDefCameraZ);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		Image* disconnectedImage = getResourceLoader()->getImage(CONTROLLER_DISCONNECTED_IMAGE);
		//TODO
		//if(disconnectedImage)
		//	disconnectedImage->render4V(Vec2(-4.01, -1), Vec2(4.01, -1), Vec2(-4.01, 1), Vec2(4.01, 1));
	}
	
}

void GameEngine::init(std::list<commandlineArg> sArgs)
{
	//Run through list for arguments we recognize
	for (std::list<commandlineArg>::iterator i = sArgs.begin(); i != sArgs.end(); i++)
		LOG(DEBUG) << "Commandline argument. Switch: " << i->sSwitch << ", value: " << i->sValue;
		
	//Load our last screen position and such
	loadConfig(getSaveLocation() + CONFIG_FILE);
	
	lua_State* L = Lua->getState();
	
	Lua->call("loadLua");

	std::string sLocale = SystemUtils::getCurLocale();
	if(sLocale.size())
	{
		LOG(INFO) << "Current system locale: " << sLocale;
		getStringbank()->setLanguage(sLocale.c_str());
	}

	//Start network thread, if SS engine is here
	if(steelSeriesClient->isValid())
	{
		//NOTE: If we have other networking stuff later, this shouldn't depend on SS engine
		if(!NetworkThread::start())
			LOG(ERROR) << "Unable to start networking thread";

		//Open communication to SteelSeries drivers
		if(steelSeriesClient->init(getAppName()))
			LOG(INFO) << "Initialized with SteelSeries drivers";
		else
			LOG(WARNING) << "Unable to communicate with SteelSeries drivers";
	}

	//Add kb+mouse controller
	getInputManager()->addController(new InputDevice(steelSeriesClient));
}

void GameEngine::pause()
{
	getSoundManager()->pauseAll();
}

void GameEngine::resume()
{
	getSoundManager()->resumeAll();
}

void GameEngine::warpObjectToNode(Object* o, Node* n)
{
	if(o && n)
		o->setPosition(n->pos);
}





















