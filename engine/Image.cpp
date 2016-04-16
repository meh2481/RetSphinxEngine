/*
 GameEngine source - Image.cpp
 Copyright (c) 2014 Mark Hutcheson
*/

#include "Image.h"
#include "Gradient.h"
#include <set>

bool g_imageBlur = true;

Image::Image(string sFilename)
{
	m_bReloadEachTime = false;
	m_hTex = 0;
	m_sFilename = sFilename;
	if(sFilename.find(".xml", sFilename.size()-4) != string::npos)
		_loadNoise(sFilename);
	else
		_load(sFilename);
	_addImgReload(this);
}

#ifdef __BIG_ENDIAN__
//returns the closest power of two value
int power_of_two(int input)
{
	int value = 1;
	while (value < input)
		value <<= 1;
	return value;
}
#endif

void Image::_load(string sFilename)
{
	errlog << "Load " << sFilename << endl;
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(sFilename.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(sFilename.c_str());
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
	{
		errlog << "Unknown image type for file " << sFilename << endl;
		return;
	}
  
	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, sFilename.c_str());
	else
		errlog << "File " << sFilename << " doesn't support reading." << endl;
	//if the image failed to load, return failure
	if(!dib)
	{
		errlog << "Error loading image " << sFilename.c_str() << endl;
		return;
	}  
	//retrieve the image data
  
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
 
	int mode, modeflip;
	if(FreeImage_GetBPP(dib) == 24) // RGB 24bit
	{
#ifdef __BIG_ENDIAN__
		mode = GL_RGB;
		modeflip = GL_RGB;
#else
		mode = GL_RGB;
		modeflip = GL_BGR;
#endif
	}
	else if(FreeImage_GetBPP(dib) == 32)  // RGBA 32bit
	{
#ifdef __BIG_ENDIAN__
		mode = GL_RGBA;
		modeflip = GL_RGBA;
#else
		mode = GL_RGBA;
		modeflip = GL_BGRA;
#endif
	}
  
	bits = FreeImage_GetBits(dib);	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
	{
		errlog << "Something went terribly horribly wrong with getting image bits; just sit and wait for the singularity" << endl;
		return;
	}
  
	//generate an OpenGL texture ID for this texture
	m_iWidth = width;
	m_iHeight = height;
	glGenTextures(1, &m_hTex);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	//store the texture data for OpenGL use
#ifdef __BIG_ENDIAN__
	m_iRealWidth = power_of_two(width);
	m_iRealHeight = power_of_two(height);
	FIBITMAP *bitmap2 = FreeImage_Allocate(m_iRealWidth, m_iRealHeight, FreeImage_GetBPP(dib));
	FreeImage_FlipVertical(dib);
	FreeImage_Paste(bitmap2, dib, 0, 0, 255);
	FreeImage_FlipVertical(bitmap2);
	bits = FreeImage_GetBits(bitmap2);
	glTexImage2D(GL_TEXTURE_2D, 0, mode, m_iRealWidth, m_iRealHeight, 0, modeflip, GL_UNSIGNED_BYTE, bits);
	FreeImage_Unload(bitmap2);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, modeflip, GL_UNSIGNED_BYTE, bits);
#endif
  
	if(g_imageBlur)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	}
	else //If you want things pixellated
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	}
	
	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);
}

//TODO: This is CRAZY slow. Rip out, preload, or otherwise fix.
void Image::_loadNoise(string sXMLFilename)
{
	m_bReloadEachTime = true;
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error opening image noise XML file: " << sXMLFilename << "- Error " << iErr << endl;
		delete doc;
		return;
	}
	
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		errlog << "Error: Root element NULL in XML file " << sXMLFilename << ". Ignoring..." << endl;
		delete doc;
		return;
	}
	
	uint32_t width = 512;
	uint32_t height = 512;
	float32 sizex = 10.0f;
	float32 sizey = 10.0f;
	float32 xoffset = randFloat(0, 5000);	//By default, use random position in noise function
	float32 yoffset = randFloat(0, 5000);
	uint32_t iterations = 5;
	float32 persistence = 0.5f;
	float32 minval = -1.0f;
	float32 maxval = 1.0f;
	float32 freqinc = 2.0f;
	
	root->QueryUnsignedAttribute("width", &width);
	root->QueryUnsignedAttribute("height", &height);
	root->QueryFloatAttribute("sizex", &sizex);
	root->QueryFloatAttribute("sizey", &sizey);
	root->QueryFloatAttribute("xoffset", &xoffset);
	root->QueryFloatAttribute("yoffset", &yoffset);
	root->QueryUnsignedAttribute("iterations", &iterations);
	root->QueryFloatAttribute("persistence", &persistence);
	root->QueryFloatAttribute("min", &minval);
	root->QueryFloatAttribute("max", &maxval);
	root->QueryFloatAttribute("freqinc", &freqinc);
	
	Gradient g;
	const char* cGradFile = root->Attribute("gradient");
	if(cGradFile != NULL)
		g.load(cGradFile);
	else
	{
		g.insert(-1.0f, 255, 255, 255, 0);
		g.insert(1.0f, 255, 255, 255, 255);	//Gradient from transparent white to opaque white
	}
	
	//Done loading XML
	delete doc;
	
	m_iWidth = width;
	m_iHeight = height;
	
	//Generate noise
	unsigned char* bits = new unsigned char[width*height*4];
	unsigned char* ptr = bits;
	for(uint32_t h = 0; h < height; h++)
	{
		for(uint32_t w = 0; w < width; w++)
		{
			//Grab noise value for this pixel from our noise summation function
			float val = sumOcatave(iterations, (float(w+1)/float(width)) + xoffset,(float(h+1)/float(height)) + yoffset, persistence, sizex, sizey, minval, maxval, freqinc);
			Color c = g.getVal(val);
			
			*ptr++ = (unsigned char)(c.r * 255.0f);
			*ptr++ = (unsigned char)(c.g * 255.0f);
			*ptr++ = (unsigned char)(c.b * 255.0f);
			*ptr++ = (unsigned char)(c.a * 255.0f);
		}
	}
	
	glGenTextures(1, &m_hTex);
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	
	delete[] bits;
}


Image::~Image()
{
	//image cleanup
	errlog << "Free " << m_sFilename << endl;
	if(m_hTex)
		glDeleteTextures(1, &m_hTex);	//Free OpenGL graphics memory
	_removeImgReload(this);
}

/* TODO: Intelligent drawing
<fgenesis> i recommend using glViewport and related functions so you don't have to scale stuff into [-1 .. 1] anymore
<fgenesis> let your gfx card do the heavy lifting, not the CPU
<fgenesis> also have a look at glOrtho() and glMatrixMode(), you'll need those
*/

void Image::render(Point size, float tilex, float tiley)
{
	// tell opengl to use the generated texture
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	const GLfloat vertexData[] =
    {
        -size.x/2.0, size.y/2.0, // upper left
        size.x/2.0, size.y/2.0, // upper right
        -size.x/2.0, -size.y/2.0, // lower left
        size.x/2.0, -size.y/2.0, // lower right
    };
    const GLfloat texCoords[] =
    {
        0, tiley, // upper left
        tilex, tiley, // upper right
        0, 0, // lower left
        tilex, 0, // lower right
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Image::renderLattice(lattice* l, Point size)
{
	glPushMatrix();
	
	glScalef(size.x, size.y, 1);
	l->renderTex(m_hTex);
	
	glPopMatrix();
}

// Shear is straightforward. To shear left/right, simply subtract from the x texel coordinates for the top part of the image,
// and add to the x texel coordinates for the bottom part of the image. Similarly, to shear up/down, add to the left side
// (move left side up) and subtract from the right side (move right side down) by the same amount. 
//void Image::render(Point size, Point shear)
//{
	//TODO: Do I even care?
	//render(size);
	/*
	float maxx, maxy;
#ifdef __BIG_ENDIAN__
	maxx = (float)m_iWidth/(float)m_iRealWidth;
	maxy = (float)m_iHeight/(float)m_iRealHeight;
#else
	maxx = maxy = 1.0;
#endif
	// tell opengl to use the generated texture
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	
	const GLfloat vertexData[] =
    {
        -size.x/2.0 - shear.x, size.y/2.0 + shear.y, // upper left
        size.x/2.0 - shear.x, size.y/2.0 - shear.y, // upper right
        -size.x/2.0 + shear.x, -size.y/2.0 + shear.y, // lower left
        size.x/2.0 + shear.x, -size.y/2.0 - shear.y, // lower right
    };
    const GLfloat texCoords[] =
    {
        0.0, maxy, // upper left
        maxx, maxy, // upper right
        0.0, 0.0, // lower left
        maxx, 0.0, // lower right
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
	// make a rectangle
	/*glBegin(GL_QUADS);
	// top left
	glTexCoord2f(0.0, maxy);	//Our real image is in the upper-left corner of memory, flipped vertically. Compensate.
	glVertex3f(-size.x/2.0 - shear.x, size.y/2.0 + shear.y, 0.0);
	// bottom left
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-size.x/2.0 + shear.x, -size.y/2.0 + shear.y, 0.0);
	// bottom right
	glTexCoord2f(maxx, 0.0);
	glVertex3f(size.x/2.0 + shear.x, -size.y/2.0 - shear.y, 0.0);
	// top right
	glTexCoord2f(maxx, maxy);
	glVertex3f(size.x/2.0 - shear.x, size.y/2.0 - shear.y, 0.0);

	glEnd();*/
  
//}

void Image::render(Point size, Rect rcImg)
{
#ifdef __BIG_ENDIAN__
	rcImg.left = rcImg.left / (float)m_iRealWidth;
	rcImg.right = rcImg.right / (float)m_iRealWidth;
	rcImg.top = rcImg.top / (float)m_iRealHeight;
	rcImg.bottom = rcImg.bottom / (float)m_iRealHeight;
#else
	rcImg.left = rcImg.left / (float)m_iWidth;
	rcImg.right = rcImg.right / (float)m_iWidth;
	rcImg.top = 1.0 - rcImg.top / (float)m_iHeight;
	rcImg.bottom = 1.0 - rcImg.bottom / (float)m_iHeight;
#endif
	
	// tell opengl to use the generated texture
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	
	const GLfloat vertexData[] =
    {
        -size.x/2.0, size.y/2.0, // upper left
        size.x/2.0, size.y/2.0, // upper right
        -size.x/2.0, -size.y/2.0, // lower left
        size.x/2.0, -size.y/2.0, // lower right
    };
    const GLfloat texCoords[] =
    {
#ifdef __BIG_ENDIAN__
      rcImg.left, 1.0-rcImg.top, // upper left
      rcImg.right, 1.0-rcImg.top, // upper right
      rcImg.left, 1.0-rcImg.bottom, // lower left
      rcImg.right, 1.0-rcImg.bottom, // lower right
#else
        rcImg.left, rcImg.top, // upper left
        rcImg.right, rcImg.top, // upper right
        rcImg.left, rcImg.bottom, // lower left
        rcImg.right, rcImg.bottom, // lower right
#endif
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  
}

void Image::render4V(Point ul, Point ur, Point bl, Point br)
{
	float maxx, maxy;
#ifdef __BIG_ENDIAN__
	maxx = (float)m_iWidth/(float)m_iRealWidth;
	maxy = (float)m_iHeight/(float)m_iRealHeight;
#else
	maxx = maxy = 1.0;
#endif
	// tell opengl to use the generated texture
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	
	const GLfloat vertexData[] =
    {
        ul.x, ul.y, // upper left
        ur.x, ur.y, // upper right
        bl.x, bl.y, // lower left
        br.x, br.y, // lower right
    };
    const GLfloat texCoords[] =
    {
        0.0, maxy, // upper left
        maxx, maxy, // upper right
        0.0, 0.0, // lower left
        maxx, 0.0, // lower right
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Image::_reload()
{
	if(m_sFilename.find(".xml", m_sFilename.size()-4) != string::npos)
		_loadNoise(m_sFilename);
	else
		_load(m_sFilename);
}

static set<Image*> sg_images;

void reloadImages()
{
	for(set<Image*>::iterator i = sg_images.begin(); i != sg_images.end(); i++)
		(*i)->_reload();
}

void _addImgReload(Image* img)
{
	sg_images.insert(img);
}

void _removeImgReload(Image* img)
{
	sg_images.erase(img);
}

static multimap<string, Image*> g_mImages;  //Image handler
Image* getImage(string sFilename)
{
	if(sFilename == "image_none") return NULL;
	
	multimap<string, Image*>::iterator i = g_mImages.find(sFilename);
	if(i == g_mImages.end() || i->second->reloadEachTime())   //This image isn't here; load it
	{
		Image* img = new Image(sFilename);   //Create this image
		//g_mImages[sFilename] = img; //Add to the map
		g_mImages.insert(std::pair<string, Image*>(sFilename,img));
		return img;
	}
	return i->second; //Return this image
}

void clearImages()
{
	for(multimap<string, Image*>::iterator i = g_mImages.begin(); i != g_mImages.end(); i++)
		delete (i->second);    //Delete each image
	g_mImages.clear();
}



