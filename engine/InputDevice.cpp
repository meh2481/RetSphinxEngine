#include "InputDevice.h"
#include "Logger.h"
#include "SteelSeriesClient.h"
#include "SteelSeriesHaptic.h"
#include "StringUtils.h"
#include <algorithm>

#define GUID_STR_SZ    256
#define MOUSE_JOYSTICK_NAME "Mouse"
#define MOUSE_CONTROLLER_NAME "Keyboard"

InputDevice::InputDevice(SteelSeriesClient* ssc)
{
    rumbleLRSupported = false;
    m_controller = NULL;
    m_deviceIndex = KB_MOUSE_DEVICE_INDEX;
    curEffect = -1;
    joystickName = MOUSE_JOYSTICK_NAME;
    controllerName = MOUSE_CONTROLLER_NAME;
    if(SDL_MouseIsHaptic() == SDL_TRUE)
    {
        m_haptic = initHapticDevice(SDL_HapticOpenFromMouse());
        if(m_haptic != NULL)
            LOG_info("Initialized haptic from mouse");
    }
    else
    {
        m_haptic = NULL;
        LOG_info("Mouse has no haptic");
    }
    if(ssc != NULL && ssc->isValid())
    {
        ssHaptic = new SteelSeriesHaptic(ssc);
        ssHaptic->init();
    }
    else
        ssHaptic = NULL;
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
            LOG_err("Unable to get joystick from game controller: %s", SDL_GetError());
        }
        else
        {

            //NOT THE SAME THING AS THE INDEX PASSED INTO THIS FUNCTION! Thanks, SDL, for adding more stupidity to the world of software engineering.
            m_deviceIndex = SDL_JoystickInstanceID(joy);

            char guid[GUID_STR_SZ];
            SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, GUID_STR_SZ);

            joystickName = SDL_JoystickName(joy);
            controllerName = SDL_GameControllerName(m_controller);

            LOG_info("Opened controller %s", controllerName.c_str());
            LOG_info("Controller joystick has GUID %s", guid);

            SDL_Haptic* newRumble = NULL;
            if(SDL_JoystickIsHaptic(joy))
                newRumble = SDL_HapticOpenFromJoystick(joy);
            if(newRumble)
            {
                LOG_info("Initialized controller %d\'s haptic.", (int)deviceIndex);
                m_haptic = initHapticDevice(newRumble);
            }
            else
                LOG_info("Controller %d has no rumble support.", (int)deviceIndex);
        }
    }
    else
        LOG_warn("Couldn't open controller %d", (int)deviceIndex);
}


SDL_Haptic* InputDevice::initHapticDevice(SDL_Haptic* newRumble)
{
    if(newRumble == NULL)
        return NULL;

    LOG_info("Haptic effect storage size: %d", SDL_HapticNumEffects(newRumble));
    LOG_info("Haptic effect play channels: %d", SDL_HapticNumEffectsPlaying(newRumble));
    LOG_info("Haptic effect number of axes: %d", SDL_HapticNumAxes(newRumble));

    unsigned int hapticQuery = SDL_HapticQuery(newRumble);
    //Haptic effects
    LOG_info("Haptic functions available: ");
    //Multi-motor
    rumbleLRSupported = ((hapticQuery & SDL_HAPTIC_LEFTRIGHT) != 0);
    LOG_info("SDL_HAPTIC_LEFTRIGHT: %d", rumbleLRSupported);

    //Rumble wave types
    LOG_info("SDL_HAPTIC_CONSTANT: %d", ((hapticQuery & SDL_HAPTIC_CONSTANT) != 0));
    LOG_info("SDL_HAPTIC_SINE: %d", ((hapticQuery & SDL_HAPTIC_SINE) != 0));
    LOG_info("SDL_HAPTIC_TRIANGLE: %d", ((hapticQuery & SDL_HAPTIC_TRIANGLE) != 0));
    LOG_info("SDL_HAPTIC_SAWTOOTHUP: %d", ((hapticQuery & SDL_HAPTIC_SAWTOOTHUP) != 0));
    LOG_info("SDL_HAPTIC_SAWTOOTHDOWN: %d", ((hapticQuery & SDL_HAPTIC_SAWTOOTHDOWN) != 0));
    LOG_info("SDL_HAPTIC_RAMP: %d", ((hapticQuery & SDL_HAPTIC_RAMP) != 0));
    //Define your own wave shape
    LOG_info("SDL_HAPTIC_CUSTOM: %d", ((hapticQuery & SDL_HAPTIC_CUSTOM) != 0));
    //Complicated stuff that requires super-specific hardware
    LOG_info("SDL_HAPTIC_SPRING: %d", ((hapticQuery & SDL_HAPTIC_SPRING) != 0));
    LOG_info("SDL_HAPTIC_DAMPER: %d", ((hapticQuery & SDL_HAPTIC_DAMPER) != 0));
    LOG_info("SDL_HAPTIC_INERTIA: %d", ((hapticQuery & SDL_HAPTIC_INERTIA) != 0));
    LOG_info("SDL_HAPTIC_FRICTION: %d", ((hapticQuery & SDL_HAPTIC_FRICTION) != 0));
    //Features
    LOG_info("SDL_HAPTIC_GAIN: %d", ((hapticQuery & SDL_HAPTIC_GAIN) != 0));
    LOG_info("SDL_HAPTIC_AUTOCENTER: %d", ((hapticQuery & SDL_HAPTIC_AUTOCENTER) != 0));
    LOG_info("SDL_HAPTIC_STATUS: %d", ((hapticQuery & SDL_HAPTIC_STATUS) != 0));
    LOG_info("SDL_HAPTIC_PAUSE: %d", ((hapticQuery & SDL_HAPTIC_PAUSE) != 0));

    if(!rumbleLRSupported)
    {
        int canRumble = SDL_HapticRumbleInit(newRumble);    //Fallback to basic SDL rumble support (No LR boo hoo)
        if(canRumble != 0)
            newRumble = NULL;    //Cannot rumble with this device
    }
    return newRumble;
}

InputDevice::~InputDevice()
{
    if(!rumbleLRSupported && m_haptic != NULL)
        SDL_HapticRumbleStop(m_haptic);
    else if(m_haptic != NULL)
        SDL_HapticClose(m_haptic);
    if(m_controller != NULL)
        SDL_GameControllerClose(m_controller);
    if(ssHaptic != NULL)
        delete ssHaptic;
}

void InputDevice::rumbleControllerBasic(float strength, uint32_t duration, float curTime)
{
    static float fLastRumble = 0.0f;

    //Last rumble still going
    if(curTime < fLastRumble)
        return;

    fLastRumble = curTime + (float)duration / 1000.0f;

    strength = std::max(strength, 0.0f);
    strength = std::min(strength, 1.0f);
    if(m_haptic != NULL)
        SDL_HapticRumblePlay(m_haptic, strength, duration);
}

void InputDevice::rumbleLR(uint32_t duration, uint16_t largeMotor, uint16_t smallMotor, float curTime)
{
    float strength = (float)(largeMotor + smallMotor);
    strength /= (float)USHRT_MAX * 2.0f;
    if(m_haptic == NULL)
    {
        if(ssHaptic != NULL && ssHaptic->isValid())
            ssHaptic->rumble(strength, duration, curTime);
        return;    //Have to have an active rumble first
    }

    //If we don't support LR rumble, just do a basic one
    if(!rumbleLRSupported)
    {
        rumbleControllerBasic(strength, duration, curTime);
        return;
    }

    static float fLastRumble = 0.0f;
    float sec = (float)duration / 1000.0f;

    //Last rumble is still going or 0-msec duration
    if(curTime < fLastRumble || duration < 1)
        return;

    fLastRumble = curTime + sec;

    LOG_info("Running LR effect: %d, %f, %d, %d", duration, sec, largeMotor, smallMotor);
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
        LOG_warn("Unable to create LR effect: %s", SDL_GetError());
        return;
    }

    if(SDL_HapticRunEffect(m_haptic, curEffect, 1) < 0)
    {
        LOG_warn("Unable to run LR effect: %s", SDL_GetError());
    }
}

bool InputDevice::hasHaptic()
{
    return m_haptic != NULL || (ssHaptic != NULL && ssHaptic->isValid());
}

int InputDevice::getAxis(int axis)
{
    return SDL_GameControllerGetAxis(m_controller, (SDL_GameControllerAxis)axis);
}

bool InputDevice::getButton(int button)
{
    return(SDL_GameControllerGetButton(m_controller, (SDL_GameControllerButton)button) > 0);
}
