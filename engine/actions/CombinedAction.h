#pragma once
#include "ActionBind.h"

//Class for unbound actions
class CombinedAction : public ActionBind
{
private:
    ActionBind* action1;
    ActionBind* action2;
    CombinedAction() {};

public:
    CombinedAction(ActionBind* a1, ActionBind* a2);
    ~CombinedAction();
    bool getDigitalAction(InputDevice* d);
    float getAnalogAction(InputDevice* d);
};
