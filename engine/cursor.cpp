/*
    GameEngine source - cursor.cpp
    Copyright (c) 2014 Mark Hutcheson
*/
#include "cursor.h"
#include "GLImage.h"
#include "tinyxml2.h"
using namespace tinyxml2;


//TODO: Get rid. Use built-in SDL cursor functionality instead. Unless we want animated cursors?
myCursor::myCursor()
{
	_init();
}

myCursor::~myCursor()
{
	
}
	
void myCursor::_init()
{
	img = NULL;
	rot = 0;
}
	
void myCursor::draw()
{
	if(img != NULL)
	{
		Point ptDrawPos = pos;
		ptDrawPos.x += size.x / 2.0;
		ptDrawPos.x -= hotSpot.x/(float)img->getWidth() * size.x;
		ptDrawPos.y -= size.y / 2.0;
		ptDrawPos.y += hotSpot.y/(float)img->getHeight() * size.y;
		glPushMatrix();
		glTranslatef(ptDrawPos.x, ptDrawPos.y, 0.0f);
		glRotatef(rot, 0, 0, 1);
		img->render(size);
		glPopMatrix();
	}
}
	
bool myCursor::fromXML(string sXMLFilename)
{
	_init();
	
	XMLDocument* doc = new XMLDocument();
    int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing XML file " << sXMLFilename << ": Error " << iErr << endl;
		delete doc;
		return false;
	}

    XMLElement* root = doc->FirstChildElement("cursor");
    if(root == NULL)
	{
		errlog << "Error: No toplevel \"cursor\" item in XML file " << sXMLFilename << endl;
		delete doc;
		return false;
	}
	
	const char* cImgPath = root->Attribute("path");
	if(cImgPath)
		img = getImage(cImgPath);
	
	const char* cSize = root->Attribute("size");
	if(cSize)
		size = pointFromString(cSize);
	
	const char* cHotSpot = root->Attribute("hotspot");
	if(cHotSpot)
		hotSpot = pointFromString(cHotSpot);	
	
	delete doc;
	return true;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	