/*
 Pony48 source - Image.h
 Copyright (c) 2014 Mark Hutcheson
*/
#ifndef IMAGE_H
#define IMAGE_H

#include "globaldefs.h"

class Image
{
private:
	Image(){};  //Default constructor is uncallable

	GLuint   	m_hTex;
	string     	m_sFilename;
	uint32_t 	m_iWidth, m_iHeight;			// width and height of original image
#ifdef BIG_ENDIAN
	uint32_t m_iRealWidth, m_iRealHeight;
#endif

	void _load(string sFilename);

public:
	
	//Constructor/destructor
	Image(string sFilename);
	~Image();
    
	//Engine use functions
	void _reload();  //Reload memory associated with this image
	void _setFilename(string s) {m_sFilename = s;};	//Potentially dangerous; use with caution

	//Accessor methods
	uint32_t getWidth()     {return m_iWidth;};
	uint32_t getHeight()    {return m_iHeight;};
	string getFilename()    {return m_sFilename;};
	
	//Drawing methods for texel-based coordinates
	void render(Point size);				//Render at 0,0 with specified texel size
	void render(Point size, Rect rcImg);
	void render(Point size, Point shear);	//Render at 0,0 with specified size and shear amount
	void render4V(Point ul, Point ur, Point bl, Point br);
};

//Image reloading handler functions
void reloadImages();
void _addImgReload(Image* img);
void _removeImgReload(Image* img);

//Other image functions
Image* getImage(string sFilename);  //Retrieves an image from the filename, creating it if necessary
void clearImages();

extern bool g_imageBlur;

#endif



