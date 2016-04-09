#include "Engine.h"

void Engine::changeScreenResolution(float32 w, float32 h)
{
	errlog << "Changing screen resolution to " << w << ", " << h << endl;
	int vsync = SDL_GL_GetSwapInterval();
//In Windoze, we copy the graphics memory to our new context, so we don't have to reload all of our images and stuff every time the resolution changes
//TODO: Look into SDL_GL_SHARE_WITH_CURRENT_CONTEXT for newer versions of Windows instead
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
	reload3DObjects();
#endif
#endif
}

void Engine::toggleFullscreen()
{
	m_bFullscreen = !m_bFullscreen;
	if(m_bFullscreen)
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);	//TODO: Does this work?
	else
		SDL_SetWindowFullscreen(m_Window, 0);
}

void Engine::setFullscreen(bool bFullscreen)
{
	if(m_bFullscreen == bFullscreen) 
		return;
	toggleFullscreen();	//TODO: WHY DO WE HAVE TWO FUNCTIONS FOR THIS?
}

bool Engine::isMaximized()
{
#ifdef _WIN32	//Apparently SDL_GetWindowFlags() is completely broken in SDL2 for determining maximization. See: https://bugzilla.libsdl.org/show_bug.cgi?id=2282
				//TODO: This bug has been marked as fixed in SDL 2.0.4, test and see if it is!
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
	return (SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MAXIMIZED);	//TODO: Test in Mac/Linux
#endif
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

//TODO: If we used to be fullscreen, and now we're not, we don't want to be in the upper-left corner. We probably don't want to do this altogether.
void Engine::setWindowPos(Point pos)
{
	SDL_SetWindowPosition(m_Window, pos.x, pos.y);
}

void Engine::maximizeWindow()
{
	SDL_MaximizeWindow(m_Window);
}

