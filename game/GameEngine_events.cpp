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
					reloadImages();
					loadScene("res/3d/solarsystem.scene.xml");
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
						toggleFullscreen();
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
	//Keyboard movement
	if(keyDown(KEY_UP1))
		shipMoveVec.y += 1.0f;
	if(keyDown(KEY_DOWN1))
		shipMoveVec.y -= 1.0f;
	if(keyDown(KEY_LEFT1))
		shipMoveVec.x -= 1.0f;
	if(keyDown(KEY_RIGHT1))
		shipMoveVec.x += 1.0f;
		
	//Joystick movement
	Sint16 axisVal = SDL_JoystickGetAxis(m_joy, JOY_AXIS_HORIZ);	//Horizontal movement
	if(abs(axisVal) > JOY_MINMOVE_TRIP)
	{
		if(axisVal > 0)
			shipMoveVec.x = (float32)(axisVal - JOY_MINMOVE_TRIP)/(float32)(JOY_AXIS_MAX - JOY_MINMOVE_TRIP);
		else
			shipMoveVec.x = -(float32)(axisVal + JOY_MINMOVE_TRIP)/(float32)(JOY_AXIS_MIN + JOY_MINMOVE_TRIP);
	}
	
	axisVal = SDL_JoystickGetAxis(m_joy, JOY_AXIS_VERT);	//Vertical movement
	if(abs(axisVal) > JOY_MINMOVE_TRIP)
	{
		if(axisVal > 0)
			shipMoveVec.y = -(float32)(axisVal - JOY_MINMOVE_TRIP)/(float32)(JOY_AXIS_MAX - JOY_MINMOVE_TRIP);
		else
			shipMoveVec.y = (float32)(axisVal + JOY_MINMOVE_TRIP)/(float32)(JOY_AXIS_MIN + JOY_MINMOVE_TRIP);
	}
}

//TODO: This doesn't belong in C++. Rip it out and stick it in Lua.
void GameEngine::updateShip()
{
	return;	//TODO: In the process of moving to Lua....
	
	/*/Update ship velocity based on current input
	float max_ship_vel = MAX_SHIP_SPEED;
	float ship_accel = SHIP_ACCEL;
	if(keyDown(SDL_SCANCODE_SPACE) || SDL_JoystickGetButton(m_joy, JOY_BUTTON_B))	//Thrust button
	{
		max_ship_vel *= 2.25f;
		ship_accel *= 2.1f;
	}

	float dt = 1.0/getFramerate();	//TODO: this is retarded why didn't I just pass in the dt
	
	if(shipMoveVec.Length() > 1.0f)
		shipMoveVec.Normalize();
	
	if(ship != NULL)
	{
		b2Body* b = ship->getBody();
		if(b != NULL)
		{
			//Accelerate ship
			Point none(0,0);
			Point v = b->GetLinearVelocity();
			if(shipMoveVec.LengthSquared())
			{
				//Ship accelerating
				none.x += dt * ship_accel * shipMoveVec.x;
				none.y += dt * ship_accel * shipMoveVec.y;
			}
			none += v;
			if(none.Length() > max_ship_vel)
			{
				none.Normalize();
				none *= max_ship_vel;
			}
			if(!shipMoveVec.LengthSquared())
			{
				//Ship not accelerating
				float f = none.Length();
				none.Normalize();
				f *= SHIP_SLOW_FAC;
				none *= f;
			}
			b->SetLinearVelocity(none);
			v = none;
			
		
			Point p = b->GetPosition();
			if(v.x != 0.0f || v.y != 0.0f)	//Don't rotate if not moving (prevents snap on stop)
			{
				float32 r = getAngle(v);
				b->SetTransform(p, r);
			}
			//CameraPos.x = -p.x;
			//CameraPos.y = -p.y;
			
			if(shipTrail != NULL)
			{
				shipTrail->emitFrom.centerOn(p);
				shipTrail->emitFrom.offset(-v.x*1.0f/getFramerate()/4.0f, -v.y*1.0f/getFramerate()/4.0f);	//Trail behind the ship just a bit
				
				shipTrail->emissionVel = v;
				shipTrail->curRate = min((v.Length()*v.Length()*v.Length()) / (MAX_SHIP_SPEED*MAX_SHIP_SPEED*MAX_SHIP_SPEED), 2.0);	//Exponentially decay as ship slows
			}
		}
	}*/
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