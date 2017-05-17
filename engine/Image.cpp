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

Image::Image(const string& sFilename)
{
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

void Image::_loadPNG(const string& sFilename)
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
        0.0f, 1.0f, // upper left
        1.0f, 1.0f, // upper right
        0.0f, 0.0f, // lower left
        1.0f, 0.0f, // lower right
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Image::_load(const string& sFilename)
{
	LOG(INFO) << "Load " << sFilename;
	_loadPNG(sFilename);
}



