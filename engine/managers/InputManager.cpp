#include "InputManager.h"
#include "InputDevice.h"

InputManager::InputManager()
{
	m_curActiveController = -1;
	m_iKeystates = SDL_GetKeyboardState(NULL);	//Get key state array once; updates automagically
}

InputManager::~InputManager()
{
	for(std::vector<InputDevice*>::iterator i = m_controllers.begin(); i != m_controllers.end(); i++)
		delete *i;
}

InputDevice* InputManager::getCurController()
{
	if(m_curActiveController < 0 || m_curActiveController > m_controllers.size() - 1)
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
	m_curActiveController = m_controllers.size() - 1;	//Set this as new active controller
}

bool InputManager::removeController(int deviceIndex)
{
	for(int i = 0; i < m_controllers.size(); i++)
	{
		if(deviceIndex == m_controllers[i]->getDeviceIndex())
		{
			//Remove this controller from the list
			delete m_controllers[i];
			m_controllers.erase(m_controllers.begin() + i);
			if(i == m_curActiveController)
			{
				return true;	//This was active controller
			}
			if(i <= m_curActiveController)
			{
				m_curActiveController--;
				if(m_curActiveController < 0)	//Was first in the list
					m_curActiveController = m_controllers.size() - 1;
			}
			break;
		}
	}
	return false;	//This was not active controller
}

void InputManager::activateController(int deviceIndex)
{
	if(deviceIndex < 0 || deviceIndex >= m_controllers.size())
		m_curActiveController = 0;
	else
	{
		for(int i = 0; i < m_controllers.size(); i++)
		{
			if(deviceIndex == m_controllers[i]->getDeviceIndex())
				m_curActiveController = i;
		}
	}
}

bool InputManager::keyDown(int32_t keyCode)
{
	if(m_iKeystates == NULL) return false;	//On first cycle, this can be NULL and cause segfaults otherwise

	//HACK: See if one of our combined keycodes
	if(keyCode == SDL_SCANCODE_CTRL) return (keyDown(SDL_SCANCODE_LCTRL) || keyDown(SDL_SCANCODE_RCTRL));
	if(keyCode == SDL_SCANCODE_SHIFT) return (keyDown(SDL_SCANCODE_LSHIFT) || keyDown(SDL_SCANCODE_RSHIFT));
	if(keyCode == SDL_SCANCODE_ALT) return (keyDown(SDL_SCANCODE_LALT) || keyDown(SDL_SCANCODE_RALT));
	if(keyCode == SDL_SCANCODE_GUI) return (keyDown(SDL_SCANCODE_LGUI) || keyDown(SDL_SCANCODE_RGUI));

	//Otherwise, just use our pre-polled list we got from SDL
	return(m_iKeystates[keyCode]);
}
