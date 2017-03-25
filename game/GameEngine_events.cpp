#include "GameEngine.h"
#include <ctime>
#include <climits>
#include <iomanip>
#include "Image.h"
#include "easylogging++.h"
#include <SDL_opengl.h>
#include "stb_image_write.h"
#include <SDL_thread.h>
#include "DebugUI.h"
#include "ResourceLoader.h"
#include "ResourceCache.h"
#include "ParticleSystem.h"
#include "ParticleEditor.h"
using namespace std;

#define GUID_STR_SZ	256

typedef struct
{
	int w, h;
	unsigned char* pixels;
	string filename;
} printscreenData;

static int saveScreenshot(void *ptr)
{
	printscreenData* ps = (printscreenData*)ptr;
	unsigned char* line_tmp = new unsigned char[3 * ps->w];
	unsigned char* line_a = ps->pixels;
	unsigned char* line_b = ps->pixels + (3 * ps->w * (ps->h - 1));
	while(line_a < line_b)
	{
		memcpy(line_tmp, line_a, ps->w * 3);
		memcpy(line_a, line_b, ps->w * 3);
		memcpy(line_b, line_tmp, ps->w * 3);
		line_a += ps->w * 3;
		line_b -= ps->w * 3;
	}

	LOG(INFO) << "Saving screenshot " << ps->filename;
	if(!stbi_write_png(ps->filename.c_str(), ps->w, ps->h, 3, ps->pixels, ps->w * 3))
		LOG(WARNING) << "stbi_write_png error while saving screenshot";
	delete[] ps->pixels;
	delete[] line_tmp;
	delete ps;
	return 0;
}


void GameEngine::handleEvent(SDL_Event event)
{
	// Input that happens no matter what
	switch(event.type)
	{
		//Key pressed
		case SDL_KEYDOWN:
			switch(event.key.keysym.scancode)
			{
#ifdef _DEBUG
				case SDL_SCANCODE_F5:
					Lua->call("clearClasses"); //Reload Lua classes
					getResourceLoader()->clearCache();
					Lua->call("loadLua", m_sLastScene.c_str());	//Restart Lua with our last map
					break;
					
				case SDL_SCANCODE_F6:
					Lua->call("clearClasses"); //Reload Lua classes
					getResourceLoader()->clearCache();
					Lua->call("loadLua");	//Restart Lua
					break;
#endif //_DEBUG

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
					ssfile << getSaveLocation() << "Screenshot " 
					       << now->tm_mon + 1 << '-' << now->tm_mday << '-' << now->tm_year + 1900 << ' '
					       << now->tm_hour << '.' << setw(2) << setfill('0') << now->tm_min << '.' << setw(2) << setfill('0') << now->tm_sec << ',' << SDL_GetTicks() << ".png";
				
					uint16_t w = getWidth();
					uint16_t h = getHeight();
					
					//Copied whatever this guy did: https://github.com/ocornut/imgui/wiki/screenshot_tool
					unsigned char* pixels = new unsigned char[3 * w * h];
					glPixelStorei(GL_PACK_ALIGNMENT, 1);
					glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

					//Spawn a new thread to handle saving the screenshot, since it's slow
					printscreenData* dat = new printscreenData;
					dat->w = w;
					dat->h = h;
					dat->pixels = pixels;
					dat->filename = ssfile.str();
					if(!SDL_CreateThread(saveScreenshot, "saveScreenshot", (void *)dat))
						LOG(WARNING) << "Could not create thread to save screenshot.";
					
					break;
				}

				case SDL_SCANCODE_RETURN:	//Alt-Enter toggles fullscreen
					if(keyDown(SDL_SCANCODE_ALT))
						setFullscreen(!isFullscreen());
					break;

				case SDL_SCANCODE_GRAVE: // Use the traditional quake key for debug console stuff
					if(m_debugUI->visible)
						playPhysics();
					m_debugUI->visible = !m_debugUI->visible;
					break;
			}

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
		case SDL_CONTROLLERDEVICEADDED:
		{
			cout << "Controller " << (int)event.cdevice.which << " connected." << endl;
			//Create new controller, don't override old one
			addController(event.cdevice.which);
			break;
		}

		case SDL_JOYDEVICEADDED:
		{
			SDL_Joystick* joy = SDL_JoystickOpen(event.jdevice.which);
			if(joy)
			{
				char guid[GUID_STR_SZ];
				SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, GUID_STR_SZ);

				cout << "Joystick " << SDL_JoystickName(joy) << " attached." << endl;
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

		case SDL_CONTROLLERDEVICEREMOVED:
			LOG(INFO) << "Controller " << (int)event.cdevice.which << " disconnected.";
			cout << "Controller " << (int)event.cdevice.which << " disconnected." << endl;
			removeController(event.cdevice.which);
			break;
			
		case SDL_CONTROLLERBUTTONDOWN:
			LOG(TRACE) << "Controller " << (int)event.cbutton.which << " pressed button " << (int)event.cbutton.button;
			activateController(event.cbutton.which);
			switch(event.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_BACK:	//TODO Not hardcoded
					quit();
					break;
			}
			break;
			
		case SDL_CONTROLLERBUTTONUP:
			LOG(TRACE) << "Controller " << (int)event.cbutton.which << " released button " << (int)event.cbutton.button;
			break;
			
		case SDL_CONTROLLERAXISMOTION:
			if(abs(event.caxis.value) > JOY_AXIS_TRIP)
				LOG(TRACE) << "Controller " << (int)event.caxis.which << " moved axis " << (int)event.caxis.axis << " to " << event.caxis.value;
			break;
			
		//case SDL_JOYHATMOTION:
		//	LOG(TRACE) << "Joystick " << (int)event.jhat.which << " moved hat " << (int)event.jhat.hat << " to " << (int)event.jhat.value;
		//	break;
	}


	switch(event.type)
	{
		//Key pressed
		case SDL_KEYDOWN:
#ifdef _DEBUG
			if(m_debugUI->visible)
				break;
			switch(event.key.keysym.scancode)
			{
				case SDL_SCANCODE_V:
					toggleDebugDraw();
					break;
					
				case SDL_SCANCODE_L:
					toggleObjDebugDraw();
					break;
					
				case SDL_SCANCODE_P:
					playPausePhysics();
					break;
					
				case SDL_SCANCODE_O:
					pausePhysics();
					stepPhysics();
					break;
			}
#endif
			break;
		
		//Key released
		case SDL_KEYUP:
			//switch(event.key.keysym.scancode)
			//{
			//}
			break;
		
		case SDL_MOUSEBUTTONDOWN:
#ifdef _DEBUG
			if(event.button.button == SDL_BUTTON_RIGHT && m_debugUI->particleEditor->open && m_debugUI->visible)
				m_debugUI->particleEditor->particles->emitFrom.centerOn(worldPosFromCursor(Vec2(event.button.x, event.button.y), Vec3(0.0f, 0.0f, m_fDefCameraZ)));
#endif
			LOG(TRACE) << "Mouse button " << (int)event.button.button << " pressed.";
			break;
			
		case SDL_MOUSEWHEEL:
			if(!m_debugUI->visible)
			{
				if(event.wheel.y > 0)
					cameraPos.z += 1.5;// min(cameraPos.z + 1.5, -5.0);
				else
					cameraPos.z -= 1.5;// max(cameraPos.z - 1.5, -3000.0);
			}
			break;

		case SDL_MOUSEBUTTONUP:
			LOG(TRACE) << "Mouse button " << (int)event.button.button << " released.";
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
#ifdef _DEBUG
			if(getCursorDown(SDL_BUTTON_RIGHT) && m_debugUI->particleEditor->open && m_debugUI->visible)
				m_debugUI->particleEditor->particles->emitFrom.centerOn(worldPosFromCursor(Vec2(event.motion.x, event.motion.y), Vec3(0.0f, 0.0f, m_fDefCameraZ)));
#endif
			//LOG(TRACE) << "Mouse moved to " << event.motion.x << ", " << event.motion.y;
			break;
	}
}

void GameEngine::handleKeys()
{
#ifdef _DEBUG
	setTimeScale(1.0f);
	if (keyDown(SDL_SCANCODE_G))
	{
		setTimeScale(0.25f);
		if (keyDown(SDL_SCANCODE_CTRL))
			setTimeScale(0.0625f);
	}

	if (keyDown(SDL_SCANCODE_H))
	{
		setTimeScale(2.0f);
		if (keyDown(SDL_SCANCODE_CTRL))
			setTimeScale(3.0f);
	}
#endif
}


