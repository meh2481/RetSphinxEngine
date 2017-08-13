#include "ImgFont.h"
#include "easylogging++.h"
#include "Quad.h"
#include <cstring>

ImgFont::ImgFont(Image* image, unsigned int count, uint32_t* codePoints, float* imgRects)
{
	//Make sure data is valid
	assert(count > 1);
	assert(image != NULL);
	assert(codePoints != NULL);
	assert(imgRects != NULL);

	num = count;
	img = image;
	codepoints = codePoints;
	rects = imgRects;
}

ImgFont::~ImgFont()
{
	free(codepoints);
	free(rects);
}

uint32_t ImgFont::getIndex(uint32_t codepoint)
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

void ImgFont::renderChar(Vec2 drawSz, Vec2 offset, float* rect)
{
	//TODO Store vertex data & don't re-send this to the gfx card every frame
	Rect rc(
		-drawSz.x / 2.0f + offset.x,
		drawSz.y / 2.0f + offset.y,
		drawSz.x / 2.0f + offset.x,
		-drawSz.y / 2.0f + offset.y);

	const float vertexData[] =
	{
		rc.left, rc.top, // upper left
		rc.right, rc.top, // upper right
		rc.right, rc.bottom, // lower right
		rc.left, rc.bottom // lower left
	};

	glVertexPointer(2, GL_FLOAT, 0, &vertexData);
	glTexCoordPointer(2, GL_FLOAT, 0, rect);
	glDrawArrays(GL_QUADS, 0, 4);

}

float* ImgFont::getNextRect(const char** str)
{
	uint32_t idx = getIndex(getNextCodepoint(str));
	return &rects[idx * 8];
}

uint32_t ImgFont::getNextCodepoint(const char** strpos)
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

void ImgFont::renderString(const char* str, float drawPt, Vec2 drawOffset)
{
	float fac = (float)img->tex.width / (float)img->tex.height;
	glBindTexture(GL_TEXTURE_2D, img->tex.tex);
	const char* strptr = str;
	while(*strptr)
	{
		//Get the rect for this character
		float* rectPos = getNextRect(&strptr);
		float charHeight = rectPos[5] - rectPos[3];
		float charWidth = (rectPos[2] - rectPos[0]) * fac;

		//Render this character
		Vec2 drawSz;
		drawSz.y = drawPt;
		drawSz.x = (drawPt / charHeight)*charWidth;
		renderChar(drawSz, drawOffset, rectPos);

		//Increase offset by this char's width
		if(charWidth > 0.0f && charHeight > 0.0f)
			drawOffset.x += (drawPt/charHeight)*charWidth;	//TODO Kerning & word wrap

		//Next loop
		strptr++;
	}
}

float ImgFont::stringWidth(const char* str, float drawPt)
{
	float fac = (float)img->tex.width / (float)img->tex.height;
	float width = 0.0f;
	const char* strptr = str;
	while(*strptr)
	{
		//Grab the index of this char
		float* rectPos = getNextRect(&strptr);
		//Increase offset by this char's width
		float charHeight = rectPos[5] - rectPos[3];
		float charWidth = (rectPos[2] - rectPos[0]) * fac;
		if(charWidth > 0.0f && charHeight > 0.0f)
			width += (drawPt / charHeight)*charWidth;	//TODO Kerning & word wrap

		//Next loop
		strptr++;
	}
	return width;
}
