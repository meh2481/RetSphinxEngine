/*
	Pony48 source - Text.h
	Class for ease of drawing bitmapped fonts
	Copyright (c) 2014 Mark Hutcheson
*/
#ifndef TEXT_H
#define TEXT_H

#include "globaldefs.h"
#include "Image.h"
#include <map>

#define ALIGN_LEFT		1
#define ALIGN_RIGHT		2
#define ALIGN_CENTER	4
#define ALIGN_TOP		8
#define ALIGN_MIDDLE	16
#define ALIGN_BOTTOM	32

class Text
{
private:
	Text(){};								//Default constructor cannot be called
	Image* m_imgFont;						//Image for this bitmap font
	map<unsigned char, Rect> m_mRectangles;	//Rectangles for drawing each character
	map<unsigned char, float32> m_mKerning;	//Kerning info for font glyphs
	string m_sName;

public:
	Color col;

	Text(string sXMLFilename);  //Create the font from this XML file
	~Text();

	//Render this text to the screen, centered on x and y
	void render(string sText, float32 x, float32 y, float pt);

	//Find the size of a given string of text
	float32 size(string sText, float pt);
	string getName()	{return m_sName;};
	void   setName(string sName)	{m_sName = sName;};

};





















#endif
