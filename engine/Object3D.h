/*
 RetSphinxEngine source - Object3D.h
 Copyright (c) 2013 Mark Hutcheson
*/
#pragma once
#include <string>
#include "Quad.h"

class Object3D
{
protected:
	const float* m_vertexPtr;
	const float* m_normalPtr;
	const float* m_texCoordPtr;
	uint32_t num;
	unsigned char* m_data;
	GLuint m_tex;
	
	void _fromData(unsigned char* data, Texture* tex);

	Object3D() {};

public:
	Object3D(unsigned char* data, Texture* tex);
    ~Object3D();

    void render();
};










