#pragma once
#include <string>
#include "Rect.h"
using namespace std;


class Observer
{
public:
	virtual void onNotify(string sMsg, Vec2 pos) = 0;
};