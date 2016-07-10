#pragma once
#include "Observer.h"
#include <list>

class Subject
{
	std::list<Observer*> observers;
	int numObservers;

public:

	void addObserver(Observer* o);
	void removeObserver(Observer* o);

	void notify(std::string sMsg, Vec2 pos);

};