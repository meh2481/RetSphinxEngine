#pragma once
#include "Color.h"
class Image;

//TODO Remove
class Drawable
{
public:
	Drawable() { depth = 0; img = NULL; active = true; };
	~Drawable() {};
	
	float depth;
	Image* img;
	bool active;
	Color col;
	
	virtual void draw(bool bDebugInfo = false) = 0;

};
