/*
    GameEngine header - cursor.h
    Defines a class for mouse cursor handling
    Copyright (c) 2014 Mark Hutcheson
*/

#pragma once

#include "globaldefs.h"

class Image;
 
class myCursor
{
protected:
	void _init();
	
public:
	myCursor();
	~myCursor();
	
	Point pos;
	Point size;
	Point hotSpot;
	Image* img;
	float rot;
	
	void draw();
	bool fromXML(string sXMLFilename);
};
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
