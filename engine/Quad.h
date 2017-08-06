#pragma once
#include "SDL_opengl.h"

//TODO: Don't like the redundant structures here
typedef struct
{
	GLuint tex;
	unsigned int width;
	unsigned int height;
} TextureHandle;

typedef struct
{
	GLuint tex;
	GLfloat uv[8];
} Texture;

typedef struct
{
	Texture tex;
	GLfloat pos[8];
} Quad;

typedef struct
{
	unsigned int width;		//Width in pixels
	unsigned int height;	//Height in pixels
	Texture tex;
} Img;	//TODO Rename to Image when migration is done

namespace Draw
{
	void drawQuad(Quad* q);	//Yeeeeaaaaaaah
}
