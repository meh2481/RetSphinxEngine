#include "InputManager.h"
#include "InputDevice.h"
#include "ActionBind.h"
#include "MovementBind.h"
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
#include "HeadTracker.h"
#include "CombinedAction.h"

#define MOUSE_KB 0
#define CONTROLLER 1

#define JOY_AXIS_TRIP 10000

InputManager::InputManager()
{
    m_curActiveController = -1;
    m_iKeystates = SDL_GetKeyboardState(NULL);    //Get key state array once; updates automagically

    bindMouseKbActions();
    bindControllerActions();
    m_headTracker = NULL;
}

InputManager::~InputManager()
{
    for(int j = 0; j < 2; j++)
    {
        for(int i = 0; i < NUM_ACTIONS; i++)
            delete actions[j][i];
        for(int i = 0; i < NUM_MOVEMENTS; i++)
            delete movements[j][i];
    }
    for(std::vector<InputDevice*>::iterator i = m_controllers.begin(); i != m_controllers.end(); i++)
        delete *i;
    if(m_headTracker)
        delete m_headTracker;
}

InputDevice* InputManager::getCurController()
{
    if(m_curActiveController < 0 || (size_t)m_curActiveController > m_controllers.size() - 1)
        return NULL;
    return m_controllers[m_curActiveController];
}

void InputManager::addController(int deviceIndex)
{
    addController(new InputDevice(deviceIndex));
}

void InputManager::addController(InputDevice* device)
{
    m_controllers.push_back(device);
    m_curActiveController = m_controllers.size() - 1;    //Set this as new active controller
}

void InputManager::addHeadTracker(SDL_Joystick * joy)
{
    if(m_headTracker != NULL)
    {
        //Only one head tracker at a time for now
        SDL_JoystickClose(joy);
        return;
    }
    m_headTracker = new HeadTracker(joy);
}

bool InputManager::removeController(int deviceIndex)
{
    for(size_t i = 0; i < m_controllers.size(); i++)
    {
        if(deviceIndex == m_controllers[i]->getDeviceIndex())
        {
            //Remove this controller from the list
            delete m_controllers[i];
            m_controllers.erase(m_controllers.begin() + i);
            if(i == m_curActiveController)
            {
                return true;    //This was active controller
            }
            if(i <= (size_t)m_curActiveController)
            {
                m_curActiveController--;
                if(m_curActiveController < 0)    //Was first in the list
                    m_curActiveController = m_controllers.size() - 1;
            }
            break;
        }
    }
    return false;    //This was not active controller
}

void InputManager::activateController(int deviceIndex)
{
    if(deviceIndex < 0 || (size_t)deviceIndex >= m_controllers.size())
        m_curActiveController = 0;
    else
    {
        for(size_t i = 0; i < m_controllers.size(); i++)
        {
            if(deviceIndex == m_controllers[i]->getDeviceIndex())
                m_curActiveController = i;
        }
    }
}

bool InputManager::keyDown(int32_t keyCode)
{
    //See if one of our combined keycodes
    if(keyCode == SDL_SCANCODE_CTRL) return (keyDown(SDL_SCANCODE_LCTRL) || keyDown(SDL_SCANCODE_RCTRL));
    if(keyCode == SDL_SCANCODE_SHIFT) return (keyDown(SDL_SCANCODE_LSHIFT) || keyDown(SDL_SCANCODE_RSHIFT));
    if(keyCode == SDL_SCANCODE_ALT) return (keyDown(SDL_SCANCODE_LALT) || keyDown(SDL_SCANCODE_RALT));
    if(keyCode == SDL_SCANCODE_GUI) return (keyDown(SDL_SCANCODE_LGUI) || keyDown(SDL_SCANCODE_RGUI));

    //Otherwise, just use our pre-polled list we got from SDL
    return !!(m_iKeystates[keyCode]);
}

bool InputManager::getDigitalAction(Action a)
{
    int mousekb = CONTROLLER;
    if(m_controllers[m_curActiveController]->getDeviceIndex() == KB_MOUSE_DEVICE_INDEX)
        mousekb = MOUSE_KB;
    return actions[mousekb][a]->getDigitalAction(m_controllers[m_curActiveController]);
}

float InputManager::getAnalogAction(Action a)
{
    int mousekb = CONTROLLER;
    if(m_controllers[m_curActiveController]->getDeviceIndex() == KB_MOUSE_DEVICE_INDEX)
        mousekb = MOUSE_KB;
    return actions[mousekb][a]->getAnalogAction(m_controllers[m_curActiveController]);
}

Vec2 InputManager::getMovement(Movement m)
{
    int mousekb = CONTROLLER;
    if(m_controllers[m_curActiveController]->getDeviceIndex() == KB_MOUSE_DEVICE_INDEX)
        mousekb = MOUSE_KB;
    Vec2 ret = movements[mousekb][m]->getMovement(m_controllers[m_curActiveController]);
    //Normalize
    //float len = ret.length();    //Apparently GLM is just dead wrong here; off by a factor of 2
    float len = sqrt(ret.x*ret.x + ret.y*ret.y);
    if(len > 1.0f)
        ret /= len;
    return ret;
}

Vec3 InputManager::getHeadMovement()
{
    if(m_headTracker == NULL)
        return Vec3(0,0,0);
    return Vec3(m_headTracker->getX(), m_headTracker->getY(), m_headTracker->getZ());
}

void InputManager::bindMouseKbActions()
{
    //TODO: Example; can be expanded upon
    actions[MOUSE_KB][JUMP] = new NoAction();
    actions[MOUSE_KB][RUN] = new NoAction();
    actions[MOUSE_KB][SHIP_THRUST] = new KeyboardAction(SDL_SCANCODE_SPACE);
    actions[MOUSE_KB][EXAMINE] = new KeyboardAction(SDL_SCANCODE_W);
    actions[MOUSE_KB][ATTACK] = new MouseButtonAction(SDL_BUTTON_LEFT);
    actions[MOUSE_KB][NOTE_UP] = new CombinedAction(new KeyboardAction(SDL_SCANCODE_W), new KeyboardAction(SDL_SCANCODE_UP));
    actions[MOUSE_KB][NOTE_DOWN] = new CombinedAction(new KeyboardAction(SDL_SCANCODE_S), new KeyboardAction(SDL_SCANCODE_DOWN));
    actions[MOUSE_KB][NOTE_LEFT] = new CombinedAction(new KeyboardAction(SDL_SCANCODE_A), new KeyboardAction(SDL_SCANCODE_LEFT));
    actions[MOUSE_KB][NOTE_RIGHT] = new CombinedAction(new KeyboardAction(SDL_SCANCODE_D), new KeyboardAction(SDL_SCANCODE_RIGHT));

    movements[MOUSE_KB][MOVE] = new KeyboardMovement(SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S);
    movements[MOUSE_KB][AIM] = new KeyboardMovement(SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN);
    movements[MOUSE_KB][PAN] = new NoMovement();
}

void InputManager::bindControllerActions()
{
    //TODO: Example; can be expanded upon
    actions[CONTROLLER][JUMP] = new NoAction();
    actions[CONTROLLER][RUN] = new NoAction();
    actions[CONTROLLER][SHIP_THRUST] = new AxisAction(SDL_CONTROLLER_AXIS_TRIGGERLEFT, JOY_AXIS_TRIP / 10);
    actions[CONTROLLER][EXAMINE] = new JoyButtonAction(SDL_CONTROLLER_BUTTON_A);
    actions[CONTROLLER][ATTACK] = new JoyButtonAction(SDL_CONTROLLER_BUTTON_A);
    actions[CONTROLLER][NOTE_UP] = new CombinedAction(new JoyButtonAction(SDL_CONTROLLER_BUTTON_Y), new JoyButtonAction(SDL_CONTROLLER_BUTTON_DPAD_UP));
    actions[CONTROLLER][NOTE_DOWN] = new CombinedAction(new JoyButtonAction(SDL_CONTROLLER_BUTTON_A), new JoyButtonAction(SDL_CONTROLLER_BUTTON_DPAD_DOWN));
    actions[CONTROLLER][NOTE_LEFT] = new CombinedAction(new JoyButtonAction(SDL_CONTROLLER_BUTTON_X), new JoyButtonAction(SDL_CONTROLLER_BUTTON_DPAD_LEFT));
    actions[CONTROLLER][NOTE_RIGHT] = new CombinedAction(new JoyButtonAction(SDL_CONTROLLER_BUTTON_B), new JoyButtonAction(SDL_CONTROLLER_BUTTON_DPAD_RIGHT));

    movements[CONTROLLER][MOVE] = new JoystickMovement(SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY, JOY_AXIS_TRIP);
    movements[CONTROLLER][AIM] = new JoystickMovement(SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY, JOY_AXIS_TRIP);
    movements[CONTROLLER][PAN] = new DpadMovement(SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
}
