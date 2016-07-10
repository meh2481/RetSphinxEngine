#pragma once
#include <string>
#include "Rect.h"


class Observer
{
public:
	virtual void onNotify(std::string sMsg, Vec2 pos) = 0;
};