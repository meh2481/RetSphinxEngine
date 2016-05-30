#include "GameEngine.h"
#include <ctime>
#include <iomanip>
#include "Image.h"
#include "easylogging++.h"

void GameEngine::handleEvent(SDL_Event event)
{
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
					
				case SDL_SCANCODE_V:
					toggleDebugDraw();
					break;
					
				case SDL_SCANCODE_L:
					toggleObjDebugDraw();
					break;
					
				case SDL_SCANCODE_ESCAPE:
					quit();
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
				//TODO Fix to work properly now
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
					
					/*BYTE* pixels = new BYTE[3 * width * height];
					glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

					//Convert to FreeImage format & save to file
					FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
					FreeImage_Save(FIF_PNG, image, ssfile.str().c_str(), PNG_Z_BEST_SPEED);

					//Free resources
					FreeImage_Unload(image);
					delete [] pixels;*/
					break;
				}
				
				case SDL_SCANCODE_RETURN:	//Alt-Enter toggles fullscreen
					if(keyDown(SDL_SCANCODE_ALT))
						setFullscreen(!isFullscreen());
					break;
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