#pragma once
#include <stdint.h>
#include "Rect.h"
#include "Quad.h"

class ImgFont
{
	unsigned int num;
	Texture* img;
	uint32_t* codepoints;
	float* rects;

	ImgFont() {};

	void renderChar(Vec2 drawSz, Vec2 offset, float* rect);
	uint32_t getIndex(uint32_t codepoint);
	uint32_t getNextCodepoint(const char** strpos);	//Get the next codepoint from the given string & increment the pointer
	float* getNextRect(const char** str);			//Get the next image rect for the given string position & increment the pointer

public:
	ImgFont(Texture* image, unsigned int count, uint32_t* codePoints, float* imgRects);	//codepoint/imgrect mem will be freed on obj deletion NOTE MUST BE CREATED WITH MALLOC
	~ImgFont();

	void renderString(const char* str, float drawPt, Vec2 drawOffset);
	float stringWidth(const char* str, float drawPt);
};
