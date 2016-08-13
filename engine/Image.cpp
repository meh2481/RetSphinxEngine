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
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "easylogging++.h"
#include "ResourceTypes.h"
using namespace std;

Image::Image(unsigned char* blob, unsigned int size)
{
	m_sFilename = "";	//Can't reload? Is this a problem?
	_loadBlob(blob, size);
}

Image::Image(string sFilename)
{
	//m_bReloadEachTime = false;
	m_hTex = 0;
	m_iWidth = m_iHeight = 0;
	m_sFilename = sFilename;
	_load(sFilename);
}

Image::~Image()
{
	//image cleanup
	if(m_sFilename.length())
		LOG(INFO) << "Free " << m_sFilename;
	if(m_hTex)
		glDeleteTextures(1, &m_hTex);	//Free OpenGL graphics memory
}

void Image::_bind(unsigned char* data, unsigned int width, unsigned int height, int mode)
{
	m_iWidth = width;
	m_iHeight = height;

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &m_hTex);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, mode, m_iWidth, m_iHeight, 0, mode, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Image::_loadBlob(unsigned char* blob, unsigned int size)
{
	if(size < sizeof(TextureHeader))
	{
		LOG(ERROR) << "Decompressed image data smaller than texture header";
		return;
	}

	//Read header
	TextureHeader header;
	memcpy(&header, blob, sizeof(TextureHeader));
	blob += sizeof(TextureHeader);
	size -= sizeof(TextureHeader);

	if(size < header.width * header.height * header.bpp / 8)
	{
		LOG(ERROR) << "Insufficient image data. Expected: " << header.width * header.height * header.bpp / 8 << ", actual: " << size;
		return;
	}

	int mode = GL_RGBA;	// RGBA 32bit
	if(header.bpp == TEXTURE_BPP_RGB) // RGB 24bit
		mode = GL_RGB;

	_bind(blob, header.width, header.height, mode);
}

void Image::_loadPNG(string sFilename)
{
	int comp = 0;
	int width = 0;
	int height = 0;
	unsigned char* cBuf = stbi_load(sFilename.c_str(), &width, &height, &comp, 0);

	int mode = GL_RGBA;	// RGBA 32bit
	if(comp == 3) // RGB 24bit
		mode = GL_RGB;
  
	if((cBuf == 0) || (width == 0) || (height == 0))
	{
		LOG(ERROR) << "Unable to load image " << sFilename;
		return;
	}
  
	_bind(cBuf, width, height, mode);

	stbi_image_free(cBuf);
}

/*/TODO: This is CRAZY slow. Rip out, preload, or otherwise fix.

SimplexNoise1234 noiseGen;

//Code modified from https://cmaher.github.io/posts/working-with-simplex-noise/
float sumOcatave(int num_iterations, float x, float y, float persistence, float scalex, float scaley, float low, float high, float freqinc)
{
float maxAmp = 0;
float amp = 1;
float freqx = scalex;
float freqy = scaley;
float noise = 0;

//add successively smaller, higher-frequency terms
for(int i = 0; i < num_iterations; ++i)
{
noise += noiseGen.noise(x * freqx, y * freqy) * amp;
maxAmp += amp;
amp *= persistence;
freqx *= freqinc;
freqy *= freqinc;
}

//take the average value of the iterations
noise /= maxAmp;

//normalize the result
noise = noise * (high - low) / 2 + (high + low) / 2;

return noise;
}

void Image::_loadNoise(string sXMLFilename)
{
	m_bReloadEachTime = true;
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		LOG(ERROR) << "Error opening image noise XML file: " << sXMLFilename << "- Error " << iErr
		delete doc;
		return;
	}
	
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		LOG(ERROR) << "Error: Root element NULL in XML file " << sXMLFilename << ". Ignoring..."
		delete doc;
		return;
	}
	
	uint32_t width = 512;
	uint32_t height = 512;
	float sizex = 10.0f;
	float sizey = 10.0f;
	float xoffset = Random::randomFloat(0, 5000);	//By default, use random position in noise function
	float yoffset = Random::randomFloat(0, 5000);
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
	
	delete[] bits;
}*/

void Image::render(Vec2 size, float tilex, float tiley)
{
	// tell opengl to use the generated texture
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	const float vertexData[] =
    {
        -size.x/2.0f, size.y/2.0f, // upper left
        size.x/2.0f, size.y/2.0f, // upper right
		size.x / 2.0f, -size.y / 2.0f, // lower right
        -size.x/2.0f, -size.y/2.0f, // lower left
    };
    const float texCoords[] =
    {
		0.0f, 0.0f, // lower left
		tilex, 0.0f, // lower right
		tilex, tiley, // upper right
        0.0f, tiley, // upper left
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
    glDrawArrays(GL_QUADS, 0, 4);
}

void Image::renderLattice(Lattice* l, Vec2 size)
{
	glPushMatrix();
	
	glScalef(size.x, size.y, 1);
	l->renderTex(m_hTex);
	
	glPopMatrix();
}

void Image::render(Vec2 size, Rect rcImg)
{
	rcImg.left = rcImg.left / (float)m_iWidth;
	rcImg.right = rcImg.right / (float)m_iWidth;
	rcImg.top = 1.0f - rcImg.top / (float)m_iHeight;
	rcImg.bottom = 1.0f - rcImg.bottom / (float)m_iHeight;
	
	const float vertexData[] =
	{
		-size.x / 2.0f, size.y / 2.0f, // upper left
		size.x / 2.0f, size.y / 2.0f, // upper right
		size.x / 2.0f, -size.y / 2.0f, // lower right
		-size.x / 2.0f, -size.y / 2.0f, // lower left
	};
    const float texCoords[] =
    {
		rcImg.left, rcImg.bottom, // lower left
		rcImg.right, rcImg.bottom, // lower right
		rcImg.right, rcImg.top, // upper right
        rcImg.left, rcImg.top, // upper left
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
    glDrawArrays(GL_QUADS, 0, 4);
}

void Image::bindTexture()
{
	// tell opengl to use the generated texture
	glBindTexture(GL_TEXTURE_2D, m_hTex);
}

void Image::render4V(Vec2 ul, Vec2 ur, Vec2 bl, Vec2 br)
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

void Image::_load(string sFilename)
{
	LOG(INFO) << "Load " << sFilename;
	_loadPNG(sFilename);
}



