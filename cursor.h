/*
    Pony48 header - cursor.h
    Defines a class for mouse cursor handling
    Copyright (c) 2014 Mark Hutcheson
*/
#include "Image.h"

#ifndef CURSOR_H
#define CURSOR_H
 
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
	float32 rot;
	
	void draw();
	bool fromXML(string sXMLFilename);
};
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#endif //defined CURSOR_H
