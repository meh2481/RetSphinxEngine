#include "CombinedAction.h"

CombinedAction::CombinedAction(ActionBind * a1, ActionBind * a2)
{
    action1 = a1;
    action2 = a2;
}

CombinedAction::~CombinedAction()
{
    delete action1;
    delete action2;
}

bool CombinedAction::getDigitalAction(InputDevice * d)
{
    return (action1->getDigitalAction(d) || action2->getDigitalAction(d));
}

float CombinedAction::getAnalogAction(InputDevice * d)
{
    float res = action1->getAnalogAction(d);
    if(res == 0.0)
        return action2->getAnalogAction(d);
    return res;
}
