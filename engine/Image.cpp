/*
 GameEngine source - Image.cpp
 Copyright (c) 2014 Mark Hutcheson
*/

#include "Image.h"
//#include "Gradient.h"
#include <set>
#include "opengl-api.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Image::Image(string sFilename)
{
	m_bReloadEachTime = false;
	m_hTex = 0;
	m_iWidth = m_iHeight = 0;
	m_sFilename = sFilename;
	if(sFilename.find(".xml", sFilename.size()-4) != string::npos)
		_loadNoise(sFilename);
	else
		_load(sFilename);
	_addImgReload(this);
}

void Image::_load(string sFilename)
{
	int comp = 0;
	unsigned char* cBuf = stbi_load(sFilename.c_str(), &m_iWidth, &m_iHeight, &comp, 0);
	errlog << "Load " << sFilename << endl;

	int mode, modeflip;
	if(comp == 3) // RGB 24bit
	{
		mode = GL_RGB;
		modeflip = GL_RGB;
	}
	else if(comp == 4)  // RGBA 32bit
	{
		mode = GL_RGBA;
		modeflip = GL_RGBA;
	}
  
	if((cBuf == 0) || (m_iWidth == 0) || (m_iHeight == 0))
	{
		errlog << "Something went terribly horribly wrong with getting image bits; just sit and wait for the singularity" << endl;
		return;
	}
  
	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &m_hTex);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, mode, m_iWidth, m_iHeight, 0, modeflip, GL_UNSIGNED_BYTE, cBuf);
  
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	stbi_image_free(cBuf);
}

//TODO: This is CRAZY slow. Rip out, preload, or otherwise fix.
void Image::_loadNoise(string sXMLFilename)
{
	/*m_bReloadEachTime = true;
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
	float sizex = 10.0f;
	float sizey = 10.0f;
	float xoffset = randFloat(0, 5000);	//By default, use random position in noise function
	float yoffset = randFloat(0, 5000);
	uint32_t iterations = 5;
	float persistence = 0.5f;
	float minval = -1.0f;
	float maxval = 1.0f;
	float freqinc = 2.0f;
	
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
	
	delete[] bits;*/
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
	
	const float vertexData[] =
    {
        -size.x/2.0f, size.y/2.0f, // upper left
        size.x/2.0f, size.y/2.0f, // upper right
        -size.x/2.0f, -size.y/2.0f, // lower left
        size.x/2.0f, -size.y/2.0f, // lower right
    };
    const float texCoords[] =
    {
		0.0f, 0.0f, // lower left
		tilex, 0.0f, // lower right
        0.0f, tiley, // upper left
        tilex, tiley, // upper right
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

void Image::render(Point size, Rect rcImg)
{
	rcImg.left = rcImg.left / (float)m_iWidth;
	rcImg.right = rcImg.right / (float)m_iWidth;
	rcImg.top = 1.0f - rcImg.top / (float)m_iHeight;
	rcImg.bottom = 1.0f - rcImg.bottom / (float)m_iHeight;
	
	// tell opengl to use the generated texture
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	
	const float vertexData[] =
    {
        -size.x/2.0f, size.y/2.0f, // upper left
        size.x/2.0f, size.y/2.0f, // upper right
        -size.x/2.0f, -size.y/2.0f, // lower left
        size.x/2.0f, -size.y/2.0f, // lower right
    };
    const float texCoords[] =
    {
		rcImg.left, rcImg.bottom, // lower left
		rcImg.right, rcImg.bottom, // lower right
        rcImg.left, rcImg.top, // upper left
        rcImg.right, rcImg.top, // upper right
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
	
	const float vertexData[] =
    {
        ul.x, ul.y, // upper left
        ur.x, ur.y, // upper right
        bl.x, bl.y, // lower left
        br.x, br.y, // lower right
    };
    const float texCoords[] =
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



