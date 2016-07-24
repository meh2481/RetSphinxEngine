#include "Font.h"
#include "Image.h"
#include "easylogging++.h"
#include <cstring>

Font::Font(Image* image, unsigned int count, uint32_t* codePoints, float* imgRects)
{
	//TODO Make sure count > 1

	num = count;
	img = image;
	codepoints = codePoints;
	rects = imgRects;
}

Font::~Font()
{
	delete[] codepoints;
	delete[] rects;
}

uint32_t Font::getIndex(uint32_t codepoint)
{
	//Binary search
	uint32_t first = 0;
	uint32_t last = num - 1;
	uint32_t middle = (first + last) / 2;

	while(first <= last)
	{
		uint32_t mid = codepoints[middle];
		if(mid < codepoint)
			first = middle + 1;
		else if(mid == codepoint)
			return middle;
		else
			last = middle - 1;

		middle = (first + last) / 2;
	}
	return 0;
}

void Font::renderChar(float drawPt, Vec2 offset, float* rect)
{
	float charHeight = rect[3] - rect[1];
	float charWidth = rect[2] - rect[0];
	//TODO
}

float* Font::getNextRect(const char** str)
{
	uint32_t idx = getIndex(getNextCodepoint(str));
	return &rects[idx * 8];
}

uint32_t getNextCodepoint(const char** strpos)
{
	//Determine UTF-8 codepoint
	int seqlen = 0;
	uint8_t pointHeader = *((uint8_t*)*strpos);
	//Determine how many characters in this UTF-8 codepoint
	while(pointHeader & 0x80)
	{
		seqlen++;
		pointHeader <<= 1;
	}
	//Build codepoint by combining bits
	uint32_t codepoint = pointHeader >> seqlen;
	for(int i = 1; i < seqlen; i++)
	{
		codepoint <<= 6;
		(*strpos)++;
		codepoint |= (**strpos & 0x3F);
	}
	return codepoint;
}

void Font::renderString(const char* str, float drawPt, Vec2 drawOffset)
{
	const char* strptr = str;
	while(*strptr)
	{
		//Render this character
		float* rectPos = getNextRect(&strptr);
		renderChar(drawPt, drawOffset, rectPos);
		//Increase offset by this char's width
		float charHeight = rectPos[3] - rectPos[1];
		float charWidth = rectPos[2] - rectPos[0];
		drawOffset.x += (drawPt/charHeight)*charWidth;	//TODO Kerning & word wrap

		//Next loop
		strptr++;
	}
}

float Font::stringWidth(const char* str, float drawPt)
{
	float width = 0.0f;
	const char* strptr = str;
	while(*strptr)
	{
		//Grab the index of this char
		float* rectPos = getNextRect(&strptr);
		//Increase offset by this char's width
		float charHeight = rectPos[3] - rectPos[1];
		float charWidth = rectPos[2] - rectPos[0];
		width += (drawPt / charHeight)*charWidth;	//TODO Kerning & word wrap

		//Next loop
		strptr++;
	}
	return width;
}
