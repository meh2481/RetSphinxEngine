/*
 RetSphinxEngine source - Object3D.h
 Copyright (c) 2013 Mark Hutcheson
*/
#pragma once
#include <string>
#include "RenderState.h"

class Image;

class Object3D
{
protected:
    uint32_t num;
    unsigned int m_tex;
    unsigned int vertBuf;
    unsigned int vertArray;
    
    void _fromData(unsigned char* data, Image* tex);

    Object3D() {};

public:
    Object3D(unsigned char* data, Image* tex);
    ~Object3D();

    void render(RenderState renderState);
};










