#pragma once
#include <vector>
class InputDevice;

class InputManager
{
private:
	std::vector<InputDevice*> m_controllers;
	int m_curActiveController;

public:
	InputManager();
	~InputManager();

	InputDevice* getCurController();
	void addController(int deviceIndex);
	void addController(InputDevice* device);
	bool removeController(int deviceIndex);		//Return true to signal this was the active one
	void activateController(int deviceIndex);

	std::vector<InputDevice*> getControllerList() { return m_controllers; }
};