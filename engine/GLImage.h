/*
 GameEngine header - Image.h
 Copyright (c) 2014 Mark Hutcheson
*/
#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "globaldefs.h"
#include "lattice.h"

class GLImage
{
private:
	GLImage(){};  //Default constructor is uncallable

	GLuint   	m_hTex;
	string     	m_sFilename;
	uint32_t 	m_iWidth, m_iHeight;			// width and height of original image
	
	bool m_bReloadEachTime;						// If we should reload the image each time we try to create it
	
#ifdef BIG_ENDIAN
	uint32_t m_iRealWidth, m_iRealHeight;
#endif

	void _load(string sFilename);
	void _loadNoise(string sXMLFilename);

public:
	GLuint _getTex() {return m_hTex;};
	
	//Constructor/destructor
	GLImage(string sFilename);
	//Image(uint32_t width, uint32_t height, float sizex = 1.0f, float sizey = 1.0f, float xoffset = 0.0f, float yoffset = 0.0f);	//Create image from random noise
	~GLImage();
    
	//Engine use functions
	void _reload();  //Reload memory associated with this image
	void _setFilename(string s) {m_sFilename = s;};	//Potentially dangerous; use with caution

	//Accessor methods
	uint32_t getWidth()     {return m_iWidth;};
	uint32_t getHeight()    {return m_iHeight;};
	const string& getFilename()    {return m_sFilename;};
	bool reloadEachTime()	{return m_bReloadEachTime;};
	
	//Drawing methods for texel-based coordinates
	void render(Point size, float tilex = 1.0f, float tiley = 1.0f);				//Render at 0,0 with specified texel size
	void renderLattice(lattice* l, Point size);	//Render at 0,0 with specified lattice
	void render(Point size, Rect rcImg);
	//void render(Point size, Point shear);	//Render at 0,0 with specified size and shear amount
	void render4V(Point ul, Point ur, Point bl, Point br);
};

//Image reloading handler functions
void reloadImages();
void _addImgReload(GLImage* img);
void _removeImgReload(GLImage* img);

//Other image functions
GLImage* getImage(string sFilename);  //Retrieves an image from the filename, creating it if necessary
void clearImages();

extern bool g_imageBlur;

#endif



