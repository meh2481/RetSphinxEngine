/*
	GameEngine header - Text.h
	Class for ease of drawing bitmapped fonts
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "Image.h"
#include "Color.h"
#include <map>

enum TextAlignment
{
    ALIGN_LEFT		= 1,
    ALIGN_RIGHT		= 2,
    ALIGN_CENTER	= 4,
    ALIGN_TOP		= 8,
    ALIGN_MIDDLE	= 16,
    ALIGN_BOTTOM	= 32
};

class Image;

class Text
{
private:
	Text(){};								//Default constructor cannot be called
	Image* m_imgFont;						//Image for this bitmap font
	std::map<unsigned char, Rect> m_mRectangles;	//Rectangles for drawing each character
	std::map<unsigned char, float> m_mKerning;	//Kerning info for font glyphs
	std::string m_sName;

public:
	Color col;

	Text(std::string sXMLFilename);  //Create the font from this XML file
	~Text();

	//Render this text to the screen, centered on x and y
	void render(std::string sText, float x, float y, float pt);

	//Find the size of a given string of text
	float size(std::string sText, float pt);
	std::string getName()	{return m_sName;};
	void   setName(std::string sName)	{m_sName = sName;};

};





















