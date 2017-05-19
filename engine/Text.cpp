/*
	GameEngine source - Text.cpp
	Copyright (c) 2014 Mark Hutcheson
*/

#include "Text.h"
#include "Image.h"
#include "opengl-api.h"
#include "tinyxml2.h"
#include "easylogging++.h"


Text::Text(const std::string& sXMLFilename)
{
	m_imgFont = NULL;

	//  Load font image and glyphs from xml
	//  File format:
	//
	//  <?xml version="1.0" encoding="UTF-8"?>
	//  <font name="green"
	//	  <image path="res/gfx/orig/text_green.png" />
	//	  <char ascii="a" rect="0,0,15,9" kern="1"/>
	//	  ...
	//  </font>

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile(sXMLFilename.c_str());

	tinyxml2::XMLElement* elem = doc->FirstChildElement("font");
	if(elem == NULL) return;
	const char* cName = elem->Attribute("name");
	if(cName != NULL)
		m_sName = cName;
	LOG(INFO) << "Creating font \"" << m_sName << "\"";
	for(elem = elem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		cName = elem->Name();
		if(cName == NULL) return;
		std::string sName(cName);
		if(sName == "image" && m_imgFont == NULL)	//Image
		{
			const char* cPath = elem->Attribute("path");
			if(cPath == NULL) return;
			m_imgFont = new Image(cPath);   //Create image

		}
		else if(sName == "char")	//Character
		{
			const char* cChar = elem->Attribute("ascii");
			if(cChar == NULL) return;
			const char* cRect = elem->Attribute("rect");
			if(cRect == NULL) return;
			m_mRectangles[cChar[0]].fromString(cRect); //Stick this into the list
			float kern = 0.0f;
			elem->QueryFloatAttribute("kern", &kern);
			m_mKerning[cChar[0]] = kern;
		}
	}
	delete doc;
}

Text::~Text()
{
	LOG(INFO) << "Destroying font \"" << m_sName << "\"";
	if(m_imgFont != NULL)
		delete m_imgFont;
}

void Text::render(const std::string& s, float x, float y, float pt)
{
	if(m_imgFont == NULL)
		return;
	std::string sText = s;
	x = -x;
	y = -y;
	glColor4f(col.r, col.g, col.b, col.a);
	float width = size(sText, pt);
	x += width / 2.0f;
	for(std::string::iterator i = sText.begin(); i != sText.end(); i++)
	{
		unsigned char c = *i;
		if(c == '\0')
			break;

		std::map<unsigned char, Rect>::iterator iRect = m_mRectangles.find(c);
		if(iRect == m_mRectangles.end())
			continue;   //Skip over chars we can't draw

		Rect rc = iRect->second;

		glPushMatrix();
		x -= rc.width() * (pt / rc.height())/2.0f;	//Add half the width to get the center (whyyy are we drawing from the center plz dood I fan)
		glTranslatef(-x, -y, 0.0);
		Vec2 sz(rc.width() * (pt / rc.height()), pt);	//Ignore kerning when drawing; we only care about that when computing position
		m_imgFont->render(sz, rc);
		glPopMatrix();
		x -= (rc.width() - m_mKerning[c]*2.0f) * (pt / rc.height())/2.0f;	//Add second half of the width, plus kerning (times 2 because divided by 2... it all works out)
	}
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

float Text::size(const std::string& s, float pt)
{
	float len = 0.0f;
	std::string sText = s;
	for(std::string::iterator i = sText.begin(); i != sText.end(); i++)
	{
		unsigned char c = *i;
		if(c == '\0')
			break;
		
		if(m_mRectangles.count(c))
			len += (m_mRectangles[c].width() - m_mKerning[c]) * (pt / m_mRectangles[c].height());
	}
	return len;
}





