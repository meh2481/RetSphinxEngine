#pragma once

class Color;
class Image;

class Drawable
{
public:
	Drawable() { depth = 0; img = NULL; active = true; };
	~Drawable() {};
	
	float depth;
	Image* img;
	bool active;
	Color col;
	
	virtual void draw(bool bDebugInfo = false) = 0;	//TODO This shouldn't have a debug thing?

};
