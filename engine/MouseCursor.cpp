/*
    GameEngine source - cursor.cpp
    Copyright (c) 2014 Mark Hutcheson
*/
#include "MouseCursor.h"
#include "Image.h"
#include "opengl-api.h"
#include "tinyxml2.h"
#include "easylogging++.h"

//TODO: Use built-in SDL cursor functionality instead.
MouseCursor::MouseCursor()
{
	_init();
}

MouseCursor::~MouseCursor()
{
	
}
	
void MouseCursor::_init()
{
	img = NULL;
	rot = 0;
}
	
void MouseCursor::draw()
{
	if(img != NULL)
	{
		Vec2 ptDrawPos = pos;
		ptDrawPos.x += size.x / 2.0f;
		ptDrawPos.x -= hotSpot.x/(float)img->getWidth() * size.x;
		ptDrawPos.y -= size.y / 2.0f;
		ptDrawPos.y += hotSpot.y/(float)img->getHeight() * size.y;
		glPushMatrix();
		glTranslatef(ptDrawPos.x, ptDrawPos.y, 0.0f);
		glRotatef(rot, 0, 0, 1);
		img->render(size);
		glPopMatrix();
	}
}
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	