#include "GameEngine.h"
#include <ctime>
#include <iomanip>

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
			//cout << "Mouse moved to " << event.motion.x << ", " << event.motion.y << endl;
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
			
		//TODO: Test if this is working now
		case SDL_JOYDEVICEREMOVED:
			errlog << "Joystick " << (int)event.jdevice.which << " disconnected." << endl;
			break;
			
		case SDL_JOYBUTTONDOWN:
#ifdef DEBUG_INPUT
			cout << "Joystick " << (int)event.jbutton.which << " pressed button " << (int)event.jbutton.button << endl;
#endif
			if(event.jbutton.button == JOY_BUTTON_BACK)
				quit();
				
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

void GameEngine::handleKeys()
{
}

//TODO: Controller haptic shouldn't be game specific? Or is this too closely tied to controller input in general?
void GameEngine::rumbleController(float32 strength, float32 sec, int priority)
{
	static float32 fLastRumble = 0.0f;
	static int prevPriority = 0;
	
	//Too low priority to rumble here; another higher-priority rumble is currently going on
	if(getSeconds() < fLastRumble && priority < prevPriority)
		return;
	
	fLastRumble = getSeconds() + sec;
	prevPriority = priority;
	strength = max(strength, 0.0f);
	strength = min(strength, 1.0f);
	if(m_rumble != NULL)
		SDL_HapticRumblePlay(m_rumble, strength, sec*1000);
}