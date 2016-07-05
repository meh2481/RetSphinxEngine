#pragma once
#include "Observer.h"
#include <list>
using namespace std;

class Subject
{
	list<Observer*> observers;
	int numObservers;

public:

	void addObserver(Observer* o);
	void removeObserver(Observer* o);

	void notify(string sMsg, Vec2 pos);

};