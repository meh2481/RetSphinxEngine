#include "InputManager.h"
#include "InputDevice.h"

InputManager::InputManager()
{
	m_curActiveController = -1;
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
