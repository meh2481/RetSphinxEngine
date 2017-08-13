#pragma once
#include "SDL_opengl.h"

class TextureHandle
{
public:
	GLuint tex;
	unsigned int width;
	unsigned int height;
};

class Image
{
public:
	TextureHandle tex;
	GLfloat uv[8];
};

class Quad
{
public:
	Image tex;
	GLfloat pos[8];
};

namespace Draw
{
	void drawQuad(Quad* q);	//Yeeeeaaaaaaah
}
