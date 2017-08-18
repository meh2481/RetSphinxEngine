/*
 RetSphinxEngine source - Object3D.cpp
 Copyright (c) 2013 Mark Hutcheson
*/
#include "Object3D.h"
#include <assert.h>
#include "opengl-api.h"
#include "ResourceTypes.h"
#include "Quad.h"
#include "SDL_opengl.h"

Object3D::Object3D(unsigned char* data, Image* tex)
{
	num = 0;
	m_vertexPtr = m_normalPtr = m_texCoordPtr = 0;
	m_tex = tex->tex.tex;

	_fromData(data, tex);
}

Object3D::~Object3D()
{
	//free(m_texCoordPtr);
	glDeleteBuffers(1, &vertBuf);
}

void Object3D::_fromData(unsigned char* data, Image* tex)
{
	MeshHeader* header = (MeshHeader*)data;
	num = header->numVertices;

	//Copy all data, since we'll be modifying UVs on the fly
	unsigned int len = num * sizeof(float) * 2 + sizeof(float) * 3 * num + sizeof(float) * 3 * num;
	float* bufferData = (float*)malloc(len);
	memcpy(bufferData, (void*)((size_t)data + sizeof(MeshHeader)), len);

	//Assign pointers to vertex/texture/normal data
	m_vertexPtr = 0;
	m_texCoordPtr = m_vertexPtr + sizeof(float) * 3 * num;
	m_normalPtr = m_texCoordPtr + sizeof(float) * 2 * num;

	//Offset UV coordinates by tex->uv
	float* uv = (float*)(m_texCoordPtr + (unsigned long)bufferData);
	float left = tex->uv[0];
	float right = tex->uv[2];
	float top = tex->uv[5];
	float bottom = tex->uv[1];
	float width = right - left;
	float height = bottom - top;
	for(unsigned int i = 0; i < num; i++)
	{
		*uv++ = (*uv * width + left);
		*uv++ = (*uv * height + top);
	}

	glGenBuffers(1, &vertBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
	glBufferData(GL_ARRAY_BUFFER, len, bufferData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	free(bufferData);
}

void Object3D::render()
{
	//TODO: Move outside of rendering function if possible
	glEnableClientState(GL_NORMAL_ARRAY);
	glShadeModel(GL_SMOOTH);

	assert(m_tex);
	assert(num > 0);

	glBindTexture(GL_TEXTURE_2D, m_tex);

	glBindBuffer(GL_ARRAY_BUFFER, vertBuf);	//Bind buffer

	glTexCoordPointer(2, GL_FLOAT, 0, (void*)m_texCoordPtr);
	glNormalPointer(GL_FLOAT, 0, (void*)m_normalPtr);
	glVertexPointer(3, GL_FLOAT, 0, (void*)m_vertexPtr);
	glDrawArrays(GL_TRIANGLES, 0, num);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	//Unbind buffer
}
