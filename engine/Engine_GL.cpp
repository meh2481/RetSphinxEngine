#include "Engine.h"
#include "opengl-api.h"
#include "OpenGLShader.h"
#include "easylogging++.h"

static PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = NULL;
#define GAME_CONTROLLER_DB_FILE "gamecontrollerdb.txt"

#ifdef _WIN32
static void __stdcall debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
#else
static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
#endif
{
	switch(severity)
	{
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                LOG(INFO) << "GL[" << severity << "]: " << message;
                break;
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
	assert(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0);
	assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) == 0);
	assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0) == 0);

	assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0);
#ifdef _DEBUG
	//TODO: Add back forward compatibility flag once all drawing uses VBOs/shaders
	assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, /*SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG |*/ SDL_GL_CONTEXT_DEBUG_FLAG) == 0);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

	assert(SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1) == 0); //Share objects between OpenGL contexts
        if(SDL_GL_CreateContext(m_Window) == NULL) 
        {
            LOG(ERROR) << "Error creating OpenGL context: " << SDL_GetError();
            exit(1);
        }
	SDL_GL_SetSwapInterval(1);	//Default to vsync on

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
	
	OpenGLAPI::LoadSymbols();	//Load our OpenGL symbols to use
	
	const char *ver = (const char*)glGetString(GL_VERSION);
        if(ver)
            LOG(INFO) << "GL version: " << ver;
        const char *ven = (const char*)glGetString(GL_VENDOR);
        if(ven)
            LOG(INFO) << "GL vendor: " << ven;

	glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)SDL_GL_GetProcAddress("glDebugMessageCallback");

#ifdef _DEBUG
	if(glDebugMessageCallback)
	{
		LOG(DEBUG) << "Using GL debug callbacks";
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(debugCallback, NULL);
	}
	else
		LOG(DEBUG) << "glDebugMessageCallback() not supported";
#endif

	LOG(INFO) << "Loading gamepad configurations from " << GAME_CONTROLLER_DB_FILE;
	if(SDL_GameControllerAddMappingsFromFile(GAME_CONTROLLER_DB_FILE) == -1)
		LOG(WARNING) << "Unable to open " << GAME_CONTROLLER_DB_FILE << ": " << SDL_GetError();

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
	
	//Enable image transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	setMSAA(m_iMSAA);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Set the rendering program
	glm::mat4 persp = glm::tweakedInfinitePerspective(glm::radians(45.0f), (float)m_iWidth / (float)m_iHeight, 0.1f);

	//TODO: Load from resources
	m_renderState.programId = OpenGLShader::loadShaders("res/shaders/default.vert", "res/shaders/default.frag");
	m_renderState.modelId = glGetUniformLocation(m_renderState.programId, "model");
	m_renderState.viewId = glGetUniformLocation(m_renderState.programId, "view");
	m_renderState.projectionId = glGetUniformLocation(m_renderState.programId, "projection");
	m_renderState.projection = persp;

#ifdef _DEBUG
	m_debugRenderState.programId = OpenGLShader::loadShaders("res/shaders/debugdraw.vert", "res/shaders/debugdraw.frag");
	m_debugRenderState.modelId = glGetUniformLocation(m_debugRenderState.programId, "model");
	m_debugRenderState.viewId = glGetUniformLocation(m_debugRenderState.programId, "view");
	m_debugRenderState.projectionId = glGetUniformLocation(m_debugRenderState.programId, "projection");
	m_debugDraw.uniformId = glGetUniformLocation(m_debugRenderState.programId, "col");
	m_debugRenderState.projection = persp;
#endif
}
