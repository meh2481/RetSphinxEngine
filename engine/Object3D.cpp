/*
 RetSphinxEngine source - Object3D.cpp
 Copyright (c) 2013 Mark Hutcheson
*/
#include "Object3D.h"
#include <assert.h>
#include "opengl-api.h"
#include "ResourceTypes.h"

Object3D::Object3D(unsigned char* data, Texture* tex)
{
	num = 0;
	m_vertexPtr = m_normalPtr = m_texCoordPtr = NULL;
	m_tex = tex->tex;

	_fromData(data, tex);
}

Object3D::~Object3D()
{
	free(m_texCoordPtr);
}

void Object3D::_fromData(unsigned char* data, Texture* tex)
{
	MeshHeader* header = (MeshHeader*)data;
	num = header->numVertices;

	//Copy UV data, since we'll be modifying UVs on the fly
	unsigned int len = num * sizeof(float) * 2;
	m_texCoordPtr = (float*)malloc(len);
	memcpy(m_texCoordPtr, (void*)((size_t)data + sizeof(MeshHeader) + sizeof(float) * 3 * num), len);

	//Assign pointers to vertex/normal data
	m_vertexPtr = (float*)((size_t)data 
		+ sizeof(MeshHeader));

	m_normalPtr = (float*)((size_t)data 
		+ sizeof(MeshHeader)
		+ sizeof(float) * 2 * num 
		+ sizeof(float) * 3 * num);

	//Offset UV coordinates by tex->uv
	float* uv = m_texCoordPtr;
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
}

void Object3D::render()
{
	assert(m_vertexPtr);
	assert(m_normalPtr);
	assert(m_texCoordPtr);
	assert(m_tex);
	assert(num > 0);

	glBindTexture(GL_TEXTURE_2D, m_tex);

	//TODO: Move outside of rendering function if possible
	glEnableClientState(GL_NORMAL_ARRAY);
	glShadeModel(GL_SMOOTH);

	glTexCoordPointer(2, GL_FLOAT, 0, m_texCoordPtr);
	glNormalPointer(GL_FLOAT, 0, m_normalPtr);
	glVertexPointer(3, GL_FLOAT, 0, m_vertexPtr);

	glDrawArrays(GL_TRIANGLES, 0, num);
}
