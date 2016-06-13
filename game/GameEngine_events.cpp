#include "GameEngine.h"
#include <ctime>
#include <iomanip>
#include "Image.h"
#include "easylogging++.h"
#include <SDL_opengl.h>
#include "stb_image_write.h"
#include <SDL_thread.h>
#include "DebugUI.h"

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
				case SDL_SCANCODE_F5:
					Lua->call("clearClasses"); //Reload Lua classes
					reloadImages();
					loadScene(m_sLastScene);	//Reload current scene
					break;
					
				case SDL_SCANCODE_F6:
					Lua->call("loadStartingMap");	//Start from initial map
					break;

				case SDL_SCANCODE_ESCAPE:
					if(m_debugUI->visible)
						m_debugUI->visible = false;
					else
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
					m_debugUI->visible = !m_debugUI->visible;
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
		case SDL_JOYDEVICEADDED:
			LOG(INFO) << "Joystick " << (int)event.jdevice.which << " connected.";
			//TODO: Create new joystick, don't override old one
			m_joy = SDL_JoystickOpen(event.jdevice.which);

			if (m_joy)
			{
				LOG(TRACE) << "Opened Joystick " << event.jdevice.which;
				LOG(TRACE) << "Name: " << SDL_JoystickNameForIndex(event.jdevice.which);
				LOG(TRACE) << "Number of Axes: " << SDL_JoystickNumAxes(m_joy);
				LOG(TRACE) << "Number of Buttons: " << SDL_JoystickNumButtons(m_joy);
				LOG(TRACE) << "Number of Balls: " << SDL_JoystickNumBalls(m_joy);
				LOG(TRACE) << "Number of Hats: " << SDL_JoystickNumHats(m_joy);

				//On Linux, "xboxdrv" is the driver I had the most success with when it came to rumble (default driver said it rumbled, but didn't)
				m_rumble = NULL;
				if (SDL_JoystickIsHaptic(m_joy))
					m_rumble = SDL_HapticOpenFromJoystick(m_joy);
				if (m_rumble)
				{
					if (SDL_HapticRumbleInit(m_rumble) != 0)
					{
						LOG(WARNING) << "Unable to initialize joystick " << (int)event.jdevice.which << " as rumble.";
						SDL_HapticClose(m_rumble);
						m_rumble = NULL;
					}
					else
					{
						LOG(INFO) << "Initialized joystick " << (int)event.jdevice.which << " as rumble.";
					}
				}
				else
					LOG(INFO) << "Joystick " << (int)event.jdevice.which << " has no rumble support.";
			}
			else
				LOG(WARNING) << "Couldn't open Joystick " << (int)event.jdevice.which;
			break;
			
		//TODO: Test if this is working now
		case SDL_JOYDEVICEREMOVED:
			LOG(INFO) << "Joystick " << (int)event.jdevice.which << " disconnected.";
			break;
			
		case SDL_JOYBUTTONDOWN:
			LOG(TRACE) << "Joystick " << (int)event.jbutton.which << " pressed button " << (int)event.jbutton.button;
			if(event.jbutton.button == JOY_BUTTON_BACK)
				quit();
				
			break;
			
		case SDL_JOYBUTTONUP:
			LOG(TRACE) << "Joystick " << (int)event.jbutton.which << " released button " << (int)event.jbutton.button;
			break;
			
		case SDL_JOYAXISMOTION:
			if(abs(event.jaxis.value) > JOY_AXIS_TRIP)
				LOG(TRACE) << "Joystick " << (int)event.jaxis.which << " moved axis " << (int)event.jaxis.axis << " to " << event.jaxis.value;
			break;
			
		case SDL_JOYHATMOTION:
			LOG(TRACE) << "Joystick " << (int)event.jhat.which << " moved hat " << (int)event.jhat.hat << " to " << (int)event.jhat.value;
			break;
	}

	//----------------------------------------------------------------------------------------------------
	// Ignore non-essential input when the debugUI has focus
	if(m_debugUI->hasFocus())
		return;


	switch(event.type)
	{
		//Key pressed
		case SDL_KEYDOWN:
			switch(event.key.keysym.scancode)
			{
				case SDL_SCANCODE_V:
					toggleDebugDraw();
					break;
					
				case SDL_SCANCODE_L:
					toggleObjDebugDraw();
					break;
					
#ifdef _DEBUG
				case SDL_SCANCODE_P:
					playPausePhysics();
					break;
					
				case SDL_SCANCODE_O:
					pausePhysics();
					stepPhysics();
					break;
#endif
				
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
			LOG(TRACE) << "Mouse button " << (int)event.button.button << " pressed.";
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

//TODO: Controller haptic shouldn't be game specific
//TODO: This is also not an event
void GameEngine::rumbleController(float strength, float sec, int priority)
{
	static float fLastRumble = 0.0f;
	static int prevPriority = 0;
	
	//Too low priority to rumble here; another higher-priority rumble is currently going on
	if(getSeconds() < fLastRumble && priority < prevPriority)
		return;
	
	fLastRumble = getSeconds() + sec;
	prevPriority = priority;
	strength = max(strength, 0.0f);
	strength = min(strength, 1.0f);
	if(m_rumble != NULL)
		SDL_HapticRumblePlay(m_rumble, strength, (Uint32)(sec*1000));
}