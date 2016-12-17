#pragma once
#include "Color.h"
class Image;

//TODO Remove
class DrawableItem
{
public:
	DrawableItem() { depth = 0; img = NULL; active = true; };
	~DrawableItem() {};
	
	float depth;
	Image* img;
	bool active;
	Color col;
	
	virtual void draw(bool bDebugInfo = false) = 0;

};
