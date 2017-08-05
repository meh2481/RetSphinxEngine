#pragma once
#include "SDL_opengl.h"

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
} Img;	//TODO Rename Image when migration is done
