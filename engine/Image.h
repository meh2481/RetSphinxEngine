/*
 GameEngine header - Image.h
 Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "globaldefs.h"
#include "lattice.h"

class Image
{
private:
	Image(){};  //Default constructor is uncallable

	unsigned   	m_hTex;
	string     	m_sFilename;
	int 		m_iWidth, m_iHeight;			// width and height of original image
	
	//bool m_bReloadEachTime;						// If we should reload the image each time we try to create it
	
#ifdef BIG_ENDIAN
	uint32_t m_iRealWidth, m_iRealHeight;
#endif

	void _load(string sFilename);
	void _loadPNG(string sFilename);
	void _loadIMG(string sFilename);
	void _loadBlob(unsigned char* blob, unsigned int size);
	void _bind(unsigned char* data, unsigned int width, unsigned int height, int mode);
	//void _loadNoise(string sXMLFilename);

public:
	unsigned _getTex() {return m_hTex;};
	
	//Constructor/destructor
	Image(string sFilename);
	//Image(uint32_t width, uint32_t height, float sizex = 1.0f, float sizey = 1.0f, float xoffset = 0.0f, float yoffset = 0.0f);	//Create image from random noise
	~Image();
    
	//Engine use functions
	void _reload();  //Reload memory associated with this image
	void _setFilename(string s) {m_sFilename = s;};	//Potentially dangerous; use with caution

	//Accessor methods
	uint32_t getWidth()     {return m_iWidth;};
	uint32_t getHeight()    {return m_iHeight;};
	const string& getFilename()    {return m_sFilename;};
	//bool reloadEachTime()	{return m_bReloadEachTime;};
	
	//Drawing methods for texel-based coordinates
	void render(Vec2 size, float tilex = 1.0f, float tiley = 1.0f);				//Render at 0,0 with specified texel size
	void renderLattice(Lattice* l, Vec2 size);	//Render at 0,0 with specified lattice
	void render(Vec2 size, Rect rcImg);
	void render4V(Vec2 ul, Vec2 ur, Vec2 bl, Vec2 br);
};

