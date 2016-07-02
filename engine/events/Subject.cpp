#include "Subject.h"

void Subject::addObserver(Observer * o)
{
	observers.push_back(o);
}

void Subject::removeObserver(Observer * o)
{
	for(list<Observer*>::iterator i = observers.begin(); i != observers.end(); i++)
	{
		if(*i == o)
		{
			observers.erase(i);
			return;
		}
	}
}

void Subject::notify(string sMsg, Point pos)
{
	for(list<Observer*>::iterator i = observers.begin(); i != observers.end(); i++)
	{
		(*i)->onNotify(sMsg, pos);
	}
}
