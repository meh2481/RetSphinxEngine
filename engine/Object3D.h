/*
 RetSphinxEngine source - Object3D.h
 Copyright (c) 2013 Mark Hutcheson
*/
#pragma once
#include <string>
#include "SDL_opengl.h"

class Image;

class Object3D
{
protected:
	const float* m_vertexPtr;
	const float* m_normalPtr;
	float* m_texCoordPtr;
	uint32_t num;
	GLuint m_tex;
	
	void _fromData(unsigned char* data, Image* tex);

	Object3D() {};

public:
	Object3D(unsigned char* data, Image* tex);
    ~Object3D();

    void render();
};










