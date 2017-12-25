/*
	GameEngine source - GameEngine.cpp
	Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include <float.h>
#include <sstream>
#include "opengl-api.h"
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
#include "Object.h"
#include "Node.h"
#include "easylogging++.h"

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

const double CAMERA_ANGLE_RAD = glm::radians(60.0);
void GameEngine::draw(RenderState& renderState)
{
	//Clear bg (not done with OpenGL funcs, cause of weird black frame glitch when loading stuff)
	glDisable(GL_CULL_FACE);	//Draw both sides of 2D objects (So we can flip images for free)
	glClear(GL_DEPTH_BUFFER_BIT);

	//Draw debug UI
	m_debugUI->draw();
	
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

	//Set flat camera
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z));
	
	//Set tilted view camera
    //Vec3 eye(-cameraPos.x, -cameraPos.y + cos(CAMERA_ANGLE_RAD)*cameraPos.z, -sin(CAMERA_ANGLE_RAD)*cameraPos.z);
    //Vec3 center(-cameraPos.x, -cameraPos.y, 0.0f);
    //Vec3 up(0.0f, 0.0f, 1.0f);
    //glm::mat4 view = glm::lookAt(eye, center, up);
	renderState.view = view;
	
	glm::mat4 model = glm::mat4(1.0f);	//Identity matrix
	renderState.model = model;
	getEntityManager()->render(renderState);

#ifdef _DEBUG
	renderState.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, m_fDefCameraZ));
	if(m_debugUI->particleEditor->open && m_debugUI->visible)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		Color col = m_debugUI->particleEditor->particleBgColor;
		glClearColor(col.r, col.g, col.b, col.a);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		m_debugUI->particleEditor->particles->draw(renderState);
	}
	renderState.view = view;
#endif

	//TODO
	//if(isControllerDisconnected())
	//{
		//glLoadIdentity();
		//glTranslatef(0.0f, 0.0f, m_fDefCameraZ);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_BLEND);
		//Image* disconnectedImage = getResourceLoader()->getImage(CONTROLLER_DISCONNECTED_IMAGE);
		//if(disconnectedImage)
		//	disconnectedImage->render4V(Vec2(-4.01, -1), Vec2(4.01, -1), Vec2(-4.01, 1), Vec2(4.01, 1));
	//}
	
}

void GameEngine::init(std::vector<commandlineArg> sArgs)
{
	//Run through list for arguments we recognize
	for (std::vector<commandlineArg>::iterator i = sArgs.begin(); i != sArgs.end(); i++)
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

bool GameEngine::drawDebugUI()
{
	return m_debugUI->visible;
}



















