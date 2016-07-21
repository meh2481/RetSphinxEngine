#pragma once
#include <stdint.h>
#include "Rect.h"

class Image;

class Font
{
	unsigned int num;
	Image* img;
	uint32_t* codepoints;
	float* rects;

	Font() {};

	void renderChar(float drawPt, Vec2 offset, float* rect);
	uint32_t getIndex(uint32_t codepoint);

public:
	Font(Image* image, unsigned int count, uint32_t* codePoints, float* imgRects);	//codepoint/imgrect mem will be freed on obj deletion
	~Font();

	void renderString(const char* str, float drawPt);
};