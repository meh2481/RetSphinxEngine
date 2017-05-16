#include "Subject.h"

void Subject::addObserver(Observer * o)
{
	observers.push_back(o);
}

void Subject::removeObserver(Observer * o)
{
	for(std::list<Observer*>::iterator i = observers.begin(); i != observers.end(); i++)
	{
		if(*i == o)
		{
			observers.erase(i);
			return;
		}
	}
}

void Subject::notify(const std::string& sMsg, Vec2 pos)
{
	for(std::list<Observer*>::iterator i = observers.begin(); i != observers.end(); i++)
	{
		(*i)->onNotify(sMsg, pos);
	}
}
