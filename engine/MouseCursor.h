/*
    GameEngine header - cursor.h
    Defines a class for mouse cursor handling
    Copyright (c) 2014 Mark Hutcheson
*/

#pragma once

#include "globaldefs.h"

class Image;
 
//TODO This should be some derivative of Drawable or some child thereof
class MouseCursor
{
protected:
	void _init();
	
public:
	MouseCursor();
	~MouseCursor();
	
	Point pos;
	Point size;
	Point hotSpot;
	Image* img;
	float rot;
	
	void draw();
	bool fromXML(string sXMLFilename);
};
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
