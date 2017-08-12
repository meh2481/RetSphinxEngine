/*
 RetSphinxEngine source - Mesh3D.h
 Copyright (c) 2013 Mark Hutcheson
*/
#pragma once
#include <string>
#include "Quad.h"

class Mesh3D
{
protected:
	const float* m_vertexPtr;
	const float* m_normalPtr;
	const float* m_texCoordPtr;
	uint32_t num;
	unsigned char* m_data;
	
	void _fromData(unsigned char* data, unsigned int len);

	Mesh3D() {};

public:
	Mesh3D(unsigned char* data, unsigned int len);
    ~Mesh3D();

    void render(GLuint tex);
};










