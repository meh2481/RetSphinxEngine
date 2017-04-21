#pragma once
#include "SDL.h"
#include <string>

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
	SteelSeriesClient* ssClient;

	void rumbleControllerBasic(float strength, uint32_t duration, float curTime);
	void rumbleSS(uint32_t duration, float curTime);
	void bindTactileEvent(std::string eventId, int rumbleLen);
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
};