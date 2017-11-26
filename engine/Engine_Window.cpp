#include "Engine.h"
#include "ResourceLoader.h"
#include "easylogging++.h"
#include "opengl-api.h"

void Engine::changeScreenResolution(int w, int h)
{
	LOG(INFO) << "Changing screen resolution to " << w << ", " << h;
	
	m_iWidth = w;
	m_iHeight = h;

	if(m_bFullscreen)
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	SDL_SetWindowSize(m_Window, m_iWidth, m_iHeight);
	
	//Set OpenGL back up
	setup_opengl();
}

void Engine::setFullscreen(bool bFullscreen)
{
	if(m_bFullscreen == bFullscreen) 
		return;
	m_bFullscreen = !m_bFullscreen;
	//TODO: Restore previous window size
	SDL_SetWindowFullscreen(m_Window, (m_bFullscreen) ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

bool Engine::isMaximized()
{
	return (SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MAXIMIZED);
}

Vec2 Engine::getWindowPos()
{
	Vec2 p;
	int x, y;
	SDL_GetWindowPosition(m_Window, &x, &y);
	p.x = (float)x;
	p.y = (float)y;
	return p;
}

//If we used to be fullscreen, and now we're not, we don't want to be in the upper-left corner.
void Engine::setWindowPos(Vec2 pos)
{
	if(pos.x > 0 && pos.y > 0)
		SDL_SetWindowPosition(m_Window, (int)pos.x, (int)pos.y);
	else
		SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);	//Center if we're upper-left corner or above or some such.
}

void Engine::maximizeWindow()
{
	SDL_MaximizeWindow(m_Window);
}

