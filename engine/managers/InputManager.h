#pragma once
#include <vector>
#include "SDL_scancode.h"
#include "Action.h"
#include "Rect.h"

//SDL codes that should be defined but aren't
#define SDL_BUTTON_FORWARD	SDL_BUTTON_X2
#define SDL_BUTTON_BACK		SDL_BUTTON_X1
#define SDL_SCANCODE_CTRL	(SDL_NUM_SCANCODES)
#define SDL_SCANCODE_SHIFT 	(SDL_NUM_SCANCODES+1)
#define SDL_SCANCODE_ALT	(SDL_NUM_SCANCODES+2)
#define SDL_SCANCODE_GUI	(SDL_NUM_SCANCODES+3)

//Class predeclarations
class InputDevice;

class InputManager
{
private:
	std::vector<InputDevice*> m_controllers;
	int m_curActiveController;
	const Uint8 *m_iKeystates;	//Keep track of keys that are pressed/released so we can poll as needed

public:
	InputManager();
	~InputManager();

	//Controller management
	InputDevice* getCurController();
	void addController(int deviceIndex);
	void addController(InputDevice* device);
	bool removeController(int deviceIndex);		//Return true to signal this was the active one
	void activateController(int deviceIndex);
	std::vector<InputDevice*> getControllerList() { return m_controllers; }

	bool keyDown(int32_t keyCode);  //Test and see if a key is currently pressed (for raw keyboard input)

	//Action (input) handling
	bool getDigitalAction(Action a);	//Get a true/false for this action
	float getAnalogAction(Action a);	//Get a normalized 0..1 for this action (emulated if digital input)
	Vec2 getMovement1();				//Get the vector for movement input 1
	Vec2 getMovement2();				//Get the vector for movement input 2
};