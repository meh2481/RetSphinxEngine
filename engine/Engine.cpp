/*
	GameEngine source - Engine.cpp
	Copyright (c) 2014 Mark Hutcheson
*/
#include <SDL_syswm.h>
#include "Engine.h"
#include "Box2D/Box2D.h"
#include "Image.h"
#include "opengl-api.h"
#include "easylogging++.h"
#include "Random.h"
#include "imgui/imgui.h"
#include "imgui_impl_sdl.h"
#include "ResourceLoader.h"
#include "EntityManager.h"
#include "Stringbank.h"
#include "stb_image.h"
#include "InputDevice.h"
using namespace std;

#define GUID_STR_SZ	256

Engine::Engine(uint16_t iWidth, uint16_t iHeight, string sTitle, string sCompanyName, string sAppName, string sIcon, bool bResizable)
{
	m_sTitle = sTitle;
	m_sAppName = sAppName;
	m_sCompanyName = sCompanyName;

	//Start logger
	el::Configurations conf("logging.conf");
	if(!conf.hasConfiguration(el::ConfigurationType::Filename))
		conf.setGlobally(el::ConfigurationType::Filename, (getSaveLocation() + "logfile.log").c_str());
	el::Loggers::reconfigureAllLoggers(conf);

	m_sIcon = sIcon;
	m_bResizable = bResizable;
	b2Vec2 gravity(0.0f, -9.8f);	//Vector for our world's gravity
	m_physicsWorld = new b2World(gravity);
	m_physicsWorld->SetAllowSleeping(true);
	m_physicsWorld->SetDebugDraw(&m_debugDraw);
	m_physicsWorld->SetContactListener(&m_clContactListener);
	m_debugDraw.SetFlags(DebugDraw::e_shapeBit | DebugDraw::e_jointBit);
	m_bObjDebugDraw = false;
	LOG(INFO) << "-----------------------BEGIN PROGRAM EXECUTION-----------------------";
#ifdef _DEBUG
	LOG(INFO) << "Debug build";
	m_bDebugDraw = true;
#else
	LOG(INFO) << "Release build";
	m_bDebugDraw = false;
#endif
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iMSAA = 0;	//Default: MSAA off
	m_iKeystates = NULL;
	m_bShowCursor = true;
	m_fFramerate = 60.0f;
	setFramerate(60);	 //60 fps default
	m_bFullscreen = true;
	m_curActiveController = -1;

	setup_sdl();
	setup_opengl();
	m_fGamma = 1.0f;
	m_bPaused = m_bControllerDisconnected = false;
	m_bPauseOnKeyboardFocus = true;
	m_bCursorShow = true;
	m_bCursorOutOfWindow = false;
#ifdef _DEBUG
	m_bSteppingPhysics = false;
	m_bStepFrame = false;
#endif

	//Initialize engine stuff
	m_fAccumulatedTime = 0.0;
	//m_bFirstMusic = true;
	m_bQuitting = false;
	Random::seed(SDL_GetTicks());
	m_fTimeScale = 1.0f;

	LOG(INFO) << "Creating resource loader";
	m_resourceLoader = new ResourceLoader(m_physicsWorld, "res/pak");
	m_entityManager = new EntityManager(m_resourceLoader, m_physicsWorld);
	m_stringBank = m_resourceLoader->getStringbank("res/stringbank.xml"); //TODO: load from elsewhere?

	_loadicon();	//Load our window icon

	LOG(INFO) << "Initializing FMOD...";
	m_bSoundDied = true;
	//TODO: Fix FMOD or find replacement
	/*if(FMOD_System_Create(&m_audioSystem) != FMOD_OK || FMOD_System_Init(m_audioSystem, 128, FMOD_INIT_NORMAL, 0) != FMOD_OK)
	{
		LOG(ERROR) << "Failed to init FMOD."
		m_bSoundDied = true;
	}
	else
	{
		m_bSoundDied = false;
		//Figure out what sound drivers for input we have here
		int numDrivers = 0;
		FMOD_System_GetRecordNumDrivers(m_audioSystem, &numDrivers);
		const int DRIVER_INFO_STR_SIZE = 512;
		char driverInfoStr[DRIVER_INFO_STR_SIZE];
		LOG(TRACE) << numDrivers << " recording drivers available."
		for(int i = 0; i < numDrivers; i++)
		{
			FMOD_System_GetRecordDriverInfo(m_audioSystem, i, driverInfoStr, DRIVER_INFO_STR_SIZE, NULL);
			LOG(TRACE) << "Driver " << i << ": " << driverInfoStr
		}
	}*/

	//This needs to be in memory when ImGUI goes to load/save INI settings, so it's static
	static const string sIniFile = getSaveLocation() + "imgui.ini";
	//Init ImGUI
	ImGui_ImplSdl_Init(m_Window, sIniFile.c_str());
	ImGui_Impl_GL2_CreateDeviceObjects();
}

Engine::~Engine()
{
	delete m_entityManager;
	delete m_resourceLoader;

	for(vector<InputDevice*>::iterator i = m_controllers.begin(); i != m_controllers.end(); i++)
		delete *i;

	ImGui_Impl_GL2_Shutdown();

	SDL_DestroyWindow(m_Window);

	//Clean up our sound effects
	/*if(!m_bSoundDied)
	{
		for(map<string, FMOD_SOUND*>::iterator i = m_sounds.begin(); i != m_sounds.end(); i++)
			FMOD_Sound_Release(i->second);
	}

	//Clean up FMOD
	if(!m_bSoundDied)
	{
		FMOD_System_Close(m_audioSystem);
		FMOD_System_Release(m_audioSystem);
	}*/

	// Clean up and shutdown
	LOG(INFO) << "Deleting phys world";
	delete m_physicsWorld;
	LOG(INFO) << "Quit SDL";
	SDL_Quit();
}

void Engine::start()
{
	// Load all that we need to
	init(lCommandLine);
	// Let's rock now!
	while(!_frame());
}

bool Engine::_processEvent(SDL_Event& e)
{
	//Engine-specific event processing
	switch(e.type)
	{
		case SDL_MOUSEMOTION:
			m_ptCursorPos.x = (float)e.motion.x;
			m_ptCursorPos.y = (float)e.motion.y;
			break;

		case SDL_WINDOWEVENT:
			if(e.window.event == SDL_WINDOWEVENT_FOCUS_LOST && m_bPauseOnKeyboardFocus)
			{
				m_bPaused = true;
				pause();
			}
			else if(e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED && m_bPauseOnKeyboardFocus)
			{
				m_bPaused = false;
				if(!m_bControllerDisconnected)
					resume();
			}
			else if(e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				if(m_bResizable)
					changeScreenResolution(e.window.data1, e.window.data2);
				else
					LOG(ERROR) << "Resize event generated, but resizable flag not set.";
			}
			else if(e.window.event == SDL_WINDOWEVENT_ENTER)
				m_bCursorOutOfWindow = false;
			else if(e.window.event == SDL_WINDOWEVENT_LEAVE)
				m_bCursorOutOfWindow = true;
			break;

		case SDL_QUIT:
			return true;

		case SDL_CONTROLLERDEVICEADDED:
			addController(e.cdevice.which);
			break;

		case SDL_CONTROLLERDEVICEREMOVED:
			LOG(INFO) << "Controller " << (int)e.cdevice.which << " disconnected.";
			removeController(e.cdevice.which);
			break;

		case SDL_JOYDEVICEADDED:
		{
			SDL_Joystick* joy = SDL_JoystickOpen(e.jdevice.which);
			if(joy)
			{
				char guid[GUID_STR_SZ];
				SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, GUID_STR_SZ);

				LOG(INFO) << "Joystick " << SDL_JoystickName(joy) << " attached. Not using joystick API, but here's info:";
				LOG(INFO) << "Joystick has GUID " << guid;
				LOG(INFO) << "Joystick Number of Axes: " << SDL_JoystickNumAxes(joy);
				LOG(INFO) << "Joystick Number of Buttons: " << SDL_JoystickNumButtons(joy);
				LOG(INFO) << "Joystick Number of Balls: " << SDL_JoystickNumBalls(joy);
				LOG(INFO) << "Joystick Number of Hats: " << SDL_JoystickNumHats(joy);
				SDL_JoystickClose(joy);
			}
			break;
		}

		case SDL_JOYDEVICEREMOVED:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYHATMOTION:
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_KEYDOWN:
			m_curActiveController = 0;	//Mouse+kb control
			//Fall through
		case SDL_CONTROLLERBUTTONDOWN:
			//Unpause when selecting with a new input device
			if(m_bControllerDisconnected)
			{
				resume();	//Unpause game
				m_bControllerDisconnected = false;
			}
			break;
	}

	return false;
}

bool Engine::_frame()
{
	updateSound();

	//Handle input events from SDL
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		ImGui_ImplSdl_ProcessEvent(&event);

		if(_processEvent(event))
			return true;

		//Pass on to derived game engine class
		if(!m_bPaused && !m_bControllerDisconnected)
			handleEvent(event);
	}

	ImGui_ImplSdl_NewFrame(m_Window);

	if(m_bPaused || m_bControllerDisconnected)
	{
		SDL_Delay(100);	//Wait 100 ms
		_render();
		return m_bQuitting;	//Break out here
	}

	float fCurTime = ((float)SDL_GetTicks()) / 1000.0f;
	if(m_fAccumulatedTime <= fCurTime)
	{
		m_fAccumulatedTime += m_fTargetTime;
		m_iKeystates = SDL_GetKeyboardState(NULL);	//Get current key state
#ifdef _DEBUG
		if(!m_bSteppingPhysics || m_bStepFrame)
		{
			m_bStepFrame = false;
#endif

			frame(m_fTargetTime);	//Box2D wants fixed timestep, so we use target framerate here instead of actual elapsed time
#ifdef _DEBUG
		}
#endif
		_render();
	}

	if(m_fAccumulatedTime + m_fTargetTime * 3.0 < fCurTime)	//We've gotten far too behind; we could have a huge FPS jump if the load lessens
		m_fAccumulatedTime = fCurTime;	 //Drop any frames past this
	return m_bQuitting;
}

void Engine::_render()
{
	// Begin rendering by clearing the screen
	glClear(GL_DEPTH_BUFFER_BIT);

	// Game-specific drawing
	draw();

	if(m_bControllerDisconnected)
	{
		//TODO Should really have the camera controlled by the engine, so we don't have to depend on state from draw() here...
		//glLoadIdentity();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		Image* disconnectedImage = getResourceLoader()->getImage("res/util/disconnected.png");
		if(disconnectedImage)
			disconnectedImage->render4V(Vec2(-4.01,-1), Vec2(4.01, -1), Vec2(-4.01, 1), Vec2(4.01, 1));
	}

	//Draw gamma/brightness overlay on top of everything else
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	Color fillCol;
	if(m_fGamma > 1.0f)
	{
		glBlendFunc(GL_DST_COLOR, GL_ONE);
		fillCol.set(m_fGamma - 1.0f, m_fGamma - 1.0f, m_fGamma - 1.0f, 1.0f);
	}
	else
	{
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		fillCol.set(m_fGamma, m_fGamma, m_fGamma, 1.0f);
	}
	fillScreen(fillCol);

	//Reset blend func
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ImGui::Render();

	glPopMatrix();

	//End rendering and update the screen
	SDL_GL_SwapWindow(m_Window);
}

void Engine::drawDebug()
{
	// Draw physics debug stuff
	if(m_bDebugDraw)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_physicsWorld->DrawDebugData();
		glColor4f(1, 1, 1, 1);
	}
}

void Engine::fillScreen(Color col)
{
	//Fill whole screen with rect (Example taken from http://yuhasapoint.blogspot.com/2012/07/draw-quad-that-fills-entire-opengl.html on 11/20/13)
	glColor4f(col.r, col.g, col.b, col.a);
	glBindTexture(GL_TEXTURE_2D, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	glVertex3i(-1, -1, -1);
	glVertex3i(1, -1, -1);
	glVertex3i(1, 1, -1);
	glVertex3i(-1, 1, -1);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0);
}

bool Engine::keyDown(int32_t keyCode)
{
	if(m_iKeystates == NULL) return false;	//On first cycle, this can be NULL and cause segfaults otherwise

	//HACK: See if one of our combined keycodes
	if(keyCode == SDL_SCANCODE_CTRL) return (keyDown(SDL_SCANCODE_LCTRL) || keyDown(SDL_SCANCODE_RCTRL));
	if(keyCode == SDL_SCANCODE_SHIFT) return (keyDown(SDL_SCANCODE_LSHIFT) || keyDown(SDL_SCANCODE_RSHIFT));
	if(keyCode == SDL_SCANCODE_ALT) return (keyDown(SDL_SCANCODE_LALT) || keyDown(SDL_SCANCODE_RALT));
	if(keyCode == SDL_SCANCODE_GUI) return (keyDown(SDL_SCANCODE_LGUI) || keyDown(SDL_SCANCODE_RGUI));

	//Otherwise, just use our pre-polled list we got from SDL
	return(m_iKeystates[keyCode]);
}

//TODO Controller/input logic should be its own class
InputDevice* Engine::getCurController()
{
	if(m_curActiveController < 0 || m_curActiveController > m_controllers.size() - 1)
		return NULL;
	return m_controllers[m_curActiveController];
}

void Engine::addController(int deviceIndex)
{
	addController(new InputDevice(deviceIndex));
}

void Engine::addController(InputDevice* device)
{
	m_controllers.push_back(device);
	m_curActiveController = m_controllers.size() - 1;	//Set this as new active controller
}

void Engine::removeController(int deviceIndex)
{
	for(int i = 0; i < m_controllers.size(); i++)
	{
		if(deviceIndex == m_controllers[i]->getDeviceIndex())
		{
			//Remove this controller from the list
			delete m_controllers[i];
			m_controllers.erase(m_controllers.begin() + i);
			if(i == m_curActiveController)
			{
				m_bControllerDisconnected = true;	//Disconnected current controller; pause game
				pause();	//Pause game
			}
			if(i <= m_curActiveController)
			{
				m_curActiveController--;
				if(m_curActiveController < 0)	//Was first in the list
					m_curActiveController = m_controllers.size() - 1;
			}
			break;
		}
	}
}

void Engine::activateController(int deviceIndex)
{
	for(int i = 0; i < m_controllers.size(); i++)
	{
		if(deviceIndex == m_controllers[i]->getDeviceIndex())
			m_curActiveController = i;
	}
}

void Engine::setFramerate(float fFramerate)
{
	if(fFramerate < 30.0)
		fFramerate = 30.0;	//30fps is bare minimum
	if(m_fFramerate == 0.0)
		m_fAccumulatedTime = (float)SDL_GetTicks() / 1000.0f;	 //If we're stuck at 0fps for a while, this number could be huge, which would cause unlimited fps for a bit
	m_fFramerate = fFramerate;
	m_fTargetTime = 1.0f / m_fFramerate;
}

void Engine::setMSAA(int iMSAA)
{
	m_iMSAA = iMSAA;
	if(iMSAA)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, iMSAA);
		glEnable(GL_MULTISAMPLE);

		// Enable OpenGL antialiasing stuff
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		glDisable(GL_MULTISAMPLE);

		// Disable OpenGL antialiasing stuff
		glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
	}
}

void Engine::_loadicon()	//Load icon into SDL window
{
	LOG(INFO) << "Load icon " << m_sIcon;

	SDL_Surface *surface = getResourceLoader()->getSDLImage(m_sIcon);
	if(surface == NULL)
		LOG(ERROR) << "NULL surface for icon " << m_sIcon;
	else
	{
		SDL_SetWindowIcon(m_Window, surface);
		SDL_FreeSurface(surface);
	}
}

bool Engine::getCursorDown(int iButtonCode)
{
	return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(iButtonCode));
}

void Engine::commandline(list<string> argv)
{
	//Step through intelligently
	for(list<string>::iterator i = argv.begin(); i != argv.end(); i++)
	{
		commandlineArg cla;
		string sSwitch = *i;
		if(sSwitch.find('-') == 0)
		{
			if(sSwitch.find("--") == 0)
				sSwitch.erase(0, 1);
			sSwitch.erase(0, 1);

			cla.sSwitch = sSwitch;
			list<string>::iterator sw = i;
			if(++sw != argv.end())	//Switch with a value
			{
				cla.sValue = *sw;
				i++;
				if(i == argv.end()) break;
			}
		}
		else	//No switch for this value
			cla.sValue = sSwitch;
		lCommandLine.push_back(cla);
	}
}

string Engine::getSaveLocation()
{
	char* cPath = SDL_GetPrefPath(m_sCompanyName.c_str(), m_sAppName.c_str());
	string s;
	if(cPath)
		s = cPath;
	else
		s = "./";
	SDL_free(cPath);
	return s;
}

Rect Engine::getCameraView(Vec3 Camera)
{
	Rect rcCamera;
	const float tan45_2 = 0.4142135623f;// tan(glm::radians(45.0f / 2.0f));
	const float fAspect = (float)getWidth() / (float)getHeight();
	rcCamera.bottom = (tan45_2 * Camera.z);
	rcCamera.top = -(tan45_2 * Camera.z);
	rcCamera.left = rcCamera.bottom * fAspect;
	rcCamera.right = rcCamera.top * fAspect;
	rcCamera.offset(-Camera.x, -Camera.y);
	return rcCamera;
}

Vec2 Engine::worldMovement(Vec2 cursormove, Vec3 Camera)
{
	cursormove.x /= (float)getWidth();
	cursormove.y /= (float)getHeight();

	Rect rcCamera = getCameraView(Camera);
	cursormove.x *= rcCamera.width();
	cursormove.y *= rcCamera.height();

	return cursormove;
}

Vec2 Engine::worldPosFromCursor(Vec2 cursorpos, Vec3 Camera)
{
	//Rectangle that the camera can see in world space
	Rect rcCamera = getCameraView(Camera);

	//Our relative position in window rect space (in rage 0-1)
	cursorpos.x /= (float)getWidth();
	cursorpos.y /= -(float)getHeight();	//Flip on y

	//Multiply this by the size of the world rect to get the relative cursor pos
	cursorpos.x = cursorpos.x * rcCamera.width() + rcCamera.left;
	cursorpos.y = cursorpos.y * rcCamera.height() + rcCamera.top;

	return cursorpos;
}

void Engine::stepPhysics(float dt)
{
	m_physicsWorld->Step(dt * m_fTimeScale, VELOCITY_ITERATIONS, PHYSICS_ITERATIONS);

	//Update collisions
	set<b2Contact*> contacts = m_clContactListener.currentContacts;	//Grab all the currently-active contacts
	//Set join the short contacts that fired and also quit this frame
	for(set<b2Contact*>::iterator i = m_clContactListener.frameContacts.begin(); i != m_clContactListener.frameContacts.end(); i++)
		contacts.insert(*i);

	//Iterate over all these
	for(set<b2Contact*>::iterator i = contacts.begin(); i != contacts.end(); i++)
	{
		Collision c = m_clContactListener.getCollision(*i);
		b2WorldManifold worldManifold;
		(*i)->GetWorldManifold(&worldManifold);
		if(c.objA && c.objB)
		{
			//Let both scripts handle colliding, for more generic collision in both
			c.objA->collide(c.objB, c.impulse);
			c.objB->collide(c.objA, c.impulse);
		}
		else if(c.objA && !c.nodeB)
		{
			b2Vec2 pt = -worldManifold.normal;	//Flip this, since a Box2D normal is defined from A->B, and we want a wall->obj normal
			c.objA->collideWall(Vec2(pt.x, pt.y), c.impulse);
		}
		else if(c.objB && !c.nodeA)
		{
			c.objB->collideWall(Vec2(worldManifold.normal.x, worldManifold.normal.y), c.impulse);
		}
		//Don't care about two non-object fixtures colliding

		//Test for objects entering nodes (sensors)
		if(c.nodeA && c.objB)
		{
			c.nodeA->collided(c.objB);
		}
		else if(c.nodeB && c.objA)
		{
			c.nodeB->collided(c.objA);
		}
	}
	m_clContactListener.clearFrameContacts();	//Erase all these now that we've handled them
}

void Engine::setDoubleBuffered(bool bDoubleBuffered)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, bDoubleBuffered);
}

bool Engine::getDoubleBuffered()
{
	int val = 1;
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &val);
	return val;
}

void Engine::setVsync(int iVsync)
{
	SDL_GL_SetSwapInterval(iVsync);
}

int Engine::getVsync()
{
	return SDL_GL_GetSwapInterval();
}

b2Body* Engine::createBody(b2BodyDef* bdef)
{
	return m_physicsWorld->CreateBody(bdef);
}

void Engine::setGravity(Vec2 ptGravity)
{
	m_physicsWorld->SetGravity(b2Vec2(ptGravity.x, ptGravity.y));
}

void Engine::setGravity(float x, float y)
{
	setGravity(Vec2(x, y));
}

bool Engine::isMouseGrabbed()
{
	return SDL_GetWindowGrab(m_Window);
}

void Engine::grabMouse(bool bGrab)
{
	SDL_SetWindowGrab(m_Window, (SDL_bool)bGrab);
}

unsigned Engine::getTicks()
{
	return SDL_GetTicks();
}
float Engine::getSeconds()
{
	return (float)SDL_GetTicks() / 1000.0f;
}
