/*
	GameEngine source - GameEngine.cpp
	Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include "tinyxml2.h"
#include <float.h>
#include <sstream>
#include <iomanip>
#include <ctime>

//For our engine functions to be able to call our Engine class functions
GameEngine* g_pGlobalEngine;
float32 g_fParticleFac;

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
	vfs.Prepare();
	
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
	ship = NULL;
	
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
#ifdef _WIN32
	JOY_BUTTON_BACK = 5;
	JOY_BUTTON_START = 4;	//TODO
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
#else
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
#endif
	
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

const float32 MUSIC_SCRUBIN_SPEED = soundFreqDefault * 2.0f;

//float fSunRotAmt = 0;
float fPlanetRotAmt = 0;

void GameEngine::frame(float32 dt)
{
	handleKeys();
	stepPhysics(dt);
	updateParticles(dt);
	updateObjects(dt);
	updateShip();
	//m_lAnimTest->update(dt);
	
	
	//m_sun->pos.x = cos(DEG2RAD*fSunRotAmt) * 50;
	//m_sun->depth = -sin(DEG2RAD*fSunRotAmt) * 50;
	//updateSceneryLayer(m_sun);
	
}

#define CAMERA_ANGLE_RAD 1.152572

void GameEngine::draw()
{
	//Clear bg (not done with OpenGL funcs, cause of weird black frame glitch when loading stuff)
	glDisable(GL_CULL_FACE);	//Draw both sides of 2D objects (So we can flip images for free)
	glDisable(GL_LIGHTING);
	fillScreen(Color(0,0,0,1));
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(CameraPos.x, CameraPos.y, CameraPos.z);
	
	drawParticles();	//Draw engine particles here
	
	//Set mouse cursor to proper location
	for(map<string, myCursor*>::iterator i = m_mCursors.begin(); i != m_mCursors.end(); i++)
	{
		i->second->pos = worldPosFromCursor(getCursorPos());
		if(i->first == "dir")
			i->second->rot = -RAD2DEG * atan2(i->second->pos.x, i->second->pos.y) + 90;
	}
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
	
	//Draw objects
	glLoadIdentity();
	//glTranslatef(CameraPos.x, CameraPos.y, CameraPos.z);
	if(ship != NULL)
	{
		b2Body* b = ship->getBody();
		if(b != NULL)
		{
			Point p = b->GetPosition();
			CameraPos.x = -p.x;
			CameraPos.y = -p.y;
			gluLookAt(p.x, p.y + cos(CAMERA_ANGLE_RAD)*CameraPos.z, -sin(CAMERA_ANGLE_RAD)*CameraPos.z, p.x, p.y, 0.0f, 0, 0, 1);
			//gluLookAt(p.x, p.y-8, -CameraPos.z+2, p.x, p.y, 0.0f, 0, 0, 1);
		}
	}
	
	glDisable(GL_LIGHTING);
	drawAll();
	drawDebug();
	
	glLoadIdentity();
	glTranslatef(-CameraPos.x, -CameraPos.y, m_fDefCameraZ);		//translate back to put cursor in the right position
	m_Cursor->pos = worldPosFromCursor(getCursorPos());
	drawCursor();
	
}

void GameEngine::init(list<commandlineArg> sArgs)
{
	//Run through list for arguments we recognize
	for(list<commandlineArg>::iterator i = sArgs.begin(); i != sArgs.end(); i++)
		errlog << "Commandline argument. Switch: " << i->sSwitch << ", value: " << i->sValue << endl;
		
	//Load our last screen position and such
	loadConfig(getSaveLocation() + "config.xml");
	
	//Set gravity to about normal
	//getWorld()->SetGravity(b2Vec2(0,-9.8));
	getWorld()->SetGravity(b2Vec2(0,0));
	
	//m_lTest = new lattice(20,20);
	//m_lAnimTest = new sinLatticeAnim(m_lTest);
	//m_lAnimTest->amp = 0.05;
	/*m_lAnimTest->distvar = 0.0075;
	m_lAnimTest->speed = 1.3;
	m_lAnimTest->hfac = 1.3;
	m_lAnimTest->vfac = 0.3;
	//m_lAnimTest->anglevar = PI;*/
	//m_lAnimTest->init();
	//Image* im = new Image();
	
	//testObj = getObject("res/3d/dome.tiny3d");//, getImage("res/3d/uvtest.png"));
	
	//m_sun = new physSegment();
	//m_sun->img = getImage("res/3d/sun.png");
	//m_sun->size.x = m_sun->size.y = 20;
	//addScenery(m_sun);
	loadScene("res/3d/solarsystem.scene.xml");
}


void GameEngine::hudSignalHandler(string sSignal)
{
	
}

void GameEngine::handleEvent(SDL_Event event)
{
	switch(event.type)
	{
		//Key pressed
		case SDL_KEYDOWN:
			switch(event.key.keysym.scancode)
			{
				case SDL_SCANCODE_F5:
					//cleanupObjects();
					//addObject(objFromXML("res/obj/ground.xml"));
					//addObject(objFromXML("res/obj/test.xml", Point(0, 5.5)));
					reloadImages();
					loadScene("res/3d/solarsystem.scene.xml");
					break;
					
				case SDL_SCANCODE_V:
					toggleDebugDraw();
					break;
					
				case SDL_SCANCODE_ESCAPE:
					quit();
					break;
					
				case SDL_SCANCODE_PRINTSCREEN:
				{
					//Save screenshot of current OpenGL window (example from https://stackoverflow.com/questions/5844858/how-to-take-screenshot-in-opengl)
					time_t t = time(0);   // get time now
					struct tm * now = localtime(&t);
				
					//Create filename that we'll save this as
					ostringstream ssfile;
					ssfile << getSaveLocation() << "/screenshots/" << "Screenshot " 
					       << now->tm_mon + 1 << '-' << now->tm_mday << '-' << now->tm_year + 1900 << ' '
					       << now->tm_hour << '.' << setw(2) << setfill('0') << now->tm_min << '.' << setw(2) << setfill('0') << now->tm_sec << ".png";
				
					uint16_t width = getWidth();
					uint16_t height = getHeight();
					
					BYTE* pixels = new BYTE[3 * width * height];
					glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

					//Convert to FreeImage format & save to file
					FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
					FreeImage_Save(FIF_PNG, image, ssfile.str().c_str(), PNG_Z_BEST_SPEED);

					//Free resources
					FreeImage_Unload(image);
					delete [] pixels;
					break;
				}
			}
			break;
		
		//Key released
		case SDL_KEYUP:
			switch(event.key.keysym.scancode)
			{
			}
			break;
		
		case SDL_MOUSEBUTTONDOWN:
		{
			//addObject(objFromXML("res/obj/test.xml", worldPosFromCursor(getCursorPos())));
		}
		break;
			
		case SDL_MOUSEWHEEL:
			if(event.wheel.y > 0)
			{
				CameraPos.z += 1.5;// min(CameraPos.z + 1.5, -5.0);
			}
			else
			{
				CameraPos.z -= 1.5;// max(CameraPos.z - 1.5, -3000.0);
			}
			//cameraBounds();
			break;

		case SDL_MOUSEBUTTONUP:
#ifdef DEBUG_INPUT
			cout << "Mouse button " << (int)event.button.button << " released." << endl;
#endif
			if(event.button.button == SDL_BUTTON_LEFT)
			{
				
			}
			else if(event.button.button == SDL_BUTTON_RIGHT)
			{
			
			}
			else if(event.button.button == SDL_BUTTON_MIDDLE)
			{
				
			}
			break;

		case SDL_MOUSEMOTION:
			break;
		
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
				case SDL_WINDOWEVENT_FOCUS_LOST:
					m_bMouseGrabOnWindowRegain = isMouseGrabbed();
					grabMouse(false);	//Ungrab mouse cursor if alt-tabbing out or such
					break;
				
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					grabMouse(m_bMouseGrabOnWindowRegain);	//Grab mouse on input regain, if we should
					break;
			}
			break;
			
		//Gamepad stuff!
		case SDL_JOYDEVICEADDED:
			errlog << "Joystick " << (int)event.jdevice.which << " connected." << endl;
			m_joy = SDL_JoystickOpen(event.jdevice.which);

			if(m_joy) 
			{
				errlog << "Opened Joystick " << event.jdevice.which << endl;
				errlog << "Name: " << SDL_JoystickNameForIndex(event.jdevice.which) << endl;
				errlog << "Number of Axes: " << SDL_JoystickNumAxes(m_joy) << endl;
				errlog << "Number of Buttons: " << SDL_JoystickNumButtons(m_joy) << endl;
				errlog << "Number of Balls: " << SDL_JoystickNumBalls(m_joy) << endl;
				errlog << "Number of Hats: " << SDL_JoystickNumHats(m_joy) << endl;
				
				//On Linux, "xboxdrv" is the driver I had the most success with when it came to rumble (default driver said it rumbled, but didn't)
				m_rumble = NULL;
				if(SDL_JoystickIsHaptic(m_joy))
					m_rumble = SDL_HapticOpenFromJoystick(m_joy);
				if(m_rumble)
				{
					if(SDL_HapticRumbleInit(m_rumble) != 0)
					{
						errlog << "Error initializing joystick " << (int)event.jdevice.which << " as rumble." << endl;
						SDL_HapticClose(m_rumble);
						m_rumble = NULL;
					}
					else 
					{
						errlog << "Initialized joystick " << (int)event.jdevice.which << " as rumble." << endl;
					}
				}
				else
					errlog << "Joystick " << (int)event.jdevice.which << " has no rumble support." << endl;
			} 
			else
				errlog << "Couldn't open Joystick " << (int)event.jdevice.which << endl;
			break;
			
		case SDL_JOYDEVICEREMOVED:
			errlog << "Joystick " << (int)event.jdevice.which << " disconnected." << endl;
			break;
			
		case SDL_JOYBUTTONDOWN:
#ifdef DEBUG_INPUT
			cout << "Joystick " << (int)event.jbutton.which << " pressed button " << (int)event.jbutton.button << endl;
#endif
			break;
			
		case SDL_JOYBUTTONUP:
#ifdef DEBUG_INPUT
			cout << "Joystick " << (int)event.jbutton.which << " released button " << (int)event.jbutton.button << endl;
#endif
			break;
			
		case SDL_JOYAXISMOTION:
			if(abs(event.jaxis.value) > JOY_MINMOVE_TRIP)
			{
#ifdef DEBUG_INPUT
				cout << "Joystick " << (int)event.jaxis.which << " moved axis " << (int)event.jaxis.axis << " to " << event.jaxis.value << endl;
#endif
			}
			break;
			
		case SDL_JOYHATMOTION:
#ifdef DEBUG_INPUT
			cout << "Joystick " << (int)event.jhat.which << " moved hat " << (int)event.jhat.hat << " to " << (int)event.jhat.value << endl;
#endif
			break;
	}
}

void GameEngine::pause()
{
	//pauseMusic();
}

void GameEngine::resume()
{
	//resumeMusic();
}

Rect GameEngine::getCameraView()
{
	Rect rcCamera;
	const float32 tan45_2 = tan(DEG2RAD*45/2);
	const float32 fAspect = (float32)getWidth() / (float32)getHeight();
	rcCamera.bottom = (tan45_2 * m_fDefCameraZ);
	rcCamera.top = -(tan45_2 * m_fDefCameraZ);
	rcCamera.left = rcCamera.bottom * fAspect;
	rcCamera.right = rcCamera.top * fAspect;
	rcCamera.offset(CameraPos.x, CameraPos.y);
	return rcCamera;
}

Point GameEngine::worldMovement(Point cursormove)
{
	cursormove.x /= (float32)getWidth();
	cursormove.y /= (float32)getHeight();
	
	Rect rcCamera = getCameraView();
	cursormove.x *= rcCamera.width();
	cursormove.y *= -rcCamera.height();	//Flip y
	
	return cursormove;
}

Point GameEngine::worldPosFromCursor(Point cursorpos)
{
	//Rectangle that the camera can see in world space
	Rect rcCamera = getCameraView();
	
	//Our relative position in window rect space (in rage 0-1)
	cursorpos.x /= (float32)getWidth();
	cursorpos.y /= (float32)getHeight();
	
	//Multiply this by the size of the world rect to get the relative cursor pos
	cursorpos.x = cursorpos.x * rcCamera.width() + rcCamera.left;
	cursorpos.y = cursorpos.y * rcCamera.height() + rcCamera.top;	//Flip on y axis
	
	return cursorpos;
}

bool GameEngine::loadConfig(string sFilename)
{
	errlog << "Parsing config file " << sFilename << endl;
	//Open file
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing config file: Error " << iErr << ". Ignoring..." << endl;
		if(isFullscreen())
			setInitialFullscreen();
		delete doc;
		return false;	//No file; ignore
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		errlog << "Error: Root element NULL in XML file. Ignoring..." << endl;
		if(isFullscreen())
			setInitialFullscreen();
		delete doc;
		return false;
	}
	
	XMLElement* window = root->FirstChildElement("window");
	if(window != NULL)
	{
		bool bFullscreen = isFullscreen();
		bool bMaximized = isMaximized();
		uint32_t width = getWidth();
		uint32_t height = getHeight();
		uint32_t framerate = getFramerate();
		bool bDoubleBuf = getDoubleBuffered();
		bool bPausesOnFocus = pausesOnFocusLost();
		int iVsync = getVsync();
		int iMSAA = getMSAA();
		bool bTexAntialias = getImgBlur();
		float32 fGamma = getGamma();
		
		window->QueryUnsignedAttribute("width", &width);
		window->QueryUnsignedAttribute("height", &height);
		window->QueryBoolAttribute("fullscreen", &bFullscreen);
		window->QueryBoolAttribute("maximized", &bMaximized);
		window->QueryUnsignedAttribute("fps", &framerate);
		window->QueryBoolAttribute("doublebuf", &bDoubleBuf);
		window->QueryIntAttribute("vsync", &iVsync);
		window->QueryIntAttribute("MSAA", &iMSAA);
		window->QueryBoolAttribute("textureantialias", &bTexAntialias);
		window->QueryFloatAttribute("brightness", &fGamma);
		window->QueryBoolAttribute("pauseminimized", &bPausesOnFocus);
		
		const char* cWindowPos = window->Attribute("pos");
		if(cWindowPos != NULL)
		{
			Point pos = pointFromString(cWindowPos);
			setWindowPos(pos);
		}
		setFullscreen(bFullscreen);
		changeScreenResolution(width, height);
		if(bMaximized && !isMaximized() && !bFullscreen)
			maximizeWindow();
		setFramerate(framerate);
		setVsync(iVsync);
		setDoubleBuffered(bDoubleBuf);
		setMSAA(iMSAA);
		setImgBlur(bTexAntialias);
		setGamma(fGamma);
		pauseOnKeyboard(bPausesOnFocus);
	}
	
	XMLElement* joystick = root->FirstChildElement("joystick");
	if(joystick != NULL)
	{
		joystick->QueryIntAttribute("axistripthreshold", &JOY_AXIS_TRIP);
		joystick->QueryUnsignedAttribute("backbutton", &JOY_BUTTON_BACK);
		joystick->QueryUnsignedAttribute("startbutton", &JOY_BUTTON_START);
		joystick->QueryUnsignedAttribute("Y", &JOY_BUTTON_Y);
		joystick->QueryUnsignedAttribute("X", &JOY_BUTTON_X);
		joystick->QueryUnsignedAttribute("A", &JOY_BUTTON_A);
		joystick->QueryUnsignedAttribute("B", &JOY_BUTTON_B);
		joystick->QueryUnsignedAttribute("LB", &JOY_BUTTON_LB);
		joystick->QueryUnsignedAttribute("RB", &JOY_BUTTON_RB);
		joystick->QueryUnsignedAttribute("leftstick", &JOY_BUTTON_LSTICK);
		joystick->QueryUnsignedAttribute("rightstick", &JOY_BUTTON_RSTICK);
		joystick->QueryUnsignedAttribute("horizontalaxis1", &JOY_AXIS_HORIZ);
		joystick->QueryUnsignedAttribute("verticalaxis1", &JOY_AXIS_VERT);
		joystick->QueryUnsignedAttribute("horizontalaxis2", &JOY_AXIS2_HORIZ);
		joystick->QueryUnsignedAttribute("verticalaxis2", &JOY_AXIS2_VERT);
		joystick->QueryUnsignedAttribute("ltaxis", &JOY_AXIS_LT);
		joystick->QueryUnsignedAttribute("rtaxis", &JOY_AXIS_RT);
	}
	
	XMLElement* keyboard = root->FirstChildElement("keyboard");
	if(keyboard != NULL)
	{
		const char* cUpKey1 = keyboard->Attribute("upkey1");
		const char* cUpKey2 = keyboard->Attribute("upkey2");
		const char* cDownKey1 = keyboard->Attribute("downkey1");
		const char* cDownKey2 = keyboard->Attribute("downkey2");
		const char* cLeftKey1 = keyboard->Attribute("leftkey1");
		const char* cLeftKey2 = keyboard->Attribute("leftkey2");
		const char* cRightKey1 = keyboard->Attribute("rightkey1");
		const char* cRightKey2 = keyboard->Attribute("rightkey2");
		const char* cEnter1 = keyboard->Attribute("enter1");
		const char* cEnter2 = keyboard->Attribute("enter2");
		if(cUpKey1)
			KEY_UP1 = SDL_GetScancodeFromName(cUpKey1);
		if(cUpKey2)
			KEY_UP2 = SDL_GetScancodeFromName(cUpKey2);
		if(cDownKey1)
			KEY_DOWN1 = SDL_GetScancodeFromName(cDownKey1);
		if(cDownKey2)
			KEY_DOWN2 = SDL_GetScancodeFromName(cDownKey2);
		if(cLeftKey1)
			KEY_LEFT1 = SDL_GetScancodeFromName(cLeftKey1);
		if(cLeftKey2)
			KEY_LEFT2 = SDL_GetScancodeFromName(cLeftKey2);
		if(cRightKey1)
			KEY_RIGHT1 = SDL_GetScancodeFromName(cRightKey1);
		if(cRightKey2)
			KEY_RIGHT2 = SDL_GetScancodeFromName(cRightKey2);
		if(cEnter1)
			KEY_ENTER1 = SDL_GetScancodeFromName(cEnter1);
		if(cEnter2)
			KEY_ENTER2 = SDL_GetScancodeFromName(cEnter2);
	}
	
	delete doc;
	return true;
}

void GameEngine::saveConfig(string sFilename)
{
	errlog << "Saving config XML " << sFilename << endl;
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("config");
	
	XMLElement* window = doc->NewElement("window");
	window->SetAttribute("width", getWidth());
	window->SetAttribute("height", getHeight());
	window->SetAttribute("fullscreen", isFullscreen());
	window->SetAttribute("maximized", isMaximized());
	window->SetAttribute("pos", pointToString(getWindowPos()).c_str());
	window->SetAttribute("fps", (uint32_t)(getFramerate()));
	window->SetAttribute("vsync", getVsync());
	window->SetAttribute("doublebuf", getDoubleBuffered());
	window->SetAttribute("MSAA", getMSAA());
	window->SetAttribute("textureantialias", getImgBlur());
	window->SetAttribute("brightness", getGamma());
	window->SetAttribute("pauseminimized", pausesOnFocusLost());
	root->InsertEndChild(window);
	
	XMLElement* joystick = doc->NewElement("joystick");
	joystick->SetAttribute("axistripthreshold", JOY_AXIS_TRIP);
	joystick->SetAttribute("backbutton", JOY_BUTTON_BACK);
	joystick->SetAttribute("startbutton", JOY_BUTTON_START);
	joystick->SetAttribute("Y", JOY_BUTTON_Y);
	joystick->SetAttribute("X", JOY_BUTTON_X);
	joystick->SetAttribute("A", JOY_BUTTON_A);
	joystick->SetAttribute("B", JOY_BUTTON_B);
	joystick->SetAttribute("LB", JOY_BUTTON_LB);
	joystick->SetAttribute("RB", JOY_BUTTON_RB);
	joystick->SetAttribute("leftstick", JOY_BUTTON_LSTICK);
	joystick->SetAttribute("rightstick", JOY_BUTTON_RSTICK);
	joystick->SetAttribute("horizontalaxis1", JOY_AXIS_HORIZ);
	joystick->SetAttribute("verticalaxis1", JOY_AXIS_VERT);
	joystick->SetAttribute("horizontalaxis2", JOY_AXIS2_HORIZ);
	joystick->SetAttribute("verticalaxis2", JOY_AXIS2_VERT);
	joystick->SetAttribute("ltaxis", JOY_AXIS_LT);
	joystick->SetAttribute("rtaxis", JOY_AXIS_RT);
	root->InsertEndChild(joystick);
	
	XMLElement* keyboard = doc->NewElement("keyboard");
	keyboard->SetAttribute("upkey1", SDL_GetScancodeName(KEY_UP1));
	keyboard->SetAttribute("upkey2", SDL_GetScancodeName(KEY_UP2));
	keyboard->SetAttribute("downkey1", SDL_GetScancodeName(KEY_DOWN1));
	keyboard->SetAttribute("downkey2", SDL_GetScancodeName(KEY_DOWN2));
	keyboard->SetAttribute("leftkey1", SDL_GetScancodeName(KEY_LEFT1));
	keyboard->SetAttribute("leftkey2", SDL_GetScancodeName(KEY_LEFT2));
	keyboard->SetAttribute("rightkey1", SDL_GetScancodeName(KEY_RIGHT1));
	keyboard->SetAttribute("rightkey2", SDL_GetScancodeName(KEY_RIGHT2));
	keyboard->SetAttribute("enter1", SDL_GetScancodeName(KEY_ENTER1));
	keyboard->SetAttribute("enter2", SDL_GetScancodeName(KEY_ENTER2));
	root->InsertEndChild(keyboard);
	
	doc->InsertFirstChild(root);
	doc->SaveFile(sFilename.c_str());
	delete doc;
}

obj* GameEngine::objFromXML(string sXMLFilename, Point ptOffset, Point ptVel)
{
	errlog << "Parsing object XML file " << sXMLFilename << endl;
	//Open file
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing object XML file: Error " << iErr << endl;
		delete doc;
		return NULL;
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		errlog << "Error: Root element NULL in XML file." << endl;
		delete doc;
		return NULL;
	}
	
	obj* o = new obj;
	
	const char* cMeshImg = root->Attribute("meshimg");
	if(cMeshImg != NULL)
		o->meshImg = getImage(cMeshImg);
	
	const char* cMeshImgSize = root->Attribute("meshsize");
	if(cMeshImgSize != NULL)
		o->meshSize = pointFromString(cMeshImgSize);
		
	bool makeMesh = false;
	root->QueryBoolAttribute("softbody", &makeMesh);
	
	string sMeshCenterObj = "";
	Point pMeshSize(10,10);
	
	if(makeMesh)
	{
		const char* cBodyRes = root->Attribute("softbodyres");
		if(cBodyRes != NULL)
			pMeshSize = pointFromString(cBodyRes);
			
		const char* cBodyCenter = root->Attribute("softbodycenter");
		if(cBodyCenter != NULL)
			sMeshCenterObj = cBodyCenter;
		else
			makeMesh = false;
	}
	
	map<string, b2Body*> mBodyNames;
	
	//Add segments
	for(XMLElement* segment = root->FirstChildElement("segment"); segment != NULL; segment = segment->NextSiblingElement("segment"))
	{
		physSegment* seg = new physSegment;
		seg->parent = o;
		XMLElement* body = segment->FirstChildElement("body");
		if(body != NULL)
		{
			string sBodyName;
			const char* cBodyName = body->Attribute("name");
			if(cBodyName)
				sBodyName = cBodyName;
			
			Point pos = ptOffset;
			const char* cBodyPos = body->Attribute("pos");
			if(cBodyPos)
			{
				Point p = pointFromString(cBodyPos);
				pos.x += p.x;
				pos.y += p.y;
			}
			
			string sBodyType = "dynamic";
			const char* cBodyType = body->Attribute("type");
			if(cBodyType)
				sBodyType = cBodyType;
			
			b2BodyDef bodyDef;
			
			if(sBodyType == "dynamic")
				bodyDef.type = b2_dynamicBody;
			else if(sBodyType == "kinematic")
				bodyDef.type = b2_kinematicBody;
			else
				bodyDef.type = b2_staticBody;
			bodyDef.position = pos;
			
			
			//Create body fixtures
			for(XMLElement* fixture = body->FirstChildElement("fixture"); fixture != NULL; fixture = fixture->NextSiblingElement("fixture"))
			{
				b2FixtureDef fixtureDef;
				b2PolygonShape dynamicBox;
				b2CircleShape dynamicCircle;
				
				const char* cFixType = fixture->Attribute("type");
				if(!cFixType) continue;
				string sFixType = cFixType;
				if(sFixType == "box")
				{
					const char* cBoxSize = fixture->Attribute("size");
					if(!cBoxSize) continue;
					
					Point pBoxSize = pointFromString(cBoxSize);
					dynamicBox.SetAsBox(pBoxSize.x/2.0f, pBoxSize.y/2.0f);
					fixtureDef.shape = &dynamicBox;
				}
				else if(sFixType == "circle")
				{
					const char* cCircPos = fixture->Attribute("pos");
					if(cCircPos)
						dynamicCircle.m_p = pointFromString(cCircPos);
					bodyDef.fixedRotation = true;
						
					dynamicCircle.m_radius = 1.0f;
					fixture->QueryFloatAttribute("radius", &dynamicCircle.m_radius);
					fixtureDef.shape = &dynamicCircle;
				}
				//else TODO
				
				fixtureDef.density = 1.0f;
				fixtureDef.friction = 0.3f;
				fixture->QueryFloatAttribute("friction", &fixtureDef.friction);
				fixture->QueryFloatAttribute("density", &fixtureDef.density);
				
				b2Body* bod = getWorld()->CreateBody(&bodyDef);
				seg->body = bod;
				
				mBodyNames[sBodyName] = bod;
				bod->CreateFixture(&fixtureDef);
			}
		}
		XMLElement* layer = segment->FirstChildElement("layer");
		if(layer != NULL)
		{
			seg->fromXML(layer);
			/*const char* cImgPath = image->Attribute("img");
			if(cImgPath)
				seg->img = getImage(cImgPath);
				
			const char* cImgSize = image->Attribute("size");
			if(cImgSize)
				seg->size = pointFromString(cImgSize);*/
		}
		o->addSegment(seg);
	}
	//Create joints
	for(XMLElement* joint = root->FirstChildElement("joint"); joint != NULL; joint = joint->NextSiblingElement("joint"))
	{
		const char* cJointType = joint->Attribute("type");
		if(cJointType)
		{
			string sJointType = cJointType;
			
			if(sJointType == "distance")
			{
				b2DistanceJointDef jd;
				const char* cBodyA = joint->Attribute("bodyA");
				const char* cBodyB = joint->Attribute("bodyB");
				if(!cBodyA || !cBodyB) continue;
				if(!mBodyNames.count(cBodyA) || !mBodyNames.count(cBodyB)) continue;
				
				jd.bodyA = mBodyNames[cBodyA];
				jd.bodyB = mBodyNames[cBodyB];
				
				jd.frequencyHz = 2.0f;
				jd.dampingRatio = 0.0f;
				
				joint->QueryFloatAttribute("frequencyHz", &jd.frequencyHz);
				joint->QueryFloatAttribute("dampingRatio", &jd.dampingRatio);
				
				const char* cAnchorA = joint->Attribute("anchorA");
				const char* cAnchorB = joint->Attribute("anchorB");
				
				jd.localAnchorA.Set(0,0);
				jd.localAnchorB.Set(0,0);
				if(cAnchorA)
					jd.localAnchorA = pointFromString(cAnchorA);
				if(cAnchorB)
					jd.localAnchorB = pointFromString(cAnchorB);
				
				b2Vec2 p1, p2, d;
				p1 = jd.bodyA->GetWorldPoint(jd.localAnchorA);
				p2 = jd.bodyB->GetWorldPoint(jd.localAnchorB);
				d = p2 - p1;
				jd.length = d.Length();
				
				getWorld()->CreateJoint(&jd);
			}
			//else TODO
		}
	}
	
	//Set up mesh animation for object
	if(makeMesh && mBodyNames.count(sMeshCenterObj))
	{
		o->meshLattice = new lattice(pMeshSize.x, pMeshSize.y);
		//sinLatticeAnim* manim = new sinLatticeAnim(o->meshLattice);
		//manim->amp = 0.05;
		softBodyAnim* manim = new softBodyAnim(o->meshLattice);
		manim->addBody(mBodyNames[sMeshCenterObj], true);
		manim->size = o->meshSize;
		//o->meshSize.Set(1,1);	//Can't take this into account on draw time; mesh will deform by hand
		for(map<string, b2Body*>::iterator i = mBodyNames.begin(); i != mBodyNames.end(); i++)
		{
			if(i->first != sMeshCenterObj)
				manim->addBody(i->second);
		}
		manim->init();
		o->meshAnim = manim;
	}
	
	delete doc;
	return o;
}

#define MAX_SHIP_SPEED 10
#define SHIP_ACCEL 70
#define SHIP_SLOW_FAC 0.985;

void GameEngine::handleKeys()
{
	float max_ship_vel = MAX_SHIP_SPEED;
	float ship_accel = SHIP_ACCEL;
	if(keyDown(SDL_SCANCODE_SPACE))
	{
		max_ship_vel *= 2.0f;
		ship_accel *= 2.0f;
	}

	float dt = 1.0/getFramerate();
	//if(keyDown(SDL_SCANCODE_A))
	//	fSunRotAmt -= dt * 50;
	//else if(keyDown(SDL_SCANCODE_D))
	//	fSunRotAmt += dt * 50;
		
	if(keyDown(SDL_SCANCODE_LEFT))
		fPlanetRotAmt -= dt * 100;
	else if(keyDown(SDL_SCANCODE_RIGHT))
		fPlanetRotAmt += dt * 100;
	
	if(ship != NULL && ship->getBody() != NULL)
	{
		Point v(0,0);
		Point shipVel = ship->getBody()->GetLinearVelocity();
		bool accel = false;
		
		if(keyDown(KEY_UP1))
		{
			v.y += dt * ship_accel;
			accel = true;
		}
		if(keyDown(KEY_DOWN1))
		{
			v.y -= dt * ship_accel;
			accel = true;
		}
		if(keyDown(KEY_LEFT1))
		{
			v.x -= dt * ship_accel;
			accel = true;
		}
		if(keyDown(KEY_RIGHT1))
		{
			v.x += dt * ship_accel;
			accel = true;
		}
			
		/*/TODO: Ship rotation calculations
		list<physSegment*>::iterator segiter = ship->segments.begin();
		if(segiter != ship->segments.end())
		{
			physSegment* sg = *segiter;
			if(sg->obj3D != NULL)
			{
				float fAngleBetween = (getAngle(v) - getAngle(shipVel))*RAD2DEG;
				sg->obj3D->rot[0] = max(min(fAngleBetween, 45.0f), -45.0f);
				sg->obj3D->rot[1] = 1.0f;
				sg->obj3D->rot[2] = 0.0f;
				sg->obj3D->rot[3] = 0.0f;
			}
		}*/
		
		
		v += shipVel;			
		if(v.Length() > max_ship_vel)
		{
			v.Normalize();
			v *= max_ship_vel;
		}
		
		if(!accel)
		{
			float f = v.Length();
			v.Normalize();
			f *= SHIP_SLOW_FAC;
			v *= f;
		}
			
		ship->getBody()->SetLinearVelocity(v);
	}
	//TODO: Control camera with this
	/*if(keyDown(SDL_SCANCODE_I))
		CameraPos.y -= dt * 5;
	else if(keyDown(SDL_SCANCODE_K))
		CameraPos.y += dt * 5;
	if(keyDown(SDL_SCANCODE_J))
		CameraPos.x += dt * 5;
	else if(keyDown(SDL_SCANCODE_L))
		CameraPos.x -= dt * 5;*/
}

void GameEngine::updateShip()
{
	if(ship != NULL)
	{
		//cout << "Not null" << endl;
		b2Body* b = ship->getBody();
		if(b != NULL)
		{
			//cout << "not null 2" << endl;
			Point p = b->GetPosition();
			Point v = b->GetLinearVelocity();
			if(v.x != 0.0f || v.y != 0.0f)	//Don't rotate if not moving (prevents snap on stop)
			{
				float32 r = getAngle(v);
				b->SetTransform(p, r);
			}
			CameraPos.x = -p.x;
			CameraPos.y = -p.y;
		}
	}
}

void GameEngine::rumbleController(float32 strength, float32 sec, bool priority)
{
	static float32 fLastRumble = 0.0f;
	if(priority)
		fLastRumble = getSeconds() + sec;
	else if(getSeconds() < fLastRumble)
		return;
	strength = max(strength, 0.0f);
	strength = min(strength, 1.0f);
	if(m_rumble != NULL)
		SDL_HapticRumblePlay(m_rumble, strength, sec*1000);
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

void GameEngine::loadScene(string sXMLFilename)
{
	cleanupObjects();
	ship = NULL;
	errlog << "Loading scene " << sXMLFilename << endl;
	
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing object XML file " << sXMLFilename << ": Error " << iErr << endl;
		delete doc;
		return;
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		errlog << "Error: Root element NULL in XML file " << sXMLFilename << endl;
		delete doc;
		return;
	}
	
	//TODO Scene boundaries
	
	//Load layers for the scene
	for(XMLElement* layer = root->FirstChildElement("layer"); layer != NULL; layer = layer->NextSiblingElement("layer"))
	{
		physSegment* seg = new physSegment();
		seg->fromXML(layer);
		
		addScenery(seg);
	}
	
	//Load objects
	for(XMLElement* object = root->FirstChildElement("object"); object != NULL; object = object->NextSiblingElement("object"))
	{
		const char* cFilename = object->Attribute("file");
		const char* cName = object->Attribute("name");
		const char* cPos = object->Attribute("pos");
		const char* cVel = object->Attribute("vel");
		if(cFilename != NULL)
		{
			Point pos(0,0);
			Point vel(0,0);
			
			if(cPos != NULL)
				pos = pointFromString(cPos);
			
			if(cVel != NULL)
				vel = pointFromString(cVel);
			
			obj* o = objFromXML(cFilename, pos, vel);
			
			if(o != NULL && cName != NULL)
			{
				string s = cName;
				if(s == "ship")
				{
					ship = o;
					list<physSegment*>::iterator segiter = o->segments.begin();
					if(segiter != o->segments.end())
					{
						physSegment* sg = *segiter;
						if(sg->obj3D != NULL)
						{
							//sg->obj3D->shaded = false;
							sg->obj3D->useGlobalLight = false;
							sg->obj3D->lightPos[0] = 0.0f;
							sg->obj3D->lightPos[1] = 0.0f;
							sg->obj3D->lightPos[2] = 3.0f;
							sg->obj3D->lightPos[3] = 1.0f;
						}
					}
				}
			}
			
			addObject(o);
		}
	}
	
	//TODO: Load other things
	
	delete doc;
	
	
	
	
	
	
	
	
	
	
	/*
	//Create test object thingy
	
	//Straight from the hello world example
	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = getWorld()->CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2PolygonShape groundBox;

	// The extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 10.0f);

	// Add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);
	
	physSegment* seg = new physSegment;
	seg->body = groundBody;
	seg->img = getImage("res/gfx/metalwall.png");
	seg->size.x = 100.0;
	seg->size.y = 20.0;
	addScenery(seg);
	
	addObject(objFromXML("res/obj/test.xml", Point(0, 3)));
	*/
}























