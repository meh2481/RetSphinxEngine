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
	float* m_vertexPtr;
	float* m_normalPtr;
	float* m_texCoordPtr;
	unsigned int num;
	
	void _fromData(const unsigned char* data, unsigned int len);

	Mesh3D() {};

public:
	Mesh3D(const unsigned char* data, unsigned int len);
    ~Mesh3D();

    void render(GLuint tex);
};










