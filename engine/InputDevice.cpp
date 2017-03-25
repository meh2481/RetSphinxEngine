#include "InputDevice.h"
#include "easylogging++.h"

#define GUID_STR_SZ	256

InputDevice::InputDevice()
{
	rumbleLRSupported = false;
	mouseKb = true;
	m_haptic = NULL;
	m_controller = NULL;
	m_deviceIndex = -1;
	curEffect = -1;
}

InputDevice::InputDevice(int deviceIndex)
{
	m_deviceIndex = deviceIndex;
	mouseKb = false;
	m_haptic = NULL;
	m_controller = NULL;
	rumbleLRSupported = false;
	curEffect = -1;

	SDL_GameController* newController = SDL_GameControllerOpen(deviceIndex);
	if(newController)
	{
		SDL_Joystick* joy = SDL_GameControllerGetJoystick(newController);
		if(!joy)
		{
			LOG(ERROR) << "Unable to get joystick from game controller: " << SDL_GetError();
			return;
		}
		char guid[GUID_STR_SZ];
		SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, GUID_STR_SZ);

		joystickName = SDL_JoystickName(joy);
		controllerName = SDL_GameControllerName(newController);

		LOG(INFO) << "Opened controller " << controllerName;
		LOG(INFO) << "Controller joystick has GUID " << guid;

		SDL_Haptic* newRumble = NULL;
		if(SDL_JoystickIsHaptic(joy))
			newRumble = SDL_HapticOpenFromJoystick(joy);
		if(newRumble)
		{
			LOG(INFO) << "Initialized controller " << (int)deviceIndex << "\'s haptic.";
			LOG(TRACE) << "Haptic effect storage size: " << SDL_HapticNumEffects(newRumble);
			LOG(TRACE) << "Haptic effect play channels: " << SDL_HapticNumEffectsPlaying(newRumble);
			LOG(TRACE) << "Haptic effect number of axes: " << SDL_HapticNumAxes(newRumble);

			unsigned int hapticQuery = SDL_HapticQuery(newRumble);
			//Haptic effects
			LOG(TRACE) << "Haptic functions available: ";
			//Multi-motor
			rumbleLRSupported = ((hapticQuery & SDL_HAPTIC_LEFTRIGHT) != 0);
			LOG(TRACE) << "SDL_HAPTIC_LEFTRIGHT: " << rumbleLRSupported;
			
			//Rumble wave types
			LOG(TRACE) << "SDL_HAPTIC_CONSTANT: " << ((hapticQuery & SDL_HAPTIC_CONSTANT) != 0);
			LOG(TRACE) << "SDL_HAPTIC_SINE: " << ((hapticQuery & SDL_HAPTIC_SINE) != 0);
			LOG(TRACE) << "SDL_HAPTIC_TRIANGLE: " << ((hapticQuery & SDL_HAPTIC_TRIANGLE) != 0);
			LOG(TRACE) << "SDL_HAPTIC_SAWTOOTHUP: " << ((hapticQuery & SDL_HAPTIC_SAWTOOTHUP) != 0);
			LOG(TRACE) << "SDL_HAPTIC_SAWTOOTHDOWN: " << ((hapticQuery & SDL_HAPTIC_SAWTOOTHDOWN) != 0);
			LOG(TRACE) << "SDL_HAPTIC_RAMP: " << ((hapticQuery & SDL_HAPTIC_RAMP) != 0);
			//Define your own wave shape
			LOG(TRACE) << "SDL_HAPTIC_CUSTOM: " << ((hapticQuery & SDL_HAPTIC_CUSTOM) != 0);
			//Complicated stuff that requires super-specific hardware
			LOG(TRACE) << "SDL_HAPTIC_SPRING: " << ((hapticQuery & SDL_HAPTIC_SPRING) != 0);
			LOG(TRACE) << "SDL_HAPTIC_DAMPER: " << ((hapticQuery & SDL_HAPTIC_DAMPER) != 0);
			LOG(TRACE) << "SDL_HAPTIC_INERTIA: " << ((hapticQuery & SDL_HAPTIC_INERTIA) != 0);
			LOG(TRACE) << "SDL_HAPTIC_FRICTION: " << ((hapticQuery & SDL_HAPTIC_FRICTION) != 0);
			//Features
			LOG(TRACE) << "SDL_HAPTIC_GAIN: " << ((hapticQuery & SDL_HAPTIC_GAIN) != 0);
			LOG(TRACE) << "SDL_HAPTIC_AUTOCENTER: " << ((hapticQuery & SDL_HAPTIC_AUTOCENTER) != 0);
			LOG(TRACE) << "SDL_HAPTIC_STATUS: " << ((hapticQuery & SDL_HAPTIC_STATUS) != 0);
			LOG(TRACE) << "SDL_HAPTIC_PAUSE: " << ((hapticQuery & SDL_HAPTIC_PAUSE) != 0);

			//if (SDL_HapticRumbleInit(m_rumble) != 0)
			//{
			//	LOG(WARNING) << "Unable to initialize controller " << (int)event.cdevice.which << " as rumble.";
			//	SDL_HapticClose(m_rumble);
			//	m_rumble = NULL;
			//}
			//else
			//{
			//	LOG(INFO) << "Initialized controller " << (int)event.cdevice.which << " as rumble.";
			//}

			if(!rumbleLRSupported)
			{
				int canRumble = SDL_HapticRumbleInit(newRumble);	//Fallback to basic SDL rumble support (No LR boo hoo)
				if(canRumble != 0)
					newRumble = NULL;	//Cannot rumble with this device
			}
			m_haptic = newRumble;
		}
		else
			LOG(INFO) << "Controller " << (int)deviceIndex << " has no rumble support.";

		m_controller = newController;
	}
	else
		LOG(WARNING) << "Couldn't open controller " << (int)deviceIndex;
}

InputDevice::~InputDevice()
{
	if(!rumbleLRSupported && m_haptic != NULL)
		SDL_HapticRumbleStop(m_haptic);
	else if(m_haptic != NULL)
		SDL_HapticClose(m_haptic);
	if(m_controller != NULL)
		SDL_GameControllerClose(m_controller);
}

int InputDevice::getAxis(int axis)
{
	if(!mouseKb)
	{
		if(m_controller == NULL) return false;
		return SDL_GameControllerGetAxis(m_controller, (SDL_GameControllerAxis)axis);
	}
	return 0;	//TODO Return kb state depending on axis
}

bool InputDevice::getButton(int buttonIndex)
{
	if(!mouseKb)
	{
		if(m_controller == NULL) return false;
		return(SDL_GameControllerGetButton(m_controller, (SDL_GameControllerButton)buttonIndex) > 0);
	}
	return false;	//TODO Return button state if kb
}

void InputDevice::rumbleControllerBasic(float strength, uint32_t duration, float curTime)
{
	static float fLastRumble = 0.0f;

	//Last rumble still going
	if(curTime < fLastRumble)
		return;
	
	fLastRumble = curTime + (float)duration / 1000.0;

	strength = max(strength, 0.0f);
	strength = min(strength, 1.0f);
	if(m_haptic != NULL)
		SDL_HapticRumblePlay(m_haptic, strength, duration);
}

void InputDevice::rumbleLR(uint32_t duration, uint16_t largeMotor, uint16_t smallMotor, float curTime)
{
	if(m_haptic == NULL) return;	//Have to have an active rumble first

	//If we don't support LR rumble, just do a basic one
	if(!rumbleLRSupported)
	{
		float strength = largeMotor + smallMotor;
		strength /= (float)USHRT_MAX * 2.0f;
		rumbleControllerBasic(strength, duration, curTime);
		return;
	}

	static float fLastRumble = 0.0f;
	static int prevPriority = 0;
	float sec = (float)duration / 1000.0;

	//Last rumble is still going or 0-msec duration
	if(curTime < fLastRumble || duration < 1)
		return;

	fLastRumble = curTime + sec;

	LOG(TRACE) << "Running LR effect: " << duration << ", " << sec << ", " << largeMotor << ", " << smallMotor;
	if(curEffect >= 0)
	{
		SDL_HapticDestroyEffect(m_haptic, curEffect);
	}

	SDL_HapticLeftRight lrHaptic;
	lrHaptic.large_magnitude = largeMotor;
	lrHaptic.small_magnitude = smallMotor;
	lrHaptic.length = duration;

	SDL_HapticEffect effect;
	effect.type = SDL_HAPTIC_LEFTRIGHT;
	effect.leftright = lrHaptic;

	curEffect = SDL_HapticNewEffect(m_haptic, &effect);
	if(curEffect < 0)
	{
		//Shouldn't ever happen, since we check for LR support on device init
		LOG(WARNING) << "Unable to create LR effect: " << SDL_GetError();
		return;
	}

	if(SDL_HapticRunEffect(m_haptic, curEffect, 1) < 0)
	{
		LOG(WARNING) << "Unable to run LR effect: " << SDL_GetError();
	}
}
