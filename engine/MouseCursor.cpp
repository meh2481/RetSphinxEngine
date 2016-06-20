/*
    GameEngine source - cursor.cpp
    Copyright (c) 2014 Mark Hutcheson
*/
#include "MouseCursor.h"
#include "Image.h"
#include "opengl-api.h"
#include "tinyxml2.h"
#include "easylogging++.h"

//TODO: Get rid. Use built-in SDL cursor functionality instead. Unless we want animated cursors?
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
		Point ptDrawPos = pos;
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
	
bool MouseCursor::fromXML(string sXMLFilename)
{
	_init();
	
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
    int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		LOG(ERROR) << "Error parsing XML file " << sXMLFilename << ": Error " << iErr;
		delete doc;
		return false;
	}

	tinyxml2::XMLElement* root = doc->FirstChildElement("cursor");
    if(root == NULL)
	{
		LOG(ERROR) << "Error: No toplevel \"cursor\" item in XML file " << sXMLFilename;
		delete doc;
		return false;
	}
	
	const char* cImgPath = root->Attribute("path");
	if(cImgPath)
		img = getImage(cImgPath);	//TODO REMOVE
	
	const char* cSize = root->Attribute("size");
	if(cSize)
		size = pointFromString(cSize);
	
	const char* cHotSpot = root->Attribute("hotspot");
	if(cHotSpot)
		hotSpot = pointFromString(cHotSpot);	
	
	delete doc;
	return true;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	