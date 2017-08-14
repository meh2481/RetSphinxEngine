#pragma once

class Texture
{
public:
	unsigned int tex;
	unsigned int width;
	unsigned int height;
};

class Image
{
public:
	Texture tex;
	float uv[8];
};

class Quad
{
public:
	Image tex;
	float pos[8];
};

namespace Draw
{
	void drawQuad(Quad* q);	//Yeeeeaaaaaaah
}
