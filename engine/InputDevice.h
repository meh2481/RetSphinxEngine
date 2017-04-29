#pragma once
#include "SDL.h"
#include "Action.h"
#include <string>

class SteelSeriesHaptic;
class SteelSeriesClient;
class ActionBind;

class InputDevice
{
private:
	SDL_Haptic* m_haptic;
	SDL_GameController* m_controller;
	bool rumbleLRSupported;
	int m_deviceIndex;
	int curEffect;
	std::string joystickName;
	std::string controllerName;
	SteelSeriesHaptic* ssHaptic;
	ActionBind* actions[NUM_ACTIONS];

	void rumbleControllerBasic(float strength, uint32_t duration, float curTime);
	SDL_Haptic* initHapticDevice(SDL_Haptic* newRumble);

public:
	InputDevice(SteelSeriesClient* ssc);	//Init from mouse and kb
	InputDevice(int deviceIndex);	//Init from controller
	~InputDevice();

	int getAxis(int axis);
	bool getButton(int buttonIndex);

	void rumbleLR(uint32_t duration, uint16_t largeMotor, uint16_t smallMotor, float curTime);

	bool hasHaptic();
	int getDeviceIndex() { return m_deviceIndex; }
	std::string getJoystickName() { return joystickName; }
	std::string getControllerName() { return controllerName; }

	//Action (input) handling
	bool getDigitalAction(Action a);	//Get a true/false for this action
	float getAnalogAction(Action a);	//Get a normalized 0..1 for this action (emulated if digital input)
};