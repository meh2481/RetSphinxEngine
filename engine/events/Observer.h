#pragma once
#include <string>
#include "globaldefs.h"


class Observer
{
public:
	virtual void onNotify(string sMsg, Vec2 pos) = 0;
};