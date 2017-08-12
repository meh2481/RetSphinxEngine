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
	free(m_data);
}

void Object3D::_fromData(unsigned char* data, Texture* tex)
{
	MeshHeader* header = (MeshHeader*)data;

	unsigned int len = sizeof(MeshHeader) + 
		header->numVertices * sizeof(float) * 3 +	//Vertices = xyz
		header->numVertices * sizeof(float) * 2 +	//UVs = uv
		header->numVertices * sizeof(float) * 3;	//Vertex Normals = xyz

	num = header->numVertices;

	//Copy data, since we'll be modifying UVs on the fly
	//TODO: More intelligent copy that only copies over the UV coordinates, since the rest never change
	m_data = (unsigned char*)malloc(len);
	memcpy(m_data, data, len);

	m_vertexPtr = (float*)((size_t)m_data + sizeof(MeshHeader));
	float* tempUv = (float*)((size_t)m_vertexPtr + sizeof(float) * 3 * num);
	m_normalPtr = (float*)((size_t)tempUv + sizeof(float) * 2 * num);

	//Offset UV coordinates by tex->uv
	float* uv = tempUv;
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

	m_texCoordPtr = tempUv;	//Store over in const pointer
}

void Object3D::render()
{
	assert(m_vertexPtr);
	assert(m_normalPtr);
	assert(m_texCoordPtr);
	assert(m_tex);
	assert(num > 0);

	glBindTexture(GL_TEXTURE_2D, m_tex);

	//TODO: Move outside of rendering function to not stall pipeline for no reason
	glEnableClientState(GL_NORMAL_ARRAY);
	glShadeModel(GL_SMOOTH);

	glTexCoordPointer(2, GL_FLOAT, 0, m_texCoordPtr);
	glNormalPointer(GL_FLOAT, 0, m_normalPtr);
	glVertexPointer(3, GL_FLOAT, 0, m_vertexPtr);

	glDrawArrays(GL_TRIANGLES, 0, num);
}
