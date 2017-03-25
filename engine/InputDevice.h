#pragma once
#include "SDL.h"

class InputDevice
{
private:
	SDL_Haptic* m_haptic;
	SDL_GameController* m_controller;
	bool rumbleLRSupported;
	bool mouseKb;
	int m_deviceIndex;
	int curEffect;

	void rumbleControllerBasic(float strength, uint32_t duration, float curTime);

public:
	InputDevice();	//Init from mouse and kb
	InputDevice(int deviceIndex);	//Init from controller

	~InputDevice();

	int getAxis(int axis);

	bool getButton(int buttonIndex);

	void rumbleLR(uint32_t duration, uint16_t largeMotor, uint16_t smallMotor, float curTime);

	int getDeviceIndex() { return m_deviceIndex; }
};