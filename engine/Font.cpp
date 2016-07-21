#include "Font.h"
#include "Image.h"
#include "easylogging++.h"
#include <cstring>

Font::Font(Image* image, unsigned int count, uint32_t* codePoints, float* imgRects)
{
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
	//TODO: Binary search
	return 0;
}

void Font::renderChar(float drawPt, Vec2 offset, float* rect)
{
	//TODO
}

void Font::renderString(const char* str, float drawPt)
{

	Vec2 drawOffset(0, 0); //TODO
	unsigned int curPos = 0;
	unsigned int strLen = strlen(str);
	const char* strptr = str;
	while(curPos < strLen)
	{
		//Determine UTF-8 codepoint
		int seqlen = 0;
		uint8_t pointHeader = *((uint8_t*)strptr);
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
			strptr++;
			curPos++;
			codepoint |= (*strptr & 0x3F);
		}

		//Render this character
		uint32_t idx = getIndex(codepoint);
		float* rectPos = &rects[idx * 8];
		renderChar(drawPt, drawOffset, rectPos);
		//Increase offset by this char's width
		drawOffset.x += rectPos[2] - rectPos[0];	//TODO Kerning and word wrap

		//Next loop
		strptr++;
		curPos++;
	}
}