/*
 CutsceneEditor source - 3DObject.cpp
 Copyright (c) 2013 Mark Hutcheson
*/
#include "Mesh3D.h"
#include <assert.h>
#include "opengl-api.h"
#include "ResourceTypes.h"

Mesh3D::Mesh3D(unsigned char* data, unsigned int len, Texture* tex)
{
	num = 0;
	m_vertexPtr = m_normalPtr = m_texCoordPtr = NULL;
	m_tex = tex->tex;

	_fromData(data, len, tex);
}

Mesh3D::~Mesh3D()
{
	free(m_data);
}

void Mesh3D::_fromData(unsigned char* data, unsigned int len, Texture* tex)
{
	assert(len >= sizeof(MeshHeader));

	MeshHeader* header = (MeshHeader*)data;

	assert(len >= sizeof(MeshHeader) + 
		header->numVertices * sizeof(float) * 3 +	//Vertices = xyz
		header->numVertices * sizeof(float) * 2 +	//UVs = uv
		header->numVertices * sizeof(float) * 3);	//Vertex Normals = xyz

	num = header->numVertices;

	m_vertexPtr = (float*)((size_t)data + sizeof(MeshHeader));
	float* tempUv = (float*)((size_t)m_vertexPtr + sizeof(float) * 3 * num);
	m_normalPtr = (float*)((size_t)tempUv + sizeof(float) * 2 * num);
	m_data = data;

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

void Mesh3D::render()
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
