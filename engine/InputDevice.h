#pragma once
#include "SDL.h"
#include "Action.h"
#include "Movement.h"
#include "Rect.h"
#include <string>

class SteelSeriesHaptic;
class SteelSeriesClient;
class ActionBind;
class MovementBind;

#define KB_MOUSE_DEVICE_INDEX -1

class InputDevice
{
private:
    SDL_Haptic* m_haptic;
    SDL_GameController* m_controller;
    bool rumbleLRSupported;
    int m_deviceIndex;
    int curEffect;    //Current effect index for haptic
    std::string joystickName;
    std::string controllerName;
    SteelSeriesHaptic* ssHaptic;

    void rumbleControllerBasic(float strength, uint32_t duration, float curTime);
    SDL_Haptic* initHapticDevice(SDL_Haptic* newRumble);

public:
    InputDevice(SteelSeriesClient* ssc);    //Init from mouse and kb
    InputDevice(int deviceIndex);    //Init from controller
    ~InputDevice();

    void rumbleLR(uint32_t duration, uint16_t largeMotor, uint16_t smallMotor, float curTime);

    int getAxis(int axis);
    bool getButton(int button);

    bool hasHaptic();
    int getDeviceIndex() { return m_deviceIndex; }
    std::string getJoystickName() { return joystickName; }
    std::string getControllerName() { return controllerName; }
};