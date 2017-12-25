#pragma once
#include "Observer.h"
#include <vector>

class Subject
{
    std::vector<Observer*> observers;
    int numObservers;

public:

    void addObserver(Observer* o);
    void removeObserver(Observer* o);

    void notify(const std::string& sMsg, Vec2 pos);

};