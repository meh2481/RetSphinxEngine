#include "Engine.h"

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
	
	const char *ver = (const char*)glGetString(GL_VERSION);
    errlog << "GL version: " << ver << endl;
    const char *ven = (const char*)glGetString(GL_VENDOR);
    errlog << "GL vendor: " << ven << endl;
}

//Set up OpenGL
void Engine::setup_opengl()
{
	//GLfloat LightAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
	// Diffuse Light Values
	//GLfloat LightDiffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	// Light Position 
	//GLfloat LightPosition[] = { 8.0f, 8.0f, -0.15f, 1.0f };

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
	//glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);

    //Set up the ambient light
    //glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);

    //Set up the diffuse light
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);

    //Position the light
    //glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

    //Enable light one
    //glEnable(GL_LIGHT1 ); 
	
	setMSAA(m_iMSAA);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}