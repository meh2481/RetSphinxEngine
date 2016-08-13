#include "Engine.h"
#include "opengl-api.h"
#include "easylogging++.h"

static PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = NULL;


static void __stdcall debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{

	switch(severity)
	{
	case GL_DEBUG_SEVERITY_MEDIUM:
	case GL_DEBUG_SEVERITY_LOW:
		printf("GL[%u]: %s\n", severity, message);
		LOG(WARNING) << "GL[" << severity << "]: " << message;
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		printf("GL[%u]: %s\n", severity, message);
		LOG(ERROR) << "GL[" << severity << "]: " << message;
		break;
	default:
		assert(false);
	}
}


void Engine::setup_sdl()
{

	if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		LOG(ERROR) << "SDL_InitSubSystem Error: " << SDL_GetError();
		exit(1);
	}
	
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0)
		LOG(ERROR) << "Unable to init SDL2 gamepad subsystem.";

	LOG(INFO) << "Loading OpenGL...";

	if (SDL_GL_LoadLibrary(NULL) == -1)
	{
		LOG(ERROR) << "SDL_GL_LoadLibrary Error: " << SDL_GetError();
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
		LOG(ERROR) << "Couldn't set video mode: " << SDL_GetError();
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	//TODO: Figure out vsync and add way to enable/disable
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//TODO: Switch to core instead of compat once all drawing uses VBOs/shaders
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
#ifdef _DEBUG
	//TODO: Add back forward compatibility flag once all drawing uses VBOs/shaders
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, /*SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG |*/ SDL_GL_CONTEXT_DEBUG_FLAG);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1); //Share objects between OpenGL contexts
	SDL_GL_CreateContext(m_Window);
	if(SDL_GL_SetSwapInterval(-1) == -1) //Apparently Vsync or something
		SDL_GL_SetSwapInterval(1);

	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, 0, &mode);
	if(!mode.refresh_rate)	//If 0, display doesn't care, so default to 60
		mode.refresh_rate = 60;
	setFramerate((float)mode.refresh_rate);
	
	int numDisplays = SDL_GetNumVideoDisplays();
	LOG(INFO) << "Available displays: " << numDisplays;
	for(int display = 0; display < numDisplays; display++)
	{
		int num = SDL_GetNumDisplayModes(display);
		LOG(TRACE) << "Available modes for display " << display + 1 << ':';
		for(int i = 0; i < num; i++)
		{
			SDL_GetDisplayMode(display, i, &mode);
			LOG(TRACE) << "Mode: " << mode.w << "x" << mode.h << " " << mode.refresh_rate << "Hz";
		}
	}
	
	
	//Hide system cursor for SDL, so we can use our own
	SDL_ShowCursor(0);
	
	OpenGLAPI::LoadSymbols();	//Load our OpenGL symbols to use
	
	_loadicon();	//Load our window icon
	
	const char *ver = (const char*)glGetString(GL_VERSION);
	LOG(INFO) << "GL version: " << ver;
    const char *ven = (const char*)glGetString(GL_VENDOR);
	LOG(INFO) << "GL vendor: " << ven;

	glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)SDL_GL_GetProcAddress("glDebugMessageCallback");

#ifdef _DEBUG
	if(glDebugMessageCallback)
	{
		puts("Using GL debug callbacks");
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(debugCallback, NULL);
	}
	else
		puts("glDebugMessageCallback() not supported");
#endif
}

//Set up OpenGL
void Engine::setup_opengl()
{
	//float LightAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
	// Diffuse Light Values
	//float LightDiffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	// Light Position 
	//float LightPosition[] = { 8.0f, 8.0f, -0.15f, 1.0f };

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
	
    glm::mat4 persp = glm::tweakedInfinitePerspective(glm::radians(45.0f), (float)m_iWidth/(float)m_iHeight, 0.1f);
    glLoadMatrixf(glm::value_ptr(persp));

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