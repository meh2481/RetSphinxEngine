#pragma once
#include <string>
#include "Rect.h"


class Observer
{
public:
	virtual void onNotify(const std::string& sMsg, Vec2 pos) = 0;
};