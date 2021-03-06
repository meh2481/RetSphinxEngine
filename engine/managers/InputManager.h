#pragma once
#include <vector>
#include "SDL_scancode.h"
#include "Action.h"
#include "Movement.h"
#include "Rect.h"
#include "SDL_joystick.h"

//SDL codes that should be defined but aren't
#define SDL_BUTTON_FORWARD    SDL_BUTTON_X2
#define SDL_BUTTON_BACK        SDL_BUTTON_X1
#define SDL_SCANCODE_CTRL    (SDL_NUM_SCANCODES)
#define SDL_SCANCODE_SHIFT     (SDL_NUM_SCANCODES+1)
#define SDL_SCANCODE_ALT    (SDL_NUM_SCANCODES+2)
#define SDL_SCANCODE_GUI    (SDL_NUM_SCANCODES+3)

//Class predeclarations
class InputDevice;
class ActionBind;
class MovementBind;
class HeadTracker;

class InputManager
{
private:
    //TODO Better mouse/kb handling than separate arrays here
    ActionBind* actions[2][NUM_ACTIONS];
    MovementBind* movements[2][NUM_MOVEMENTS];
    std::vector<InputDevice*> m_controllers;
    HeadTracker* m_headTracker;    //Only support one at a time for now
    int m_curActiveController;
    const Uint8 *m_iKeystates;    //Keep track of keys that are pressed/released so we can poll as needed

    void bindMouseKbActions();
    void bindControllerActions();

public:
    InputManager();
    ~InputManager();

    //Controller management
    InputDevice* getCurController();
    void addController(int deviceIndex);
    void addController(InputDevice* device);
    void addHeadTracker(SDL_Joystick* joy);
    bool removeController(int deviceIndex);        //Return true to signal this was the active one
    void activateController(int deviceIndex);
    std::vector<InputDevice*> getControllerList() { return m_controllers; }

    bool keyDown(int32_t keyCode);  //Test and see if a key is currently pressed (for raw keyboard input)

    //Action (input) handling
    bool getDigitalAction(Action a);    //Get a true/false for this action for the current input device
    float getAnalogAction(Action a);    //Get a normalized 0..1 for this action (emulated if digital input) for the current input device
    Vec2 getMovement(Movement m);        //Get the vector for movement
    Vec3 getHeadMovement();                //Get head tracker movement
};