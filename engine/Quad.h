#pragma once
#include "SDL_opengl.h"

typedef struct
{
	GLuint tex;
	unsigned int width;
	unsigned int height;
} TextureHandle;

typedef struct
{
	TextureHandle tex;
	GLfloat uv[8];
} Texture;

typedef struct
{
	Texture tex;
	GLfloat pos[8];
} Quad;

namespace Draw
{
	void drawQuad(Quad* q);	//Yeeeeaaaaaaah
}
