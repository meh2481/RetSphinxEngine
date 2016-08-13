/*
 GameEngine header - Image.h
 Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "lattice.h"
#include "SDL_opengl.h"

class Image
{
private:
	Image(){};  //Default constructor is uncallable

	GLuint    	m_hTex;
	std::string     	m_sFilename;
	int 		m_iWidth, m_iHeight;			// width and height of original image
	
#ifdef BIG_ENDIAN
	uint32_t m_iRealWidth, m_iRealHeight;
#endif

	void _load(std::string sFilename);
	void _loadPNG(std::string sFilename);
	void _loadBlob(unsigned char* blob, unsigned int size);
	void _bind(unsigned char* data, unsigned int width, unsigned int height, int mode);
	//void _loadNoise(string sXMLFilename);

public:
	GLuint  _getTex() {return m_hTex;};
	
	//Constructor/destructor
	Image(std::string sFilename);
	Image(unsigned char* blob, unsigned int size);
	//Image(uint32_t width, uint32_t height, float sizex = 1.0f, float sizey = 1.0f, float xoffset = 0.0f, float yoffset = 0.0f);	//Create image from random noise
	~Image();

	//Accessor methods
	uint32_t getWidth()     {return m_iWidth;};
	uint32_t getHeight()    {return m_iHeight;};
	const std::string& getFilename()    {return m_sFilename;};
	
	//Drawing methods for texel-based coordinates
	void render(Vec2 size, float tilex = 1.0f, float tiley = 1.0f);				//Render at 0,0 with specified texel size
	void renderLattice(Lattice* l, Vec2 size);	//Render at 0,0 with specified lattice
	void render(Vec2 size, Rect rcImg);			//NOTE: Doesn't bind texture!
	void render4V(Vec2 ul, Vec2 ur, Vec2 bl, Vec2 br);

	void bindTexture();	//Bind texture to OpenGL (so we don't have to bind each draw call)
};

