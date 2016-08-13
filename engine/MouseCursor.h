/*
    GameEngine header - cursor.h
    Defines a class for mouse cursor handling
    Copyright (c) 2014 Mark Hutcheson
*/

#pragma once
#include "Rect.h"

class Image;
 
class MouseCursor
{
public:
	void _init();

	MouseCursor();
	~MouseCursor();
	
	Vec2 pos;
	Vec2 size;
	Vec2 hotSpot;
	Image* img;
	float rot;
	
	void draw();
};
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
