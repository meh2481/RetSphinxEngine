#pragma once
#include "SDL.h"
#include "Rect.h"
#include "Action.h"
#include <string>

class SteelSeriesHaptic;
class SteelSeriesClient;

class InputDevice
{
private:
	SDL_Haptic* m_haptic;
	SDL_GameController* m_controller;
	bool rumbleLRSupported;
	bool mouseKb;
	int m_deviceIndex;
	int curEffect;
	std::string joystickName;
	std::string controllerName;
	SteelSeriesHaptic* ssHaptic;

	void rumbleControllerBasic(float strength, uint32_t duration, float curTime);
	SDL_Haptic* initHapticDevice(SDL_Haptic* newRumble);

public:
	InputDevice(SteelSeriesClient* ssc);	//Init from mouse and kb
	InputDevice(int deviceIndex);	//Init from controller

	~InputDevice();

	Vec2 getMovement();
	bool getDigitalAction(Action act);
	float getAnalogAction(Action act);

	int getAxis(int axis);	//DEPRECATED
	bool getButton(int buttonIndex);	//DEPRECATED

	void rumbleLR(uint32_t duration, uint16_t largeMotor, uint16_t smallMotor, float curTime);

	bool hasHaptic();
	int getDeviceIndex() { return m_deviceIndex; }
	std::string getJoystickName() { return joystickName; }
	std::string getControllerName() { return controllerName; }
};