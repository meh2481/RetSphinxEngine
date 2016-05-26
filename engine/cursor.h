/*
    GameEngine header - cursor.h
    Defines a class for mouse cursor handling
    Copyright (c) 2014 Mark Hutcheson
*/

#ifndef CURSOR_H
#define CURSOR_H

#include "globaldefs.h"

class GLImage;
 
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
	GLImage* img;
	float rot;
	
	void draw();
	bool fromXML(string sXMLFilename);
};
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#endif //defined CURSOR_H
