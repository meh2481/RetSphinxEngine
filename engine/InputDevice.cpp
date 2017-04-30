#include "InputDevice.h"
#include "easylogging++.h"
#include "SteelSeriesClient.h"
#include "SteelSeriesHaptic.h"
#include "StringUtils.h"
#include "NoAction.h"
#include "KeyboardAction.h"
#include "JoyButtonAction.h"
#include "AxisAction.h"
#include "MouseButtonAction.h"
#include "MovementBind.h"
#include "DpadMovement.h"
#include "JoystickMovement.h"
#include "KeyboardMovement.h"
#include "NoMovement.h"

#define JOY_AXIS_TRIP 20000
#define GUID_STR_SZ	256
#define MOUSE_JOYSTICK_NAME "Mouse"
#define MOUSE_CONTROLLER_NAME "Keyboard"

InputDevice::InputDevice(SteelSeriesClient* ssc)
{
	rumbleLRSupported = false;
	m_controller = NULL;
	m_deviceIndex = 0;
	curEffect = -1;
	joystickName = MOUSE_JOYSTICK_NAME;
	controllerName = MOUSE_CONTROLLER_NAME;
	if(SDL_MouseIsHaptic() == SDL_TRUE)
	{
		m_haptic = initHapticDevice(SDL_HapticOpenFromMouse());
		if(m_haptic != NULL)
			LOG(INFO) << "Initialized haptic from mouse";
	}
	else
	{
		m_haptic = NULL;
		LOG(INFO) << "Mouse has no haptic";
	}
	if(ssc != NULL && ssc->isValid())
	{
		ssHaptic = new SteelSeriesHaptic(ssc);
		ssHaptic->init();
	}
	else
		ssHaptic = NULL;

	bindMouseKbActions();
}

InputDevice::InputDevice(int deviceIndex)
{
	ssHaptic = NULL;
	m_haptic = NULL;
	rumbleLRSupported = false;
	curEffect = -1;

	m_controller = SDL_GameControllerOpen(deviceIndex);
	if(m_controller)
	{
		SDL_Joystick* joy = SDL_GameControllerGetJoystick(m_controller);
		if(!joy)
		{
			LOG(ERROR) << "Unable to get joystick from game controller: " << SDL_GetError();
		}
		else
		{

			//NOT THE SAME THING AS THE INDEX PASSED INTO THIS FUNCTION! Thanks, SDL, for adding more stupidity to the world of software engineering.
			m_deviceIndex = SDL_JoystickInstanceID(joy);

			char guid[GUID_STR_SZ];
			SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, GUID_STR_SZ);

			joystickName = SDL_JoystickName(joy);
			controllerName = SDL_GameControllerName(m_controller);

			LOG(INFO) << "Opened controller " << controllerName;
			LOG(INFO) << "Controller joystick has GUID " << guid;

			SDL_Haptic* newRumble = NULL;
			if(SDL_JoystickIsHaptic(joy))
				newRumble = SDL_HapticOpenFromJoystick(joy);
			if(newRumble)
			{
				LOG(INFO) << "Initialized controller " << (int)deviceIndex << "\'s haptic.";
				m_haptic = initHapticDevice(newRumble);
			}
			else
				LOG(INFO) << "Controller " << (int)deviceIndex << " has no rumble support.";
		}
	}
	else
		LOG(WARNING) << "Couldn't open controller " << (int)deviceIndex;

	bindControllerActions();
}


SDL_Haptic* InputDevice::initHapticDevice(SDL_Haptic* newRumble)
{
	if(newRumble == NULL)
		return NULL;

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

	if(!rumbleLRSupported)
	{
		int canRumble = SDL_HapticRumbleInit(newRumble);	//Fallback to basic SDL rumble support (No LR boo hoo)
		if(canRumble != 0)
			newRumble = NULL;	//Cannot rumble with this device
	}
	return newRumble;
}

void InputDevice::bindMouseKbActions()
{
	//TODO: Example; can be expanded upon
	actions[JUMP] = new NoAction();
	actions[RUN] = new NoAction();
	actions[SHIP_THRUST] = new KeyboardAction(SDL_SCANCODE_SPACE);
	actions[EXAMINE] = new KeyboardAction(SDL_SCANCODE_W);
	actions[ATTACK] = new MouseButtonAction(SDL_BUTTON_LEFT);

	movements[MOVE] = new KeyboardMovement(SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S);
	movements[AIM] = new KeyboardMovement(SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN);
	movements[PAN] = new NoMovement();
}

void InputDevice::bindControllerActions()
{
	if(!m_controller)
	{
		for(int i = 0; i < NUM_ACTIONS; i++)
			actions[i] = new NoAction();
		return;
	}

	//TODO: Example; can be expanded upon
	actions[JUMP] = new NoAction();
	actions[RUN] = new NoAction();
	actions[SHIP_THRUST] = new AxisAction(m_controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT, JOY_AXIS_TRIP/10);
	actions[EXAMINE] = new JoyButtonAction(m_controller, SDL_CONTROLLER_BUTTON_A);
	actions[ATTACK] = new JoyButtonAction(m_controller, SDL_CONTROLLER_BUTTON_A);

	movements[MOVE] = new JoystickMovement(m_controller, SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY, JOY_AXIS_TRIP);
	movements[AIM] = new JoystickMovement(m_controller, SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY, JOY_AXIS_TRIP);
	movements[PAN] = new DpadMovement(m_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
}

InputDevice::~InputDevice()
{
	for(int i = 0; i < NUM_ACTIONS; i++)
		delete actions[i];
	for(int i = 0; i < NUM_MOVEMENTS; i++)
		delete movements[i];
	if(!rumbleLRSupported && m_haptic != NULL)
		SDL_HapticRumbleStop(m_haptic);
	else if(m_haptic != NULL)
		SDL_HapticClose(m_haptic);
	if(m_controller != NULL)
		SDL_GameControllerClose(m_controller);
	if(ssHaptic != NULL)
		delete ssHaptic;
}

int InputDevice::getAxis(int axis)//DEPRECATED
{
	if(m_controller == NULL)
		return 0;
	return SDL_GameControllerGetAxis(m_controller, (SDL_GameControllerAxis)axis);
}

bool InputDevice::getButton(int buttonIndex)//DEPRECATED
{
	if(m_controller == NULL) 
		return false;
	return(SDL_GameControllerGetButton(m_controller, (SDL_GameControllerButton)buttonIndex) > 0);
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
	float strength = largeMotor + smallMotor;
	strength /= (float)USHRT_MAX * 2.0f;
	if(m_haptic == NULL)
	{
		if(ssHaptic != NULL && ssHaptic->isValid())
			ssHaptic->rumble(strength, duration, curTime);
		return;	//Have to have an active rumble first
	}

	//If we don't support LR rumble, just do a basic one
	if(!rumbleLRSupported)
	{
		rumbleControllerBasic(strength, duration, curTime);
		return;
	}

	static float fLastRumble = 0.0f;
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

bool InputDevice::hasHaptic()
{
	return m_haptic != NULL || (ssHaptic != NULL && ssHaptic->isValid());
}

bool InputDevice::getDigitalAction(Action a)
{
	return actions[a]->getDigitalAction();
}

float InputDevice::getAnalogAction(Action a)
{
	return actions[a]->getAnalogAction();
}

Vec2 InputDevice::getMovement(Movement m)
{
	return movements[m]->getMovement();
}
