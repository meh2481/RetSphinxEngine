/*
	Pony48 source - Engine.cpp
	Copyright (c) 2014 Mark Hutcheson
*/

#include "Engine.h"
#ifdef USE_SDL_FRAMEWORK
#include <SDL_syswm.h>
#else
#include <SDL2/SDL_syswm.h>
#endif
#include "opengl-api.h"
ofstream errlog;

void PrintEvent(const SDL_Event * event)
{
	if (event->type == SDL_WINDOWEVENT) {
		switch (event->window.event) {
		case SDL_WINDOWEVENT_SHOWN:
			printf("Window %d shown\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_HIDDEN:
			printf("Window %d hidden\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			printf("Window %d exposed\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_MOVED:
			printf("Window %d moved to %d,%d\n",
					event->window.windowID, event->window.data1,
					event->window.data2);
			break;
		case SDL_WINDOWEVENT_RESIZED:
			printf("Window %d resized to %dx%d\n",
					event->window.windowID, event->window.data1,
					event->window.data2);
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			printf("Window %d minimized\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
			printf("Window %d maximized\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_RESTORED:
			printf("Window %d restored\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_ENTER:
			printf("Mouse entered window %d\n",
					event->window.windowID);
			break;
		case SDL_WINDOWEVENT_LEAVE:
			printf("Mouse left window %d\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			printf("Window %d gained keyboard focus\n",
					event->window.windowID);
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			printf("Window %d lost keyboard focus\n",
					event->window.windowID);
			break;
		case SDL_WINDOWEVENT_CLOSE:
			printf("Window %d closed\n", event->window.windowID);
			break;
		default:
			printf("Window %d got unknown event %d\n",
					event->window.windowID, event->window.event);
			break;
		}
	}
}

bool Engine::_frame()
{
	updateSound();
	
	//Handle input events from SDL
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		//PrintEvent(&event);
		//Update internal cursor position if cursor has moved
		if(event.type == SDL_MOUSEMOTION)
		{
			m_ptCursorPos.x = event.motion.x;
			m_ptCursorPos.y = event.motion.y;
		}
		else if(event.type == SDL_WINDOWEVENT)
		{
			if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST && m_bPauseOnKeyboardFocus)
			{
				m_bPaused = true;
				pause();
			}
			else if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED && m_bPauseOnKeyboardFocus)
			{
				m_bPaused = false;
				resume();
			}
			else if(event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				if(m_bResizable)
					changeScreenResolution(event.window.data1, event.window.data2);
				else
					errlog << "Error! Resize event generated, but resizable flag not set." << endl;
			}
			else if(event.window.event == SDL_WINDOWEVENT_ENTER)
				m_bCursorOutOfWindow = false;
			else if(event.window.event == SDL_WINDOWEVENT_LEAVE)
				m_bCursorOutOfWindow = true;
		}
		else if(event.type == SDL_QUIT)
			return true;
			
		//Let final game engine handle it, whatever the case
		if(!m_bPaused)
			handleEvent(event);
	}
	if(m_bPaused)
	{
		SDL_Delay(100);	//Wait 100 ms
		return m_bQuitting;	//Break out here
	}

	float32 fCurTime = ((float32)SDL_GetTicks())/1000.0;
	if(m_fAccumulatedTime <= fCurTime)
	{
		m_fAccumulatedTime += m_fTargetTime;
		m_iKeystates = SDL_GetKeyboardState(NULL);	//Get current key state
		frame(m_fTargetTime);	//Box2D wants fixed timestep, so we use target framerate here instead of actual elapsed time
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
	
	//Draw cursor over everything
	glClear(GL_DEPTH_BUFFER_BIT);
	if(m_cursor && m_bCursorShow && !m_bCursorOutOfWindow)
		m_cursor->draw();
	
	//Draw gamma/brightness overlay on top of everything else
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	Color fillCol;
	if(m_fGamma > 1.0f)
	{
		glBlendFunc(GL_DST_COLOR, GL_ONE);
		fillCol.set(m_fGamma - 1.0, m_fGamma - 1.0, m_fGamma - 1.0, 1);
	}
	else
	{
		glBlendFunc( GL_ZERO, GL_SRC_COLOR );
		fillCol.set(m_fGamma, m_fGamma, m_fGamma, 1);
	}
	fillScreen(fillCol);
	
	//Reset blend func
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//End rendering and update the screen
	SDL_GL_SwapWindow(m_Window);
}

Engine::Engine(uint16_t iWidth, uint16_t iHeight, string sTitle, string sAppName, string sIcon, bool bResizable)
{
	m_sTitle = sTitle;
	m_sAppName = sAppName;
	errlog.open((getSaveLocation() + "err.log").c_str());
	if(errlog.fail())
		errlog.open("err.log");
	m_sIcon = sIcon;
	m_bResizable = bResizable;
	b2Vec2 gravity(0.0, -9.8);	//Vector for our world's gravity
	m_physicsWorld = new b2World(gravity);
	m_ptCursorPos.SetZero();
	m_physicsWorld->SetAllowSleeping(true);
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iMSAA = 0;	//Default: MSAA off
	m_iKeystates = NULL;
	m_bShowCursor = true;
	m_fFramerate = 60.0f;
	setFramerate(60);	 //60 fps default
	m_bFullscreen = true;
	setup_sdl();
	setup_opengl();
	m_fGamma = 1.0f;
	m_bPaused = false;
	m_bPauseOnKeyboardFocus = true;
	m_cursor = NULL;
	m_bCursorShow = true;
	m_bCursorOutOfWindow = false;

	//Initialize engine stuff
	m_fAccumulatedTime = 0.0;
	//m_bFirstMusic = true;
	m_bQuitting = false;
	srand(SDL_GetTicks());	//Not as random as it could be... narf
	m_fTimeScale = 1.0f;

	errlog << "Initializing FMOD..." << endl;
	if(FMOD_System_Create(&m_audioSystem) != FMOD_OK || FMOD_System_Init(m_audioSystem, 128, FMOD_INIT_NORMAL, 0) != FMOD_OK)
	{
		errlog << "Failed to init FMOD." << std::endl;
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
		errlog << numDrivers << " recording drivers available." << endl;
		for(int i = 0; i < numDrivers; i++)
		{
			FMOD_System_GetRecordDriverInfo(m_audioSystem, i, driverInfoStr, DRIVER_INFO_STR_SIZE, NULL);
			errlog << "Driver " << i << ": " << driverInfoStr << endl;
		}
	}
}

Engine::~Engine()
{
	SDL_DestroyWindow(m_Window);

	//Clean up our image map
	errlog << "Clearing images" << endl;
	clearImages();

	//Clean up our sound effects
	if(!m_bSoundDied)
	{
		for(map<string, FMOD_SOUND*>::iterator i = m_sounds.begin(); i != m_sounds.end(); i++)
			FMOD_Sound_Release(i->second);
	}
	
	//Clean up FMOD
	if(!m_bSoundDied)
	{
		FMOD_System_Close(m_audioSystem);
		FMOD_System_Release(m_audioSystem);
	}

	// Clean up and shutdown
	errlog << "Deleting phys world" << endl;
	delete m_physicsWorld;
	errlog << "Quit SDL" << endl;
	SDL_Quit();
}

void Engine::start()
{
	// Load all that we need to
	init(lCommandLine);
	// Let's rock now!
	while(!_frame());
}

void Engine::fillRect(Point p1, Point p2, Color col)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glColor4f(col.r,col.g,col.b,col.a);
	glVertex3f(p1.x, p1.y, 0.0);
	glVertex3f(p2.x, p1.y, 0.0);
	glVertex3f(p2.x, p2.y, 0.0);
	glVertex3f(p1.x, p2.y, 0.0);
	glEnd();
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

bool Engine::hasMic()
{
	if(m_bSoundDied) return false;
	int numDrivers = 0;
	FMOD_System_GetRecordNumDrivers(m_audioSystem, &numDrivers);
	return numDrivers;
}

void Engine::createSound(string sPath, string sName)
{
	if(m_bSoundDied || m_sounds.count(sName)) return;	//Don't duplicate sounds or attempt to play sounds if we can't
	errlog << "Load sound " << sPath << endl;
	FMOD_SOUND* handle;
	FMOD_MODE open_mode = FMOD_CREATESAMPLE;
#ifndef DEBUG_REVSOUND
	if(sName == "music")
		open_mode = FMOD_CREATESTREAM;	//Open music as a stream, sounds as samples (so the sounds can play multiple times at once)
#endif
	if(FMOD_System_CreateSound(m_audioSystem, sPath.c_str(), open_mode, 0, &handle) == FMOD_OK)
		m_sounds[sName] = handle;
}

void Engine::playSound(string sName, float32 volume, float32 pan, float32 pitch)
{
	if(m_bSoundDied || !m_sounds.count(sName)) return;
	FMOD_CHANNEL* channel;
	FMOD_System_PlaySound(m_audioSystem, FMOD_CHANNEL_FREE, m_sounds[sName], false, &channel);
	pair<string, FMOD_CHANNEL*> chan;
	chan.first = sName;
	chan.second = channel;
	FMOD_Channel_SetVolume(channel, volume);
	FMOD_Channel_SetPan(channel, pan);
	FMOD_Channel_SetFrequency(channel, pitch * soundFreqDefault);
	m_channels.insert(chan);
}

FMOD_CHANNEL* Engine::getChannel(string sSoundName)
{
	multimap<string, FMOD_CHANNEL*>::iterator i = m_channels.find(sSoundName);
	if(i != m_channels.end())
		return i->second;
	return NULL;
}

void Engine::pauseMusic()
{
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPaused(getChannel("music"), true);
}

void Engine::stopMusic()
{
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPaused(getChannel("music"), true);
}

void Engine::restartMusic()
{
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPosition(getChannel("music"), 0, FMOD_TIMEUNIT_MS);
}

void Engine::resumeMusic()
{
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPaused(getChannel("music"), false);
}

void Engine::seekMusic(float32 fTime)
{
	if(m_bSoundDied) return;
	if(!m_channels.count("music")) return;
	FMOD_Channel_SetPosition(getChannel("music"), fTime * 1000.0, FMOD_TIMEUNIT_MS);
}

float32 Engine::getMusicPos()
{
	if(m_channels.count("music"))
	{
		FMOD_CHANNEL* mus = getChannel("music");
		unsigned int ms;
		FMOD_Channel_GetPosition(mus, &ms, FMOD_TIMEUNIT_MS);
		return (float32)ms / 1000.0f;
	}
	return -1;
}

void Engine::playMusic(string sName, float32 volume, float32 pan, float32 pitch)
{
	if(m_bSoundDied) return;
	if(m_sounds.count("music"))
	{
		stopMusic();
		m_sounds.erase("music");
		m_channels.erase("music");
	}
	createSound(sName, "music");
	playSound("music", volume, pan, pitch);
	if(m_channels.count("music"))
	{
		FMOD_CHANNEL* mus = getChannel("music");
		FMOD_Channel_SetLoopCount(mus, -1);
		FMOD_Channel_SetMode(mus, FMOD_LOOP_NORMAL);
		FMOD_Channel_SetPosition(mus, 0, FMOD_TIMEUNIT_MS);
	}
}

void Engine::musicLoop(float32 startSec, float32 endSec)
{
	if(m_bSoundDied) return;
	if(m_channels.count("music"))
	{
		FMOD_CHANNEL* mus = getChannel("music");
		FMOD_Channel_SetLoopPoints(mus, startSec * 1000, FMOD_TIMEUNIT_MS, endSec * 1000, FMOD_TIMEUNIT_MS);
		//Flush music stream
		FMOD_Channel_SetMode(mus, FMOD_LOOP_NORMAL);
		unsigned int ms;
		FMOD_Channel_GetPosition(mus, &ms, FMOD_TIMEUNIT_MS);
		FMOD_Channel_SetPosition(mus, ms, FMOD_TIMEUNIT_MS);
	}
}

void Engine::volumeMusic(float32 fVol)
{
	if(m_channels.count("music"))
	{
		FMOD_CHANNEL* mus = getChannel("music");
		FMOD_Channel_SetVolume(mus, fVol);
	}
}

float32 Engine::getMusicFrequency()
{
	if(m_channels.count("music"))
	{
		float freq;
		FMOD_Channel_GetFrequency(getChannel("music"), &freq);
		return freq;
	}
	return -1;
}

void Engine::setMusicFrequency(float32 freq)
{
	if(m_channels.count("music"))
		FMOD_Channel_SetFrequency(getChannel("music"), freq);
}

bool Engine::keyDown(int32_t keyCode)
{
	if(m_iKeystates == NULL) return false;	//On first cycle, this can be NULL and cause segfaults otherwise
	
	//HACK: See if one of our combined keycodes
	if(keyCode == SDL_SCANCODE_CTRL) return (keyDown(SDL_SCANCODE_LCTRL)||keyDown(SDL_SCANCODE_RCTRL));
	if(keyCode == SDL_SCANCODE_SHIFT) return (keyDown(SDL_SCANCODE_LSHIFT)||keyDown(SDL_SCANCODE_RSHIFT));
	if(keyCode == SDL_SCANCODE_ALT) return (keyDown(SDL_SCANCODE_LALT)||keyDown(SDL_SCANCODE_RALT));
	if(keyCode == SDL_SCANCODE_GUI) return (keyDown(SDL_SCANCODE_LGUI)||keyDown(SDL_SCANCODE_RGUI));
	
	//Otherwise, just use our pre-polled list we got from SDL
	return(m_iKeystates[keyCode]);
}

void Engine::setFramerate(float32 fFramerate)
{
	if(fFramerate < 30.0)
	fFramerate = 30.0;	//30fps is bare minimum
	if(m_fFramerate == 0.0)
		m_fAccumulatedTime = (float32)SDL_GetTicks()/1000.0;	 //If we're stuck at 0fps for a while, this number could be huge, which would cause unlimited fps for a bit
	m_fFramerate = fFramerate;
	m_fTargetTime = 1.0 / m_fFramerate;
}

void Engine::setup_sdl()
{

	if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		errlog << "SDL_InitSubSystem Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	if(SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0)
		errlog << "Unable to init SDL2 gamepad subsystem." << endl;

	errlog << "Loading OpenGL..." << std::endl;

	if (SDL_GL_LoadLibrary(NULL) == -1)
	{
		errlog << "SDL_GL_LoadLibrary Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	// Quit SDL properly on exit
	atexit(SDL_Quit);
	
	// Set the minimum requirements for the OpenGL window
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	
	//Vsync and stuff	//TODO: Toggle? Figure out what it's actually doing? My pathetic gfx card doesn't do anything with any of these values
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	//Apparently double-buffering or something
	
	// Create SDL window
	Uint32 flags = SDL_WINDOW_OPENGL;
	if(m_bResizable)
		flags |= SDL_WINDOW_RESIZABLE;
	
	m_Window = SDL_CreateWindow(m_sTitle.c_str(),
							 SDL_WINDOWPOS_UNDEFINED,
							 SDL_WINDOWPOS_UNDEFINED,
							 m_iWidth, 
							 m_iHeight,
							 flags);

	if(m_Window == NULL)
	{
		errlog << "Couldn't set video mode: " << SDL_GetError() << endl;
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1); //Share objects between OpenGL contexts
	SDL_GL_CreateContext(m_Window);
	if(SDL_GL_SetSwapInterval(-1) == -1) //Apparently Vsync or something
		SDL_GL_SetSwapInterval(1);

	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, 0, &mode);
	if(!mode.refresh_rate)	//If 0, display doesn't care, so default to 60
		mode.refresh_rate = 60;
	setFramerate(mode.refresh_rate);
	
	int numDisplays = SDL_GetNumVideoDisplays();
	errlog << "Available displays: " << numDisplays << endl;
	for(int display = 0; display < numDisplays; display++)
	{
		int num = SDL_GetNumDisplayModes(display);
		errlog << "Available modes for display " << display+1 << ':' << endl;
		for(int i = 0; i < num; i++)
		{
			SDL_GetDisplayMode(display, i, &mode);
			errlog << "Mode: " << mode.w << "x" << mode.h << " " << mode.refresh_rate << "Hz" << endl;
		}
	}
	
	
	//Hide system cursor for SDL, so we can use our own
	SDL_ShowCursor(0);
	
	OpenGLAPI::LoadSymbols();	//Load our OpenGL symbols to use
	
	_loadicon();	//Load our window icon
	
}

//Set up OpenGL
void Engine::setup_opengl()
{
	// Make the viewport
	glViewport(0, 0, m_iWidth, m_iHeight);

	// set the clear color to black
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	//Enable image transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set the camera projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)m_iWidth/(GLfloat)m_iHeight, 0.1f, 500.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	
	//Set up lighting
	glShadeModel(GL_SMOOTH);
	
	setMSAA(m_iMSAA);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
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
	errlog << "Load icon " << m_sIcon << endl;
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib(0);
	BYTE* bits(0);
	unsigned int width(0), height(0);
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(m_sIcon.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(m_sIcon.c_str());
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
	{
		errlog << "Unknown image type for file " << m_sIcon << endl;
		return;
	}
	
	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, m_sIcon.c_str());
	else
		errlog << "File " << m_sIcon << " doesn't support reading." << endl;
	//if the image failed to load, return failure
	if(!dib)
	{
		errlog << "Error loading image " << m_sIcon.c_str() << endl;
		return;
	}	
	//retrieve the image data
	
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	
	FreeImage_FlipVertical(dib);
	
	bits = FreeImage_GetBits(dib);	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
	{
		errlog << "Something went terribly horribly wrong with getting image bits; just sit and wait for the singularity" << endl;
		return;
	}
	
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(bits, width, height, FreeImage_GetBPP(dib), FreeImage_GetPitch(dib), 
													0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	if(surface == NULL)
		errlog << "NULL surface for icon " << m_sIcon << endl;
	else
	{
		SDL_SetWindowIcon(m_Window, surface);
		SDL_FreeSurface(surface);
	}
	
	FreeImage_Unload(dib);
}

void Engine::changeScreenResolution(float32 w, float32 h)
{
	errlog << "Changing screen resolution to " << w << ", " << h << endl;
	int vsync = SDL_GL_GetSwapInterval();
//In Windoze, we copy the graphics memory to our new context, so we don't have to reload all of our images and stuff every time the resolution changes
#ifdef _WIN32
	SDL_SysWMinfo info;
 
	//Get window handle from SDL
	SDL_VERSION(&info.version);
	if(SDL_GetWindowWMInfo(m_Window, &info) == -1) 
	{
		errlog << "SDL_GetWMInfo #1 failed" << endl;
		return;
	}

	//Get device context handle
	HDC tempDC = GetDC(info.info.win.window);

	//Create temporary context
	HGLRC tempRC = wglCreateContext(tempDC);
	if(tempRC == NULL) 
	{
		errlog << "wglCreateContext failed" << endl;
		return;
	}
	
	//Share resources to temporary context
	SetLastError(0);
	if(!wglShareLists(wglGetCurrentContext(), tempRC))
	{
		errlog << "wglShareLists #1 failed" << endl;
		return;
	}
#endif
	
	m_iWidth = w;
	m_iHeight = h;
	
	if(m_bFullscreen)
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	SDL_SetWindowSize(m_Window, m_iWidth, m_iHeight);

	SDL_GL_CreateContext(m_Window);
	if(SDL_GL_SetSwapInterval(vsync) == -1) //old vsync won't work in this new mode; default to vsync on
		SDL_GL_SetSwapInterval(1);
	
	//Set OpenGL back up
	setup_opengl();
	
#ifdef _WIN32
	//Previously used structure may possibly be invalid, to be sure we get it again
	SDL_VERSION(&info.version);
	if(SDL_GetWindowWMInfo(m_Window, &info) == -1) 
	{
		errlog << "SDL_GetWMInfo #2 failed" << endl;
		return;
	}
 
	//Share resources to our new SDL-created context
	if(!wglShareLists(tempRC, wglGetCurrentContext()))
	{
		errlog << "wglShareLists #2 failed" << endl;
		return;
	}
 
	//We no longer need our temporary context
	if(!wglDeleteContext(tempRC))
	{
		errlog << "wglDeleteContext failed" << endl;
		return;
	}
#else
	//Reload images & models
#ifdef IMG_RELOAD
	reloadImages();
#endif
#endif
}

void Engine::toggleFullscreen()
{
	m_bFullscreen = !m_bFullscreen;
	if(m_bFullscreen)
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(m_Window, 0);
}

void Engine::setFullscreen(bool bFullscreen)
{
	if(m_bFullscreen == bFullscreen) 
		return;
	toggleFullscreen();
}

Point Engine::getWindowPos()
{
	Point p;
	int x, y;
	SDL_GetWindowPosition(m_Window, &x, &y);
	p.x = x;
	p.y = y;
	return p;
}

void Engine::setWindowPos(Point pos)
{
	SDL_SetWindowPosition(m_Window, pos.x, pos.y);
}

void Engine::maximizeWindow()
{
	SDL_MaximizeWindow(m_Window);
}

bool Engine::isMaximized()
{
#ifdef _WIN32	//Apparently SDL_GetWindowFlags() is completely broken in SDL2 for determining maximization, and nobody seems to care: https://bugzilla.libsdl.org/show_bug.cgi?id=2282
	SDL_SysWMinfo info;
 
	//Get window handle from SDL
	SDL_VERSION(&info.version);
	if(SDL_GetWindowWMInfo(m_Window, &info) == -1) 
	{
		errlog << "SDL_GetWMInfo failed" << endl;
		return false;
	}
	
	return IsZoomed(info.info.win.window);
#else
	return (SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MAXIMIZED);	//TODO: This is borked in Linux, also. Fix or wait for SDL patch
#endif
}

void Engine::setCursorPos(int32_t x, int32_t y)
{
	SDL_WarpMouseInWindow(m_Window, x, y);
//#ifdef __APPLE__
//	hideCursor(); //TODO: Warping the mouse shows it again in Mac, and this doesn't work. Hermph.
//#endif
}

bool Engine::getCursorDown(int iButtonCode)
{
	Uint8 ms = SDL_GetMouseState(NULL, NULL);
	switch(iButtonCode)
	{
		case LMB:
			return(ms & SDL_BUTTON(SDL_BUTTON_LEFT));
		case RMB:
			return(ms & SDL_BUTTON(SDL_BUTTON_RIGHT));
		case MMB:
			return(ms & SDL_BUTTON(SDL_BUTTON_MIDDLE));
		default:
			errlog << "Unsupported mouse code: " << iButtonCode << endl;
			break;
	}
	return false;
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
				sSwitch.erase(0,1);
			sSwitch.erase(0,1);
			
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

void Engine::addObject(obj* o)
{
	m_lObjects.push_back(o);
}

void Engine::drawObjects()
{
	multiset<physSegment*>::iterator layer;
	for(layer = m_lScenery.begin(); layer != m_lScenery.end(); layer++)	//Draw bg layers
	{
		if((*layer)->depth > 0)
			break;
		(*layer)->draw();
	}
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)	//Draw objects
		(*i)->draw();
	for(; layer != m_lScenery.end(); layer++)	//Draw fg layers
		(*layer)->draw();
}

void Engine::cleanupObjects()
{
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
		delete (*i);
	for(multiset<physSegment*>::iterator i = m_lScenery.begin(); i != m_lScenery.end(); i++)
		delete (*i);
	m_lScenery.clear();
	m_lObjects.clear();
}

void Engine::updateObjects(float32 dt)
{
	list<b2BodyDef*> lAddObj;
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
	{
		b2BodyDef* def = (*i)->update(dt);
		if(def != NULL)
			lAddObj.push_back(def);
	}
	for(list<b2BodyDef*>::iterator i = lAddObj.begin(); i != lAddObj.end(); i++)
	{
		objFromXML(*((string*)(*i)->userData), (*i)->position, (*i)->linearVelocity);
		delete (*i);	//Free up memory
	}
}

string Engine::getSaveLocation()	//TODO: Allow for user-specified save dir
{
	string s = ttvfs::GetAppDir(m_sAppName.c_str());
	s += "/";
	ttvfs::CreateDirRec(s.c_str());
	return s;
}

void Engine::updateSound()
{
	if(m_bSoundDied) return;
	
	//Update FMOD
	FMOD_System_Update(m_audioSystem);
	
	//Get rid of sounds that aren't currently playing
	for(multimap<string, FMOD_CHANNEL*>::iterator i = m_channels.begin(); i != m_channels.end();i++)
	{
		FMOD_BOOL bPlaying;
		if(FMOD_Channel_IsPlaying(i->second, &bPlaying) != FMOD_OK) continue;
		if(!bPlaying && i->first != "music")
		{
			m_channels.erase(i);
			continue;
		}
	}
}

void Engine::cleanupParticles()
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
		delete *i;
	m_particles.clear();
}

void Engine::drawParticles()
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
		(*i)->draw();
}

void Engine::updateParticles(float32 dt)
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
	{
		(*i)->update(dt);
		if((*i)->done())
		{
			delete *i;
			i = m_particles.erase(i);
			continue;
		}
	}
}







