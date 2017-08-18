/*
 RetSphinxEngine source - Object3D.h
 Copyright (c) 2013 Mark Hutcheson
*/
#pragma once
#include <string>

class Image;

class Object3D
{
protected:
	unsigned long m_vertexPtr;
	unsigned long m_normalPtr;
	unsigned long m_texCoordPtr;
	uint32_t num;
	unsigned int m_tex;
	unsigned int vertBuf;
	
	void _fromData(unsigned char* data, Image* tex);

	Object3D() {};

public:
	Object3D(unsigned char* data, Image* tex);
    ~Object3D();

    void render();
};










